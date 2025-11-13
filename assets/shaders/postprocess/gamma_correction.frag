#version 330

uniform sampler2D u_tex0;
uniform float u_gamma;

in vec2 uv;

out vec4 fragment;

void main()
{
    fragment = texture(u_tex0, uv);
    fragment.rgb = pow(fragment.rgb, vec3(1.0 / u_gamma)); // Gamma correction with gamma = u_gamma
}