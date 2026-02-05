#version 330

uniform sampler2D u_tex0;

in vec2 uv;
out vec4 fragment;

void main()
{
    float r = texture(u_tex0, uv).r;
    fragment = vec4(r);
    // vec4 textColor = texture(u_tex0, uv);
    // if (textColor.a < 0.01)
    //     discard;
    // fragment = textColor;
}
