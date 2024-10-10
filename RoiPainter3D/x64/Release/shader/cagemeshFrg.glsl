uniform float u_cuboid_w;
uniform float u_cuboid_h;
uniform float u_cuboid_d;
uniform float u_opacity;

uniform float u_crssec_x_01;
uniform float u_crssec_y_01;
uniform float u_crssec_z_01;


varying vec4 worldCoord;
varying vec3 worldNormal;


//phone shading parameters
const vec3  basecolor = vec3( 0.8, 0.8, 0);
const vec3  lightPos  = vec3( 5000, 5000, 1000);
const vec3  Ks        = vec3( 0.6 , 0.6 , 0.2 );
const float Shin      = 128.0;

const float offset = 0.003;


vec3  phogShading( const vec3 objPos, const vec3 normal)
{
  vec3 L = normalize( lightPos - objPos );
  float diffK = abs( dot( normal, L) )   ;
  return (diffK + 0.2) * basecolor;
}


void main(void)
{
  //position = worldCorrd.x / cuboid.w
  float posx_ofst = abs( worldCoord.x / u_cuboid_w - u_crssec_x_01);
  float posy_ofst = abs( worldCoord.y / u_cuboid_h - u_crssec_y_01);
  float posz_ofst = abs( worldCoord.z / u_cuboid_d - u_crssec_z_01);

  if ( posx_ofst < offset || posy_ofst < offset || posz_ofst < offset )
  {
    float minv = min(posx_ofst, min(posy_ofst, posz_ofst) );
    gl_FragColor.xyz = vec3(0.3, 1.0- 10 * minv, 0.3);
    gl_FragColor.w = 1.0;
  }
  else
  {
    gl_FragColor.xyz = phogShading( worldCoord.xyz, worldNormal );
    gl_FragColor.w = u_opacity;
  }
}

