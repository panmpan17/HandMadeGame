#pragma once

#include <linmath.h>

float randomFloat();
float randomFloat(float fMin, float fMax);
int randomInt(int nMin, int nMax);

void randomBetweenVec2(vec2& vResult, const vec2& vMin, const vec2& vMax);
void randomBetweenVec3(vec3& vResult, const vec3& vMin, const vec3& vMax);
void randomBetweenVec4(vec4& vResult, const vec4& vMin, const vec4& vMax);
