#ifndef WEBSOCKET_SERVER_HPP
#define WEBSOCKET_SERVER_HPP

#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <mutex>
#include <thread>

namespace beast = boost::beast;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

class WebSocketServer {
public:
    WebSocketServer();
    ~WebSocketServer();
    
    void start(uint16_t port);
    void stop();
    void run(); // New method to keep server running
    bool isRunning() const { return running_; }
    void broadcastOrderbookUpdates(const std::string& symbol, const std::string& data);

private:
    void acceptLoop(uint16_t port);
    void handleClient(std::shared_ptr<beast::websocket::stream<tcp::socket>> ws);
    
    std::atomic<bool> running_;
    asio::io_context ioc_;
    std::thread serverThread_;
    std::mutex mutex_;
    std::unordered_map<std::string, std::unordered_set<std::shared_ptr<beast::websocket::stream<tcp::socket>>>> subscriptions_;
};

#endif