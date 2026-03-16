
#include <stdbool.h>
#include "throttle.h"
#include "structs.h"
#include "utils.h"
#include "my_math.h"
#include "console.h"
#include "project_config.h"

#include "FreeRTOS.h"

#define MAX_APPS_TIMEOUT 200
#define MAX_WHEEL_TIMEOUT 400
#define MAX_BREAK_TIMEOUT 150

float GetUserThrottleCommand(const MotorControlState_t *motorState)
{
    bool brake = motorState->brakePedalPressed; // Param::GetBool(Param::din_brake);
    // int potmode = 0;    // Param::GetInt(Param::potmode);
    int direction = 1; // Param::GetInt(Param::dir);

    int pot1val = motorState->appsValues[0].data; // Param::GetInt(Param::pot);
    int pot2val = motorState->appsValues[1].data; // Param::GetInt(Param::pot2);

    console_print("potval1 = %d potval2 = %d\n", pot1val, pot2val);

    bool inRange1 = CheckAndLimitRange(&pot1val, 0);
    bool inRange2 = CheckAndLimitRange(&pot2val, 1);
    int useChannel = 0; // default case: use Throttle 1

    // when there's something wrong with the dual throttle values,
    // we try to make the best of it and use the valid one
    if (inRange1 && inRange2)
    {
        // These are only temporary values, because they can change
        // if the "limp mode" is activated.
        float pot1nomTmp = NormalizeThrottle(pot1val, 0); ////normaliser pour avoir une valeure entre 0 et 100% en fonction de potmin
        float pot2nomTmp = NormalizeThrottle(pot2val, 1); // et pot max

        if (ABS(pot2nomTmp - pot1nomTmp) > 10.0f) // si la difference est trop grande, on prend la valeure la plus petite, et si elle est trop grande
        {                                         // on la met a 50%de potmax
            // utils::PostErrorIfRunning(ERR_THROTTLE12DIFF);

            // simple implementation of a limp mode: select the lower of
            // the two throttle inputs and limiting the throttle value
            // to 50%
            if (pot1nomTmp < pot2nomTmp)
            {
                if (pot1nomTmp > 50.0f)
                    pot1val = potmax[0] / 2;

                useChannel = 0;
            }
            else
            {
                if (pot2nomTmp > 50.0f)
                    pot2val = potmax[1] / 2;

                useChannel = 1;
            }
        }
    }
    else if (inRange1 && !inRange2)
    {
        // utils::PostErrorIfRunning(ERR_THROTTLE2);

        useChannel = 0; // use throttle channel 1
    }
    else if (!inRange1 && inRange2)
    {
        // utils::PostErrorIfRunning(ERR_THROTTLE1);

        useChannel = 1; // use throttle channel 2
    }
    else // !inRange1 && !inRange2
    {
        // utils::PostErrorIfRunning(ERR_THROTTLE12);

        return 0.0;
    }

    // don't return a throttle value if we are in neutral
    // TODO: the direction for FORWARD/NEUTRAL/REVERSE needs an enum in param_prj.h as well
    if (direction == 0)
        return 0.0;

    if (direction == 2) // No throttle val if in PARK also.
        return 0.0;

    // calculate the throttle depending on the channel we've decided to use
    if (useChannel == 0)
        return CalcThrottle(motorState, pot1val, 0);
    else if (useChannel == 1)
        return CalcThrottle(motorState, pot2val, 1);
    else
        return 0.0;
}

float ProcessThrottle(const MotorControlState_t *motorState, const GlobalState_t *globalState, TickType_t time_now)
{
    // we claculate messageTimouts here to minimize the probability of a context switch that would make the time_now
    // not reliable. If Max timouts are big enough, that should not pose any problem.
    float limitMsgTimeouts = checkMessageTimeStamps(motorState, globalState, time_now);

    float finalSpnt;
    int speed = motorState->speed;

    if (speed < throtRampRpm)
    {
        actualThrottleRamp = throtRamp;
    }
    else
    {
        actualThrottleRamp = throtRampMax;
    }

    finalSpnt = GetUserThrottleCommand(motorState);

    // Throttle::UdcLimitCommand(finalSpnt,Variable             s::GetFloat(Variables::udc));
    // Throttle::IdcLimitCommand(finalSpnt, ABS(Variables::GetFloat(Variables::idc)));
    SpeedLimitCommand(&finalSpnt, ABS(speed));

    if (TemperatureDerate(globalState, &finalSpnt))
    {
        console_print("derating becouse of temprature\n");
    }

    finalSpnt = RampThrottle(finalSpnt);

    // make sure the torque percentage is NEVER out of range
    if (finalSpnt < -100.0f)
        finalSpnt = -100.0f;
    else if (finalSpnt > 100.0f)
        finalSpnt = 100.0f;

    return finalSpnt;
}

int calculateSpeed(const MotorControlState_t *motorState)
{

    // Simple average of the four wheel speeds
    int sum = 0;
    for (int i = 0; i < 4; ++i)
    {
        sum += motorState->wheelSpeeds[i].data;
    }
    return sum / 4;
}

float checkMessageTimeStamps(const MotorControlState_t *motorState, GlobalState_t *globalState, TickType_t time_now)
{
    // on ne retourne pas tout de suite pour que si on change les limites, on ne soit pas obliger de changer le code.(on pourrait retourner dans)
    // l'ordre croissant des limites, mais si on changes->il faut changer la structure du code)

    /////////// APPS /////////////////
    int limit = 100;
    if (time_now - motorState->appsValues[0].timestamp > MAX_APPS_TIMEOUT && time_now - motorState->appsValues[1].timestamp > MAX_APPS_TIMEOUT)
    {
        limit = MIN(limit, 0);
    }
    else if (time_now - motorState->appsValues[0].timestamp > MAX_APPS_TIMEOUT)
    {
        limit = MIN(limit, 50);
        globalState->derateReason |= DERATE_APPS1_OUTDATED;
    }
    else if (time_now - motorState->appsValues[1].timestamp > MAX_APPS_TIMEOUT)
    {
        limit = MIN(limit, 50);
        globalState->derateReason |= DERATE_APPS2_OUTDATED;
    }

    /////////// WheelSpeed /////////////////
    int nbOutdatedWheelValues = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (time_now - motorState->wheelSpeeds[i].timestamp > MAX_WHEEL_TIMEOUT)
        {
            globalState->derateReason |= DERATE_WHEEL_OUTDATED(i);
            nbOutdatedWheelValues++;
        }
    }
    if (nbOutdatedWheelValues == 4)
    {
        limit = MIN(limit, 0);
    }
    else if (nbOutdatedWheelValues == 3)
    {
        limit = MIN(limit, 40);
    }
    else if (nbOutdatedWheelValues >= 1)
    {
        limit = MIN(limit, 50);
    }

    /////////// Breakpedal /////////////////
    if (time_now - motorState->lastCheckedBreakPedal > MAX_BREAK_TIMEOUT)
    {
        limit = MIN(limit, 0);
        globalState->derateReason |= DERATE_BREAK_OUTDATED;
    }
}

void RegulateTemprature(GlobalState_t *globalState, TempratureVoltageState_t tempVltState)
{
}