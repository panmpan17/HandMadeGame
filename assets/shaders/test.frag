#version 330

#include "assets/shaders/base/voronoi.glsl"
#include "assets/shaders/base/time_data.glsl"

uniform sampler2D u_perlinNoiseTex;

in vec2 fragUV;
out vec4 fragment;

vec3 lerp(vec3 a, vec3 b, float f)
{
    return a + (b - a) * f;
}

vec3 lerpClamp(vec3 a, vec3 b, float f)
{
    f = clamp(f, 0.0, 1.0);
    return a + (b - a) * f;
}

void main()
{
    vec3 baseColor = vec3(0.19564591, 0.6502349, 0.86764705);
    vec3 waveColor = vec3(0.50026435, 0.7952124, 0.9362745);

    float waveStrength = voronoi(fragUV * 10, u_Time);

    float perlinValue = texture(u_perlinNoiseTex, fragUV).r;
    waveStrength *= perlinValue;

    fragment = vec4(lerpClamp(baseColor, waveColor, waveStrength), 1);
}
