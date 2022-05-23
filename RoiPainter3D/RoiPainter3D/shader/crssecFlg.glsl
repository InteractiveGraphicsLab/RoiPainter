uniform sampler3D u_img3_vol ;
uniform sampler3D u_img3_gMag;
uniform sampler3D u_img3_flg ;
uniform sampler3D u_img3_mask;
uniform sampler1D u_img1_tf  ;
uniform sampler1D u_img1_psu ;

uniform int       u_doHL       ;
uniform int       u_gMag       ;
uniform vec4      u_texCdOfst  ; //(x,y,z,0)

void main(void)
{
    float imgI;
	if( u_gMag ) imgI = texture3D(u_img3_gMag, gl_TexCoord[0]).x;
	else         imgI = texture3D(u_img3_vol , gl_TexCoord[0]).x;
    gl_FragColor = vec4(imgI, imgI, imgI, 1 );
}
