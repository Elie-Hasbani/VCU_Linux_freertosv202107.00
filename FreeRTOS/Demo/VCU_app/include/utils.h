
#include "FreeRTOS.h"

float GetUserThrottleCommand(const MotorControlState_t *motorState);
float ProcessThrottle(const MotorControlState_t *motorState, const GlobalState_t *globalState, TickType_t time_now);
float checkMessageTimeStamps(const MotorControlState_t *motorState, GlobalState_t *globalState, TickType_t time_now);
void RegulateTemprature(GlobalState_t *globalState, TempratureVoltageState_t tempVltState);

inline static int32_t change(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline static float changeFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}