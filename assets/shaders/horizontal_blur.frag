#version 330

uniform sampler2D u_tex0;
uniform int u_textureWidth;
uniform int u_blurRadius; // 0 to 20
uniform float u_blurSigma; // 0 to 20.0
const int MAX_RADIUS = 20; // maximum radius for the blur

in vec2 uv;

out vec4 fragment;

void main()
{
    vec2 texelSize = vec2(1.0 / u_textureWidth, 0.0);
    vec4 sum = vec4(0.0);
    float weightSum = 0.0;

    int _min = -max(MAX_RADIUS, u_blurRadius);
    int _max = max(MAX_RADIUS, u_blurRadius);
    for (int i = _min; i <= _max; i++) {
        float w = exp(-0.5 * (i * i) / (u_blurSigma * u_blurSigma));
        sum += texture(u_tex0, uv + float(i) * texelSize) * w;
        weightSum += w;
    }

    fragment = sum / weightSum; // normalize
}