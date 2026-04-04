#version 330

uniform sampler2D u_originalScreenTexture;
uniform sampler2D u_bloomTexture;
uniform float u_bloomTextureScale;
uniform float u_intensity;

in vec2 uv;

out vec4 fragment;

void main()
{
    vec4 textColor1 = texture(u_originalScreenTexture, uv);
    vec4 textColor2 = texture(u_bloomTexture, uv * u_bloomTextureScale);

    fragment = textColor1 + (textColor2 * u_intensity);
}