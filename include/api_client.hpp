#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <string>
#include <json.hpp>

class APIClient {
public:
    APIClient(const std::string& client_id, const std::string& client_secret);

    nlohmann::json placeOrder(const std::string& instrument_name, const std::string& type, double amount, double price);
    nlohmann::json cancelOrder(const std::string& order_id);
    nlohmann::json modifyOrder(const std::string& order_id, double new_price, double new_amount);
    nlohmann::json getOrderBook(const std::string& instrument_name);
    nlohmann::json getPositions();

private:
    std::string client_id;
    std::string client_secret;
    std::string access_token;

    void authenticate(); 
    nlohmann::json sendGetRequest(const std::string& endpoint);
};

#endif
