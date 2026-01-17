#include <metal_stdlib>
#include "camera_data.metal"

struct imageGizmos_VertexIn {
    metal::float2 position [[attribute(0)]];
    metal::float2 texCoord [[attribute(1)]];
};

struct imageGizmos_Uniforms {
    metal::float3 worldPosition;
    metal::float4 color;
};

struct imageGizmos_VertexOut {
    metal::float4 position [[position]];
    metal::float2 texCoord;
};


vertex imageGizmos_VertexOut imageGizmos_vertexMain(imageGizmos_VertexIn in [[stage_in]],
                                        constant imageGizmos_Uniforms& uniforms [[buffer(2)]],
                                        constant CameraMatrices& cameraMatrices [[buffer(3)]]) {
    
    metal::float3x3 viewRotation = metal::float3x3(cameraMatrices.view.columns[0].xyz,
                                                   cameraMatrices.view.columns[1].xyz,
                                                   cameraMatrices.view.columns[2].xyz);
        
    metal::float3x3 billboardRotation = metal::transpose(viewRotation);
    metal::float3 worldPosition = uniforms.worldPosition +
                                  billboardRotation * metal::float3(in.position, 0.0);

    imageGizmos_VertexOut out;
    out.position = cameraMatrices.projection * cameraMatrices.view * metal::float4(worldPosition, 1.0);
    out.texCoord = in.texCoord;
    return out;
}

fragment metal::float4 imageGizmos_fragmentMain(imageGizmos_VertexOut in [[stage_in]],
                                   metal::texture2d<float> colorTexture [[texture(0)]],
                                   metal::sampler textureSampler [[sampler(0)]],
                                   constant imageGizmos_Uniforms& uniforms [[buffer(2)]]) {
    return colorTexture.sample(textureSampler, in.texCoord) * uniforms.color;
}
