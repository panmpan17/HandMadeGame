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


vertex VertexOut coloredVertices_vertexMain(uint vertexID [[vertex_id]],
                            constant float2* positions [[buffer(0)]],
                            constant packed_float3* colors [[buffer(1)]]) {
    VertexOut out;
    out.position = float4(positions[vertexID], 0.0, 1.0);
    out.color = colors[vertexID];
    return out;
}

fragment float4 coloredVertices_fragmentMain(VertexOut in [[stage_in]]) {
    return float4(in.color, 1.0);
}

//  xcrun -sdk macosx metal -o assets/metal_shaders/colored_vertices.ir  -c assets/metal_shaders/colored_vertices.metal
//  xcrun -sdk macosx metal -o assets/metal_shaders.metallib assets/metal_shaders/colored_vertices.ir