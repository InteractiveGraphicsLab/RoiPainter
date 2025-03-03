#pragma unmanaged 
#include "ModeVizMask.h"
#include "OglForCLI.h"
#include "ImageCore.h"
#include "ModeCommonTools.h"

#pragma managed 
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormVisMask.h"
#pragma unmanaged 


using namespace RoiPainter4D;


ModeVizMask::ModeVizMask()
{
  std::cout << "ModeVizMask const...----------------------\n";
  m_bL = m_bR = m_bM = false;
  std::cout << "ModeVizMask DONE -------------------------\n";
}


ModeVizMask::~ModeVizMask()
{
}


bool ModeVizMask::CanEndMode()
{
  return true;
}


void ModeVizMask::StartMode()
{
  m_bL = m_bR = m_bM = false;
  
  UpdateImageCoreVisVolumes();
  formVisMask_Show();
  formVisMask_updateList();
}


////////////////////////////////////////////////////////////////////////////
//MouseListener/////////////////////////////////////////////////////////////
void ModeVizMask::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;
  ogl->BtnDown_Trans(p);
}

void ModeVizMask::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeVizMask::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
  ogl->BtnDown_Rot(p);
}

void ModeVizMask::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeVizMask::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}

void ModeVizMask::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeVizMask::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizMask::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizMask::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizMask::KeyDown(int nChar) {}
void ModeVizMask::KeyUp(int nChar) {}


void ModeVizMask::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  ogl->MouseMove(p);
  formMain_RedrawMainPanel();
}


void ModeVizMask::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  if(!WheelingCrssec(p, z_delta, ogl) ) 
  {
    ogl->ZoomCamera(z_delta * 0.1f);
  }
  formMain_RedrawMainPanel();
}


void ModeVizMask::DrawScene(const EVec3f &cam_pos, const EVec3f &cam_cnt)
{
  ImageCore::GetInst()->UpdateImgMaskColor();

  BindAllVolumes();
  DrawCrossSectionsVisMask(!IsShiftKeyOn());

  if ( formVisParam_bRendVol() )
  {
    DrawVolumeVisMask(!IsShiftKeyOn(), cam_pos, cam_cnt);
  }
  
}

