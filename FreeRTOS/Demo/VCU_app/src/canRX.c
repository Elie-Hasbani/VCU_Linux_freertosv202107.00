
#include "FreeRTOS.h"
#include "queue.h"

#include "structs.h"

#include "console.h"
#include "canRX.h"
#include "queue_handles.h"
void TaskCanRx(void *pvParameters)
{
    TickType_t lastWakeUp = xTaskGetTickCount();
    unsigned long i = 0;
    CanMessage_t msg = {0};

    while (1)
    {
        // console_print((ledState = !ledState) ? "Led ON\n" : "Led OFF\n");
        msg.id++;
        msg.timestamp = xTaskGetTickCount();
        xQueueSend(xMainQueue, &msg, portMAX_DELAY);

        xTaskDelayUntil(&lastWakeUp, pdMS_TO_TICKS(1000));
    }
}