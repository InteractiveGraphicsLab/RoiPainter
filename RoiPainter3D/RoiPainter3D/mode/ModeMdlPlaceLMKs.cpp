#pragma unmanaged
#include "ModeMdlPlaceLMKs.h"
#include "OglForCLI.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "ModeCore.h"

#pragma managed
#include "../FormMdlPlaceLMKs.h"
#include "../FormVisParam.h"
#include "../FormMain.h"
#include "FormMaskIDselection.h"
#pragma unmanaged

#include "tmarchingcubes.h"
#include "tcolor.h"

using namespace RoiPainter3D;
using namespace marchingcubes;

ModeMdlPlaceLMKs::ModeMdlPlaceLMKs()
{
  m_bL = m_bR = m_bM = false;

}

static int PickLandmark(const EVec3f& ray_pos, const EVec3f& ray_dir, const std::vector<EVec3f>& lmk, const float lmk_radius) {
  float min_depth = FLT_MAX;
  int min_ID = -1;

  for (int i = 0; i < lmk.size(); i++) {
    if (DistRayAndPoint(ray_pos, ray_dir, lmk[i]) > lmk_radius) {
      continue;
    }

    float d = (ray_pos - lmk[i]).norm();
    if (d < min_depth) {
      min_ID = i;
      min_depth = d;
    }
  }
  return min_ID;
}

static int EraseLandmark(const EVec3f& ray_pos, const EVec3f& ray_dir, std::vector<EVec3f>& lmk, const float lmk_radius) {
  int lmk_ID = PickLandmark(ray_pos, ray_dir, lmk, lmk_radius);
  if (lmk_ID != -1) {
    lmk.erase(lmk.begin() + lmk_ID);
    return true;
  }
  return false;
}


void ModeMdlPlaceLMKs::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = false;
  ogl->BtnUp();

  m_drag_lmk_ID = -1;

  RedrawScene();
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
  
  if (IsShiftKeyOn()) {
    EVec3f rayPos, rayDir, pos;
    ogl->GetCursorRay(p, rayPos, rayDir);

    m_drag_lmk_ID = PickLandmark(rayPos, rayDir, m_lmk, m_lmk_radius);
    if (m_drag_lmk_ID != -1) return;

    if (PickIsoSurface(rayPos, rayDir, pos)) {
      m_lmk.push_back(pos);
      m_drag_lmk_ID = (int)m_lmk.size() - 1;
    }
  } else {
    ogl->BtnDown_Trans(p);
  }

}

void ModeMdlPlaceLMKs::RBtnDown(const EVec2i& p, OglForCLI* ogl) 
{
  m_bR = true;
  if (IsShiftKeyOn()) {
      EVec3f ray_pos, ray_dir, pos;
      ogl->GetCursorRay(p, ray_pos, ray_dir);

      if (EraseLandmark(ray_pos, ray_dir, m_lmk, m_lmk_radius)) {
          RedrawScene();
      }
  } else {
      ogl->BtnDown_Rot(p);
  }
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
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if (m_drag_lmk_ID != -1) {
    if (PickIsoSurface(ray_pos, ray_dir, pos)) {
		m_lmk[m_drag_lmk_ID] = pos;
    }
  }


  ogl->MouseMove(p);

  RedrawScene();
}



void ModeMdlPlaceLMKs::MouseWheel(const EVec2i& p, short z_delta, OglForCLI* ogl) {}

void ModeMdlPlaceLMKs::KeyDown(int nChar) {}
void ModeMdlPlaceLMKs::KeyUp(int nChar) {}


bool ModeMdlPlaceLMKs::CanLeaveMode()
{
  return true;
}

void ModeMdlPlaceLMKs::StartMode()
{
  ImageCore::GetInst()->InitializeVolFlgByLockedMask();

  m_bL = m_bR = m_bM = false;
  formMdlPlaceLMKs_Show();

  m_lmk_radius = ImageCore::GetInst()->GetPitch()[0];
  m_lmk_mesh.InitializeAsSphere(m_lmk_radius, 10, 10);

  formVisParam_UnlockPitchBox();
}

void ModeMdlPlaceLMKs::FinishSegmentation() {
  ImageCore::GetInst()->StoreForegroundAsNewMask();
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  RedrawScene();
}

static void DrawColoredLandmark(const TMesh& lmk_mesh, const std::vector<EVec3f>& lmk) {
  static const int NUM_COL = 9;
  static float COLOR[9][4] = {
    {1.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f, 0.5f},
    {1.0f, 0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.5f, 0.5f}, {0.5f, 0.0f, 1.0f, 0.5f},
    {0.5f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.5f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.5f, 0.5f}
  };

  for (int i = 0; i < lmk.size(); i++) {
    float* c = COLOR[i % NUM_COL];
    glPushMatrix();
    glTranslated(lmk[i][0], lmk[i][1], lmk[i][2]);

    lmk_mesh.Draw(c, c, COLOR_W, COLOR_SHIN64);
    glPopMatrix();
  }
}

static float COLOR_HY[4] = { 0.3f, 0.4f, 0.1f, 0.3f };

void ModeMdlPlaceLMKs::DrawScene(const EVec3f& cam_pos, const EVec3f& cam_center)
{
  BindAllVolumes();

  DrawCrsSec_Standard();

  glEnable(GL_LIGHTING);
  DrawColoredLandmark(m_lmk_mesh, m_lmk);


  m_isosurface.Draw(COLOR_HY, COLOR_HY, COLOR_W, COLOR_SHIN64);

  if (formVisParam_bRendVol())
  {
    bool on_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolume_Standard(cam_pos, cam_center, on_manip);
  }

}



static void GenIsoSurFaceHalf(const EVec3i reso, const EVec3f pitch, const short* volume, const int isovalue, TTriangleSoup& mesh) {
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

void ModeMdlPlaceLMKs::GenIsoSurface(const int isovalue) {
  m_isovalue = isovalue;

  const EVec3i reso = ImageCore::GetInst()->GetResolution();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();
  bool do_halfen = reso[0] % 2 == 0 && reso[1] % 2 == 0 && reso[2] % 2 == 0;

  short* volume = ImageCore::GetInst()->m_vol_orig;

  if (do_halfen)
    GenIsoSurFaceHalf(reso, pitch, volume, isovalue, m_isosurface);
  else
    MarchingCubesPolygonSoup(reso, pitch, volume, isovalue, 0, 0, m_isosurface);
}



bool ModeMdlPlaceLMKs::PickIsoSurface(const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f& pos) {
  EVec3f p;
  bool pick = m_isosurface.PickByRay(ray_pos, ray_dir, p);

  pos = p;
  return pick;
}

void ModeMdlPlaceLMKs::ExportLandmarks(std::string fname) {
  std::ofstream ofs(fname.c_str());
  if (!ofs.is_open()) {
    std::cout << "error when opening " << fname << "\n";
    return;
  }

  ofs << (int)m_lmk.size() << "\n";
  for (int i = 0; i < m_lmk.size(); i++) {
    WriteToFstream(ofs, m_lmk[i]);
  }
  ofs.close();
}

void ModeMdlPlaceLMKs::ImportLandmarks(std::string fname) {
  std::ifstream ifs(fname);
  if (ifs.fail()) {
    std::cout << "file open error" << "\n";
    return;
  }

  m_lmk.clear();
  int num_lmk;

  ifs >> num_lmk;
  m_lmk.resize(num_lmk);
  for (int i = 0; i < m_lmk.size(); i++) {
    ReadFromFstream(ifs, m_lmk[i]);
  }
  ifs.close();
  RedrawScene();
}



