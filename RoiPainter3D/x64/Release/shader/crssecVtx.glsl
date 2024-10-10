
varying vec3 globalPos;

void main(void)
{
    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
    globalPos      = gl_Vertex.xyz;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}

