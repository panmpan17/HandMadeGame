#include <metal_stdlib>
#include "camera_data.metal"
#include "common.metal"

struct VertexIn_Lit {
    metal::float3 position [[attribute(0)]];
    metal::float2 texCoord [[attribute(2)]];
    metal::float3 normal [[attribute(1)]];
    metal::float3 tangent [[attribute(3)]];
    metal::float3 bitangent [[attribute(4)]];
};

struct VertexOut_Lit {
    metal::float4 position [[position]];
    metal::float2 texCoord;
};

vertex VertexOut_Lit LitDefault3D_vertexMain(
        VertexIn_Lit in [[stage_in]],
        constant CameraMatrices& cameraMatrices [[buffer(1)]],
        constant Uniform_ModelMatrix& modelMatrixUniform [[buffer(2)]])
{
    VertexOut_Lit out;

    metal::float4 worldPosition = modelMatrixUniform.modelMatrix * metal::float4(in.position, 1.0);
    out.position = cameraMatrices.projection * cameraMatrices.view * worldPosition;

    out.texCoord = in.texCoord;
    return out;
}

fragment float4 LitDefault3D_fragmentMain(VertexOut_Lit in [[stage_in]])
{
    return metal::float4(1.0, 1.0, 1.0, 1.0);
}
