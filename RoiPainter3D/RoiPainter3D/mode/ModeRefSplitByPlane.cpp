#pragma unmanaged
#include "ModeRefSplitByPlane.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"
#include "time.h"
#include "LogCore.h"
#include "OglImage.h"

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormRefSplitByPlane.h"
#include "FormMaskIDselection.h"
#include "climessagebox.h"

#pragma unmanaged

using namespace RoiPainter3D;

ModeRefSplitByPlane::~ModeRefSplitByPlane()
{
}


ModeRefSplitByPlane::ModeRefSplitByPlane() :
  m_volume_shader("shader/volVtx.glsl", "shader/volFlg_Seg.glsl"),
  m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
  m_bL = m_bR = m_bM = false;
  m_trgt_maskid = 0;
  m_cp_radi = 1.0f;
}


bool ModeRefSplitByPlane::CanLeaveMode()
{
  if (!m_is_updated) return true;

  if (CLI_MessageBox_YESNO_Show("Do you want to leave without saving current points?", "message"))
  {
    return true;
  }
  return false;
}


//volFlg
// 0  : locked and never changed, 
// 1  : back  
//255 : fore 
void ModeRefSplitByPlane::StartMode()
{
  m_trgt_maskid = formMaskIdSelection_DoSingleSelection(
    ImageCore::GetInst()->GetMaskData(),
    ImageCore::GetInst()->GetActiveMaskID());

  if (m_trgt_maskid < 0) 
  {
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }

  if (m_trgt_maskid == 0) 
  {
    CLI_MessageBox_OK_Show("Cant modify mask ID = 0", "message");
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }

  formRefSplitByPlane_Show();

  const int   nun_voxels = ImageCore::GetInst()->GetNumVoxels();
  const byte* msk3d      = ImageCore::GetInst()->m_vol_mask.GetVolumePtr();
  byte*       flg3d      = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();

  for (int i = 0; i < nun_voxels; ++i)
    flg3d[i] = (msk3d[i] == m_trgt_maskid) ? 255 : 0;

  ImageCore::GetInst()->m_vol_flag.SetUpdated();

  EVec3f p = ImageCore::GetInst()->GetPitch();
  m_cp_radi = (p[0] + p[1] + p[2]);
  m_cps.clear();

  m_b_drawstroke = false;
  m_stroke.clear();
  m_drag_cpid = -1;

  m_cp_sphere.InitializeIcosaHedron(m_cp_radi);
  m_is_updated = false;
}




void ModeRefSplitByPlane::cancelSegmentation()
{
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  RedrawScene();
}




void ModeRefSplitByPlane::finishSegmentation()
{
  const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
  byte* msk3d = ImageCore::GetInst()->m_vol_mask.GetVolumePtr();
  byte* flg3d = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();


  //一旦 トリムされた画素のIDを0に
  for (int i = 0; i < num_voxels; ++i)
  {
    if (msk3d[i] == m_trgt_maskid && flg3d[i] != 255) msk3d[i] = 0;
  }
  ImageCore::GetInst()->ClearMaskSurface(m_trgt_maskid);

  //トリムされた領域を新しい領域として追加
  bool bTrimRegionExist = false;
  for (int i = 0; i < num_voxels; ++i)
  {
    if (flg3d[i] == 1) bTrimRegionExist = true;
    flg3d[i] = (flg3d[i] == 1) ? 255 : 0;
  }
  if (bTrimRegionExist)
    ImageCore::GetInst()->StoreForegroundAsNewMask();

  m_is_updated = false;
  ImageCore::GetInst()->m_vol_mask.SetUpdated();
  ImageCore::GetInst()->m_vol_flag.SetUpdated();
  ModeCore ::GetInst()->ModeSwitch(MODE_VIS_MASK);
  RedrawScene();
}


int ModeRefSplitByPlane::PickCPs(EVec3f &ray_pos, EVec3f &ray_dir)
{
  int idx = -1;
  float depth = FLT_MAX;
  for (int i = 0; i < m_cps.size(); ++i)
  {
    if (DistRayAndPoint(ray_pos, ray_dir, m_cps[i]) > m_cp_radi) continue;
    float d = Dist(ray_pos, m_cps[i]);
    if (d < depth)
    {
      depth = d;
      idx = i;
    }
  }
  return idx;
}


////////////////////////////////////////////////////////////////////////
//Mouse Listener ///////////////////////////////////////////////////////

