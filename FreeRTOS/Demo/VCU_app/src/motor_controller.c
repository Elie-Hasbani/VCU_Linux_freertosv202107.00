#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "structs.h"

#include "console.h"
#include "motor_controller.h"
#include "queue_handles.h"

#include "utils.h"

MotorControlState_t motorState = {0};

void MotorController(void *pvParameters)
{
    TickType_t lastWakeUp = xTaskGetTickCount();
    motorState.speed = 0;
    motorState.direction = 1; // forward
    motorState.brakePedalPressed = false;
    motorState.appsValues[0].data = 1050;
    motorState.appsValues[1].data = 1070;

    MotorControllerParams_t *params = (MotorControllerParams_t *)pvParameters;
    GlobalState_t *globalState = params->globalState;

    while (1)
    {
        // console_print((ledState = !ledState) ? "Led2 ON\n" : "Led2 OFF\n");
        CanMessage_t msg = {0};
        BaseType_t xReturn = xQueueReceive(xMainQueue, &msg, pdMS_TO_TICKS(500));

        if (xReturn == pdPASS)
        {
            console_print("Received from queue: id=%lu, timestamp=%lu\n", msg.id, msg.timestamp);
            // float throttle = GetUserThrottleCommand(&motorState);
            float throttle = ProcessThrottle(&motorState, globalState);
            console_print("Calculated throttle command: %f\n", throttle);
        }
        else
        {
            console_print("Failed to receive from queue\n");
        }
        // xTaskDelayUntil(&lastWakeUp, pdMS_TO_TICKS(1000));
    }
}