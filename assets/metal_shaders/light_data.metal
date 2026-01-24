#include <metal_stdlib>

struct DirectionLight
{
    metal::float4 direction; // 12 + 4
    metal::float4 color;     // 12 + 4
};

struct PointLight
{
    metal::float4 positionAndRange; // 16
    metal::float4 color;            // 12 + 4
    metal::float4 attenuation;      // 12 + 4
};

struct LightData
{
    metal::float3 ambientColor;           // 12 + 4
    DirectionLight   directionLights[4];  // 32 * 16 = 64
    PointLight   pointLights[8];          // 48 * 8 = 384
    metal::int2 lightCounts;              // 8 + 8
};
