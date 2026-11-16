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
#include "tvolumelader.h"
#include "RefCurveDeformData.h"
#include <algorithm>
#include <exception>
#include <sstream>

using namespace RoiPainter4D;
using namespace System::IO;

ModeRefCurveDeform::~ModeRefCurveDeform() {}

ModeRefCurveDeform::ModeRefCurveDeform()
{
  std::cout << "ModeRefCurveDeform...\n";
  m_bL = m_bR = m_bM = false;
  std::cout << "ModeRefCurveDeform done\n";
}


bool ModeRefCurveDeform::CanEndMode()
{
  if (!m_b_modified) return true;

  return ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Leaving?");
}


void ModeRefCurveDeform::StartMode()
{
  m_b_modified = false;
  m_meshes_imported = false;

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
  m_meshes_orig.clear();
  m_meshes_def.clear();
  m_curves = std::vector<std::vector<PlanarCurve>>(num_frame);
  m_select_info.Clear();
  m_current_frame_idx = formVisParam_getframeI();

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


void ModeRefCurveDeform::OnFrameChanged(int frame_idx)
{
  if (m_current_frame_idx == frame_idx) return;

  // Finish a shared-curve drag before discarding its selection.
  const int curve_idx = m_select_info.curve_idx;
  if (m_bL && m_select_info.selected && m_select_info.is_shared &&
      0 <= curve_idx && curve_idx < (int)m_shared_curves.size())
    m_shared_curves[curve_idx].UpdateNonManipCurves();

  // Standard curve indices belong to one frame, even when another frame
  // happens to contain a curve with the same index.
  m_select_info.Clear();
  m_bL = false;
  m_current_frame_idx = frame_idx;
}


void ModeRefCurveDeform::ValidateSelectionForFrame(int frame_idx)
{
  OnFrameChanged(frame_idx);
  if (!m_select_info.selected) return;

  const int curve_idx = m_select_info.curve_idx;
  const int cp_idx = m_select_info.cp_idx;
  if (frame_idx < 0 || frame_idx >= (int)m_curves.size() ||
      curve_idx < 0 || cp_idx < 0)
  {
    m_select_info.Clear();
    return;
  }

  if (m_select_info.is_shared)
  {
    if (curve_idx >= (int)m_shared_curves.size() ||
        frame_idx >= m_shared_curves[curve_idx].GetNumFrames() ||
        cp_idx >= m_shared_curves[curve_idx].GetCurve(frame_idx).GetNumCPs())
      m_select_info.Clear();
  }
  else if (curve_idx >= (int)m_curves[frame_idx].size() ||
           cp_idx >= m_curves[frame_idx][curve_idx].GetNumCPs())
  {
    m_select_info.Clear();
  }
}


PlanarCurveSelectionInfo ModeRefCurveDeform::PickCpAtCurrentFrame(
    const EVec3f& ray_pos, 
    const EVec3f& ray_dir)
{
  const int frame_idx = formVisParam_getframeI();
  ValidateSelectionForFrame(frame_idx);
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
  const int frame_idx = formVisParam_getframeI();
  ValidateSelectionForFrame(frame_idx);
  m_bL = true;
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
        //最近傍のmesh法線を取得
        EVec3f mesh_normal(0.0f, 0.0f, 0.0f);
        if (frame_idx >= 0 && frame_idx < m_meshes_def.size() && m_meshes_def[frame_idx].m_vSize > 0)
        {
          const TMesh& mesh = m_meshes_def[frame_idx];
          int nearest_vertex_idx = mesh.GetNearestVertexIdx(pos);
          if (nearest_vertex_idx >= 0) mesh_normal = mesh.m_vNorms[nearest_vertex_idx];
        }

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
          if(m_curves[frame_idx][m_select_info.curve_idx].AddCP(pos, cpidx)) m_select_info.cp_idx = cpidx;
          //2点目が追加されたら外側を向くように法線計算
          if(m_curves[frame_idx][m_select_info.curve_idx].GetNumCPs() == 2) m_curves[frame_idx][m_select_info.curve_idx].AlignNormalWithMesh(mesh_normal);
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
  const int frame_idx = formVisParam_getframeI();
  ValidateSelectionForFrame(frame_idx);
  m_bL = false;

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
        m_select_info = info;
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
  const int frame_idx = formVisParam_getframeI();
  ValidateSelectionForFrame(frame_idx);
  if (!m_bL && !m_bR && !m_bM) return;

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
  ValidateSelectionForFrame(frame_idx);
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
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    m_meshes_def[frame_idx].Draw();
    glDisable(GL_POLYGON_OFFSET_FILL);
    glUseProgram(0);
  }

  //DrawCurves
  const bool VIS_CP = IsShiftKeyOn();
  const bool VIS_ONLY_SEL = FormRefCurveDeform_VisOnlySelCurve();

  DrawPlanerCurvesAndSharedCurves(
    frame_idx, m_select_info, 
    VIS_ONLY_SEL, VIS_CP, CP_RAD, 
    m_curves[frame_idx], m_shared_curves);


  if (IsSpaceKeyOn() && 0 <= frame_idx && frame_idx < (int)m_meshes_orig.size())
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
  m_select_info.Clear();
  m_bL = false;

  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}



