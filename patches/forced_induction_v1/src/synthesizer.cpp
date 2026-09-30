#include "../include/synthesizer.h"

#include "../include/utilities.h"
#include "../include/delta.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

#undef min
#undef max

Synthesizer::Synthesizer() {
    m_inputChannels = nullptr;
    m_inputChannelCount = 0;
    m_inputBufferSize = 0;
    m_inputWriteOffset = 0.0;
    m_inputSamplesRead = 0;

    m_audioBufferSize = 0;

    m_inputSampleRate = 0.0;
    m_audioSampleRate = 0.0;

    m_lastInputSampleOffset = 0.0;

    m_run = true;
    m_thread = nullptr;
    m_filters = nullptr;
    m_exhaustChannelCount = 0;
}

Synthesizer::~Synthesizer() {
    assert(m_inputChannels == nullptr);
    assert(m_thread == nullptr);
    assert(m_filters == nullptr);
}

void Synthesizer::initialize(const Parameters &p) {
    m_inputChannelCount = p.inputChannelCount;
    m_exhaustChannelCount = (p.exhaustChannelCount < 0)
        ? p.inputChannelCount
        : std::min(p.exhaustChannelCount, p.inputChannelCount);
    m_inputBufferSize = p.inputBufferSize;
    m_inputWriteOffset = p.inputBufferSize;
    m_audioBufferSize = p.audioBufferSize;
    m_inputSampleRate = p.inputSampleRate;
    m_audioSampleRate = p.audioSampleRate;
    m_audioParameters = p.initialAudioParameters;

    m_inputSamplesRead = 0;

    m_inputWriteOffset = 0;
    m_processed = true;

    m_audioBuffer.initialize(p.audioBufferSize);
    m_inputChannels = new InputChannel[p.inputChannelCount];
    for (int i = 0; i < p.inputChannelCount; ++i) {
        m_inputChannels[i].transferBuffer = new float[p.inputBufferSize];
        m_inputChannels[i].data.initialize(p.inputBufferSize);
    }

    m_filters = new ProcessingFilters[p.inputChannelCount];
    for (int i = 0; i < p.inputChannelCount; ++i) {
        m_filters[i].airNoiseLowPass.setCutoffFrequency(
            m_audioParameters.airNoiseFrequencyCutoff, m_audioSampleRate);

        m_filters[i].derivative.m_dt = 1 / m_audioSampleRate;

        m_filters[i].inputDcFilter.setCutoffFrequency(10.0);
        m_filters[i].inputDcFilter.m_dt = 1 / m_audioSampleRate;

        m_filters[i].jitterFilter.initialize(
            10,
            m_audioParameters.inputSampleNoiseFrequencyCutoff,
            m_audioSampleRate);

        m_filters[i].antialiasing.setCutoffFrequency(1900.0f, m_audioSampleRate);
    }

    m_levelingFilter.p_target = m_audioParameters.levelerTarget;
    m_levelingFilter.p_maxLevel = m_audioParameters.levelerMaxGain;
    m_levelingFilter.p_minLevel = m_audioParameters.levelerMinGain;
    m_antialiasing.setCutoffFrequency(m_audioSampleRate * 0.45f, m_audioSampleRate);

    // Structural transfer band for combustion noise: engine structures
    // attenuate least in the ~1-3 kHz region (Austen & Priede), so the
    // radiated knock is shaped by a broad band-pass centred there.
    m_layers = LayerState{};
    m_layers.structuralBand.setBandPass(1600.0f, 1.5f, m_audioSampleRate);

    for (int i = 0; i < m_audioBufferSize; ++i) {
        m_audioBuffer.write(0);
    }
}

void Synthesizer::initializeImpulseResponse(
    const int16_t *impulseResponse,
    unsigned int samples,
    float volume,
    int index)
{
    unsigned int clippedLength = 0;
    for (unsigned int i = 0; i < samples; ++i) {
        if (std::abs(impulseResponse[i]) > 100) {
            clippedLength = i + 1;
        }
    }

    const unsigned int sampleCount = std::min(10000U, clippedLength);
    std::vector<float> coefficients(sampleCount);
    for (unsigned int i = 0; i < sampleCount; ++i) {
        coefficients[i] = volume * impulseResponse[i] / INT16_MAX;
    }

    // The direct convolution costs one multiply-add per tap per channel per
    // output sample and runs on the single audio thread. A 16-cylinder engine
    // with four exhaust channels on the same impulse response needed ~1.6 s
    // of CPU per second of audio, so the output buffer ran dry (clicks and
    // gaps). Reuse an earlier channel's filter when the coefficients match.
    for (int j = 0; j < index; ++j) {
        ConvolutionFilter &other = m_filters[j].convolution;
        if (m_filters[j].convolutionOwner != j) continue;
        if (other.getSampleCount() != static_cast<int>(sampleCount)) continue;
        if (!std::equal(coefficients.begin(), coefficients.end(), other.getImpulseResponse())) continue;
        m_filters[index].convolutionOwner = j;
        return;
    }

    m_filters[index].convolutionOwner = index;
    m_filters[index].convolution.initialize(sampleCount);
    std::copy(coefficients.begin(), coefficients.end(), m_filters[index].convolution.getImpulseResponse());
}

