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

void displayMenu() {
    std::cout << "\nDeribit Trading System Menu:\n";
    std::cout << "1. Place Order\n";
    std::cout << "2. Cancel Order\n";
    std::cout << "3. Modify Order\n";
    std::cout << "4. Get Orderbook\n";
    std::cout << "5. Get Positions\n";
    std::cout << "6. Exit\n";
    std::cout << "Enter your choice: \n";
}

void processOrderbook(WebSocketServer& wsServer, APIClient& apiClient) {
    while (running) {
        try {
            auto orderbook = apiClient.getOrderBook("BTC-PERPETUAL");
            std::string orderbookStr = orderbook.dump();
            wsServer.broadcastOrderbookUpdates("BTC-PERPETUAL", orderbookStr);
        } catch (const std::exception& e) {
            // Silent catch - no terminal output
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
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
        wsServer.start(9002);
        std::cout << "WebSocket server started on port 9002 \n";
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to start WebSocket server: " << e.what() << "\n";
        return 1;
    }

    // Start orderbook update thread
    std::thread orderbookThread([&]() {
        processOrderbook(wsServer, apiClient);
    });

    while (running) {
        displayMenu();
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try {
            switch (choice) {
                case 1: { // Place Order
                    std::string instrument_name, type;
                    double amount, price;

                    std::cout << "Enter instrument name (e.g., BTC-PERPETUAL): ";
                    std::getline(std::cin, instrument_name);
                    
                    std::cout << "Enter order type (limit/market): ";
                    std::getline(std::cin, type);
                    
                    std::cout << "Enter amount: ";
                    std::cin >> amount;
                    
                    if (type == "limit") {
                        std::cout << "Enter price: ";
                        std::cin >> price;
                    } else {
                        price = 0; // Market order
                    }

                    std::string orderId = orderManager.placeOrder(instrument_name, type, amount, price);
                    if (!orderId.empty()) {
                        std::cout << "Order placed successfully. Order ID: " << orderId << "\n";
                    } else {
                        std::cout << "Failed to place order.\n";
                    }
                    break;
                }

                case 2: { // Cancel Order
                    std::string orderId;
                    std::cout << "Enter order ID to cancel: ";
                    std::getline(std::cin, orderId);

                    if (orderManager.cancelOrder(orderId)) {
                        std::cout << "Order canceled successfully.\n";
                    } else {
                        std::cout << "Failed to cancel order.\n";
                    }
                    break;
                }

                case 3: { // Modify Order
                    std::string orderId;
                    double newPrice, newAmount;
                    
                    std::cout << "Enter order ID to modify: ";
                    std::getline(std::cin, orderId);
                    
                    std::cout << "Enter new price (0 to keep current): ";
                    std::cin >> newPrice;
                    
                    std::cout << "Enter new amount (0 to keep current): ";
                    std::cin >> newAmount;

                    if (orderManager.modifyOrder(orderId, newPrice, newAmount)) {
                        std::cout << "Order modified successfully.\n";
                    } else {
                        std::cout << "Failed to modify order.\n";
                    }
                    break;
                }

                case 4: { // Get Orderbook
                    std::string symbol;
                    std::cout << "Enter instrument name (e.g., BTC-PERPETUAL): ";
                    std::getline(std::cin, symbol);
                    
                    auto orderbook = apiClient.getOrderBook(symbol);
                    wsServer.broadcastOrderbookUpdates(symbol, orderbook.dump());
                    std::cout << "Orderbook data sent to WebSocket subscribers.\n";
                    break;
                }

                case 5: { // Get Positions
                    auto positions = apiClient.getPositions();
                    wsServer.broadcastOrderbookUpdates("positions", positions.dump());
                    std::cout << "Position data sent to WebSocket subscribers.\n";
                    break;
                }

                case 6: // Exit
                    running = false;
                    break;

                default:
                    std::cout << "Invalid choice. Please try again.\n";
            }
        }
        catch (const std::exception& ex) {
            std::cerr << "Error: " << ex.what() << "\n";
        }
    }

    // Cleanup
    orderbookThread.join();
    wsServer.stop();
    return 0;
}