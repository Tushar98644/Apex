#include "websocket_server.hpp"
#include <boost/beast/websocket.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

WebSocketServer::WebSocketServer() : running_(false) {}

WebSocketServer::~WebSocketServer() {
    if (running_) {
        ioc_.stop();
        if (serverThread_.joinable())
            serverThread_.join();
    }
}

void WebSocketServer::start(uint16_t port) {
    if (running_.exchange(true)) {
        throw std::runtime_error("Server is already running.");
    }

    serverThread_ = std::thread(&WebSocketServer::acceptConnections, this, port);
}

void WebSocketServer::run() {
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
}

void WebSocketServer::acceptConnections(uint16_t port) {
    try {
        tcp::acceptor acceptor(ioc_, tcp::endpoint(tcp::v4(), port));
        std::cout << "Listening for WebSocket connections on port " << port << "...\n";

        while (running_) {
            tcp::socket socket(ioc_);
            acceptor.accept(socket);

            beast::websocket::stream<tcp::socket> ws(std::move(socket));
            ws.accept();

            std::cout << "New WebSocket connection established.\n";

            // Echo loop
            for (;;) {
                beast::flat_buffer buffer;
                ws.read(buffer);

                std::string message = beast::buffers_to_string(buffer.data());
                std::cout << "Received: " << message << "\n";

                ws.text(ws.got_text());
                ws.write(buffer.data()); // Echo the message back
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "WebSocket server exception: " << e.what() << "\n";
    }
}
