varying vec3 worldCoord ;
varying vec3 worldNormal;


void main(void)
{
  gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
  worldCoord.xyz = gl_Vertex.xyz; 
  worldNormal    = normalize(gl_NormalMatrix * gl_Normal);

  gl_TexCoord[0] = gl_MultiTexCoord0;
}



//gl_Vertex						 : input vertex position
//gl_ModelViewProjectionMatrix   : Top of the matrix stack
//worldCoord                     : position of the vertex (varying to send fragment shader)

