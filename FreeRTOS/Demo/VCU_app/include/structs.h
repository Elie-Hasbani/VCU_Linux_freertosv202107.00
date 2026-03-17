#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"

#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct
{
    uint8_t id;
    int32_t data;
    uint8_t length;
    TickType_t timestamp;

} CanMessage_t;

typedef struct
{
    int32_t ThrottleCommand; // calculated throttle command to be sent to the inverter
    uint32_t speed;          // calculated speed

    CanMessage_t wheelSpeeds[4];
    CanMessage_t appsValues[2];

    bool brakePedalPressed; // true if brake pedal is pressed, false otherwise
    TickType_t lastCheckedBreakPedal;

    TickType_t lastCallTimeStmp;

    bool direction; // true for forward, false for reverse

} MotorControlState_t;

typedef struct
{
    CanMessage_t motorTemp;
    CanMessage_t inverterTemp;
    CanMessage_t Voltage;

    TickType_t lastCallTimeStmp;

    bool inverterTempChanged;
    bool fansOrder;

    bool motorTempChanged;
    bool pumpsOrder;

} TempratureVoltageState_t;

typedef enum Order
{
    FaultLight = 0,
    CoolingPump = 1,
    CoolingFans = 2,
    R2Dbuzzer = 3,

} Order_t;

typedef struct
{
    Order_t order;
    bool state;

} IHMOrder_t;

typedef struct
{
    uint16_t derateReason;
} GlobalState_t;

//
//
//
//
// Tasks parameters
typedef struct
{
    GlobalState_t *globalState;
    QueueHandle_t *xMotorControllerQueue;
    QueueHandle_t *IHMQueue;
    QueueHandle_t *xCanTxQueue;
} MotorControllerParams_t;

typedef struct
{
    QueueHandle_t *xTemperatureVoltageQueue;
    QueueHandle_t *xMotorControllerQueue;
} CanRxParams_t;

typedef struct
{
    QueueHandle_t *xCanTxQueue;
} CanTxParam_t;

typedef struct
{
    GlobalState_t *globalState;
    QueueHandle_t *xTemperatureVoltageQueue;
    QueueHandle_t *IHMQueue;

} TmpVltMngrParams_t;
#endif // STRUCTS_H