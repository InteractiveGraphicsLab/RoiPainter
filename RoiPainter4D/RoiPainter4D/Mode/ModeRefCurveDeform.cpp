#pragma unmanaged

#include "ModeRefCurveDeform.h"
#include "ModeCommonTools.h"

#include "OglForCLI.h"   // openGLを利用するためのクラス
#include "ImageCore.h"   // 画像データを保持しているクラス 　
#include "CrsSecCore.h"  // 切断面描画や、切断面への操作を管理
#include "LaplacianSurfaceEditing.h"
#include <fstream>
#include <queue>
#include "./Mode/GlslShader.h"

#pragma managed
#include "CliMessageBox.h"
#include "FormMain.h"     // Main Window
#include "FormVisParam.h" // 右上のダイアログ
#include "FormRefCurveDeform.h"
#pragma unmanaged

#include "tmarchingcubes.h"

using namespace RoiPainter4D;


ModeRefCurveDeform::ModeRefCurveDeform()
{
  std::cout << "ModeRefCurveDeform...\n";
  m_bL = m_bR = m_bM = false;
  std::cout << "ModeRefCurveDeform done\n";
}


ModeRefCurveDeform::~ModeRefCurveDeform(){}


bool ModeRefCurveDeform::CanEndMode()
{
  if (m_is_not_saved_state)
  {
    return ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Do you want to leave without saving curves?");
  }
  return true;
}


void ModeRefCurveDeform::StartMode()
{
  std::cout << "ModeRefCurveDeform...startMode----------\n";
  const auto cuboid    = ImageCore::GetInst()->GetCuboidF();
  const auto num_frame = ImageCore::GetInst()->GetNumFrames();

  m_bL = m_bR = m_bM = false;
  m_is_not_saved_state = false;
  
  m_cp_rate = cuboid[0] * 0.0006f;
  m_shared_curves.clear();
  m_curves = std::vector<std::vector<PlanarCurve>>(num_frame);
  m_laplacian_deformer = std::vector<LaplacianDeformer>();

  //for (int i = 0; i < ImageCore::GetInst()->GetNumFrames(); ++i) {
  //  m_laplacian_deformer.push_back(LaplacianDeformer(m_mask_mesh.GetMesh(i)));
  //}

  m_history = std::stack<SnapShot>();

  FormRefCurveDeform_InitAllItems();
  FormRefCurveDeform_Show();
  std::cout << "ModeRefCurveDeform...startMode DONE-----\n";
  std::cout << "Debug mode is available (Shift+Ctrl+F12).\n";
}



ModeRefCurveDeform::SelectionInfo ModeRefCurveDeform::PickCPatCurrentFrame(
    const EVec3f& ray_pos, 
    const EVec3f& ray_dir)
{
  const float CP_RAD = m_cp_rate * FormRefCurveDeform_GetCpSize();
  const int frame_idx = formVisParam_getframeI();
  SelectionInfo info;
  
  // pick standard curves
  for (int crv_i = 0; crv_i < (int)m_curves[frame_idx].size(); ++crv_i)
  {
    int cpidx = m_curves[frame_idx][crv_i].PickCPs(ray_pos, ray_dir, CP_RAD);
    if (cpidx == -1) continue;

    EVec3f pos           = m_curves[frame_idx][crv_i].GetCP(cpidx);
    CRSSEC_ID crssec_id  = m_curves[frame_idx][crv_i].GetCrssecId();
    float     crssec_pos = m_curves[frame_idx][crv_i].GetCrssecPos();
    if ( !info.selected || (ray_pos - pos).norm() < (ray_pos - info.pos).norm())
      info.Set(true, false, crv_i, cpidx, pos, crssec_id, crssec_pos);
  }

  // pick shared curves
  for (int crv_i = 0; crv_i < (int)m_shared_curves.size(); ++crv_i)
  {
    int cpidx = m_shared_curves[crv_i].PickCPs(frame_idx, ray_pos, ray_dir, CP_RAD);
    if (cpidx == -1) continue;

    EVec3f pos          = m_shared_curves[crv_i].GetCP(frame_idx, cpidx);
    CRSSEC_ID crssec_id = m_shared_curves[crv_i].GetCrssecId();
    float     crssec_pos= m_shared_curves[crv_i].GetCrssecPos();
    if (!info.selected || (ray_pos - pos).norm() < (ray_pos - info.pos).norm())
      info.Set(true, true, crv_i, cpidx, pos, crssec_id, crssec_pos);
  }
  return info;
}



