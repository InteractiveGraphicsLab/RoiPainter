uniform sampler3D u_img3_vol ;
uniform sampler3D u_img3_gMag ;
uniform sampler3D u_img3_flg ;
uniform sampler3D u_img3_mask;
uniform sampler1D u_img1_tf  ;
uniform sampler1D u_img1_psu ;
uniform sampler1D u_img1_mskC;

uniform float     u_alpha      ;
uniform int       u_doPsuedo   ;
uniform int       u_doOtherROIs;
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
    vec3 V = normalize( u_eyePos - objPos );
    vec3 R = normalize( 2 * dot( L, N ) * N - L );

    float diffK =      abs( dot(N,L) )        ;
    float specK = pow( abs( dot(R,V) ), Shin );

    return (diffK + 0.2) * baseColor + specK * Ks ; 
}


const float COEF_1_255     = 0.00392;
const float COEF_1_255_2   = 0.00196;
const float THRESH_NO_MASK = COEF_1_255  ;
const float THRESH_FORE    = COEF_1_255 * 254.0 + COEF_1_255_2;


void main(void)
{
  float imgI   = texture3D( u_img3_vol, gl_TexCoord[0].xyz ).x;
  float imgFlg = texture3D( u_img3_flg, gl_TexCoord[0].xyz ).x;
  float imgGM  = texture3D( u_img3_gMag, gl_TexCoord[0].xyz ).x;

  gl_FragColor.xyz = u_doPsuedo ? texture1D( u_img1_psu, imgI ).xyz : 
                                  vec3( imgI, imgI, imgI );
	gl_FragColor.w = u_alpha * texture1D( u_img1_tf, imgI ).x * 
                             texture1D( u_img1_tf, imgGM).y ;

  bool isForeVoxel = imgFlg > 1-COEF_1_255;
  bool isBackVoxel = COEF_1_255_2*3 < imgFlg && imgFlg < COEF_1_255_2*5;

	if( isForeVoxel )
	{
		gl_FragColor.xyz = vec3(0,1,0);
		gl_FragColor.w = 1.0;
	}
	else if( isBackVoxel )
	{
		gl_FragColor.xyz = vec3(0,0,1);
		gl_FragColor.w = 0.3;
	}
  else if ( u_doOtherROIs )
  {
    float maskID  = texture3D( u_img3_mask, gl_TexCoord[0].xyz );
    if (maskID > COEF_1_255_2 )
    {
      //mask color
      vec4  color   = texture1D( u_img1_mskC, maskID + COEF_1_255_2).xyzw;
      gl_FragColor.xyz = 0.8 * color.xyz + 0.2 * vec3( imgI, imgI, imgI );
      gl_FragColor.w   = 0.5;//color.w ;
    }
  }  

  vec3 N = calcGradDir( imgI, gl_TexCoord[0].xyz ) ;
  gl_FragColor.xyz = phogShading( worldCoord, gl_FragColor.xyz, N );

}