void Synthesizer::startAudioRenderingThread() {
    m_run = true;
    m_thread = new std::thread(&Synthesizer::audioRenderingThread, this);
}

void Synthesizer::endAudioRenderingThread() {
    if (m_thread != nullptr) {
        m_run = false;
        endInputBlock();

        m_thread->join();
        delete m_thread;

        m_thread = nullptr;
    }
}

void Synthesizer::destroy() {
    m_audioBuffer.destroy();

    for (int i = 0; i < m_inputChannelCount; ++i) {
        m_inputChannels[i].data.destroy();
        m_filters[i].convolution.destroy();
    }

    delete[] m_inputChannels;
    delete[] m_filters;

    m_inputChannels = nullptr;
    m_filters = nullptr;

    m_inputChannelCount = 0;
}

int Synthesizer::readAudioOutput(int samples, int16_t *buffer) {
    std::lock_guard<std::mutex> lock(m_lock0);

    const int newDataLength = m_audioBuffer.size();
    if (newDataLength >= samples) {
        m_audioBuffer.readAndRemove(samples, buffer);
    }
    else {
        m_audioBuffer.readAndRemove(newDataLength, buffer);
        memset(
            buffer + newDataLength,
            0,
            sizeof(int16_t) * ((size_t)samples - newDataLength));
    }
    
    const int samplesConsumed = std::min(samples, newDataLength);

    return samplesConsumed;
}

void Synthesizer::waitProcessed() {
    {
        std::unique_lock<std::mutex> lk(m_lock0);
        m_cv0.wait(lk, [this] { return m_processed; });
    }
}

void Synthesizer::writeInput(const double *data) {
    m_inputWriteOffset += (double)m_audioSampleRate / m_inputSampleRate;
    if (m_inputWriteOffset >= (double)m_inputBufferSize) {
        m_inputWriteOffset -= (double)m_inputBufferSize;
    }

    for (int i = 0; i < m_inputChannelCount; ++i) {
        RingBuffer<float> &buffer = m_inputChannels[i].data;
        const double lastInputSample = m_inputChannels[i].lastInputSample;
        const size_t baseIndex = buffer.writeIndex();
        const double distance =
            inputDistance(m_inputWriteOffset, m_lastInputSampleOffset);
        double s =
            inputDistance(baseIndex, m_lastInputSampleOffset);
        for (; s <= distance; s += 1.0) {
            if (s >= m_inputBufferSize) s -= m_inputBufferSize;

            const double f = s / distance;
            const double sample = lastInputSample * (1 - f) + data[i] * f;

            buffer.write(m_filters[i].antialiasing.fast_f(static_cast<float>(sample)));
        }

        m_inputChannels[i].lastInputSample = data[i];
    }

    m_lastInputSampleOffset = m_inputWriteOffset;
}

void Synthesizer::endInputBlock() {
    std::unique_lock<std::mutex> lk(m_inputLock); 

    for (int i = 0; i < m_inputChannelCount; ++i) {
        m_inputChannels[i].data.removeBeginning(m_inputSamplesRead);
    }

    if (m_inputChannelCount != 0) {
        m_latency = m_inputChannels[0].data.size();
    }
    
    m_inputSamplesRead = 0;
    m_processed = false;

    lk.unlock();
    m_cv0.notify_one();
}

void Synthesizer::audioRenderingThread() {
    while (m_run) {
        renderAudio();
    }
}

