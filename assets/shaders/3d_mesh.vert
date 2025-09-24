#version 330

uniform mat4 u_MVP;

layout (location = 0) in vec3 a_vPos;
layout (location = 1) in vec2 a_vUV;
layout (location = 2) in vec3 a_vNormal;

// out vec2 fragUV;

void main()
{
    gl_Position = u_MVP * vec4(a_vPos, 1.0);

    // fragUV = uv;
}