void ModeRefCurveDeform::LBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = true;
  const int frame_idx = formVisParam_getframeI();
  const EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();

  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);


  if (IsShiftKeyOn())
  {
    Do_RecordSnapShot();
    auto info = PickCPatCurrentFrame(ray_pos, ray_dir);

    if (info.selected) 
    {
      //picking成功 - このcurve/CPを選択状態に(位置に平面を移動)
      m_select_info = info;
      CrssecCore::GetInst()->SetPlanePos(info.crssec_id, info.crssec_pos, cuboid);
    }
    else
    {
      //picking失敗 - 新curve追加 / 新CP追加 / 選択解除
      CRSSEC_ID crssec_id = PickCrssec(ray_pos, ray_dir, pos);
      if (crssec_id != CRSSEC_NON)
      {
        if( !m_select_info.selected)
        {
          float crssec_pos = crssec_id == CRSSEC_XY ? pos[2] :
                             crssec_id == CRSSEC_YZ ? pos[0] : pos[1];
          m_curves[frame_idx].push_back(PlanarCurve(crssec_id, crssec_pos, pos));
          int curve_idx = (int)m_curves[frame_idx].size() - 1;
          m_select_info.Set(true, false, curve_idx, 0, pos, crssec_id, crssec_pos);
        }
        else if (m_select_info.selected && !m_select_info.is_shared)
        {
          int cpidx;
          if( m_curves[frame_idx][m_select_info.curve_idx].AddCP(pos, cpidx) ) 
            m_select_info.cp_idx = cpidx;
        }
      }
      else
      {
        //clear selection
        m_select_info.Clear();
      }
    }
  }
  else
  {
    m_select_info.Clear();
    ogl->BtnDown_Trans(p);
  }

  formMain_RedrawMainPanel();
}



void ModeRefCurveDeform::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = false;
  const int frame_idx = formVisParam_getframeI();

  if (m_select_info.selected && m_select_info.is_shared )
  {
    int ci = m_select_info.curve_idx;
    if (0 <= ci && ci <(int)m_shared_curves.size())
      m_shared_curves[ci].UpdateNonManipCurves();
  }

  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeRefCurveDeform::RBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = true;
  const int frame_idx = formVisParam_getframeI();
  

  if (IsShiftKeyOn())
  {
    EVec3f ray_pos, ray_dir, pos;
    ogl->GetCursorRay(p, ray_pos, ray_dir);
    auto info = PickCPatCurrentFrame(ray_pos, ray_dir);

    if ( info.selected && info.is_shared)
    {
      if (ShowMsgDlgYesNo("All-frame曲線を解除しますか？\n（自動補間された曲線は削除されます）", "Unlock?"))
      {
        Do_RecordSnapShot();
        MakeSelectedStroke_Unshared();
        m_select_info.Clear();
      }
    }
    else if (info.selected && !info.is_shared)
    {
      //delete cp
      Do_RecordSnapShot();

      std::vector<PlanarCurve> &curves = m_curves[frame_idx];
      curves[info.curve_idx].DeleteCP(info.cp_idx);
      if (curves[info.curve_idx].GetNumCPs() == 0)
      {
        curves.erase(m_curves[frame_idx].begin() + info.curve_idx);
        m_select_info.Clear();
      }
      else
      { 
        m_select_info = info;
        m_select_info.cp_idx = curves[info.curve_idx].GetNumCPs() - 1;
      }
    }
  }
  else
  {
    m_select_info.Clear();
    ogl->BtnDown_Rot(p);
  }

  formMain_RedrawMainPanel();
}


void ModeRefCurveDeform::RBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeRefCurveDeform::MBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = true;
  m_select_info.Clear();
  ogl->BtnDown_Zoom(p);
  formMain_RedrawMainPanel();
}


