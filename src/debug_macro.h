#include <print>

#define IS_DEBUG_VERSION 1


#ifdef IS_DEBUG_VERSION

#define DEBUG_PRINT(msg) std::println(msg)
#define DEBUG_PRINT_EX(msg, ...) std::println(msg, __VA_ARGS__)

#else

#define DEBUG_PRINT(msg) do {} while (0)
#define DEBUG_PRINT_EX(msg, ...) do {} while (0)

#endif