// Mock FreeRTOS header for testing
#ifndef FREERTOS_MOCK_H
#define FREERTOS_MOCK_H

#include <stdint.h>

// Mock TickType_t
typedef uint32_t TickType_t;

// Basic FreeRTOS types that might be needed
#define pdTRUE 1
#define pdFALSE 0
#define pdPASS pdTRUE
#define pdFAIL pdFALSE

#endif // FREERTOS_MOCK_H
