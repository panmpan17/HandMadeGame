#version 330

uniform bool UseTexture;
uniform sampler2D u_tex0;

in vec4 fragmentColor;
in vec2 uv;

out vec4 fragment;

void main()
{
    if (UseTexture)
    {
        fragment = texture(u_tex0, uv) * fragmentColor;
    }
    else
    {
        fragment = fragmentColor;
    }
}
