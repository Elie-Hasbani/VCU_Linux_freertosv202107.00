#include <stdio.h>
#include <stdlib.h>
#include "test_framework.h"

/* ============================================================================
   Declare test suite runner functions
   ============================================================================ */

extern void run_throttle_tests(void);
extern void run_utils_tests(void);

/* ============================================================================
   Initialize global test statistics
   ============================================================================ */

TestStats g_test_stats = {
    .total_tests = 0,
    .passed_tests = 0,
    .failed_tests = 0,
    .total_assertions = 0,
    .passed_assertions = 0,
    .failed_assertions = 0};

int g_test_passed = 1;

/* ============================================================================
   Print Header Banner
   ============================================================================ */

static void print_header(void)
{
    printf("\n");
    printf(COLOR_BOLD COLOR_CYAN);
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                                                            ║\n");
    printf("║              VCU TEST FRAMEWORK - TEST SUITE               ║\n");
    printf("║                                                            ║\n");
    printf("║  Testing: throttle.c, utils.c                             ║\n");
    printf("║                                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET "\n");
}

/* ============================================================================
   Main Entry Point
   ============================================================================ */

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    print_header();

    // Run all test suites
    run_throttle_tests();
    run_utils_tests();

    // Print final summary
    printf("\n");
    print_test_summary();

    // Return exit code based on test results
    if (g_test_stats.failed_tests == 0 && g_test_stats.failed_assertions == 0)
    {
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}
