#ifndef ATG_ENGINE_SIM_ENGINE_SYNTHESIZER_H
#define ATG_ENGINE_SIM_ENGINE_SYNTHESIZER_H

#include "convolution_filter.h"
#include "leveling_filter.h"
#include "derivative_filter.h"
#include "low_pass_filter.h"
#include "jitter_filter.h"
#include "ring_buffer.h"
#include "butterworth_low_pass_filter.h"

#include <cinttypes>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

class Synthesizer {
    public:
        struct AudioParameters {
            float volume = 1.0f;
            float convolution = 1.0f;
            float dF_F_mix = 0.01f;
            float inputSampleNoise = 0.5f;
            float inputSampleNoiseFrequencyCutoff = 10000.0f;
            float airNoise = 1.0f;
            float airNoiseFrequencyCutoff = 2000.0f;
            float levelerTarget = 30000.0f;
            float levelerMaxGain = 1.9f;
            float levelerMinGain = 0.00001f;

            // Engine-independent layer levels, set once for all engines; the
            // difference between engines comes from their physics. Global
            // reference calibration (engine-sim-audio-render, 2026-09-30):
            //  - knock: -15 dB relative to the exhaust signal for the ALCO
            //    6-251D at full speed command (free rev);
            //  - turbo: -20 dB for the ALCO 16-251B in the same free rev
            //    (low compressor power); it grows as sqrt(power) under load.
            // Structure-borne combustion noise ("diesel knock"): level per unit
            // of structural force rate sum(piston area * combustion dp/dt), N/s.
            float combustionNoiseLevel = 4.0e-4f;
            // Turbocharger: level per sqrt(W) of turbine + compressor power.
            float turboSoundLevel = 15.0f;
            // Fraction of the turbo layer that is tonal (the rest is narrow-band
            // noise around the blade-pass frequency). Turbocharger noise is
            // dominated by the blade-pass tone and its harmonics.
            float turboTonalFraction = 0.8f;
        };

        // Auxiliary input channels written after the exhaust channels when
        // Parameters::exhaustChannelCount >= 0 (see AuxiliaryChannel).
        enum AuxiliaryChannel {
            StructuralForceRate = 0,    // N/s, >= 0
            TurboBladePassFrequency,    // Hz
            TurboAmplitude,             // sqrt(W)
            AuxiliaryChannelCount
        };

        struct Parameters {
            // -1: every input channel is an exhaust channel (original layout).
            int exhaustChannelCount = -1;
            int inputChannelCount = 1;
            int inputBufferSize = 1024;
            int audioBufferSize = 44100;
            float inputSampleRate = 10000;
            float audioSampleRate = 44100;
            AudioParameters initialAudioParameters;
        };

        struct InputChannel {
            RingBuffer<float> data;
            float *transferBuffer = nullptr;
            double lastInputSample = 0.0f;
        };

        struct ProcessingFilters {
            ConvolutionFilter convolution;
            DerivativeFilter derivative;
            JitterFilter jitterFilter;
            ButterworthLowPassFilter<float> airNoiseLowPass;
            LowPassFilter inputDcFilter;
            ButterworthLowPassFilter<double> antialiasing;

            // Channels whose impulse responses are identical share one
            // convolution (convolution is linear, so convolving the sum of
            // their inputs equals the sum of their convolutions). This
            // channel's input is added to convolutionInput of its owner.
            int convolutionOwner = -1;
            float convolutionInput = 0.0f;
        };

    public:
        Synthesizer();
        ~Synthesizer();

        void initialize(const Parameters &p);
        void initializeImpulseResponse(
            const int16_t *impulseResponse,
            unsigned int samples,
            float volume,
            int index);
        void startAudioRenderingThread();
        void endAudioRenderingThread();
        void destroy();

        int readAudioOutput(int samples, int16_t *buffer);

        void writeInput(const double *data);
        void endInputBlock();

        void waitProcessed();

        void audioRenderingThread();
        void renderAudio();

        double getLatency() const;

        int inputDelta(int s1, int s0) const;
        double inputDistance(double s1, double s0) const;

        void setInputSampleRate(double sampleRate);
        double getInputSampleRate() const { return m_inputSampleRate; }

        int16_t renderAudio(int inputOffset);

        double getLevelerGain();
        AudioParameters getAudioParameters();
        void setAudioParameters(const AudioParameters &params);

        // Second-order section (RBJ cookbook), used by the layers below.
        struct Biquad {
            float b0 = 0, b1 = 0, b2 = 0, a1 = 0, a2 = 0;
            float x1 = 0, x2 = 0, y1 = 0, y2 = 0;
            void setBandPass(float frequency, float q, float sampleRate);
            float f(float x);
        };

        struct LayerState {
            Biquad structuralBand;      // fixed structural transfer band
            Biquad turboBand;           // retuned every sample to the BPF
            double turboPhase = 0.0;
            uint32_t rng = 0x1234567u;
            float white();
        };

        float renderLayers(int inputSample);

    //protected:
        int m_exhaustChannelCount;
        LayerState m_layers;
        ButterworthLowPassFilter<float> m_antialiasing;
        LevelingFilter m_levelingFilter;
        InputChannel *m_inputChannels;
        AudioParameters m_audioParameters;
        int m_inputChannelCount;
        int m_inputBufferSize;
        int m_inputSamplesRead;
        int m_latency;
        double m_inputWriteOffset;
        double m_lastInputSampleOffset;

        RingBuffer<int16_t> m_audioBuffer;
        int m_audioBufferSize;

        float m_inputSampleRate;
        float m_audioSampleRate;

        std::thread *m_thread;
        std::atomic<bool> m_run;
        bool m_processed;

        std::mutex m_inputLock;
        std::mutex m_lock0;
        std::condition_variable m_cv0;

        ProcessingFilters *m_filters;
};

#endif /* ATG_ENGINE_SIM_ENGINE_SYNTHESIZER_H */