void ModeRefCurveDeform::MBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeRefCurveDeform::MouseMove(const EVec2i& p, OglForCLI* ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  const int frame_idx = formVisParam_getframeI();
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if (IsShiftKeyOn() && m_bL)
  {
    if (PickCrssec(ray_pos, ray_dir, pos) != CRSSEC_NON)
    {
      int curve_idx = m_select_info.curve_idx;
      int cp_idx    = m_select_info.cp_idx;
      if (m_select_info.selected && m_select_info.is_shared)
      {
        m_shared_curves[curve_idx].MoveCP(frame_idx, cp_idx, pos);
      }
      else if(m_select_info.selected && !m_select_info.is_shared)
      {
        m_curves[frame_idx][curve_idx].MoveCP(cp_idx, pos); 
      }
    }
  }
  else
  {
    ogl->MouseMove(p);
  }

  formMain_RedrawMainPanel();
}



void ModeRefCurveDeform::MouseWheel(
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


void ModeRefCurveDeform::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}

void ModeRefCurveDeform::LBtnDclk(const EVec2i& p, OglForCLI* ogl){}

void ModeRefCurveDeform::RBtnDclk(const EVec2i& p, OglForCLI* ogl)
{
  const int frame_idx = formVisParam_getframeI();

  if (m_select_info.selected)
  {
    //選択されたcurveを削除
    Do_RecordSnapShot();

    if(m_select_info.is_shared)
    {
      if (ShowMsgDlgYesNo("All-frame曲線を解除しますか？\n（自動補間された曲線は削除されます）", "Unlock?"))
      {
        MakeSelectedStroke_Unshared();
      }
    }
    else if( 0 <= m_select_info.curve_idx && m_select_info.curve_idx < m_curves[frame_idx].size())
    {
      m_curves[frame_idx].erase( m_curves[frame_idx].begin() + m_select_info.curve_idx );
    }
  }
}




void ModeRefCurveDeform::KeyDown(int nChar)
{
  static bool debug = false;

  if (nChar == VK_F1)
  {
    // show stroke matching
    const int frame_idx = formVisParam_getframeI();
    std::vector<int> fixed_verts_idxs;
    m_matched_pos = std::vector<Eigen::Vector3f>();
    std::vector<EVec3f> target_pos = std::vector<Eigen::Vector3f>();
    FindClosestPointFromStroke(frame_idx, fixed_verts_idxs, target_pos, m_matched_pos);
  }
  else if (nChar == VK_F2)
  {
    // deform all frame
    DeformAllFrame();
  }
  else if (nChar == VK_F3)
  {
    const int num_frames = ImageCore::GetInst()->GetNumFrames();
    for (int i = 0; i < num_frames; ++i)
    {
      m_meshes_def[i] = m_meshes_orig[i];
    }
    
  }
  else if (nChar == VK_F4)
  {
    MakeSelectedStroke_Shared();
  }
  else if (nChar == VK_F5)
  {
    MakeSelectedStroke_Unshared();
  }
  else if (nChar == VK_F6)
  {
    // lock selected stroke
    // LockSelectedStroke();
  }
  else if (nChar == VK_F7)
  {
    // unlock selected stroke
    // UnlockSelectedStroke();
  }
  else if (nChar == VK_F8)
  {

  }
  else if (nChar == VK_F9)
  {

  }
  else if (nChar == VK_F10)
  {

  }
  else if (nChar == VK_F11)
  {

  }
  else if (nChar == VK_F12)
  {
    if (IsShiftKeyOn()&& IsCtrKeyOn())
    {
      debug = !debug;
      std::cout << "debug mode: " << (debug ? "ON" : "OFF") << "\n";
      if (debug)
      {
        std::cout << "[F1]  Show matching between strokes and mesh" << "\n";
        std::cout << "[F2]  Deform mesh all frame" << "\n";
        std::cout << "[F3]  Reload mesh all frame" << "\n";
        std::cout << "[F4]  Share selected stroke" << "\n";
        std::cout << "[F5]  Unshare selected stroke" << "\n";
        std::cout << "[F6]  Lock selected stroke" << "\n";
        std::cout << "[F7]  Unlock selected stroke" << "\n";
        std::cout << "[F12] Switch debug mode (Shift+Ctrl+F12)" << "\n";
      }
    }
  }
}

