#include <metal_stdlib>
#include "camera_data.metal"
#include "light_data.metal"
#include "common.metal"

vertex VertexOut_Position DepthOnly3D_vertexMain(
        VertexIn_Lit in [[stage_in]],
        constant metal::float4x4& MVP [[buffer(1)]])
{
    VertexOut_Position out;
    out.position = MVP * metal::float4(in.position, 1.0);
    return out;
}
