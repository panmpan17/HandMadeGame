#include <metal_stdlib>
#include "common.metal"

vertex VertexOut_Image pureTexture_vertexMain(VertexIn_Position2DAndUV in [[stage_in]])
{
    VertexOut_Image out;
    out.position = metal::float4(in.position, 0.0, 1.0);
    out.texCoord = in.texCoord;
    return out;
}

fragment float4 pureTexture_fragmentMain(VertexOut_Image in [[stage_in]],
                                          metal::texture2d<float> texture0 [[texture(0)]],
                                          metal::sampler textureSampler [[sampler(0)]])
{
    in.texCoord.y = 1.0 - in.texCoord.y;
    return texture0.sample(textureSampler, in.texCoord);
}