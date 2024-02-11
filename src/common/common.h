#pragma once

#ifndef CSL_COMMON_H
#define CSL_COMMON_H

#include <cassert>

void custom_printf(const char* format, ...);

#define PRINTF(format, _VA_ARGS_) custom_printf(format, _VA_ARGS_);

#endif // !COMMON_H