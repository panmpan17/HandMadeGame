#version 330

uniform sampler2D u_tex0;

in vec2 uv;

out vec4 fragment;

void main()
{
    vec4 textColor = texture(u_tex0, uv);

    float colorMax = max(max(textColor.r, textColor.g), textColor.b);
    if(colorMax < 0.8f)
        textColor = vec4(0, 0, 0, 1);
    else
        textColor = vec4(textColor.rgb, 1);

    fragment = textColor;//vec4(uv, 0, 1);
}