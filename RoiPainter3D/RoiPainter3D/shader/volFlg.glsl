uniform sampler3D u_img3_vol ;
uniform sampler3D u_img3_gMag ;
uniform sampler3D u_img3_flg ;
uniform sampler3D u_img3_mask;
uniform sampler1D u_img1_tf  ;
uniform sampler1D u_img1_psu ;

uniform float     u_alpha      ;
uniform int       u_doPsuedo   ;
uniform vec4      u_texCdOfst  ; //(x,y,z,0)
uniform vec4      u_eyePos     ;

varying vec4 worldCoord;


vec3 calcGradDir( const float pivC, vec3  texCd     )
{
  float px = pivC - texture3D( u_img3_vol, texCd + u_texCdOfst.xww)+ 0.001;
  float py = pivC - texture3D( u_img3_vol, texCd + u_texCdOfst.wxw);
  float pz = pivC - texture3D( u_img3_vol, texCd + u_texCdOfst.wwx);
  return normalize( vec3( px, py,pz ) );
}


const vec3  lightPos = vec3( 5000, 5000, 1000);
const vec3  Ks       = vec3( 0.6 , 0.6 , 0.6 );
const float Shin     = 128;

vec3  phogShading( const vec3 objPos, const vec3 baseColor, const vec3 N)
{
  vec3 L = normalize( lightPos - objPos );
  float diffK = abs( dot(N,L) )        ;
    
  //高速化してみた　vec3 V = normalize( u_eyePos - objPos );
  //高速化してみた　vec3 R = normalize( 2 * dot( L, N ) * N - L );
  //高速化してみた　float specK = pow( abs( dot(R,V) ), Shin );

  return (diffK + 0.2) * baseColor;// 高速化してみた　+ specK * Ks ; 
}



void main(void)
{
  float imgI  = texture3D( u_img3_vol , gl_TexCoord[0].xyz ).x;
  float imgGM = texture3D( u_img3_gMag, gl_TexCoord[0].xyz ).x;

  if( u_doPsuedo ) gl_FragColor.xyz = texture1D( u_img1_psu, imgI ).xyz;
  else           	 gl_FragColor.xyz = vec3( imgI, imgI, imgI );
	
  vec3 N = calcGradDir( imgI, gl_TexCoord[0].xyz ) ;
  gl_FragColor.xyz = phogShading( worldCoord, gl_FragColor.xyz, N );
  gl_FragColor.w   = u_alpha * texture1D( u_img1_tf, imgI ).x * texture1D( u_img1_tf, imgGM).y ;
}





