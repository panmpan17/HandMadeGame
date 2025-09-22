#version 330

uniform sampler2D u_originalScreenTexture;

in vec2 uv;

out vec4 fragment;

void main()
{
    fragment = texture(u_originalScreenTexture, uv);
}