#version 330


// uniform vec4 u_imageColor;
uniform sampler2D u_tex0;

out vec4 fragment;
in vec2 uv;

void main()
{
    fragment = texture(u_tex0, uv);
}
