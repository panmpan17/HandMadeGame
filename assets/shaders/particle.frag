#version 330


uniform vec4 particleColor;

// uniform sampler2D u_tex0;
// uniform vec2 u_tex0Resolution;

out vec4 fragment;
// in vec2 uv;

void main()
{
    // vec2 diffToCenter = (gl_FragCoord.xy - ripperCenter.xy) / u_resolution.xy;
    // vec4 textColor = texture(u_tex0, uv);
    // fragment = textColor * imageColor;
    fragment = particleColor;
}
