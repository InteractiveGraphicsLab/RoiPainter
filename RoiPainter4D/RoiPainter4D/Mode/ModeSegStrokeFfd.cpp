#pragma unmanaged

#include "ModeSegStrokeFfd.h"
#include "ModeCommonTools.h"

#include "OglForCLI.h"   // openGLを利用するためのクラス
#include "ImageCore.h"   // 画像データを保持しているクラス 　
#include "CrsSecCore.h"  // 切断面描画や、切断面への操作を管理

#pragma managed
#include "FormMain.h"     // Main Window
#include "FormVisParam.h" // 右上のダイアログ
#include "FormSegStrokeFfd.h"
#pragma unmanaged

using namespace RoiPainter4D;

ModeSegStrokeFfd::~ModeSegStrokeFfd()
{
}


ModeSegStrokeFfd::ModeSegStrokeFfd() :
  m_volume_shader("shader/volVtx.glsl"   , "shader/volFlg.glsl"   ),
  m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
{
  std::cout << "ModeSegStrokeFfd...\n";
  m_bL = m_bR = m_bM = false;
  m_b_drawstroke = false;
  std::cout << "ModeSegStrokeFfd done\n";
}


bool ModeSegStrokeFfd::canEndMode()
{
  //今処理を終わっていいですか? 
  return true;
}


void ModeSegStrokeFfd::startMode()
{
  std::cout << "ModeSegStrokeFfd...startMode----------\n";
  m_bL = m_bR = m_bM = false;
  FormSegStrokeFfd_MyFunction();
  FormSegStrokeFfd_Show();
  std::cout << "ModeSegStrokeFfd...startMode DONE-----\n";
}


void ModeSegStrokeFfd::LBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = true;

  if (isCtrKeyOn())
  {
    m_stroke.clear();
    m_b_drawstroke = true;
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }
}


void ModeSegStrokeFfd::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  if (m_b_drawstroke)
  {
    //EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();
    //CrssecCore::CreateCurvedCrssec(cuboid, ogl->GetCamPos(), m_stroke);
  }

  m_b_drawstroke = false;
  m_bL = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::RBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = true;
  ogl->BtnDown_Rot(p);
}


void ModeSegStrokeFfd::RBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::MBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}


void ModeSegStrokeFfd::MBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::LBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeSegStrokeFfd::RBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeSegStrokeFfd::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}


void ModeSegStrokeFfd::MouseMove(const EVec2i& p, OglForCLI* ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  if (m_b_drawstroke)
  {
    EVec3f ray_pos, ray_dir, pos;
    ogl->GetCursorRay(p, ray_pos, ray_dir);
    m_stroke.push_back(ray_pos + 0.1f * ray_dir);
  }
  else 
  {
    ogl->MouseMove(p);
  }

  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::MouseWheel(
    const EVec2i& p, 
    short z_delta, 
    OglForCLI* ogl)
{
  if (!WheelingCrssec(p, z_delta, ogl)) 
  {
    ogl->ZoomCamera(z_delta * 0.1f);
  }
  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::keyDown(int nChar) {}
void ModeSegStrokeFfd::keyUp(int nChar) {}


void ModeSegStrokeFfd::drawScene(
    const EVec3f& cuboid, 
    const EVec3f& camP  , 
    const EVec3f& camF  )
{
  const EVec3i reso  = ImageCore::GetInst()->GetReso();
  const EVec3f cube  = ImageCore::GetInst()->GetCuboid();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();

  //bind volumes 
  BindAllVolumes();

  //draw cross section
  DrawCrossSections(cuboid, reso, false, m_crssec_shader);

  //cut stroke
  if (m_b_drawstroke)
  {
    t_DrawPolyLine(EVec3f(1, 1, 0), 3, m_stroke, false);
  }

  if (formVisParam_bRendVol())
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    const bool  b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolumeSlices(cuboid, reso, camP, camF, false, b_onmanip, m_volume_shader);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }
}

