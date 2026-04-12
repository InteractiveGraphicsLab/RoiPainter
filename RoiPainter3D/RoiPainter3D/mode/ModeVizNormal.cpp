#pragma unmanaged
#include "ModeVizNormal.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"
#include <iostream>

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormVisNorm.h"

#pragma unmanaged

using namespace RoiPainter3D;



ModeVizNormal::~ModeVizNormal(){}

ModeVizNormal::ModeVizNormal() 
{
  std::cout << "ModeVizNormal...\n";

  m_bL = m_bR = m_bM = false;
  m_bDrawStr = false;

  std::cout << "ModeVizNormal done\n";
}



bool ModeVizNormal::CanLeaveMode()
{
  return true;
}



void ModeVizNormal::StartMode()
{
  std::cout << "ModeVizNormal...startMode----------\n";

  m_bL = m_bR = m_bM = false;
  formVisNorm_Show();

  //Lock/Unlock pitch box 
  formVisParam_UnlockPitchBox();

  std::cout << "ModeVizNormal...startMode DONE-----\n";
}



void ModeVizNormal::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;

  if (IsCtrKeyOn())
  {
    m_stroke.clear();
    m_bDrawStr = true;
  }
  ogl->BtnDown_Trans(p);
}


void ModeVizNormal::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if (m_bDrawStr)
  {
    EVec3f cube = ImageCore::GetInst()->GetCuboid();
    CrssecCore::GetInst()->GenerateCurvedCrssec(cube, ogl->GetCamPos(), m_stroke);
  }

  m_bDrawStr = false;
  m_bL = false;
  ogl->BtnUp();

  RedrawScene();
}

void ModeVizNormal::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
  ogl->BtnDown_Rot(p);
}

void ModeVizNormal::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  ogl->BtnUp();
  RedrawScene(false);
}

void ModeVizNormal::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}

void ModeVizNormal::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  RedrawScene(false);
}


void ModeVizNormal::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizNormal::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizNormal::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


void ModeVizNormal::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  EVec3f rayP, rayD, pos;
  ogl->GetCursorRay(p, rayP, rayD);

  CRSSEC_ID id = PickCrssec( rayP, rayD, &pos);
  if(id != CRSSEC_NON){
    short v = ImageCore::GetInst()->GetVoxelValue(pos);
    formVisNorm_setVoxelVis(v);
  }

  if (m_bDrawStr)
  {
    m_stroke.push_back(rayP + 0.1f * rayD);
    RedrawScene(false);
  }
  else if (m_bL || m_bR || m_bM)
  {
    ogl->MouseMove(p);
    RedrawScene(false);
  }
}


void ModeVizNormal::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
  if( !PickToMoveCrossSecByWheeling(p, ogl, zDelta ) )
  {
    ogl->ZoomCamera(zDelta * 0.1f);
  }
  RedrawScene();
}


void ModeVizNormal::KeyDown(int nChar) {}
void ModeVizNormal::KeyUp(int nChar) {}

void ModeVizNormal::DrawScene(const EVec3f &camP, const EVec3f &camF)
{
  BindAllVolumes();

  if (m_bDrawStr) DrawPolyLine(EVec3f(1,1,0), 3, m_stroke);

  DrawCrsSec_Standard();

  if (formVisParam_bRendVol())
  {
    bool on_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolume_Standard(camP, camF, on_manip);
  }

}
    
