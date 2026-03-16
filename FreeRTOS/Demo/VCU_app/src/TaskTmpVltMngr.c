#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "structs.h"

#include "console.h"
#include "TaskTmpVltMngr.h"

#include "utils.h"

void TaskTmpVltMngr(void *pvParameters)
{
    TempratureVoltageState_t tempState = {0};

    TickType_t timeNow = xTaskGetTickCount();

    TmpVltMngrParams_t *params = (TmpVltMngrParams_t *)pvParameters;
    GlobalState_t *globalState = params->globalState;
    QueueHandle_t *xTemperatureVoltageQueue = params->xTemperatureVoltageQueue;
    QueueHandle_t *xIHMQueue = params->IHMQueue;

    while (1)
    {
        // console_print((ledState = !ledState) ? "Led2 ON\n" : "Led2 OFF\n");
        CanMessage_t msg = {0};
        BaseType_t xReturn = xQueueReceive(*xTemperatureVoltageQueue, &msg, pdMS_TO_TICKS(500));
        console_print("------TempVltController------\n");

        if (xReturn == pdPASS)
        {
            switch (msg.id)
            {
            case 0x100: // Wheel1 speed message
                tempState.motorTemp = msg;
                break;
            case 0x101: // Wheel2 speed message
                tempState.inverterTemp = msg;
                break;
            case 0x102: // Wheel3 speed message
                tempState.Voltage = msg;
                break;
            }

            console_print("Received from queue: id=%lu, timestamp=%lu\n", msg.id, msg.timestamp);
        }
        else
        {
            console_print("Failed to receive from queue\n");
        }

        if (tempState.lastCallTimeStmp - xTaskGetTickCount() > pdMS_TO_TICKS(5))
        {
            RegulateTemprature(globalState, tempState);
        }

        console_print("------TempVltController------\n\n");
        tempState.lastCallTimeStmp = xTaskGetTickCount();

        // xTaskDelayUntil(&timeNow, pdMS_TO_TICKS(1000));
    }
}