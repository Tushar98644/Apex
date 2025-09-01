cmake_minimum_required(VERSION 3.12)
project(trading_engine)

set(CMAKE_CXX_STANDARD 17)

# Get pybind11 includes from active environment
execute_process(
    COMMAND python3 -m pybind11 --includes
    OUTPUT_VARIABLE PYBIND11_INCLUDES
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE PYBIND11_RESULT
)

if(NOT PYBIND11_RESULT EQUAL 0)
    message(FATAL_ERROR "pybind11 not found! Make sure virtual environment is activated and pybind11 is installed.")
endif()

execute_process(
    COMMAND python3-config --extension-suffix
    OUTPUT_VARIABLE PYTHON_EXT_SUFFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Include directories
include_directories(include)
include_directories(libraries)

# Find dependencies dynamically using CMake's built-in modules
find_package(CURL REQUIRED)
find_package(Boost 1.70 COMPONENTS system REQUIRED)

# Source files
set(SOURCES
    src/python_bindings.cpp
    src/api_client.cpp
    src/config.cpp
    src/order_manager.cpp
    src/logger.cpp
    src/websocket_server.cpp
    src/latency_benchmark.cpp
)

# Create Python module
add_library(trading_engine MODULE ${SOURCES})

# Set module properties
set_target_properties(trading_engine PROPERTIES
    PREFIX ""
    SUFFIX ${PYTHON_EXT_SUFFIX}
)

# Parse and add pybind11 includes
string(REPLACE " -I" ";-I" PYBIND11_INCLUDES_LIST ${PYBIND11_INCLUDES})
separate_arguments(PYBIND11_INCLUDES_LIST)
target_compile_options(trading_engine PRIVATE ${PYBIND11_INCLUDES_LIST})

# macOS-specific linker flags
if(APPLE)
    set_target_properties(trading_engine PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Link libraries using modern CMake targets
target_link_libraries(trading_engine PRIVATE 
    CURL::libcurl      # Modern CMake target for CURL
    Boost::system      # Modern CMake target for Boost
)

# Optimization flags
target_compile_options(trading_engine PRIVATE -O3 -march=native)