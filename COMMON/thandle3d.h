#ifndef COMMON_THANDLE_H_
#define COMMON_THANDLE_H_
#pragma unmanaged

//3rd party (glew)
#include "gl/glew.h"
#include <gl/gl.h> 
#include <gl/glu.h> 

//COMMON
#include "tmath.h"

enum ORTHO_HANDLE_ID{
  OHDL_NON, 
  OHDL_X, 
  OHDL_Y, 
  OHDL_Z,
  OHDL_XY, 
  OHDL_YZ, 
  OHDL_ZX
};


// draw translate / rotate handle 
inline void t_DrawCylinder(double len, double r)
{
  glBegin(GL_TRIANGLE_FAN);
  glNormal3d(0, 1, 0);
  for (int i = 0; i < 6; i++)
  {
    double a = i * M_PI / 3;
    glVertex3d(r * sin(a), len, r * cos(a));
  }
  glEnd();

  glBegin(GL_TRIANGLE_STRIP);
  for (int i = 0; i <= 6; i++) {
    double a = -i * M_PI / 3;
    glNormal3d(sin(a), 0, cos(a));
    glVertex3d(r * sin(a), 0, r * cos(a));
    glVertex3d(r * sin(a), len, r * cos(a));
  }
  glEnd();

  glBegin(GL_TRIANGLE_FAN);
  glNormal3d(0, -1, 0);
  for (int i = 0; i < 6; i++) {
    double a = -i * M_PI / 3;
    glVertex3d(r * sin(a), 0, r * cos(a));
  }
  glEnd();
}


inline void t_DrawCone(double len, double r)
{

  glBegin(GL_TRIANGLE_FAN);
  glNormal3d(0, 1, 0);
  glVertex3d(0, len, 0);
  for (int i = 0; i <= 6; i++) {
    double a = i * M_PI / 3;
    glNormal3d(sin(a), 0, cos(a));
    glVertex3d(r * sin(a), 0, r * cos(a));
  }
  glEnd();

  glBegin(GL_TRIANGLE_FAN);
  glNormal3d(0, -1, 0);
  for (int i = 0; i < 6; i++) {
    double a = -i * M_PI / 3;
    glVertex3d(r * sin(a), 0, r * cos(a));
  }
  glEnd();
}


inline void t_DrawCube(double cube_size)
{
  double r = cube_size;
  glBegin(GL_TRIANGLES);
  glNormal3d(0, 0, -1);
  glVertex3d(-r, -r, -r); glVertex3d(r, r, -r); glVertex3d(r, -r, -r);
  glVertex3d(-r, -r, -r); glVertex3d(-r, r, -r); glVertex3d(r, r, -r);
  glNormal3d(0, 0, 1);
  glVertex3d(-r, -r, r); glVertex3d(r, -r, r); glVertex3d(r, r, r);
  glVertex3d(-r, -r, r); glVertex3d(r, r, r); glVertex3d(-r, r, r);
  glNormal3d(0, -1, 0);
  glVertex3d(-r, -r, -r); glVertex3d(r, -r, -r); glVertex3d(r, -r, r);
  glVertex3d(-r, -r, -r); glVertex3d(r, -r, r); glVertex3d(-r, -r, r);
  glNormal3d(0, 1, 0);
  glVertex3d(-r, r, -r); glVertex3d(r, r, r); glVertex3d(r, r, -r);
  glVertex3d(-r, r, -r); glVertex3d(-r, r, r); glVertex3d(r, r, r);
  glNormal3d(-1, 0, 0);
  glVertex3d(-r, -r, -r); glVertex3d(-r, r, r); glVertex3d(-r, r, -r);
  glVertex3d(-r, -r, -r); glVertex3d(-r, -r, r); glVertex3d(-r, r, r);
  glNormal3d(1, 0, 0);
  glVertex3d(r, -r, -r); glVertex3d(r, r, -r); glVertex3d(r, r, r);
  glVertex3d(r, -r, -r); glVertex3d(r, r, r); glVertex3d(r, -r, r);
  glEnd();
}



