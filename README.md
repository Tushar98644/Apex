# Trading-System


deribit_trading_system/
│
├── src/
│   ├── main.cpp                # Entry point of the application
│   ├── order_manager.cpp       # Handles order-related functions
│   ├── websocket_server.cpp    # WebSocket server for market data streaming
│   ├── api_client.cpp          # Handles communication with Deribit API
│   ├── market_data_handler.cpp # Handles market data processing
│   ├── logger.cpp              # Logging implementation
│   ├── utils.cpp               # Helper functions (e.g., time measurement, string parsing)
│   ├── config.cpp              # Configuration loading
│   └── performance.cpp         # Performance analysis and benchmarking
│
├── include/
│   ├── order_manager.h         # Header for order management functions
│   ├── websocket_server.h      # Header for WebSocket server
│   ├── api_client.h            # Header for API client
│   ├── market_data_handler.h   # Header for market data processing
│   ├── logger.h                # Logging interface
│   ├── utils.h                 # Helper function declarations
│   ├── config.h                # Configuration structure and functions
│   ├── performance.h           # Performance analysis and benchmarking interface
│   └── common.h                # Common constants and types
│
├── tests/
│   ├── order_manager_tests.cpp       # Unit tests for order management
│   ├── websocket_server_tests.cpp    # Unit tests for WebSocket server
│   ├── api_client_tests.cpp          # Unit tests for API client
│   ├── market_data_handler_tests.cpp # Unit tests for market data handling
│   ├── performance_tests.cpp         # Performance benchmarking tests
│   └── test_utils.cpp                # Utilities for testing
│
├── config/
│   ├── config.json                   # Configuration file (e.g., API keys, WebSocket settings)
│   └── logger_config.json            # Logger configuration
│
├── docs/
│   ├── user_guide.md                 # Documentation for using the system
│   ├── developer_guide.md            # Technical documentation for developers
│   ├── performance_analysis.md       # Bonus: Performance analysis report
│   └── benchmarking_results.md       # Bonus: Benchmarking results
│
├── scripts/
│   ├── setup_env.sh                  # Script to set up the environment
│   ├── run_tests.sh                  # Script to run unit tests
│   └── run_system.sh                 # Script to start the system
│
├── third_party/
│   ├── websocketpp/                  # WebSocket++ library for WebSocket implementation
│   └── json/                         # JSON library for handling API responses
│
├── build/
│   ├── Makefile                      # Build instructions
│   └── CMakeLists.txt                # CMake configuration
│
├── logs/                             # Directory to store log files
│   └── system.log                    # System logs
│
└── README.md                         # Overview of the project
