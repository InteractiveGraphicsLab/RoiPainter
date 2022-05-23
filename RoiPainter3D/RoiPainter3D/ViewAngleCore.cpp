#pragma unmanaged 
#include "ViewAngleCore.h"
#include "OglForCLI.h"



ViewAngleCore::ViewAngleCore()
{
  m_texture.Allocate("./shader/boxTex.bmp");
  m_texture.FlipInY();
}

ViewAngleCore::~ViewAngleCore()
{
}


static float  CAM_DIST  = 5.0f;
static double VIEW_RATE = 0.3 ;



static EVec3f ps[6][4] =
  { { EVec3f(-1,-1,-1), EVec3f( 1,-1,-1), EVec3f( 1,-1, 1), EVec3f(-1,-1, 1) },
    { EVec3f( 1,-1,-1), EVec3f( 1, 1,-1), EVec3f( 1, 1, 1), EVec3f( 1,-1, 1) },
    { EVec3f( 1, 1,-1), EVec3f(-1, 1,-1), EVec3f(-1, 1, 1), EVec3f( 1, 1, 1) },
    { EVec3f(-1, 1,-1), EVec3f(-1,-1,-1), EVec3f(-1,-1, 1), EVec3f(-1, 1, 1) },
    { EVec3f( 1, 1, 1), EVec3f(-1, 1, 1), EVec3f(-1,-1, 1), EVec3f( 1,-1, 1) },
    { EVec3f(-1, 1,-1), EVec3f( 1, 1,-1), EVec3f( 1,-1,-1), EVec3f(-1,-1,-1) }
  };

static EVec2f ts[6][4] = 
  {
    { EVec2f(0 / 4., 1 / 3.), EVec2f(1 / 4., 1 / 3.), EVec2f(1 / 4., 2 / 3.), EVec2f(0 / 4., 2 / 3.) }, //A
    { EVec2f(1 / 4., 1 / 3.), EVec2f(2 / 4., 1 / 3.), EVec2f(2 / 4., 2 / 3.), EVec2f(1 / 4., 2 / 3.) }, //R
    { EVec2f(2 / 4., 1 / 3.), EVec2f(3 / 4., 1 / 3.), EVec2f(3 / 4., 2 / 3.), EVec2f(2 / 4., 2 / 3.) }, //P
    { EVec2f(3 / 4., 1 / 3.), EVec2f(4 / 4., 1 / 3.), EVec2f(4 / 4., 2 / 3.), EVec2f(3 / 4., 2 / 3.) }, //L
    { EVec2f(0 / 4., 2 / 3.), EVec2f(1 / 4., 2 / 3.), EVec2f(1 / 4., 3 / 3.), EVec2f(0 / 4., 3 / 3.) }, //T
    { EVec2f(0 / 4., 0 / 3.), EVec2f(1 / 4., 0 / 3.), EVec2f(1 / 4., 1 / 3.), EVec2f(0 / 4., 1 / 3.) } //B
  };




