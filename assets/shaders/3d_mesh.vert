#version 330

#include "assets/shaders/base/camera_data.glsl"

uniform mat4 u_Model;
uniform mat4 u_LightMatrix;

layout (location = 0) in vec3 a_vPos;
layout (location = 1) in vec2 a_vUV;
layout (location = 2) in vec3 a_vNormal;
layout (location = 3) in vec3 a_vTangent;
layout (location = 4) in vec3 a_vBitangent;

out vec2 fragUV;
out vec3 fragPos;
out mat3 fragTBN;
out vec4 fragLightSpacePos;

void main()
{
    vec4 worldPosition = u_Model * vec4(a_vPos, 1.0);
    gl_Position = u_Projection * u_View * worldPosition;

    fragPos = worldPosition.xyz;
    fragUV = a_vUV;
    mat3 m = mat3(transpose(inverse(u_Model)));
    fragLightSpacePos = u_LightMatrix * worldPosition;

    vec3 T = normalize(vec3(u_Model * vec4(a_vTangent, 0)));
    vec3 B = normalize(vec3(u_Model * vec4(a_vBitangent, 0)));
    vec3 N = normalize(vec3(u_Model * vec4(a_vNormal, 0)));
    fragTBN = mat3(T, B, N);
}
