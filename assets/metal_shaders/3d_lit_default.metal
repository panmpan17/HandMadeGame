#include <metal_stdlib>
#include "camera_data.metal"
#include "light_data.metal"
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
                                          constant LightData& lightData [[buffer(2)]],
                                          metal::texture2d<float> albedoTexture [[texture(0)]],
                                          metal::texture2d<float> specularTexture [[texture(1)]],
                                          metal::texture2d<float> normalTexture [[texture(2)]],
                                          metal::sampler textureSampler [[sampler(0)]],
                                          constant int& textureFlags [[buffer(3)]])
{
    metal::float3 normalDirection;
    metal::float3x3 TBN = metal::float3x3(in.tangent, in.bitangent, in.normal);

    if (textureFlags & (1 << 2))
    {
        metal::float3 textureNormal = normalTexture.sample(textureSampler, in.texCoord).xyz * 2.0 - 1.0;
        normalDirection = metal::normalize(TBN * textureNormal);
    }
    else
    {
        normalDirection = metal::normalize(TBN * metal::float3(0.0, 0.0, 1.0));
    }

    metal::float3 viewDirection = metal::normalize(cameraMatrices.position.xyz - in.vertexWorldPosition);

    metal::float3 diffuseSum = metal::float3(0.0, 0.0, 0.0);
    metal::float3 specularSum = metal::float3(0.0, 0.0, 0.0);

    // TODO: Get from uniform
    metal::float2 specularParam = metal::float2(32.f, 1.f);
    float specularPower = metal::max(specularParam.x, 32.0);
    float specularStrength = metal::max(specularParam.y, 1.0);

    for (int i = 0; i < lightData.lightCounts.x; i++)
    {
        metal::float3 lightDirection = metal::normalize(-lightData.directionLights[i].direction.xyz);
        float diff = metal::max(metal::dot(normalDirection, lightDirection), 0.0);
        diffuseSum += diff * lightData.directionLights[i].color.xyz;

        metal::float3 reflectDirection = metal::reflect(-lightDirection, normalDirection);
        float specular = metal::pow(metal::max(metal::dot(viewDirection, reflectDirection), 0.0), specularPower);
        specularSum += specularStrength * specular * lightData.directionLights[i].color.xyz;
    }

    for (int i = 0; i < lightData.lightCounts.y; i++)
    {
        metal::float3 lightDirection = metal::normalize(lightData.pointLights[i].positionAndRange.xyz - in.vertexWorldPosition);
        float distance = metal::max(metal::length(lightData.pointLights[i].positionAndRange.xyz - in.vertexWorldPosition), 0.1);
        
        // TODO: Check range
        {
            float attenuation = 1.0 / (lightData.pointLights[i].attenuation.x +
                                       lightData.pointLights[i].attenuation.y * distance +
                                       lightData.pointLights[i].attenuation.z * distance * distance);

            float diff = metal::max(metal::dot(normalDirection, lightDirection), 0.0);
            diffuseSum += diff * lightData.pointLights[i].color.xyz * (attenuation * lightData.pointLights[i].attenuation.w);

            metal::float3 reflectDirection = metal::reflect(-lightDirection, normalDirection);
            float specular = metal::pow(metal::max(metal::dot(viewDirection, reflectDirection), 0.0), specularPower);
            specularSum += specularStrength * specular * attenuation * lightData.pointLights[i].attenuation.w * lightData.pointLights[i].color.xyz;
        }
    }

    metal::float3 texColor = metal::float3(.85, .85, .85);
    if (textureFlags & (1 << 0))
    {
        texColor = albedoTexture.sample(textureSampler, in.texCoord).rgb;
    }

    if (textureFlags & (1 << 1))
    {
        metal::float3 specularColor = specularTexture.sample(textureSampler, in.texCoord).rgb;
        specularSum *= specularColor;
    }

    metal::float3 noAmbientLightSum = (diffuseSum + specularSum) * texColor;
    // TODO: Add shadow texture sampling

    metal::float3 finalLighting = (texColor * lightData.ambientColor) + noAmbientLightSum;
    return metal::float4(finalLighting, 1.0);
}
