#pragma unmanaged

#include "ModeSegStrokeFfd.h"
#include "ModeCommonTools.h"

#include "ImageCore.h"        
#include "CrsSecCore.h"       
#include "ModeCore.h"       
#include "deform_by_stroke.h" 
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


static bool b_in_FinishAndStore = false;


bool ModeSegStrokeFfd::CanEndMode()
{
  if( b_in_FinishAndStore) return true;
  return ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Leaving?");
}



void ModeSegStrokeFfd::StartMode()
{
  std::cout << "ModeSegStrokeFfd...startMode----------\n";

  const int    num_frame = ImageCore::GetInst()->GetNumFrames();
  const EVec3f cube      = ImageCore::GetInst()->GetCuboidF();

  m_bL = m_bR = m_bM = false;
  m_b_draw_selectionrect = false;
  m_draghandle_id = OHDL_NON;

  m_cp_rate    = cube[0] * 0.0006f;
  m_handle_len = cube[0] * 0.06f * 3;
  m_handle_wid = cube[0] * 0.06f * 3 * 0.04f;

  m_meshseq.Clear();
  m_shared_curves.clear();
  m_curves = std::vector<std::vector<PlanarCurve>>(num_frame);
  m_select_info.Clear();

  m_history = std::stack<SnapShot>();


  ImageCore::GetInst()->InitializeFlg4dByMask(formMain_SetProgressValue);

  FormSegStrokeFfd_InitAllItems();
  FormSegStrokeFfd_Show();
  std::cout << "ModeSegStrokeFfd...startMode DONE-----\n";
}



void ModeSegStrokeFfd::FinishSegmentation()
{
  b_in_FinishAndStore = true;
  const int     num_voxels  = ImageCore::GetInst()->GetNumVoxels();
  const int     num_frames  = ImageCore::GetInst()->GetNumFrames();
  const EVec3i  reso   = ImageCore::GetInst()->GetReso();
  const EVec3f  pitch  = ImageCore::GetInst()->GetPitch();

  byte* flgInOut = new byte[num_voxels];

  int count = 0;
  for (int f = 0; f < num_frames; ++f)
  {
    m_meshseq.GetMesh(f).GenBinaryVolume(reso, pitch, flgInOut);

    byte* flg3d = ImageCore::GetInst()->m_flg4d[f];
#pragma omp parallel for
    for (int i = 0; i < num_voxels; ++i)
    {
      flg3d[i] = (flg3d[i]    == 0) ?   0 :
                 (flgInOut[i] == 1) ? 255 : 1;
    }

    formMain_SetProgressValue((float)f / num_frames);
    std::cout << "fillInMesh " << f << "\n";
  }
  delete[] flgInOut;

  formMain_SetProgressValue(0);

  if (!bForeVoxelExist_flg4())
  {
    ShowMsgDlg_OK(MESSAGE_NO_FOREGROUND, "no foreground");
    return;
  }

  ImageCore::GetInst()->mask_storeCurrentForeGround();
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  formMain_RedrawMainPanel();
  b_in_FinishAndStore = false;
}





