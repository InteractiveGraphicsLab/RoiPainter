#pragma unmanaged

#include "ModeRefCurveDeform.h"
#include "ModeCommonTools.h"

#include "OglForCLI.h"   
#include "ImageCore.h"   
#include "CrsSecCore.h"  
#include "LaplacianSurfaceEditing.h"
#include <fstream>
#include <queue>
#include "./Mode/GlslShader.h"
#include "./ModeCore.h"

#pragma managed
#include "CliMessageBox.h"
#include "FormMain.h"     // Main Window
#include "FormVisParam.h" // 右上のダイアログ
#include "FormRefCurveDeform.h"
#include "FormSelectMskId.h"
#pragma unmanaged

#include "tmarchingcubes.h"

using namespace RoiPainter4D;

ModeRefCurveDeform::~ModeRefCurveDeform() {}

ModeRefCurveDeform::ModeRefCurveDeform()
{
  std::cout << "ModeRefCurveDeform...\n";
  m_bL = m_bR = m_bM = false;
  std::cout << "ModeRefCurveDeform done\n";
}


bool ModeRefCurveDeform::CanEndMode()
{
  return ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Do you want to leave without saving curves?");
}


void ModeRefCurveDeform::StartMode()
{
  std::cout << "ModeRefCurveDeform...startMode----------\n";

  const std::vector<MaskData>& mask_data = ImageCore::GetInst()->GetMaskData();
  m_target_mask_id = formSelectMasdskID_showModalDialog();

  if (m_target_mask_id < 0 || mask_data.size() <= m_target_mask_id )
  {
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }

  if (m_target_mask_id == 0)
  {
    ShowMsgDlg_OK("0th region (background) cannot be trimmed", "caution");
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }

  const auto cuboid    = ImageCore::GetInst()->GetCuboidF();
  const auto num_frame = ImageCore::GetInst()->GetNumFrames();

  m_bL = m_bR = m_bM = false;
  
  m_cp_rate = cuboid[0] * 0.0006f;
  m_shared_curves.clear();
  m_curves = std::vector<std::vector<PlanarCurve>>(num_frame);

  m_history = std::stack<SnapShot>();

  m_debug_matched_vids.clear();
  m_debug_matched_trgtpos.clear();
  m_debug_matched_vids.resize(num_frame);
  m_debug_matched_trgtpos.resize(num_frame);

  FormRefCurveDeform_InitAllItems();
  FormRefCurveDeform_Show();
  std::cout << "ModeRefCurveDeform...startMode DONE-----\n";
  std::cout << "Debug mode is available (Shift+Ctrl+F12).\n";
}


