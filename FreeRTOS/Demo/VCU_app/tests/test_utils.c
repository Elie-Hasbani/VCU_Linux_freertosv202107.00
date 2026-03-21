#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "test_framework.h"

/* ============================================================================
   Helper Macros (from my_math.h)
   ============================================================================ */

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* ============================================================================
   Utility Functions to Test (defined as static to avoid linker conflicts)
   ============================================================================ */

static inline int32_t change(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static inline float changeFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* ============================================================================
   Test Suites for Utils Functions
   ============================================================================ */

void test_change_int(void)
{
    TEST_SUITE_START("change (Integer conversion)");

    // Test 1: Simple 0-10 to 0-100 mapping
    {
        TEST_START("Map 0-10 range to 0-100 (min value)");
        int32_t result = change(0, 0, 10, 0, 100);
        ASSERT_EQUAL_INT(result, 0);
        TEST_END;
    }

    // Test 2: Mid-range value
    {
        TEST_START("Map 0-10 range to 0-100 (mid value 5 -> 50)");
        int32_t result = change(5, 0, 10, 0, 100);
        ASSERT_EQUAL_INT(result, 50);
        TEST_END;
    }

    // Test 3: Maximum value
    {
        TEST_START("Map 0-10 range to 0-100 (max value 10 -> 100)");
        int32_t result = change(10, 0, 10, 0, 100);
        ASSERT_EQUAL_INT(result, 100);
        TEST_END;
    }

    // Test 4: Different input range
    {
        TEST_START("Map 100-200 range to 0-1000 (150 -> 500)");
        int32_t result = change(150, 100, 200, 0, 1000);
        ASSERT_EQUAL_INT(result, 500);
        TEST_END;
    }

    // Test 5: Negative output range
    {
        TEST_START("Map 0-100 range to -50 to +50 (50 -> 0)");
        int32_t result = change(50, 0, 100, -50, 50);
        ASSERT_EQUAL_INT(result, 0);
        TEST_END;
    }

    // Test 6: Negative output range (max)
    {
        TEST_START("Map 0-100 range to -100 to 0 (100 -> 0)");
        int32_t result = change(100, 0, 100, -100, 0);
        ASSERT_EQUAL_INT(result, 0);
        TEST_END;
    }

    // Test 7: Negative input range
    {
        TEST_START("Map -100 to 100 range to 0-1000 (0 -> 500)");
        int32_t result = change(0, -100, 100, 0, 1000);
        ASSERT_EQUAL_INT(result, 500);
        TEST_END;
    }

    // Test 8: Identity mapping (same range)
    {
        TEST_START("Identity mapping (0-100 to 0-100)");
        int32_t result = change(75, 0, 100, 0, 100);
        ASSERT_EQUAL_INT(result, 75);
        TEST_END;
    }

    // Test 9: Reverse output range
    {
        TEST_START("Reverse mapping (0-100 to 100-0)");
        int32_t result = change(0, 0, 100, 100, 0);
        ASSERT_EQUAL_INT(result, 100);
        TEST_END;
    }

    // Test 10: Reverse mapping (mid point)
    {
        TEST_START("Reverse mapping at mid point (50 in 0-100 to 50-0)");
        int32_t result = change(50, 0, 100, 50, 0);
        ASSERT_EQUAL_INT(result, 25);
        TEST_END;
    }
}

void test_changeFloat(void)
{
    TEST_SUITE_START("changeFloat (Float conversion)");

    // Test 1: Simple 0.0-1.0 to 0.0-100.0 mapping
    {
        TEST_START("Map 0.0-1.0 to 0.0-100.0 (min value)");
        float result = changeFloat(0.0f, 0.0f, 1.0f, 0.0f, 100.0f);
        ASSERT_EQUAL_FLOAT(result, 0.0f, 0.01f);
        TEST_END;
    }

    // Test 2: Mid-range value
    {
        TEST_START("Map 0.0-1.0 to 0.0-100.0 (mid value 0.5 -> 50.0)");
        float result = changeFloat(0.5f, 0.0f, 1.0f, 0.0f, 100.0f);
        ASSERT_EQUAL_FLOAT(result, 50.0f, 0.01f);
        TEST_END;
    }

    // Test 3: Maximum value
    {
        TEST_START("Map 0.0-1.0 to 0.0-100.0 (max value 1.0 -> 100.0)");
        float result = changeFloat(1.0f, 0.0f, 1.0f, 0.0f, 100.0f);
        ASSERT_EQUAL_FLOAT(result, 100.0f, 0.01f);
        TEST_END;
    }

    // Test 4: Voltage range mapping (common use case)
    {
        TEST_START("Map 0.0-3.3V to 0.0-4095.0 (ADC conversion)");
        float result = changeFloat(1.65f, 0.0f, 3.3f, 0.0f, 4095.0f);
        ASSERT_EQUAL_FLOAT(result, 2047.5f, 1.0f);
        TEST_END;
    }

    // Test 5: Temperature range mapping
    {
        TEST_START("Map -40 to +125°C to 0-255 (sensor conversion)");
        float result = changeFloat(42.5f, -40.0f, 125.0f, 0.0f, 255.0f);
        TEST_INFO_FLOAT("Result", result);
        ASSERT_TRUE(result >= 0.0f && result <= 255.0f);
        TEST_END;
    }

    // Test 6: Negative output range
    {
        TEST_START("Map 0-100 to -50 to +50 (50 -> 0)");
        float result = changeFloat(50.0f, 0.0f, 100.0f, -50.0f, 50.0f);
        ASSERT_EQUAL_FLOAT(result, 0.0f, 0.01f);
        TEST_END;
    }

    // Test 7: Reverse scaling
    {
        TEST_START("Reverse scaling (0-100 to 100-0)");
        float result = changeFloat(25.0f, 0.0f, 100.0f, 100.0f, 0.0f);
        ASSERT_EQUAL_FLOAT(result, 75.0f, 0.01f);
        TEST_END;
    }

    // Test 8: Small precision test
    {
        TEST_START("Precision test (0-1000 to 0-1 at 333.333)");
        float result = changeFloat(333.333f, 0.0f, 1000.0f, 0.0f, 1.0f);
        ASSERT_EQUAL_FLOAT(result, 0.333333f, 0.001f);
        TEST_END;
    }

    // Test 9: Negative input range
    {
        TEST_START("Map -100 to 100 to 0-255 (0 -> 127.5)");
        float result = changeFloat(0.0f, -100.0f, 100.0f, 0.0f, 255.0f);
        ASSERT_EQUAL_FLOAT(result, 127.5f, 0.1f);
        TEST_END;
    }

    // Test 10: Identity mapping with floats
    {
        TEST_START("Identity mapping (0.0-100.0 to 0.0-100.0)");
        float result = changeFloat(42.5f, 0.0f, 100.0f, 0.0f, 100.0f);
        ASSERT_EQUAL_FLOAT(result, 42.5f, 0.01f);
        TEST_END;
    }
}

void test_math_helpers(void)
{
    TEST_SUITE_START("Math Helper Macros");

    // Test ABS macro
    {
        TEST_START("ABS macro - positive number");
        int result = ABS(42);
        ASSERT_EQUAL_INT(result, 42);
        TEST_END;
    }

    {
        TEST_START("ABS macro - negative number");
        int result = ABS(-42);
        ASSERT_EQUAL_INT(result, 42);
        TEST_END;
    }

    // Test MIN macro
    {
        TEST_START("MIN macro - first smaller");
        int result = MIN(10, 20);
        ASSERT_EQUAL_INT(result, 10);
        TEST_END;
    }

    {
        TEST_START("MIN macro - second smaller");
        int result = MIN(20, 10);
        ASSERT_EQUAL_INT(result, 10);
        TEST_END;
    }

    // Test MAX macro
    {
        TEST_START("MAX macro - first larger");
        int result = MAX(10, 20);
        ASSERT_EQUAL_INT(result, 20);
        TEST_END;
    }

    {
        TEST_START("MAX macro - second larger");
        int result = MAX(20, 10);
        ASSERT_EQUAL_INT(result, 20);
        TEST_END;
    }
}

void test_practical_scenarios(void)
{
    TEST_SUITE_START("Practical Scenarios");

    // Test 1: Throttle pedal mapping (0-1000 ADC to 0-100%)
    {
        TEST_START("Throttle pedal: ADC 0-1000 to 0-100%");
        float throttle_pct = changeFloat(512.0f, 0.0f, 1000.0f, 0.0f, 100.0f);
        TEST_INFO_FLOAT("ADC 512 maps to", throttle_pct);
        ASSERT_EQUAL_FLOAT(throttle_pct, 51.2f, 0.1f);
        TEST_END;
    }

    // Test 2: Brake pressure mapping
    {
        TEST_START("Brake pressure: 0-5000 kPa to 0-255 PWM");
        int32_t pwm = change(2500, 0, 5000, 0, 255);
        TEST_INFO_INT("2500 kPa maps to PWM", pwm);
        ASSERT_EQUAL_INT(pwm, 127);
        TEST_END;
    }

    // Test 3: Motor RPM to voltage scaling
    {
        TEST_START("Motor RPM: 0-15000 RPM to 0-3.3V output");
        float voltage = changeFloat(7500.0f, 0.0f, 15000.0f, 0.0f, 3.3f);
        ASSERT_EQUAL_FLOAT(voltage, 1.65f, 0.01f);
        TEST_END;
    }

    // Test 4: Battery voltage monitoring
    {
        TEST_START("Battery voltage: Input 0-48V to 0-4095 ADC counts");
        int32_t adc_count = change(24, 0, 48, 0, 4095);
        ASSERT_EQUAL_INT(adc_count, 2047);
        TEST_END;
    }

    // Test 5: Current limiting
    {
        TEST_START("Current limiting: 0-200A to 0-100% torque limit");
        float torque_limit = changeFloat(100.0f, 0.0f, 200.0f, 0.0f, 100.0f);
        ASSERT_EQUAL_FLOAT(torque_limit, 50.0f, 0.1f);
        TEST_END;
    }
}

/* ============================================================================
   Main Test Runner
   ============================================================================ */

void run_utils_tests(void)
{
    printf("\n");
    printf(COLOR_BOLD COLOR_MAGENTA "╔════════════════════════════════════════════════════════════╗" COLOR_RESET "\n");
    printf(COLOR_BOLD COLOR_MAGENTA "║" COLOR_RESET "  " COLOR_BOLD "UTILITIES MODULE TESTS" COLOR_RESET "\n");
    printf(COLOR_BOLD COLOR_MAGENTA "╚════════════════════════════════════════════════════════════╝" COLOR_RESET "\n");

    test_change_int();
    test_changeFloat();
    test_math_helpers();
    test_practical_scenarios();
}
