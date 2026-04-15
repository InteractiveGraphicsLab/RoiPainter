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
#include "tcolor.h"

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

  m_lmkRad = ImageCore::GetInst()->GetPitch()[0];
  m_lmkMesh.InitializeAsSphere(m_lmkRad, 10, 10);

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


bool ModeMdlPlaceLMKs::PickIsosurface(const EVec3f& rayPos, const EVec3f& rayDir, EVec3f& pos) {
  EVec3f p;
  bool pick = m_isosurface.PickByRay(rayPos, rayDir, p);

  pos = p;
  return pick;
}

static int PickLMK(const EVec3f& rayPos, const EVec3f& rayDir, const std::vector<EVec3f>& lmk, const float lmkRad) {
    float minDepth = FLT_MAX;
	int minIdx = -1;

    for (int i = 0; i < lmk.size(); i++) {
        if (DistRayAndPoint(rayPos, rayDir, lmk[i]) > lmkRad) {
			continue;
        }

        float d = (rayPos - lmk[i]).norm();
        if (d < minDepth) {
            minIdx = i;
			minDepth = d;
        }
    }
    return minIdx;
}

static int PickToEraseLMK(const EVec3f& rayPos, const EVec3f& rayDir, std::vector<EVec3f>& lmk, const float lmkRad) {
    int lmkIdx = PickLMK(rayPos, rayDir, lmk, lmkRad);
    if (lmkIdx != -1) {
        lmk.erase(lmk.begin() + lmkIdx);
        return true;
    }
    return false;
}


void ModeMdlPlaceLMKs::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = false;
  ogl->BtnUp();

  m_dragLmkIdx = -1;

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

    m_dragLmkIdx = PickLMK(rayPos, rayDir, m_lmk, m_lmkRad);
    if (m_dragLmkIdx != -1) return;

    if (PickIsosurface(rayPos, rayDir, pos)) {
      m_lmk.push_back(pos);
      m_dragLmkIdx = (int)m_lmk.size() - 1;
    }
  } else {
    ogl->BtnDown_Trans(p);
  }

}

void ModeMdlPlaceLMKs::RBtnDown(const EVec2i& p, OglForCLI* ogl) 
{
  m_bR = true;
  if (IsShiftKeyOn()) {
      EVec3f rayPos, rayDir, pos;
      ogl->GetCursorRay(p, rayPos, rayDir);

      if (PickToEraseLMK(rayPos, rayDir, m_lmk, m_lmkRad)) {
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
  EVec3f rayPos, rayDir, pos;
  ogl->GetCursorRay(p, rayPos, rayDir);

  if (m_dragLmkIdx != -1) {
    if (PickIsosurface(rayPos, rayDir, pos)) {
		m_lmk[m_dragLmkIdx] = pos;
    }
  }


  ogl->MouseMove(p);

  RedrawScene();
}



void ModeMdlPlaceLMKs::MouseWheel(const EVec2i& p, short z_delta, OglForCLI* ogl) {}

void ModeMdlPlaceLMKs::KeyDown(int nChar) {}
void ModeMdlPlaceLMKs::KeyUp(int nChar) {}


static void DrawColoredLMKs(const TMesh& lmkMesh, const std::vector<EVec3f>& lmk) {
  static const int NUM_COL = 9;//今のところ9色
  static float COLOR[9][4] = {
    {1.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f, 0.5f},
    {1.0f, 0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.5f, 0.5f}, {0.5f, 0.0f, 1.0f, 0.5f},
    {0.5f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.5f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.5f, 0.5f}
  };

  for (int i = 0; i < lmk.size(); i++) {
    float* c = COLOR[i % NUM_COL];
    glPushMatrix();
    glTranslated(lmk[i][0], lmk[i][1], lmk[i][2]);

    lmkMesh.Draw(c, c, COLOR_W, COLOR_SHIN64);
    glPopMatrix();
  }
}

static float COLOR_HY[4] = { 0.3f, 0.4f, 0.1f, 0.3f };
static float COLOR_RB[4] = { 0.8f, 0.2f, 0.8f, 0.3f };

void ModeMdlPlaceLMKs::DrawScene(const EVec3f& cam_pos, const EVec3f& cam_center) 
{
  BindAllVolumes();

  DrawCrsSec_Standard();

  glEnable(GL_LIGHTING);
  DrawColoredLMKs(m_lmkMesh, m_lmk);


  m_isosurface.Draw(COLOR_HY, COLOR_HY, COLOR_W, COLOR_SHIN64);

  if (formVisParam_bRendVol())
  {
    bool on_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolume_Standard(cam_pos, cam_center, on_manip);
  }

}

