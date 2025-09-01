#include <iostream>
#include "config.hpp"
#include "api_client.hpp"
#include "order_manager.hpp"
#include "websocket_server.hpp"
#include "latency_benchmark.hpp"
#include <thread>
#include <atomic>

std::atomic<bool> running(true);

void signalHandler(int signum)
{
    running = false;
}

void displayMenu()
{
    std::cout << "\nDeribit Trading System Menu:\n";
    std::cout << "1. Place Order\n";
    std::cout << "2. Cancel Order\n";
    std::cout << "3. Modify Order\n";
    std::cout << "4. Get Orderbook\n";
    std::cout << "5. Get Positions\n";
    std::cout << "6. Exit\n";
    std::cout << "Enter your choice: \n";
}

void processOrderbook(WebSocketServer &wsServer, APIClient &apiClient)
{
    while (running)
    {
        auto subscriptions = wsServer.getSubscriptions();

        for (const auto &[symbol, clients] : subscriptions)
        {
            if (clients.empty())
                continue;

            try
            {
                auto orderbook = apiClient.getOrderBook(symbol);

                std::string orderbookStr = orderbook.dump();

                wsServer.broadcastOrderbookUpdates(symbol, orderbookStr);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error fetching orderbook for symbol " << symbol << ": " << e.what() << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    signal(SIGINT, signalHandler);

    Config config;
    if (!config.load("../config/config.json"))
    {
        std::cerr << "Failed to load configuration. Exiting...\n";
        return 1;
    }

    std::cout << "Deribit Trading System Initialized Successfully!\n";

    LatencyBenchmark::initialize();

    APIClient apiClient(config.getClientId(), config.getClientSecret());
    OrderManager orderManager(apiClient);

    WebSocketServer wsServer;
    try
    {
        wsServer.start(9002);
        std::cout << "WebSocket server started on port 9002 \n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to start WebSocket server: " << e.what() << "\n";
        return 1;
    }

    std::thread orderbookThread([&](){ processOrderbook(wsServer, apiClient); });

    while (running)
    {
        displayMenu();
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try
        {
            switch (choice)
            {
            case 1:
            {
                std::string instrument_name, type;
                double amount, price;

                std::cout << "Enter instrument name (e.g., BTC-PERPETUAL): ";
                std::getline(std::cin, instrument_name);

                std::cout << "Enter order type (limit/market): ";
                std::getline(std::cin, type);

                std::cout << "Enter amount: ";
                std::cin >> amount;

                if (type == "limit")
                {
                    std::cout << "Enter price: ";
                    std::cin >> price;
                }
                else
                {
                    price = 0;
                }

                std::string orderId = orderManager.placeOrder(instrument_name, type, amount, price);
                if (!orderId.empty())
                {
                    std::cout << "Order placed successfully. Order ID: " << orderId << "\n";
                }
                else
                {
                    std::cout << "Failed to place order.\n";
                }
                break;
            }

            case 2:
            {
                std::string orderId;
                std::cout << "Enter order ID to cancel: ";
                std::getline(std::cin, orderId);

                if (orderManager.cancelOrder(orderId))
                {
                    std::cout << "Order canceled successfully.\n";
                }
                else
                {
                    std::cout << "Failed to cancel order.\n";
                }
                break;
            }

            case 3:
            {
                std::string orderId;
                double newPrice, newAmount;

                std::cout << "Enter order ID to modify: ";
                std::getline(std::cin, orderId);

                std::cout << "Enter new price (0 to keep current): ";
                std::cin >> newPrice;

                std::cout << "Enter new amount (0 to keep current): ";
                std::cin >> newAmount;

                if (orderManager.modifyOrder(orderId, newPrice, newAmount))
                {
                    std::cout << "Order modified successfully.\n";
                }
                else
                {
                    std::cout << "Failed to modify order.\n";
                }
                break;
            }

            case 4:
            {
                std::string symbol;
                std::cout << "Enter instrument name (e.g., BTC-PERPETUAL): ";
                std::getline(std::cin, symbol);

                std::cout << orderManager.getOrderBook(symbol);

                break;
            }

            case 5:
            {
                auto positions = orderManager.getPositions();
                wsServer.broadcastOrderbookUpdates("positions", positions.dump());
                std::cout << "Position data sent to WebSocket subscribers.\n";
                break;
            }

            case 6:
                running = false;
                break;

            default:
                std::cout << "Invalid choice. Please try again.\n";
            }
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Error: " << ex.what() << "\n";
        }
    }

    orderbookThread.join();
    wsServer.stop();
    return 0;
}