void ModeRefCurveDeform::KeyUp(int nChar) {}


static const std::string vtxshader_fname = std::string("shader/cagemeshVtx.glsl");
static const std::string frgshader_fname = std::string("shader/cagemeshFrg.glsl");
static GLuint gl2Program = -1;


void ModeRefCurveDeform::DrawScene(
    const EVec3f& cam_pos,
    const EVec3f& cam_cnt)
{
  if (gl2Program == -1)
  {
    t_InitializeShader(vtxshader_fname.c_str(),  
                       frgshader_fname.c_str(), gl2Program);
  }

  const float CP_RAD  = m_cp_rate * (float) FormRefCurveDeform_GetCpSize();
  const int frame_idx = formVisParam_getframeI();
  //ImageCore::GetInst()->UpdateImgMaskColor();

  ImageCore::GetInst()->BindAllVolumes();
  DrawCrossSectionsNormal();

  if (IsMKeyOn() && formVisParam_bRendVol())
  {
    DrawVolumeVisMask(!IsShiftKeyOn(), cam_pos, cam_cnt);
  }

  glDepthMask(true);
  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);

  // draw mesh
  if (!IsSpaceKeyOn() && 0 <= frame_idx && frame_idx < (int)m_meshes_def.size())
  {
    glUseProgram(gl2Program);
    m_meshes_def[frame_idx].Draw();
    glUseProgram(0);
  }

  // draw stroke
  if (!IsSKeyOn())
  {
    if (     m_select_info.selected && !m_select_info.is_shared)
    {
      float thickness = 3.0f;
      m_curves[frame_idx][m_select_info.curve_idx].Draw(COLOR_Y, thickness);
      m_curves[frame_idx][m_select_info.curve_idx].DrawCPs(COLOR_Y, CP_RAD, m_select_info.cp_idx);
    }
    else if (m_select_info.selected && m_select_info.is_shared)
    {
      m_shared_curves[m_select_info.curve_idx].Draw(frame_idx, true);
    }

    for (int i = 0; i < m_matched_pos.size(); ++i)
    {
      glEnable(GL_LIGHTING);
      TMesh::DrawSphere(m_matched_pos[i], 0.3f, COLOR_G, COLOR_G, COLOR_W, COLOR_SHIN64);
      glDisable(GL_LIGHTING);
    }
  }

  //curveの色
  // 作業中 --> 赤
  // sharedで操作あり --> 緑
  // sharedで操作なし --> 水色
  // sharedではなくて、操作中じゃない --> 黄色 ※ここでは対象外
  //
  //CPの色 
  // 作業中 & 選択中　--> 赤
  // Shared & 操作あり --> 緑
  // Shared & 操作なし --> 水色
  // それ以外 --> 黄色 
  //
  
  // draw all control points
  if (IsShiftKeyOn())
  {
    //normal
    for (int i = 0; i < m_curves[frame_idx].size(); ++i)
    {
      if (m_select_info.selected && !m_select_info.is_shared && m_select_info.curve_idx == i)
        m_curves[frame_idx][i].DrawCPs(COLOR_Y, CP_RAD, m_select_info.cp_idx);
      else
        m_curves[frame_idx][i].DrawCPs(COLOR_Y, CP_RAD, -1);
    }
    //shared
    for (int i = 0; i < m_shared_curves.size(); ++i)
    {
      if (m_select_info.selected && m_select_info.is_shared && m_select_info.curve_idx == i)
        m_shared_curves[i].DrawCPs(frame_idx, true, CP_RAD, m_select_info.cp_idx);
      else
        m_shared_curves[i].DrawCPs(frame_idx, false, CP_RAD, -1);
    }
  }
}


void ModeRefCurveDeform::Deform()
{
  Deform(formVisParam_getframeI());
}

