#version 330 core

#include "assets/shaders/base/camera_data.glsl"

layout(location = 0) in vec3 aPos;

out vec3 TexCoords;



void main()
{
    TexCoords = aPos;
    mat4 view = mat4(mat3(u_View)); // remove translation from the view matrix
    vec4 pos = u_Projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}