//handle for translation
inline void t_DrawHandleOrthoArrows(
  const EVec3f &gc, 
  const double length,
  const double radius, 
  const float* color_x,
  const float* color_y,
  const float* color_z )
{
  const double cylinder_length = length * 0.8;
  const double cylinder_radius = radius;
  const double cone_length     = length * 0.2;
  const double cone_radius     = radius * 1.5;
  const float white[4] = {1,1,1,1};
  const float shin[1] = {64};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shin);

  glPushMatrix();
  glTranslated((double)gc[0], (double)gc[1], (double)gc[2]);
  glEnable(GL_LIGHTING);

  //y
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color_y);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color_y);

  glTranslated(0, -cylinder_length, 0);
  t_DrawCylinder( 2*cylinder_length, cylinder_radius);
  glTranslated(0, 2*cylinder_length, 0);
  t_DrawCone( cone_length, cone_radius);
  glTranslated(0, -cylinder_length, 0);

  //x
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color_x);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color_x);
  glRotated(-90, 0, 0, 1);
  glTranslated(0, -cylinder_length, 0);
  t_DrawCylinder( 2*cylinder_length, cylinder_radius);
  glTranslated(0, 2*cylinder_length, 0);
  t_DrawCone( cone_length, cone_radius);
  glTranslated(0, -cylinder_length, 0);

  //z
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color_z);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color_z);
  glRotated(90, 1, 0, 0);
  glTranslated(0, -cylinder_length, 0);
  t_DrawCylinder( 2*cylinder_length, cylinder_radius);
  glTranslated(0, 2*cylinder_length, 0);
  t_DrawCone( cone_length, cone_radius);
  glTranslated(0, -cylinder_length, 0);

  glPopMatrix();
}


//handle for rotation
inline void t_DrawHandleOrthoCircles
(
  const EVec3f &gc, 
  const double radius
)
{
  glDisable(GL_LIGHTING);
  glLineWidth(5);

  glPushMatrix();
  glTranslated((double)gc[0], (double)gc[1], (double)gc[2]);
  
  const int RES = 50;
  const double step = 2 * M_PI / (RES-1);
  const double r = radius;

  glBegin(GL_LINE_STRIP);
  glColor3d(1, 0, 0);
  for (int i = 0; i < RES; ++i) 
    glVertex3d( r*cos(step * i), r* sin(step * i), 0);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glColor3d(0, 1, 0);
  for (int i = 0; i < RES; ++i) 
    glVertex3d( 0, r*cos(step * i), r* sin(step * i));
  glEnd();
  
  glBegin(GL_LINE_STRIP);
  glColor3d(0, 0, 1);
  for (int i = 0; i < RES; ++i) 
    glVertex3d( r* sin(step * i), 0, r*cos(step * i));
  glEnd();

  glPopMatrix();
}


//for scaling handle
inline void t_DrawHandleOrthoCubes(
  const EVec3f &gc, 
  const double length,
  const double radius, 
  const float* color_x,
  const float* color_y,
  const float* color_z
)
{
  const float white[4] = {1,1,1,1};
  const float shin[1] = {64};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shin);

  glPushMatrix();
  glTranslated((double)gc[0], (double)gc[1], (double)gc[2]);

  glEnable(GL_LIGHTING);

  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color_y);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color_y);
  glTranslated(0, -length, 0);
  t_DrawCylinder(2*length, radius);
  glTranslated(0, 2*length, 0);
  t_DrawCube( 1.5 * radius );
  glTranslated(0, -length, 0);

  //x
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color_x);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color_x);
  glRotated(-90, 0, 0, 1);
  glTranslated(0, -length, 0);
  t_DrawCylinder(2*length, radius);
  glTranslated(0, 2*length, 0);
  t_DrawCube( 1.5 * radius );
  glTranslated(0, -length, 0);

  //z
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color_z);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color_z);
  glRotated(90, 1, 0, 0);
  glTranslated(0, -length, 0);
  t_DrawCylinder(2*length, radius);
  glTranslated(0, 2*length, 0);
  t_DrawCube( 1.5 * radius );
  glTranslated(0, -length, 0);

  glPopMatrix();
}


