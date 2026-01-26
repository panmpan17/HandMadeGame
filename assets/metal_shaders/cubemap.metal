#include <metal_stdlib>
#include "camera_data.metal"
#include "common.metal"
#include "math.metal"

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
