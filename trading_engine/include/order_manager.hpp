#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include "api_client.hpp"
#include <unordered_map>

struct Order {
    std::string order_id;
    std::string instrument_name;
    double price;
    double amount;
};

class OrderManager {
public:
    OrderManager(APIClient& api_client);

    std::string placeOrder(const std::string& instrument_name, const std::string& type, double amount, double price);
    bool cancelOrder(const std::string& order_id);
    bool modifyOrder(const std::string& order_id, double new_price, double new_amount);

    std::unordered_map<std::string, Order> getActiveOrders() const;

private:
    APIClient& api_client;
    std::unordered_map<std::string, Order> active_orders;
};

#endif
