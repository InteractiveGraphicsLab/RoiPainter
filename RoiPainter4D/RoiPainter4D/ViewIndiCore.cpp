
#pragma unmanaged
#include "ViewIndiCore.h"
#include "OglForCLI.h"

ViewIndiCore::ViewIndiCore()
{
  m_texture.Allocate("./shader/boxTex.bmp");
  m_texture.FlipInY();
}


ViewIndiCore::~ViewIndiCore()
{
}

static float  CAM_DIST = 5.0f;
static double VIEW_RATE = 0.3;

void ViewIndiCore::DrawIndicator(int curViewW, int curViewH, EVec3f camP, EVec3f camC, EVec3f camY)
{

  EVec3d m_p[6][4] =
  { { EVec3d(-1,-1,-1), EVec3d(1,-1,-1), EVec3d(1,-1, 1), EVec3d(-1,-1, 1) },
    { EVec3d(1,-1,-1), EVec3d(1, 1,-1), EVec3d(1, 1, 1), EVec3d(1,-1, 1) },
    { EVec3d(1, 1,-1), EVec3d(-1, 1,-1), EVec3d(-1, 1, 1), EVec3d(1, 1, 1) },
    { EVec3d(-1, 1,-1), EVec3d(-1,-1,-1), EVec3d(-1,-1, 1), EVec3d(-1, 1, 1) },
    { EVec3d(1, 1, 1), EVec3d(-1, 1, 1), EVec3d(-1,-1, 1), EVec3d(1,-1, 1) },
    { EVec3d(-1, 1,-1), EVec3d(1, 1,-1), EVec3d(1,-1,-1), EVec3d(-1,-1,-1) }
  };

  EVec2d m_t[6][4] = 
  { { EVec2d(0 / 4., 1 / 3.), EVec2d(1 / 4., 1 / 3.), EVec2d(1 / 4., 2 / 3.), EVec2d(0 / 4., 2 / 3.) }, //A
    { EVec2d(1 / 4., 1 / 3.), EVec2d(2 / 4., 1 / 3.), EVec2d(2 / 4., 2 / 3.), EVec2d(1 / 4., 2 / 3.) }, //R
    { EVec2d(2 / 4., 1 / 3.), EVec2d(3 / 4., 1 / 3.), EVec2d(3 / 4., 2 / 3.), EVec2d(2 / 4., 2 / 3.) }, //P
    { EVec2d(3 / 4., 1 / 3.), EVec2d(4 / 4., 1 / 3.), EVec2d(4 / 4., 2 / 3.), EVec2d(3 / 4., 2 / 3.) }, //L
    { EVec2d(0 / 4., 2 / 3.), EVec2d(1 / 4., 2 / 3.), EVec2d(1 / 4., 3 / 3.), EVec2d(0 / 4., 3 / 3.) }, //T
    { EVec2d(0 / 4., 0 / 3.), EVec2d(1 / 4., 0 / 3.), EVec2d(1 / 4., 1 / 3.), EVec2d(0 / 4., 1 / 3.) } //B
  };


  //set camera
  camP = camP - camC;
  camC << 0, 0, 0;

  camP = camP / camP.norm() * CAM_DIST;

  const int viewW = (int)(curViewW * VIEW_RATE);
  const int viewH = (int)(curViewH * VIEW_RATE);

  //setup opengl
  glViewport(0, 0, viewW, viewH);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluPerspective(45.0, viewW / (double)viewH, 0.1, 10.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  gluLookAt(camP[0], camP[1], camP[2], camC[0], camC[1], camC[2], camY[0], camY[1], camY[2]);

  glActiveTextureARB(GL_TEXTURE0);
  m_texture.BindOgl(true);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);

  glColor3d(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);

  for (int i = 0; i < 6; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      glTexCoord2dv(m_t[i][j].data());
      glVertex3dv(m_p[i][j].data());
    }
  }

  glEnd();

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glViewport(0, 0, curViewW, curViewH);

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


int  ViewIndiCore::PickIndicator(
    const OglForCLI *ogl, 
    int curViewW, 
    int curViewH, 
    EVec3f camP, 
    EVec3f camC, 
    EVec3f camY, 
    EVec2i p)
{
  if (ogl == nullptr) return 0;

  if (!ogl->IsDrawing()) ogl->oglMakeCurrent();
  p[1] = curViewH - p[1];

  //set camera
  camP = camP - camC;
  camC << 0, 0, 0;

  camP = camP / camP.norm() * CAM_DIST;

  const int viewW = (int)(curViewW * VIEW_RATE);
  const int viewH = (int)(curViewH * VIEW_RATE);

  //setup opengl
  glViewport(0, 0, viewW, viewH);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluPerspective(45.0, viewW / (double)viewH, 0.1, 10.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  gluLookAt(camP[0], camP[1], camP[2], camC[0], camC[1], camC[2], camY[0], camY[1], camY[2]);


  //get cursor ray;
  double x1, y1, z1, x2, y2, z2;
  unProject_correctY(p[0], p[1], 0.01, x1, y1, z1);
  unProject_correctY(p[0], p[1], 0.2, x2, y2, z2);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glViewport(0, 0, curViewW, curViewH);

  if (!ogl->IsDrawing()) wglMakeCurrent(NULL, NULL);



  //picking
  EVec3f rayP((float)x1, (float)y1, (float)z1);
  EVec3f rayD((float)(x2 - x1), (float)(y2 - y1), (float)(z2 - z1));
  rayD.normalize();


  if (rayD[1] > 0 && isInViewCube(rayP + ((-1 - rayP[1]) / rayD[1]) * rayD)) return 1; // A  y = -1
  if (rayD[1] < 0 && isInViewCube(rayP + ((1 - rayP[1]) / rayD[1]) * rayD)) return 3; // P  y =  1 

  if (rayD[0] > 0 && isInViewCube(rayP + ((-1 - rayP[0]) / rayD[0]) * rayD)) return 4; // L  x = -1
  if (rayD[0] < 0 && isInViewCube(rayP + ((1 - rayP[0]) / rayD[0]) * rayD)) return 2; // R  x =  1

  if (rayD[2] > 0 && isInViewCube(rayP + ((-1 - rayP[2]) / rayD[2]) * rayD)) return 6; // B  z = -1
  if (rayD[2] < 0 && isInViewCube(rayP + ((1 - rayP[2]) / rayD[2]) * rayD)) return 5; // T  z =  1


  return 0;
}