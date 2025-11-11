#version 330

#include "assets/shaders/base/camera_data.glsl"

uniform mat4 u_Model;
uniform mat4 u_LightMatrix;

layout (location = 0) in vec3 a_vPos;
layout (location = 1) in vec2 a_vUV;
layout (location = 2) in vec3 a_vNormal;

out vec2 fragUV;
out vec3 fragPos;
out vec3 fragNormal;
out vec4 fragLightSpacePos;

void main()
{
    vec4 worldPosition = u_Model * vec4(a_vPos, 1.0);
    gl_Position = u_Projection * u_View * worldPosition;

    fragPos = worldPosition.xyz;
    fragUV = a_vUV;
    fragNormal = mat3(transpose(inverse(u_Model))) * a_vNormal;
    fragLightSpacePos = u_LightMatrix * worldPosition;
}