#undef max
void Synthesizer::renderAudio() {
    std::unique_lock<std::mutex> lk0(m_lock0);

    m_cv0.wait(lk0, [this] {
        const bool inputAvailable =
            m_inputChannels[0].data.size() > 0
            && m_audioBuffer.size() < 2000;
        return !m_run || (inputAvailable && !m_processed);
    });

    const int n = std::min(
        std::max(0, 2000 - (int)m_audioBuffer.size()),
        (int)m_inputChannels[0].data.size());

    for (int i = 0; i < m_inputChannelCount; ++i) {
        m_inputChannels[i].data.read(n, m_inputChannels[i].transferBuffer);
    }
    
    m_inputSamplesRead = n;
    m_processed = true;

    lk0.unlock();

    for (int i = 0; i < m_inputChannelCount; ++i) {
        m_filters[i].airNoiseLowPass.setCutoffFrequency(
            static_cast<float>(m_audioParameters.airNoiseFrequencyCutoff), m_audioSampleRate);
        m_filters[i].jitterFilter.setJitterScale(m_audioParameters.inputSampleNoise);
    }

    for (int i = 0; i < n; ++i) {
        m_audioBuffer.write(renderAudio(i));
    }

    m_cv0.notify_one();
}

double Synthesizer::getLatency() const {
    return (double)m_latency / m_audioSampleRate;
}

int Synthesizer::inputDelta(int s1, int s0) const {
    return (s1 < s0)
        ? m_inputBufferSize - s0 + s1
        : s1 - s0;
}

double Synthesizer::inputDistance(double s1, double s0) const {
    return (s1 < s0)
        ? (double)m_inputBufferSize - s0 + s1
        : s1 - s0;
}

void Synthesizer::setInputSampleRate(double sampleRate) {
    if (sampleRate != m_inputSampleRate) {
        std::lock_guard<std::mutex> lock(m_lock0);
        m_inputSampleRate = sampleRate;
    }
}

int16_t Synthesizer::renderAudio(int inputSample) {
    const float airNoise = m_audioParameters.airNoise;
    const float dF_F_mix = m_audioParameters.dF_F_mix;
    const float convAmount = m_audioParameters.convolution;

    float signal = 0;
    for (int i = 0; i < m_exhaustChannelCount; ++i) {
        const float r_0 = 2.0 * ((double)rand() / RAND_MAX) - 1.0;

        const float jitteredSample =
            m_filters[i].jitterFilter.fast_f(m_inputChannels[i].transferBuffer[inputSample]);

        const float f_in = jitteredSample;
        const float f_dc = m_filters[i].inputDcFilter.fast_f(f_in);
        const float f = f_in - f_dc;
        const float f_p = m_filters[i].derivative.f(f_in);

        const float noise = 2.0 * ((double)rand() / RAND_MAX) - 1.0;
        const float r =
            m_filters->airNoiseLowPass.fast_f(noise);
        const float r_mixed =
            airNoise * r + (1 - airNoise);

        float v_in =
            f_p * dF_F_mix
            + f * r_mixed * (1 - dF_F_mix);
        if (fpclassify(v_in) == FP_SUBNORMAL) {
            v_in = 0;
        }

        // Dry part now; the wet part is convolved once per shared filter.
        signal += (1 - convAmount) * v_in;
        const int owner = m_filters[i].convolutionOwner;
        if (owner >= 0) m_filters[owner].convolutionInput += v_in;
    }

    for (int i = 0; i < m_exhaustChannelCount; ++i) {
        if (m_filters[i].convolutionOwner != i) continue;
        signal += convAmount * m_filters[i].convolution.f(m_filters[i].convolutionInput);
        m_filters[i].convolutionInput = 0.0f;
    }

    signal = m_antialiasing.fast_f(signal);

    m_levelingFilter.p_target = m_audioParameters.levelerTarget;
    // The level control follows the exhaust (engine) signal only. The knock
    // and turbo layers receive the same gain, so their level relative to the
    // engine is set by the physics and they can never turn the engine down.
    const float exhaustLeveled = m_levelingFilter.f(signal);
    const float layers = renderLayers(inputSample) * m_levelingFilter.getAttenuation();
    const float v_leveled = (exhaustLeveled + layers) * m_audioParameters.volume;
    int r_int = std::lround(v_leveled);
    if (r_int > INT16_MAX) {
        r_int = INT16_MAX;
    }
    else if (r_int < INT16_MIN) {
        r_int = INT16_MIN;
    }

    return static_cast<int16_t>(r_int);
}

double Synthesizer::getLevelerGain() {
    std::lock_guard<std::mutex> lock(m_lock0);
    return m_levelingFilter.getAttenuation();
}

