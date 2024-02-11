#include "common.h"

#include <cstdarg>
#include <cstdio>

void custom_printf(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stdout, format, argptr);
    va_end(argptr);
}
