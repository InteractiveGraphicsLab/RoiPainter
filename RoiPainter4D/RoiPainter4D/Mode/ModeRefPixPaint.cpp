

#pragma unmanaged 
#include "ModeRefPixPaint.h"

#pragma managed 
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormRefPixPaint.h"
#include "FormSelectMskId.h"
#include "FormVisMask.h"
#include "CliMessageBox.h"
#pragma unmanaged 

#include "ImageCore.h"
#include "CrsSecCore.h"
#include "ModeCore.h"
#include "ModeCommonTools.h"
#include "tqueue.h"

using namespace RoiPainter4D;


ModeRefPixPaint::ModeRefPixPaint()
{
  std::cout << "ModeRefPixPaint...\n";
  m_bL = m_bR = m_bM = false;
  m_b_drawlasso = false;
  m_b_modified = false;

  std::cout << "ModeRefPixPaint DONE\n";
}


ModeRefPixPaint::~ModeRefPixPaint()
{
}


bool ModeRefPixPaint::CanEndMode()
{
  if (!m_b_modified) return true;

  else return ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Leaving?");
  return true;
}



void ModeRefPixPaint::StartMode()
{
  const int fi = formVisParam_getframeI();

  m_bL = m_bR = m_bM = false;
  m_b_drawlasso = false;
  m_b_modified = false;
  m_trgt_crssecid = CRSSEC_NON;
  m_b_draw_crssec = true;
  m_lasso.clear();
  m_lasso_previous.clear();
  m_msk3D_pre_fi = fi;

  formRefPixPaint_Show();
  EVec2i minmax = ImageCore::GetInst()->GetVolumeMinMax();
  formRefPixPaint_InitAllItems(minmax[0], minmax[1]);

  //get target mask id
  int maskID = ImageCore::GetInst()->GetSelectMaskIdx();
  if (maskID == -1)
  {
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }

  if (maskID == 0)
  {
    ShowMsgDlg_OK("0th region (background) cannot be trimmed", "caution");
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }

  const auto& maskdata = ImageCore::GetInst()->GetMaskData();
  if (maskdata[maskID].lock)
  {
    ShowMsgDlg_OK("Lockのかかったmask IDを変更しようとしています", "caution");
  }


  formRefPixPaint_Show();

  //initialzie vFlg
  ImageCore::GetInst()->InitializeFlg4dByMask(formMain_SetProgressValue);

  //update flg 4d
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const std::vector<byte*>& flg4d = ImageCore::GetInst()->m_flg4d;
  std::vector<byte*>& mask4d = ImageCore::GetInst()->m_mask4d;

#pragma omp parallel for
  for (int f = 0; f < num_frames; ++f)
  {
    byte* flg3d = flg4d[f];
    byte* msk3d = mask4d[f];
    for (int i = 0; i < num_voxels; ++i)
    {
      //flg3d[i] = (msk3d[i] == maskID) ? 255 : 1;
      if (msk3d[i] == maskID)flg3d[i] = 255;
    }
  }

  //4D volume (cpu) --> vis volume (gpu)
  UpdateImageCoreVisVolumes();


  if (m_msk3D_pre) delete[] m_msk3D_pre;
  m_msk3D_pre = new byte[num_voxels];
  memcpy(m_msk3D_pre, &flg4d[fi][0], sizeof(byte) * num_voxels);

  std::cout << "finish start mode!!!!\n";
}




///////////////////////////////////////////////////////////////////////////////
//Functions for Lasso /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template<class T>
static bool IsInRange(const T& val, const T& minv, const T& maxv) {
  return minv <= val && val <= maxv;
}

//注目している点が lasso の内側であるか判定
static bool t_IsInsideLasso
(
  const EVec3f& pos,
  const std::vector<EVec3f>& lasso_stroke,
  const CRSSEC_ID& id
)
{
  const int N = (int)lasso_stroke.size();

  EVec3f axis = (id == CRSSEC_XY) ? EVec3f(0, 0, 1) :
    (id == CRSSEC_YZ) ? EVec3f(1, 0, 0) : EVec3f(0, 1, 0);

  double sum = 0;
  for (int i = 0; i < N; ++i)
  {
    sum += CalcAngle(lasso_stroke[i] - pos, lasso_stroke[(i == N - 1) ? 0 : i + 1] - pos, axis);
  }

  return fabs(2 * M_PI - fabs(sum)) < M_PI * 0.5;
}



