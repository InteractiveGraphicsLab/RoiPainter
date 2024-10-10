#pragma unmanaged
#include "ModeVizNormal.h"
#include "ModeCommonTools.h"
#include "OglForCLI.h"
#include "ImageCore.h"
#include "CrsSecCore.h"

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormVisNorm.h"
#pragma unmanaged

using namespace RoiPainter4D;

ModeVizNormal::~ModeVizNormal()
{
}


ModeVizNormal::ModeVizNormal() :
  m_volume_shader("shader/volVtx.glsl", "shader/volFlg.glsl"),
  m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
{
  std::cout << "ModeVizNormal...\n";
  m_bL = m_bR = m_bM = false;
  m_b_draw_cutstroke = false;
  std::cout << "ModeVizNormal done\n";
}


bool ModeVizNormal::CanEndMode()
{
  return true;
}


void ModeVizNormal::StartMode()
{
  std::cout << "ModeVizNormal...startMode----------\n";
  m_bL = m_bR = m_bM = false;
  formVisNorm_Show();
  std::cout << "ModeVizNormal...startMode DONE-----\n";
}


void ModeVizNormal::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;

  if (IsCtrKeyOn() )
  {
    m_stroke.clear();
    m_b_draw_cutstroke = true;
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }
}


void ModeVizNormal::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if (m_b_draw_cutstroke)
  {
    EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();
    CrssecCore::CreateCurvedCrssec(cuboid, ogl->GetCamPos(), m_stroke);
  }

  m_b_draw_cutstroke = false;
  m_bL = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
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
  formMain_RedrawMainPanel();
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
  formMain_RedrawMainPanel();
}


void ModeVizNormal::LBtnDclk(const EVec2i &p, OglForCLI *ogl){}
void ModeVizNormal::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizNormal::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


void ModeVizNormal::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);
  CRSSEC_ID id = PickCrssec( ray_pos, ray_dir, pos);
  
  if(id != CRSSEC_NON)
  {
    int frame_idx = formVisParam_getframeI();
    short v = ImageCore::GetInst()->GetVoxelValue(frame_idx, pos);
    formVisNorm_setVoxelVis(v);
  }

  if ( m_b_draw_cutstroke )
  {
    m_stroke.push_back(ray_pos + 0.1f * ray_dir);
    formMain_RedrawMainPanel();
  }
  else if (m_bL || m_bR || m_bM)
  {
    ogl->MouseMove(p);
    formMain_RedrawMainPanel();
  }
}


void ModeVizNormal::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  if(!WheelingCrssec(p, z_delta, ogl) ) {
    ogl->ZoomCamera(z_delta * 0.1f);
  }
  formMain_RedrawMainPanel();
}


void ModeVizNormal::KeyDown(int nChar) {}
void ModeVizNormal::KeyUp(int nChar) {}


void ModeVizNormal::DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
  const EVec3i reso = ImageCore::GetInst()->GetReso();

  //bind volumes 
  BindAllVolumes();

  //draw cross section
  DrawCrossSections(cuboid, reso, false, m_crssec_shader);

  //cut stroke
  if ( m_b_draw_cutstroke )
  {
    DrawPolyLine( EVec3f(1,1,0), 3, m_stroke, false);
  }

  if ( formVisParam_bRendVol() )
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    const bool  b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolumeSlices(cuboid, reso, camP, camF, 
                     false, b_onmanip, m_volume_shader);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }
}

