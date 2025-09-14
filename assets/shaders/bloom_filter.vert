#version 330

in vec2 a_vPos;
in vec2 a_vUV;

out vec2 uv;

void main()
{
    gl_Position = vec4(a_vPos, 0.0, 1.0);
    uv = vec2(a_vUV.x, a_vUV.y);
}
