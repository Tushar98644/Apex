#include <iostream>
#include "config.hpp"
#include "websocket_server.hpp"

int main() {
    Config config;
    if (!config.load("../config/config.json")) {
        std::cerr << "Failed to load configuration. Exiting...\n";
        return 1;
    }

    std::cout << "Deribit Trading System Initialized Successfully!\n";
    std::cout << "API Key: " << config.getApiKey() << "\n";

    WebSocketServer wsServer;
    try {
        wsServer.start(9002); // Start WebSocket server on port 9002
        std::cout << "WebSocket server running on port 9002.\n";
    } catch (const std::exception& e) {
        std::cerr << "WebSocket server error: " << e.what() << "\n";
        return 1;
    }

    std::cout << "Modules not yet implemented.\n";

    // Run the server loop
    wsServer.run();

    return 0;
}
