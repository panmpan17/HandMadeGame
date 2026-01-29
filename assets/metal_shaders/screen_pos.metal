#include <metal_stdlib>
#include "common.metal"

vertex VertexOut_Image screenPosVertexMain(VertexIn_Position2DAndUV in [[stage_in]])
{
    VertexOut_Image out;
    out.position = metal::float4(in.position, 0.0, 1.0);
    out.texCoord = in.texCoord;
    return out;
}
