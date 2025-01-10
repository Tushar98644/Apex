#include "api_client.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

APIClient::APIClient(const std::string& baseUrl) : baseUrl_(baseUrl) {}

std::string APIClient::getMarketData(const std::string& instrument) {
    try {
        asio::io_context ioc;

        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // Resolve the server
        auto const results = resolver.resolve("www.deribit.com", "443");
        stream.connect(results);

        // Set up an HTTP GET request
        http::request<http::string_body> req{http::verb::get,
                                             "/api/v2/public/ticker?instrument_name=" + instrument,
                                             11};
        req.set(http::field::host, "www.deribit.com");
        req.set(http::field::user_agent, "DeribitClient");

        // Send the HTTP request
        http::write(stream, req);

        // Receive the HTTP response
        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(stream, buffer, res);

        // Gracefully close the socket
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        // Handle response
        if (ec && ec != beast::errc::not_connected) {
            throw beast::system_error{ec};
        }

        return beast::buffers_to_string(res.body().data());
    } catch (const std::exception& e) {
        std::cerr << "Error fetching market data: " << e.what() << "\n";
        return "";
    }
}