PlanarCurveSelectionInfo ModeSegStrokeFfd::PickCpAtCurrentFrame(
  const EVec3f& ray_pos,
  const EVec3f& ray_dir)
{
  const int frame_idx = formVisParam_getframeI();
  const float CP_RAD = m_cp_rate * FormSegStrokeFfd_CpSize();
  const bool  ONLY_SELECTED = FormSegStrokeFfd_GetShowOnlySelectedStroke();

  PlanarCurveSelectionInfo info;
  int cp_idx;
  EVec3f cp_pos;

  // pick standard curves
  for (int i = 0; i < (int)m_curves[frame_idx].size(); ++i)
  {
    const PlanarCurve& c = m_curves[frame_idx][i];

    if (ONLY_SELECTED && !m_select_info.IsStdCurveSelect(i)) continue;
    if (!c.PickCPs(ray_pos, ray_dir, CP_RAD, cp_idx, cp_pos)) continue;

    if (!info.selected || Dist(ray_pos, cp_pos) < Dist(ray_pos, info.pos))
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






void ModeSegStrokeFfd::LBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = true;
  const int frame_idx  = formVisParam_getframeI();
  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();
  const EVec3f    cube = ImageCore::GetInst()->GetCuboidF();
  const bool vis_selectstr_only = FormSegStrokeFfd_GetShowOnlySelectedStroke();

  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if (mode == SFFD_STROKE)
  {
    if (IsShiftKeyOn())
    {
      Do_RecordSnapShot();
      auto info = PickCpAtCurrentFrame(ray_pos, ray_dir);
      CRSSEC_ID crssec_id = PickCrssec(ray_pos, ray_dir, pos);

      if (info.selected ) 
      {
        //picking成功 - このcurve/CPを選択状態に
        m_select_info = info;
        CrssecCore::GetInst()->SetPlanePos(info.crssec_id, info.crssec_pos, cube);
      }
      else if(crssec_id != CRSSEC_NON)
      {
        //picking失敗 & crosec pick成功 - 新curve追加 or 新CP追加
        if (!m_select_info.selected)
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
          if (m_curves[frame_idx][m_select_info.curve_idx].AddCP(pos, cpidx))
            m_select_info.cp_idx = cpidx;
        }
      }
      else
      {
        //clear selection
        m_select_info.Clear();
      } 
    }
    else
    {
      ogl->BtnDown_Trans(p);
    }
  }
  else if (mode == SFFD_CAGE)
  {
    if (IsShiftKeyOn() && m_meshseq.IsInitialized() && !m_bR)
    {
      //start Cage vertex selection
      m_meshseq.SelectCageVtxByPick(frame_idx, ray_pos, ray_dir, 1);
      m_b_draw_selectionrect = true;
      for (int i = 0; i < 4; ++i) m_selectrect[i] = EVec3f(0, 0, 0);
    }
    else if(IsCtrKeyOn() && m_meshseq.IsInitialized()&& m_meshseq.GetNumSelectedVtx() > 0)
    {
      Do_RecordSnapShot();
      const EVec3f center = m_meshseq.GetSelectedVtxCentroid(frame_idx);
      int trs = FormSegStrokeFfd_GetModeTransRotScale();
      m_draghandle_id = PickHandle(center, m_handle_len, m_handle_wid, ray_pos, ray_dir, trs);
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
  const int frame_idx = formVisParam_getframeI();
  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();

  if (mode == SFFD_STROKE && m_select_info.selected && m_select_info.is_shared)
  {
    int ci = m_select_info.curve_idx;
    if (0 <= ci && ci < (int)m_shared_curves.size())
      m_shared_curves[ci].UpdateNonManipCurves();
  }

  if (mode == SFFD_CAGE && m_b_draw_selectionrect && m_meshseq.IsInitialized())
  {
    m_meshseq.SelectCageVtxByRect(frame_idx, m_initpt, p, ogl, 1);
  }

  m_bL = false;
  m_b_draw_selectionrect = false;
  m_draghandle_id = OHDL_NON;
  ogl->BtnUp();

  formMain_RedrawMainPanel();
}



void ModeSegStrokeFfd::RBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = true;
  const int       frame_idx  = formVisParam_getframeI();
  const SFFD_MODE mode       = FormSegStrokeFfd_GetDeformMode();
  const bool vis_selstr_only = FormSegStrokeFfd_GetShowOnlySelectedStroke();
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if (mode == SFFD_STROKE && IsShiftKeyOn())
  {
    //pickしたCPの削除（sharedならshared解除、非sharedならcp削除）
    auto info = PickCpAtCurrentFrame(ray_pos, ray_dir);

    if (info.selected && info.is_shared)
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
      Do_RecordSnapShot();
      std::vector<PlanarCurve>& curves = m_curves[frame_idx];
      curves[info.curve_idx].DeleteCP(info.cp_idx);
      if (curves[info.curve_idx].GetNumCPs() == 0)
      {
        curves.erase(curves.begin() + info.curve_idx);
        m_select_info.Clear();
      }
      else
      {
        m_select_info = info;
        m_select_info.cp_idx = curves[info.curve_idx].GetNumCPs() - 1;
      }
    }
  }
  else if (mode == SFFD_STROKE && !IsShiftKeyOn())
  {
    m_select_info.Clear();
    ogl->BtnDown_Rot(p);
  }
  else if (mode == SFFD_CAGE)
  {
    if (IsShiftKeyOn() && m_meshseq.IsInitialized() && !m_bL)
    {
      //pick to remove selection
      m_meshseq.SelectCageVtxByPick(frame_idx, ray_pos, ray_dir, 0);
      m_b_draw_selectionrect = true;
      for (int i = 0; i < 4; ++i) m_selectrect[i] = EVec3f(0, 0, 0);
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
  const int frame_idx = formVisParam_getframeI();

  if (FormSegStrokeFfd_GetDeformMode() == SFFD_CAGE)
  {
    if (m_b_draw_selectionrect && m_meshseq.IsInitialized())
      m_meshseq.SelectCageVtxByRect(frame_idx, m_initpt, p, ogl, 0);
  }

  m_draghandle_id = OHDL_NON;
  m_b_draw_selectionrect = false;
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}



void ModeSegStrokeFfd::MBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
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

  const int  frame_idx = formVisParam_getframeI();
  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();
  
  if (mode == SFFD_STROKE && IsShiftKeyOn() && m_bL)
  {
    EVec3f ray_pos, ray_dir, pos;
    ogl->GetCursorRay(p, ray_pos, ray_dir);

    if (PickCrssec(ray_pos, ray_dir, pos) != CRSSEC_NON)
    {
      int curve_idx = m_select_info.curve_idx;
      int cp_idx = m_select_info.cp_idx;
      if (m_select_info.selected && m_select_info.is_shared)
      {
        m_shared_curves[curve_idx].MoveCP(frame_idx, cp_idx, pos);
      }
      else if (m_select_info.selected && !m_select_info.is_shared)
      {
        m_curves[frame_idx][curve_idx].MoveCP(cp_idx, pos);
      }
    }
  }
  else if (mode == SFFD_CAGE && m_b_draw_selectionrect)
  {
    //rect points
    int x = m_initpt[0], y = m_initpt[1];
    auto ray0 = ogl->GetCursorRay1(std::min(p[0], x), std::min(p[1], y));
    auto ray1 = ogl->GetCursorRay1(std::min(p[0], x), std::max(p[1], y));
    auto ray2 = ogl->GetCursorRay1(std::max(p[0], x), std::max(p[1], y));
    auto ray3 = ogl->GetCursorRay1(std::max(p[0], x), std::min(p[1], y));
    m_selectrect[0] = std::get<0>(ray0) + 0.1f * std::get<1>(ray0);
    m_selectrect[1] = std::get<0>(ray1) + 0.1f * std::get<1>(ray1);
    m_selectrect[2] = std::get<0>(ray2) + 0.1f * std::get<1>(ray2);
    m_selectrect[3] = std::get<0>(ray3) + 0.1f * std::get<1>(ray3);
  }
  else if (m_draghandle_id != OHDL_NON)
  {
    const int mode_trs = FormSegStrokeFfd_GetModeTransRotScale();

    if (mode_trs == 0)
      m_meshseq.TranslateSelectedVerts(frame_idx, m_prevpt, p, m_draghandle_id, ogl);
    else if (mode_trs == 1)
      m_meshseq.RotateSelectedVerts   (frame_idx, m_prevpt, p, m_draghandle_id, m_handle_len, ogl);
    else if (mode_trs == 2)
      m_meshseq.ScaleSelectedVerts    (frame_idx, m_prevpt, p, ogl);
  }
  else
  {
    ogl->MouseMove(p);
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

  if (mode == SFFD_CAGE && m_meshseq.IsInitialized())
  {
    m_meshseq.ClearSelectedVtx();
  }
}


void ModeSegStrokeFfd::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}


void ModeSegStrokeFfd::KeyUp(int nChar) {}


void ModeSegStrokeFfd::KeyDown(int nChar)
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
    Deform();
  }
  else if (nChar == VK_F4){}
  else if (nChar == VK_F5){}
  else if (nChar == VK_F6){}
  else if (nChar == VK_F7){}
  else if (nChar == VK_F8){}
  else if (nChar == VK_F9){}
  else if (nChar == VK_F10){}
  else if (nChar == VK_F11){}
  else if (nChar == VK_F12)
  {
    std::cout
      << "[F1]  Change deform mode\n"
      << "[F2]  Switch between stroke mode and cage mode\n"
      << "[F3]  Deform by strokes\n"
      << std::endl;
  }
}
// 木村くんオリジナルの実装 : Ctrキーを押したら全選択（なんで必要なんだろう？）
//const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();

