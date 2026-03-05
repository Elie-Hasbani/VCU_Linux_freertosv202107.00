#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../VCU_app/include/project_config.h" // Use project config
#include "test_helpers.h"

// Include utils functions from main project
#include "../../VCU_app/include/utils.h"
#include "../../VCU_app/include/throttle.h"
#include "../../VCU_app/include/my_math.h"

// External variables needed for utils tests
extern int potmin[2];
extern int potmax[2];
extern float regenRpm;
extern float brknompedal;
extern float regenmax;
extern float regenBrake;
extern float brkcruise;
extern float throtmax;
extern float throtmaxRev;
extern float throtmin;
extern float throtdead;
extern int idleSpeed;
extern int cruiseSpeed;
extern float speedkp;
extern int speedflt;
extern float idleThrotLim;
extern float regenRamp;
extern float actualThrottleRamp;
extern int bmslimhigh;
extern int bmslimlow;
extern int accelmax;
extern int accelflt;
extern float udcmin;
extern float udcmax;
extern float idcmin;
extern float idcmax;
extern int speedLimit;
extern float regenendRpm;
extern float ThrotRpmFilt;
extern int throtRampRpm;

/**
 * @brief Initialize parameters for utils tests
 */
void init_utils_params(void)
{
    // Initialize potentiometer ranges
    potmin[0] = 1000;
    potmax[0] = 4000;
    potmin[1] = 1000;
    potmax[1] = 4000;

    // Regen parameters
    regenRpm = 1000.0f;
    regenendRpm = 100.0f;
    regenmax = -30.0f;
    regenBrake = -40.0f;
    regenRamp = 5.0f;

    // Throttle parameters
    throtmax = 100.0f;
    throtmaxRev = -50.0f;
    throtmin = -50.0f;
    throtdead = 5.0f;

    // Speed control
    idleSpeed = 500;
    cruiseSpeed = 2000;
    speedLimit = 5000;
    speedkp = 0.5f;
    speedflt = 4;

    // Current/voltage limits
    udcmin = 250.0f;
    udcmax = 450.0f;
    idcmin = -100.0f;
    idcmax = 200.0f;

    // Acceleration parameters
    accelmax = 100;
    accelflt = 4;
    bmslimhigh = 80;
    bmslimlow = 20;

    // Ramping parameters
    throtRampRpm = 2000;
    actualThrottleRamp = 10.0f;
    idleThrotLim = 20.0f;
    ThrotRpmFilt = 100.0f;
}

/**
 * @brief Test GetUserThrottleCommand with both throttles valid and matching
 */
void test_GetUserThrottleCommand_BothValid(TestResults_t *tests)
{
    TEST_SECTION("Testing GetUserThrottleCommand - Both Throttles Valid");

    init_utils_params();
    MotorControlState_t motorState = create_dummy_motor_state();

    // Test 1: Both throttles valid and matching
    motorState.direction = 1; // Forward
    motorState.brakePedalPressed = false;
    motorState.speed = 1000;
    set_apps_values(&motorState, 2500, 2500, 0, 0);

    float result = GetUserThrottleCommand(&motorState);
    TEST_ASSERT(result > 0, "Both valid matching throttles should give positive output");

    // Test 2: Both valid but slightly different (within 10%)
    set_apps_values(&motorState, 2500, 2600, 0, 0); // ~3.3% difference
    result = GetUserThrottleCommand(&motorState);
    TEST_ASSERT(result > 0, "Small difference (<10%) should give positive output");

    // Test 3: Neutral direction should return 0
    motorState.direction = 0;
    set_apps_values(&motorState, 2500, 2500, 0, 0);
    result = GetUserThrottleCommand(&motorState);
    TEST_ASSERT_FLOAT_EQUAL(0.0f, result, 0.1f, "Neutral direction should return 0");

    // Test 4: Park mode should return 0
    motorState.direction = 2;
    set_apps_values(&motorState, 2500, 2500, 0, 0);
    result = GetUserThrottleCommand(&motorState);
    TEST_ASSERT_FLOAT_EQUAL(0.0f, result, 0.1f, "Park mode should return 0");
}

/**
 * @brief Test GetUserThrottleCommand with throttle mismatch (limp mode)
 */
