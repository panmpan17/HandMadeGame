#include <metal_stdlib>
using namespace metal;

struct image_VertexIn {
    float2 position [[attribute(0)]];
    float2 texCoord [[attribute(1)]];
};

struct image_Uniforms {
    float4x4 MVPMatrix;
};

struct image_VertexOut {
    float4 position [[position]];
    float4 color;
};


vertex image_VertexOut image_vertexMain(image_VertexIn in [[stage_in]], constant image_Uniforms& uniforms [[buffer(2)]]) {
    image_VertexOut out;
    out.position = uniforms.MVPMatrix * float4(in.position, 0.0, 1.0);
    out.color = float4(in.texCoord, 0.0, 1.0);
    return out;
}

fragment float4 image_fragmentMain(image_VertexOut in [[stage_in]]) {
    return in.color;
}

//  xcrun -sdk macosx metal -o assets/metal_shaders/colored_vertices.ir  -c assets/metal_shaders/colored_vertices.metal
//  xcrun -sdk macosx metal -o assets/metal_shaders.metallib assets/metal_shaders/colored_vertices.ir