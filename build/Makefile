# Variables
CXX = clang++
CXXFLAGS = -I../include -I/opt/homebrew/Cellar/boost/1.87.0/include -I../libraries -Wall -Wextra -std=c++17
SRCDIR = ../src
BINDIR = ../bin
CMDDIR = ../cmd
BUILDDIR = ../build
LDFLAGS = -L/opt/homebrew/Cellar/boost/1.87.0/lib
LIBS = -lboost_system

# Source files
SOURCES =$(CMDDIR)/main.cpp $(SRCDIR)/config.cpp ${SRCDIR}/websocket_server.cpp

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
