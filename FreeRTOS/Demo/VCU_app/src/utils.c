
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

float GetUserThrottleCommand(int pot1val, int pot2val, int speed, bool brake)
{

    bool inRange1 = CheckAndLimitRange(&pot1val, 0);
    bool inRange2 = CheckAndLimitRange(&pot2val, 1);

    float pot1nom = NormalizeThrottle(pot1val, 0); ////normaliser pour avoir une valeure entre 0 et 100% en fonction de potmin
    float pot2nom = NormalizeThrottle(pot2val, 1); // et pot max

    // when there's something wrong with the dual throttle values,
    // we try to make the best of it and use the valid one
    if (inRange1 && inRange2)
    {

        if (ABS(pot2nom - pot1nom) > 10.0f) // si la difference est trop grande, on prend la valeure la plus petite, et si elle est trop grande
        {                                   // on la met a 50%de potmax
            // utils::PostErrorIfRunning(ERR_THROTTLE12DIFF);

            // simple implementation of a limp mode: select the lower of
            // the two throttle inputs and limiting the throttle value
            // to 50%
            if (pot1nom < pot2nom)
            {
                pot1nom = MIN(pot1nom, 50.0f);
                return CalcThrottle(pot1nom, speed, brake);
            }
            else
            {
                pot2nom = MIN(pot2nom, 50.0f);
                return CalcThrottle(pot2nom, speed, brake);
            }
        }
    }
    else if (inRange1 && !inRange2)
    {
        // utils::PostErrorIfRunning(ERR_THROTTLE2);
        pot1nom = MIN(pot1nom, 50.0f);
        return CalcThrottle(pot1nom, speed, brake);
    }
    else if (!inRange1 && inRange2)
    {
        // utils::PostErrorIfRunning(ERR_THROTTLE1);
        pot2nom = MIN(pot2nom, 50.0f);
        return CalcThrottle(pot2nom, speed, brake);
    }
    else // !inRange1 && !inRange2
    {
        // utils::PostErrorIfRunning(ERR_THROTTLE12);
        return 0.0;
    }
}

float ProcessThrottle(float finalSpnt, int speed, float motorTemp, float inverterTemp, float voltage)
{
    // we claculate messageTimouts here to minimize the probability of a context switch that would make the time_now
    // not reliable. If Max timouts are big enough, that should not pose any problem.
    // float limitMsgTimeouts = checkMessageTimeStamps(motorState, globalState, time_now);

    if (speed < throtRampRpm)
    {
        actualThrottleRamp = throtRamp;
    }
    else
    {
        actualThrottleRamp = throtRampMax;
    }

    // Throttle::UdcLimitCommand(finalSpnt,Variable             s::GetFloat(Variables::udc));
    // Throttle::IdcLimitCommand(finalSpnt, ABS(Variables::GetFloat(Variables::idc)));
    SpeedLimitCommand(&finalSpnt, ABS(speed));

    if (TemperatureDerate(motorTemp, motorTempMax, &finalSpnt))
    {
        console_print("derating becouse of motor temprature\n");
    }

    if (TemperatureDerate(inverterTemp, inverterTempMax, &finalSpnt))
    {
        console_print("derating because of inverter temprature\n");
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

/*float checkMessageTimeStamps(const MotorControlState_t *motorState, GlobalState_t *globalState, TickType_t time_now)
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
}*/
