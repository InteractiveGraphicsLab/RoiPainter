varying vec3 worldCoord;
varying vec3 globalPos ; //only for polar Cd rendering 


void main(void)
{
    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
    worldCoord.xyz = gl_Position.xyz; //ftransform();
	globalPos      = gl_Vertex.xyz;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}



//gl_Vertex						 : input vertex position
//gl_ModelViewProjectionMatrix   : Top of the matrix stack
//worldCoord                     : position of the vertex (varying to send fragment shader)