void ModeRefCurveDeform::Deform(const int _frame_idx)
{
  _Deform(_frame_idx);
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::DeformAllFrame()
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
#pragma omp parallel for
  for (int i = 0; i < num_frames; ++i)
  {
    _Deform(i);
  }
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


// 現在の状態をスナップショットとして history.undo に保存し，history.redo をクリアする
void ModeRefCurveDeform::Do_RecordSnapShot()
{
  const int frame_idx = formVisParam_getframeI();
  m_history.push({ frame_idx, m_curves, m_shared_curves });
  m_is_not_saved_state = true;
}


// Undo：1つ前の状態に戻し，現在の状態を Redo 用に保存する
void ModeRefCurveDeform::Undo_LoadSnapShot()
{
  const int frame_idx = formVisParam_getframeI();
  if (m_history.empty())
  {
    std::cout << "No more undo.\n";
    return;
  }

  if (m_history.top().frame_idx != frame_idx)
  {
    if( !ShowMsgDlgYesNo("undo for different frame", "異なるframeのUndoをしてよいですか？"))
      return;
  }
  
  const auto snap = m_history.top();
  m_history.pop();
  m_curves = snap.curves;
  m_shared_curves = snap.shared_curves;

  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}



void ModeRefCurveDeform::ConvertMaskToMesh()
{
  const int stride = FormRefCurveDeform_GetMcStride();
  const int scale_num = (int)pow(stride, 3);
  const int active_maskid = ImageCore::GetInst()->GetSelectMaskIdx();
  const std::vector<MaskData>& mask_data = ImageCore::GetInst()->GetMaskData();

  if (active_maskid < 0 || mask_data.size() <= active_maskid) return;

  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const EVec3i reso    = ImageCore::GetInst()->GetReso();
  const EVec3f pitch   = ImageCore::GetInst()->GetPitch();

  const int w = reso[0] / stride;
  const int h = reso[1] / stride;
  const int d = reso[2] / stride;
  std::cout << "m_num_voxels: " << num_voxels << " m_reso: " << reso << "\n";

  m_meshes_orig = std::vector<TMesh>(num_frames);
  m_meshes_def  = std::vector<TMesh>(num_frames);

#pragma omp parallel for
  for (int frame_idx = 0; frame_idx < num_frames; ++frame_idx)
  {
    const byte* mask = ImageCore::GetInst()->m_mask4d[frame_idx];
    std::unique_ptr<short[]> v = std::make_unique<short[]>(num_voxels / scale_num);
    for (int z = 0; z < d; ++z) {
      for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
          float ave = 0;
          for (int c = 0; c < stride; ++c) {
            for (int b = 0; b < stride; ++b) {
              for (int a = 0; a < stride; ++a) {
                const int idx = ((z * stride + c) * reso[1] * reso[0]) + ((y * stride + b) * reso[0]) + (x * stride + a);
                ave += (mask[idx] == active_maskid) ? 1.0f : 0.0f;
              }
            }
          }
          ave /= powf(static_cast<float>(stride), 3.0f);
          v[z * h * w + y * w + x] = ave >= 0.49f ? 255 : 0;
        }
      }
    }

    TMesh mesh;
    marchingcubes::MarchingCubes(reso / stride, pitch, v.get(), 128, 0, 0, mesh);
    mesh.Scale(static_cast<float>(stride));
    mesh.Smoothing(3);
    m_meshes_orig[frame_idx] = mesh;
    m_meshes_def[frame_idx] = mesh;
    v.reset();
  }

  m_laplacian_deformer = std::vector<LaplacianDeformer>();
  for (int i = 0; i < num_frames; ++i) 
  {
    m_laplacian_deformer.push_back(LaplacianDeformer(m_meshes_orig[i]));
  }

  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}

