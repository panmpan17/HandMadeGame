#version 330 core

#include "assets/shaders/base/camera_data.glsl"

// uniform mat4 u_Model;
uniform vec3 u_WorldPosition;

layout (location = 0) in vec2 a_vPos;
layout (location = 1) in vec2 a_vUV;

out vec2 uv;

void main()
{
    // vec4 billboardWorldPos = u_Model[3];
    mat3 viewRotation = mat3(u_View);

    mat3 billboardRotation = transpose(viewRotation);
    vec3 worldPos = u_WorldPosition + billboardRotation * vec3(a_vPos, 0.0);

    gl_Position = u_Projection * u_View * vec4(worldPos, 1.0);
    uv = a_vUV;
}
