#pragma once


#include <format>
#include <iostream>
#include "../../utils/platform.h"
#include <ctime>


#if !defined(DEBUG_FLAG) && !(__WIN32__ || __WIN64__)
#define DEBUG_FLAG 1
#endif

#define IS_DEBUG_VERSION (DEBUG_FLAG == 1)


#if IS_DEBUG_VERSION

#define LOG(msg, ...) std::cout << std::format(msg __VA_OPT__(,) __VA_ARGS__);
#define LOGLN(msg, ...) std::cout << std::format(msg __VA_OPT__(,) __VA_ARGS__) << std::endl;

#define LOGERR(msg, ...) std::cout << "\033[91m" << std::format(msg __VA_OPT__(,) __VA_ARGS__) << "\033[0m" << std::endl;

#define ASSERT(condition, msg) \
    do { \
        if (!(condition)) { \
            std::cout << "Assertion failed: " << msg << std::endl; \
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


bool Profiler_CheckTagIsInAllow(const std::string_view& strTag);
bool Profiler_CheckTagIsInIgnore(const std::string_view& strTag);

#define PROFILER_CHECK_TAG_COULD_PRINT(strTag) (Profiler_CheckTagIsInAllow(strTag) && !Profiler_CheckTagIsInIgnore(strTag))
#define PROFILER_START_TIMER() std::clock_t oDebugTimedStart = std::clock();
#define PROFILER_END_TIMER(strTag, strMsg) if (PROFILER_CHECK_TAG_COULD_PRINT(strTag)) LOGLN("{}: {}, Time: {} ms", strTag, strMsg, (std::clock() - oDebugTimedStart) / (double)(CLOCKS_PER_SEC / 1000)); oDebugTimedStart = std::clock();
#define PROFILER_FRESH_TIMER() oDebugTimedStart = std::clock();

#else

#define LOG(msg, ...) do {} while (0)
#define LOGLN(msg, ...) do {} while (0)

#define ASSERT(condition, msg) do {} while (0)

#define LOGERR(msg, ...) do {} while (0)

#define PROFILER_START_TIMER() do {} while (0)
#define PROFILER_END_TIMER(strTag, strMsg) do {} while (0)
#define PROFILER_FRESH_TIMER() do {} while (0)

#endif