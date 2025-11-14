#version 330
// uniform mat4 u_MVP;

// in vec2 a_vPos;
// in vec3 a_vCol;
// out vec3 color;

// void main()
// {
//     gl_Position = u_MVP * vec4(a_vPos, 0.0, 1.0);
//     color = a_vCol;
// }


#include "assets/shaders/base/camera_data.glsl"

uniform mat4 u_Model;

layout (location = 0) in vec3 a_vPos;
layout (location = 1) in vec2 a_vUV;

out vec2 fragUV;

void main()
{
    vec4 worldPosition = u_Model * vec4(a_vPos, 1.0);
    gl_Position = u_Projection * u_View * worldPosition;

    fragUV = a_vUV;
}