void test_GetUserThrottleCommand_Mismatch(TestResults_t *tests)
{
    TEST_SECTION("Testing GetUserThrottleCommand - Throttle Mismatch (Limp Mode)");

    init_utils_params();
    MotorControlState_t motorState = create_dummy_motor_state();

    // Test 1: Large difference (>10%) - should enter limp mode
    motorState.direction = 1; // Forward
    motorState.brakePedalPressed = false;
    motorState.speed = 1000;
    set_apps_values(&motorState, 1500, 3000, 0, 0); // ~50% difference

    float result = GetUserThrottleCommand(&motorState);
    TEST_ASSERT(result >= 0, "Limp mode should still give non-negative output");
    // In limp mode, it should use the lower value and cap at 50%
    TEST_ASSERT(result <= 50.0f, "Limp mode should limit to 50% of max");

    // Test 2: Mismatch with both high values
    set_apps_values(&motorState, 3500, 2000, 0, 0);
    result = GetUserThrottleCommand(&motorState);
    TEST_ASSERT(result >= 0, "Limp mode should work with reversed mismatch");
}

/**
 * @brief Test GetUserThrottleCommand with one throttle out of range
 */
void test_GetUserThrottleCommand_OneOutOfRange(TestResults_t *tests)
{
    TEST_SECTION("Testing GetUserThrottleCommand - One Throttle Out of Range");

    init_utils_params();
    MotorControlState_t motorState = create_dummy_motor_state();

    // Test 1: Throttle 1 valid, Throttle 2 out of range
    motorState.direction = 1;
    motorState.brakePedalPressed = false;
    motorState.speed = 1000;
    set_apps_values(&motorState, 2500, 500, 0, 0); // 500 is way out of range

    float result = GetUserThrottleCommand(&motorState);
    TEST_ASSERT(result >= 0, "Should use valid throttle when other is out of range");

    // Test 2: Throttle 1 out of range, Throttle 2 valid
    set_apps_values(&motorState, 500, 2500, 0, 0); // 500 is way out of range
    result = GetUserThrottleCommand(&motorState);
    TEST_ASSERT(result >= 0, "Should use valid throttle when other is out of range");

    // Test 3: Both throttles out of range
    set_apps_values(&motorState, 100, 200, 0, 0); // Both way out of range
    result = GetUserThrottleCommand(&motorState);
    TEST_ASSERT_FLOAT_EQUAL(0.0f, result, 0.1f, "Both out of range should return 0");
}

/**
 * @brief Test calculateSpeed function
 */
void test_calculateSpeed(TestResults_t *tests)
{
    TEST_SECTION("Testing calculateSpeed");

    MotorControlState_t motorState = create_dummy_motor_state();

    // Test 1: All wheels same speed
    set_wheel_speeds(&motorState, 1000, 1000, 1000, 1000, 0);
    int speed = calculateSpeed(&motorState);
    TEST_ASSERT_EQUAL(1000, speed, "All wheels at same speed should give that speed");

    // Test 2: Different wheel speeds (average)
    set_wheel_speeds(&motorState, 1000, 1100, 900, 1000, 0);
    speed = calculateSpeed(&motorState);
    TEST_ASSERT_EQUAL(1000, speed, "Should calculate average of wheel speeds");

    // Test 3: Zero speed
    set_wheel_speeds(&motorState, 0, 0, 0, 0, 0);
    speed = calculateSpeed(&motorState);
    TEST_ASSERT_EQUAL(0, speed, "All zeros should give zero speed");

    // Test 4: Mixed speeds
    set_wheel_speeds(&motorState, 500, 600, 700, 800, 0);
    speed = calculateSpeed(&motorState);
    int expected = (500 + 600 + 700 + 800) / 4;
    TEST_ASSERT_EQUAL(expected, speed, "Should calculate correct average");

    // Test 5: High speeds
    set_wheel_speeds(&motorState, 5000, 5100, 4900, 5000, 0);
    speed = calculateSpeed(&motorState);
    expected = (5000 + 5100 + 4900 + 5000) / 4;
    TEST_ASSERT_EQUAL(expected, speed, "Should work with high speeds");
}

/**
 * @brief Test checkMessageTimeStamps - APPS timeout
 */
