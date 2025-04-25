#pragma unmanaged

#include "ModeSegStrokeFfd.h"
#include "ModeCommonTools.h"

#include "ImageCore.h"        // 画像データを保持しているクラス 　
#include "CrsSecCore.h"       // 切断面描画や、切断面への操作を管理
#include "deform_by_stroke.h" // 曲線制約による変形
#include <fstream>

#pragma managed
#include "CliMessageBox.h"
#include "FormMain.h"     // Main Window
#include "FormVisParam.h" // 右上のダイアログ
#include "FormSegStrokeFfd.h"
#pragma unmanaged

using namespace RoiPainter4D;


ModeSegStrokeFfd::~ModeSegStrokeFfd() {}

ModeSegStrokeFfd::ModeSegStrokeFfd()
{
  std::cout << "ModeSegStrokeFfd...\n";
  m_bL = m_bR = m_bM = false;
  std::cout << "ModeSegStrokeFfd done\n";
}



bool ModeSegStrokeFfd::CanEndMode()
{
  if (m_is_not_saved_state)
  {
    return ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Leaving?");
  }
  return true;
}



void ModeSegStrokeFfd::StartMode()
{
  std::cout << "ModeSegStrokeFfd...startMode----------\n";
  //flgs   
  m_bL = m_bR = m_bM = false;

  m_meshseq.Clear();

  auto c = ImageCore::GetInst()->GetCuboidF();
  m_cp_size = 10 * c[0] * 0.0006f;
  m_handle_len = c[0] * 0.03f * 3;
  m_handle_wid = c[0] * 0.03f * 3 * 0.04f;

  m_prev_frame_idx = formVisParam_getframeI();
  m_strokes = std::vector<DeformationStrokes>(ImageCore::GetInst()->GetNumFrames());
  m_shared_stroke_idxs = std::set<int>();
  m_histories = std::vector<History>(ImageCore::GetInst()->GetNumFrames());
  m_prev_selected_stroke_idx = -1;
  m_draghandle_id = OHDL_NON;
  m_b_draw_selectionrect = false;
  m_is_not_saved_state = false;
  m_debug = false;


  FormSegStrokeFfd_InitAllItems();
  FormSegStrokeFfd_Show();
  std::cout << "ModeSegStrokeFfd...startMode DONE-----\n";
  std::cout << "Debug mode is available with Shift+Ctrl+F12\n";
}


