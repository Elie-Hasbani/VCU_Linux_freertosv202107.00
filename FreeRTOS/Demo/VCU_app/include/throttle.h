
#include "structs.h"

#ifndef THROTTLE_H
#define THROTTLE_H

bool CheckAndLimitRange(int *potval, int potIdx);
float NormalizeThrottle(int potval, int potIdx);
float CalcThrottle(MotorControlState_t *motorControlState, int processedPotVal, int processedPotValIdx);
float CalcIdleSpeed(int speed);
float CalcCruiseSpeed(int speed);
bool TemperatureDerate(float tmp, float tmpMax, float *finalSpnt);
void UdcLimitCommand(float *finalSpnt, float udc);
void IdcLimitCommand(float *finalSpnt, float idc);
void SpeedLimitCommand(float *finalSpnt, int speed);
float RampThrottle(float finalSpnt);
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
extern float throttleRamp;
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
extern int speedFiltered;
extern float potnomFiltered;
extern float brkRamped;

#endif // THROTTLE_H