//if (mode == SFFD_STROKE && IsCtrKeyOn() && m_meshseq.IsInitialized())
//{
//  const int n = m_meshseq.GetNumCageVertex();
//  std::set<int> ids;
//  for (int i = 0; i < n; ++i)
//  {
//    ids.insert(i);
//  }
//  m_meshseq.SetCageVtxSelected(ids);
//}





void ModeSegStrokeFfd::DrawScene(
  const EVec3f& cam_pos,
  const EVec3f& cam_cnt)
{
  const int frame_idx = formVisParam_getframeI();
  const EVec3f cuboid = ImageCore::GetCuboid();
  const float CP_RAD = m_cp_rate * FormSegStrokeFfd_CpSize();

  // bind volumes 
  ImageCore::GetInst()->BindAllVolumes();
  DrawCrossSectionsNormal();

  if (formVisParam_bRendVol())
  {
    DrawVolumeVisMask(!IsShiftKeyOn(), cam_pos, cam_cnt);
  }


  //Draw Cross Section (Highlight at mesh intersection)
  glDisable(GL_CULL_FACE);
  glDepthMask(false);
  glEnable(GL_BLEND);
  
  if (!IsSpaceKeyOn())
  {
    const bool b_xy = formVisParam_bPlaneXY();
    const bool b_yz = formVisParam_bPlaneYZ();
    const bool b_zx = formVisParam_bPlaneZX();
    float planexy = b_xy ? CrssecCore::GetInst()->GetPlanePosXY() : -1;
    float planeyz = b_yz ? CrssecCore::GetInst()->GetPlanePosYZ() : -1;
    float planezx = b_zx ? CrssecCore::GetInst()->GetPlanePosZX() : -1;

    float opacity = IsSKeyOn() ? (float)1 : (float)0.4;
    const auto &m = m_meshseq.GetMesh(frame_idx);
    DrawMeshWithCrossecHL(m, planeyz, planezx, planexy, opacity, cuboid);
  }

  glDepthMask(true);
  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);

  const SFFD_MODE mode = FormSegStrokeFfd_GetDeformMode();
  const int  mode_trs = FormSegStrokeFfd_GetModeTransRotScale();

  if (mode == SFFD_STROKE)
  {
    // draw cage
    if (IsCKeyOn())
    {
      const auto& c = m_meshseq.GetCage(frame_idx);
      DrawCageWithCPs(c, true, CP_RAD, m_meshseq.GetSelectedCageVtxVec());
    }

    const bool VIS_CP = IsShiftKeyOn();
    const bool VIS_ONLY_SEL = FormSegStrokeFfd_GetShowOnlySelectedStroke();
    DrawPlanerCurvesAndSharedCurves(
      frame_idx, m_select_info,
      VIS_ONLY_SEL, VIS_CP, CP_RAD,
      m_curves[frame_idx], m_shared_curves);
  
    if (IsCtrKeyOn() && m_meshseq.GetNumSelectedVtx() > 0)
    {
      // draw handle
      const EVec3f center = m_meshseq.GetSelectedVtxCentroid(frame_idx);
      DrawHandle(center, m_handle_len, m_handle_wid, mode_trs);
    }
  }
  else if (mode == SFFD_CAGE)
  {
    const auto&   cage = m_meshseq.GetCage(frame_idx);
    const EVec3f& cntr = m_meshseq.GetSelectedVtxCentroid(frame_idx);

    if (IsShiftKeyOn())
    {
      DrawCageWithCPs(cage, true, CP_RAD, m_meshseq.GetSelectedCageVtxVec());
    }
    else if (IsCtrKeyOn() && m_meshseq.GetNumSelectedVtx() > 0)
    {
      DrawCageWithCPs(cage, true, CP_RAD, m_meshseq.GetSelectedCageVtxVec());
      DrawHandle(cntr, m_handle_len, m_handle_wid, mode_trs);
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
  float alpha, beta, gamma;
  try
  {
    alpha = std::stof(FormSegStrokeFfd_GetAlpha());
    beta  = std::stof(FormSegStrokeFfd_GetBeta());
    gamma = std::stof(FormSegStrokeFfd_GetGamma());
  }
  catch (...)
  {
    std::cout << "αβγに0以上の実数値を入力してください.\n";
    return;
  }

  const int frame_idx = formVisParam_getframeI();
  if (m_meshseq.IsInitialized())
  {
    std::cout << "Deforming...\n";
    
    std::vector<std::vector<EVec3f>> curves;
    for (const auto& c : m_curves[frame_idx]) 
    {
      curves.push_back(c.GetCurve());
    }
    for (const auto& sc : m_shared_curves)
    {
      curves.push_back(sc.GetCurve(frame_idx).GetCurve());
    }


    Do_RecordSnapShot();
    for (int i = 0; i < 20; ++i)
    {
      DeformByStroke::Deform(m_meshseq, curves, frame_idx, alpha, beta, gamma);
      formMain_RedrawMainPanel();
    }
    std::cout << "Deformed.\n";
  }
  formMain_ActivateMainForm();
}


void ModeSegStrokeFfd::LoadMeshAndCage(const std::string& fpath_mesh, const std::string& fpath_cage)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const EVec3f cuboid  = ImageCore::GetInst()->GetCuboidF();
  m_meshseq.Clear();
  m_meshseq.Initialize(num_frames, cuboid, fpath_mesh, fpath_cage);
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeSegStrokeFfd::SaveMeshAndCage(const std::string& fpath_mesh, const std::string& fpath_cage)
{
  m_meshseq.ExportCageMeshSequenceAsObj(fpath_mesh, false);
  m_meshseq.ExportCageMeshSequenceAsObj(fpath_cage, true);
  std::cout << "Saved mesh and cage.\n";
  formMain_ActivateMainForm();
}



void ModeSegStrokeFfd::SaveState(const std::string& _fpath)
{
  std::ofstream file(_fpath);
  if (!file)
  {
    std::cout << "Failed to save state.\n";
    return;
  }

  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  file << "ModeSegStrokeFfd_State\n";
  file << "NumFrames: " << std::to_string(num_frames) + "\n";

  //normal curves 
  for (int fi = 0; fi < num_frames; ++fi)
  {
    file << "Frame: " << std::to_string(fi) + "\n";
    file << "num_curves: " << std::to_string(m_curves[fi].size()) << "\n";
    for (int j = 0; j < m_curves[fi].size(); ++j)
      m_curves[fi][j].WriteToFile(file);
  }

  //Shared curve
  file << "SharedCurves: " << std::to_string(m_shared_curves.size()) << "\n";
  for (size_t i = 0; i < m_shared_curves.size(); ++i)
  {
    m_shared_curves[i].WriteToFile(file);
  }

  //cage vertex
  file << "CageVerts: " << std::to_string(num_frames) + "\n";
  for (int fi = 0; fi < num_frames; ++fi)
  {
    std::vector<EVec3f>& verts = m_meshseq.GetCageVertices(fi);
    file << "Num_Verts: " << std::to_string(verts.size()) + "\n";
    for (const auto& v : verts)
    {
      file << std::to_string(v[0]) + " " 
            + std::to_string(v[1]) + " "
            + std::to_string(v[2]) + "\n";
    }
  }
  file.close();

  std::cout << "Save as: " + _fpath + "\n";
  formMain_ActivateMainForm();
}


// TODO 使いまわしがあるので後で一部関数化
void ModeSegStrokeFfd::LoadState(const std::string& fpath)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  std::ifstream file(fpath);
  if (!file) return;

  std::string line, key;
  std::stringstream ss;

  // ModeSegStrokeFfd_State
  if (!ReadNextLine(file, line, ss)) return;
  std::cout << line << "\n";

  // NumFrames: n
  if (!ReadNextLine(file, line, ss)) return;
  int _num_frames = 0;
  ss >> key >> _num_frames;
  std::cout << key << _num_frames << "\n";

  if (_num_frames != num_frames) return;

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


  //CageVerts: n
  if (!ReadNextLine(file, line, ss)) return;
  int _num_frames1 = 0;
  ss >> key >> _num_frames1;
  std::cout << key << _num_frames1 << "\n";
  if (_num_frames1 != num_frames) return;

  for (int fi = 0; fi < num_frames; ++fi)
  {
    int num_verts = 0;
    if (!ReadNextLine(file, line, ss)) return;
    ss >> key >> num_verts;
    std::vector<EVec3f> verts;
    for (int j = 0; j < num_verts; ++j)
    {
      if (!ReadNextLine(file, line, ss)) return;
      float x, y, z;
      ss >> x >> y >> z;
      verts.push_back(EVec3f(x, y, z));
    }
    
    m_meshseq.SetCageVertices(fi, verts);
  }

  file.close();
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}




