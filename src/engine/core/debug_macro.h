#pragma once


#include <format>
#include <iostream>
#include "../../utils/platform.h"
#include <ctime>

#if !IS_PLATFORM_WINDOWS
#include <print>
#endif


#ifndef DEBUG_FLAG
#define DEBUG_FLAG 1
#endif

#define IS_DEBUG_VERSION (DEBUG_FLAG == 1)


#if IS_DEBUG_VERSION && !IS_PLATFORM_WINDOWS

#define LOG(msg, ...) std::print(msg __VA_OPT__(,) __VA_ARGS__)
#define LOGLN(msg, ...) std::println(msg __VA_OPT__(,) __VA_ARGS__)

#define LOGERR(msg, ...) std::cout << "\033[91m" << std::format(msg __VA_OPT__(,) __VA_ARGS__) << "\033[0m" << std::endl;

#define ASSERT(condition, msg) \
    do { \
        if (!(condition)) { \
            std::println("Assertion failed: {}", msg); \
            exit(1); \
        } \
    } while (0)

// A macro to make it easy to use
// #define GL_CHECK() \
//     do { \
//         GLenum error;\
//         while ((error = glGetError()) != GL_NO_ERROR) {\
//             std::println("OpenGL Error: {} at {}:{}", error, __FILE__, __LINE__);\
//         }\
//     } while (0)

#define DEBUG_START_TIMER() std::clock_t oDebugTimedStart = std::clock();
#define DEBUG_END_TIMER(strTag) LOGLN("{}, Time: {} ms", strTag, (std::clock() - oDebugTimedStart) / (double)(CLOCKS_PER_SEC / 1000)); oDebugTimedStart = std::clock();
#define DEBUG_REFRESH_TIMER() oDebugTimedStart = std::clock();

#else

#define LOG(msg, ...) do {} while (0)
#define LOGLN(msg, ...) do {} while (0)

#define ASSERT(condition, msg) do {} while (0)

#define LOGERR(msg, ...) do {} while (0)

#define DEBUG_START_TIMER() do {} while (0)
#define DEBUG_END_TIMER(strTag) do {} while (0)
#define DEBUG_REFRESH_TIMER() do {} while (0)

#endif