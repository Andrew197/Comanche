uniform float time;
uniform float speed;
varying vec3 var_L;
varying vec3 var_N;
 
void main() 
{
    vec4 glVert = gl_Vertex;
    mat2 spin = mat2(cos(time * speed), -sin(time * speed), sin(time * speed), cos(time * speed));
    gl_Vertex.xy = glVert.xy * spin;
    
    var_L = gl_LightSource[0].position.xyz;
    var_N = gl_NormalMatrix * gl_Normal;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = ftransform();
}