PlanarCurveSelectionInfo ModeRefCurveDeform::PickCpAtCurrentFrame(
    const EVec3f& ray_pos, 
    const EVec3f& ray_dir)
{
  const int frame_idx = formVisParam_getframeI();
  const float CP_RAD = m_cp_rate * FormRefCurveDeform_GetCpSize();
  const bool  ONLY_SELECTED = FormRefCurveDeform_VisOnlySelCurve();

  PlanarCurveSelectionInfo info;
  int cp_idx;
  EVec3f cp_pos;

  // pick standard curves
  for (int i = 0; i < (int)m_curves[frame_idx].size(); ++i)
  {
    const PlanarCurve& c = m_curves[frame_idx][i];

    if (ONLY_SELECTED && !m_select_info.IsStdCurveSelect(i) ) continue;
    if (!c.PickCPs(ray_pos, ray_dir, CP_RAD, cp_idx, cp_pos)) continue;

    if ( !info.selected || Dist(ray_pos, cp_pos) < Dist(ray_pos, info.pos) )
      info.Set(true, false, i, cp_idx, cp_pos, c.GetCrssecId(), c.GetCrssecPos());
  }

  // pick shared curves
  for (int i = 0; i < (int)m_shared_curves.size(); ++i)
  {
    SharedCurves& sc = m_shared_curves[i];
    if (ONLY_SELECTED && !m_select_info.IsSharedCurveSelect(i)) continue;
    if (!sc.PickCPs(frame_idx, ray_pos, ray_dir, CP_RAD, cp_idx, cp_pos)) continue;

    if (!info.selected || Dist(ray_pos, cp_pos) < Dist(ray_pos, info.pos))
      info.Set(true, true, i, cp_idx, cp_pos, sc.GetCrssecId(), sc.GetCrssecPos());
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
    auto info = PickCpAtCurrentFrame(ray_pos, ray_dir);

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
    auto info = PickCpAtCurrentFrame(ray_pos, ray_dir);

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


void ModeRefCurveDeform::MBtnDclk(const EVec2i& p, OglForCLI* ogl){}
void ModeRefCurveDeform::LBtnDclk(const EVec2i& p, OglForCLI* ogl){}
void ModeRefCurveDeform::RBtnDclk(const EVec2i& p, OglForCLI* ogl){}


static const std::string vtxshader_fname = std::string("shader/cagemeshVtx.glsl");
static const std::string frgshader_fname = std::string("shader/cagemeshFrg.glsl");
static GLuint gl2Program = 0;


void ModeRefCurveDeform::DrawScene(
    const EVec3f& cam_pos,
    const EVec3f& cam_cnt)
{
  if (gl2Program == 0)
  {
    t_InitializeShader(vtxshader_fname.c_str(),  
                       frgshader_fname.c_str(), gl2Program);
  }

  const float CP_RAD  = m_cp_rate * (float) FormRefCurveDeform_GetCpSize();
  const int frame_idx = formVisParam_getframeI();
  //ImageCore::GetInst()->UpdateImgMaskColor();

  ImageCore::GetInst()->BindAllVolumes();
  DrawCrossSectionsNormal();

  if (IsMKeyOn())
  {
    DrawVolumeVisMask(!IsShiftKeyOn(), cam_pos, cam_cnt);
  }

  glDepthMask(true);
  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);

  // draw mesh
  if (!IsSpaceKeyOn() && 0 <= frame_idx && frame_idx < (int)m_meshes_def.size())
  {
    glUseProgram(gl2Program);
    m_meshes_def[frame_idx].Draw();
    glUseProgram(0);
  }

  //DrawCurves
  const bool VIS_CP = IsShiftKeyOn();
  const bool VIS_ONLY_SEL = FormRefCurveDeform_VisOnlySelCurve();

  DrawPlanerCurvesAndSharedCurves(
    frame_idx, m_select_info, 
    VIS_ONLY_SEL, VIS_CP, CP_RAD, 
    m_curves[frame_idx], m_shared_curves);


  if (IsSpaceKeyOn())
  {
    const TMesh &m = m_meshes_orig[frame_idx];
    const std::vector<int>    &vids = m_debug_matched_vids[frame_idx];
    const std::vector<EVec3f> &trgtpos = m_debug_matched_trgtpos[frame_idx];
    //debug
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 1.0f);  
    for (size_t i = 0; i < vids.size(); ++i)
    {
      glVertex3fv(m.m_vVerts[vids[i]].data());
      glVertex3fv(trgtpos[i].data());
    }
    glEnd();
  }
}




