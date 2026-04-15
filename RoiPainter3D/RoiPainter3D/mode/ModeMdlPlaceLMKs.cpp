#pragma unmanaged
#include "ModeMdlPlaceLMKs.h"
#include "OglForCLI.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"

#pragma managed
#include "../FormMdlPlaceLMKs.h"
#include "../FormVisParam.h"
#include "../FormMain.h"
#pragma unmanaged

#include "tmarchingcubes.h"

using namespace RoiPainter3D;
using namespace marchingcubes;

ModeMdlPlaceLMKs::ModeMdlPlaceLMKs()
{
  m_bL = m_bR = m_bM = false;

}


bool ModeMdlPlaceLMKs::CanLeaveMode()
{
  return true;
}

void ModeMdlPlaceLMKs::StartMode() 
{
  m_bL = m_bR = m_bM = false;
  formMdlPlaceLMKs_Show();
}


static void GenIsoSurFaceHalf(const EVec3i reso, const EVec3f pitch, const short* volume, const int isovalue, TTriangleSoup &mesh) {
  const int W = reso[0], H = reso[1], D = reso[2];
  const int hW = W / 2, hH = H / 2, hD = D / 2;
  EVec3i rh(hW, hH, hD);
  EVec3f ph(pitch[0] * 2, pitch[1] * 2, pitch[2] * 2);

  short* vh = new short[hW * hH * hD];
  for (int z = 0; z < hD; ++z)
    for (int y = 0; y < hH; ++y)
      for (int x = 0; x < hW; ++x)
        vh[z * hW * hH + y * hW + x] = volume[2 * z * W * H + 2 * y * W + 2 * x];
  MarchingCubesPolygonSoup(rh, ph, vh, isovalue, 0, 0, mesh);

  delete[] vh;
}

void ModeMdlPlaceLMKs::GenIsoSurFace(const int isovalue) {
  m_isovalue = isovalue;

  const EVec3i reso  = ImageCore::GetInst()->GetResolution();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();
  bool do_halfen = reso[0] % 2 == 0 && reso[1] % 2 == 0 && reso[2] % 2 == 0;

  short* volume = ImageCore::GetInst()->m_vol_orig;

  if (do_halfen)
    GenIsoSurFaceHalf(reso, pitch, volume, isovalue, m_isosurface);
  else
    MarchingCubesPolygonSoup(reso, pitch, volume, isovalue, 0, 0, m_isosurface);
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