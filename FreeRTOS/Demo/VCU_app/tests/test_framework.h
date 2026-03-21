#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

/* ============================================================================
   ANSI Color Codes for terminal output
   ============================================================================ */
#define COLOR_RESET "\x1b[0m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_WHITE "\x1b[37m"
#define COLOR_BOLD "\x1b[1m"

#define SPACE "   "

/* ============================================================================
   Global Test Statistics
   ============================================================================ */
typedef struct
{
    int total_tests;
    int passed_tests;
    int failed_tests;
    int total_assertions;
    int passed_assertions;
    int failed_assertions;
} TestStats;

extern TestStats g_test_stats;
extern int g_test_passed;

/* ============================================================================
   Test Macros
   ============================================================================ */

#define ASSERT_EQUAL_INT(actual, expected)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        g_test_stats.total_assertions++;                                                                               \
        if ((actual) == (expected))                                                                                    \
        {                                                                                                              \
            g_test_stats.passed_assertions++;                                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            g_test_stats.failed_assertions++;                                                                          \
            g_test_passed = 0;                                                                                         \
            printf(COLOR_RED "    ✗ FAILED: " COLOR_RESET "Expected %d but got %d\n", (int)(expected), (int)(actual)); \
        }                                                                                                              \
    } while (0)

#define ASSERT_EQUAL_FLOAT(actual, expected, tolerance)                                                       \
    do                                                                                                        \
    {                                                                                                         \
        g_test_stats.total_assertions++;                                                                      \
        float diff = fabsf((actual) - (expected));                                                            \
        if (diff <= (tolerance))                                                                              \
        {                                                                                                     \
            g_test_stats.passed_assertions++;                                                                 \
        }                                                                                                     \
        else                                                                                                  \
        {                                                                                                     \
            g_test_stats.failed_assertions++;                                                                 \
            g_test_passed = 0;                                                                                \
            printf(COLOR_RED "    ✗ FAILED: " COLOR_RESET "Expected %f but got %f (diff=%f, tolerance=%f)\n", \
                   (float)(expected), (float)(actual), diff, (float)(tolerance));                             \
        }                                                                                                     \
    } while (0)

#define ASSERT_TRUE(condition)                                                              \
    do                                                                                      \
    {                                                                                       \
        g_test_stats.total_assertions++;                                                    \
        if ((condition))                                                                    \
        {                                                                                   \
            g_test_stats.passed_assertions++;                                               \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            g_test_stats.failed_assertions++;                                               \
            g_test_passed = 0;                                                              \
            printf(COLOR_RED "    ✗ FAILED: " COLOR_RESET "Expected true but got false\n"); \
        }                                                                                   \
    } while (0)

#define ASSERT_FALSE(condition)                                                             \
    do                                                                                      \
    {                                                                                       \
        g_test_stats.total_assertions++;                                                    \
        if (!(condition))                                                                   \
        {                                                                                   \
            g_test_stats.passed_assertions++;                                               \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            g_test_stats.failed_assertions++;                                               \
            g_test_passed = 0;                                                              \
            printf(COLOR_RED "    ✗ FAILED: " COLOR_RESET "Expected false but got true\n"); \
        }                                                                                   \
    } while (0)

#define ASSERT_IN_RANGE(value, min, max)                                                                   \
    do                                                                                                     \
    {                                                                                                      \
        g_test_stats.total_assertions++;                                                                   \
        if ((value) >= (min) && (value) <= (max))                                                          \
        {                                                                                                  \
            g_test_stats.passed_assertions++;                                                              \
        }                                                                                                  \
        else                                                                                               \
        {                                                                                                  \
            g_test_stats.failed_assertions++;                                                              \
            g_test_passed = 0;                                                                             \
            printf(COLOR_RED "    ✗ FAILED: " COLOR_RESET "Expected value in range [%f, %f] but got %f\n", \
                   (float)(min), (float)(max), (float)(value));                                            \
        }                                                                                                  \
    } while (0)

