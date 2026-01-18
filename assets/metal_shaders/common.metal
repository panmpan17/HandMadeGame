#include <metal_stdlib>

struct VertexIn_Position {
    metal::float3 position [[attribute(0)]];
};

struct Uniform_ModelMatrix {
    metal::float4x4 modelMatrix;
};

struct VertexOut_Position {
    metal::float4 position [[position]];
};

struct VertexOut_CubeMap {
    metal::float4 position [[position]];
    metal::float3 textCoord;
};
