#include "websocket_server.hpp"
#include "latency_benchmark.hpp"
#include <iostream>
#include <chrono>
#include <boost/beast/core/buffers_to_string.hpp>
#include <thread>

WebSocketServer::WebSocketServer() : running_(false) {}

WebSocketServer::~WebSocketServer() {
    stop();
}

void WebSocketServer::start(uint16_t port) {
    if (running_) return;
    running_ = true;
    serverThread_ = std::thread([this, port]() { acceptLoop(port); });
}

void WebSocketServer::stop() {
    if (!running_) return;
    running_ = false;
    ioc_.stop();
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
}

void WebSocketServer::run() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

std::unordered_map<std::string, std::unordered_set<std::shared_ptr<beast::websocket::stream<tcp::socket>>>> WebSocketServer::getSubscriptions() {
    std::lock_guard<std::mutex> lock(mutex_); 
    return subscriptions_;
}

void WebSocketServer::acceptLoop(uint16_t port) {
    try {
        tcp::acceptor acceptor(ioc_, {tcp::v4(), port});
        std::cout << "WebSocket server listening on port " << port << std::endl;

        while (running_) {
            try {
                tcp::socket socket(ioc_);
                acceptor.accept(socket);
                
                auto ws = std::make_shared<beast::websocket::stream<tcp::socket>>(std::move(socket));
                ws->accept();
                
                std::thread([this, ws]() {
                    try {
                        handleClient(ws);
                    } catch (const std::exception& e) {
                        std::cerr << "Client handler error: " << e.what() << std::endl;
                    }
                }).detach();
            } catch (const std::exception& e) {
                std::cerr << "Accept iteration error: " << e.what() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Accept loop error: " << e.what() << std::endl;
    }
}

void WebSocketServer::handleClient(std::shared_ptr<beast::websocket::stream<tcp::socket>> ws) {
    try {
        auto endpoint = ws->next_layer().remote_endpoint();
        std::string clientId = endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
        std::cout << "Client connected: " << clientId << std::endl;

        while (running_) {
            beast::flat_buffer buffer;
            ws->read(buffer);
            
            std::string message = beast::buffers_to_string(buffer.data());
            
            if (message.find("subscribe:") == 0) {
                std::string symbol = message.substr(10);
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    subscriptions_[symbol].insert(ws);
                }
                std::cout << "Client " << clientId << " subscribed to " << symbol << std::endl;
                ws->write(asio::buffer("Subscribed to " + symbol));
            }
            else if (message.find("unsubscribe:") == 0) {
                std::string symbol = message.substr(12);
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    subscriptions_[symbol].erase(ws);
                    if (subscriptions_[symbol].empty()) {
                        subscriptions_.erase(symbol);
                    }
                }
                std::cout << "Client " << clientId << " unsubscribed from " << symbol << std::endl;
            }
        }
    }
    catch (const beast::system_error& e) {
        if (e.code() != beast::websocket::error::closed) {
            std::cerr << "WebSocket error: " << e.what() << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void WebSocketServer::broadcastOrderbookUpdates(const std::string& symbol, const std::string& data) {
    auto start = LatencyBenchmark::start();
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = subscriptions_.find(symbol);
    if (it != subscriptions_.end()) {
        for (auto client_it = it->second.begin(); client_it != it->second.end();) {
            try {
                (*client_it)->write(asio::buffer(data));
                ++client_it;
            }
            catch (...) {
                client_it = it->second.erase(client_it);
            }
        }
        
        if (it->second.empty()) {
            subscriptions_.erase(it);
        }
    }
    LatencyBenchmark::end(start, "WebSocket Message Propagation Delay");
}