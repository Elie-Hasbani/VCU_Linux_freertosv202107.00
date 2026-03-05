// Mock console for testing - simple printf wrapper
#ifndef CONSOLE_MOCK_H
#define CONSOLE_MOCK_H

#include <stdio.h>
#include <stdarg.h>

// Mock console_print function
void console_print(const char *format, ...);

#endif // CONSOLE_MOCK_H