void ModeRefCurveDeform::RebuildMeshSequence(
    const std::vector<const byte*>& masks, bool imported)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const EVec3i reso = ImageCore::GetInst()->GetReso();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();
  const int stride = FormRefCurveDeform_GetMcStride();
  if (num_frames <= 0 || masks.size() != num_frames ||
      m_target_mask_id <= 0 || m_target_mask_id > 255 ||
      stride <= 0 || reso.minCoeff() < stride)
    throw std::invalid_argument("Invalid frame count, target label or mesh sampling stride.");

  // Keep the current meshes until every frame has been generated successfully.
  std::vector<TMesh> original(num_frames);
  std::exception_ptr failure;
#pragma omp parallel for
  for (int fi = 0; fi < num_frames; ++fi)
  {
    try
    {
      auto sampled = RefCurveDeformData::DownsampleMask(masks[fi], reso, stride, (byte)m_target_mask_id);
      TMesh& mesh = original[fi];
      marchingcubes::MarchingCubes(reso / stride, pitch, sampled.data(), 128, 0, 0, mesh);
      mesh.Scale(static_cast<float>(stride));
      mesh.Smoothing(3);
    }
    catch (...)
    {
#pragma omp critical(refcurve_mesh_failure)
      {
        if (!failure) failure = std::current_exception();
      }
    }
  }
  if (failure) std::rethrow_exception(failure);
  std::vector<TMesh> deformed = original;
  std::vector<std::vector<int>> matched_vids(num_frames);
  std::vector<std::vector<EVec3f>> matched_targets(num_frames);
  m_meshes_orig.swap(original);
  m_meshes_def.swap(deformed);
  m_debug_matched_vids.swap(matched_vids);
  m_debug_matched_trgtpos.swap(matched_targets);
  m_meshes_imported = imported;
  m_b_modified = imported;
}


void ModeRefCurveDeform::ConvertMaskToMesh()
{
  const auto& mask_data = ImageCore::GetInst()->GetMaskData();
  if (m_target_mask_id <= 0 || m_target_mask_id >= (int)mask_data.size()) return;
  const auto& masks = ImageCore::GetInst()->m_mask4d;
  try
  {
    RebuildMeshSequence(std::vector<const byte*>(masks.begin(), masks.end()), false);
  }
  catch (const std::exception& error)
  {
    ShowMsgDlg_OK(error.what(), "Create mesh models");
    return;
  }
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}

