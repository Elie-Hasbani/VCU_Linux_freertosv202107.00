
#include "FreeRTOS.h"
#include "queue.h"

#include "structs.h"

#include "console.h"
#include "TaskCanTx.h"

void TaskCanTx(void *pvParameters)
{
    TickType_t lastWakeUp = xTaskGetTickCount();
    unsigned long i = 0;
    CanMessage_t msg = {0};

    CanTxParam_t *params = (CanTxParam_t *)pvParameters;
    QueueHandle_t *xCanTxQueue = params->xCanTxQueue;

    while (1)
    {

        msg.id++;
        msg.timestamp = xTaskGetTickCount();

        CanMessage_t msg = {0};
        BaseType_t xReturn = xQueueReceive(*xCanTxQueue, &msg, portMAX_DELAY);
        console_print("------CanTX------\n");
        if (xReturn == pdPASS)
        {
            console_print("sending Throttle Order = %d to inverter\n", msg.data);
        }
        console_print("------CanTX------\n\n");
    }
}