void test_checkMessageTimeStamps_APPS(TestResults_t *tests)
{
    TEST_SECTION("Testing checkMessageTimeStamps - APPS Messages");

    init_utils_params();
    MotorControlState_t motorState = create_dummy_motor_state();
    GlobalState_t globalState = create_dummy_global_state();

    // Test 1: Both APPS messages fresh
    set_apps_values(&motorState, 2500, 2500, 0, 0);
    set_wheel_speeds(&motorState, 1000, 1000, 1000, 1000, 0);
    set_brake_pedal(&motorState, false, 0);

    float limit = checkMessageTimeStamps(&motorState, &globalState, 50); // 50ms later
    TEST_ASSERT(limit >= 50, "Fresh APPS messages should not limit severely");
    TEST_ASSERT((globalState.derateReason & DERATE_APPS1_OUTDATED) == 0,
                "No APPS1 timeout flag");
    TEST_ASSERT((globalState.derateReason & DERATE_APPS2_OUTDATED) == 0,
                "No APPS2 timeout flag");

    // Test 2: APPS1 timeout (>200ms)
    globalState.derateReason = 0; // Reset
    set_apps_values(&motorState, 2500, 2500, 0, 0);
    limit = checkMessageTimeStamps(&motorState, &globalState, 250); // 250ms later
    TEST_ASSERT((globalState.derateReason & DERATE_APPS1_OUTDATED) != 0,
                "APPS1 timeout should set flag");

    // Test 3: APPS2 timeout
    globalState.derateReason = 0; // Reset
    set_apps_values(&motorState, 2500, 2500, 0, 0);
    motorState.appsValues[0].timestamp = 50;                        // Update APPS1 timestamp
    limit = checkMessageTimeStamps(&motorState, &globalState, 250); // 250ms later
    TEST_ASSERT((globalState.derateReason & DERATE_APPS2_OUTDATED) != 0,
                "APPS2 timeout should set flag");

    // Test 4: Both APPS timeout (complete derate)
    globalState.derateReason = 0; // Reset
    set_apps_values(&motorState, 2500, 2500, 0, 0);
    limit = checkMessageTimeStamps(&motorState, &globalState, 500); // 500ms later
    // Both APPS timed out - should severely limit throttle
    TEST_ASSERT((globalState.derateReason & DERATE_APPS1_OUTDATED) != 0,
                "Both APPS timeout should set both flags");
    TEST_ASSERT((globalState.derateReason & DERATE_APPS2_OUTDATED) != 0,
                "Both APPS timeout should set both flags");
}

/**
 * @brief Test checkMessageTimeStamps - Wheel speed timeout
 */
void test_checkMessageTimeStamps_Wheels(TestResults_t *tests)
{
    TEST_SECTION("Testing checkMessageTimeStamps - Wheel Speed Messages");

    init_utils_params();
    MotorControlState_t motorState = create_dummy_motor_state();
    GlobalState_t globalState = create_dummy_global_state();

    // Test 1: All wheels fresh
    set_apps_values(&motorState, 2500, 2500, 50, 50);
    set_wheel_speeds(&motorState, 1000, 1000, 1000, 1000, 0);
    set_brake_pedal(&motorState, false, 50);

    float limit = checkMessageTimeStamps(&motorState, &globalState, 100); // 100ms later
    // Wheels should be fine (not timed out yet)

    // Test 2: One wheel timeout (>400ms)
    globalState.derateReason = 0; // Reset
    set_apps_values(&motorState, 2500, 2500, 50, 50);
    set_wheel_speeds(&motorState, 1000, 1000, 1000, 1000, 0);
    set_brake_pedal(&motorState, false, 50);
    motorState.wheelSpeeds[0].timestamp = 0;

    limit = checkMessageTimeStamps(&motorState, &globalState, 450);
    TEST_ASSERT((globalState.derateReason & DERATE_WHEEL_OUTDATED(0)) != 0,
                "Wheel 0 timeout should set flag");

    // Test 3: Three wheels timeout
    globalState.derateReason = 0; // Reset
    set_apps_values(&motorState, 2500, 2500, 50, 50);
    set_wheel_speeds(&motorState, 1000, 1000, 1000, 1000, 0);
    set_brake_pedal(&motorState, false, 50);
    motorState.wheelSpeeds[0].timestamp = 0;
    motorState.wheelSpeeds[1].timestamp = 0;
    motorState.wheelSpeeds[2].timestamp = 0;

    limit = checkMessageTimeStamps(&motorState, &globalState, 450);
    TEST_ASSERT((globalState.derateReason & DERATE_WHEEL_OUTDATED(0)) != 0,
                "Multiple wheel timeouts should set flags");

    // Test 4: All wheels timeout
    globalState.derateReason = 0; // Reset
    set_apps_values(&motorState, 2500, 2500, 50, 50);
    set_wheel_speeds(&motorState, 1000, 1000, 1000, 1000, 0);
    set_brake_pedal(&motorState, false, 50);

    limit = checkMessageTimeStamps(&motorState, &globalState, 450);
    // Should have severe limitations
}

