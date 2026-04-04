#include "debug_macro.h"

const std::string_view g_arrDebugTagAllow[] = {
    // "World",
};
constexpr int g_DebugTagAllowLength = sizeof(g_arrDebugTagAllow) / sizeof(std::string_view);

const std::string_view g_arrDebugTagIgnore[] = {
    "Initialization",
};
constexpr int g_DebugTagIgnoreLength = sizeof(g_arrDebugTagIgnore) / sizeof(std::string_view);


bool Profiler_CheckTagIsInAllow(std::string_view strTag)
{
    if (g_DebugTagAllowLength == 0)
    {
        return true;
    }

    for (int i = 0; i < g_DebugTagAllowLength; i++)
    {
        if (strTag == g_arrDebugTagAllow[i])
        {
            return true;
        }
    }

    return false;
}

bool Profiler_CheckTagIsInIgnore(std::string_view strTag)
{
    for (int i = 0; i < g_DebugTagIgnoreLength; i++)
    {
        if (strTag == g_arrDebugTagIgnore[i])
        {
            return true;
        }
    }

    return false;
}