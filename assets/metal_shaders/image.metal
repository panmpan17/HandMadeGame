#include <metal_stdlib>
using namespace metal;

struct image_VertexIn {
    float2 position [[attribute(0)]];
    float2 texCoord [[attribute(1)]];
};

struct image_Uniforms {
    float4x4 MVPMatrix;
    float4 color;
};

struct image_VertexOut {
    float4 position [[position]];
    float2 texCoord;
};


vertex image_VertexOut image_vertexMain(image_VertexIn in [[stage_in]],
                                        constant image_Uniforms& uniforms [[buffer(2)]]) {
    image_VertexOut out;
    out.position = uniforms.MVPMatrix * float4(in.position, 0.0, 1.0);
    out.texCoord = in.texCoord;
    return out;
}

fragment float4 image_fragmentMain(image_VertexOut in [[stage_in]],
                                   texture2d<float> colorTexture [[texture(0)]],
                                   sampler textureSampler [[sampler(0)]],
                                   constant image_Uniforms& uniforms [[buffer(2)]]) {
    return colorTexture.sample(textureSampler, in.texCoord) * uniforms.color;
}

//  xcrun -sdk macosx metal -o assets/metal_shaders/colored_vertices.ir  -c assets/metal_shaders/colored_vertices.metal
//  xcrun -sdk macosx metal -o assets/metal_shaders.metallib assets/metal_shaders/colored_vertices.ir