// 現在の状態をスナップショットとして history.undo に保存し，history.redo をクリアする
void ModeRefCurveDeform::Do_RecordSnapShot()
{
  const int frame_idx = formVisParam_getframeI();
  m_history.push({ frame_idx, m_curves, m_shared_curves });
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
  const std::vector<MaskData>& mask_data = ImageCore::GetInst()->GetMaskData();
  if (m_target_mask_id <= 0 || mask_data.size() <= m_target_mask_id) return;

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
                ave += (mask[idx] == m_target_mask_id) ? 1.0f : 0.0f;
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

  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::CancelSegmentation()
{
  if (ShowMsgDlgYesNo("Cancel segmentation? 分割結果を破棄して良いですか?", "Cancel Segmentation?")) 
  {
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
  }
}


void ModeRefCurveDeform::FinishSegmentation()
{
  const int    num_frames = ImageCore::GetInst()->GetNumFrames();
  const auto&  mask_data  = ImageCore::GetInst()->GetMaskData();
  const int    num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const EVec3i reso       = ImageCore::GetInst()->GetReso();
  const EVec3f pitch      = ImageCore::GetInst()->GetPitch();

  if (m_target_mask_id <= 0 || mask_data.size() <= m_target_mask_id) return;

  byte mask_locked[256] = {};
  for (int i = 0; i < (int)mask_data.size(); ++i)
  {
    mask_locked[i] = mask_data[i].lock ? 1 : 0;
  }
  mask_locked[m_target_mask_id] = 0;

#pragma omp parallel for
  for (int fi = 0; fi < num_frames; ++fi)
  {
    if (m_curves[fi].size() + m_shared_curves.size() == 0) continue; //変化無し

    byte* mask = ImageCore::GetInst()->m_mask4d[fi];

    std::unique_ptr<byte[]> img = std::make_unique<byte[]>(num_voxels);
    m_meshes_def[fi].GenBinaryVolume(reso, pitch, img.get());

    for (int i = 0; i < num_voxels; ++i)
    {
      if (mask_locked[mask[i]]) continue;
      if ( img[i] ) mask[i] = m_target_mask_id;
      if (!img[i] && mask[i] == m_target_mask_id) mask[i] = 0;
    }
    img.reset();
  }

  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
}


void ModeRefCurveDeform::ReloadOrigMeshCurrentFrame()
{
  if (m_meshes_orig.size() == 0 || m_meshes_orig.front().m_vSize == 0) return;

  const int frame_idx = formVisParam_getframeI();
  m_meshes_def[frame_idx] = m_meshes_orig[frame_idx];

  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}

 

////////////////////////////////////////////////////////////////
//Curve Modification////////////////////////////////////////////
void ModeRefCurveDeform::CopyFromPrevFrame()
{
  const int frame_idx = formVisParam_getframeI();
  if (frame_idx <= 0) return;
  Do_RecordSnapShot();
  std::cout << "Copy from previous frame.\n";
  m_curves[frame_idx] = m_curves[frame_idx - 1];
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::CopyStrokesToAllFrame()
{
  const int frame_idx = formVisParam_getframeI();
  const int mum_frames = ImageCore::GetInst()->GetNumFrames();
  Do_RecordSnapShot();
  for (int i = 0; i < mum_frames; ++i)
  {
    if (i == frame_idx) continue;
    m_curves[i] = m_curves[frame_idx];
  }
  std::cout << "Copy strokes to all frame.\n";
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}



static bool FindNearestVertex(
  const EVec3f&   pos,
  const TMesh& mesh,
  const EVec3f& crssec_normal,
  const EVec3f& curve_tangent,
  const EVec3f& curve_normal ,
  int&  min_idx,
  float& min_dist)
{
  min_dist = FLT_MAX;
  min_idx = -1;
  
  for (int k = 0; k < mesh.m_vSize; ++k)
  {
    if (mesh.m_vNorms[k].dot(curve_normal) <= 0.0f) continue;

    const EVec3f diff = pos - mesh.m_vVerts[k];
    Eigen::Matrix3f mat = 
        powf(5.0f, 2.0f) * crssec_normal * crssec_normal.transpose()
      + powf(1.0f, 2.0f) * curve_tangent * curve_tangent.transpose()
      + powf(0.2f, 2.0f) * curve_normal  * curve_normal.transpose();
    float d = diff.transpose() * mat * diff;

    if (d < min_dist)
    {
      min_dist = d;
      min_idx = k;
    }
  }
  return (min_idx >= 0);
}


static void FindPullOrientation(
  const int _frame_idx,
  const TMesh& mesh,
  const std::vector<PlanarCurve>& curves,
  const std::vector<SharedCurves>& shared_curves,
  std::vector<int>&    const_vids,
  std::vector<EVec3f>& const_trgtpos) 
{
  std::vector<float>  vert_mindist  (mesh.m_vSize, FLT_MAX);
  std::vector<EVec3f> vert_targetpos(mesh.m_vSize, EVec3f(0.0f, 0.0f, 0.0f));

  const int num_curves = (int) curves.size();
  const int num_shared_curves = (int) shared_curves.size();

  for (int crv_i = 0; crv_i < num_curves + num_shared_curves; ++crv_i)
  {
    const PlanarCurve& c = (crv_i < num_curves) ? curves[crv_i] :
      shared_curves[crv_i - num_curves].GetCurve(_frame_idx);

    const auto&  points        = c.GetCurve();
    const bool   normal_side   = c.GetNormalSide();
    const EVec3f crssec_normal = c.GetCrssecNorm();

    for (int idx = 1; idx < (int)points.size() - 1; ++idx)
    {
      EVec3f curve_tangent = (points[idx + 1] - points[idx - 1]);
      if (curve_tangent.norm() < 0.0001f) continue;
      curve_tangent.normalize();

      EVec3f curve_normal = crssec_normal.cross(curve_tangent).normalized();
      curve_normal *= (normal_side ? 1.0f : -1.0f);

      // get nearest vertex idx
      float min_dist = FLT_MAX;
      int   min_idx  = -1;
      bool tf = FindNearestVertex( points[idx], mesh,
                                   crssec_normal, curve_tangent, curve_normal,
                                   min_idx, min_dist);
      if (!tf) continue;
      //mesh vertex側からみて、より近い点なら更新
      if (min_dist < vert_mindist[min_idx])
      {
        vert_mindist[min_idx] = min_dist;
        vert_targetpos[min_idx] = points[idx];
      }
    }
  }
  
  const_vids.clear();
  const_trgtpos.clear();

  for (int vi = 0; vi < mesh.m_vSize; ++vi)
  {
    if (vert_mindist[vi] < FLT_MAX)
    {
      const_vids.push_back(vi);
      const_trgtpos.push_back(vert_targetpos[vi]);
    }
  }
}


void ModeRefCurveDeform::_Deform(const int _frame_idx)
{
  if (m_meshes_orig.size() == 0 || m_meshes_orig.front().m_vSize == 0) return;
  if (m_curves[_frame_idx].size() + m_shared_curves.size() == 0) return;

  std::cout << "_Deform : " << _frame_idx << "\n";

  m_meshes_def[_frame_idx] = m_meshes_orig[_frame_idx];

  std::vector<int>    const_vids;
  std::vector<EVec3f> const_trgtpos;
  FindPullOrientation(
    _frame_idx, 
    m_meshes_def[_frame_idx],
    m_curves[_frame_idx],
    m_shared_curves,
    const_vids, const_trgtpos);
  
  m_debug_matched_vids[_frame_idx] = const_vids;
  m_debug_matched_trgtpos[_frame_idx] = const_trgtpos;

  if (const_vids.size() == 0) return;

  // Deform
  LaplacianDeformer deformer;
  deformer.Deform(m_meshes_def[_frame_idx], const_vids, const_trgtpos);
}


void ModeRefCurveDeform::DeformCurrentFrame()
{
  _Deform(formVisParam_getframeI());
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
    file << "num_curves: " << std::to_string(m_curves[fi].size()) << "\n";
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

  std::cout << "Save as: " + _fpath + "\n";
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
  file.close();
  formMain_RedrawMainPanel();

}



void ModeRefCurveDeform::KeyDown(int nChar)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int frame_idx = formVisParam_getframeI();

  if (nChar == VK_F1)
  {
    std::cout << "COMPUTE Matching\n";
    const TMesh& mesh = m_meshes_orig[frame_idx];
    m_debug_matched_vids   [frame_idx].clear();
    m_debug_matched_trgtpos[frame_idx].clear();
    std::vector<int>    const_vids;
    std::vector<EVec3f> const_trgt;
    FindPullOrientation( frame_idx, mesh, 
                         m_curves[frame_idx], m_shared_curves,
                         const_vids, const_trgt);

    for (size_t i = 0; i < const_vids.size(); ++i)
    {
      EVec3f pos = mesh.m_vVerts[ const_vids[i] ];
      m_debug_matched_vids   [frame_idx].push_back(const_vids[i]);
      m_debug_matched_trgtpos[frame_idx].push_back(const_trgt[i]);
    }
  }
  else if (nChar == VK_F2)
  {
    // deform all frame
    DeformAllFrame();
  }
  else if (nChar == VK_F3)
  {
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
    for (int fi = 0; fi < num_frames; ++fi)
    {
      std::cout << "Frame " << fi << " : num curves = " << m_curves[fi].size() << "\n"; 
    }
    std::cout << "num shared curves = " << m_shared_curves.size() << "\n";
  }
  else if (nChar == VK_F9 ) {}
  else if (nChar == VK_F10) {}
  else if (nChar == VK_F11) {}
  else if (nChar == VK_F12)
  {
      std::cout << "[F1]  Show matching between strokes and mesh" << "\n";
      std::cout << "[F2]  Deform mesh all frame" << "\n";
      std::cout << "[F3]  Reload mesh all frame" << "\n";
      std::cout << "[F4]  Share selected stroke" << "\n";
      std::cout << "[F5]  Unshare selected stroke" << "\n";
      //std::cout << "[F6]  Lock selected stroke" << "\n";
      //std::cout << "[F7]  Unlock selected stroke" << "\n";
      std::cout << "[F8]  Show Num of curves" << "\n";
  }
}

void ModeRefCurveDeform::KeyUp(int nChar) {}
