#version 330

uniform mat4 u_MVP;
uniform int u_spriteSheetXCount;
uniform int u_spriteSheetYCount;
uniform vec2 u_uvOffset;

in vec2 a_vPos;
in vec2 a_vUV;

out vec2 uv;

void main()
{
    gl_Position = u_MVP * vec4(a_vPos, 0.0, 1.0);

    uv = vec2(a_vUV.x / float(u_spriteSheetXCount), a_vUV.y / float(u_spriteSheetYCount)) + u_uvOffset;
}