inline ORTHO_HANDLE_ID t_PickHandleOrthoArrows
(
    const EVec3f &ray_pos,
    const EVec3f &ray_dir,
    const EVec3f &center,
    const float   length,
    const float   radius
)
{
  EVec3f line_x0(center[0] - length, center[1], center[2]);
  EVec3f line_x1(center[0] + length, center[1], center[2]);
  EVec3f line_y0(center[0], center[1] - length, center[2]);
  EVec3f line_y1(center[0], center[1] + length, center[2]);
  EVec3f line_z0(center[0], center[1], center[2] - length);
  EVec3f line_z1(center[0], center[1], center[2] + length);

  float dx,dy,dz;
  EVec3f px,py,pz;
  std::tie(dx,px) = t_DistRayAndLineSegm(ray_pos, ray_dir, line_x0, line_x1 );
  std::tie(dy,py) = t_DistRayAndLineSegm(ray_pos, ray_dir, line_y0, line_y1 );
  std::tie(dz,pz) = t_DistRayAndLineSegm(ray_pos, ray_dir, line_z0, line_z1 );

  int min_idx = -1;
  float min_dist = FLT_MAX;

  if ( dx < radius ) {
    min_idx = 0;
    min_dist = t_Dist(ray_pos, px);
  }

  if ( dy < radius && t_Dist(ray_pos, py) < min_dist ) {
    min_idx = 1;
    min_dist = t_Dist(ray_pos, py);
  }

  if ( dz < radius && t_Dist(ray_pos, pz) < min_dist) {
    min_idx = 2;
    min_dist = t_Dist(ray_pos, pz);
  }

  return min_idx == 0 ? OHDL_X : 
         min_idx == 1 ? OHDL_Y : 
         min_idx == 2 ? OHDL_Z : OHDL_NON;
}


inline ORTHO_HANDLE_ID t_PickHandleOrthoCircles
(
    const EVec3f &ray_pos,
    const EVec3f &ray_dir,
    const EVec3f &center,
    const float   length,
    const float   radius
)
{
  //x (yz平面)

  const int   RES  = 50;
  const float step = (float)(2 * M_PI / RES);

  float min_dist = FLT_MAX;
  int   min_idx  = -1;

  float d;
  EVec3f p;
  EVec3f p0, p1;
 
  //xy平面
  for (int i = 0; i < RES; ++i) 
  {
    p0 = EVec3f( length*cos(step * (i-1)), length* sin(step * (i-1)), 0) + center;
    p1 = EVec3f( length*cos(step * i    ), length* sin(step * i    ), 0) + center;

    std::tie(d,p) = t_DistRayAndLineSegm(ray_pos, ray_dir, p0, p1);
    if ( d < radius && t_Dist(p, ray_pos) < min_dist ) {
      min_dist = t_Dist(p, ray_pos);
      min_idx = 0;
    }
  }
   
  //yz平面
  for (int i = 0; i < RES; ++i) 
  {
    p0 = EVec3f( 0, length*cos(step * (i-1)), length* sin(step * (i-1)) ) + center;
    p1 = EVec3f( 0, length*cos(step * i    ), length* sin(step * i    ) ) + center;

    std::tie(d,p) = t_DistRayAndLineSegm(ray_pos, ray_dir, p0, p1);
    if ( d < radius && t_Dist(p, ray_pos) < min_dist ) {
      min_dist = t_Dist(p, ray_pos);
      min_idx = 1;
    }
  }

  //zx平面
  for (int i = 0; i < RES; ++i) 
  {
    p0 = EVec3f( length* sin(step * (i-1)), 0, length*cos(step * (i-1)) ) + center;
    p1 = EVec3f( length* sin(step * i    ), 0, length*cos(step * i    ) ) + center;

    std::tie(d,p) = t_DistRayAndLineSegm(ray_pos, ray_dir, p0, p1);
    if ( d < radius && t_Dist(p, ray_pos) < min_dist ) {
      min_dist = t_Dist(p, ray_pos);
      min_idx = 2;
    }
  }

  return min_idx == 0 ? OHDL_XY : 
         min_idx == 1 ? OHDL_YZ : 
         min_idx == 2 ? OHDL_ZX : OHDL_NON;
}


#endif
