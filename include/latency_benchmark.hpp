#ifndef LATENCY_BENCHMARK_HPP
#define LATENCY_BENCHMARK_HPP

#include <chrono>
#include <string>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <filesystem>

class LatencyBenchmark {
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    static TimePoint start() {
        return Clock::now();
    }

    static void end(const TimePoint& startTime, const std::string& metricName) {
        auto endTime = Clock::now();
        double duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000.0;
        log(metricName, duration);
    }

    static void initialize();
    static void cleanup();

private:
    static void log(const std::string& metricName, double durationMs);
    static std::string logFilePath;
    static std::mutex logMutex;
};

#endif // LATENCY_BENCHMARK_HPP
