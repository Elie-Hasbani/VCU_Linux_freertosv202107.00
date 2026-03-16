#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "structs.h"

#include "console.h"
#include "TaskMotorController.h"

#include "utils.h"

void TaskMotorController(void *pvParameters)
{
    MotorControlState_t motorState = {0};

    TickType_t timeNow = xTaskGetTickCount();
    motorState.speed = 0;
    motorState.direction = 1; // forward
    motorState.brakePedalPressed = false;
    motorState.appsValues[0].data = 1050;
    motorState.appsValues[1].data = 1070;

    MotorControllerParams_t *params = (MotorControllerParams_t *)pvParameters;
    GlobalState_t *globalState = params->globalState;
    QueueHandle_t *xMotorControllerQueue = params->xMotorControllerQueue;
    QueueHandle_t *xIHMQueue = params->IHMQueue;
    QueueHandle_t *xCanTxQueue = params->xCanTxQueue;

    while (1)
    {
        // console_print((ledState = !ledState) ? "Led2 ON\n" : "Led2 OFF\n");
        CanMessage_t msg = {0};
        BaseType_t xReturn = xQueueReceive(*xMotorControllerQueue, &msg, pdMS_TO_TICKS(500));
        console_print("------MotorController------\n");

        if (xReturn == pdPASS)
        {
            switch (msg.id)
            {
            case 0x100: // Wheel1 speed message
                motorState.wheelSpeeds[0] = msg;
                break;
            case 0x101: // Wheel2 speed message
                motorState.wheelSpeeds[1] = msg;
                break;
            case 0x102: // Wheel3 speed message
                motorState.wheelSpeeds[2] = msg;
                break;
            case 0x103: // Wheel4 speed message
                motorState.wheelSpeeds[3] = msg;
                break;
            case 0x201: // APPS1 value
                motorState.appsValues[0] = msg;
                break;
            case 0x202: // APPS2 value
                motorState.appsValues[1] = msg;
                break;
            }

            console_print("Received from queue: id=%lu, timestamp=%lu\n", msg.id, msg.timestamp);
        }
        else
        {
            console_print("Failed to receive from queue\n");
        }

        if (motorState.lastCallTimeStmp - xTaskGetTickCount() > pdMS_TO_TICKS(5))
        {
            float throttle = ProcessThrottle(&motorState, globalState, xTaskGetTickCount());
            CanMessage_t throttleMsg = {
                .id = 0x300,                        // Throttle command message ID
                .data = (uint32_t)(throttle * 100), // Convert to percentage and scale
                .length = 4,
                .timestamp = xTaskGetTickCount()

            };

            xQueueSend(*xCanTxQueue, &throttleMsg, portMAX_DELAY);
            console_print("Calculated throttle command: %f\n", throttle);
        }

        console_print("------MotorController------\n\n");
        motorState.lastCallTimeStmp = xTaskGetTickCount();

        // xTaskDelayUntil(&timeNow, pdMS_TO_TICKS(1000));
    }
}