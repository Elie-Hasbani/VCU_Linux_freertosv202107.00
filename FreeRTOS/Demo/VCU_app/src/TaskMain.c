#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "structs.h"

#include "console.h"
#include "TaskMain.h"

#include "utils.h"
#include "my_fp.h"
#include "project_config.h"

void PrintMotorControllerState(const MotorControlState_t *motorState);

void TaskMain(void *pvParameters)
{
    MotorControlState_t motorState = {0};

    TickType_t timeNow = xTaskGetTickCount();
    motorState.speed = 0;
    motorState.direction = 1; // forward
    motorState.brakePedalPressed = false;
    motorState.appsValues[0].data = 1050;
    motorState.appsValues[1].data = 1070;

    MainParams_t *params = (MainParams_t *)pvParameters;
    QueueHandle_t *xMainQueue = params->xMainQueue;
    QueueHandle_t *xTRVPQueu = params->xTRVPQueue;
    QueueHandle_t *xIHMQueue = params->xIHMQueue;
    QueueHandle_t *xCanTxQueue = params->xCanTxQueue;

    while (1)
    {
        // console_print((ledState = !ledState) ? "Led2 ON\n" : "Led2 OFF\n");

        dataMessage_t msg = {0};
        BaseType_t xReturn;
        console_print("------(B)MotorController------\n");

        while ((xQueueReceive(*xMainQueue, &msg, pdMS_TO_TICKS(100))) == pdPASS)
        {
            switch (msg.id)
            {
            case wheel1Id: // Wheel1 speed message
                motorState.wheelSpeeds[0] = msg;
                break;
            case wheel2Id: // Wheel2 speed message
                motorState.wheelSpeeds[1] = msg;
                break;
            case wheel3Id: // Wheel3 speed message
                motorState.wheelSpeeds[2] = msg;
                break;
            case wheel4Id: // Wheel4 speed message
                motorState.wheelSpeeds[3] = msg;
                break;
            case apps1Id: // APPS1 value
                motorState.appsValues[0] = msg;
                break;
            case apps2Id: // APPS2 value
                motorState.appsValues[1] = msg;
                break;
            }

            console_print("Received from queue: id=%x, timestamp=%lu\n", msg.id, msg.timestamp);
        }

        if (motorState.lastCallTimeStmp - xTaskGetTickCount() > pdMS_TO_TICKS(5))
        {
            float throttle = 8.0f; // ProcessThrottle(&motorState, globalState, xTaskGetTickCount());
            dataMessage_t throttleMsg = {
                .id = 0x50,                   // Throttle command message ID
                .data = FP_FROMFLT(throttle), // Convert to percentage and scale
                .timestamp = xTaskGetTickCount()

            };

            xQueueSend(*xCanTxQueue, &throttleMsg, portMAX_DELAY);
            console_print("Calculated throttle command: %f\n", throttle);
        }

        PrintMotorControllerState(&motorState);

        console_print("------(E)MotorController------\n\n");
        motorState.lastCallTimeStmp = xTaskGetTickCount();
    }

    // xTaskDelayUntil(&timeNow, pdMS_TO_TICKS(1000));
}

// functio, to print current state of the motor controller (for debug)
void PrintMotorControllerState(const MotorControlState_t *motorState)
{
    console_print("Motor Control State:\n");
    console_print("  Throttle Command: %lu\n", motorState->ThrottleCommand);
    console_print("  Speed: %lu\n", motorState->speed);
    console_print("  Direction: %s\n", motorState->direction ? "Forward" : "Reverse");
    console_print("  Brake Pedal Pressed: %s\n", motorState->brakePedalPressed ? "Yes" : "No");
    for (int i = 0; i < 4; ++i)
    {
        console_print("  Wheel Speed %d: ID=%x, Data=%lu, Timestamp=%lu\n", i + 1, motorState->wheelSpeeds[i].id, motorState->wheelSpeeds[i].data, motorState->wheelSpeeds[i].timestamp);
    }
    for (int i = 0; i < 2; ++i)
    {
        console_print("  APPS Value %d: ID=%x, Data=%lu, Timestamp=%lu\n", i + 1, motorState->appsValues[i].id, motorState->appsValues[i].data, motorState->appsValues[i].timestamp);
    }
}