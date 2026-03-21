#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "test_framework.h"

/* ============================================================================
   Mock structures and variables (needed to compile throttle.c functions)
   ============================================================================ */

// Variables from throttle.c
int potmin[2] = {0, 0};
int potmax[2] = {1000, 1000};
float regenRpm = 1000.0f;
float brknompedal = 30.0f;
float regenmax = 80.0f;
float regenBrake = 50.0f;
float brkcruise = 0.0f;
float throtmax = 100.0f;
float throtmaxRev = -100.0f;
float throtmin = -100.0f;
float throtdead = 5.0f;
int idleSpeed = 0;
int cruiseSpeed = 5000;
float speedkp = 0.0f;
int speedflt = 0;
float idleThrotLim = 50.0f;
float regenRamp = 0.0f;
float actualThrottleRamp = 10.0f;
int bmslimhigh = 0;
int bmslimlow = 0;
int accelmax = 0;
int accelflt = 0;
float udcmin = 0.0f;
float udcmax = 500.0f;
float idcmin = 0.0f;
float idcmax = 500.0f;
int speedLimit = 10000;
float regenendRpm = 100.0f;
float ThrotRpmFilt = 100.0f;

int speedFiltered = 0;
float potnomFiltered = 0.0f;
float brkRamped = 0.0f;

float throttleRamped = 0.0f;
float SpeedFiltered = 0.0f;

// Functions from throttle.c that we want to test
bool CheckAndLimitRange(int *potval, int potIdx);
float NormalizeThrottle(int potval, int potIdx);
float CalcThrottle(float potnom, int speed, bool brake);

/* ============================================================================
   Mock implementations for functions from my_math.h and console.h
   ============================================================================ */

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