void ViewAngleCore::DrawIndicator
(
  int screen_width, 
  int screen_height, 
  const EVec3f &camera_pos , 
  const EVec3f &camera_center , 
  const EVec3f &camera_ydir
)
{
  const int viewW = (int)(screen_width  * VIEW_RATE);
  const int viewH = (int)(screen_height * VIEW_RATE);

  //setup opengl
  glViewport(0, 0, viewW, viewH);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluPerspective(45.0, viewW / (double)viewH, 0.1, 10.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  //set camera
  EVec3f cam_p = (camera_pos - camera_center).normalized() * CAM_DIST;
  EVec3f cam_c(0,0,0);
  gluLookAt(cam_p[0], cam_p[1], cam_p[2], cam_c[0], cam_c[1], cam_c[2], camera_ydir[0], camera_ydir[1], camera_ydir[2]);

  glActiveTextureARB(GL_TEXTURE0);
  m_texture.BindOgl(true);

  glEnable  (GL_CULL_FACE);
  glCullFace(GL_BACK);
  glDisable (GL_DEPTH_TEST);
  glEnable  (GL_TEXTURE_2D);
  glDisable (GL_LIGHTING);

  glColor3d(1.0, 1.0, 1.0);

  glBegin(GL_QUADS);
  for (int i = 0; i < 6; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      glTexCoord2fv(ts[i][j].data());
      glVertex3fv  (ps[i][j].data());
    }
  }
  glEnd();

  glEnable (GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glViewport(0, 0, screen_width, screen_height);

}








static void unProject_correctY(const double cx, const double cy, const double depth, double &x, double &y, double &z)
{
  double modelMat[16], projMat[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, modelMat);
  glGetDoublev(GL_PROJECTION_MATRIX, projMat);
  int vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  gluUnProject(cx, cy, depth, modelMat, projMat, vp, &x, &y, &z);
}


static bool isInViewCube(const EVec3f &p)
{
  return -1.0001 <= p[0] && p[0] <= 1.0001 &&
         -1.0001 <= p[1] && p[1] <= 1.0001 &&
         -1.0001 <= p[2] && p[2] <= 1.0001;
}



int  ViewAngleCore::PickIndicator
(
    const OglForCLI &ogl, 
    int screen_width, 
    int screen_height, 
    const EVec3f &camera_position, 
    const EVec3f &camera_center, 
    const EVec3f &camera_ydir, 
    const EVec2i &cursor_pos    // 
)
{
  if (!ogl.IsDrawing()) ogl.oglMakeCurrent();

  EVec2i point(cursor_pos[0], screen_height - cursor_pos[1]);

  //set camera
  EVec3f cam_p = (camera_position - camera_center).normalized() * CAM_DIST;
  EVec3f cam_c( 0,0,0);


  const int viewW = (int)(screen_width  * VIEW_RATE);
  const int viewH = (int)(screen_height * VIEW_RATE);

  //setup opengl
  glViewport(0, 0, viewW, viewH);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluPerspective(45.0, viewW / (double)viewH, 0.1, 10.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  gluLookAt(cam_p[0], cam_p[1], cam_p[2], cam_c[0], cam_c[1], cam_c[2], camera_ydir[0], camera_ydir[1], camera_ydir[2]);

  //get cursor ray;
  double x1, y1, z1, x2, y2, z2;
  unProject_correctY(point[0], point[1], 0.01, x1, y1, z1);
  unProject_correctY(point[0], point[1], 0.2 , x2, y2, z2);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glViewport(0, 0, screen_width, screen_height);

  if (!ogl.IsDrawing()) wglMakeCurrent(NULL, NULL);

  //picking
  EVec3f rayP((float)x1, (float)y1, (float)z1);
  EVec3f rayD((float)(x2 - x1), (float)(y2 - y1), (float)(z2 - z1));
  rayD.normalize();

  if (rayD[1] > 0 && isInViewCube(rayP + ((-1 - rayP[1]) / rayD[1]) * rayD)) return 1; // A  y = -1
  if (rayD[1] < 0 && isInViewCube(rayP + (( 1 - rayP[1]) / rayD[1]) * rayD)) return 3; // P  y =  1 

  if (rayD[0] > 0 && isInViewCube(rayP + ((-1 - rayP[0]) / rayD[0]) * rayD)) return 4; // L  x = -1
  if (rayD[0] < 0 && isInViewCube(rayP + (( 1 - rayP[0]) / rayD[0]) * rayD)) return 2; // R  x =  1

  if (rayD[2] > 0 && isInViewCube(rayP + ((-1 - rayP[2]) / rayD[2]) * rayD)) return 6; // B  z = -1
  if (rayD[2] < 0 && isInViewCube(rayP + (( 1 - rayP[2]) / rayD[2]) * rayD)) return 5; // T  z =  1

  return 0;
}

