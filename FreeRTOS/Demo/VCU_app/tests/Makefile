# ============================================================================
# VCU Test Framework Makefile
# ============================================================================
# Tests for: throttle.c, utils.c
# ============================================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -lm
CFLAGS_DEBUG = $(CFLAGS) -g -O0 -DDEBUG
CFLAGS_RELEASE = $(CFLAGS) -O2

# Directories
TEST_DIR = .
SRC_DIR = ../src
INC_DIR = ../include
BUILD_DIR = ./build

# Source files
TEST_SOURCES = $(TEST_DIR)/test_main.c \
               $(TEST_DIR)/test_throttle.c \
               $(TEST_DIR)/test_utils.c

# Object files
TEST_OBJS = $(BUILD_DIR)/test_main.o \
            $(BUILD_DIR)/test_throttle.o \
            $(BUILD_DIR)/test_utils.o

# Executable
TEST_EXEC = $(BUILD_DIR)/test_runner

# ============================================================================
# Default Target
# ============================================================================

.PHONY: all
all: build run

# ============================================================================
# Build Targets
# ============================================================================

.PHONY: build
build: prepare $(TEST_EXEC)

.PHONY: prepare
prepare:
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_main.o: $(TEST_DIR)/test_main.c
	@echo "$(COLOR_YELLOW)[COMPILE]$(COLOR_RESET) $<"
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS_DEBUG) -I$(INC_DIR) -c $< -o $@

$(BUILD_DIR)/test_throttle.o: $(TEST_DIR)/test_throttle.c
	@echo "$(COLOR_YELLOW)[COMPILE]$(COLOR_RESET) $<"
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS_DEBUG) -I$(INC_DIR) -c $< -o $@

$(BUILD_DIR)/test_utils.o: $(TEST_DIR)/test_utils.c
	@echo "$(COLOR_YELLOW)[COMPILE]$(COLOR_RESET) $<"
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS_DEBUG) -I$(INC_DIR) -c $< -o $@

$(TEST_EXEC): $(TEST_OBJS)
	@echo "$(COLOR_GREEN)[LINK]$(COLOR_RESET) $@"
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS_DEBUG) -I$(INC_DIR) $^ -lm -o $@
	@echo "$(COLOR_GREEN)[SUCCESS]$(COLOR_RESET) Test executable created: $@"

# ============================================================================
# Run Targets
# ============================================================================

.PHONY: run
run: build
	@echo ""
	@echo "$(COLOR_MAGENTA)╔════════════════════════════════════════════════════════════╗$(COLOR_RESET)"
	@echo "$(COLOR_MAGENTA)║$(COLOR_RESET) Running tests...$(COLOR_MAGENTA)                                                     ║$(COLOR_RESET)"
	@echo "$(COLOR_MAGENTA)╚════════════════════════════════════════════════════════════╝$(COLOR_RESET)"
	@echo ""
	@$(TEST_EXEC)

.PHONY: run-verbose
run-verbose: build
	@echo ""
	@echo "$(COLOR_MAGENTA)╔════════════════════════════════════════════════════════════╗$(COLOR_RESET)"
	@echo "$(COLOR_MAGENTA)║$(COLOR_RESET) Running tests (verbose)...$(COLOR_MAGENTA)                                      ║$(COLOR_RESET)"
	@echo "$(COLOR_MAGENTA)╚════════════════════════════════════════════════════════════╝$(COLOR_RESET)"
	@echo ""
	@$(TEST_EXEC) -v

.PHONY: run-gdb
run-gdb: build
	@echo "$(COLOR_YELLOW)Starting GDB debugger...$(COLOR_RESET)"
	@gdb $(TEST_EXEC)

# ============================================================================
# Utility Targets
# ============================================================================

.PHONY: debug
debug: CFLAGS := $(CFLAGS_DEBUG)
debug: clean build
	@echo "$(COLOR_CYAN)[DEBUG]$(COLOR_RESET) Debug build complete"

.PHONY: release
release: CFLAGS := $(CFLAGS_RELEASE)
release: clean build
	@echo "$(COLOR_GREEN)[RELEASE]$(COLOR_RESET) Release build complete"

.PHONY: clean
clean:
	@echo "$(COLOR_YELLOW)[CLEAN]$(COLOR_RESET) Removing build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "$(COLOR_GREEN)[CLEAN]$(COLOR_RESET) Done"

.PHONY: rebuild
rebuild: clean build
	@echo "$(COLOR_GREEN)[REBUILD]$(COLOR_RESET) Complete rebuild finished"

# ============================================================================
# Help and Info
# ============================================================================

