#version 330

uniform mat4 MVP;
uniform int spriteSheetXCount;
uniform int spriteSheetYCount;
uniform vec2 uvOffset;

in vec2 vPos;
in vec2 vUV;

out vec2 uv;

void main()
{
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);

    uv = vec2(vUV.x / float(spriteSheetXCount), vUV.y / float(spriteSheetYCount)) + uvOffset;
}
