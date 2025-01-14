#include "latency_benchmark.hpp"
#include <iostream>
#include <filesystem>

std::string LatencyBenchmark::logFilePath;
std::mutex LatencyBenchmark::logMutex;

void LatencyBenchmark::initialize() {
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path rootPath = currentPath.parent_path();
    logFilePath = (rootPath / "log_metrics.log").string();

    std::ofstream logFile(logFilePath, std::ios::trunc);
    if (!logFile.is_open()) {
        throw std::runtime_error("Failed to create log file at: " + logFilePath);
    }
    logFile.close();
}

void LatencyBenchmark::cleanup() {
    std::cout << "Latency log file is saved at: " << logFilePath << std::endl;
}

void LatencyBenchmark::log(const std::string& metricName, double durationMs) {
    std::lock_guard<std::mutex> lock(logMutex);

    std::ofstream logFile(logFilePath, std::ios::app);
    if (logFile.is_open()) {
        logFile << std::fixed << std::setprecision(3);
        logFile << metricName << ": " << durationMs << " ms\n";
        logFile.close();
    }
}
