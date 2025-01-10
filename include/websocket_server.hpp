#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <thread>
#include <atomic>

class WebSocketServer {
public:
    WebSocketServer();
    ~WebSocketServer();

    void start(uint16_t port);
    void run();

private:
    boost::asio::io_context ioc_;
    std::thread serverThread_;
    std::atomic<bool> running_;

    void acceptConnections(uint16_t port);
};

#endif 