//TODO 後で変える（FinishModeに移動）
void ModeRefCurveDeform::ConvertMeshToMask()
{
  const int num_frames    = ImageCore::GetInst()->GetNumFrames();
  const int active_maskid = ImageCore::GetInst()->GetSelectMaskIdx();
  const auto&  mask_data  = ImageCore::GetInst()->GetMaskData();
  const int num_voxels    = ImageCore::GetInst()->GetNumVoxels();
  const EVec3i reso       = ImageCore::GetInst()->GetReso();
  const EVec3f pitch      = ImageCore::GetInst()->GetPitch();

  byte mask_locked[256] = {};
  for (int i = 0; i < (int)mask_data.size(); ++i)
  {
    mask_locked[i] = mask_data[i].lock ? 1 : 0;
  }
  mask_locked[active_maskid] = 0;

#pragma omp parallel for
  for (int frame_idx = 0; frame_idx < num_frames; ++frame_idx)
  {
    byte* mask = ImageCore::GetInst()->m_mask4d[frame_idx];
    std::unique_ptr<byte[]> img = std::make_unique<byte[]>(num_voxels);

    m_meshes_def[frame_idx].GenBinaryVolume(reso, pitch, img.get());

    for (int i = 0; i < num_voxels; ++i)
    {
      if (mask_locked[mask[i]]) continue;
      if ( img[i] ) mask[i] = active_maskid;
      if ( !img[i] && mask[i] == active_maskid) mask[i] = 0;
    }
    img.reset();
  }

  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
  m_is_not_saved_state = false;
}


void ModeRefCurveDeform::ReloadOrigMeshCurrentFrame()
{
  if (m_meshes_orig.size() == 0 || m_meshes_orig.front().m_vSize == 0) return;

  const int frame_idx = formVisParam_getframeI();
  m_meshes_def[frame_idx] = m_meshes_def[frame_idx];

  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}

 

