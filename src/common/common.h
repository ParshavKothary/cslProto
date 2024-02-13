#pragma once

#ifndef CSL_COMMON_H
#define CSL_COMMON_H

#include <cassert>

void custom_printf(const char* format, ...);

#define PRINTF(...) custom_printf(__VA_ARGS__);

#endif // !COMMON_H