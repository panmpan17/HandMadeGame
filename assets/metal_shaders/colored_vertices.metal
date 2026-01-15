#include <metal_stdlib>
using namespace metal;

struct coloredVertices_VertexIn {
    float2 position [[attribute(0)]];
    float3 color [[attribute(1)]];
};

struct coloredVertices_Uniforms {
    float4x4 MVPMatrix;
};

struct coloredVertices_VertexOut {
    float4 position [[position]];
    float3 color;
};


vertex coloredVertices_VertexOut coloredVertices_vertexMain(coloredVertices_VertexIn in [[stage_in]], constant coloredVertices_Uniforms& uniforms [[buffer(2)]]) {
    coloredVertices_VertexOut out;
    out.position = uniforms.MVPMatrix * float4(in.position, 0.0, 1.0);
    out.color = in.color;
    return out;
}

fragment float4 coloredVertices_fragmentMain(coloredVertices_VertexOut in [[stage_in]]) {
    return float4(in.color, 1.0);
}

//  xcrun -sdk macosx metal -o assets/metal_shaders/colored_vertices.ir  -c assets/metal_shaders/colored_vertices.metal
//  xcrun -sdk macosx metal -o assets/metal_shaders.metallib assets/metal_shaders/colored_vertices.ir