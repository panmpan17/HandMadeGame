#include <metal_stdlib>
#include "common.metal"

vertex VertexOut_Image gammaCorrection_vertexMain(VertexIn_Position2DAndUV in [[stage_in]])
{
    VertexOut_Image out;
    out.position = metal::float4(in.position, 0.0, 1.0);
    out.texCoord = in.texCoord;
    return out;
}

fragment float4 gammaCorrection_fragmentMain(VertexOut_Image in [[stage_in]],
                                             constant float& gammaValue [[buffer(1)]],
                                             metal::texture2d<float> texture0 [[texture(0)]],
                                             metal::sampler textureSampler [[sampler(0)]])
{
    in.texCoord.y = 1.0 - in.texCoord.y;
    metal::float3 rgb = texture0.sample(textureSampler, in.texCoord).rgb;

    return metal::float4(metal::pow(rgb, metal::float3(1.0 / gammaValue)), 1.0);
}