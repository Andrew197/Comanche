uniform sampler2D diffuse;
varying vec3 var_L;
varying vec3 var_N;

void main()
{
    vec3 V = vec3(0.0, 0.0, 1.0);                       // View vector
    vec3 L = normalize(var_L);                          // Light vector
    vec3 N = normalize(var_N);                          // Normal vector
    vec3 H = normalize(L + V);                          // Half-angle vector
    vec4 D = texture2D(diffuse, gl_TexCoord[0].xy);     // Diffuse color
    vec4 S = gl_FrontMaterial.specular * 5;             // Specular color (exaggerated)
    float n = gl_FrontMaterial.shininess;               // Specular exponent
    float kd = max(dot(N, L), 0.0);                     // Diffuse intensity
    float ks = pow(max(dot(N, H), 0.0), n);             // Specular intensity
    vec3 rgb = D.rgb * kd + S.rgb * ks;                 // RGB channels
    float a = D.a;                                      // Alpha channel
    gl_FragColor = vec4(rgb, a);                        // Fragment color
}