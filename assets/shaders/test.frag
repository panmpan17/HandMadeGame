#version 330

#include "assets/shaders/base/voronoi.glsl"
#include "assets/shaders/base/time_data.glsl"

in vec2 fragUV;
out vec4 fragment;

void main()
{
    vec3 baseColor = vec3(1.0, 0.0, 0.0);
    float waveColor = voronoi(fragUV * 10, u_Time);
    
    fragment = vec4(baseColor + vec3(waveColor), 1);
}
