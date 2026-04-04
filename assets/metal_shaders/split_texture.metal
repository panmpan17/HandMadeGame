#include <metal_stdlib>
#include "common.metal"

fragment float4 splitTexture_fragmentMain(VertexOut_Image in [[stage_in]],
                                            metal::texture2d<float> texture1 [[texture(0)]],
                                            metal::texture2d<float> texture2 [[texture(1)]],
                                            constant float& splitFactor [[buffer(1)]],
                                            metal::sampler textureSampler [[sampler(0)]])
{
    in.texCoord.y = 1.0 - in.texCoord.y;

    if (in.texCoord.x < splitFactor)
    {
        metal::float3 color = texture1.sample(textureSampler, in.texCoord).rgb;
        return float4(color, 1.0);
    }
    else
    {
        metal::float3 color = texture2.sample(textureSampler, in.texCoord).rgb;
        return float4(color, 1.0);
    }
}