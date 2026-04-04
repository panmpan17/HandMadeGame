#version 330

#include "assets/shaders/base/camera_data.glsl"

uniform mat4 u_Model;

layout (location = 0) in vec3 a_vPos;

void main()
{
    gl_Position = u_Projection * u_View * (u_Model * vec4(a_vPos, 1.0));
}
