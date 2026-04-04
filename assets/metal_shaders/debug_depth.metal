#include <metal_stdlib>
#include "common.metal"

fragment float4 debugDepth_fragmentMain(VertexOut_Image in [[stage_in]],
                                          metal::texture2d<float> texture0 [[texture(0)]],
                                          metal::sampler textureSampler [[sampler(0)]])
{
    in.texCoord.y = 1.0 - in.texCoord.y;
    float textureValue = texture0.sample(textureSampler, in.texCoord).r;

    // if (textureValue >= 1)
    // {
    //     return float4(0.2, 0, 0, 1.0);
    // }

    // float visibleDepth = metal::pow(textureValue, 50.0);
    return float4(textureValue, textureValue, textureValue, 1.0);
}