void ModeRefSplitByPlane::LBtnDclk(const EVec2i& p, OglForCLI* ogl) 
{
  EVec3f ray_pos, ray_dir;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  int pick_idx = PickCPs(ray_pos, ray_dir);
  if (pick_idx != -1)
  {
    m_cps.erase(m_cps.begin() + pick_idx);
    RedrawScene();
    return;
  }

  EVec3f pos;
  if (PickCrssec(ray_pos, ray_dir, &pos) != CRSSEC_NON)
  {
    m_cps.push_back(pos);
    RedrawScene();
  }

}


void ModeRefSplitByPlane::RBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeRefSplitByPlane::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}

void ModeRefSplitByPlane::LBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = true;

  if (IsCtrKeyOn())
  {
    m_b_drawstroke = true;
    m_stroke.clear();
    return;
  }

  //pick to activate and drag 
  EVec3f ray_pos, ray_dir;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  m_drag_cpid = PickCPs(ray_pos, ray_dir);
  if (m_drag_cpid == -1)
  {
    ogl->BtnDown_Trans(p);
  }

  RedrawScene();
}


void ModeRefSplitByPlane::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  if (m_b_drawstroke)
  {
    EVec3f cuboid = ImageCore::GetInst()->GetCuboid();
    CrssecCore::GetInst()->GenerateCurvedCrssec(cuboid, ogl->GetCamPos(), m_stroke);
  }

  ogl->BtnUp();
  m_bL = false;
  m_b_drawstroke = false;
  m_drag_cpid = -1;

  RedrawScene();
}


void ModeRefSplitByPlane::RBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  ogl->BtnDown_Rot(p);
  m_bR = true;
}



void ModeRefSplitByPlane::RBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  ogl->BtnUp();
  m_bR = false;
  RedrawScene();
}



void ModeRefSplitByPlane::MBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  ogl->BtnDown_Zoom(p);
  m_bM = true;
}



void ModeRefSplitByPlane::MBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  ogl->BtnUp();
  m_bM = false;
  RedrawScene();
}



void ModeRefSplitByPlane::MouseMove(const EVec2i& p, OglForCLI* ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  EVec3f ray_p, ray_d;
  ogl->GetCursorRay(p, ray_p, ray_d);

  if (m_b_drawstroke)
  {
    m_stroke.push_back(ray_p + 0.1f * ray_d);
  }
  else if (m_drag_cpid != -1)
  {
    EVec3f p;
    if (PickCrssec(ray_p, ray_d, &p) != CRSSEC_NON)
      m_cps[m_drag_cpid] = p;
  }
  else
  {
    ogl->MouseMove(p);
  }

  RedrawScene(false);

}



void ModeRefSplitByPlane::MouseWheel(const EVec2i& p, short zDelta, OglForCLI* ogl)
{
  if (!PickToMoveCrossSecByWheeling(p, ogl, zDelta))
    ogl->ZoomCamera(zDelta * 0.1f);
  RedrawScene();
}



void ModeRefSplitByPlane::KeyDown(int nChar){}
void ModeRefSplitByPlane::KeyUp(int nChar) {}


