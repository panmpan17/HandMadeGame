#include <metal_stdlib>
#include "camera_data.metal"
#include "common.metal"

metal::float4x4 mat3ToMat4(metal::float3x3 mat3) {
    metal::float4x4 mat4 = metal::float4x4(1.0);
    mat4.columns[0].xyz = mat3.columns[0];
    mat4.columns[1].xyz = mat3.columns[1];
    mat4.columns[2].xyz = mat3.columns[2];
    return mat4;
}

metal::float3x3 mat4ToMat3(metal::float4x4 mat4) {
    return metal::float3x3(mat4.columns[0].xyz,
                          mat4.columns[1].xyz,
                          mat4.columns[2].xyz);
}

vertex VertexOut_CubeMap cubemap_vertexMain(VertexIn_Position in [[stage_in]],
                                            constant CameraMatrices& cameraMatrices [[buffer(1)]]) {
    VertexOut_CubeMap out;
    out.textCoord = in.position;

    metal::float4x4 view = mat3ToMat4(mat4ToMat3(cameraMatrices.view));
    metal::float4 pos = cameraMatrices.projection * view * metal::float4(in.position, 1.0);
    out.position = pos.xyww;
    return out;
}

fragment metal::float4 cubemap_fragmentMain(VertexOut_CubeMap in [[stage_in]],
                                   metal::texturecube<float> cubeTexture [[texture(0)]],
                                   metal::sampler textureSampler [[sampler(0)]]) {
    return cubeTexture.sample(textureSampler, in.textCoord);
}
