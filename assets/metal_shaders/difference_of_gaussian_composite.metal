#include <metal_stdlib>
#include "common.metal"

fragment float4 differenceOfGaussianComposite_fragmentMain(VertexOut_Image in [[stage_in]],
                                                           metal::texture2d<float> originalScreenTexture [[texture(0)]],
                                                           metal::texture2d<float> blurTexture1 [[texture(1)]],
                                                           metal::texture2d<float> blurTexture2 [[texture(2)]],
                                                           metal::sampler textureSampler [[sampler(0)]])
{
    in.texCoord.y = 1.0 - in.texCoord.y;

    metal::float3 blurColor1 = blurTexture1.sample(textureSampler, in.texCoord).rgb;
    metal::float3 blurColor2 = blurTexture2.sample(textureSampler, in.texCoord).rgb;

    metal::float3 difference = blurColor1 - blurColor2;
    // float 

    metal::float3 originalColor = originalScreenTexture.sample(textureSampler, in.texCoord).rgb;

    return float4(originalColor + (difference * 3), 1.0);
    // return float4(in.textc, 1.0);
}