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

struct PointLight // 32
{
    vec4 positionAndRange; // 16
    vec3 color; // 12 + 4
};

layout(std140) uniform LightData // 408
{
    vec3 u_AmbientLightColor; // 12 + 4
    
    DirectionLight u_SunLights[4]; // 32 * 4 = 128

    PointLight u_PointLights[8]; // 32 * 8 = 256

    ivec2 u_LightCounts; // 8 + 8
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

    for (int i = 0; i < u_LightCounts.x; i++)
    {
        vec3 lightDir = normalize(-u_SunLights[i].direction);
        float diff = max(dot(norm, lightDir), 0.0);
        diffuseSum += diff * u_SunLights[i].color;
    }

    for (int i = 0; i < u_LightCounts.y; i++)
    {
        vec3 lightDir = normalize(u_PointLights[i].positionAndRange.xyz - fragPos);
        float distance = length(u_PointLights[i].positionAndRange.xyz - fragPos);
        if (distance < u_PointLights[i].positionAndRange.w)
        {
            float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance)); // Example attenuation
            float diff = max(dot(norm, lightDir), 0.0);
            diffuseSum += diff * u_PointLights[i].color * attenuation;
        }
    }

    fragment = vec4(diffuseSum + u_AmbientLightColor, 1);
}