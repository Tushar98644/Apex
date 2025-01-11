#include <iostream>
#include "config.hpp"
#include "api_client.hpp"
#include "order_manager.hpp"

int main()
{
    Config config;
    if (!config.load("../config/config.json"))
    {
        std::cerr << "Failed to load configuration. Exiting...\n";
        return 1;
    }

    std::cout << "Deribit Trading System Initialized Successfully!\n";

    APIClient apiClient(config.getClientId(), config.getClientSecret());

    OrderManager orderManager(apiClient);

    try
    {
        std::string instrument_name = "BTC-PERPETUAL";
        std::string type = "limit";
        double amount = 10.0;
        double price = 50000.0;

        std::string orderId = orderManager.placeOrder(instrument_name, type, amount, price);
        if (!orderId.empty())
        {
            std::cout << "Order placed successfully. Order ID: " << orderId << "\n";

            auto activeOrders = orderManager.getActiveOrders();
            std::cout << "Current active orders:\n";
            for (const auto &[id, order] : activeOrders)
            {
                std::cout << "Order ID: " << id << ", Instrument: " << order.instrument_name
                          << ", Price: " << order.price << ", Amount: " << order.amount << "\n";
            }

            if (orderManager.cancelOrder(orderId))
            {
                std::cout << "Order canceled successfully.\n";
            }
            else
            {
                std::cout << "Failed to cancel the order.\n";
            }

            apiClient.getOrderBook("BTC-PERPETUAL");
            apiClient.getPositions();
        }
        else
        {
            std::cout << "Failed to place order.\n";
        }
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error during OrderManager operations: " << ex.what() << "\n";
    }

    return 0;
}
