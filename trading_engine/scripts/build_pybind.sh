#!/bin/bash
cd ../build
cmake ..
make
cp trading_engine*.so ../../api_server/
echo "✅ C++ code ready for FastAPI."