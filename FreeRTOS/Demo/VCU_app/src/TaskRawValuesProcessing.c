#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "structs.h"

#include "console.h"
#include "TaskRawValuesProcessing.h"

#include "utils.h"
#include "project_config.h"

void TaskRawValuesProcessing(void *pvParameters)
{
    TempratureVoltageState_t tempState = {0};
    tempState.inverterTempChanged = false;
    tempState.motorTempChanged = false;

    TickType_t timeNow = xTaskGetTickCount();

    TRVPParams_t *params = (TRVPParams_t *)pvParameters;
    QueueHandle_t *xTRVPQueue = params->xTRVPQueue;

    while (1)
    {
        // console_print((ledState = !ledState) ? "Led2 ON\n" : "Led2 OFF\n");
        dataMessage_t msg = {0};
        console_print("------(B)TempVltController------\n");

        while ((xQueueReceive(*xTRVPQueue, &msg, pdMS_TO_TICKS(100))) == pdPASS)
        {
            switch (msg.id)
            {
            case motor_tempId: // motor temp
                tempState.motorTemp = msg;
                break;
            case inverter_tempId: // inverter temp
                tempState.inverterTemp = msg;
                break;
            case voltageId: // voltage message
                tempState.Voltage = msg;
                break;
            }

            console_print("Received from queue: id=%lu, timestamp=%lu\n", msg.id, msg.timestamp);
        }

        if (tempState.lastCallTimeStmp - xTaskGetTickCount() > pdMS_TO_TICKS(5))
        {
        }

        console_print("------(E)TempVltController------\n\n");
        tempState.lastCallTimeStmp = xTaskGetTickCount();

        // xTaskDelayUntil(&timeNow, pdMS_TO_TICKS(1000));
    }
}