#version 330

#include "assets/shaders/base/voronoi.glsl"

in vec2 fragUV;
out vec4 fragment;

void main()
{
    float fValue = voronoi(fragUV * 10, 0);
    fragment = vec4(fValue, fValue, fValue, 1);
}
