
/*#include "FreeRTOS.h"
#include "queue.h"

#include "structs.h"

#include "console.h"
#include "TaskCanRX.h"

void TaskCanRx(void *pvParameters)
{
    TickType_t lastWakeUp = xTaskGetTickCount();
    unsigned long i = 0;
    CanMessage_t msg = {0};

    CanRxParams_t *params = (CanRxParams_t *)pvParameters;
    QueueHandle_t *xTemperatureVoltageQueue = params->xTemperatureVoltageQueue;
    QueueHandle_t *xMotorControllerQueue = params->xMotorControllerQueue;

    while (1)
    {
        // console_print((ledState = !ledState) ? "Led ON\n" : "Led OFF\n");
        msg.id++;
        msg.timestamp = xTaskGetTickCount();
        xQueueSend(*xMotorControllerQueue, &msg, portMAX_DELAY);

        xTaskDelayUntil(&lastWakeUp, pdMS_TO_TICKS(1000));
    }
}*/