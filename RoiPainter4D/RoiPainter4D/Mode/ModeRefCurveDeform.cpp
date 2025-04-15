#pragma unmanaged

#include "ModeRefCurveDeform.h"
#include "ModeCommonTools.h"

#include "OglForCLI.h"   // openGLを利用するためのクラス
#include "ImageCore.h"   // 画像データを保持しているクラス 　
#include "CrsSecCore.h"  // 切断面描画や、切断面への操作を管理
#include "MaskMeshSequence.h"
#include "LaplacianSurfaceEditing.h"
#include <fstream>
#include <queue>

#pragma managed
#include "CliMessageBox.h"
#include "FormMain.h"     // Main Window
#include "FormVisParam.h" // 右上のダイアログ
#include "FormRefCurveDeform.h"
#pragma unmanaged

//#define DEBUG_MODE_REF_CURVEDEFORM


using namespace RoiPainter4D;


ModeRefCurveDeform::ModeRefCurveDeform()
{
  std::cout << "ModeRefCurveDeform...\n";
  m_bL = m_bR = m_bM = false;
  std::cout << "ModeRefCurveDeform done\n";
}


ModeRefCurveDeform::~ModeRefCurveDeform()
{

}


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
  m_bL = m_bR = m_bM = false;
  m_is_not_saved_state = false;

  m_mask_mesh = MaskMeshSequence();
  auto c = ImageCore::GetInst()->GetCuboidF();
  m_cp_rate = c[0] * 0.0006f;
  m_cp_size = 10;
  m_prev_frame_idx = formVisParam_getframeI();
  m_strokes = std::vector<DeformationStrokes>(ImageCore::GetInst()->GetNumFrames());
  m_tmeshes = std::vector<TMesh>(ImageCore::GetInst()->GetNumFrames());
  m_laplacian_deformer = std::vector<LaplacianDeformer>();
  //for (int i = 0; i < ImageCore::GetInst()->GetNumFrames(); ++i) {
  //  m_laplacian_deformer.push_back(LaplacianDeformer(m_mask_mesh.GetMesh(i)));
  //}
  m_shared_stroke_idxs = std::set<int>();
  m_histories = std::vector<History>(ImageCore::GetInst()->GetNumFrames());
  m_show_only_selected_stroke = true;
  m_prev_selected_stroke_idx = -1;
  m_draw_surf_trans = true;
  m_exist_mesh = false;

  FormRefCurveDeform_InitAllItems();
  FormRefCurveDeform_Show();
  std::cout << "ModeRefCurveDeform...startMode DONE-----\n";
  std::cout << "Debug mode is available (Shift+Ctrl+F12).\n";
}


void ModeRefCurveDeform::LBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = true;
  const int frame_idx = formVisParam_getframeI();

  if (IsShiftKeyOn())
  {
    Do();
    EVec3f ray_pos, ray_dir, pos;
    ogl->GetCursorRay(p, ray_pos, ray_dir);
    const int selected_stroke_idx = m_strokes[frame_idx].PickControlPoint(ray_pos, ray_dir, m_cp_rate * m_cp_size, false, m_show_only_selected_stroke);

    if (selected_stroke_idx == -1)
    {
      DeformationStrokes& dstroke = m_strokes[frame_idx];
      if (PickCrssec(ray_pos, ray_dir, pos) != CRSSEC_NON)
      {
        if (dstroke.GetSelectedStrokeIdx() == -1)
        {
          dstroke.AddStroke();
        }
        if (!dstroke.AddControlPoint(pos))
        {
          dstroke.ReleaseStroke();
          m_prev_selected_stroke_idx = -1;
        }
      }
      else
      {
        dstroke.ReleaseStroke();
        m_prev_selected_stroke_idx = -1;
      }
    }
    else
    {
      m_strokes[frame_idx].PickControlPoint(ray_pos, ray_dir, m_cp_rate * m_cp_size, true, m_show_only_selected_stroke);
      const int common_xyz = m_strokes[frame_idx].GetCommonXYZ();
      const float common_coord = m_strokes[frame_idx].GetCommonCoord();
      const EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();
      if (common_xyz == 0)
      {
        CrssecCore::GetInst()->SetPlanePosYZ(common_coord, cuboid);
      }
      else if (common_xyz == 1)
      {
        CrssecCore::GetInst()->SetPlanePosZX(common_coord, cuboid);
      }
      else if (common_xyz == 2)
      {
        CrssecCore::GetInst()->SetPlanePosXY(common_coord, cuboid);
      }
    }
  }
  else
  {
    m_strokes[frame_idx].ReleaseStroke();
    m_prev_selected_stroke_idx = -1;
    ogl->BtnDown_Trans(p);
  }

  m_prevpt = m_initpt = p;
  formMain_RedrawMainPanel();
}


