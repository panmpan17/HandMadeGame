#pragma once

#undef IS_PLATFORM_WINDOWS
#undef IS_PLATFORM_LINUX
#undef IS_PLATFORM_MACOS


#if defined(_WIN32) || defined(_WIN64)
#define IS_PLATFORM_WINDOWS true
#define IS_PLATFORM_LINUX false
#define IS_PLATFORM_MACOS false

#define M_PI 3.14159265358979323846

#elif defined(__linux__)
#define IS_PLATFORM_WINDOWS false
#define IS_PLATFORM_LINUX true
#define IS_PLATFORM_MACOS false

#define M_PI 3.14159265358979323846

#define SIZE_MAX 18446744073709551615UL

#elif defined(__APPLE__) || defined(__MACH__)
#define IS_PLATFORM_WINDOWS false
#define IS_PLATFORM_LINUX false
#define IS_PLATFORM_MACOS true
#else
#error "Unsupported platform"
#endif
