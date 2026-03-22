#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "structs.h"

#include "console.h"
#include "TaskMain.h"

#include "utils.h"
#include "my_fp.h"
#include "project_config.h"

void PrintMainState(const MainState_t *mainState);

void TaskMain(void *pvParameters)
{
    MainState_t mainState = {0};

    TickType_t timeNow = xTaskGetTickCount();

    MainParams_t *params = (MainParams_t *)pvParameters;
    QueueHandle_t *xMainQueue = params->xMainQueue;
    QueueHandle_t *xTRVPQueu = params->xTRVPQueue;
    QueueHandle_t *xIHMQueue = params->xIHMQueue;
    QueueHandle_t *xCanTxQueue = params->xCanTxQueue;

    mainState.inverterTemp.timeStatus = STATUS_NOT_READY;
    mainState.motorTemp.timeStatus = STATUS_NOT_READY;
    mainState.Voltage.timeStatus = STATUS_NOT_READY;
    mainState.speed.timeStatus = STATUS_NOT_READY;
    mainState.ThrottleCommand.timeStatus = STATUS_NOT_READY;

    while (1)
    {
        // console_print((ledState = !ledState) ? "Led2 ON\n" : "Led2 OFF\n");

        dataMessage_t msg = {0};
        BaseType_t xReturn;
        console_print("------(B)MainState------\n");

        while ((xQueueReceive(*xMainQueue, &msg, pdMS_TO_TICKS(100))) == pdPASS)
        {
            msg.timeStatus = STATUS_TIME_OK;
            switch (msg.id)
            {
            case motor_tempId: // motor temp
                mainState.motorTemp = msg;
                break;
            case inverter_tempId: // inverter temp
                mainState.inverterTemp = msg;
                break;
            case voltageId: // voltage message
                mainState.Voltage = msg;
                break;
            case calculatedSpeedId: // calculated speed
                mainState.speed = msg;
                break;
            case processedThrottleId: // processed throttle value
                mainState.ThrottleCommand = msg;
                break;
            }
            console_print("Received from queue: id=%x, timestamp=%lu\n", msg.id, msg.timestamp);
        }

        if (mainState.lastCallTimeStmp - xTaskGetTickCount() > pdMS_TO_TICKS(5))
        {
            float throttle = ProcessThrottle(mainState.ThrottleCommand.data, mainState.speed.data, mainState.motorTemp.data, mainState.inverterTemp.data, mainState.Voltage.data);
            dataMessage_t throttleMsg = {
                .id = 0x50,                   // Throttle command message ID
                .data = FP_FROMFLT(throttle), // Convert to percentage and scale
                .timestamp = xTaskGetTickCount()

            };

            xQueueSend(*xCanTxQueue, &throttleMsg, portMAX_DELAY);
            console_print("Calculated throttle command: %f\n", throttle);
        }

        PrintMainState(&mainState);

        console_print("------(E)MainState------\n\n");
        mainState.lastCallTimeStmp = xTaskGetTickCount();
    }

    // xTaskDelayUntil(&timeNow, pdMS_TO_TICKS(1000));
}

// function, to print current state of the motor controller (for debug)
void PrintMainState(const MainState_t *mainState)
{
    console_print("Main State:\n");
    console_print("  Throttle Command: %lu\n", mainState->ThrottleCommand.data);
    console_print("  Speed: %lu\n", mainState->speed.data);
    console_print("  Direction: %s\n", mainState->direction ? "Forward" : "Reverse");
    console_print("  Brake Pedal Pressed: %s\n", mainState->brakePedalPressed ? "Yes" : "No");
    console_print("  Motor Temp: %d (timestamp: %lu)\n", mainState->motorTemp.data, mainState->motorTemp.timestamp);
    console_print("  Inverter Temp: %d (timestamp: %lu)\n", mainState->inverterTemp.data, mainState->inverterTemp.timestamp);
    console_print("  Voltage: %d (timestamp: %lu)\n", mainState->Voltage.data, mainState->Voltage.timestamp);
}