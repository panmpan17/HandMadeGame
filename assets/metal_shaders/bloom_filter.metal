#include <metal_stdlib>
#include "common.metal"

fragment float4 bloomFilter_fragmentMain(VertexOut_Image in [[stage_in]],
                                             constant float& u_threshold [[buffer(1)]],
                                             metal::texture2d<float> texture0 [[texture(0)]],
                                             metal::sampler textureSampler [[sampler(0)]])
{
    in.texCoord.y = 1.0 - in.texCoord.y;
    metal::float3 rgb = texture0.sample(textureSampler, in.texCoord).rgb;

    float colorMax = metal::max(metal::max(rgb.r, rgb.g), rgb.b);
    if (colorMax < u_threshold)
        rgb = float3(0, 0, 0);
    
    return float4(rgb, 1.0);
}