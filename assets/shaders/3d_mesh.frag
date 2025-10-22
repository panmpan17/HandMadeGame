#version 330

// layout(std140) uniform CameraMatrices
// {
//     mat4 u_View;
//     mat4 u_Projection;
// };

struct DirectionLight // 32
{
    vec3 direction; // 12 + 4
    vec3 color; // 12 + 4
};

layout(std140) uniform LightData // 160
{
    vec3 u_AmbientLightColor; // 12 + 4
    
    DirectionLight u_SunLights[4]; // 32 * 4 = 128
    int u_NumSunLights; // 4 + 12
};

uniform sampler2D u_MainTex;

in vec2 fragUV;
in vec3 fragPos;
in vec3 fragNormal;

out vec4 fragment;

void main()
{
    vec3 norm = normalize(fragNormal);

    vec3 diffuseSum = vec3(0.0);

    for (int i = 0; i < u_NumSunLights; i++)
    {
        vec3 lightDir = normalize(-u_SunLights[i].direction);
        float diff = max(dot(norm, lightDir), 0.0);
        diffuseSum += diff * u_SunLights[i].color;
    }

    fragment = vec4(diffuseSum + u_AmbientLightColor, 1);
}