uniform sampler3D u_img3_vol ;
uniform sampler3D u_img3_gMag;
uniform sampler3D u_img3_flg ;
uniform sampler3D u_img3_mask;
uniform sampler1D u_img1_tf  ;
uniform sampler1D u_img1_psu ;
uniform sampler1D u_img1_mskC;

uniform int       u_doHL       ;
uniform int       u_gMag       ;
uniform vec4      u_texCdOfst  ; //(x,y,z,0)

const float COEF_1_255     = 0.00392;
const float COEF_1_255_2   = 0.00196;

//u_img3_flg  0  - non target 
//            1  - trgt back
//            2  - negative
//            3  - now
//           255 - foreground 



void main(void)
{
    float imgI; 
	if( u_gMag ) imgI = texture3D(u_img3_gMag, gl_TexCoord[0]).x;
	else         imgI = texture3D(u_img3_vol , gl_TexCoord[0]).x;

    gl_FragColor = vec4(imgI, imgI, imgI, 1 );

	if( u_doHL ) 
	{

		float imgFlg = texture3D( u_img3_flg, gl_TexCoord[0].xyz ).x;

		if( imgFlg > 1 - COEF_1_255_2) //flg == 255
		{
			gl_FragColor.x = 1.0;
		}
		else if( COEF_1_255_2*3 < imgFlg && imgFlg < COEF_1_255_2*5)
		{
			gl_FragColor.x -= 0.3;
			gl_FragColor.y -= 0.3;
			gl_FragColor.z += 0.8;
		}
		else if( imgFlg < COEF_1_255_2) //flg == 0
		{
			float maskID  = texture3D( u_img3_mask, gl_TexCoord[0].xyz);
			vec4  color   = texture1D(u_img1_mskC, maskID + COEF_1_255_2).xyzw;
   			gl_FragColor.xyz = 0.5 * (color.xyz + vec3( imgI, imgI, imgI ));
		}
	} 
}
