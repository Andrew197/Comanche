varying vec3 var_L;
varying vec3 var_N;
varying vec2 var_P;

void main()
{
var_L = gl_LightSource[0].position.xyz;     // Light vector
var_N = gl_NormalMatrix * gl_Normal;        // Normal vector
var_P = gl_Vertex.xy;                       // Position vector
gl_Position = ftransform();                 // Vertex position
}