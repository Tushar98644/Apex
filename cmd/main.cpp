#include <iostream>
#include "config.hpp"

int main() {

    Config config;
    if (!config.load("../config/config.json")) {
        std::cerr << "Failed to load configuration. Exiting...\n";
        return 1;
    }

    std::cout << "Deribit Trading System Initialized Successfully!\n";
    std::cout << "API Key: " << config.getApiKey() << "\n";

    std::cout << "Modules not yet implemented.\n";

    return 0;
}
