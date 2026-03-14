#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../VCU_app/include/project_config.h" // Use project config
#include "test_helpers.h"

// Include throttle functions from main project
#include "../../VCU_app/include/throttle.h"
#include "../../VCU_app/include/my_math.h"

// External variables that need to be initialized for throttle tests
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
 * @brief Initialize throttle parameters with default values for testing
 */
void init_throttle_params(void)
{
    // Initialize potentiometer ranges (normal non-inverted)
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
 * @brief Test CheckAndLimitRange function
 */
void test_CheckAndLimitRange(TestResults_t *tests)
{
    TEST_SECTION("Testing CheckAndLimitRange");

    init_throttle_params();

    // Test 1: Value within range
    int val = 2500;
    bool result = CheckAndLimitRange(&val, 0);
    TEST_ASSERT(result == true, "Value 2500 should be within range [1000, 4000]");
    TEST_ASSERT_EQUAL(2500, val, "Value should not change when within range");

    // Test 2: Value below minimum (with slack)
    val = 700; // Below potmin - POT_SLACK
    result = CheckAndLimitRange(&val, 0);
    TEST_ASSERT(result == false, "Value 700 should be out of range");
    TEST_ASSERT_EQUAL(1000, val, "Value should be clamped to potmin");

    // Test 3: Value above maximum (with slack)
    val = 4300; // Above potmax + POT_SLACK
    result = CheckAndLimitRange(&val, 0);
    TEST_ASSERT(result == false, "Value 4300 should be out of range");
    TEST_ASSERT_EQUAL(1000, val, "Value should be set to potmin when too high");

    // Test 4: Value slightly below minimum (within slack)
    val = 950;
    result = CheckAndLimitRange(&val, 0);
    TEST_ASSERT(result == true, "Value 950 should be within range with POT_SLACK");
    TEST_ASSERT_EQUAL(1000, val, "Value should be clamped to potmin");

    // Test 5: Value slightly above maximum (within slack)
    val = 4100;
    result = CheckAndLimitRange(&val, 0);
    TEST_ASSERT(result == true, "Value 4100 should be within range with POT_SLACK");
    TEST_ASSERT_EQUAL(4000, val, "Value should be clamped to potmax");

    // Test 6: Inverted potentiometer (potmax < potmin)
    potmin[1] = 4000;
    potmax[1] = 1000;
    val = 2500;
    result = CheckAndLimitRange(&val, 1);
    TEST_ASSERT(result == true, "Value should work with inverted pot");
    TEST_ASSERT_EQUAL(2500, val, "Value should not change in inverted pot");

    // Test 7: Inverted pot - out of range
    val = 500;
    result = CheckAndLimitRange(&val, 1);
    TEST_ASSERT(result == false, "Value 500 should be out of range for inverted pot");
}

/**
 * @brief Test NormalizeThrottle function
 */
void test_NormalizeThrottle(TestResults_t *tests)
{
    TEST_SECTION("Testing NormalizeThrottle");

    init_throttle_params();

    // Test 1: Mid-range value (50%)
    float result = NormalizeThrottle(2500, 0);
    TEST_ASSERT_FLOAT_EQUAL(50.0f, result, 0.1f, "Value 2500 should normalize to 50%");

    // Test 2: Minimum value (0%)
    result = NormalizeThrottle(1000, 0);
    TEST_ASSERT_FLOAT_EQUAL(0.0f, result, 0.1f, "Minimum value should normalize to 0%");

    // Test 3: Maximum value (100%)
    result = NormalizeThrottle(4000, 0);
    TEST_ASSERT_FLOAT_EQUAL(100.0f, result, 0.1f, "Maximum value should normalize to 100%");

    // Test 4: 75% value
    result = NormalizeThrottle(3250, 0);
    TEST_ASSERT_FLOAT_EQUAL(75.0f, result, 0.1f, "Value 3250 should normalize to 75%");

    // Test 5: 25% value
    result = NormalizeThrottle(1750, 0);
    TEST_ASSERT_FLOAT_EQUAL(25.0f, result, 0.1f, "Value 1750 should normalize to 25%");

    // Test 6: Invalid index (negative)
    result = NormalizeThrottle(2500, -1);
    TEST_ASSERT_FLOAT_EQUAL(0.0f, result, 0.1f, "Invalid index should return 0%");

    // Test 7: Invalid index (too high)
    result = NormalizeThrottle(2500, 2);
    TEST_ASSERT_FLOAT_EQUAL(0.0f, result, 0.1f, "Invalid index should return 0%");

    // Test 8: Equal potmin and potmax (avoid division by zero)
    potmin[0] = 2000;
    potmax[0] = 2000;
    result = NormalizeThrottle(2000, 0);
    TEST_ASSERT_FLOAT_EQUAL(0.0f, result, 0.1f, "Equal potmin/potmax should return 0%");

    // Reset values
    init_throttle_params();
}

/**
 * @brief Test CalcThrottle function with various scenarios
 */
void test_CalcThrottle(TestResults_t *tests)
{
    TEST_SECTION("Testing CalcThrottle");

    init_throttle_params();
    MotorControlState_t motorState = create_dummy_motor_state();

    // Test 1: Neutral direction (no throttle)
    motorState.direction = 0;
    motorState.speed = 100;
    motorState.brakePedalPressed = false;
    float result = CalcThrottle(&motorState, 2500, 0);
    TEST_ASSERT_FLOAT_EQUAL(0.0f, result, 0.1f, "Neutral direction should return 0% throttle");

    // Test 2: Forward with brake pressed and low speed (no regen)
    motorState.direction = 1;
    motorState.speed = 50; // Below regenendRpm
    motorState.brakePedalPressed = true;
    result = CalcThrottle(&motorState, 2500, 0);
    TEST_ASSERT_FLOAT_EQUAL(0.0f, result, 0.1f, "Low speed braking should return 0% (no regen)");

    // Test 3: Forward with brake pressed and medium speed (tapered regen)
    motorState.direction = 1;
    motorState.speed = 500; // Between regenendRpm and regenRpm
    motorState.brakePedalPressed = true;
    result = CalcThrottle(&motorState, 2500, 0);
    TEST_ASSERT(result < 0, "Medium speed braking should return negative (regen)");
    TEST_ASSERT(result > regenBrake, "Regen should be tapered at medium speed");

    // Test 4: Forward with brake pressed and high speed (max regen)
    motorState.direction = 1;
    motorState.speed = 1500; // Above regenRpm
    motorState.brakePedalPressed = true;
    result = CalcThrottle(&motorState, 2500, 0);
    TEST_ASSERT_FLOAT_EQUAL(regenBrake, result, 0.1f, "High speed braking should return max regen");

    // Test 5: Forward without brake, below deadzone
    motorState.direction = 1;
    motorState.speed = 100;
    motorState.brakePedalPressed = false;
    int potval = 1000 + (int)(throtdead * 30.0f) - 10; // Just below deadzone
    result = CalcThrottle(&motorState, potval, 0);
    TEST_ASSERT_FLOAT_EQUAL(0.0f, result, 0.1f, "Below deadzone should return 0%");

    // Test 6: Forward without brake, above deadzone
    motorState.direction = 1;
    motorState.speed = 100;
    motorState.brakePedalPressed = false;
    result = CalcThrottle(&motorState, 2500, 0);
    TEST_ASSERT(result > 0, "Normal throttle should be positive");
    TEST_ASSERT(result <= throtmax, "Throttle should not exceed throtmax");

    // Test 7: Forward with high speed (no regen available at low throttle)
    motorState.direction = 1;
    motorState.speed = 1500;
    motorState.brakePedalPressed = false;
    result = CalcThrottle(&motorState, 2500, 0);
    TEST_ASSERT(result > 0, "High speed throttle should be positive");
}

/**
 * @brief Test RampThrottle function
 */
void test_RampThrottle(TestResults_t *tests)
{
    TEST_SECTION("Testing RampThrottle");

    init_throttle_params();

    // Test 1: Ramp up from 0 to 50
    extern float throttleRamped;
    throttleRamped = 0.0f;
    actualThrottleRamp = 10.0f;

    float result = RampThrottle(50.0f);
    TEST_ASSERT_FLOAT_EQUAL(10.0f, result, 0.1f, "First ramp should increase by ramp rate");

    // Test 2: Continue ramping up
    result = RampThrottle(50.0f);
    TEST_ASSERT_FLOAT_EQUAL(20.0f, result, 0.1f, "Second ramp should continue increase");

    // Test 3: Reach target before full ramp
    result = RampThrottle(22.0f);
    TEST_ASSERT_FLOAT_EQUAL(22.0f, result, 0.1f, "Should reach target exactly");

    // Test 4: Instant ramp down (no ramping)
    throttleRamped = 50.0f;
    result = RampThrottle(20.0f);
    TEST_ASSERT_FLOAT_EQUAL(20.0f, result, 0.1f, "Ramp down should be instant for positive values");

    // Test 5: Ramp up negative values (regen)
    throttleRamped = -10.0f;
    regenRamp = 5.0f;
    result = RampThrottle(-30.0f);
    TEST_ASSERT_FLOAT_EQUAL(-15.0f, result, 0.1f, "Negative ramp should use regenRamp rate");

    // Test 6: Clamp to throtmax
    throttleRamped = 0.0f;
    throtmax = 80.0f;
    result = RampThrottle(100.0f);
    TEST_ASSERT(result <= 80.0f, "Should clamp to throtmax");

    // Test 7: Clamp to throtmin
    throttleRamped = 0.0f;
    throtmin = -40.0f;
    result = RampThrottle(-60.0f);
    TEST_ASSERT(result >= -40.0f, "Should clamp to throtmin");
}

/**
 * @brief Test SpeedLimitCommand function
 */
void test_SpeedLimitCommand(TestResults_t *tests)
{
    TEST_SECTION("Testing SpeedLimitCommand");

    init_throttle_params();
    speedLimit = 5000;

    // Test 1: Speed well below limit
    float throttle = 80.0f;
    SpeedLimitCommand(&throttle, 3000);
    TEST_ASSERT(throttle > 0, "Below speed limit should allow positive throttle");

    // Test 2: Speed at limit
    throttle = 80.0f;
    SpeedLimitCommand(&throttle, 5000);
    TEST_ASSERT(throttle >= 0, "At speed limit should limit throttle");
    TEST_ASSERT(throttle < 80.0f, "At speed limit should reduce requested throttle");

    // Test 3: Negative throttle (regen) should not be limited
    throttle = -20.0f;
    float original = throttle;
    SpeedLimitCommand(&throttle, 5500);
    TEST_ASSERT_FLOAT_EQUAL(original, throttle, 0.1f, "Negative throttle should not be limited");

    // Test 4: Zero throttle
    throttle = 0.0f;
    SpeedLimitCommand(&throttle, 5500);
    TEST_ASSERT_FLOAT_EQUAL(0.0f, throttle, 0.1f, "Zero throttle should remain zero");
}

/**
 * @brief Test TemperatureDerate function
 */
void test_TemperatureDerate(TestResults_t *tests)
{
    TEST_SECTION("Testing TemperatureDerate");

    init_throttle_params();
    GlobalState_t globalState = create_dummy_global_state();

    // Test 1: Normal temperature (no derate)
    globalState.derateReason = 0;
    float throttle = 80.0f;
    bool derated = TemperatureDerate(&globalState, &throttle);
    TEST_ASSERT(derated == false, "Normal temp should not derate");
    TEST_ASSERT_FLOAT_EQUAL(80.0f, throttle, 0.1f, "Normal temp should not change throttle");

    // Test 2: High temperature (50% derate)
    globalState.derateReason = DERATE_HIGHTEMP;
    throttle = 80.0f;
    derated = TemperatureDerate(&globalState, &throttle);
    TEST_ASSERT(derated == true, "High temp should derate");
    TEST_ASSERT_FLOAT_EQUAL(50.0f, throttle, 0.1f, "High temp should limit to 50%");

    // Test 3: Over temperature (full derate)
    globalState.derateReason = DERATE_OVERTEMP;
    throttle = 80.0f;
    derated = TemperatureDerate(&globalState, &throttle);
    TEST_ASSERT(derated == true, "Over temp should derate");
    TEST_ASSERT_FLOAT_EQUAL(0.0f, throttle, 0.1f, "Over temp should limit to 0%");

    // Test 4: Both high and over temp (full derate)
    globalState.derateReason = DERATE_HIGHTEMP | DERATE_OVERTEMP;
    throttle = 80.0f;
    derated = TemperatureDerate(&globalState, &throttle);
    TEST_ASSERT_FLOAT_EQUAL(0.0f, throttle, 0.1f, "Both flags should result in 0%");

    // Test 5: Negative throttle with high temp
    globalState.derateReason = DERATE_HIGHTEMP;
    throttle = -40.0f;
    TemperatureDerate(&globalState, &throttle);
    TEST_ASSERT(throttle >= -50.0f, "Negative throttle should also be limited");
}

/**
 * @brief Main test runner for throttle functions
 */
int main(void)
{
    TestResults_t results;
    init_test_results(&results);

    printf("\n");
    printf(COLOR_YELLOW "═══════════════════════════════════════\n");
    printf("  THROTTLE FUNCTIONS TEST SUITE\n");
    printf("═══════════════════════════════════════\n" COLOR_RESET);

    /*test_CheckAndLimitRange(&results);
    test_NormalizeThrottle(&results);
    test_CalcThrottle(&results);
    test_RampThrottle(&results);*/
    test_SpeedLimitCommand(&results);
    // test_TemperatureDerate(&results);

    TEST_SUMMARY(results);

    return (results.failed == 0) ? 0 : 1;
}
