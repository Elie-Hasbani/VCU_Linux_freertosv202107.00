#include <stdint.h>
#include <stdbool.h>

#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct
{
    uint32_t id;
    uint32_t data;
    uint8_t length;
    uint32_t timestamp;

} CanMessage_t;

typedef struct
{
    uint32_t ThrottleCommand; // calculated throttle command to be sent to the inverter
    uint32_t speed;           // calculated speed

    CanMessage_t wheelSpeeds[4];
    CanMessage_t appsValues[2];

    bool brakePedalPressed; // true if brake pedal is pressed, false otherwise
    bool direction;         // true for forward, false for reverse

} MotorControlState_t;

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
    GlobalState_t *globalState
} MotorControllerParams_t;

#endif // STRUCTS_H