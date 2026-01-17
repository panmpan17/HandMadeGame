#include <metal_stdlib>
#include "camera_data.metal"

struct meshGizmos_VertexIn {
    metal::float3 position [[attribute(0)]];
};

struct meshGizmos_Uniforms {
    metal::float4x4 modelMatrix;
};

struct meshGizmos_VertexOut {
    metal::float4 position [[position]];
};


vertex meshGizmos_VertexOut meshGizmos_vertexMain(meshGizmos_VertexIn in [[stage_in]],
                                        constant CameraMatrices& cameraMatrices [[buffer(1)]],
                                        constant meshGizmos_Uniforms& uniforms [[buffer(2)]]) {
    meshGizmos_VertexOut out;
    metal::float4 worldPosition = uniforms.modelMatrix * metal::float4(in.position, 1.0);
    out.position = cameraMatrices.projection * cameraMatrices.view * worldPosition;
    return out;
}

fragment metal::float4 meshGizmos_fragmentMain(constant metal::float3& in [[buffer(0)]]) {
    return metal::float4(in, 1.0);
}