void ModeRefCurveDeform::CancelSegmentation()
{
  if (ShowMsgDlgYesNo("分割結果を破棄して良いですか?\nDo you want to cancel segmentation? ", "Cancel Segmentation?")) 
  {
    m_b_modified = false;
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

  if (!m_b_modified)
  {
    ShowMsgDlg_OK("編集された領域が存在しません．\n No edited region exist", "no edited.");
    return;
  }

  m_b_modified = false;

  byte mask_locked[256] = {};
  for (int i = 0; i < (int)mask_data.size(); ++i)
  {
    mask_locked[i] = mask_data[i].lock ? 1 : 0;
  }
  mask_locked[m_target_mask_id] = 0;

#pragma omp parallel for
  for (int fi = 0; fi < num_frames; ++fi)
  {
    if (!m_meshes_imported && m_curves[fi].empty() && m_shared_curves.empty()) continue; //変化無し

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
  const int frame_idx = formVisParam_getframeI();
  if (frame_idx < 0 || frame_idx >= (int)m_meshes_orig.size() ||
      frame_idx >= (int)m_meshes_def.size()) return;
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
  m_select_info.Clear();
  m_bL = false;
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
    const EVec3f crssec_normal = c.GetCrssecNorm();

    for (int idx = 1; idx < (int)points.size() - 1; ++idx)
    {
      EVec3f curve_tangent = (points[idx + 1] - points[idx - 1]);
      if (curve_tangent.norm() < 0.0001f) continue;
      curve_tangent.normalize();

      EVec3f curve_normal;
      if (!c.GetCurveNormal(idx, curve_normal)) continue;

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
  if (_frame_idx < 0 || _frame_idx >= (int)m_meshes_orig.size() ||
      _frame_idx >= (int)m_meshes_def.size() ||
      m_meshes_orig[_frame_idx].m_vSize == 0) return;
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
#pragma omp critical(refcurve_modified)
  {
    m_b_modified = true;
  }
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




void ModeRefCurveDeform::FlipSelectedCurveNormals()
{
  const int frame_idx = formVisParam_getframeI();
  ValidateSelectionForFrame(frame_idx);
  if (!m_select_info.selected) return;

  // Normal direction changes the deformation constraints and must be undoable.
  Do_RecordSnapShot();
  const int curve_idx = m_select_info.curve_idx;
  if (m_select_info.is_shared) m_shared_curves[curve_idx].FlipNormal();
  else m_curves[frame_idx][curve_idx].FlipNormal();
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::ToggleSelectedCurveNormals()
{
  const int frame_idx = formVisParam_getframeI();
  ValidateSelectionForFrame(frame_idx);
  if (!m_select_info.selected) return;

  const int curve_idx = m_select_info.curve_idx;
  if (m_select_info.is_shared) m_shared_curves[curve_idx].ToggleNormalVisibility();
  else m_curves[frame_idx][curve_idx].ToggleNormalVisibility();
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}

void ModeRefCurveDeform::MakeSelectedStroke_Shared()
{
  const int frame_idx = formVisParam_getframeI();
  ValidateSelectionForFrame(frame_idx);
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
  m_select_info.is_shared = true;
  m_select_info.curve_idx = (int)m_shared_curves.size() - 1;

  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}



void ModeRefCurveDeform::MakeSelectedStroke_Unshared()
{
  const int frame_idx = formVisParam_getframeI();
  ValidateSelectionForFrame(frame_idx);
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  bool tf = !m_select_info.selected  || 
            !m_select_info.is_shared ||
             m_select_info.curve_idx < 0 ||
             m_shared_curves.size() <= m_select_info.curve_idx;
  if (tf) return;
  
  for( int fi = 0; fi < num_frames; ++fi )
  {
    if (m_shared_curves[m_select_info.curve_idx].IsManipulated(fi) )
    {
      PlanarCurve c = m_shared_curves[m_select_info.curve_idx].GetCurve(fi);
      m_curves[fi].push_back(c);
    }
  }

  m_shared_curves.erase(m_shared_curves.begin() + m_select_info.curve_idx);
  m_select_info.Clear();
  m_bL = false;
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

  // The loaded curves replace the objects referenced by the old selection.
  m_select_info.Clear();
  m_bL = false;

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

bool ModeRefCurveDeform::ExportDeformationConfig(
    const std::wstring& config_path, const std::string& input_dir,
    const std::string& output_dir, std::string& error) const
{
  // Validate and serialize first, so invalid data cannot truncate an existing file.
  std::ostringstream contents;
  if (!RefCurveDeformData::WriteConfig(contents, input_dir, output_dir,
      m_target_mask_id, m_curves, m_shared_curves, error)) return false;
  std::ofstream file(config_path.c_str(), std::ios::binary);
  if (!file)
  {
    error = "Cannot open the YAML configuration file for writing.";
    return false;
  }
  file << contents.str();
  file.close();
  if (!file)
  {
    error = "Failed to write the complete YAML configuration file.";
    return false;
  }
  return true;
}


bool ModeRefCurveDeform::ImportMeshSequenceFromMha(
    std::vector<std::wstring> file_paths, std::string& error)
{
  error.clear();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const EVec3i expected_reso = ImageCore::GetInst()->GetReso();
  const EVec3f expected_pitch = ImageCore::GetInst()->GetPitch();
  if (num_frames <= 0 || file_paths.size() != num_frames || num_voxels <= 0 ||
      m_target_mask_id <= 0 || m_target_mask_id > 255)
  {
    error = "Select one MHA mask for each image frame and a valid target mask.";
    return false;
  }
  std::sort(file_paths.begin(), file_paths.end(), RefCurveDeformData::FrameFileNameLess);
  try
  {
    std::vector<std::unique_ptr<byte[]>> masks;
    bool labels[256] = {};
    for (int fi = 0; fi < num_frames; ++fi)
    {
      EVec3i reso;
      EVec3f pitch;
      std::unique_ptr<byte[]> bytes;
      std::unique_ptr<short[]> shorts;
      const std::string frame = "Frame " + std::to_string(fi) + ": ";
      if (!ReadMha3d(file_paths[fi], reso, pitch, bytes, shorts))
      {
        error = frame + "Cannot read the MHA mask. Use a complete, local, little-endian MET_UCHAR or MET_SHORT volume.";
        return false;
      }
      if (reso != expected_reso ||
          (pitch - expected_pitch).cwiseAbs().maxCoeff() >
              0.0001f * std::max(1.0f, expected_pitch.cwiseAbs().maxCoeff()))
      {
        error = frame + "Mask dimensions or voxel spacing do not match the loaded image.";
        return false;
      }
      if (shorts)
      {
        bytes = std::make_unique<byte[]>(num_voxels);
        for (int i = 0; i < num_voxels; ++i)
        {
          if (shorts[i] < 0 || shorts[i] > 255)
          {
            error = frame + "Mask labels must be between 0 and 255.";
            return false;
          }
          bytes[i] = static_cast<byte>(shorts[i]);
        }
      }
      for (int i = 0; i < num_voxels; ++i) labels[bytes[i]] = true;
      masks.push_back(std::move(bytes));
    }

    // Accept conventional binary masks as well as masks carrying the target ID.
    int foreground_label = 0, foreground_labels = 0;
    for (int label = 1; label < 256; ++label)
      if (labels[label]) { foreground_label = label; ++foreground_labels; }
    if (!labels[m_target_mask_id] && foreground_labels > 0)
    {
      if (foreground_labels != 1 || (foreground_label != 1 && foreground_label != 255))
      {
        error = "The MHA masks do not contain the selected target label. Binary masks must use 1 or 255 for foreground.";
        return false;
      }
      for (auto& mask : masks)
        for (int i = 0; i < num_voxels; ++i)
          if (mask[i] != 0) mask[i] = static_cast<byte>(m_target_mask_id);
    }
    std::vector<const byte*> mask_data;
    for (const auto& mask : masks) mask_data.push_back(mask.get());
    RebuildMeshSequence(mask_data, true);
  }
  catch (const std::exception& exception)
  {
    error = std::string("Cannot import the mesh sequence: ") + exception.what();
    return false;
  }
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
  return true;
}

void ModeRefCurveDeform::KeyDown(int nChar)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int frame_idx = formVisParam_getframeI();

  if (nChar == VK_F1)
  {
    if (frame_idx < 0 || frame_idx >= (int)m_meshes_orig.size()) return;
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
    for (int i = 0; i < (int)m_meshes_orig.size(); ++i)
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
