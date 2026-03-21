#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"

#ifndef STRUCTS_H
#define STRUCTS_H

typedef enum messageIds
{
    wheel1Id,
    wheel2Id,
    wheel3Id,
    wheel4Id,

    apps1Id,
    apps2Id,

    motor_tempId,
    inverter_tempId,
    voltageId,

    throtle_cmdId

} MessageIds;

typedef struct
{
    uint8_t id;
    int32_t data;
    TickType_t timestamp;

} dataMessage_t;

typedef struct
{
    int32_t ThrottleCommand; // calculated throttle command to be sent to the inverter
    uint32_t speed;          // calculated speed

    dataMessage_t wheelSpeeds[4];
    dataMessage_t appsValues[2];

    bool brakePedalPressed; // true if brake pedal is pressed, false otherwise
    TickType_t lastCheckedBreakPedal;

    TickType_t lastCallTimeStmp;

    bool direction; // true for forward, false for reverse

} MotorControlState_t;

typedef struct
{
    int32_t ThrottleCommand; // calculated throttle command to be sent to the inverter
    uint32_t speed;          // calculated speed

    dataMessage_t wheelSpeeds[4];
    dataMessage_t appsValues[2];

    bool brakePedalPressed; // true if brake pedal is pressed, false otherwise
    TickType_t lastCheckedBreakPedal;

    TickType_t lastCallTimeStmp;

    bool direction; // true for forward, false for reverse

} MainState_t;

typedef struct
{
    dataMessage_t motorTemp;
    dataMessage_t inverterTemp;
    dataMessage_t Voltage;

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

//
//
//
//
// Tasks parameters
typedef struct
{
    QueueHandle_t *xMainQueue;
    QueueHandle_t *xTRVPQueue;
    QueueHandle_t *xIHMQueue;
    QueueHandle_t *xCanTxQueue;
} MainParams_t;

typedef struct
{
    QueueHandle_t *xMainQueue;
    QueueHandle_t *xTRVPQueue;
} CanRxParams_t;

typedef struct
{
    QueueHandle_t *xCanTxQueue;
} CanTxParam_t;

typedef struct
{
    QueueHandle_t *xTRVPQueue;
    // QueueHandle_t *xIHMQueue;
} TRVPParams_t;
#endif // STRUCTS_H