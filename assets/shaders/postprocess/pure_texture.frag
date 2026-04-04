#version 330

uniform sampler2D u_tex0;

in vec2 uv;

out vec4 fragment;

void main()
{
    vec4 color = texture(u_tex0, uv);

    float _max = max(color.r, max(color.g, color.b));
    if (_max < 0.01)
    {
        discard;
    }

    fragment = color;
}