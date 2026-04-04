#pragma once


struct Color
{
    float r;
    float g;
    float b;
    float a;

    Color() : r(0), g(0), b(0), a(1) {}
    Color(float red, float green, float blue, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}
};