/**
 * @brief Test checkMessageTimeStamps - Brake timeout
 */
void test_checkMessageTimeStamps_Brake(TestResults_t *tests)
{
    TEST_SECTION("Testing checkMessageTimeStamps - Brake Pedal Message");

    init_utils_params();
    MotorControlState_t motorState = create_dummy_motor_state();
    GlobalState_t globalState = create_dummy_global_state();

    // Test 1: Brake message fresh
    set_apps_values(&motorState, 2500, 2500, 50, 50);
    set_wheel_speeds(&motorState, 1000, 1000, 1000, 1000, 50);
    set_brake_pedal(&motorState, false, 0);

    float limit = checkMessageTimeStamps(&motorState, &globalState, 100); // 100ms later
    TEST_ASSERT((globalState.derateReason & DERATE_BREAK_OUTDATED) == 0,
                "Fresh brake message should not set timeout flag");

    // Test 2: Brake message timeout (>150ms)
    globalState.derateReason = 0; // Reset
    set_apps_values(&motorState, 2500, 2500, 50, 50);
    set_wheel_speeds(&motorState, 1000, 1000, 1000, 1000, 50);
    set_brake_pedal(&motorState, false, 0);

    limit = checkMessageTimeStamps(&motorState, &globalState, 200); // 200ms later
    TEST_ASSERT((globalState.derateReason & DERATE_BREAK_OUTDATED) != 0,
                "Brake timeout should set flag");
}

/**
 * @brief Test inline helper functions (change, changeFloat)
 */
void test_inline_helpers(TestResults_t *tests)
{
    TEST_SECTION("Testing Inline Helper Functions");

    // Test change() function
    int result = change(50, 0, 100, 0, 200);
    TEST_ASSERT_EQUAL(100, result, "change(50, 0, 100, 0, 200) should be 100");

    result = change(0, 0, 100, 0, 200);
    TEST_ASSERT_EQUAL(0, result, "change at min should map to out_min");

    result = change(100, 0, 100, 0, 200);
    TEST_ASSERT_EQUAL(200, result, "change at max should map to out_max");

    result = change(25, 0, 100, 100, 200);
    TEST_ASSERT_EQUAL(125, result, "change should work with offset output");

    // Test changeFloat() function
    float fresult = changeFloat(50.0f, 0.0f, 100.0f, 0.0f, 200.0f);
    TEST_ASSERT_FLOAT_EQUAL(100.0f, fresult, 0.1f,
                            "changeFloat(50, 0, 100, 0, 200) should be 100");

    fresult = changeFloat(0.0f, 0.0f, 100.0f, 0.0f, 200.0f);
    TEST_ASSERT_FLOAT_EQUAL(0.0f, fresult, 0.1f,
                            "changeFloat at min should map to out_min");

    fresult = changeFloat(100.0f, 0.0f, 100.0f, 0.0f, 200.0f);
    TEST_ASSERT_FLOAT_EQUAL(200.0f, fresult, 0.1f,
                            "changeFloat at max should map to out_max");

    fresult = changeFloat(75.0f, 0.0f, 100.0f, -50.0f, 50.0f);
    TEST_ASSERT_FLOAT_EQUAL(25.0f, fresult, 0.1f,
                            "changeFloat should work with negative ranges");
}

/**
 * @brief Main test runner for utils functions
 */
int main(void)
{
    TestResults_t results;
    init_test_results(&results);

    printf("\n");
    printf(COLOR_YELLOW "═══════════════════════════════════════\n");
    printf("  UTILS FUNCTIONS TEST SUITE\n");
    printf("═══════════════════════════════════════\n" COLOR_RESET);

    test_GetUserThrottleCommand_BothValid(&results);
    test_GetUserThrottleCommand_Mismatch(&results);
    test_GetUserThrottleCommand_OneOutOfRange(&results);
    test_calculateSpeed(&results);
    test_checkMessageTimeStamps_APPS(&results);
    test_checkMessageTimeStamps_Wheels(&results);
    test_checkMessageTimeStamps_Brake(&results);
    test_inline_helpers(&results);

    TEST_SUMMARY(results);

    return (results.failed == 0) ? 0 : 1;
}
