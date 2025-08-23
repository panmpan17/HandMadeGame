#include "random.h"
#include <random>
#include "platform.h"



static std::random_device rd;
static std::mt19937 gen(rd());
std::uniform_real_distribution<float> randomRange0To1(0.0f, 1.0f);


float randomFloat()
{
    return randomRange0To1(gen);
}

float randomFloat(float fMin, float fMax)
{
    if (fMin == fMax) return fMin;
    float fValue = randomRange0To1(gen);
    fValue = fValue * (fMax - fMin) + fMin;
    return fValue;
}

int randomInt(int nMin, int nMax)
{
    if (nMin == nMax) return nMin;
    std::uniform_int_distribution<int> randomRange(nMin, nMax);
    return randomRange(gen);
}

void randomBetweenVec2(vec2& vResult, const vec2& vMin, const vec2& vMax)
{
    vResult[0] = randomFloat(vMin[0], vMax[0]);
    vResult[1] = randomFloat(vMin[1], vMax[1]);
}

void randomBetweenVec3(vec3& vResult, const vec3& vMin, const vec3& vMax)
{
    vResult[0] = randomFloat(vMin[0], vMax[0]);
    vResult[1] = randomFloat(vMin[1], vMax[1]);
    vResult[2] = randomFloat(vMin[2], vMax[2]);
}

void randomBetweenVec4(vec4& vResult, const vec4& vMin, const vec4& vMax)
{
    vResult[0] = randomFloat(vMin[0], vMax[0]);
    vResult[1] = randomFloat(vMin[1], vMax[1]);
    vResult[2] = randomFloat(vMin[2], vMax[2]);
    vResult[3] = randomFloat(vMin[3], vMax[3]);
}

void randomOnUnitCircle(vec2& vResult)
{
    float angle = randomFloat(0.0f, 2.0f * M_PI);
    vResult[0] = cos(angle);
    vResult[1] = sin(angle);
}

void randomOnUnitSphere(vec3& vResult)
{
    float theta = randomFloat(0.0f, 2.0f * M_PI);
    float phi = randomFloat(0.0f, M_PI);
    vResult[0] = sin(phi) * cos(theta);
    vResult[1] = sin(phi) * sin(theta);
    vResult[2] = cos(phi);
}
