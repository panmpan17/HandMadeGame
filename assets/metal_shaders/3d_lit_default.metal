#include <metal_stdlib>
#include "camera_data.metal"
#include "common.metal"

struct VertexIn_Lit {
    metal::float3 position [[attribute(0)]];
    metal::float2 texCoord [[attribute(1)]];
    metal::float3 normal [[attribute(2)]];
    metal::float3 tangent [[attribute(3)]];
    metal::float3 bitangent [[attribute(4)]];
};

struct VertexOut_Lit {
    metal::float4 vertexPosition [[position]];
    metal::float3 vertexWorldPosition;
    metal::float2 texCoord;
    
    // Can't use 3x3 int vertex output structs, so use 3 separate float3s
    metal::float3 tangent;
    metal::float3 bitangent;
    metal::float3 normal;
};

vertex VertexOut_Lit LitDefault3D_vertexMain(
        VertexIn_Lit in [[stage_in]],
        constant CameraMatrices& cameraMatrices [[buffer(1)]],
        constant Uniform_ModelMatrix& modelMatrixUniform [[buffer(2)]])
{
    VertexOut_Lit out;

    metal::float4 worldPosition = modelMatrixUniform.modelMatrix * metal::float4(in.position, 1.0);
    out.vertexPosition = cameraMatrices.projection * cameraMatrices.view * worldPosition;
    out.vertexWorldPosition = worldPosition.xyz;

    out.texCoord = in.texCoord;

    out.tangent = metal::normalize((modelMatrixUniform.modelMatrix * metal::float4(in.tangent, 0.0)).xyz);
    out.bitangent = metal::normalize((modelMatrixUniform.modelMatrix * metal::float4(in.bitangent, 0.0)).xyz);
    out.normal = metal::normalize((modelMatrixUniform.modelMatrix * metal::float4(in.normal, 0.0)).xyz);
    return out;
}

fragment float4 LitDefault3D_fragmentMain(VertexOut_Lit in [[stage_in]],
                                          constant CameraMatrices& cameraMatrices [[buffer(1)]],
                                          metal::texture2d<float> albedoTexture [[texture(0)]],
                                          metal::sampler textureSampler [[sampler(0)]])
{
    metal::float3 normalDirection;
    metal::float3x3 TBN = metal::float3x3(in.tangent, in.bitangent, in.normal);

    // if (thereIsNormalTexture)
    // {
    // TODO: Sample normal texture and convert from [0,1] to [-1,1]
    // }
    // else
    {
        normalDirection = metal::normalize(TBN * metal::float3(0.0, 0.0, 1.0));
    }

    metal::float3 viewDirection = metal::normalize(cameraMatrices.position.xyz - in.vertexWorldPosition);

    metal::float3 diffuseSum = metal::float3(0.0, 0.0, 0.0);
    metal::float3 specularSum = metal::float3(0.0, 0.0, 0.0);

    // TODO: Add direction lights
    // TODO: Add point lights

    metal::float3 texColor = metal::float3(.85, .85, .85);
    if (true) // TODO: thereIsAlbedoTexture
    {
        texColor = albedoTexture.sample(textureSampler, in.texCoord).rgb;
    }

    // TODO: Add specular texture sampling

    metal::float3 noAmbientLightSum = (diffuseSum + specularSum) * texColor;
    // TODO: Add shadow texture sampling

    metal::float3 finalLighting = (texColor * 1) + noAmbientLightSum;
    return metal::float4(finalLighting, 1.0);
}
