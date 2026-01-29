#include <metal_stdlib>
#include "common.metal"

fragment float4 pureTexture_fragmentMain(VertexOut_Image in [[stage_in]],
                                          metal::texture2d<float> texture0 [[texture(0)]],
                                          metal::sampler textureSampler [[sampler(0)]])
{
    in.texCoord.y = 1.0 - in.texCoord.y;
    return texture0.sample(textureSampler, in.texCoord);
}