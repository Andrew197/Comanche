void main()
{
    gl_TexCoord[1] = gl_TextureMatrix[4] * gl_Vertex;       // Shadow
    gl_TexCoord[0] = gl_MultiTexCoord0;                     // Texture
    gl_Position = ftransform();
}