void ModeRefCurveDeform::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = false;
  const int frame_idx = formVisParam_getframeI();

  if (m_strokes[frame_idx].IsSelectedStrokeShared())
  {
    UpdateSharedStroke();
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
    const int selected_stroke_idx = m_strokes[frame_idx].PickControlPoint(ray_pos, ray_dir, m_cp_rate * m_cp_size, true, m_show_only_selected_stroke);

    if (selected_stroke_idx != -1)
    {
      if (m_strokes[frame_idx].IsSelectedStrokeShared() == true)
      {
        if (ShowMsgDlgYesNo("All-frame曲線を解除しますか？\n（自動補間された曲線は削除されます）", "Unlock?"))
        {
          UnshareSelectedStroke();
        }
      }
      else
      {
        m_prev_selected_stroke_idx = selected_stroke_idx;
        Do();
        m_strokes[frame_idx].DeleteSelectedControlPoint();
      }
    }
  }
  else
  {
    m_strokes[frame_idx].ReleaseStroke();
    m_prev_selected_stroke_idx = -1;
    ogl->BtnDown_Rot(p);
  }
  
  m_prevpt = m_initpt = p;
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

  const int frame_idx = formVisParam_getframeI();
  m_strokes[frame_idx].ReleaseStroke();
  m_prev_selected_stroke_idx = -1;

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
      m_strokes[frame_idx].MoveSelectedControlPoint(pos);
    }
  }
  else
  {
    ogl->MouseMove(p);
  }

  m_prevpt = p;
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


void ModeRefCurveDeform::LBtnDclk(const EVec2i& p, OglForCLI* ogl)
{

}


void ModeRefCurveDeform::RBtnDclk(const EVec2i& p, OglForCLI* ogl)
{
  const int frame_idx = formVisParam_getframeI();

  if (m_prev_selected_stroke_idx != -1)
  {
    m_strokes[frame_idx].SetSelectedStrokeIdx(m_prev_selected_stroke_idx);
    if (m_strokes[frame_idx].IsSelectedStrokeShared() == true)
    {
      if (ShowMsgDlgYesNo("All-frame曲線を解除しますか？\n（自動補間された曲線は削除されます）", "Unlock?"))
      {
        UnshareSelectedStroke();
      }
    }
    else
    {
      Do();
      m_strokes[frame_idx].DeleteSelectedStroke();
    }
  }
}


void ModeRefCurveDeform::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}


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
    // deform all frame
    const int num_frames = ImageCore::GetInst()->GetNumFrames();
    for (int i = 0; i < num_frames; ++i)
    {
      ReloadMesh(i);
    }
  }
  else if (nChar == VK_F4)
  {
    // share selected stroke
    ShareSelectedStroke();
  }
  else if (nChar == VK_F5)
  {
    // unshare selected stroke
    UnshareSelectedStroke();
  }
  else if (nChar == VK_F6)
  {
    // lock selected stroke
    LockSelectedStroke();
  }
  else if (nChar == VK_F7)
  {
    // unlock selected stroke
    UnlockSelectedStroke();
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



void ModeRefCurveDeform::DrawScene(
    const EVec3f& cam_pos,
    const EVec3f& cam_cnt)
{
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
  if (!IsSpaceKeyOn())
  {
    m_mask_mesh.DrawMesh(frame_idx);
  }

  // draw stroke
  if (!IsSKeyOn())
  {
    m_strokes[frame_idx].DrawStrokes(m_show_only_selected_stroke);

    for (int i = 0; i < m_matched_pos.size(); ++i)
    {
      static const float COLOR_W[4] = { 1.0f, 1.0f, 1.0f, 0.5f };
      static const float COLOR_SHIN64[1] = { 64 };
      static const float COLOR_G[4] = { 0.0f, 1.0f, 0.0f, 0.5f };

      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, COLOR_W);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, COLOR_SHIN64);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, COLOR_G);
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, COLOR_G);

      glEnable(GL_LIGHTING);
      TMesh::DrawSphere(m_matched_pos[i], 0.3f);
      glDisable(GL_LIGHTING);
    }
  }

  // draw control points
  if (IsShiftKeyOn())
  {
    m_strokes[frame_idx].DrawControlPoints(m_cp_rate * m_cp_size, m_show_only_selected_stroke);
  }

}