//lassoの内側にあるvoxelを fore/backに変更
//b_fore = true  --> vFlg 1   --> 255に
//b_fore = false --> vFlg 255 --> 1に
void ModeRefPixPaint::t_addPixsInsideLasso
(
  const CRSSEC_ID       id,
  const EVec3i          reso,
  const EVec3f          pitch,
  const std::vector<EVec3f>& lasso_stroke
)
{
  const int     fi = formVisParam_getframeI();
  std::vector<byte*>& flg4d = ImageCore::GetInst()->m_flg4d;
  const short* img3d = ImageCore::GetInst()->m_img4d[fi];
  std::vector<byte*>& img4D_flg = ImageCore::GetInst()->m_flg4d;
  byte* vol_flg = flg4d[fi];

  const short maxv = formRefPixPaint_maxThreshValue();
  const short minv = formRefPixPaint_minThreshValue();

  const int W = reso[0];
  const int H = reso[1];
  const int D = reso[2], WH = W * H;
  std::vector<EVec3f> lasso;
  const int new_num = std::max(10, std::min((int)lasso_stroke.size(), (int)(VertsLength(lasso_stroke, true) / pitch[0])));

  memcpy(m_msk3D_pre, &img4D_flg[fi][0], sizeof(byte) * reso[0] * reso[1] * reso[2]);
  m_msk3D_pre_fi = fi;

  VertsResampleEqualInterval(new_num, lasso_stroke, lasso);

  //compute bounding box
  EVec3f tmp_min, tmp_max;
  CalcBoundingBox(lasso, tmp_min, tmp_max);
  EVec3i bb_min((int)(tmp_min[0] / pitch[0]), (int)(tmp_min[1] / pitch[1]), (int)(tmp_min[2] / pitch[2]));
  EVec3i bb_max((int)(tmp_max[0] / pitch[0]), (int)(tmp_max[1] / pitch[1]), (int)(tmp_max[2] / pitch[2]));
  bb_min[0] = std::max(0, bb_min[0] - 1);
  bb_min[1] = std::max(0, bb_min[1] - 1);
  bb_min[2] = std::max(0, bb_min[2] - 1);
  bb_max[0] = std::min(W - 1, bb_max[0] + 1);
  bb_max[1] = std::min(H - 1, bb_max[1] + 1);
  bb_max[2] = std::min(D - 1, bb_max[2] + 1);

  if (id == CRSSEC_XY)
  {
    const float zpos = lasso.front()[2];
    const int   zi = (int)(zpos / pitch[2]);

#pragma omp parallel for
    for (int yi = bb_min[1]; yi < bb_max[1]; ++yi)
    {
      for (int xi = bb_min[0]; xi < bb_max[0]; ++xi)
      {
        int idx = xi + yi * W + zi * WH;
        if (vol_flg[idx] == 0 && !formRefPixPaint_bForcedOverwite()) continue;
        if (formRefPixPaint_bPaintMode() && vol_flg[idx] == 255) continue;
        if (formRefPixPaint_bEraseMode() && vol_flg[idx] == 1) continue;

        EVec3f p((xi + 0.5f) * pitch[0], (yi + 0.5f) * pitch[1], zpos);
        if (t_IsInsideLasso(p, lasso, id))
          vol_flg[idx] = formRefPixPaint_bPaintMode() ? 255 :
                        formRefPixPaint_bEraseMode() ? 1 :
                        IsInRange(img3d[idx], minv, maxv) ? 255 : 1;
      }
    }
  }

  if (id == CRSSEC_YZ)
  {
    const float xpos = lasso.front()[0];
    const int   xi = (int)(xpos / pitch[0]);

    #pragma omp parallel for
    for (int zi = bb_min[2]; zi < bb_max[2]; ++zi)
    {
      for (int yi = bb_min[1]; yi < bb_max[1]; ++yi)
      {
        int idx = xi + yi * W + zi * WH;
        if (vol_flg[idx] == 0 && !formRefPixPaint_bForcedOverwite()) continue;
        if (formRefPixPaint_bPaintMode() && vol_flg[idx] == 255) continue;
        if (formRefPixPaint_bEraseMode() && vol_flg[idx] == 1) continue;
        EVec3f p(xpos, (yi + 0.5f) * pitch[1], (zi + 0.5f) * pitch[2]);
        if (t_IsInsideLasso(p, lasso, id))
          vol_flg[idx] = formRefPixPaint_bPaintMode() ? 255 :
          formRefPixPaint_bEraseMode() ? 1 :
          IsInRange(img3d[idx], minv, maxv) ? 255 : 1;
      }
    }
  }

  if (id == CRSSEC_ZX)
  {
    const float ypos = lasso.front()[1];
    const int   yi = (int)(ypos / pitch[1]);

    #pragma omp parallel for
    for (int zi = bb_min[2]; zi < bb_max[2]; ++zi)
    {
      for (int xi = bb_min[0]; xi < bb_max[0]; ++xi)
      {
        int idx = xi + yi * W + zi * WH;
        if (vol_flg[idx] == 0 && !formRefPixPaint_bForcedOverwite()) continue;
        if (formRefPixPaint_bPaintMode() && vol_flg[idx] == 255) continue;
        if (formRefPixPaint_bEraseMode() && vol_flg[idx] == 1) continue;

        EVec3f p((xi + 0.5f) * pitch[0], ypos, (zi + 0.5f) * pitch[2]);
        if (t_IsInsideLasso(p, lasso, id))
          vol_flg[idx] = formRefPixPaint_bPaintMode() ? 255 :
          formRefPixPaint_bEraseMode() ? 1 :
          IsInRange(img3d[idx], minv, maxv) ? 255 : 1;

      }
    }
  }
}





