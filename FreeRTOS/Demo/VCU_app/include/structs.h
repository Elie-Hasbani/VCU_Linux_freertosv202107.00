#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint8_t data[8];
    uint8_t length;
    uint32_t timestamp;

} CanMessage_t;