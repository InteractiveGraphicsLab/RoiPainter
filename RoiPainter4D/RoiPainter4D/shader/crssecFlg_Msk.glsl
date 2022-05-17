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


void main(void)
{
    float imgI;
	if( u_gMag ) imgI = texture3D(u_img3_gMag, gl_TexCoord[0]).x;
	else         imgI = texture3D(u_img3_vol , gl_TexCoord[0]).x;

	if( u_doHL )
	{
		float maskID  = texture3D( u_img3_mask, gl_TexCoord[0].xyz )  ;
		vec4  color   = texture1D( u_img1_mskC, maskID + COEF_1_255_2);
   		gl_FragColor.xyz = 0.5 * (color.xyz + vec3( imgI, imgI, imgI ));
	}
	else
	{
   		gl_FragColor.xyz = vec3( imgI, imgI, imgI );
	}
}




