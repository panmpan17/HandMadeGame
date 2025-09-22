#version 330

uniform mat4 u_MVP;

in vec2 a_vPos;
// in vec2 a_vUV;

// out vec2 uv;

void main()
{
    gl_Position = u_MVP * vec4(a_vPos, 0.0, 1.0);
    // uv = a_vUV;
}
