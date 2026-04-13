#pragma unmanaged
#include "ModeMdlPlaceLMKs.h"
#include "OglForCLI.h"
#include "ModeCommonTools.h"

#pragma managed
#include "../FormMdlPlaceLMKs.h"
#include "../FormVisParam.h"
#include "../FormMain.h"
#pragma unmanaged

using namespace RoiPainter3D;

ModeMdlPlaceLMKs::ModeMdlPlaceLMKs()
{


}


bool ModeMdlPlaceLMKs::CanLeaveMode()
{
  return true;
}

void ModeMdlPlaceLMKs::StartMode() 
{
  formMdlPlaceLMKs_Show();
}



void ModeMdlPlaceLMKs::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = false;
  ogl->BtnUp();
}

void ModeMdlPlaceLMKs::RBtnUp(const EVec2i& p, OglForCLI* ogl) 
{
  m_bR = false;
  ogl->BtnUp();
}
void ModeMdlPlaceLMKs::MBtnUp(const EVec2i& p, OglForCLI* ogl) 
{
  m_bM = false;
  ogl->BtnUp();
}

void ModeMdlPlaceLMKs::LBtnDown(const EVec2i& p, OglForCLI* ogl) 
{
  m_bL = true;
  
  if (IsShiftKeyOn())
  {

  }
  else
  {
    ogl->BtnDown_Trans(p);
  }

}
void ModeMdlPlaceLMKs::RBtnDown(const EVec2i& p, OglForCLI* ogl) 
{
  m_bR = true;
  ogl->BtnDown_Rot(p);
}
void ModeMdlPlaceLMKs::MBtnDown(const EVec2i& p, OglForCLI* ogl) 
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);

}

void ModeMdlPlaceLMKs::LBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeMdlPlaceLMKs::RBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeMdlPlaceLMKs::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}

void ModeMdlPlaceLMKs::MouseMove(const EVec2i& p, OglForCLI* ogl) 
{
  if (!m_bL && !m_bR && !m_bM) return;

  ogl->MouseMove(p);

  RedrawScene();
}



void ModeMdlPlaceLMKs::MouseWheel(const EVec2i& p, short z_delta, OglForCLI* ogl) {}

void ModeMdlPlaceLMKs::KeyDown(int nChar) {}
void ModeMdlPlaceLMKs::KeyUp(int nChar) {}



void ModeMdlPlaceLMKs::DrawScene(const EVec3f& cam_pos, const EVec3f& cam_center) 
{
  BindAllVolumes();

  DrawCrsSec_Standard();

  if (formVisParam_bRendVol())
  {
    bool on_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolume_Standard(cam_pos, cam_center, on_manip);
  }


}