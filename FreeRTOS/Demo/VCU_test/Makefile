# Makefile for VCU Test Suite
# Tests for throttle.c and utils.c functions

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 -DTickType_t=uint32_t
LDFLAGS = -lm

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
VCU_SRC = ../VCU_app/src
VCU_INC = ../VCU_app/include

# Include paths (test includes first to override original headers)
INCLUDES = -I$(INC_DIR) -I$(VCU_INC)

# Source files
TEST_HELPERS = $(SRC_DIR)/test_helpers.c
CONSOLE_MOCK = $(SRC_DIR)/console.c
VCU_THROTTLE = $(VCU_SRC)/throttle.c
VCU_UTILS = $(VCU_SRC)/utils.c

# Test executables
TEST_THROTTLE = $(BUILD_DIR)/test_throttle
TEST_UTILS = $(BUILD_DIR)/test_utils

# All tests
ALL_TESTS = $(TEST_THROTTLE) $(TEST_UTILS)

# Phony targets
.PHONY: all clean test test-throttle test-utils help

# Default target
all: $(BUILD_DIR) $(ALL_TESTS)

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Build test_throttle
$(TEST_THROTTLE): $(SRC_DIR)/test_throttle.c $(TEST_HELPERS) $(CONSOLE_MOCK) $(VCU_THROTTLE)
	@echo "Building throttle tests..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)
	@echo "✓ Throttle tests compiled successfully"

# Build test_utils
$(TEST_UTILS): $(SRC_DIR)/test_utils.c $(TEST_HELPERS) $(CONSOLE_MOCK) $(VCU_UTILS) $(VCU_THROTTLE)
	@echo "Building utils tests..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)
	@echo "✓ Utils tests compiled successfully"

# Run all tests
test: all
	@echo ""
	@echo "═══════════════════════════════════════"
	@echo "  Running All Tests"
	@echo "═══════════════════════════════════════"
	@echo ""
	@$(TEST_THROTTLE)
	@echo ""
	@$(TEST_UTILS)
	@echo ""
	@echo "═══════════════════════════════════════"
	@echo "  All Tests Complete"
	@echo "═══════════════════════════════════════"

# Run throttle tests only
test-throttle: $(TEST_THROTTLE)
	@echo ""
	@echo "Running Throttle Tests..."
	@$(TEST_THROTTLE)

# Run utils tests only
test-utils: $(TEST_UTILS)
	@echo ""
	@echo "Running Utils Tests..."
	@$(TEST_UTILS)

# Clean build artifacts
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "✓ Clean complete"

# Help target
help:
	@echo "VCU Test Suite Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all           - Build all test executables (default)"
	@echo "  test          - Build and run all tests"
	@echo "  test-throttle - Build and run throttle tests only"
	@echo "  test-utils    - Build and run utils tests only"
	@echo "  clean         - Remove build directory and artifacts"
	@echo "  help          - Display this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make              # Build all tests"
	@echo "  make test         # Build and run all tests"
	@echo "  make test-throttle # Run only throttle tests"
	@echo "  make clean        # Clean build artifacts"
