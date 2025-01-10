#include "config.hpp"
#include <iostream>
#include <fstream>
#include <json.hpp>

using json = nlohmann::json;

bool Config::load(const std::string& filepath) {
    std::ifstream configFile(filepath);
    if (!configFile.is_open()) {
        std::cerr << "Error: Could not open config file at " << filepath << "\n";
        return false;
    }

    try {
        json configJson;
        configFile >> configJson;
        apiKey = configJson.at("api_key").get<std::string>();
    } catch (const std::exception& e) {
        std::cerr << "Error reading configuration: " << e.what() << "\n";
        return false;
    }

    return true;
}

std::string Config::getApiKey() const {
    return apiKey;
}