Synthesizer::AudioParameters Synthesizer::getAudioParameters() {
    std::lock_guard<std::mutex> lock(m_lock0);
    return m_audioParameters;
}

void Synthesizer::setAudioParameters(const AudioParameters &params) {
    std::lock_guard<std::mutex> lock(m_lock0);
    m_audioParameters = params;
}

void Synthesizer::Biquad::setBandPass(float frequency, float q, float sampleRate) {
    // RBJ band-pass, constant 0 dB peak gain.
    const float w0 = 2.0f * 3.14159265358979f * frequency / sampleRate;
    const float alpha = std::sin(w0) / (2.0f * q);
    const float a0 = 1.0f + alpha;
    b0 = alpha / a0;
    b1 = 0.0f;
    b2 = -alpha / a0;
    a1 = -2.0f * std::cos(w0) / a0;
    a2 = (1.0f - alpha) / a0;
}

float Synthesizer::Biquad::f(float x) {
    const float y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
    x2 = x1; x1 = x;
    y2 = y1; y1 = (std::fpclassify(y) == FP_SUBNORMAL) ? 0.0f : y;
    return y1;
}

float Synthesizer::LayerState::white() {
    // Uniform white noise with unit variance.
    rng = 1664525u * rng + 1013904223u;
    const float u = static_cast<float>(rng) / 4294967296.0f;
    return (2.0f * u - 1.0f) * 1.7320508f;
}

float Synthesizer::renderLayers(int inputSample) {
    if (m_exhaustChannelCount + AuxiliaryChannelCount > m_inputChannelCount) return 0.0f;
    const int base = m_exhaustChannelCount;
    const float forceRate = m_inputChannels[base + StructuralForceRate].transferBuffer[inputSample];
    const float bladePass = m_inputChannels[base + TurboBladePassFrequency].transferBuffer[inputSample];
    const float turboAmplitude = m_inputChannels[base + TurboAmplitude].transferBuffer[inputSample];

    // Diesel knock: the combustion force rate on the structure (coherent
    // part, band-limited by the physics rate) plus noise with the same
    // envelope for the content above that rate, through the structural band.
    const float n = m_layers.white();
    const float envelope = std::max(0.0f, forceRate);
    // The coherent force-rate transient (synchronous with each combustion
    // event) dominates; a smaller noise share with the same envelope fills
    // in content above the physics rate. The structure rings at the band.
    const float knock = m_audioParameters.combustionNoiseLevel
        * m_layers.structuralBand.f(forceRate + 0.3f * envelope * n);

    // Turbocharger: narrow-band noise plus a tonal part at the blade-pass
    // frequency, generated at the audio rate (no imaging, no physics-rate
    // Nyquist limit). Amplitude follows sqrt(compressor power).
    float turbo = 0.0f;
    const float nyquistGuard = 0.45f * m_audioSampleRate;
    if (turboAmplitude > 0.0f && bladePass > 20.0f && bladePass < nyquistGuard) {
        constexpr float TurboQ = 30.0f;
        m_layers.turboBand.setBandPass(bladePass, TurboQ, m_audioSampleRate);
        // Normalise the band-limited noise to unit RMS (equivalent noise
        // bandwidth of the 0 dB peak band-pass is (pi/2) * f / Q).
        const float bandwidth = 1.5707963f * bladePass / TurboQ;
        const float norm = 1.0f / std::sqrt(std::max(1.0e-9f, bandwidth / (0.5f * m_audioSampleRate)));
        const float noisePart = m_layers.turboBand.f(n) * norm;
        m_layers.turboPhase += 2.0 * 3.14159265358979 * bladePass / m_audioSampleRate;
        if (m_layers.turboPhase > 6.283185307179586) m_layers.turboPhase -= 6.283185307179586;
        // Blade-pass tone plus its second harmonic (at 0.35 amplitude),
        // normalised to unit RMS; the harmonic is dropped near Nyquist.
        const bool harmonic = 2.0f * bladePass < nyquistGuard;
        const float h2 = harmonic ? 0.35f : 0.0f;
        const float tone = 1.4142136f / std::sqrt(1.0f + h2 * h2) * static_cast<float>(
            std::sin(m_layers.turboPhase) + h2 * std::sin(2.0 * m_layers.turboPhase));
        const float tonal = m_audioParameters.turboTonalFraction;
        turbo = m_audioParameters.turboSoundLevel * turboAmplitude
            * ((1.0f - tonal) * noisePart + tonal * tone);
    }

    return knock + turbo;
}
