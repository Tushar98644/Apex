# Optimization and Benchmarking

This document describes the optimizations implemented for the Order Management System to reduce latency and improve performance across different areas: memory management, network communication, data structure selection, thread management, and CPU optimization.

---

## 1. Memory Management Optimization

### Approach:
- **Smart Pointers**: used `std::shared_ptr` and `std::unique_ptr` for automatic memory management, preventing memory leaks.
- **Memory Pooling**: Allocated memory for frequently used objects is pre-allocated using custom memory pools to avoid fragmentation and frequent heap allocations.
- **Avoiding Memory Copies**: References and move semantics are used wherever possible to reduce the overhead caused by copying large objects.

### Justification:
Memory optimizations reduce the overhead caused by constant allocations and deallocations, which can significantly impact performance in real-time applications like order placement and market data processing. By reducing memory fragmentation, we ensure that the system scales efficiently.

---

## 2. Network Communication Optimization

### Approach:
- **(HTTP/2 & WebSocket)**: Replaced stateless HTTP requests with persistent HTTP/2 connections and WebSocket for real-time data streaming. HTTP/2 allows multiplexing of requests over a single connection, significantly reducing the round-trip time (RTT).
- **Enable Compression for WebSocket and HTTP**: Enabled compression for WebSocket using `ws.set_option(beast::websocket::permessage_deflate());` to reduce the size of transmitted messages.
- **Reuse curl handles**: Reused cURL handles to save on repetitive allocations and deallocations.
- **Keep-alive**: Used `curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L)` to maintain persistent connections, avoiding the overhead of repeatedly setting up and tearing down TCP connections.

### Justification:
Persistent connections reduce the overhead of creating new connections, leading to faster response times and lower RTTs. Compression reduces the size of transmitted data, improving network efficiency.

---

## 3. Data Structure Selection Optimization

### Approach:
- **Orderbook and Subscription Management**: Used `unordered_map` and `unordered_set` to store subscriptions and orderbook entries, ensuring **O(1)** average-time complexity for lookups, insertions, and deletions.
- **Efficient Memory Usage**: Replaced less efficient data structures (like `std::vector`) with `std::unordered_map` or `std::unordered_set` for faster access times in managing orders and subscriptions.

### Justification:
Choosing the right data structures allows the system to manage large amounts of data efficiently. For example, using `unordered_map` for fast lookups is critical in reducing the time it takes to place and cancel orders in a high-frequency trading environment.

---

## 4. Thread Management Optimization

### Approach:
- **Thread Pooling**: The WebSocketServer uses a thread pool to handle client requests. Each incoming WebSocket connection is assigned to a worker thread via boost::asio::post, which optimizes the handling of multiple concurrent client connections.
-**Separate Threads for Key Operations**: Used separate threads for key operations like order matching,connections and market data processing to ensure that these operations are not blocked by other tasks.
- **Asynchronous Operations**: Used asynchronous operations wherever possible to avoid blocking threads and improve concurrency.
- **Mutex Optimization**: mutex locks are applied more granularly, especially around shared resources like subscriptions, to minimize blocking.

### Justification:
Using thread pooling and dedicated threads for specific tasks (e.g., accepting connections, WebSocket communication) improves concurrency and responsiveness. Fine-grained locking reduces contention, allowing multiple threads to operate simultaneously without unnecessary delays.