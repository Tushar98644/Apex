# Variables
CXX = clang++
CXXFLAGS = -I../include -I/opt/homebrew/Cellar/boost/1.87.0/include -I/opt/homebrew/Cellar/openssl@3/3.4.0/include -I/opt/homebrew/Cellar/curl/8.11.0_1/include -I../libraries -Wall -Wextra -std=c++17
SRCDIR = ../src
BINDIR = ../bin
CMDDIR = ../cmd
BUILDDIR = ../build
LDFLAGS = -L/opt/homebrew/Cellar/boost/1.87.0/lib -L/opt/homebrew/Cellar/openssl@3/3.4.0/lib -L/opt/homebrew/Cellar/curl/8.11.0_1/lib
LIBS = -lboost_system -lssl -lcrypto -lcurl

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(CMDDIR)/*.cpp)
# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Output executable
EXECUTABLE = $(BINDIR)/main

# Default target
all: $(EXECUTABLE) clean_executables

# Rule to build the executable
$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $(EXECUTABLE)

# Rule to build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create bin directory if it does not exist
$(BINDIR):
	mkdir -p $(BINDIR)

# Clean up build files
clean:
	rm -f $(SRCDIR)/*.o $(CMDDIR)/*.o $(BINDIR)/main $(BUILDDIR)/*.o

.PHONY: all clean
