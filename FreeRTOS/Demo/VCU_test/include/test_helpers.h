#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Mock TickType_t must be defined before including structs.h
typedef uint32_t TickType_t;

// Include structs from main project
#include "../../VCU_app/include/structs.h"

// Test result tracking
typedef struct
{
    int passed;
    int failed;
    int total;
} TestResults_t;

// Colors for test output
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_RESET "\033[0m"

// Test macros
#define TEST_ASSERT(condition, message)                                \
    do                                                                 \
    {                                                                  \
        tests->total++;                                                \
        if (condition)                                                 \
        {                                                              \
            tests->passed++;                                           \
            printf("  " COLOR_GREEN "✓" COLOR_RESET " %s\n", message); \
        }                                                              \
        else                                                           \
        {                                                              \
            tests->failed++;                                           \
            printf("  " COLOR_RED "✗" COLOR_RESET " %s\n", message);   \
        }                                                              \
    } while (0)

#define TEST_ASSERT_EQUAL(expected, actual, message)                               \
    do                                                                             \
    {                                                                              \
        tests->total++;                                                            \
        if ((expected) == (actual))                                                \
        {                                                                          \
            tests->passed++;                                                       \
            printf("  " COLOR_GREEN "✓" COLOR_RESET " %s\n", message);             \
        }                                                                          \
        else                                                                       \
        {                                                                          \
            tests->failed++;                                                       \
            printf("  " COLOR_RED "✗" COLOR_RESET " %s (expected: %d, got: %d)\n", \
                   message, (int)(expected), (int)(actual));                       \
        }                                                                          \
    } while (0)

#define TEST_ASSERT_FLOAT_EQUAL(expected, actual, tolerance, message)                              \
    do                                                                                             \
    {                                                                                              \
        tests->total++;                                                                            \
        float diff = (expected) - (actual);                                                        \
        if (diff < 0)                                                                              \
            diff = -diff;                                                                          \
        if (diff <= (tolerance))                                                                   \
        {                                                                                          \
            tests->passed++;                                                                       \
            printf("  " COLOR_GREEN "✓" COLOR_RESET " %s\n", message);                             \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            tests->failed++;                                                                       \
            printf("  " COLOR_RED "✗" COLOR_RESET " %s (expected: %.2f, got: %.2f, diff: %.2f)\n", \
                   message, (expected), (actual), diff);                                           \
        }                                                                                          \
    } while (0)

#define TEST_SECTION(name) \
    printf("\n" COLOR_BLUE "▶ %s" COLOR_RESET "\n", name)

#define TEST_SUMMARY(results)                                                     \
    do                                                                            \
    {                                                                             \
        printf("\n" COLOR_YELLOW "═══════════════════════════════════════\n");    \
        printf("SUMMARY:\n");                                                     \
        printf("  Total tests: %d\n", (results).total);                           \
        printf("  " COLOR_GREEN "Passed: %d" COLOR_RESET "\n", (results).passed); \
        printf("  " COLOR_RED "Failed: %d" COLOR_RESET "\n", (results).failed);   \
        printf("═══════════════════════════════════════" COLOR_RESET "\n");       \
    } while (0)

// Helper functions to create dummy structures
MotorControlState_t create_dummy_motor_state(void);
GlobalState_t create_dummy_global_state(void);

// Helper functions to set specific values
void set_apps_values(MotorControlState_t *state, int val1, int val2, TickType_t time1, TickType_t time2);
void set_wheel_speeds(MotorControlState_t *state, int w1, int w2, int w3, int w4, TickType_t time);
void set_brake_pedal(MotorControlState_t *state, bool pressed, TickType_t time);

// Initialize test results
void init_test_results(TestResults_t *results);

#endif // TEST_HELPERS_H
