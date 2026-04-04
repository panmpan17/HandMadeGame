#include <metal_stdlib>
#include "common.metal"
using namespace metal;

struct image_Uniforms {
    float4x4 MVPMatrix;
    float4 color;
    int spriteSheetCountX;
    int spriteSheetCountY;
    float2 uvOffset;
};


vertex VertexOut_Image image_vertexMain(VertexIn_Position2DAndUV in [[stage_in]],
                                        constant image_Uniforms& uniforms [[buffer(2)]]) {
    VertexOut_Image out;
    out.position = uniforms.MVPMatrix * float4(in.position, 0.0, 1.0);
    out.texCoord = metal::float2((in.texCoord.x / float(uniforms.spriteSheetCountX)) + uniforms.uvOffset.x,
                                 (in.texCoord.y / float(uniforms.spriteSheetCountY)) + uniforms.uvOffset.y);
    return out;
}

fragment float4 image_fragmentMain(VertexOut_Image in [[stage_in]],
                                   texture2d<float> colorTexture [[texture(0)]],
                                   sampler textureSampler [[sampler(0)]],
                                   constant image_Uniforms& uniforms [[buffer(2)]]) {
    return colorTexture.sample(textureSampler, in.texCoord) * uniforms.color;
}

//  xcrun -sdk macosx metal -o assets/metal_shaders/colored_vertices.ir  -c assets/metal_shaders/colored_vertices.metal
//  xcrun -sdk macosx metal -o assets/metal_shaders.metallib assets/metal_shaders/colored_vertices.ir