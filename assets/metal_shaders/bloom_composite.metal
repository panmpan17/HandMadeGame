#include <metal_stdlib>
#include "common.metal"

fragment float4 bloomComposite_fragmentMain(VertexOut_Image in [[stage_in]],
                                            metal::texture2d<float> originalScreenTexture [[texture(0)]],
                                            metal::texture2d<float> bloomTexture [[texture(1)]],
                                            constant metal::float2& bloomTextureScaleAndIntensity [[buffer(1)]],
                                            metal::sampler textureSampler [[sampler(0)]])
{
    in.texCoord.y = 1.0 - in.texCoord.y;

    metal::float3 originalColor = originalScreenTexture.sample(textureSampler, in.texCoord).rgb;

    metal::float2 bloomTexCoord = in.texCoord * bloomTextureScaleAndIntensity.x;
    metal::float3 bloomColor = bloomTexture.sample(textureSampler, bloomTexCoord).rgb;
    
    return float4(bloomColor + (originalColor * bloomTextureScaleAndIntensity.y), 1.0);
}