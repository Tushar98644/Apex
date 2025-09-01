#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "order_manager.hpp"
#include "api_client.hpp"
#include "config.hpp"

namespace py = pybind11;

PYBIND11_MODULE(trading_engine, m) {
    m.doc() = "Trading Engine Python bindings";
    
    // Bind Config class
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("load", &Config::load, "Load configuration from file")
        .def("getClientId", &Config::getClientId, "Get client ID")
        .def("getClientSecret", &Config::getClientSecret, "Get client secret");
    
    // Bind Order struct
    py::class_<Order>(m, "Order")
        .def_readwrite("order_id", &Order::order_id)
        .def_readwrite("instrument_name", &Order::instrument_name)
        .def_readwrite("price", &Order::price)
        .def_readwrite("amount", &Order::amount);
    
    // Bind APIClient class
    py::class_<APIClient>(m, "APIClient")
        .def(py::init<const std::string&, const std::string&>(), 
             "Create API client with credentials");
    
    // Bind OrderManager class
    py::class_<OrderManager>(m, "OrderManager")
        .def(py::init<APIClient&>(), "Create OrderManager with APIClient")
        .def("placeOrder", &OrderManager::placeOrder, 
             "Place a new order", 
             py::arg("instrument_name"), py::arg("type"), py::arg("amount"), py::arg("price"))
        .def("cancelOrder", &OrderManager::cancelOrder, 
             "Cancel an existing order", py::arg("order_id"))
        .def("modifyOrder", &OrderManager::modifyOrder, 
             "Modify an existing order", 
             py::arg("order_id"), py::arg("new_price"), py::arg("new_amount"))
        .def("getActiveOrders", &OrderManager::getActiveOrders, 
             "Get all active orders")
        .def("getOrderBook", &OrderManager::getOrderBook,
             "Get orderbook for instrument", py::arg("instrument_name"))
        .def("getPositions", &OrderManager::getPositions,
             "Get current positions");
}