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
- **Thread Pooling**: Used a thread pool to manage multiple threads for WebSocket message handling, avoiding the overhead of creating and destroying threads for each incoming connection.
- **Mutex Optimization**: Replaced global mutexes with finer-grained locks where possible to reduce contention. Critical sections were minimized to avoid unnecessary thread blocking.

### Justification:
Using a thread pool avoids the overhead of creating and destroying threads dynamically. Fine-grained locking ensures minimal blocking, allowing multiple threads to work concurrently.

---
<!-- 
## 5. CPU Optimization

### Objective:
CPU optimization aims to reduce the computational overhead and ensure that the CPU is used efficiently, minimizing delays in processing orders and market data.

### Approach:
- **Efficient Algorithm Selection**: Optimized order matching algorithms to reduce unnecessary comparisons.
- **Profiling and Hotspot Identification**: Used tools like `gprof` and `valgrind` to identify performance bottlenecks in the code and refactor critical areas to improve CPU usage.
- **Vectorization**: Applied SIMD (Single Instruction, Multiple Data) instructions where applicable to accelerate data processing tasks.

### Justification:
By optimizing algorithms and identifying CPU bottlenecks, we reduce the time spent in processing orders and market data. SIMD instructions help in utilizing the full capacity of modern processors. -->
