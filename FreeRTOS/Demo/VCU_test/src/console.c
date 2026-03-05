#include "console.h"

// Mock console_print implementation for testing
// This is a simple wrapper around vprintf
void console_print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    // Uncomment below to see debug output during tests
    // vprintf(format, args);
    va_end(args);
}
