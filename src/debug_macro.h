#pragma once

#include <print>

#ifndef DEBUG_FLAG
#define DEBUG_FLAG 1
#endif

#define IS_DEBUG_VERSION (DEBUG_FLAG == 1)


#if IS_DEBUG_VERSION

#define LOG(msg) std::print(msg)
#define LOG_EX(msg, ...) std::print(msg, __VA_ARGS__)

#define LOGLN(msg) std::println(msg)
#define LOGLN_EX(msg, ...) std::println(msg, __VA_ARGS__)

#else

#define LOG(msg) do {} while (0)
#define LOG_EX(msg, ...) do {} while (0)

#define LOGLN(msg) do {} while (0)
#define LOGLN_EX(msg, ...) do {} while (0)

#endif