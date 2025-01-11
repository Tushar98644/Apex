#include <iostream>
#include "config.hpp"
#include "api_client.hpp"
#include "order_manager.hpp"
#include "websocket_server.hpp"
#include <thread>
#include <atomic>

std::atomic<bool> running(true);

void signalHandler(int signum) {
    running = false;
}

int main() {
    // Register signal handler
    signal(SIGINT, signalHandler);
    
    Config config;
    if (!config.load("../config/config.json")) {
        std::cerr << "Failed to load configuration. Exiting...\n";
        return 1;
    }

    std::cout << "Deribit Trading System Initialized Successfully!\n";

    APIClient apiClient(config.getClientId(), config.getClientSecret());
    OrderManager orderManager(apiClient);

    WebSocketServer wsServer;
    try {
        wsServer.start(9001);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to start WebSocket server: " << e.what() << "\n";
        return 1;
    }

    // Create a thread for your trading logic
    std::thread tradingThread([&]() {
        try {
            std::string instrument_name = "BTC-PERPETUAL";
            std::string type = "limit";
            double amount = 10.0;
            double price = 50000.0;

            std::string orderId = orderManager.placeOrder(instrument_name, type, amount, price);
            if (!orderId.empty()) {
                std::cout << "Order placed successfully. Order ID: " << orderId << "\n";

                auto activeOrders = orderManager.getActiveOrders();
                std::cout << "Current active orders:\n";
                for (const auto& [id, order] : activeOrders) {
                    std::cout << "Order ID: " << id << ", Instrument: " << order.instrument_name
                              << ", Price: " << order.price << ", Amount: " << order.amount << "\n";
                }

                if (orderManager.cancelOrder(orderId)) {
                    std::cout << "Order canceled successfully.\n";
                }
                else {
                    std::cout << "Failed to cancel the order.\n";
                }
            }

            // Periodic orderbook updates
            while (running) {
                auto orderbook = apiClient.getOrderBook("BTC-PERPETUAL");
                std::string orderbookStr = orderbook.dump();
                wsServer.broadcastOrderbookUpdates("BTC-PERPETUAL", orderbookStr);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        catch (const std::exception& ex) {
            std::cerr << "Error during OrderManager operations: " << ex.what() << "\n";
        }
    });

    // Keep the main thread running and handling WebSocket connections
    std::cout << "Press Ctrl+C to exit...\n";
    while (running && wsServer.isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Cleanup
    tradingThread.join();
    wsServer.stop();
    return 0;
}