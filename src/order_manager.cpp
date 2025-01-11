#include "order_manager.hpp"
#include <iostream>

OrderManager::OrderManager(APIClient& api_client) : api_client(api_client) {}

std::string OrderManager::placeOrder(const std::string& instrument_name, const std::string& type, double amount, double price) {
    auto response = api_client.placeOrder(instrument_name, type, amount, price);

    if (response.contains("result") && response["result"].contains("order") && response["result"]["order"].contains("order_id")) {
        std::string order_id = response["result"]["order"]["order_id"];
        active_orders[order_id] = {order_id, instrument_name, price, amount};
        return order_id;
    }

    std::cout << "Failed response from API: " << response.dump(4) << "\n";

    return "";
}


bool OrderManager::cancelOrder(const std::string& order_id) {
    auto response = api_client.cancelOrder(order_id);
    if (response.contains("result") && response["result"].contains("order_state") && response["result"]["order_state"] == "cancelled") {
        active_orders.erase(order_id);
        return true;
    }
    return false;
}

bool OrderManager::modifyOrder(const std::string& order_id, double new_price, double new_amount) {
    auto response = api_client.modifyOrder(order_id, new_price, new_amount);
    if (response.contains("result") && response["result"].contains("order") && response["result"]["order"].contains("order_id")) {
        active_orders[order_id] = {order_id, active_orders[order_id].instrument_name, new_price, new_amount};
        return true;
    }
    return false;
}

std::unordered_map<std::string, Order> OrderManager::getActiveOrders() const {
    return active_orders;
}
