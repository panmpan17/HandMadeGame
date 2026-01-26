#include <metal_stdlib>

struct VertexIn_Position {
    metal::float3 position [[attribute(0)]];
};

struct VertexIn_Position2DAndUV {
    float2 position [[attribute(0)]];
    float2 texCoord [[attribute(1)]];
};

struct Uniform_ModelMatrix {
    metal::float4x4 modelMatrix;
};

struct Uniform_Blur
{
    metal::int2 sizeAndBlurRadius;
    float sigma;
};

struct VertexOut_Position {
    metal::float4 position [[position]];
};

struct VertexOut_Image {
    metal::float4 position [[position]];
    metal::float2 texCoord;
    metal::float4 color;
};

struct VertexOut_CubeMap {
    metal::float4 position [[position]];
    metal::float3 textCoord;
};