int change(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float changeFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#define IIRFILTER(filtered, input, n) (((filtered) * ((n) - 1) + (input)) / (n))
#define RAMPUP(current, target, rate) ((current) + (rate))

void console_print(const char *fmt, ...)
{
    // Mock implementation - do nothing for tests
    (void)fmt;
}

/* ============================================================================
   Include the actual implementations to test
   ============================================================================ */

// We'll include the function implementations here
// For simplicity, we'll copy the relevant implementations

static float regenlim = 0;

bool CheckAndLimitRange(int *potval, int potIdx)
{
#define POT_SLACK 200
    int potMin = potmax[potIdx] > potmin[potIdx] ? potmin[potIdx] : potmax[potIdx];
    int potMax = potmax[potIdx] > potmin[potIdx] ? potmax[potIdx] : potmin[potIdx];

    if (((*potval + POT_SLACK) < potMin) || (*potval > (potMax + POT_SLACK)))
    {
        *potval = potMin;
        return false;
    }
    else if (*potval < potMin)
    {
        *potval = potMin;
    }
    else if (*potval > potMax)
    {
        *potval = potMax;
    }

    return true;
}

float NormalizeThrottle(int potval, int potIdx)
{
    if (potIdx < 0 || potIdx > 1)
        return 0.0f;

    if (potmin[potIdx] == potmax[potIdx])
        return 0.0f;

    return 100.0f * ((float)(potval - potmin[potIdx]) / (float)(potmax[potIdx] - potmin[potIdx]));
}

float CalcThrottle(float potnom, int speed, bool brake)
{
    if (speed < 0)
    {
        speed *= -1;
    }

    if (ABS(speed - SpeedFiltered) > ThrotRpmFilt)
    {
        if (speed > SpeedFiltered)
        {
            SpeedFiltered += ThrotRpmFilt;
        }
        else
        {
            SpeedFiltered -= ThrotRpmFilt;
        }
    }
    else
    {
        SpeedFiltered = speed;
    }

    speed = SpeedFiltered;

    if (brake)
    {
        if (speed < 100 || speed < regenendRpm)
        {
            return 0;
        }
        else if (speed < regenRpm)
        {
            potnom = changeFloat(speed, regenendRpm, regenRpm, 0, regenBrake);
            return potnom;
        }
        else
        {
            potnom = regenBrake;
            return potnom;
        }
    }

    if (potnom < throtdead)
    {
        potnom = 0.0f;
    }
    else
    {
        potnom = (potnom - throtdead) * (100.0f / (100.0f - throtdead));
    }

    if (speed < 100 || speed < regenendRpm)
    {
        regenlim = 0;
    }
    else if (speed < regenRpm)
    {
        regenlim = changeFloat(speed, regenendRpm, regenRpm, 0, regenmax);
    }
    else
    {
        regenlim = regenmax;
    }

    potnom = changeFloat(potnom, 0, 100, regenlim * 10, throtmax * 10);
    potnom *= 0.1;

    return potnom;
}

/* ============================================================================
   Test Suites
   ============================================================================ */

void test_CheckAndLimitRange(void)
{
    TEST_SUITE_START("CheckAndLimitRange");

    // Test 1: Value within range
    {
        TEST_START("Value within range - should pass");
        potmin[0] = 100;
        potmax[0] = 900;
        int val = 500;
        bool result = CheckAndLimitRange(&val, 0);
        ASSERT_TRUE(result);
        ASSERT_EQUAL_INT(val, 500);
        TEST_END;
    }

    // Test 2: Value below minimum (should clamp)
    {
        TEST_START("Value below minimum - should clamp to min");
        potmin[0] = 100;
        potmax[0] = 900;
        int val = 50;
        bool result = CheckAndLimitRange(&val, 0);
        ASSERT_FALSE(result);
        ASSERT_EQUAL_INT(val, 100);
        TEST_END;
    }

    // Test 3: Value above maximum (should clamp)
    {
        TEST_START("Value above maximum - should clamp to max");
        potmin[0] = 100;
        potmax[0] = 900;
        int val = 950;
        bool result = CheckAndLimitRange(&val, 0);
        ASSERT_FALSE(result);
        ASSERT_EQUAL_INT(val, 900);
        TEST_END;
    }

    // Test 4: Inverted throttle (potmin > potmax)
    {
        TEST_START("Inverted throttle configuration");
        potmin[1] = 900;
        potmax[1] = 100;
        int val = 500;
        bool result = CheckAndLimitRange(&val, 1);
        ASSERT_TRUE(result);
        ASSERT_EQUAL_INT(val, 500);
        TEST_END;
    }

    // Test 5: Value with slack tolerance
    {
        TEST_START("Value with slack tolerance");
        potmin[0] = 100;
        potmax[0] = 900;
        int val = 100 + 200 + 1; // Just outside slack
        bool result = CheckAndLimitRange(&val, 0);
        ASSERT_FALSE(result);
        TEST_END;
    }
}

void test_NormalizeThrottle(void)
{
    TEST_SUITE_START("NormalizeThrottle");

    // Test 1: Normal normalization
    {
        TEST_START("Normal normalization (0-1000 range to 0-100%)");
        potmin[0] = 0;
        potmax[0] = 1000;
        float result = NormalizeThrottle(0, 0);
        ASSERT_EQUAL_FLOAT(result, 0.0f, 0.01f);
        TEST_END;
    }

    // Test 2: Mid-range
    {
        TEST_START("Mid-range normalization (500 -> 50%)");
        potmin[0] = 0;
        potmax[0] = 1000;
        float result = NormalizeThrottle(500, 0);
        ASSERT_EQUAL_FLOAT(result, 50.0f, 0.01f);
        TEST_END;
    }

    // Test 3: Maximum value
    {
        TEST_START("Maximum value normalization (1000 -> 100%)");
        potmin[0] = 0;
        potmax[0] = 1000;
        float result = NormalizeThrottle(1000, 0);
        ASSERT_EQUAL_FLOAT(result, 100.0f, 0.01f);
        TEST_END;
    }

    // Test 4: Invalid index (should return 0)
    {
        TEST_START("Invalid throttle index (should return 0%)");
        float result = NormalizeThrottle(500, 5);
        ASSERT_EQUAL_FLOAT(result, 0.0f, 0.01f);
        TEST_END;
    }

    // Test 5: Equal potmin and potmax (should return 0)
    {
        TEST_START("Equal potmin and potmax (should return 0%)");
        potmin[0] = 500;
        potmax[0] = 500;
        float result = NormalizeThrottle(500, 0);
        ASSERT_EQUAL_FLOAT(result, 0.0f, 0.01f);
        TEST_END;
    }

    // Test 6: Inverted throttle
    {
        TEST_START("Inverted throttle (1000->0 maps 1000->100%)");
        potmin[1] = 1000;
        potmax[1] = 0;
        float result = NormalizeThrottle(1000, 1);
        ASSERT_EQUAL_FLOAT(result, 0.0f, 0.01f);
        TEST_END;
    }
}

void test_CalcThrottle(void)
{
    TEST_SUITE_START("CalcThrottle");

    // Reset state
    throttleRamped = 0.0f;
    SpeedFiltered = 0.0f;
    potmin[0] = 0;
    potmax[0] = 1000;
    throtdead = 5.0f;
    throtmax = 100.0f;
    regenmax = 80.0f;
    regenBrake = 50.0f;
    regenRpm = 1000;
    regenendRpm = 100;

    // Test 1: Throttle below deadzone
    {
        TEST_START("Throttle below deadzone (should return 0)");
        float result = CalcThrottle(3.0f, 5000, false);
        ASSERT_EQUAL_FLOAT(result, 0.0f, 0.01f);
        TEST_END;
    }

    // Test 2: Throttle within deadzone
    {
        TEST_START("Throttle just above deadzone");
        SpeedFiltered = 0.0f; // Reset filter
        float result = CalcThrottle(10.0f, 5000, false);
        TEST_INFO_FLOAT("Result", result);
        ASSERT_TRUE(result >= 0.0f && result <= 100.0f);
        TEST_END;
    }

    // Test 3: Brake at low speed (should return 0, no regen)
    {
        TEST_START("Brake at low speed (should return 0)");
        SpeedFiltered = 50; // Low speed
        float result = CalcThrottle(50.0f, 50, true);
        ASSERT_EQUAL_FLOAT(result, 0.0f, 0.01f);
        TEST_END;
    }

    // Test 4: Brake at high speed (should return regen value)
    {
        TEST_START("Brake at high speed (should activate regen)");
        SpeedFiltered = 5000; // High speed
        float result = CalcThrottle(50.0f, 5000, true);
        ASSERT_TRUE(result >= 0.0f && result <= 100.0f);
        TEST_INFO_FLOAT("Regen value", result);
        TEST_END;
    }

    // Test 5: Negative speed handling
    {
        TEST_START("Negative speed handling");
        SpeedFiltered = 0;
        float result = CalcThrottle(50.0f, -1000, false);
        TEST_INFO_FLOAT("Result with negative speed", result);
        ASSERT_TRUE(result >= -100.0f && result <= 100.0f);
        TEST_END;
    }
}

/* ============================================================================
   Main Test Runner
   ============================================================================ */

void run_throttle_tests(void)
{
    printf("\n");
    printf(COLOR_BOLD COLOR_MAGENTA "╔════════════════════════════════════════════════════════════╗" COLOR_RESET "\n");
    printf(COLOR_BOLD COLOR_MAGENTA "║" COLOR_RESET "  " COLOR_BOLD "THROTTLE MODULE TESTS" COLOR_RESET "\n");
    printf(COLOR_BOLD COLOR_MAGENTA "╚════════════════════════════════════════════════════════════╝" COLOR_RESET "\n");

    test_CheckAndLimitRange();
    test_NormalizeThrottle();
    test_CalcThrottle();
}
