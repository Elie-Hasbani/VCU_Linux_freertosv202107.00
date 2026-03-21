// Mock FreeRTOS header for testing
#ifndef FREERTOS_MOCK_H
#define FREERTOS_MOCK_H

#include <stdint.h>

// Mock TickType_t
typedef uint32_t TickType_t;

// Basic FreeRTOS types that might be needed
typedef int32_t BaseType_t;
typedef uint32_t UBaseType_t;
typedef void *QueueHandle_t;
typedef void *TaskHandle_t;

#define pdTRUE 1
#define pdFALSE 0
#define pdPASS pdTRUE
#define pdFAIL pdFALSE

#define PRIVILEGED_FUNCTION

#endif // FREERTOS_MOCK_H