void ModeRefCurveDeform::Deform()
{
  const int frame_idx = formVisParam_getframeI();
  _Deform(frame_idx);
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
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


void ModeRefCurveDeform::Do()
{
  const int frame_idx = formVisParam_getframeI();
  Action now = { m_strokes[frame_idx] };
  History& history = m_histories[frame_idx];
  if (history.undo.empty() || (now.strokes != history.undo.top().strokes))
  {
    history.undo.push(now);
    history.redo = std::stack<Action>();
  }
  formMain_RedrawMainPanel();
  m_is_not_saved_state = true;
}


void ModeRefCurveDeform::Undo()
{
  const int frame_idx = formVisParam_getframeI();
  Action now = { m_strokes[frame_idx] };
  History& history = m_histories[frame_idx];
  if (!history.undo.empty())
  {
    const Action prev = history.undo.top();
    history.undo.pop();
    history.redo.push(now);
    m_strokes[frame_idx] = prev.strokes;
    std::cout << "UNDO\n";
  }
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::Redo()
{
  const int frame_idx = formVisParam_getframeI();
  Action now = { m_strokes[frame_idx] };
  History& history = m_histories[frame_idx];
  if (!history.redo.empty())
  {
    const Action next = history.redo.top();
    history.redo.pop();
    history.undo.push(now);
    m_strokes[frame_idx] = next.strokes;
    std::cout << "REDO\n";
  }
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::SetCPSize()
{
  const int cp_size = FormRefCurveDeform_GetCPSize();

  if (cp_size <= 0)
  {
    std::cout << "1以上の整数値を入力してください.\n";
    return;
  }

  m_cp_size = cp_size;

  std::cout << "Change CP size: " << m_cp_size << std::endl;
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::ConvertMaskToMesh()
{
  const int scale = FormRefCurveDeform_GetMCScale();

  if (!m_mask_mesh.LoadMask(scale))
  {
    std::cout << "Failed to load mask." << "\n";
  }

  for (int i = 0; i < ImageCore::GetInst()->GetNumFrames(); ++i) {
    m_tmeshes[i] = m_mask_mesh.GetMesh(i);
  }

  m_laplacian_deformer = std::vector<LaplacianDeformer>();
  for (int i = 0; i < ImageCore::GetInst()->GetNumFrames(); ++i) {
    m_laplacian_deformer.push_back(LaplacianDeformer(m_mask_mesh.GetMesh(i)));
  }

  m_exist_mesh = true;

  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::ConvertMeshToMask()
{
  m_mask_mesh.UpdateMask();
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
  m_is_not_saved_state = false;
}


void ModeRefCurveDeform::ReloadMesh()
{
  const int frame_idx = formVisParam_getframeI();
  ReloadMesh(frame_idx);
}


void ModeRefCurveDeform::ReloadMesh(const int _frame_idx)
{
  if (!m_exist_mesh || !m_mask_mesh.is_initialized) return;

  m_mask_mesh.GetMesh(_frame_idx).Set(m_tmeshes[_frame_idx]);

  std::cout << "Mesh reloaded.\n";

  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::CopyFromPrevFrame()
{
  const int frame_idx = formVisParam_getframeI();
  if (frame_idx <= 0) return;
  m_strokes[frame_idx] = m_strokes[frame_idx - 1];
  std::cout << "Copy from previous frame.\n";
  Do();
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::CopyStrokesToAllFrame()
{
  const int frame_idx = formVisParam_getframeI();
  for (int i = 0; i < ImageCore::GetInst()->GetNumFrames(); ++i) {
    if (i == frame_idx) continue;
    m_strokes[i] = m_strokes[frame_idx];
  }
  std::cout << "Copy strokes to all frame.\n";
  Do();
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::SetShowOnlySelectedStroke()
{
  m_show_only_selected_stroke = FormRefCurveDeform_GetShowOnlySelectedStroke();
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::_Deform(const int _frame_idx)
{
  if (!m_exist_mesh || !m_mask_mesh.is_initialized) return;

  m_mask_mesh.GetMesh(_frame_idx).Set(m_tmeshes[_frame_idx]);

  std::vector<int> fixed_verts_idxs;
  std::vector<EVec3f> fixed_positions;
  std::vector<EVec3f> src_positions;

  FindClosestPointFromStroke(_frame_idx, fixed_verts_idxs, fixed_positions, src_positions);

  // Sort by index
  std::vector<std::pair<int, EVec3f>> indexPointPairs;

  for (size_t i = 0; i < fixed_verts_idxs.size(); ++i) {
    indexPointPairs.emplace_back(fixed_verts_idxs[i], fixed_positions[i]);
  }

  std::sort(indexPointPairs.begin(), indexPointPairs.end(),
    [](const std::pair<int, EVec3f>& a, const std::pair<int, EVec3f>& b) {
      return a.first < b.first;
    });

  for (size_t i = 0; i < indexPointPairs.size(); ++i) {
    fixed_verts_idxs[i] = indexPointPairs[i].first;
    fixed_positions[i] = indexPointPairs[i].second;
  }

  if (fixed_verts_idxs.size() == 0) return;

  // Deform
  m_laplacian_deformer[_frame_idx].Deform(fixed_verts_idxs, fixed_positions);
}



void ModeRefCurveDeform::FindClosestPointFromStroke(const int _frame_idx, std::vector<int>& _idxs, std::vector<EVec3f>& _target_pos, std::vector<EVec3f>& _src_pos)
{
  if (!m_mask_mesh.is_initialized) return;

  TMesh& mesh = m_mask_mesh.GetMesh(_frame_idx);

  auto& strokes = m_strokes[_frame_idx].GetStroke();
  const int num_stroke = static_cast<int>(strokes.size());

  // for stroke
  for (int i = 0; i < num_stroke; ++i) {
    const auto stroke = strokes[i];
    const int num_p = static_cast<int>(stroke.size());
    const int commonXYZ = m_strokes[_frame_idx].GetStrokeInst(i)->GetCommonXYZ();
    const EVec3f vec0 = EVec3f(
      commonXYZ == 0 ? 1.0f : 0.0f,
      commonXYZ == 1 ? 1.0f : 0.0f,
      commonXYZ == 2 ? 1.0f : 0.0f
    );

#ifdef DEBUG_MODE_REF_CURVEDEFORM
    std::cout << "num_p: " << num_p << "\n";
#endif

    // order by curvature
    std::vector<std::pair<int, float>> curvature_idxs = std::vector<std::pair<int, float>>();
    for (int j = 0; j < num_p; ++j)
    {

      const EVec3f& p_prev = j > 0 ? stroke[j - 1] : stroke[0];
      const EVec3f& p = stroke[j];
      const EVec3f& p_next = j < num_p - 1 ? stroke[j + 1] : stroke[num_p - 1];

      if (p(0) < 0.0f || p(1) < 0.0f || p(2) < 0.0f)
      {
        continue;
      }

      const EVec3f vec0 = p - p_prev;
      const EVec3f vec1 = p_next - p;
      const float  len0 = vec0.norm();
      const float  len1 = vec1.norm();

      const float curvature = (len0 + len1 > 0.0f) ? (2.0f * vec0.cross(vec1).norm()) / (len0 * len1) : 0.0f;

      curvature_idxs.emplace_back(j, curvature);
    }

    std::sort(curvature_idxs.begin(), curvature_idxs.end(),
      [](const std::pair<float, int>& a, const std::pair<float, int>& b) {
        return a.second > b.second;
      });

    std::vector<int> sorted_idxs;
    for (const auto& pair : curvature_idxs) {
      sorted_idxs.push_back(pair.first);
    }

    // for points
    for (int idx : sorted_idxs) {
      const EVec3f& p_prev = idx > 0 ? stroke[idx - 1] : stroke[0];
      const EVec3f& p = stroke[idx];
      const EVec3f& p_next = idx < num_p - 1 ? stroke[idx + 1] : stroke[num_p - 1];
      const EVec3f vec1 = (p_next - p_prev).normalized();
      const EVec3f vec2 = vec0.cross(vec1).normalized();

      // get nearest vertex idx
      float dist_max = FLT_MAX;
      int idx_max = -1;
      for (int k = 0; k < mesh.m_vSize; ++k)
      {
        const EVec3f diff = p - mesh.m_vVerts[k];
        Eigen::MatrixXf mat =
            powf(5.0f, 2.0f) * vec0 * vec0.transpose()
          + powf(1.0f, 2.0f) * vec1 * vec1.transpose()
          + powf(0.2f, 2.0f) * vec2 * vec2.transpose();
        const float dist = sqrtf(diff.transpose() * mat * diff);

        if (dist < dist_max) {
          dist_max = dist;
          idx_max = k;
        }
      }

#ifdef DEBUG_MODE_REF_CURVEDEFORM
      std::cout << idx_max << "\n";
#endif

      if (idx_max < 0)
      {
        continue;
      }

      const bool found = std::find(_idxs.begin(), _idxs.end(), idx_max) != _idxs.end();
      if (!found)
      {
        _idxs.push_back(idx_max);
        _target_pos.push_back(p);
        _src_pos.push_back(mesh.m_vVerts[idx_max]);
      }
    }
  }
}


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




void ModeRefCurveDeform::SaveState(const std::string& _fpath, const std::set<int>& _set_frame_idx)
{
  std::ofstream file(_fpath);
  if (!file)
  {
    std::cout << "Failed to save state.\n";
    return;
  }
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  for (int i = 0; i < num_frames; ++i)
  {
    if (!_set_frame_idx.count(i)) continue;

    file << "Frame:\n";
    file << std::to_string(i) + "\n\n";
    file << "Strokes:\n";
    file << m_strokes[i].OutputAsText();
    file << "EndStrokes\n\n";
    file << "EndFrame\n\n\n";
  }
  file.close();

  std::cout << "Saved as \"" + _fpath + "\"\n";
  m_is_not_saved_state = false;
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::LoadState(const std::string& _fpath, const std::set<int>& _set_frame_idx)
{
  enum class E_State
  {
    Init,
    Frame_init,
    Frame,
    Stroke_init,
    Stroke,
  };

  std::ifstream file(_fpath);
  if (!file)
  {
    std::cout << "Failed to load state.\n";
    return;
  }

  int frame_idx = -1;
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  E_State state = E_State::Init;
  std::vector<std::vector<EVec3f>> vec_strokes;
  std::vector<EVec3f> stroke;
  m_shared_stroke_idxs = std::set<int>();
  std::vector<int> vec_shared_idxs;
  int shared_idx;

  while (!file.eof())
  {
    char line_char[200];
    file.getline(line_char, 200);
    std::string line = std::string(line_char);

    if (state == E_State::Init)
    {
      if (line == "Frame:")
      {
        state = E_State::Frame_init;
      }
    }
    else if (state == E_State::Frame_init)
    {
      int x;
      if (sscanf_s(line_char, "%d", &x) == 1)
      {
        if ((x < 0) || (x >= num_frames))
        {
          return;
        }
        else if (_set_frame_idx.count(x))
        {
          frame_idx = x;
          state = E_State::Frame;
        }
        else
        {
          frame_idx = -1;
          state = E_State::Init;
        }
      }
    }
    else if (state == E_State::Frame)
    {
      if (line == "Strokes:")
      {
        vec_shared_idxs = std::vector<int>();
        vec_strokes = std::vector<std::vector<EVec3f>>();
        state = E_State::Stroke_init;
      }
      else if (line == "EndFrame")
      {
        std::cout << "Loaded frame: " + std::to_string(frame_idx) + ".\n";
        state = E_State::Init;
      }
    }
    else if ((state == E_State::Stroke_init) || (state == E_State::Stroke))
    {
      if (state == E_State::Stroke_init)
      {
        stroke = std::vector<EVec3f>();
        state = E_State::Stroke;
        int x;
        if (sscanf_s(line_char, "%d", &x) == 1)
        {
          shared_idx = x;
          continue;
        }
        else
        {
          shared_idx = -1;
        }
      }

      float x, y, z;
      if (sscanf_s(line_char, "%f %f %f", &x, &y, &z) == 3)
      {
        EVec3f vec;
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
        stroke.push_back(vec);
      }
      else if (line == "EndStrokes")
      {
        m_strokes[frame_idx].LoadState(vec_shared_idxs, vec_strokes);
        state = E_State::Frame;
      }
      else
      {
        if ((shared_idx >= 0) && (m_shared_stroke_idxs.count(shared_idx) == 0))
        {
          m_shared_stroke_idxs.insert(shared_idx);
        }
        vec_shared_idxs.push_back(shared_idx);
        vec_strokes.push_back(stroke);
        state = E_State::Stroke_init;
      }
    }
  }

  file.close();
  UpdateSharedStroke();
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::ShareSelectedStroke()
{
  const int frame_idx = formVisParam_getframeI();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  DeformationStrokes& dstrokes = m_strokes[frame_idx];

  if (dstrokes.GetSelectedStrokeIdx() == -1) return;

  int shared_idx;
  const int MAX_IDX = 1000;
  for (shared_idx = 0; shared_idx < MAX_IDX; ++shared_idx)
  {
    if (m_shared_stroke_idxs.count(shared_idx) == 0)
    {
      break;
    }
  }
  if (shared_idx == MAX_IDX)
  {
    std::cout << "Could not share.\n";
    return;
  }
  m_shared_stroke_idxs.insert(shared_idx);
  DeformationStrokes::Stroke* stroke = dstrokes.ShareSelectedStroke(shared_idx);
  for (int i = 0; i < num_frames; ++i)
  {
    m_strokes[i].UpdateSharedStrokes(shared_idx, stroke, nullptr);
  }
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeRefCurveDeform::UnshareSelectedStroke()
{
  const int frame_idx = formVisParam_getframeI();
  DeformationStrokes& dstrokes = m_strokes[frame_idx];
  const int shared_idx = dstrokes.UnshareSelectedStroke();
  if (m_shared_stroke_idxs.count(shared_idx) == 0)
  {
    return;
  }
  for (auto& stroke : m_strokes)
  {
    stroke.UpdateSharedStrokes(shared_idx, nullptr, nullptr);
  }
  m_shared_stroke_idxs.erase(shared_idx);
  formMain_RedrawMainPanel();
}


void ModeRefCurveDeform::LockSelectedStroke()
{
  const int frame_idx = formVisParam_getframeI();
  DeformationStrokes& dstrokes = m_strokes[frame_idx];
  dstrokes.LockSelectedStroke();
  UpdateSharedStroke();
  formMain_RedrawMainPanel();
}


void ModeRefCurveDeform::UnlockSelectedStroke()
{
  const int frame_idx = formVisParam_getframeI();
  DeformationStrokes& dstrokes = m_strokes[frame_idx];
  dstrokes.UnlockSelectedStroke();
  UpdateSharedStroke();
  formMain_RedrawMainPanel();
}


void ModeRefCurveDeform::UpdateSharedStroke()
{
  const int frame_idx = formVisParam_getframeI();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  for (const auto& shared_idx : m_shared_stroke_idxs)
  {
    for (int frame_i = 0; frame_i < num_frames; ++frame_i)
    {
      DeformationStrokes& dstrokes = m_strokes[frame_i];

      // get stroke from previous frame
      DeformationStrokes::Stroke* stroke_left;
      int framei_left;
      for (framei_left = frame_i; framei_left >= 0; --framei_left)
      {
        stroke_left = m_strokes[framei_left].GetSharedStroke(shared_idx);
        if (stroke_left != nullptr)
        {
          break;
        }
      }

      // get stroke from following frame
      DeformationStrokes::Stroke* stroke_right;
      int framei_right;
      for (framei_right = frame_i + 1; framei_right < num_frames; ++framei_right)
      {
        stroke_right = m_strokes[framei_right].GetSharedStroke(shared_idx);
        if (stroke_right != nullptr)
        {
          break;
        }
      }

      // update
      float rate = 0.0f;
      if ((stroke_left != nullptr) && (stroke_right != nullptr))
      {
        rate = (float)(frame_i - framei_left) / (framei_right - framei_left);
      }
      dstrokes.UpdateSharedStrokes(shared_idx, stroke_left, stroke_right, rate);
    }
  }
  formMain_RedrawMainPanel();
}
