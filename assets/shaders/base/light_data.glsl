struct DirectionLight // 32
{
    vec4 direction; // 12 + 4
    vec4 color; // 12 + 4
};

struct PointLight // 48
{
    vec4 positionAndRange; // 16
    vec4 color; // 12 + 4
    vec4 attenuationParams; // 12 + 4
};

layout(std140) uniform LightData // 544
{
    vec3 u_AmbientLightColor; // 12 + 4
    
    DirectionLight u_DirectionLights[4]; // 32 * 4 = 128

    PointLight u_PointLights[8]; // 48 * 8 = 384

    ivec2 u_LightCounts; // 16 + 16
};