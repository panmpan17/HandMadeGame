#include <metal_stdlib>
#include "camera_data.metal"
#include "common.metal"

vertex VertexOut_Position meshGizmos_vertexMain(VertexIn_Position in [[stage_in]],
                                        constant CameraMatrices& cameraMatrices [[buffer(1)]],
                                        constant Uniform_ModelMatrix& uniforms [[buffer(2)]]) {
    VertexOut_Position out;
    metal::float4 worldPosition = uniforms.modelMatrix * metal::float4(in.position, 1.0);
    out.position = cameraMatrices.projection * cameraMatrices.view * worldPosition;
    return out;
}

fragment metal::float4 meshGizmos_fragmentMain(constant metal::float4& color [[buffer(0)]]) {
    return color;
}
