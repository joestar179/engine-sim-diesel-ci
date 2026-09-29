#ifndef ATG_ENGINE_SIM_TELEMETRY_LOG_H
#define ATG_ENGINE_SIM_TELEMETRY_LOG_H

#include <chrono>
#include <fstream>
#include <string>
#include <vector>

class Engine;
class Simulator;

// Read-only run logger for interactive sessions. It never changes simulation
// state. sampleStep() accumulates per-step statistics; endFrame() writes a
// full key=value snapshot every `interval` seconds of simulated time and an
// EVENT line whenever an operator control changes.
class TelemetryLog {
public:
    TelemetryLog();
    ~TelemetryLog();

    // Opens <directory>/telemetry_<engine>_<timestamp>.log and writes the
    // configuration header. Returns false if the file cannot be created.
    bool open(Engine *engine, Simulator *simulator, const std::string &directory,
              double interval = 0.5);
    void close();
    bool isOpen() const { return m_file.is_open(); }
    const std::string &path() const { return m_path; }

    void sampleStep();
    void endFrame();

    // Directory next to the running executable: <exe dir>/../logs.
    static std::string defaultDirectory();

private:
    struct CylinderStats {
        double peakPressure = 0.0;
        double peakTemperature = 0.0;
        double runnerPeakPressure = 0.0;
        double burntAtStart = 0.0;
        int litEdges = 0;
        bool wasLit = false;
    };

    struct Controls {
        bool starter = false;
        bool ignition = false;
        bool dyno = false;
        bool hold = false;
        double dynoSpeed = 0.0;
        int gear = -2;
        double speedControl = -1.0;
    };

    void writeHeader();
    void writeSnapshot();
    void checkControls();
    void resetInterval();
    Controls readControls() const;

    template <typename T>
    void kv(const char *key, T value);
    template <typename T>
    void kv(const std::string &key, T value) { kv(key.c_str(), value); }

    Engine *m_engine = nullptr;
    Simulator *m_simulator = nullptr;
    std::ofstream m_file;
    std::string m_path;
    double m_interval = 0.5;

    std::chrono::steady_clock::time_point m_wallStart;
    double m_simTime = 0.0;
    double m_nextSample = 0.0;
    long long m_steps = 0;
    long long m_frames = 0;
    long long m_samples = 0;

    // Interval statistics.
    long long m_intervalSteps = 0;
    double m_intervalStart = 0.0;
    double m_rpmMin = 0.0, m_rpmMax = 0.0, m_rpmSum = 0.0;
    double m_rackMin = 0.0, m_rackMax = 0.0, m_rackSum = 0.0;
    double m_turbineInletMax = 0.0, m_turbinePowerMax = 0.0, m_turbinePowerSum = 0.0;
    double m_turbineMassFlowSum = 0.0, m_compressorMassFlowSum = 0.0, m_intakeMassFlowSum = 0.0;
    double m_plenumMin = 0.0, m_plenumMax = 0.0, m_plenumSum = 0.0;
    double m_dynoTorqueSum = 0.0;
    double m_injectedAtStart = 0.0, m_burnedAtStart = 0.0;
    std::vector<double> m_scrollMax, m_scrollSum;
    std::vector<CylinderStats> m_cylinders;

    Controls m_controls;
    bool m_first = true;
};

#endif /* ATG_ENGINE_SIM_TELEMETRY_LOG_H */
