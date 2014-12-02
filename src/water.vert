uniform float time;
uniform float height;
uniform float speed;
varying vec3 var_L;
varying vec3 var_N;

void main()
{
    vec4 P = gl_Vertex;
    P.y += height*sin(P.x + time * speed) + height*sin(P.z + time * speed);     //wave generation
    var_L = gl_LightSource[0].position.xyz;                                     // Light vector
    var_N = gl_NormalMatrix * gl_Normal;                                        // Normal vector
    gl_TexCoord[0] = gl_MultiTexCoord0;                                         // Texture coordinate
    gl_Position = gl_ModelViewProjectionMatrix * P;		 	                    // Vertex position
}

