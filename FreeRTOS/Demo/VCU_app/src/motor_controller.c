#include "FreeRTOS.h"
#include "queue.h"

#include "structs.h"

#include "console.h"
#include "motor_controller.h"
#include "queue_handles.h"

void MotorController(void *pvParameters)
{
    TickType_t lastWakeUp = xTaskGetTickCount();

    while (1)
    {
        // console_print((ledState = !ledState) ? "Led2 ON\n" : "Led2 OFF\n");
        CanMessage_t msg = {0};
        BaseType_t xReturn = xQueueReceive(xMainQueue, &msg, pdMS_TO_TICKS(500));
        if (xReturn == pdPASS)
        {
            console_print("Received from queue: id=%lu, timestamp=%lu\n", msg.id, msg.timestamp);
        }
        else
        {
            console_print("Failed to receive from queue\n");
        }
        // xTaskDelayUntil(&lastWakeUp, pdMS_TO_TICKS(1000));
    }
}