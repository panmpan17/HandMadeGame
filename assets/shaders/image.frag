#version 330


uniform vec4 u_imageColor;
uniform sampler2D u_tex0;
uniform bool u_useTexture;
// uniform vec2 u_tex0Resolution;

out vec4 fragment;
in vec2 uv;

void main()
{
    // vec2 diffToCenter = (gl_FragCoord.xy - ripperCenter.xy) / u_resolution.xy;

    if (u_useTexture)
    {
        vec4 textColor = texture(u_tex0, uv);
        fragment = textColor * u_imageColor;
    }
    else
    {
        fragment = u_imageColor;
    }
}
