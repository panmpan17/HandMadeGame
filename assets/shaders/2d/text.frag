#version 330

uniform sampler2D u_tex0;

in vec2 uv;
out vec4 fragment;

void main()
{
    float r = texture(u_tex0, uv).r;
    if (r < 0.01) discard;
    fragment = vec4(r);
}
