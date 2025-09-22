#version 330

uniform sampler2D u_tex0;
uniform sampler2D u_tex1;
uniform float u_splitFactor;

in vec2 uv;

out vec4 fragment;

void main()
{
    if (uv.x < u_splitFactor)
    {
        fragment = texture(u_tex0, uv);
    }
    else
    {
        fragment = texture(u_tex1, uv);
    }
}