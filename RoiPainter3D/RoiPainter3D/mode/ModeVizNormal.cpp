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



ModeVizNormal::~ModeVizNormal()
{

}

ModeVizNormal::ModeVizNormal() :
  m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg.glsl"),
  m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
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


void ModeVizNormal::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{


}


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

void ModeVizNormal::DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{

  //bind volumes ---------------------------------------
  BindAllVolumes();

  if (m_bDrawStr) t_DrawPolyLine(EVec3f(1,1,0), 3, m_stroke);

  const EVec3i reso = ImageCore::GetInst()->GetResolution();
  DrawCrossSections(cuboid, reso, m_crssecShader);

  const bool   b_rend_vol = formVisParam_bRendVol();

  if (b_rend_vol)
  {
    const bool  b_pse   = formVisParam_bDoPsued();
    const float alpha   = formVisParam_getAlpha();
    const bool  b_roi   = formVisParam_GetOtherROI();
    const bool  b_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int   n_slice = (int)((b_manip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());

    glDisable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    m_volumeShader.Bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, b_pse, b_roi);
    t_DrawCuboidSlices(n_slice, camP, camF, cuboid);
    m_volumeShader.Unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }

}
    