void ModeRefPixPaint::RunRegionGrow3D_OneFrame(const short minv, const short maxv, const EVec3f pos)
{
  const int frameI = formVisParam_getframeI();
  const EVec3i   reso = ImageCore::GetInst()->GetReso();
  std::vector<byte*>& img4D_flg = ImageCore::GetInst()->m_flg4d;
  memcpy(m_msk3D_pre, &img4D_flg[frameI][0], sizeof(byte) * reso[0] * reso[1] * reso[2]);
  m_msk3D_pre_fi = frameI;


  std::cout << "runRegionGrow6...\n";

  int W, H, D, WH, WHD;
  std::tie(W, H, D, WH, WHD) = ImageCore::GetInst()->GetResolution5();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const short* img3d = ImageCore::GetInst()->m_img4d[frameI];
  byte* flg3d = ImageCore::GetInst()->m_flg4d[frameI];

  if (frameI < 0 || num_frames <= frameI)
  {
    std::cout << "strange input!\n";
    return;
  }

  ////volFlg : 0:never change, 1:back, 255:fore//ModeSegRGrow::RunRegionGrow3D_OneFrameではこの処理をするが、ここではしないでおく。
  //for (int i = 0; i < WHD; ++i)
  //{
  //  flg3d[i] = (flg3d[i] == 0) ? 0 : 1;
  //}


  //CP --> pixel id
  TQueue<PixIdx4D> Q;
  const int I = ImageCore::GetInst()->GetVoxelIndex(pos);

  if (flg3d[I] != 0 && IsInRange(img3d[I], minv, maxv))
  {
    Q.push_back(PixIdx4D(I, frameI));
    flg3d[I] = 255;
  }

  //growing
  while (!Q.empty())
  {
    const int I = Q.front().idx;
    const int x = I % W;
    const int y = (I / W) % H;
    const int z = I / WH;
    const int f = Q.front().frameI;
    Q.pop_front();

    //grow to 8 neighbors
    int K;
    K = I - 1;  if (x > 0 && flg3d[K] == 1 && IsInRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I - W;  if (y > 0 && flg3d[K] == 1 && IsInRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I - WH; if (z > 0 && flg3d[K] == 1 && IsInRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I + 1;  if (x < W - 1 && flg3d[K] == 1 && IsInRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I + W;  if (y < H - 1 && flg3d[K] == 1 && IsInRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I + WH; if (z < D - 1 && flg3d[K] == 1 && IsInRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
  }

  std::cout << "runRegionGrow6...DONE\n\n";
}








////////////////////////////////////////////////////////////////////////////
//MouseListener/////////////////////////////////////////////////////////////
void ModeRefPixPaint::LBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = true;

  if (IsShiftKeyOn())
  {
    m_b_drawlasso = false;
    EVec3f ray_pos, ray_dir, pos;
    ogl->GetCursorRay(p, ray_pos, ray_dir);
    m_trgt_crssecid = PickCrssec(ray_pos, ray_dir, pos);
    if (m_trgt_crssecid != CRSSEC_NON) m_b_drawlasso = true;
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }

}



void ModeRefPixPaint::RBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  if (m_bL && m_b_drawlasso)
  {
    m_bL = false;
    m_b_drawlasso = false;
    m_lasso.clear();
  }
  else if (IsShiftKeyOn()&&IsCtrKeyOn())
  {
    EVec3f rayP, rayD, pos;
    ogl->GetCursorRay(p, rayP, rayD);
    if (PickCrssec(rayP, rayD, pos) != CRSSEC_NON) {
      const short maxv = formRefPixPaint_maxThreshValue();
      const short minv = formRefPixPaint_minThreshValue();
      RunRegionGrow3D_OneFrame(minv, maxv, pos);
      UpdateImageCoreVisVolumes();
      formMain_RedrawMainPanel();
      m_b_modified = true;
    }
  }
  m_bR = true;
  ogl->BtnDown_Rot(p);
}

void ModeRefPixPaint::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  const EVec3i   reso = ImageCore::GetInst()->GetReso();
  const EVec3f  pitch = ImageCore::GetInst()->GetPitch();

  if (m_b_drawlasso)
  {
    t_addPixsInsideLasso(m_trgt_crssecid, reso, pitch, m_lasso);
    m_lasso_previous = m_lasso;
    m_b_modified = true;
    UpdateImageCoreVisVolumes();
  }

  m_bL = false;
  m_b_drawlasso = false;
  m_lasso.clear();
  ogl->BtnUp();

  formMain_RedrawMainPanel();
}

void ModeRefPixPaint::RBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeRefPixPaint::MBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}


void ModeRefPixPaint::MBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeRefPixPaint::MouseMove(const EVec2i& p, OglForCLI* ogl)
{
  EVec3f rayP, rayD, pos;
  ogl->GetCursorRay(p, rayP, rayD);
  CRSSEC_ID id = PickCrssec(rayP, rayD, pos);
  if (id != CRSSEC_NON)
  {
    int frame_idx = formVisParam_getframeI();
    short v = ImageCore::GetInst()->GetVoxelValue(frame_idx, pos);
    std::cout << v << "\n";
  }



  if (!m_bL && !m_bR && !m_bM) return;



  if (m_b_drawlasso)
  {
    if (PickCrssec(m_trgt_crssecid, rayP, rayD, pos) != CRSSEC_NON) m_lasso.push_back(pos);
  }
  else
  {
    ogl->MouseMove(p);
  }

  formMain_RedrawMainPanel();
}


void ModeRefPixPaint::MouseWheel(const EVec2i& p, short z_delta, OglForCLI* ogl)
{
  bool wheeling = WheelingCrssec(p, z_delta, ogl);

  if (!wheeling) {
    ogl->ZoomCamera(z_delta * 0.1f);
  }
  else
  {
    EVec3f rayP, rayD, pos;
    ogl->GetCursorRay(p, rayP, rayD);
    const CRSSEC_ID mouse_id = PickCrssec(rayP, rayD, pos);
    if (mouse_id != CRSSEC_NON && mouse_id == m_trgt_crssecid)
    {
      const int crssecid = (m_trgt_crssecid == CRSSEC_XY) ? 2 :
                           (m_trgt_crssecid == CRSSEC_YZ) ? 0 : 1;
      for (int i = 0; i < m_lasso_previous.size(); ++i) { m_lasso_previous[i][crssecid] = pos[crssecid]; }
    }
  }

  formMain_RedrawMainPanel();
}


void ModeRefPixPaint::LBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeRefPixPaint::RBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeRefPixPaint::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}


void ModeRefPixPaint::KeyDown(int nChar)
{
  if (nChar == 'Z' && IsCtrKeyOn())
  {
    const int fi = formVisParam_getframeI();

    if (fi == m_msk3D_pre_fi || ShowMsgDlgYesNo("現在のフレームではないフレームを変更しようとしています．\nよろしいですか？", "caution"))
    {
      std::cout << "undo!!";
      const int      num_voxels = ImageCore::GetInst()->GetNumVoxels();
      std::vector<byte*>& flg4d = ImageCore::GetInst()->m_flg4d;
      memcpy(&flg4d[m_msk3D_pre_fi][0], m_msk3D_pre, sizeof(byte) * num_voxels);
      UpdateImageCoreVisVolumes();

    }
  }
  else if (nChar == VK_TAB)
  {
    m_b_draw_crssec = !m_b_draw_crssec;
  }
  else if (nChar == 'R') {
    std::vector<byte*>& flg4d = ImageCore::GetInst()->m_flg4d;
    const EVec3i   reso = ImageCore::GetInst()->GetReso();
    const EVec3f  pitch = ImageCore::GetInst()->GetPitch();
    t_addPixsInsideLasso(m_trgt_crssecid, reso, pitch, m_lasso_previous);
    UpdateImageCoreVisVolumes();

    formMain_RedrawMainPanel();
  }
}

void ModeRefPixPaint::KeyUp(int nChar)
{
}



void ModeRefPixPaint::DrawScene(const EVec3f& cam_pos, const EVec3f& cam_cnt)
{
  ImageCore::GetInst()->BindAllVolumes();

  if (m_b_drawlasso && m_lasso.size() > 0)
    DrawPolyLine(EVec3f(1, 0, 1), 4, m_lasso, true);

  if (m_lasso_previous.size() > 0 && !IsDKeyOn())
    DrawPolyLine(EVec3f(1, 1, 1), 4, m_lasso_previous, true);

  if (m_b_draw_crssec)
    DrawCrossSectionsVisFore(!IsSKeyOn());

  if (formVisParam_bRendVol())
  {
    DrawVolumeVisFore(!IsSpaceKeyOn(), cam_pos, cam_cnt, FormRefPixPaint_bUseRoughRend());
  }
}








void t_calcBoundBox2(const std::vector<EVec2i>& vs, EVec2i& minBB, EVec2i& maxBB)
{
  if (vs.size() <= 0) return;
  minBB << vs.front().x(), vs.front().y();
  maxBB << vs.front().x(), vs.front().y();
  for (const auto& v : vs)
  {
    minBB[0] = std::min(v.x(), minBB[0]);
    minBB[1] = std::min(v.y(), minBB[1]);
    maxBB[0] = std::max(v.x(), maxBB[0]);
    maxBB[1] = std::max(v.y(), maxBB[1]);
  }
}




void ModeRefPixPaint::finishTrim()
{

  if (!m_b_modified)
  {
    ShowMsgDlg_OK("編集された領域が存在しません．\n No edited voxel exist", "no trimmed.");
    return;
  }

  //このタイミングでfalseにしないとcanEndModeでダイアログが表示される
  m_b_modified = false;

  ImageCore::GetInst()->mask_storeCurrentForeGround();


  int maskID = ImageCore::GetInst()->GetSelectMaskIdx();
  //maskを入れ替えたい
  //①maskIDのまま残っているボクセルを新しく作るmaskに入れる。
  //initialzie vFlg
  ImageCore::GetInst()->InitializeFlg4dByMask(formMain_SetProgressValue);
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const std::vector<byte*>& flg4d = ImageCore::GetInst()->m_flg4d;
  std::vector<byte*>& mask4d = ImageCore::GetInst()->m_mask4d;

#pragma omp parallel for
  for (int f = 0; f < num_frames; ++f)
  {
    byte* flg3d = flg4d[f];
    byte* msk3d = mask4d[f];
    for (int i = 0; i < num_voxels; ++i)
    {
      if (msk3d[i] == maskID)flg3d[i] = 255;
    }
  }
  ImageCore::GetInst()->mask_storeCurrentForeGround();

  //②編集して塗った領域をmnaskIDへマージする。
  const auto& maskdata = ImageCore::GetInst()->GetMaskData();
  ImageCore::GetInst()->SelectedMsk_marge(static_cast<int>(maskdata.size())-2);
  formVisMask_updateList();

  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  formMain_RedrawMainPanel();
}



void ModeRefPixPaint::cancelBtnClick()
{
  if (ShowMsgDlgYesNo("現在の編集結果を破棄してよいですか\nDo you want to cancel current Editing?", "cancel?"))
  {
    m_b_modified = false;
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    formMain_RedrawMainPanel();
  }
}
