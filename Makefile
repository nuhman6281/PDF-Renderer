# Makefile for PostScript Interpreter
# Simple build system for the PostScript interpreter project

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lz

# Directories
SRCDIR = src
BUILDDIR = build
BINDIR = bin

# Source files (including PDF components)
SOURCES = $(wildcard $(SRCDIR)/*.cpp) \
          $(wildcard $(SRCDIR)/data_structures/*.cpp) \
          $(wildcard $(SRCDIR)/interpreter/*.cpp) \
          $(wildcard $(SRCDIR)/parser/*.cpp)

# Object files
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

# Target executable
TARGET = $(BINDIR)/ps_interpreter

# Main target
all: $(TARGET)

# Create directories
$(BUILDDIR):
	mkdir -p $(BUILDDIR)
	mkdir -p $(BUILDDIR)/data_structures
	mkdir -p $(BUILDDIR)/interpreter
	mkdir -p $(BUILDDIR)/parser
	mkdir -p $(BINDIR)

# Build object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build executable
$(TARGET): $(OBJECTS) | $(BUILDDIR)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Clean build files
clean:
	rm -rf $(BUILDDIR) $(BINDIR)

# Run the interpreter
run: $(TARGET)
	./$(TARGET)

# Test with example file
test: $(TARGET)
	./$(TARGET) examples/test.ps

# Graphics test
graphics-test: $(TARGET)
	./$(TARGET) examples/graphics_test.ps

# Advanced features test
advanced-test: $(TARGET)
	./$(TARGET) examples/advanced_features.ps

# Install (optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall (optional)
uninstall:
	rm -f /usr/local/bin/ps_interpreter

# Help
help:
	@echo "Available targets:"
	@echo "  all       - Build the PostScript interpreter"
	@echo "  clean     - Remove build files"
	@echo "  run       - Run the interpreter"
	@echo "  test      - Run with test.ps"
	@echo "  graphics-test - Run graphics test"
	@echo "  advanced-test - Run advanced features test"
	@echo "  install   - Install to /usr/local/bin"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  help      - Show this help"

# PDF test (exclude build/main.o)
PDF_TEST_OBJECTS = $(filter-out build/main.o, $(OBJECTS))
bin/pdf_test: $(PDF_TEST_OBJECTS) examples/pdf_test.cpp
	$(CXX) $(CXXFLAGS) $(PDF_TEST_OBJECTS) examples/pdf_test.cpp -o bin/pdf_test $(LDFLAGS)

pdf-test: bin/pdf_test
	bin/pdf_test

.PHONY: all clean run test install uninstall help graphics-test advanced-test pdf-test 