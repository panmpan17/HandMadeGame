#version 330


uniform vec4 u_imageColor;
uniform sampler2D u_tex0;

out vec4 fragment;
in vec2 uv;

void main()
{
    if (u_imageColor.a < 0.01)
        discard;

    vec4 textColor = texture(u_tex0, uv);
    if (textColor.a < 0.01) discard;

    fragment = textColor * u_imageColor;
}
