#include <metal_stdlib>
#include "common.metal"
#include "camera_data.metal"
// #include "math.metal"

metal::float3x3 mat4ToMat3(metal::float4x4 mat4);

metal::float2x2 angleToMat2(float angle) {
    float c = metal::cos(angle);
    float s = metal::sin(angle);
    return metal::float2x2(c, -s, s, c);
}

struct Uniform_Particle {
    metal::float4x4 modelMatrix;
    float useNodeTransform;
};

struct ParticleInstanceData {
    metal::float3 position;
    metal::float4 color;
    float rotation;
    float scale;
    float opacity;
};

vertex VertexOut_Image particleInstance_vertexMain(
        VertexIn_Position2DAndUV quad [[stage_in]],
        constant Uniform_Particle& particleUniform [[buffer(2)]],
        constant CameraMatrices& cameraMatrices [[buffer(3)]],
        constant ParticleInstanceData* instanceData [[buffer(4)]],
        uint instanceID [[instance_id]])
{
    VertexOut_Image oData;

    metal::float2x2 rotationMatrix = angleToMat2(instanceData[instanceID].rotation);
    metal::float2 translatedPos = rotationMatrix * (quad.position * instanceData[instanceID].scale);
    metal::float4 position = metal::float4(translatedPos + instanceData[instanceID].position.xy, instanceData[instanceID].position.z, 1.0);
    
    bool useNodeTransform = particleUniform.useNodeTransform;
    if (useNodeTransform)
    {
        metal::float4x4 mvp = cameraMatrices.projection * cameraMatrices.view * particleUniform.modelMatrix;
        oData.position = mvp * position;
    }
    else
    {
        metal::float4x4 mvp = cameraMatrices.projection * cameraMatrices.view;
        oData.position = mvp * position;
    }

    oData.color = instanceData[instanceID].color;
    oData.color.a *= instanceData[instanceID].opacity;
    oData.texCoord = quad.texCoord;
    return oData;
}

vertex VertexOut_Image particleInstanceBillboardVertexMain(
        VertexIn_Position2DAndUV quad [[stage_in]],
        constant Uniform_Particle& particleUniform [[buffer(2)]],
        constant CameraMatrices& cameraMatrices [[buffer(3)]],
        constant ParticleInstanceData* instanceData [[buffer(4)]],
        uint instanceID [[instance_id]])
{
    VertexOut_Image oData;

    metal::float2x2 rotationMatrix = angleToMat2(instanceData[instanceID].rotation);
    metal::float2 translatedPos = rotationMatrix * (quad.position * instanceData[instanceID].scale);

    // metal::float4 position = metal::float4(translatedPos + instanceData[instanceID].position.xy, instanceData[instanceID].position.z, 1.0);

    metal::float3x3 viewRotation = mat4ToMat3(cameraMatrices.view);
    metal::float3x3 billboardRotation = metal::transpose(viewRotation);

    // vec2 scaledPos = quadPos * scale;

    metal::float3 verticeRelativePos = billboardRotation * metal::float3(translatedPos, 0.0);
    metal::float4 verticePos = metal::float4(instanceData[instanceID].position + verticeRelativePos, 1.0);
    
    bool useNodeTransform = particleUniform.useNodeTransform;
    if (useNodeTransform)
    {
        metal::float4x4 mvp = cameraMatrices.projection * cameraMatrices.view * particleUniform.modelMatrix;
        oData.position = mvp * verticePos;
    }
    else
    {
        metal::float4x4 mvp = cameraMatrices.projection * cameraMatrices.view;
        oData.position = mvp * verticePos;
    }

    oData.color = instanceData[instanceID].color;
    oData.color.a *= instanceData[instanceID].opacity;
    oData.texCoord = quad.texCoord;
    return oData;
}

fragment float4 particleInstance_fragmentMain(VertexOut_Image in [[stage_in]],
                                              metal::texture2d<float> texture0 [[texture(0)]],
                                              metal::sampler textureSampler [[sampler(0)]])
{
    return in.color * texture0.sample(textureSampler, in.texCoord);
}