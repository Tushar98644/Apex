#include "api_client.hpp"
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include "latency_benchmark.hpp"

namespace beast = boost::beast;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

APIClient::APIClient(const std::string &client_id, const std::string &client_secret)
    : client_id(client_id), client_secret(client_secret)
{
    authenticate();
}

void APIClient::authenticate() {
    std::string endpoint = "/api/v2/public/auth";
    std::string query = "?grant_type=client_credentials&client_id=" + client_id + "&client_secret=" + client_secret;

    auto response = sendGetRequest(endpoint + query);

    if (response.contains("result") && response["result"].contains("access_token")) {
        access_token = response["result"]["access_token"];
    } else {
        throw std::runtime_error("Authentication failed: " + response.dump());
    }
}

nlohmann::json APIClient::placeOrder(const std::string &instrument_name, const std::string &type, double amount, double price) {
    std::string query =
        "?instrument_name=" + instrument_name +
        "&type=" + type +
        "&amount=" + std::to_string(amount);

    if (type == "limit") {
        query += "&price=" + std::to_string(price);
    }

    auto response = sendGetRequest("/api/v2/private/buy" + query);
    return response;
}

nlohmann::json APIClient::cancelOrder(const std::string &order_id)
{
    auto response = sendGetRequest("/api/v2/private/cancel?order_id=" + order_id);
    return response;
}

nlohmann::json APIClient::modifyOrder(const std::string &order_id, double new_price, double new_amount)
{
    std::string query =
        "?order_id=" + order_id +
        "&price=" + std::to_string(new_price) +
        "&amount=" + std::to_string(new_amount);
    auto response = sendGetRequest("/api/v2/private/edit" + query);
    return response;
}

nlohmann::json APIClient::getOrderBook(const std::string &instrument_name)
{
    auto start = LatencyBenchmark::start();
    auto response = sendGetRequest("/api/v2/public/get_order_book?instrument_name=" + instrument_name);
    LatencyBenchmark::end(start, "Market Data Processing Latency for " + instrument_name);
    return response;
}

nlohmann::json APIClient::getPositions()
{
    auto reponse = sendGetRequest("/api/v2/private/get_positions");
    std::cout << "API Response from get positions: " << reponse.dump(4) << "\n";
    return reponse;
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *out)
{
    size_t totalSize = size * nmemb;
    if (out)
    {
        out->append((char *)contents, totalSize);
    }
    return totalSize;
}

nlohmann::json APIClient::sendGetRequest(const std::string &endpoint) {
    static CURL *curl = curl_easy_init(); // Persistent CURL instance
    if (!curl) {
        throw std::runtime_error("CURL initialization failed");
    }

    std::string responseStr;
    std::string url = "https://test.deribit.com" + endpoint;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);

    // Enable persistent connections
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 0L);
    curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 0L);

    // Use HTTP/2
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

    // Token handling
    struct curl_slist *headers = nullptr;
    if (!access_token.empty()) {
        std::string authHeader = "Authorization: Bearer " + access_token;
        headers = curl_slist_append(headers, authHeader.c_str());
    }
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw std::runtime_error("GET request failed: " + std::string(curl_easy_strerror(res)));
    }

    curl_slist_free_all(headers);

    try {
        return nlohmann::json::parse(responseStr);
    } catch (const std::exception &e) {
        throw std::runtime_error("Failed to parse JSON: " + std::string(e.what()) + "\nResponse: " + responseStr);
    }
}

void APIClient::connectToWebSocket(const std::string &url, const std::function<void(const std::string&)> &onMessageCallback) {
    webSocketRunning = true;

    webSocketThread = std::make_unique<std::thread>([this, url, onMessageCallback]() {
        try {
            asio::io_context ioc;
            tcp::resolver resolver(ioc);
            beast::websocket::stream<tcp::socket> ws(ioc);

            auto const results = resolver.resolve("test.deribit.com", "443");
            asio::connect(ws.next_layer(), results);

            ws.set_option(beast::websocket::permessage_deflate());
            ws.handshake("test.deribit.com", "/ws/api/v2");

            std::cout << "Connected to WebSocket: " << url << std::endl;

            beast::flat_buffer buffer;
            while (webSocketRunning) {
                buffer.consume(buffer.size());
                ws.read(buffer);

                std::string message = beast::buffers_to_string(buffer.data());
                onMessageCallback(message);
            }

            ws.close(beast::websocket::close_code::normal);
        } catch (const std::exception &e) {
            std::cerr << "WebSocket exception: " << e.what() << std::endl;
        }
    });
}

void APIClient::sendWebSocketMessage(const std::string &message) {
    if (!webSocketRunning) {
        throw std::runtime_error("WebSocket is not connected.");
    }

    std::cout << "Sending WebSocket message: " << message << std::endl;
}

void APIClient::disconnectWebSocket() {
    webSocketRunning = false;
    if (webSocketThread && webSocketThread->joinable()) {
        webSocketThread->join();
    }
    std::cout << "WebSocket disconnected." << std::endl;
}