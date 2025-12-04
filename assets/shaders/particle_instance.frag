#version 330

uniform bool u_useTexture;
uniform sampler2D u_tex0;

in vec4 fragmentColor;
in vec2 uv;

out vec4 fragment;

void main()
{
    if (fragmentColor.a < 0.01)
    {
        discard;
    }

    if (u_useTexture)
    {
        vec4 textColor = texture(u_tex0, uv);
        if (textColor.a < 0.01)
        {
            discard;
        }
        fragment = textColor * fragmentColor;
    }
    else
    {
        fragment = fragmentColor;
    }
}
