#pragma once

// float abs(float fValue) { return fValue >= 0 ? fValue : -fValue; }


inline float moveTowards(float fCurrent, float fTarget, float fAmount)
{
    float fDelta = fTarget - fCurrent;
    if (std::abs(fDelta) < fAmount)
    {
        return fTarget;
    }

    return fDelta >= 0 ? fCurrent + fAmount : fCurrent - fAmount;
}

inline float moveTowards(float fCurrent, float fTarget, float fAmount, bool& bIsReached)
{
    float fDelta = fTarget - fCurrent;
    if (abs(fDelta) < fAmount)
    {
        bIsReached = true;
        return fTarget;
    }

    bIsReached = false;
    return fDelta >= 0 ? fCurrent + fAmount : fCurrent - fAmount;
}
