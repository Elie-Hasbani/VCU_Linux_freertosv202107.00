
#include <stdbool.h>
#include "throttle.h"
#include "structs.h"
#include "utils.h"
#include "my_math.h"
#include "console.h"

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

float ProcessThrottle(const MotorControlState_t *motorState, const GlobalState_t *globalState)
{
    float finalSpnt;
    int speed = motorState->speed;

    if (speed < ThrotRampRpm)
    {
        throttleRamp = ThrotRamp;
    }

    else
    {
        throttleRamp = ThrotRampMax;
    }

    finalSpnt = GetUserThrottleCommand(motorState);

    // Throttle::UdcLimitCommand(finalSpnt,Variable             s::GetFloat(Variables::udc));
    // Throttle::IdcLimitCommand(finalSpnt, ABS(Variables::GetFloat(Variables::idc)));
    SpeedLimitCommand(&finalSpnt, ABS(speed));

    if (TemperatureDerate(globalState, &finalSpnt))
    {
        console_print("derating becouse of temprature");
    }

    // finalSpnt = RampThrottle(finalSpnt);

    // make sure the torque percentage is NEVER out of range
    if (finalSpnt < -100.0f)
        finalSpnt = -100.0f;
    else if (finalSpnt > 100.0f)
        finalSpnt = 100.0f;

    return finalSpnt;
}