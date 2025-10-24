#version 330

uniform sampler2D u_tex0;

in vec2 uv;

out vec4 fragment;

void main()
{
    fragment = texture(u_tex0, uv);
}