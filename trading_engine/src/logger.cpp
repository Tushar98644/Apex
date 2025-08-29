#include "logger.hpp"
#include <iostream>
#include <ctime>

Logger::Logger() {
    logFile.open("logs/system.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error: Unable to open log file.\n";
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        time_t now = time(0);
        logFile << ctime(&now) << ": " << message << "\n";
    }
}
