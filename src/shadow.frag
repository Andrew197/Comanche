#version 120

uniform sampler2DShadow shadow;

void main()
{
    gl_FragColor = shadow2DProj(shadow, gl_TexCoord[1]);
}