void ModeRefSplitByPlane::DrawScene(
    const EVec3f& cuboid, 
    const EVec3f& cam_pos, 
    const EVec3f& cam_center)
{
  //bind volumes ---------------------------------------
  BindAllVolumes();

  //draw cut stroke 
  if (m_b_drawstroke) DrawPolyLine(EVec3f(1, 1, 0), 3, m_stroke);

  //draw planes
  const EVec3i reso = ImageCore::GetInst()->GetResolution();
  DrawCrossSections(cuboid, reso, m_crssec_shader);

  glDisable(GL_CULL_FACE);
  glColor3d(1, 1, 1);
  m_crssec_shader.Bind(0, 1, 2, 3, 6, reso, false, !IsSpaceKeyOn());
  m_plane.Draw();
  m_crssec_shader.Unbind();

  //volume rendering
  if (formVisParam_bRendVol() && !IsSpaceKeyOn())
  {
    const bool   b_pse = formVisParam_bDoPsued();
    const bool   b_roi = formVisParam_GetOtherROI();
    const float  alpha = formVisParam_getAlpha();
    const bool   b_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int    n_slice = (int)((b_manip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    m_volume_shader.Bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, cam_pos, b_pse, b_roi);
    t_DrawCuboidSlices(n_slice, cam_pos, cam_center, cuboid);
    m_volume_shader.Unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }


  //draw control points
  glDisable(GL_LIGHTING);
  glColor3d(0.8, 0.1, 0.1);
  for (const auto& it : m_cps)
  {
    glTranslated( it[0],  it[1],  it[2]);
    m_cp_sphere.Draw();
    glTranslated(-it[0], -it[1], -it[2]);
  }
  
  glDisable(GL_LIGHTING);
  //draw plane 
  float c = 10.0f;
  EVec3f v1 = m_plane_pos + c * m_plane_x;
  EVec3f v2 = m_plane_pos + c * m_plane_y;
  EVec3f v3 = m_plane_pos + c * m_plane_norm;
  glBegin(GL_LINES);
  glColor3d(1, 0, 0); glVertex3fv(m_plane_pos.data()); glVertex3fv(v1.data());
  glColor3d(0, 1, 0); glVertex3fv(m_plane_pos.data()); glVertex3fv(v2.data());
  glColor3d(0, 0, 1); glVertex3fv(m_plane_pos.data()); glVertex3fv(v3.data());
  glEnd();


}


void ModeRefSplitByPlane::GeneratePlaneFromPoints()
{
  if (m_cps.size() < 3)
  {
    CLI_MessageBox_OK_Show("Place at least 3 points (3点以上の点を置いてください", "message");
    return;
  }

  //step compute plane  
  m_plane_pos = CalcGravityCenter(m_cps);
  EMat3f A = CalcCovMatrix(m_cps);
  Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> solver(A);

  EVec3f v = solver.eigenvalues();
  int minidx = min3id(v[0], v[1], v[2]);
  m_plane_norm = solver.eigenvectors().col(minidx); //最も分散が小さい方向
  if (m_plane_norm.norm() != 0) m_plane_norm.normalize();

  //generate plane mesh 
  const EVec3f c = ImageCore::GetInst()->GetCuboid();
  std::vector<EVec3f> Vs(4);
  std::vector<TPoly> Ps = { TPoly(0, 1, 2), TPoly(0, 2, 3) };
  int maxidx = max3id(abs(m_plane_norm[0]), abs(m_plane_norm[1]), abs(m_plane_norm[2]));

  if (maxidx == 0) // 法線がx軸方向を向いている
  {
    EVec3f x = EVec3f(1, 0, 0);
    IntersectRayToPlane(EVec3f(0, 0  ,  0 ), x, m_plane_pos, m_plane_norm, Vs[0]);
    IntersectRayToPlane(EVec3f(0,c[1],  0 ), x, m_plane_pos, m_plane_norm, Vs[1]);
    IntersectRayToPlane(EVec3f(0,c[1],c[2]), x, m_plane_pos, m_plane_norm, Vs[2]);
    IntersectRayToPlane(EVec3f(0, 0  ,c[2]), x, m_plane_pos, m_plane_norm, Vs[3]);
    m_plane_x = EVec3f(0, 1, 0);
  }
  else if (maxidx == 1)// 法線がy軸方向を向いている
  {
    EVec3f y = EVec3f(0, 1, 0);
    IntersectRayToPlane(EVec3f( 0  ,0,  0 ), y, m_plane_pos, m_plane_norm, Vs[0]);
    IntersectRayToPlane(EVec3f(c[0],0,  0 ), y, m_plane_pos, m_plane_norm, Vs[1]);
    IntersectRayToPlane(EVec3f(c[0],0,c[2]), y, m_plane_pos, m_plane_norm, Vs[2]);
    IntersectRayToPlane(EVec3f( 0  ,0,c[2]), y, m_plane_pos, m_plane_norm, Vs[3]);
    m_plane_x = EVec3f(1, 0, 0);
  }
  else
  {
    EVec3f z = EVec3f(0, 0, 1);
    IntersectRayToPlane(EVec3f(  0 ,  0 ,0), z, m_plane_pos, m_plane_norm, Vs[0]);
    IntersectRayToPlane(EVec3f(  0 ,c[1],0), z, m_plane_pos, m_plane_norm, Vs[1]);
    IntersectRayToPlane(EVec3f(c[0],c[1],0), z, m_plane_pos, m_plane_norm, Vs[2]);
    IntersectRayToPlane(EVec3f(c[0],  0 ,0), z, m_plane_pos, m_plane_norm, Vs[3]);
    m_plane_x = EVec3f(1, 0, 0);
  }
  std::cout << Vs[0] << Vs[1] << Vs[2] << Vs[3];
  m_plane.Initialize(Vs, Ps);
  for (int i = 0; i < m_plane.m_vSize; ++i)
  {
    m_plane.m_vTexCd[i][0] = m_plane.m_vVerts[i][0] / c[0];
    m_plane.m_vTexCd[i][1] = m_plane.m_vVerts[i][1] / c[1];
    m_plane.m_vTexCd[i][2] = m_plane.m_vVerts[i][2] / c[2];
  }

  m_plane_x += (-m_plane_x.dot(m_plane_norm)) * m_plane_norm;
  if (m_plane_x.norm() != 0) m_plane_x.normalize();
  m_plane_y = m_plane_norm.cross(m_plane_x);

}




void ModeRefSplitByPlane::FlipPlaneNormal()
{
  m_plane_norm *= -1;
  m_plane_y = m_plane_norm.cross(m_plane_x);
  RedrawScene();
}


void ModeRefSplitByPlane::SplitVolumeByPlane() 
{
  int W, H, D, WH, WHD;
  std::tie(W, H, D, WH, WHD) = ImageCore::GetInst()->GetResolution5();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();
  byte* flg3d = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();


#pragma omp parallel for
  for (int z = 0; z < D; ++z)
  {
    for (int y = 0; y < H; ++y)
    {
      for (int x = 0; x < W; ++x)
      {
        const int I = x + y * W + z * WH;
        if (flg3d[I] == 0) continue;
        EVec3f p((x + 0.5f) * pitch[0], 
                 (y + 0.5f) * pitch[1],
                 (z + 0.5f) * pitch[2]);
        float d = m_plane_norm.dot(p - m_plane_pos);
        flg3d[I] = (d >= 0) ? 255 : 1;
      }
    }
  }
  ImageCore::GetInst()->m_vol_flag.SetUpdated();
  m_is_updated = true;
  RedrawScene();
}

inline bool isInCube(const EVec3f& p, const EVec3f& cube)
{
  return 0 <= p[0] && 0 <= p[1] && 0 <= p[2] &&
    p[0] <= cube[0] && p[1] <= cube[1] && p[1] <= cube[1];
}

inline float CalcHeight(
  const EVec3f &piv,
  const EVec3f& dir,
  const EVec3f& cube,
  const float  &pitch,
  const byte   *vol_flg)
{
  float height = 0.0f;
  float h = 0;

  EVec3f p = piv;
  while (true)
  {
    p += pitch * dir;
    h += pitch;
    if (!isInCube(p, cube)) break;
    int I = ImageCore::GetInst()->GetVoxelIndex(p);
    if (vol_flg[I] == 255) height = h;
  }
  return height;
}



void ModeRefSplitByPlane::AnalysisWithCurrentPlane(std::string fname) 
{
  const EVec3i reso  = ImageCore::GetInst()->GetResolution();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();
  const EVec3f cube  = ImageCore::GetInst()->GetCuboid();
  const byte* vflg = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();
  const byte* vol  = ImageCore::GetInst()->m_vol.GetVolumePtr();

  //画像の解像度・ピッチ・中心・左上を取得 (中心 -->cuboidの中心を平面に射影)
  int W = reso[0], H = reso[1];
  float px = pitch[0], py = pitch[1];
  EVec3f center = ProjectPointToPlane(m_plane_pos, m_plane_norm, 0.5f * cube);
  EVec3f piv = center - (px * W / 2.0f) * m_plane_x
                      - (py * H / 2.0f) * m_plane_y;

  byte* img_val = new byte[W * H];
  float* img_height = new float[W * H];
  memset(img_val, 0, sizeof(byte) * W * H);
  memset(img_height, 0, sizeof(float) * W * H);

  for (int y = 0; y < H; ++y)
  {
    for (int x = 0; x < W; ++x)
    {
      EVec3f pos = piv + x * px * m_plane_x + y * py * m_plane_y;
      EVec4i vox_idx = ImageCore::GetInst()->GetVoxelIndex4i(pos);
      if (!isInCube(pos, cube))continue;

      int I = y * W + x;
      img_val   [I] = vol[vox_idx[3]];
      img_height[I] = CalcHeight(pos, m_plane_norm, cube, 0.5f * px, vflg);
    }
  }

  //画像を作成してセーブ
  OGLImage2D1 oglimg_val;
  OGLImage2D4 oglimg_height;
  oglimg_val.Allocate(W, H);
  oglimg_height.Allocate(W, H);
  int WH = W * H;
  
  float max_height = 0;
  for (int i = 0; i < WH; ++i) 
    max_height = std::max(max_height, img_height[i]);


  for (int i = 0; i < WH; ++i)
  {
    oglimg_val[i] = img_val[i];

    oglimg_height[4 * i] = oglimg_height[4 * i + 1] = oglimg_height[4 * i + 2] = 0;

    float h = img_height[i] / max_height;
    if (h >= 0.5f)
    {
      oglimg_height[4 * i + 0] = 255;
      oglimg_height[4 * i + 1] = (byte)((h - 0.5f) * 2.0f * 255);
    }
    else if (h > 0)
    {
      oglimg_height[4 * i + 0] = (byte)(h * 2.0f * 255);
    }
  }

  std::string fname1 = fname + "_val.png";
  std::string fname2 = fname + "_height.png";
  oglimg_val.SaveAs(fname1.c_str());
  oglimg_height.SaveAs(fname2.c_str());

}
