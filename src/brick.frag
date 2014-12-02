varying vec3 var_L;
varying vec3 var_N;
varying vec2 var_P;

uniform vec3 mortar_color;
uniform vec3 brick_color;
uniform vec2 brick_size;
uniform vec2 brick_frac;

vec3 material_color(vec2 position)
{
    vec2 p = position / brick_size;
    if (fract(p.y * 0.5) > 0.5)
        p.x += 0.5;
    p = fract(p);
    vec2 b = step(p, brick_frac);
    return mix(mortar_color, brick_color, b.x * b.y);
}

void main()
{
    vec3 L = normalize(var_L);                              // Light vector
    vec3 N = normalize(var_N);                              // Normal vector
    float kd = max(dot(N, L), 0.0);                         // Diffuse intensity
    gl_FragColor = vec4(material_color(var_P) * kd, 1.0);   // Fragment color
}