// 現在の状態をスナップショットとして history.undo に保存し，history.redo をクリアする
void ModeSegStrokeFfd::Do_RecordSnapShot()
{
  const int frame_idx = formVisParam_getframeI();
  const auto& cage_verts = m_meshseq.GetCageVertices(frame_idx);
  m_history.push({ frame_idx, m_curves, m_shared_curves, cage_verts});
}


void ModeSegStrokeFfd::Undo_LoadSnapShot()
{
  const int frame_idx = formVisParam_getframeI();
  if (m_history.empty())
  {
    std::cout << "No more undo.\n";
    return;
  }

  if (m_history.top().frame_idx != frame_idx)
  {
    if (!ShowMsgDlgYesNo("undo for different frame", "異なるframeのUndoをしてよいですか？"))
      return;
  }

  const auto snap = m_history.top();
  m_history.pop();
  m_curves = snap.curves;
  m_shared_curves = snap.shared_curves;
  m_meshseq.SetCageVertices(frame_idx, snap.cage_verts);

  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}



void ModeSegStrokeFfd::CopyFromPrevFrame()
{
  const int frame_idx = formVisParam_getframeI();
  if (frame_idx <= 0) return;
  std::cout << "Copy from previous frame.\n";
  Do_RecordSnapShot();
  m_curves[frame_idx] = m_curves[frame_idx - 1];
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



void ModeSegStrokeFfd::MakeSelectedStroke_Shared()
{
  const int frame_idx = formVisParam_getframeI();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  bool tf = !m_select_info.selected ||
    m_select_info.is_shared ||
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



void ModeSegStrokeFfd::FlipSelectedStrokeNormalSide()
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



void ModeSegStrokeFfd::MakeSelectedStroke_Unshared()
{
  const int frame_idx = formVisParam_getframeI();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  bool tf = !m_select_info.selected ||
    !m_select_info.is_shared ||
    m_select_info.curve_idx < 0 ||
    m_shared_curves.size() <= m_select_info.curve_idx;

  for (int fi = 0; fi < num_frames; ++fi)
  {
    if (m_shared_curves[m_select_info.curve_idx].IsManipulated(fi))
    {
      PlanarCurve c = m_shared_curves[m_select_info.curve_idx].GetCurve(fi);
      m_curves[fi].push_back(c);
    }
  }

  m_shared_curves.erase(m_shared_curves.begin() + m_select_info.curve_idx);
  formMain_RedrawMainPanel();
  formMain_ActivateMainForm();
}


void ModeSegStrokeFfd::ClearSelectionInfo()
{
  m_select_info.Clear();
}



/*
void ModeSegStrokeFfd::LockSelectedStroke()
{
  const int frame_idx = formVisParam_getframeI();
  DeformationStrokes& dstrokes = m_strokes[frame_idx];
  dstrokes.Lock_SelStroke();
  UpdateSharedStroke();
  formMain_RedrawMainPanel();
}


void ModeSegStrokeFfd::UnlockSelectedStroke()
{
  const int frame_idx = formVisParam_getframeI();
  DeformationStrokes& dstrokes = m_strokes[frame_idx];
  dstrokes.Unlock_SelStroke();
  UpdateSharedStroke();
  formMain_RedrawMainPanel();
}
*/