.PHONY: help
help:
	@echo ""
	@echo "$(COLOR_BOLD)VCU Test Framework - Makefile Help$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_CYAN)Available Targets:$(COLOR_RESET)"
	@echo "  $(COLOR_GREEN)make all$(COLOR_RESET)         - Build and run tests (default)"
	@echo "  $(COLOR_GREEN)make build$(COLOR_RESET)       - Build test executable only"
	@echo "  $(COLOR_GREEN)make run$(COLOR_RESET)         - Build and run tests"
	@echo "  $(COLOR_GREEN)make run-verbose$(COLOR_RESET) - Run tests with verbose output"
	@echo "  $(COLOR_GREEN)make run-gdb$(COLOR_RESET)     - Run tests under GDB debugger"
	@echo "  $(COLOR_GREEN)make debug$(COLOR_RESET)       - Build with debug symbols"
	@echo "  $(COLOR_GREEN)make release$(COLOR_RESET)     - Build optimized release version"
	@echo "  $(COLOR_GREEN)make clean$(COLOR_RESET)       - Remove build artifacts"
	@echo "  $(COLOR_GREEN)make rebuild$(COLOR_RESET)     - Clean and rebuild everything"
	@echo "  $(COLOR_GREEN)make help$(COLOR_RESET)        - Show this help message"
	@echo ""
	@echo "$(COLOR_CYAN)Examples:$(COLOR_RESET)"
	@echo "  $(COLOR_WHITE)make run$(COLOR_RESET)          # Run all tests"
	@echo "  $(COLOR_WHITE)make debug$(COLOR_RESET)        # Build with debug symbols and run"
	@echo "  $(COLOR_WHITE)make run-gdb$(COLOR_RESET)      # Debug with GDB"
	@echo "  $(COLOR_WHITE)make clean$(COLOR_RESET)        # Clean up"
	@echo ""

# ============================================================================
# Color definitions for terminal output
# ============================================================================

COLOR_RESET   = \033[0m
COLOR_RED     = \033[31m
COLOR_GREEN   = \033[32m
COLOR_YELLOW  = \033[33m
COLOR_BLUE    = \033[34m
COLOR_MAGENTA = \033[35m
COLOR_CYAN    = \033[36m
COLOR_BOLD    = \033[1m

# ============================================================================
# Compiler checks
# ============================================================================

.PHONY: check-compiler
check-compiler:
	@echo "$(COLOR_CYAN)[CHECK]$(COLOR_RESET) Checking for GCC..."
	@which $(CC) > /dev/null 2>&1 || (echo "$(COLOR_RED)Error: $(CC) not found!$(COLOR_RESET)" && exit 1)
	@echo "$(COLOR_GREEN)[OK]$(COLOR_RESET) $(CC) found: $$($(CC) --version | head -n1)"

# ============================================================================
# Information Targets
# ============================================================================

.PHONY: info
info:
	@echo ""
	@echo "$(COLOR_BOLD)Build Configuration:$(COLOR_RESET)"
	@echo "  Compiler:        $(CC)"
	@echo "  C Flags:         $(CFLAGS)"
	@echo "  Test Directory:  $(TEST_DIR)"
	@echo "  Source Directory: $(SRC_DIR)"
	@echo "  Include Directory: $(INC_DIR)"
	@echo "  Build Directory: $(BUILD_DIR)"
	@echo "  Test Executable: $(TEST_EXEC)"
	@echo ""

.PHONY: list-tests
list-tests:
	@echo ""
	@echo "$(COLOR_BOLD)Test Files:$(COLOR_RESET)"
	@echo "  $(COLOR_CYAN)test_main.c$(COLOR_RESET)      - Main test runner"
	@echo "  $(COLOR_CYAN)test_throttle.c$(COLOR_RESET)  - Throttle module tests"
	@echo "  $(COLOR_CYAN)test_utils.c$(COLOR_RESET)     - Utilities module tests"
	@echo ""
	@echo "$(COLOR_BOLD)Test Suites:$(COLOR_RESET)"
	@echo "  $(COLOR_GREEN)✓ CheckAndLimitRange$(COLOR_RESET)"
	@echo "  $(COLOR_GREEN)✓ NormalizeThrottle$(COLOR_RESET)"
	@echo "  $(COLOR_GREEN)✓ CalcThrottle$(COLOR_RESET)"
	@echo "  $(COLOR_GREEN)✓ change (integer conversion)$(COLOR_RESET)"
	@echo "  $(COLOR_GREEN)✓ changeFloat (float conversion)$(COLOR_RESET)"
	@echo "  $(COLOR_GREEN)✓ Math helpers (ABS, MIN, MAX)$(COLOR_RESET)"
	@echo "  $(COLOR_GREEN)✓ Practical scenarios$(COLOR_RESET)"
	@echo ""

# ============================================================================
# Phony targets
# ============================================================================

.PHONY: clean build run debug release all