/* ============================================================================
   Test Suite Macros
   ============================================================================ */

#define TEST_SUITE_START(suite_name)                                                                                      \
    printf("\n" COLOR_BOLD COLOR_BLUE "╔════════════════════════════════════════════════════════════╗" COLOR_RESET "\n"); \
    printf(COLOR_BOLD COLOR_BLUE "║" COLOR_RESET " Test Suite: " COLOR_CYAN "%s" COLOR_RESET "\n", suite_name);           \
    printf(COLOR_BOLD COLOR_BLUE "╚════════════════════════════════════════════════════════════╝" COLOR_RESET "\n")

#define TEST_START(test_name)                                               \
    do                                                                      \
    {                                                                       \
        g_test_stats.total_tests++;                                         \
        g_test_passed = 1;                                                  \
        printf("\n" SPACE COLOR_YELLOW "▶ " COLOR_RESET "%s\n", test_name); \
    } while (0)

#define TEST_END                                                        \
    do                                                                  \
    {                                                                   \
        if (g_test_passed)                                              \
        {                                                               \
            g_test_stats.passed_tests++;                                \
            printf(SPACE COLOR_GREEN "✓ Test PASSED" COLOR_RESET "\n"); \
        }                                                               \
        else                                                            \
        {                                                               \
            g_test_stats.failed_tests++;                                \
            printf(SPACE COLOR_RED "✗ Test FAILED" COLOR_RESET "\n");   \
        }                                                               \
    } while (0)

#define TEST_INFO(msg) \
    printf(SPACE COLOR_CYAN "ℹ " COLOR_RESET "%s\n", msg)

#define TEST_INFO_INT(label, value) \
    printf(SPACE COLOR_CYAN "ℹ " COLOR_RESET "%s: %d\n", label, (int)(value))

#define TEST_INFO_FLOAT(label, value) \
    printf(SPACE COLOR_CYAN "ℹ " COLOR_RESET "%s: %f\n", label, (float)(value))

/* ============================================================================
   Final Report Macro
   ============================================================================ */

static inline void print_test_summary(void)
{
    printf("\n" COLOR_BOLD COLOR_MAGENTA "╔════════════════════════════════════════════════════════════╗" COLOR_RESET "\n");
    printf(COLOR_BOLD COLOR_MAGENTA "║" COLOR_RESET " " COLOR_BOLD "TEST SUMMARY" COLOR_RESET "\n");
    printf(COLOR_BOLD COLOR_MAGENTA "╚════════════════════════════════════════════════════════════╝" COLOR_RESET "\n");
    printf("\n" COLOR_BOLD "  Tests:" COLOR_RESET "\n");
    printf(SPACE "  Total:  %d\n", g_test_stats.total_tests);
    printf(SPACE COLOR_GREEN "  Passed: %d" COLOR_RESET "\n", g_test_stats.passed_tests);
    printf(SPACE COLOR_RED "  Failed: %d" COLOR_RESET "\n", g_test_stats.failed_tests);

    printf("\n" COLOR_BOLD "  Assertions:" COLOR_RESET "\n");
    printf(SPACE "  Total:  %d\n", g_test_stats.total_assertions);
    printf(SPACE COLOR_GREEN "  Passed: %d" COLOR_RESET "\n", g_test_stats.passed_assertions);
    printf(SPACE COLOR_RED "  Failed: %d" COLOR_RESET "\n", g_test_stats.failed_assertions);

    if (g_test_stats.failed_tests == 0)
    {
        printf("\n" COLOR_BOLD COLOR_GREEN "  ✓ ALL TESTS PASSED!" COLOR_RESET "\n\n");
    }
    else
    {
        printf("\n" COLOR_BOLD COLOR_RED "  ✗ SOME TESTS FAILED!" COLOR_RESET "\n\n");
    }
}

#endif // TEST_FRAMEWORK_H
