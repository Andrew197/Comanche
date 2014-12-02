uniform sampler2D diffuse;
uniform float time;
varying vec3 var_L;
varying vec3 var_N;

void main()
{
    vec3 V = vec3(0.0, 0.0, 1.0);                       // View vector
    vec3 L = normalize(var_L);                          // Light vector
    vec3 N = normalize(var_N);                          // Normal vector
    vec3 H = normalize(L + V);                          // Half-angle vector
    vec4 D = texture2D(diffuse, gl_TexCoord[0].xy);     // Diffuse color
    vec4 S = gl_FrontMaterial.specular * 5;             // Specular color
    float n = gl_FrontMaterial.shininess;               // Specular exponent
    float kd = max(dot(N, L), 0.0);                     // Diffuse intensity
    float ks = 0;                                       // Specular intensity
    
    D.r += D.r * sin(time * 0.005);                     //red
    D.g += D.g * sin(time * 0.0005);                    //green
    D.b += D.b * sin(time * 0.0025);                    //blue

    vec3 rgb = D.rgb * kd + S.rgb * ks;                 // RGB final calculation
    float a = D.a;                                      // Alpha channel

    gl_FragColor = vec4(rgb, a);                        // Fragment color
}