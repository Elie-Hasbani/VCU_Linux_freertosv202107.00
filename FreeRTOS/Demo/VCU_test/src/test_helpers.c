#include "test_helpers.h"

/**
 * @brief Create a dummy motor control state with default values
 */
MotorControlState_t create_dummy_motor_state(void)
{
    MotorControlState_t state;
    memset(&state, 0, sizeof(MotorControlState_t));

    // Default values
    state.ThrottleCommand = 0;
    state.speed = 0;
    state.brakePedalPressed = false;
    state.direction = true; // Forward
    state.lastCheckedBreakPedal = 0;

    // Initialize APPS values
    for (int i = 0; i < 2; i++)
    {
        state.appsValues[i].id = 0x100 + i;
        state.appsValues[i].data = 0;
        state.appsValues[i].length = 8;
        state.appsValues[i].timestamp = 0;
    }

    // Initialize wheel speeds
    for (int i = 0; i < 4; i++)
    {
        state.wheelSpeeds[i].id = 0x200 + i;
        state.wheelSpeeds[i].data = 0;
        state.wheelSpeeds[i].length = 8;
        state.wheelSpeeds[i].timestamp = 0;
    }

    return state;
}

/**
 * @brief Create a dummy global state with default values
 */
GlobalState_t create_dummy_global_state(void)
{
    GlobalState_t state;
    memset(&state, 0, sizeof(GlobalState_t));

    state.derateReason = 0; // No derate

    return state;
}

/**
 * @brief Set APPS values in the motor state
 */
void set_apps_values(MotorControlState_t *state, int val1, int val2, TickType_t time1, TickType_t time2)
{
    state->appsValues[0].data = val1;
    state->appsValues[0].timestamp = time1;

    state->appsValues[1].data = val2;
    state->appsValues[1].timestamp = time2;
}

/**
 * @brief Set wheel speeds in the motor state
 */
void set_wheel_speeds(MotorControlState_t *state, int w1, int w2, int w3, int w4, TickType_t time)
{
    state->wheelSpeeds[0].data = w1;
    state->wheelSpeeds[0].timestamp = time;

    state->wheelSpeeds[1].data = w2;
    state->wheelSpeeds[1].timestamp = time;

    state->wheelSpeeds[2].data = w3;
    state->wheelSpeeds[2].timestamp = time;

    state->wheelSpeeds[3].data = w4;
    state->wheelSpeeds[3].timestamp = time;
}

/**
 * @brief Set brake pedal state
 */
void set_brake_pedal(MotorControlState_t *state, bool pressed, TickType_t time)
{
    state->brakePedalPressed = pressed;
    state->lastCheckedBreakPedal = time;
}

/**
 * @brief Initialize test results
 */
void init_test_results(TestResults_t *results)
{
    results->passed = 0;
    results->failed = 0;
    results->total = 0;
}
