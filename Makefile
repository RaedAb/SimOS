# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Iinclude

# Directories
SRCDIR = src
BUILDDIR = build
INCLUDEDIR = include
TESTDIR = test_driver

# Source files
SRCS = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(TESTDIR)/main.cpp)

# Object files
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(filter $(SRCDIR)/%.cpp,$(SRCS))) \
       $(patsubst $(TESTDIR)/%.cpp,$(BUILDDIR)/%.o,$(filter $(TESTDIR)/%.cpp,$(SRCS)))

# Executable name
EXEC = runme

# Default target
all: $(EXEC)

# Link the executable
$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@

# Compile source files to object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: $(TESTDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Clean up build directory and executable
clean:
	rm -rf $(BUILDDIR) $(EXEC)

# Phony targets
.PHONY: all clean