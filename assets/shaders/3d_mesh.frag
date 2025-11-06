#version 330

layout(std140) uniform CameraMatrices
{
    mat4 u_View;
    mat4 u_Projection;
    vec3 u_CamPos;
};

struct DirectionLight // 32
{
    vec4 direction; // 12 + 4
    vec4 color; // 12 + 4
};

struct PointLight // 48
{
    vec4 positionAndRange; // 16
    vec4 color; // 12 + 4
    vec4 attenuationParams; // 12 + 4
};

layout(std140) uniform LightData // 544
{
    vec3 u_AmbientLightColor; // 12 + 4
    
    DirectionLight u_DirectionLights[4]; // 32 * 4 = 128

    PointLight u_PointLights[8]; // 48 * 8 = 384

    ivec2 u_LightCounts; // 16 + 16
};

uniform sampler2D u_tex0; // main texture
uniform sampler2D u_tex1; // specular map
uniform sampler2D u_tex2; // normal map
uniform sampler2D u_tex3; // depth map
uniform int u_textureEnabled; // bitmask for texture usage, 1: main texture, 2: specular map, 3: both

uniform vec2 u_SpecularParams; // x: intensity, y: power

in vec2 fragUV;
in vec3 fragPos;
in vec3 fragNormal;
in vec4 fragLightSpacePos;

out vec4 fragment;

float shadowCalculation(vec4 lightSpacePos)
{
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(u_tex3, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = 0.005;
    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 norm;

    if ((u_textureEnabled & 4) != 0)
    {
        vec4 texColor = texture(u_tex2, fragUV);
        norm = normalize(texColor.xyz * 2.0 - 1.0);
    }
    else
    {
        norm = normalize(fragNormal);
    }

    vec3 viewDir = normalize(u_CamPos - fragPos);

    vec3 diffuseSum = vec3(0.0);
    vec3 specularSum = vec3(0.0);

    for (int i = 0; i < u_LightCounts.x; i++)
    {
        vec3 lightDir = normalize(-u_DirectionLights[i].direction.xyz);
        float diff = max(dot(norm, lightDir), 0.0);
        diffuseSum += diff * u_DirectionLights[i].color.xyz;

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), max(u_SpecularParams.y, 32.0));
        specularSum += max(u_SpecularParams.x, 1) * spec * u_DirectionLights[i].color.xyz;
    }

    for (int i = 0; i < u_LightCounts.y; i++)
    {
        vec3 lightDir = normalize(u_PointLights[i].positionAndRange.xyz - fragPos);
        float distance = max(length(u_PointLights[i].positionAndRange.xyz - fragPos), 0.1);

        // if (distance < (u_PointLights[i].positionAndRange.w * u_PointLights[i].positionAndRange.w))
        {
            float attenuation = 1 / (u_PointLights[i].attenuationParams.x + (u_PointLights[i].attenuationParams.y * distance) + (u_PointLights[i].attenuationParams.z * distance * distance));

            float diff = max(dot(norm, lightDir), 0.0);
            diffuseSum += diff * u_PointLights[i].color.xyz * attenuation * u_PointLights[i].positionAndRange.w;

            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), max(u_SpecularParams.y, 32.0));
            specularSum += max(u_SpecularParams.x, 1) * spec * u_PointLights[i].color.xyz * attenuation * u_PointLights[i].positionAndRange.w;
        }
    }

    vec3 lighting = vec3(1);

    vec4 texColor = vec4(1);
    if ((u_textureEnabled & 1) != 0)
    {
        texColor = texture(u_tex0, fragUV);
    }

    if ((u_textureEnabled & 2) != 0)
    {
        vec4 specularMapColor = texture(u_tex1, fragUV);
        specularSum *= specularMapColor.xyz;
    }

    float shadow = shadowCalculation(fragLightSpacePos);
    lighting = (((diffuseSum + specularSum) * (1 - shadow)) + u_AmbientLightColor) * texColor.xyz;

    fragment = vec4(lighting, texColor.w);

    /*
    vec4 color = vec4(0);

    if ((u_textureEnabled & 1) != 0)
    {
        vec4 texColor = texture(u_tex0, fragUV);
        color.xyz += texColor.xyz * (u_AmbientLightColor + diffuseSum);
        color.w = texColor.w; // preserve alpha from texture
    }
    else
    {
        color.xyz += u_AmbientLightColor + diffuseSum;
        color.w = 1.0;
    }

    if ((u_textureEnabled & 2) != 0)
    {
        vec4 specularMapColor = texture(u_tex1, fragUV);
        color.xyz += specularSum * specularMapColor.xyz;
    }
    else
    {
        color.xyz += specularSum;
    }

    fragment = color;
    */
}