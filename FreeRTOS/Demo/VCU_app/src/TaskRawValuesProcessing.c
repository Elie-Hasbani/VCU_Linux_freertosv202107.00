#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "structs.h"

#include "console.h"
#include "TaskRawValuesProcessing.h"

#include "utils.h"
#include "project_config.h"

void TaskTmpVltMngr(void *pvParameters)
{
    TempratureVoltageState_t tempState = {0};
    tempState.inverterTempChanged = false;
    tempState.motorTempChanged = false;

    TickType_t timeNow = xTaskGetTickCount();

    TmpVltMngrParams_t *params = (TmpVltMngrParams_t *)pvParameters;
    GlobalState_t *globalState = params->globalState;
    QueueHandle_t *xTemperatureVoltageQueue = params->xTemperatureVoltageQueue;
    QueueHandle_t *xIHMQueue = params->IHMQueue;

    while (1)
    {
        // console_print((ledState = !ledState) ? "Led2 ON\n" : "Led2 OFF\n");
        dataMessage_t msg = {0};
        BaseType_t xReturn = xQueueReceive(*xTemperatureVoltageQueue, &msg, pdMS_TO_TICKS(500));
        console_print("------(B)TempVltController------\n");

        while ((xQueueReceive(*xTemperatureVoltageQueue, &msg, pdMS_TO_TICKS(100))) == pdPASS)
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
            RegulateTemprature(globalState, &tempState);
            if (tempState.motorTempChanged)
            {
                IHMOrder_t order = {CoolingPump, tempState.pumpsOrder};
                xQueueSend(*xIHMQueue, &order, portMAX_DELAY);
                console_print("Motor temp changed, sending order to IHM: %s\n", tempState.pumpsOrder ? "ON" : "OFF");
            }

            if (tempState.inverterTempChanged)
            {
                IHMOrder_t order = {CoolingFans, tempState.fansOrder};
                xQueueSend(*xIHMQueue, &order, portMAX_DELAY);
                console_print("Inverter temp changed, sending order to IHM: %s\n", tempState.fansOrder ? "ON" : "OFF");
            }
            tempState.motorTempChanged = false;
            tempState.inverterTempChanged = false;
        }

        console_print("------(E)TempVltController------\n\n");
        tempState.lastCallTimeStmp = xTaskGetTickCount();

        // xTaskDelayUntil(&timeNow, pdMS_TO_TICKS(1000));
    }
}