////////////////////////////////////////////////////////////////
//Curve Modification////////////////////////////////////////////
void ModeRefCurveDeform::CopyFromPrevFrame()
{
  const int frame_idx = formVisParam_getframeI();
  if (frame_idx <= 0) return;
  std::cout << "Copy from previous frame.\n";
  m_curves[frame_idx] = m_curves[frame_idx - 1];
  Do_RecordSnapShot();
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::CopyStrokesToAllFrame()
{
  const int frame_idx = formVisParam_getframeI();
  const int mum_frames = ImageCore::GetInst()->GetNumFrames();
  for (int i = 0; i < mum_frames; ++i)
  {
    if (i == frame_idx) continue;
    m_curves[i] = m_curves[frame_idx];
  }
  std::cout << "Copy strokes to all frame.\n";
  Do_RecordSnapShot();
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}



void ModeRefCurveDeform::_Deform(const int _frame_idx)
{
  if (m_meshes_orig.size() == 0 || m_meshes_orig.front().m_vSize == 0) return;

  m_meshes_def[_frame_idx] = m_meshes_orig[_frame_idx];

  std::vector<int   > fixed_verts_idxs;
  std::vector<EVec3f> fixed_positions;
  std::vector<EVec3f> src_positions;

  FindClosestPointFromStroke(_frame_idx, fixed_verts_idxs, fixed_positions, src_positions);

  // Sort by index
  std::vector<std::pair<int, EVec3f>> indexPointPairs;

  for (size_t i = 0; i < fixed_verts_idxs.size(); ++i) 
  {
    indexPointPairs.emplace_back(fixed_verts_idxs[i], fixed_positions[i]);
  }

  std::sort(indexPointPairs.begin(), indexPointPairs.end(),
    [](const std::pair<int, EVec3f>& a, const std::pair<int, EVec3f>& b) {
      return a.first < b.first;
    });

  for (size_t i = 0; i < indexPointPairs.size(); ++i) 
  {
    fixed_verts_idxs[i] = indexPointPairs[i].first;
    fixed_positions[i] = indexPointPairs[i].second;
  }

  if (fixed_verts_idxs.size() == 0) return;

  // Deform
  m_laplacian_deformer[_frame_idx].Deform(fixed_verts_idxs, fixed_positions);
}



void ModeRefCurveDeform::FindClosestPointFromStroke(
    const int _frame_idx, 
    std::vector<int   >& _idxs, 
    std::vector<EVec3f>& _target_pos, 
    std::vector<EVec3f>& _src_pos)
{
  if (m_meshes_orig.size() == 0 || m_meshes_orig.front().m_vSize == 0) return;

  TMesh& mesh = m_meshes_def[_frame_idx];
  
  const size_t num_curves = m_curves[_frame_idx].size();
  const size_t num_shared_curves = m_shared_curves.size();
  // for curve
  for (size_t i = 0; i < num_curves + num_shared_curves; ++i)
  {
    const PlanarCurve& c = (i < num_curves) ? m_curves[_frame_idx][i] : 
                                              m_shared_curves[i - num_curves].GetCurve(_frame_idx);
  
    const std::vector<EVec3f> &points = c.GetCurve();
    const bool   normal_side   = c.GetNormalSide();
    const EVec3f crssec_normal = c.GetCrssecNorm();

    for (size_t idx = 1; idx < points.size() - 1; ++idx)
    {
      EVec3f pos = points[idx];
      EVec3f curve_tangent = (points[idx + 1] - points[idx - 1]);
      if (curve_tangent.norm() < 0.0001f) continue;
      curve_tangent.normalize();
      EVec3f curve_normal  = crssec_normal.cross(curve_tangent).normalized();
      curve_normal *= (normal_side ? 1.0f : -1.0f);

      // get nearest vertex idx
      float dist_min = FLT_MAX;
      int idx_min = -1;
      for (int k = 0; k < mesh.m_vSize; ++k)
      {
        if (mesh.m_vNorms[k].dot(curve_normal) <= 0.0f) continue;
        const EVec3f diff = pos - mesh.m_vVerts[k];
        Eigen::Matrix3f mat =
            powf(5.0f, 2.0f) * crssec_normal * crssec_normal.transpose()
          + powf(1.0f, 2.0f) * curve_tangent * curve_tangent.transpose()
          + powf(0.2f, 2.0f) * curve_normal  * curve_normal.transpose();
        const float dist = diff.transpose() * mat * diff;

        if (dist < dist_min) {
          dist_min = dist;
          idx_min = k;
        }
      }
      if (idx_min < 0) continue;

      const bool found = std::find(_idxs.begin(), _idxs.end(), idx_min) != _idxs.end();
      if (!found)
      {
        _idxs      .push_back(idx_min);
        _target_pos.push_back(pos);
        _src_pos   .push_back(mesh.m_vVerts[idx_min]);
      }
    }
  }
}




void ModeRefCurveDeform::FlipSelectedStrokeNormalSide()
{
  const int frame_idx = formVisParam_getframeI();
  const int cidx = m_select_info.curve_idx;

  if (m_select_info.selected && m_select_info.is_shared && cidx != -1)
  {
    m_shared_curves[cidx].FlipNormal();
  }
  if (m_select_info.selected && !m_select_info.is_shared && cidx != -1)
  {
    m_curves[frame_idx][cidx].FlipNormal();
  }
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}



void ModeRefCurveDeform::MakeSelectedStroke_Shared()
{
  const int frame_idx = formVisParam_getframeI();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  bool tf = !m_select_info.selected      || 
             m_select_info.is_shared     || 
             m_select_info.curve_idx < 0 || 
             m_curves[frame_idx].size() <= m_select_info.curve_idx;
  if (tf) return;

  std::vector<PlanarCurve>& curves = m_curves[frame_idx];
  PlanarCurve src = curves[m_select_info.curve_idx];
  curves.erase(curves.begin() + m_select_info.curve_idx);
  m_shared_curves.push_back(SharedCurves(num_frames, frame_idx, src));

  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}



void ModeRefCurveDeform::MakeSelectedStroke_Unshared()
{
  const int frame_idx = formVisParam_getframeI();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  bool tf = !m_select_info.selected  || 
            !m_select_info.is_shared ||
             m_select_info.curve_idx < 0 ||
             m_shared_curves.size() <= m_select_info.curve_idx;
  
  for( int fi = 0; fi < num_frames; ++fi )
  {
    if (m_shared_curves[m_select_info.curve_idx].IsManipulated(fi) )
    {
      PlanarCurve c = m_shared_curves[m_select_info.curve_idx].GetCurve(fi);
      m_curves[fi].push_back(c);
    }
  }

  m_shared_curves.erase(m_shared_curves.begin() + m_select_info.curve_idx);
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}



void ModeRefCurveDeform::SaveState(
    const std::string& _fpath)
{
  std::ofstream file(_fpath);
  if (!file)
  {
    std::cout << "Failed to save state.\n";
    return;
  }

  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  file << "RefCurveDeform_State\n";
  file << "NumFrames: " << std::to_string(num_frames) + "\n";

  //normal curves 
  for (int fi = 0; fi < num_frames; ++fi)
  {
    file << "Frame: " << std::to_string(fi) + "\n";
    file << "num_curves: " << std::to_string(m_curves.size()) << "\n";
    for( int j = 0; j < m_curves[fi].size(); ++j)
      m_curves[fi][j].WriteToFile(file);
  }

  //Shared curve
  file << "SharedCurves: " << std::to_string(m_shared_curves.size()) << "\n";
  for (size_t i = 0; i < m_shared_curves.size(); ++i)
  {
    m_shared_curves[i].WriteToFile(file);
  }

  file.close();

  std::cout << "Saved as \"" + _fpath + "\n";
  m_is_not_saved_state = false;
  formMain_ActivateMainForm();
}

void ModeRefCurveDeform::LoadState(const std::string& _fpath)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  std::ifstream file(_fpath);
  if (!file) return;

  std::string line, key;
  std::stringstream ss;

  // RefCurveDeform_State
  if (!ReadNextLine(file, line, ss) ) return;
  std::cout << line << "\n"; 

  // NumFrames: n
  if (!ReadNextLine(file, line, ss) ) return;
  int _num_frames = 0;
  ss >> key >> _num_frames;
  std::cout << key << _num_frames << "\n";
  
  if(_num_frames != num_frames) return;

  //normal curves 
  for (int fi = 0; fi < num_frames; ++fi)
  {
    int fidx, num_curves;

    //Frame: fi
    if (!ReadNextLine(file, line, ss)) return;
    ss >> key >> fidx;
    std::cout << key << fidx << "\n";

    //num_curves: n
    if (!ReadNextLine(file, line, ss)) return;
    ss >> key >> num_curves;
    m_curves[fi].clear();
    std::cout << key << num_curves << "\n";
    for (int j = 0; j < num_curves; ++j)
    {
      PlanarCurve c(file);
      m_curves[fi].push_back(c);
    }
  }

  //SharedCurves: n
  if (!ReadNextLine(file, line, ss)) return;
  int num_shared_curves = 0;
  ss >> key >> num_shared_curves;
  std::cout << key << num_shared_curves << "\n";
  m_shared_curves.clear();

  for (int i = 0; i < num_shared_curves; ++i)
  {
    SharedCurves sc(file);
    m_shared_curves.push_back(sc);
    sc.UpdateNonManipCurves();
  }
}

