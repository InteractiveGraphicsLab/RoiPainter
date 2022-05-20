#pragma unmanaged
#include "ModeVizMask.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "CrsSecCore.h"
#include "tmesh.h"
#include "tmarchingcubes.h"

#pragma managed
#include "FormVisMask.h"
#include "FormVisParam.h"
#include "FormMain.h"

#pragma unmanaged



using namespace RoiPainter3D;


ModeVizMask::ModeVizMask() :
  m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg_Msk.glsl"),
  m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg_Msk.glsl")
{
  std::cout << "ModeVizMask const...----------------------\n";
  m_bL = m_bR = m_bM = false;
  std::cout << "ModeVizMask DONE -------------------------\n";
}

ModeVizMask::~ModeVizMask()
{
}

bool ModeVizMask::CanLeaveMode()
{
  return true;
}



void ModeVizMask::StartMode()
{
  m_bL = m_bR = m_bM = false;
  formVisMask_Show();
  formVisMask_updateList();

  //Lock/Unlock pitch box 
  formVisParam_UnlockPitchBox();
}



///////////////////////////////////////////////////////////////////////////////
//Mouse Listeners//////////////////////////////////////////////////////////////
void ModeVizMask::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;
  
  if (IsCtrKeyOn())
  {
    m_stroke.clear();
    m_bDrawStr = true;
  }

  ogl->BtnDown_Trans(p);
}

void ModeVizMask::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{

  if (m_bDrawStr)
  {
    EVec3f cube = ImageCore::GetInst()->GetCuboid();
    CrssecCore::GetInst()->GenerateCurvedCrssec(cube, ogl->GetCamPos(), m_stroke);
  }
  m_bL = false;
  m_bDrawStr = false;
  
  ogl->BtnUp();
  RedrawScene();
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
  RedrawScene(false);
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
  RedrawScene(false);
}


void ModeVizMask::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizMask::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizMask::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}

void ModeVizMask::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  if (m_bDrawStr)
  {
    EVec3f rayP, rayD, pos;
    ogl->GetCursorRay(p, rayP, rayD);
    m_stroke.push_back(rayP + 0.1f * rayD);
  }
  else{
    ogl->MouseMove(p);
  }
  RedrawScene(false);
}


void ModeVizMask::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  if( !PickToMoveCrossSecByWheeling(p, ogl, z_delta ) )
  {
    ogl->ZoomCamera(z_delta * 0.1f);
  }

  RedrawScene();
}


void ModeVizMask::KeyDown(int nChar) {}
void ModeVizMask::KeyUp(int nChar) {}






void ModeVizMask::DrawScene(
    const EVec3f &cuboid, 
    const EVec3f &cam_pos, 
    const EVec3f &cam_cnt)
{
  ImageCore::GetInst()->UpdateOGLMaskColorImg();

  //bind volumes ---------------------------------------
  BindAllVolumes();

  if (m_bDrawStr) t_DrawPolyLine(EVec3f(1,1,0), 3, m_stroke);

  //Cross Section
  const EVec3i reso = ImageCore::GetInst()->GetResolution();
  DrawCrossSections(cuboid, reso, m_crssecShader);

  //draw mask surface
  ImageCore::GetInst()->DrawMaskSurfaces();

  //Volume 
  if ( formVisParam_bRendVol() )
  {
    const bool   b_pse   = formVisParam_bDoPsued();
    const float  alpha   = formVisParam_getAlpha();
    const bool   b_roi   = formVisParam_GetOtherROI();
    const bool   b_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int    n_slice = (int)((b_manip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    m_volumeShader.Bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, cam_pos, b_pse, b_roi);
    t_DrawCuboidSlices(n_slice, cam_pos, cam_cnt, cuboid);
    m_volumeShader.Unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }
}


