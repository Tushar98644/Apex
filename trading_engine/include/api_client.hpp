#ifndef API_CLIENT_HPP
#define API_CLIENT_HPP

#include <string>
#include <json.hpp>
#include <thread>
#include <functional>

class APIClient
{
public:
    APIClient(const std::string &client_id, const std::string &client_secret);

    // Core order management functions
    nlohmann::json placeOrder(const std::string &instrument_name, const std::string &type, double amount, double price);
    nlohmann::json cancelOrder(const std::string &order_id);
    nlohmann::json modifyOrder(const std::string &order_id, double new_price, double new_amount);
    nlohmann::json getOrderBook(const std::string &instrument_name);
    nlohmann::json getPositions();

    // WebSocket functionality
    void connectToWebSocket(const std::string &url, const std::function<void(const std::string &)> &onMessageCallback);
    void sendWebSocketMessage(const std::string &message);
    void disconnectWebSocket();

private:
    // Authentication
    void authenticate();

    // Helper methods
    nlohmann::json sendGetRequest(const std::string &endpoint);

    // CURL-based HTTP attributes
    std::string client_id;
    std::string client_secret;
    std::string access_token;

    // WebSocket attributes
    std::unique_ptr<std::thread> webSocketThread;
    bool webSocketRunning;
};

#endif