/*

void ModeRefCurveDeform::LockSelectedStroke()
{
  const int frame_idx = formVisParam_getframeI();
  DeformationStrokes& dstrokes = m_strokes[frame_idx];
  dstrokes.Lock_SelStroke();
  UpdateSharedStroke();
  formMain_RedrawMainPanel();
}


void ModeRefCurveDeform::UnlockSelectedStroke()
{
  const int frame_idx = formVisParam_getframeI();
  DeformationStrokes& dstrokes = m_strokes[frame_idx];
  dstrokes.Unlock_SelStroke();
  UpdateSharedStroke();
  formMain_RedrawMainPanel();
}
*/



//inline std::vector<int> dijkstra(const Eigen::Vector3f* vertices, const std::vector<int>* adjacencyList, int numVertices, int start, int end) {
//  std::vector<float> distances(numVertices, std::numeric_limits<float>::infinity());
//  std::vector<int> previous(numVertices, -1);
//  std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> pq;
//
//  distances[start] = 0.0f;
//  pq.push({ 0.0f, start });
//
//  while (!pq.empty()) {
//    int current = pq.top().second;
//    pq.pop();
//
//    if (current == end) break;
//
//    for (int neighbor : adjacencyList[current]) {
//      float edgeWeight = (vertices[current] - vertices[neighbor]).norm();
//      float newDist = distances[current] + edgeWeight;
//
//      if (newDist < distances[neighbor]) {
//        distances[neighbor] = newDist;
//        previous[neighbor] = current;
//        pq.push({ newDist, neighbor });
//      }
//    }
//  }
//
//  std::vector<int> path;
//  for (int at = end; at != -1; at = previous[at]) {
//    path.push_back(at);
//  }
//  std::reverse(path.begin(), path.end());
//
//  if (path.front() != start) {
//    path.clear();
//  }
//
//  return path;
//}