void ModeSegStrokeFfd::LBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = true;
  const int frame_idx  = formVisParam_getframeI();
  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();
  const bool vis_activestroke_only = FormSegStrokeFfd_GetShowOnlySelectedStroke();

  if (mode == SFFD_STROKE)
  {
    if (IsShiftKeyOn())
    {
      Do();
      EVec3f ray_pos, ray_dir, pos;
      ogl->GetCursorRay(p, ray_pos, ray_dir);
      const int selected_stroke_idx = m_strokes[frame_idx].PickControlPoint(ray_pos, ray_dir, m_cp_size, false, vis_activestroke_only);

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
        m_strokes[frame_idx].PickControlPoint(ray_pos, ray_dir, m_cp_size, true, vis_activestroke_only);
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
    else if (IsCtrKeyOn() && m_meshseq.IsInitialized())
    {
      EVec3f ray_p, ray_d;
      std::tie(ray_p, ray_d) = ogl->GetCursorRay1(p);

      const EVec3f center = m_meshseq.GetSelectedVtxCentroid(frame_idx);
      int trs = FormSegStrokeFfd_GetModeTransRotScale();
      m_draghandle_id = PickHandle(center, m_handle_len, m_handle_wid, ray_p, ray_d, trs);
    }
    else
    {
      m_strokes[frame_idx].ReleaseStroke();
      m_prev_selected_stroke_idx = -1;
      ogl->BtnDown_Trans(p);
    }
  }
  else if (mode == SFFD_CAGE)
  {
    if (IsShiftKeyOn() && m_meshseq.IsInitialized() && !m_bR)
    {
      EVec3f ray_p, ray_d;
      std::tie(ray_p, ray_d) = ogl->GetCursorRay1(p);

      //pick to add
      m_meshseq.SelectCageVtxByPick(frame_idx, ray_p, ray_d, 1);

      //start drawing rect
      m_b_draw_selectionrect = true;
      for (int i = 0; i < 4; ++i)
      {
        m_selectrect[i] = EVec3f(0, 0, 0);
      }
    }
    else if (IsCtrKeyOn() && m_meshseq.IsInitialized())
    {
      EVec3f ray_p, ray_d;
      std::tie(ray_p, ray_d) = ogl->GetCursorRay1(p);
      if (m_meshseq.GetNumSelectedVtx() > 0)
      {
        Do();
        const EVec3f center = m_meshseq.GetSelectedVtxCentroid(frame_idx);
        int trs = FormSegStrokeFfd_GetModeTransRotScale();
        m_draghandle_id = PickHandle(center, m_handle_len, m_handle_wid, ray_p, ray_d, trs);
      }
    }
    else
    {
      ogl->BtnDown_Trans(p);
    }
  }

  m_prevpt = m_initpt = p;
  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_draghandle_id = OHDL_NON;
  m_bL = false;
  const int frame_idx = formVisParam_getframeI();
  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();

  if (mode == SFFD_STROKE)
  {
    if (m_strokes[frame_idx].IsSelectedStrokeShared())
    {
      UpdateSharedStroke();
    }
  }
  else if (mode == SFFD_CAGE)
  {
    if (m_b_draw_selectionrect && m_meshseq.IsInitialized())
    {
      m_meshseq.SelectCageVtxByRect(frame_idx, m_initpt, p, ogl, 1);
    }

    m_draghandle_id = OHDL_NON;
    m_b_draw_selectionrect = false;
  }

  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::RBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = true;
  const int frame_idx = formVisParam_getframeI();
  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();
  const bool vis_activestroke_only = FormSegStrokeFfd_GetShowOnlySelectedStroke();

  if (mode == SFFD_STROKE)
  {
    if (IsShiftKeyOn())
    {
      EVec3f ray_pos, ray_dir, pos;
      ogl->GetCursorRay(p, ray_pos, ray_dir);
      const int selected_stroke_idx = m_strokes[frame_idx].PickControlPoint(ray_pos, ray_dir, m_cp_size, true, vis_activestroke_only);

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
  }
  else if (mode == SFFD_CAGE)
  {
    if (IsShiftKeyOn() && m_meshseq.IsInitialized() && !m_bL)
    {
      EVec3f ray_p, ray_d;
      std::tie(ray_p, ray_d) = ogl->GetCursorRay1(p);

      //pick to add
      m_meshseq.SelectCageVtxByPick(frame_idx, ray_p, ray_d, 0);

      //start drawing rect
      m_b_draw_selectionrect = true;
      for (int i = 0; i < 4; ++i)
      {
        m_selectrect[i] = EVec3f(0, 0, 0);
      }
    }
    else
    {
      ogl->BtnDown_Rot(p);
    }
  }

  m_prevpt = m_initpt = p;
  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::RBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = false;
  ogl->BtnUp();
  const int frame_idx = formVisParam_getframeI();
  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();

  if (mode == SFFD_CAGE)
  {
    if (m_b_draw_selectionrect && m_meshseq.IsInitialized())
    {
      m_meshseq.SelectCageVtxByRect(frame_idx, m_initpt, p, ogl, 0);
    }

    m_draghandle_id = OHDL_NON;
    m_b_draw_selectionrect = false;
  }

  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::MBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();

  if (mode == SFFD_STROKE)
  {
    const int frame_idx = formVisParam_getframeI();
    m_strokes[frame_idx].ReleaseStroke();
    m_prev_selected_stroke_idx = -1;
  }

  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::MBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::MouseMove(const EVec2i& p, OglForCLI* ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  const int frame_idx  = formVisParam_getframeI();
  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();
  const int mode_trs = FormSegStrokeFfd_GetModeTransRotScale();

  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);
  
  if (mode == SFFD_STROKE)
  {
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

    // deform
    if (m_draghandle_id != OHDL_NON)
    {
      if (mode_trs == 0)
        m_meshseq.TranslateSelectedVerts(frame_idx, m_prevpt, p, m_draghandle_id, ogl);
      else if (mode_trs == 1)
        m_meshseq.RotateSelectedVerts   (frame_idx, m_prevpt, p, m_draghandle_id, m_handle_len, ogl);
      else if (mode_trs == 2)
        m_meshseq.ScaleSelectedVerts    (frame_idx, m_prevpt, p, ogl);
    }
  }
  else if (mode == SFFD_CAGE)
  {
    if (m_draghandle_id != OHDL_NON)
    {
      if (mode_trs == 0)
      {
        m_meshseq.TranslateSelectedVerts(frame_idx, m_prevpt, p, m_draghandle_id, ogl);
      }
      else if (mode_trs == 1)
      {
        m_meshseq.RotateSelectedVerts(frame_idx, m_prevpt, p, m_draghandle_id, m_handle_len, ogl);
      }
      else if (mode_trs == 2)
      {
        m_meshseq.ScaleSelectedVerts(frame_idx, m_prevpt, p, ogl);
      }
    }
    else if (m_b_draw_selectionrect)
    {
      //draw selecting rect
      EVec2i rectpt[4] = {
        EVec2i(std::min(p[0],m_initpt[0]), std::min(p[1],m_initpt[1])),
        EVec2i(std::min(p[0],m_initpt[0]), std::max(p[1],m_initpt[1])),
        EVec2i(std::max(p[0],m_initpt[0]), std::max(p[1],m_initpt[1])),
        EVec2i(std::max(p[0],m_initpt[0]), std::min(p[1],m_initpt[1])) };

      for (int i = 0; i < 4; ++i)
      {
        auto ray = ogl->GetCursorRay1(rectpt[i]);
        m_selectrect[i] = std::get<0>(ray) + 0.1f * std::get<1>(ray);
      }
    }
    else
    {
      ogl->MouseMove(p);
    }
  }

  m_prevpt = p;
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


void ModeSegStrokeFfd::LBtnDclk(const EVec2i& p, OglForCLI* ogl)
{
  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();

  if (mode == SFFD_CAGE && m_meshseq.IsInitialized())
  {
    const int n = m_meshseq.GetNumCageVertex();
    std::set<int> ids;
    for (int i = 0; i < n; ++i) ids.insert(i);
    m_meshseq.SetCageVtxSelected(ids);
  }
}


void ModeSegStrokeFfd::RBtnDclk(const EVec2i& p, OglForCLI* ogl)
{
  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();

  if ( mode == SFFD_STROKE)
  {
    const int frame_idx = formVisParam_getframeI();
    if (IsShiftKeyOn())
    {
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
  }
  else if (mode == SFFD_CAGE && m_meshseq.IsInitialized())
  {
    m_meshseq.ClearSelectedVtx();
  }
}


void ModeSegStrokeFfd::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}


void ModeSegStrokeFfd::KeyDown(int nChar)
{
  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();

  if ((mode == SFFD_STROKE) && IsCtrKeyOn() && m_meshseq.IsInitialized())
  {
    const int n = m_meshseq.GetNumCageVertex();
    std::set<int> ids;
    for (int i = 0; i < n; ++i)
    {
      ids.insert(i);
    }
    m_meshseq.SetCageVtxSelected(ids);
  }

  // for debug (Shift+Ctrl+F12)
  if ((nChar == VK_F12) && (m_debug || (IsShiftKeyOn() && IsCtrKeyOn())))
  {
    m_debug = !m_debug;
    std::cout << "\ndebug mode: " << (m_debug ? "ON" : "OFF") << std::endl;
  }
  if (m_debug)
  {
    if (nChar == VK_F1)
    {
      FormSegStrokeFfd_SwitchUiModeNext();
    }
    else if (nChar == VK_F2)
    {
      FormSegStrokeFfd_SwithDeformMode();
    }
    else if (nChar == VK_F3)
    {
      Undo();
    }
    else if (nChar == VK_F4)
    {
      Redo();
    }
    else if (nChar == VK_F5)
    {
      Deform();
    }
    else if (nChar == VK_F6)
    {
      Do();
      CopyFromPrevFrame();
    }
    else if (nChar == VK_F7)
    {
      Do();
      const int frame_idx = formVisParam_getframeI();
      m_strokes[frame_idx].ClearAllStrokes();
    }
    else if (nChar == VK_F8)
    {
      ShareSelectedStroke();
    }
    else if (nChar == VK_F9)
    {
      UnshareSelectedStroke();
    }
    else if (nChar == VK_F10)
    {
      LockSelectedStroke();
    }
    else if (nChar == VK_F11)
    {
      UnlockSelectedStroke();
    }
    else if (nChar == VK_F12)
    {
      std::cout
        << "[F1]  Change deform mode\n"
        << "[F2]  Switch between stroke mode and cage mode\n"
        << "[F3]  UNDO\n"
        << "[F4]  REDO\n"
        << "[F5]  Deform by strokes\n"
        << "[F6]  Copy strokes from previous frame\n"
        << "[F7]  Clear all strokes in this frame\n"
        << "[F8]  Share selected stroke\n"
        << "[F9]  Unshare selected stroke\n"
        << "[F10] Lock selected stroke\n"
        << "[F11] Unlock selected stroke\n"
        << "[F12] Off debug mode\n"
        << std::endl;
    }
  }
}


void ModeSegStrokeFfd::KeyUp(int nChar) {}


void ModeSegStrokeFfd::DrawScene(
  const EVec3f& cam_pos,
  const EVec3f& cam_cnt)
{
  const EVec3f cuboid = ImageCore::GetCuboid();
  const int frame_idx = formVisParam_getframeI();
  const int mode_trs  = FormSegStrokeFfd_GetModeTransRotScale();
  const bool vis_activestroke_only = FormSegStrokeFfd_GetShowOnlySelectedStroke();

  // bind volumes 
  ImageCore::GetInst()->BindAllVolumes();
  DrawCrossSectionsNormal();

  if (formVisParam_bRendVol())
  {
    DrawVolumeVisMask(!IsShiftKeyOn(), cam_pos, cam_cnt);
  }

  // draw cage & mesh 
  const bool draw_bound = 1;
  //const bool draw_surf_trans = 1;

  glDisable(GL_CULL_FACE);
  glDepthMask(false);
  glEnable(GL_BLEND);

  const bool b_xy = formVisParam_bPlaneXY();
  const bool b_yz = formVisParam_bPlaneYZ();
  const bool b_zx = formVisParam_bPlaneZX();
  float planexy = b_xy ? CrssecCore::GetInst()->GetPlanePosXY() : -1;
  float planeyz = b_yz ? CrssecCore::GetInst()->GetPlanePosYZ() : -1;
  float planezx = b_zx ? CrssecCore::GetInst()->GetPlanePosZX() : -1;
  if (!draw_bound)
  {
    planexy = planeyz = planezx = -1;
  }

  if (!IsSpaceKeyOn())
  {
    //float opacity = m_draw_surf_trans ? (float)0.4 : (float)0;
    float opacity = IsSKeyOn() ? (float)1 : (float)0.4;
    const auto &m = m_meshseq.GetMesh(frame_idx);
    DrawMeshWithCrossecHL(m, planeyz, planezx, planexy, opacity, cuboid);
  }

  glDepthMask(true);
  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);

  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();

  if (mode == SFFD_STROKE)
  {
    // draw cage
    if (IsCKeyOn())
    {
      const auto& c = m_meshseq.GetCage(frame_idx);
      DrawCageWithCPs(c, true, m_cp_size, m_meshseq.GetSelectedCageVtxVec());
    }

    // draw stroke
    if (!IsSKeyOn())
    {
      m_strokes[frame_idx].DrawStrokes(vis_activestroke_only);
    }

    if (IsShiftKeyOn())
    {
      // draw control points
      m_strokes[frame_idx].DrawControlPoints(m_cp_size, vis_activestroke_only);
    }
    else if (IsCtrKeyOn() && m_meshseq.GetNumSelectedVtx() > 0)
    {
      // draw handle
      const EVec3f center = m_meshseq.GetSelectedVtxCentroid(frame_idx);
      DrawHandle(center, m_handle_len, m_handle_wid, mode_trs);
    }
  }
  else if (mode == SFFD_CAGE)
  {

    if (IsShiftKeyOn())
    {
      const auto& c = m_meshseq.GetCage(frame_idx);
      DrawCageWithCPs(c, true, m_cp_size, m_meshseq.GetSelectedCageVtxVec());
    }
    else if (IsCtrKeyOn() && m_meshseq.GetNumSelectedVtx() > 0)
    {
      const auto& c = m_meshseq.GetCage(frame_idx);
      DrawCageWithCPs(c, true, m_cp_size, m_meshseq.GetSelectedCageVtxVec());

      // draw handle
      const EVec3f center = m_meshseq.GetSelectedVtxCentroid(frame_idx);
      DrawHandle(center, m_handle_len, m_handle_wid, mode_trs);
    }

    // draw selection rect
    if (m_b_draw_selectionrect)
    {
      DrawPolyLine(EVec3f(1, 1, 0), 3, 4, m_selectrect, true);
    }
  }
}


void ModeSegStrokeFfd::Deform()
{
  float alpha;
  float beta;
  float gamma;
  try
  {
    alpha = std::stof(FormSegStrokeFfd_GetAlpha());
    beta = std::stof(FormSegStrokeFfd_GetBeta());
    gamma = std::stof(FormSegStrokeFfd_GetGamma());
  }
  catch (...)
  {
    std::cout << "0以上の実数値を入力してください.\n";
    return;
  }

  const int frame_idx = formVisParam_getframeI();
  if (m_meshseq.IsInitialized())
  {
    std::cout << "Deforming...\n";
    Do();
    for (int i = 0; i < 20; ++i)
    {
      DeformByStroke::Deform(m_meshseq, m_strokes[frame_idx].GetStroke(), frame_idx, alpha, beta, gamma);
      formMain_RedrawMainPanel();
    }
    std::cout << "Deformed.\n";
  }
  formMain_ActivateMainForm();
}


void ModeSegStrokeFfd::LoadMeshAndCage(const std::string& _fpath_mesh, const std::string& _fpath_cage)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();
  m_meshseq.Clear();
  m_meshseq.Initialize(num_frames, cuboid, _fpath_mesh, _fpath_cage);
  std::cout << "Loaded mesh and cage.\n";
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeSegStrokeFfd::SaveMeshAndCage(const std::string& _fpath_mesh, const std::string& _fpath_cage)
{
  m_meshseq.ExportCageMeshSequenceAsObj(_fpath_mesh, false);
  m_meshseq.ExportCageMeshSequenceAsObj(_fpath_cage, true);
  std::cout << "Saved mesh and cage.\n";
  formMain_ActivateMainForm();
}


void ModeSegStrokeFfd::SaveState(const std::string& _fpath, const std::set<int>& _set_frame_idx)
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
    file << "Cage:\n";
    std::vector<EVec3f>& cage_vertices = m_meshseq.GetCageVertices(i);
    for (const auto& vec : cage_vertices)
    {
      for (int i = 0; i < 3; ++i)
      {
        file << std::to_string(vec[i]) + " ";
      }
      file << "\n";
    }
    file << "EndCage\n";
    file << "EndFrame\n\n\n";
  }
  file.close();

  std::cout << "Saved as \"" + _fpath + "\"\n";
  m_is_not_saved_state = false;
  formMain_ActivateMainForm();
}


