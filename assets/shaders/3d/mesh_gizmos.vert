#version 330

#include "assets/shaders/base/camera_data.glsl"

uniform vec3 u_WorldPosition;
uniform vec3 u_Scale;

layout (location = 0) in vec3 a_vPos;

void main()
{
    vec4 worldPosition = vec4(u_WorldPosition + (a_vPos * u_Scale), 1.0);
    gl_Position = u_Projection * u_View * worldPosition;
}
