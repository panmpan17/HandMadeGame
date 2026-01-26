#include <metal_stdlib>
#include "common.metal"

fragment float4 horizontalBlur_fragmentMain(VertexOut_Image in [[stage_in]],
                                            constant Uniform_Blur& uniform [[buffer(1)]],
                                            metal::texture2d<float> texture0 [[texture(0)]],
                                            metal::sampler textureSampler [[sampler(0)]])
{
    in.texCoord.y = 1.0 - in.texCoord.y;

    metal::float2 texelSize = metal::float2(1.0 / float(uniform.sizeAndBlurRadius.x), 0);
    metal::float3 sum = metal::float3(0.0);
    float weightSum = 0.0;

    int min = -metal::max(20, uniform.sizeAndBlurRadius.y);
    int max = metal::max(20, uniform.sizeAndBlurRadius.y);

    for (int i = min; i <= max; i++)
    {
        float weight = metal::exp((-0.5 * (i * i)) / float(uniform.sigma * uniform.sigma));
        metal::float2 newUV = metal::float2(float(i)) * texelSize + in.texCoord;

        if (newUV.x < 0.0 || newUV.x > 1.0)
        {
            continue;
        }

        sum += texture0.sample(textureSampler, newUV).rgb * weight;
        weightSum += weight;
    }

    return float4(sum / weightSum, 1.0);
}