
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "structs.h"

#include "console.h"

#include "canRX.h"
#include "motor_controller.h"
#include "queue_handles.h"

void MainApp(void)
{
   xMainQueue = xQueueCreate(10, sizeof(CanMessage_t));
   xQueue2 = xQueueCreate(10, sizeof(unsigned long));

   xTaskCreate(
       TaskCanRx,
       "CAN_RX",
       configMINIMAL_STACK_SIZE,
       NULL,
       tskIDLE_PRIORITY,
       NULL);

   xTaskCreate(
       MotorController,
       "MOTOR_CONTROLLER",
       configMINIMAL_STACK_SIZE,
       NULL,
       tskIDLE_PRIORITY,
       NULL);

   vTaskStartScheduler();

   for (;;)
   {
   }
}