void ModeSegStrokeFfd::LoadState(const std::string& _fpath, const std::set<int>& _set_frame_idx)
{
  enum class E_State
  {
    Init,
    Frame_init,
    Frame,
    Stroke_init,
    Stroke,
    Cage,
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
  std::vector<EVec3f> cage_vertices;
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
      else if (line == "Cage:")
      {
        cage_vertices = std::vector<EVec3f>();
        state = E_State::Cage;
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
    else if (state == E_State::Cage)
    {
      float x, y, z;
      if (sscanf_s(line_char, "%f %f %f", &x, &y, &z) == 3)
      {
        EVec3f vec;
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
        cage_vertices.push_back(vec);
      }
      else if (line == "EndCage")
      {
        m_meshseq.SetCageVertices(frame_idx, cage_vertices, true);
        state = E_State::Frame;
      }
    }
  }

  file.close();
  UpdateSharedStroke();
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeSegStrokeFfd::Do()
{
  const int frame_idx = formVisParam_getframeI();
  Action now = { m_strokes[frame_idx], m_meshseq.GetCageVertices(frame_idx) };
  History& history = m_histories[frame_idx];
  if (history.undo.empty() || (now.strokes != history.undo.top().strokes) || (now.verts != history.undo.top().verts))
  {
    history.undo.push(now);
    history.redo = std::stack<Action>();
  }
  formMain_RedrawMainPanel();
  m_is_not_saved_state = true;
}


void ModeSegStrokeFfd::Undo()
{
  const int frame_idx = formVisParam_getframeI();
  Action now = { m_strokes[frame_idx], m_meshseq.GetCageVertices(frame_idx) };
  History& history = m_histories[frame_idx];
  if (!history.undo.empty())
  {
    const Action prev = history.undo.top();
    history.undo.pop();
    history.redo.push(now);
    m_strokes[frame_idx] = prev.strokes;
    m_meshseq.SetCageVertices(frame_idx, prev.verts);
    std::cout << "UNDO\n";
  }
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeSegStrokeFfd::Redo()
{
  const int frame_idx = formVisParam_getframeI();
  Action now = { m_strokes[frame_idx], m_meshseq.GetCageVertices(frame_idx) };
  History& history = m_histories[frame_idx];
  if (!history.redo.empty())
  {
    const Action next = history.redo.top();
    history.redo.pop();
    history.undo.push(now);
    m_strokes[frame_idx] = next.strokes;
    m_meshseq.SetCageVertices(frame_idx, next.verts);
    std::cout << "REDO\n";
  }
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeSegStrokeFfd::CopyFromPrevFrame()
{
  const int frame_idx = formVisParam_getframeI();
  if (frame_idx <= 0) return;
  m_strokes[frame_idx] = m_strokes[frame_idx - 1];
  m_strokes[frame_idx].UnlockAllStrokes();
  UpdateSharedStroke();
  std::vector<EVec3f>& prev_verts = m_meshseq.GetCageVertices(frame_idx - 1);
  m_meshseq.SetCageVertices(frame_idx, prev_verts, true);
  std::cout << "Copy from previous frame.\n";
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeSegStrokeFfd::CopyCageToAllFrames()
{
  const int frame_idx = formVisParam_getframeI();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  if (frame_idx < 0) return;
  std::vector<EVec3f>& verts = m_meshseq.GetCageVertices(frame_idx);
  for (int i = 0; i < num_frames; ++i)
  {
    if (i == frame_idx) continue;
    m_meshseq.SetCageVertices(i, verts, true);
  }
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeSegStrokeFfd::ShareSelectedStroke()
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


void ModeSegStrokeFfd::UnshareSelectedStroke()
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


void ModeSegStrokeFfd::LockSelectedStroke()
{
  const int frame_idx = formVisParam_getframeI();
  DeformationStrokes& dstrokes = m_strokes[frame_idx];
  dstrokes.LockSelectedStroke();
  UpdateSharedStroke();
  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::UnlockSelectedStroke()
{
  const int frame_idx = formVisParam_getframeI();
  DeformationStrokes& dstrokes = m_strokes[frame_idx];
  dstrokes.UnlockSelectedStroke();
  UpdateSharedStroke();
  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::UpdateSharedStroke()
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



void ModeSegStrokeFfd::SetCPSize(int size)
{
  const int cp_size = size;

  if (cp_size <= 0)
  {
    std::cout << "strange input\n";
    return;
  }
  auto c = ImageCore::GetInst()->GetCuboidF();
  m_cp_size = cp_size * c[0] * 0.0006f;

  std::cout << "Change CP size: " << m_cp_size << std::endl;
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}



void ModeSegStrokeFfd::ClearSelectedStrokes()
{
  try
  {
    m_strokes[m_prev_frame_idx].ReleaseStroke();
    m_prev_frame_idx = formVisParam_getframeI();
    //for (auto& dstroke : m_strokes)
    //{
    //  dstroke.ReleaseStroke();
    //}
  }
  catch (...)
  {

  }
}
