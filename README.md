# Project Directory Structure

Below is the directory structure of the project, along with a brief explanation of each component:

```
.
├── LICENSE
├── README.md
├── bin
├── build
│   └── Makefile
├── cmd
│   └── main.cpp
├── config
│   ├── config.json
│   └── sample_config.json
├── docs
│   ├── benchmark.md
│   └── optimization.md
├── include
│   ├── api_client.hpp
│   ├── config.hpp
│   ├── latency_benchmark.hpp
│   ├── logger.hpp
│   ├── order_manager.hpp
│   └── websocket_server.hpp
├── libraries
│   └── json.hpp
├── logs
│   └── log_metrics.log
├── scripts
│   ├── build.sh
│   ├── clean_build.sh
│   └── run.sh
└── src
    ├── api_client.cpp
    ├── config.cpp
    ├── latency_benchmark.cpp
    ├── logger.cpp
    ├── order_manager.cpp
    └── websocket_server.cpp

11 directories, 25 files
```

## Directory Explanation

- **LICENSE**: Contains the licensing information for the project.
- **README.md**: Project overview, setup, and usage instructions.
- **bin/**: Output directory for compiled binaries.
- **build/**: Contains the `Makefile` used for compiling the program.
- **cmd/**: Entry point of the program, with `main.cpp` as the primary file.
- **config/**: Contains configuration files (`config.json` and `sample_config.json`) for setting up the application.
- **docs/**: Documentation files, including details on benchmarking (`benchmark.md`) and optimization (`optimization.md`).
- **include/**: Header files for the project, defining interfaces and declarations for core components.
- **libraries/**: External libraries used in the project (e.g., `json.hpp` for JSON handling).
- **logs/**: Directory for log files, such as `log_metrics.log` for tracking metrics.
- **scripts/**: Shell scripts for building, cleaning, and running the program.
  - `build.sh`: Builds the program.
  - `clean_build.sh`: Cleans up the build artifacts.
  - `run.sh`: Runs the compiled program.
- **src/**: Source code implementation of the project components.
  - Files like `api_client.cpp`, `config.cpp`, and `order_manager.cpp` implement key functionalities.

## Setup and Run Instructions

### Prerequisites
- Ensure that `make` and a C++ compiler (e.g., g++) are installed on your system.
- Install any additional dependencies required by the project(like `libcurl` for making HTTP requests).

### Steps to Build and Run the Program

1. Open the project directory in your terminal.

2. **Configuration**:
   Naviagte to `config` directory and create a `config.json` file similar to the `sample_config.json` file as needed to set up application parameters.

3. **Build the Program**:
   Navigate to the `scripts` directory and run the `build.sh` script to compile the program.
    ```bash
    ./build.sh
    ```
4. **Run the Program**:
   Use the `run.sh` script to execute the program.
   ```bash
   ./run.sh
   ```

5. **Clean Build Artifacts**:
   To clean up compiled files and reset the build directory, use the `clean_build.sh` script.
   ```bash
   ./clean_build.sh
   ```

6. **Logs**:
   Check the `logs/log_metrics.log` file for runtime metrics and logging details.

This setup can efficiently build, run, and maintain the project.
