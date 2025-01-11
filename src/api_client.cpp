#include "api_client.hpp"
#include <iostream>
#include <curl/curl.h>
#include <sstream>
#include <iomanip>
#include <string>

APIClient::APIClient(const std::string &client_id, const std::string &client_secret)
    : client_id(client_id), client_secret(client_secret)
{
    authenticate();
}

void APIClient::authenticate()
{
    std::string endpoint = "/api/v2/public/auth";
    std::string query = "?grant_type=client_credentials&client_id=" + client_id + "&client_secret=" + client_secret;
    auto response = sendGetRequest(endpoint + query);
    std::cout << "Authentication response: " << response.dump(4) << std::endl;
    if (response.contains("result") && response["result"].contains("access_token"))
    {
        access_token = response["result"]["access_token"];
    }
    else
    {
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

    std::cout << "API Response from place order: " << response.dump(4) << "\n";

    return response;

}

nlohmann::json APIClient::cancelOrder(const std::string &order_id)
{
    auto response = sendGetRequest("/api/v2/private/cancel?order_id=" + order_id);
    std::cout << "API Response from cancel order: " << response.dump(4) << "\n";
    return response;
}

nlohmann::json APIClient::modifyOrder(const std::string &order_id, double new_price, double new_amount)
{
    std::string query =
        "?order_id=" + order_id +
        "&price=" + std::to_string(new_price) +
        "&amount=" + std::to_string(new_amount);
    auto response = sendGetRequest("/api/v2/private/edit" + query);
    std::cout << "API Response from modify order: " << response.dump(4) << "\n";
    return response;
}

nlohmann::json APIClient::getOrderBook(const std::string &instrument_name)
{
    auto response = sendGetRequest("/api/v2/public/get_order_book?instrument_name=" + instrument_name);
    std::cout << "API Response from get order book: " << response.dump(4) << "\n";
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

nlohmann::json APIClient::sendGetRequest(const std::string &endpoint)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        throw std::runtime_error("CURL initialization failed");
    }

    std::string responseStr;
    std::string url = "https://test.deribit.com" + endpoint;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);

    struct curl_slist *headers = nullptr;
    if (!access_token.empty())
    {
        std::string authHeader = "Authorization: Bearer " + access_token;
        headers = curl_slist_append(headers, authHeader.c_str());
    }
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        throw std::runtime_error("GET request failed: " + std::string(curl_easy_strerror(res)));
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    try
    {
        return nlohmann::json::parse(responseStr);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("Failed to parse JSON: " + std::string(e.what()) + "\nResponse: " + responseStr);
    }
}
