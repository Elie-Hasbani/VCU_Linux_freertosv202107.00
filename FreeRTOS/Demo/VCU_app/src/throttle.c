#include <stdbool.h>
#include "throttle.h"
#include "structs.h"
#include "utils.h"
#include "my_math.h"

#define POT_SLACK 200

int potmin[2];
int potmax[2];
float regenRpm;
float brknompedal;
float regenmax;
float regenBrake;
float brkcruise;
float throtmax;
float throtmaxRev;
float throtmin;
float throtdead;
int idleSpeed;
int cruiseSpeed;
float speedkp;
int speedflt;
float idleThrotLim;
float regenRamp;
float throttleRamp;
int bmslimhigh;
int bmslimlow;
int accelmax;
int accelflt;
float udcmin;
float udcmax;
float idcmin;
float idcmax;
int speedLimit;
float regenendRpm;
float ThrotRpmFilt;

int speedFiltered;
float potnomFiltered;
float brkRamped;

float UDCres;
float IDCres;
float UDCprevspnt = 0;
float IDCprevspnt = 0;

float throttleRamped = 0.0;
float SpeedFiltered = 0.0; // limitation des saut de vitesse

static float regenlim = 0;

#define PedalPosArrLen 50
float PedalPos;
float LastPedalPos;
float PedalChange = 0;
float PedalPosTot = 0;
float PedalPosArr[PedalPosArrLen];
uint8_t PedalPosIdx = 0;
int8_t PedalReq = 0; // positive is accel negative is decell

/**
 * @brief Check the throttle input for sanity and limit the range to min/max values
 *
 * @param potval Pointer to the throttle input array, range should be [potMin, potMax].
 * @param potIdx Index of the throttle input array, range is [0, 1].
 * @return true if the throttle input was within bounds (accounting for POT_SLACK).
 * @return false the throttle was too far out of bounds. Setting potval to the minimum.
 */
bool CheckAndLimitRange(int *potval, int potIdx)
{
    // The range check accounts for inverted throttle pedals, where the minimum
    // value is higher than the maximum. To accomodate for that, the potMin and potMax
    // variables are set for internal use.
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

/**
 * @brief Normalize the throttle input value to the min-max scale.
 *
 * Returns 0.0% for illegal indices and if potmin and potmax are equal.
 *
 * @param potval Throttle input value, range is [potmin[potIdx], potmax[potIdx]], not checked!
 * @param potIdx Index of the throttle input, should be [0, 1].
 * @return Normalized throttle value output with range [0.0, 100.0] with correct input.
 */
float NormalizeThrottle(int potval, int potIdx)
{
    if (potIdx < 0 || potIdx > 1)
        return 0.0f;

    if (potmin[potIdx] == potmax[potIdx])
        return 0.0f;

    return 100.0f * ((float)(potval - potmin[potIdx]) / (float)(potmax[potIdx] - potmin[potIdx]));
}

/**
 * @brief Calculate a throttle percentage from the potval input.
 *
 * After the previous range checks, the throttle input potval lies within the
 * range of [potmin[0], potmax[0]]. From this range, the input is converted to
 * a percent range of [-100.0, 100.0].

 *
 * TODO: No regen implemented. Commanding 0 throttle while braking, otherwise direct output.
 *
 * @param potval
 * @param idx Index of the throttle input that should be used for calculation.
 * @param brkpedal Brake pedal input (true for brake pedal pressed, false otherwise).
 * @return float
 */
float CalcThrottle(MotorControlState_t *motorControlState, int processedPotVal, int processedPotValIdx) /*int potval, int potIdx, bool brkpedal*/
{
    int speed = motorControlState->speed;    // Variables::GetInt(VarIds::SPEED);
    bool dir = motorControlState->direction; // Variables::GetInt(VarIds::DIRECTION);
    float potnom = 0.0f;                     // normalize potval against the potmin and potmax values

    if (speed < 0) // make sure speed is not negative
    {
        speed *= -1;
    }

    // limiting speed change rate(pas d'accouts de vitesse)
    //  Si on recois des trames CAN éronnées, ou si il y as du bruit,les vcaleurs de speed recus seront fausses,donc les valeur de throttle calculés seront trop grands,
    // par rapport au précedante si speed ondule beaucou trop. c'est pour ca qu'on ramp le speed, meme si le couple calculé n'auras pas d'effet
    // ca evite de faire partie en vrille notre inverter a cause de bruit/glich CAN.
    if (ABS(speed - SpeedFiltered) > ThrotRpmFilt) /// max change is ThrotRpmFilt value
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

    ///////////////////////

    if (dir == 0) // neutral no torque command
    {
        return 0;
    }

    if (motorControlState->brakePedalPressed) // if break is pressed, we must finish here and return a negative value.
    {
        if (speed < 100 || speed < regenendRpm)
        {
            return 0; // noregen, too slow
        }
        else if (speed < regenRpm)
        {
            potnom = change(speed, regenendRpm, regenRpm, 0, regenBrake); // taper regen according to speed
            return potnom;
        }
        else
        { // very fast, max regen
            potnom = regenBrake;
            return potnom;
        }
    }

    // substract offset, bring potval to the potmin-potmax scale and make a percentage
    potnom = NormalizeThrottle(processedPotVal, processedPotValIdx); // to bring it between 0 and 100

    // Apply the deadzone parameter. To avoid that we lose the range between
    // 0 and throtdead, the scale of potnom is mapped from the [0.0, 100.0] scale
    // to the [throtdead, 100.0] scale.
    if (potnom < throtdead)
    {
        potnom = 0.0f; /// throtdead is the min accepted by the Inverter
    }
    else
    {
        potnom = (potnom - throtdead) * (100.0f / (100.0f - throtdead));
    }

    //!! pedal command intent coding

    /*PedalPos = potnom; //save comparison next time to check if pedal had moved

    float TempAvgPos = AveragePos(PedalPos); //get an rolling average pedal position over the last 50 measurements for smoothing

    PedalChange = PedalPos - TempAvgPos; //current pedal position compared to average


    if(PedalChange < -1.0 )//Check pedal is release compared to last time
    {
        PedalReq = -1; //pedal is released enough - Commanding regen or slowing
    }
    else if(PedalChange > 1.0 )//Check pedal is increased compared to last time
    {
        PedalReq = 1; //pedal pressed - Commanding accelerating - thus always more power
    }
    else//pedal not changed
    {
        potnom = TempAvgPos; //use the averaged pedal
    }*/

    // Do clever bits for regen and such.

    if (speed < 100 || speed < regenendRpm) // No regen under 100 rpm or speed under regenendRpm
    {
        regenlim = 0;
    }
    else if (speed < regenRpm)
    {
        regenlim = change(speed, regenendRpm, regenRpm, 0, regenmax); // taper regen according to speed
    }
    else
    {
        regenlim = regenmax;
    }

    //!!!potnom is throttle position up to this point//

    if (dir == 1) // Forward
    {
        // change limits to uint32, multiply by 10 then 0.1 to add a decimal to remove the hard edges
        potnom = changeFloat(potnom, 0, 100, regenlim * 10, throtmax * 10); ////most important line for regen calculation, will have a schematics to explain how it works.
        potnom *= 0.1;
    }

    LastPedalPos = PedalPos; // Save current pedal position for next loop. //inutilisé
    return potnom;
}