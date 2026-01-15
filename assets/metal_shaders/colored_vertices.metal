#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float2 position [[attribute(0)]];
    float3 color [[attribute(1)]];
};

struct VertexOut {
    float4 position [[position]];
    float3 color;
};


vertex VertexOut coloredVertices_vertexMain(VertexIn in [[stage_in]]) {
    VertexOut out;
    out.position = float4(in.position, 0.0, 1.0);
    out.color = in.color;
    return out;
}

fragment float4 coloredVertices_fragmentMain(VertexOut in [[stage_in]]) {
    return float4(in.color, 1.0);
}

//  xcrun -sdk macosx metal -o assets/metal_shaders/colored_vertices.ir  -c assets/metal_shaders/colored_vertices.metal
//  xcrun -sdk macosx metal -o assets/metal_shaders.metallib assets/metal_shaders/colored_vertices.ir