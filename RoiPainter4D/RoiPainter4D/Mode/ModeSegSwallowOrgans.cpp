#pragma unmanaged
#include "ModeSegSwallowOrgans.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#pragma managed
#include "CliMessageBox.h"
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegSwallowOrgans.h"
#include "FormSegSwallowOrganTimeline.h"
#pragma unmanaged

#include "GlslShader.h"
#include <fstream>
#include <iomanip> //setw用(obj出力時のファイル名）


using namespace RoiPainter4D;

/////////////////////////////////////////////////////////////////
/////////ModeSegSwallowOrgans////////////////////////////////////
/////////////////////////////////////////////////////////////////

ModeSegSwallowOrgans::ModeSegSwallowOrgans()
{
  std::cout << "ModeSegSwallowOrgans...\n";
  m_bL = m_bR = m_bM = false;
  std::cout << "ModeSegSwallowOrgans DONE\n";
}



ModeSegSwallowOrgans::~ModeSegSwallowOrgans()
{
  m_meshseq.Clear();
}



void ModeSegSwallowOrgans::StartMode()
{
  m_bL = m_bR = m_bM = false;
  m_meshseq.Clear();

  m_b_draw_cutstroke = false;
  m_b_draw_selectionrect = false;

  m_draghandle_id = OHDL_NON;
  m_manip_log.clear();
  m_undo_piv = -1;
  m_b_vismesh_transparent = false;

  auto c = ImageCore::GetInst()->GetCuboidF();
  int cprate = FormSegSwqllowOrgans_GetCpSize();
  m_handle_len = c[0] * 0.03f * cprate;
  m_handle_wid = c[0] * 0.03f * cprate * 0.04f;
  m_cp_radi    = c[0] * 0.0015f * cprate;

  //initialize vFlg
  ImageCore::GetInst()->InitializeFlg4dByMask(formMain_SetProgressValue);
  UpdateImageCoreVisVolumes();

  //initialize cage point growp
  for (int i = 0; i < NUM_POINT_GROUPS; ++i)
  {
    m_groups[i].m_name = "na";
    m_groups[i].m_ids.clear();
  }

  //show dialog
  FormSegSwallowOrgans_Show();
  FormSegSwallowOrgans_InitAllItems();

  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  FormSegSwallowTimeline_StartMode(num_frames);
  FormSegSwallowTimeline_Show();
  FormSegSwallowOrgans_UpdateList();
}



bool ModeSegSwallowOrgans::CanEndMode()
{
  if (!m_meshseq.IsInitialized())
    return true;
  else return
    ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Leaving?");
}



void ModeSegSwallowOrgans::FinishSegmentation()
{
  FillInMesh();

  if (!bForeVoxelExist_flg4())
  {
    ShowMsgDlg_OK(MESSAGE_NO_FOREGROUND, "no foreground");
    return;
  }
  ImageCore::GetInst()->mask_storeCurrentForeGround();

  m_meshseq.Clear();

  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  formMain_RedrawMainPanel();
}



void ModeSegSwallowOrgans::CancelSegmentation()
{
  if (!m_meshseq.IsInitialized() ||
    ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "cancel?"))
  {
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    formMain_RedrawMainPanel();
  }
}



void ModeSegSwallowOrgans::FillInMesh()
{
  if (!m_meshseq.IsInitialized())
  {
    return;
  }

  const EVec3i        reso  = ImageCore::GetInst()->GetReso();
  const EVec3f        pitch = ImageCore::GetInst()->GetPitch();
  std::vector<byte*>& flg4d = ImageCore::GetInst()->m_flg4d;

  const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  byte* v_bin = new byte[num_voxels];

  for (int f = 0; f < num_frames; ++f)
  {
    m_meshseq.GetMesh(f).GenBinaryVolume(reso, pitch, v_bin);

    byte* flg3d = flg4d[f];
#pragma omp parallel for
    for (int i = 0; i < num_voxels; ++i)
    {
      flg3d[i] = (flg3d[i] == 0) ? 0 :
                 (v_bin[i] == 1) ? 255 : 1;
    }

    formMain_SetProgressValue(f / (float)num_frames);
    std::cout << "fillInMesh " << f << "/" << num_frames << "\n";
  }
  delete[] v_bin;

  formMain_SetProgressValue(0);
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}



void ModeSegSwallowOrgans::CopyCurrentMeshToOtherFrames()
{
  static const char* MESSAGE_FOR_COPY =
    "The system is going to copy current shape to the other all frames, OK?\n"
    "現在frameの形状を全フレームにコピーしてよいですか？\n"
    "この処理を行うとここまでのundoができなくなります";

  if (!ShowMsgDlgYesNo(MESSAGE_FOR_COPY, "Message"))
    return;

  const int frame_idx = formVisParam_getframeI();
  m_meshseq.CopyOneFrameToTheOtherAllFrames(frame_idx);
  m_manip_log.clear();
  m_undo_piv = -1;
}



void ModeSegSwallowOrgans::SaveCageMeshSequenceObj(std::string fname, bool b_cage)
{
  m_meshseq.ExportCageMeshSequenceAsObj(fname, b_cage);
}


void ModeSegSwallowOrgans::SaveCageMeshSequenceTxt(std::string fname, bool b_cage)
{
  m_meshseq.ExportCageMeshSequenceAsTxt(fname, b_cage);
}


void ModeSegSwallowOrgans::LoadCageSequence(std::string fname)
{
  m_meshseq.ImportCageSequenceFromTxt(fname);
  m_manip_log.clear();
  m_undo_piv = -1;
  formMain_RedrawMainPanel();
}


void ModeSegSwallowOrgans::LoadNewMeshAndCage
(
  std::string mesh_path,
  std::string cage_path
)
{
  m_fpath_mesh = mesh_path;
  m_fpath_cage = cage_path;

  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();
  m_meshseq.Initialize(num_frames, cuboid, mesh_path, cage_path);

  //initialize group
  for (int i = 0; i < NUM_POINT_GROUPS; ++i)
  {
    m_groups[i].m_name = "na";
    m_groups[i].m_ids.clear();
  }

  m_groups[0].m_name = "all";
  for (int i = 0; i < m_meshseq.GetNumCageVertex(); ++i)
    m_groups[0].m_ids.insert(i);

  m_manip_log.clear();
  m_undo_piv = -1;

  FormSegSwallowOrgans_UpdateList();
  formMain_RedrawMainPanel();
}

//ある階層で変形した結果を、次の階層のcageに適用する関数
//1. 新しく obj (次の階層のケージ)を読み込み、
//2. 変形用のhamonic coordinate作成
//3. 現在の変形結果を適用し、objの変形を出力する
void ModeSegSwallowOrgans::LoadObj_ApplyFfdExportAsTxt(
  std::string fname_obj,
  std::string fname_output)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();

  //Step 1 & 2 現在のケージと、fname_objを利用して CageMeshSequence作成
  CagedMeshSequence meshseq;
  meshseq.Initialize(num_frames, cuboid, fname_obj, m_fpath_cage);

  //Step 3a. 現在の変形結果を適用
  const int num_cage_vtx = meshseq.GetNumCageVertex();
  for (int f = 0; f < num_frames; ++f)
  {
    for (int i = 0; i < num_cage_vtx; ++i)
    {
      EVec3f p = m_meshseq.GetCageVertex(f, i);
      meshseq.SetCageVertex(f, i, p, false);
    }
    meshseq.UpdateMeshShape(f);
  }

  //Step 3b. cage頂点を出力
  meshseq.ExportCageMeshSequenceAsTxt(fname_output, false);
}



///////////////////////////////////////////////////////////////////////////////
//Mouse Listener///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ModeSegSwallowOrgans::LBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = true;

  const int frame_idx = formVisParam_getframeI();
  EVec3f ray_p, ray_d;
  std::tie(ray_p, ray_d) = ogl->GetCursorRay1(p);

  if (IsCtrKeyOn())
  {
    m_stroke.clear();
    m_b_draw_cutstroke = true;
  }
  else if (IsShiftKeyOn() && m_meshseq.IsInitialized())
  {
    if (m_meshseq.GetNumSelectedVtx() > 0)
    {
      const EVec3f c = m_meshseq.GetSelectedVtxCentroid(frame_idx);
      int trans_rot_scale = FormSegSwallowOrgans_ModeTransRotScale();
      m_draghandle_id = PickHandle(c, m_handle_len, m_handle_wid, ray_p, ray_d, trans_rot_scale);

      if (m_draghandle_id > 0)
      {
        m_cagevtx_prev = m_meshseq.GetCageVertices(frame_idx);
      }
    }

    if (m_draghandle_id == OHDL_NON)
    {
      if (IsSKeyOn()) m_meshseq.ClearSelectedVtx();

      //pick to add
      m_meshseq.SelectCageVtxByPick(frame_idx, ray_p, ray_d, m_cp_radi);
      FormSegSwallowOrgans_SetNumSelectVtx(m_meshseq.GetNumSelectedVtx());
      FormSegSwallowTimeline_RedrawPanel();

      //start drawing rect
      m_b_draw_selectionrect = true;
      for (int i = 0; i < 4; ++i) m_selectrect[i] = EVec3f(0, 0, 0);

      formMain_RedrawMainPanel();
    }
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }

  m_prevpt = m_initpt = p;
}



void ModeSegSwallowOrgans::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  if (m_b_draw_cutstroke)
  {
    EVec3f c = ImageCore::GetCuboid();
    CrssecCore::CreateCurvedCrssec(c, ogl->GetCamPos(), m_stroke);
  }

  if (m_draghandle_id != OHDL_NON)
  {
    const int frameidx = formVisParam_getframeI();
    auto verts = m_meshseq.GetCageVertices(frameidx);

    if (m_undo_piv < (int)m_manip_log.size() - 1)
    {
      //m_manip_log[m_undo_piv + 1...]を削除
      m_manip_log.erase(m_manip_log.begin() + m_undo_piv + 1, m_manip_log.end());
    }

    m_manip_log.push_back(CageManipLog(frameidx, m_cagevtx_prev, verts));
    m_undo_piv = (int)m_manip_log.size() - 1;
  }

  if (m_b_draw_selectionrect && m_meshseq.IsInitialized())
  {
    m_meshseq.SelectCageVtxByRect(formVisParam_getframeI(), m_initpt, p, ogl);
    FormSegSwallowOrgans_SetNumSelectVtx(m_meshseq.GetNumSelectedVtx());
    FormSegSwallowTimeline_RedrawPanel();

    if (FormSegSwallowOrgans_bFitCrssec()) FitCrssecToSelectedCageVtx();
  }

  m_draghandle_id = OHDL_NON;
  m_b_draw_cutstroke = false;
  m_b_draw_selectionrect = false;

  m_bL = false;
  ogl->BtnUp();
  formMain_setCursorDefault();
  formMain_RedrawMainPanel();
}



void ModeSegSwallowOrgans::RBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  if (m_draghandle_id != OHDL_NON)
  {
    //Translateのドラッグ中の右クリックキャンセル
    const int frame_idx = formVisParam_getframeI();
    m_meshseq.SetCageVertices(frame_idx, m_cagevtx_prev);

    m_draghandle_id = OHDL_NON;
    FormSegSwallowTimeline_RedrawPanel();
  }
  else
  {
    ogl->BtnDown_Rot(p);
  }
  m_bR = true;
}

void ModeSegSwallowOrgans::RBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}

void ModeSegSwallowOrgans::MBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}

void ModeSegSwallowOrgans::MBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}



void ModeSegSwallowOrgans::MouseMove(const EVec2i& p, OglForCLI* ogl)
{
  const int frame_idx = formVisParam_getframeI();
  const int mode_trs  = FormSegSwallowOrgans_ModeTransRotScale();
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if (!m_bL && !m_bR && !m_bM)
  {
    formMain_setCursorDefault();
    if (IsShiftKeyOn() && m_meshseq.IsInitialized() && m_meshseq.GetNumSelectedVtx() > 0)
    {
      const EVec3f c  = m_meshseq.GetSelectedVtxCentroid(frame_idx);
      auto tmp_draghandle_id = PickHandle(c, m_handle_len, m_handle_wid, ray_pos, ray_dir, mode_trs);
      if (tmp_draghandle_id != OHDL_NON) formMain_setCursorNESW();
    }
    return;
  }

  if (m_b_draw_cutstroke)
  {
    m_stroke.push_back(ray_pos + 0.1f * ray_dir);
  }
  else if (m_draghandle_id != OHDL_NON)
  {
    if (mode_trs == 0)
      m_meshseq.TranslateSelectedVerts(frame_idx, m_prevpt, p, m_draghandle_id, ogl);
    else if (mode_trs == 1)
      m_meshseq.RotateSelectedVerts(frame_idx, m_prevpt, p, m_draghandle_id, m_handle_len, ogl);
    else if (mode_trs == 2)
      m_meshseq.ScaleSelectedVerts(frame_idx, m_prevpt, p, ogl);

    if (FormSegSwallowOrgans_bFitCrssec()) FitCrssecToSelectedCageVtx();
    FormSegSwallowTimeline_RedrawPanel();

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

  m_prevpt = p;
  formMain_RedrawMainPanel();
}


void ModeSegSwallowOrgans::MouseWheel(
  const EVec2i& p,
  short z_delta,
  OglForCLI* ogl)
{
  if (!WheelingCrssec(p, z_delta, ogl))
    ogl->ZoomCamera(z_delta * 0.1f);
  formMain_RedrawMainPanel();
}


void ModeSegSwallowOrgans::LBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeSegSwallowOrgans::RBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeSegSwallowOrgans::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeSegSwallowOrgans::KeyUp(int nChar) {}


void ModeSegSwallowOrgans::KeyDown(int nChar)
{
  if (nChar == 'C')
  {
    SelectedCageVtx_Clear();
    FormSegSwallowOrgans_SetNumSelectVtx(0);
    FormSegSwallowTimeline_RedrawPanel();
  }
  else if (nChar == 'R') { this->RegistPointGroup(); }
  else if (nChar == '0') { this->CheckoutPointGroup(0); }
  else if (nChar == '1') { this->CheckoutPointGroup(1); }
  else if (nChar == '2') { this->CheckoutPointGroup(2); }
  else if (nChar == '3') { this->CheckoutPointGroup(3); }
  else if (nChar == '4') { this->CheckoutPointGroup(4); }
  else if (nChar == '5') { this->CheckoutPointGroup(5); }
  else if (nChar == '6') { this->CheckoutPointGroup(6); }
  else if (nChar == '7') { this->CheckoutPointGroup(7); }
  else if (nChar == '8') { this->CheckoutPointGroup(8); }
  else if (nChar == '9') { this->CheckoutPointGroup(9); }
  else if (nChar == 'Z' && IsCtrKeyOn()) {
    this->UndoCpManipulation();
  }
  else if (nChar == 'Y' && IsCtrKeyOn()) {
    this->RedoCpManipulation();
  }
  else if (nChar == VK_F12)
  {
    m_b_vismesh_transparent = !m_b_vismesh_transparent;
  }
  //std::cout << nChar << "\n";
}



void ModeSegSwallowOrgans::SelectedCageVtx_Clear()
{
  m_meshseq.ClearSelectedVtx();
}


static int VIS_COL_MAX = 5;
static float diff_vis[5][4] = {
  {0.9f,0.5f,0.5f,0.2f},
  {0.9f,0.5f,0.1f,0.2f},
  {0.5f,0.9f,0.5f,0.2f},
  {0.3f,0.9f,0.4f,0.2f},
  {0.5f,0.5f,0.9f,0.2f},
};

static float ambi_vis[5][4] = {
  {0.7f,0.5f,0.5f,0.2f},
  {0.7f,0.5f,0.1f,0.2f},
  {0.5f,0.7f,0.5f,0.2f},
  {0.3f,0.7f,0.4f,0.2f},
  {0.5f,0.5f,0.7f,0.2f},
};
static float spec[4] = { 0.9f,0.9f,0.9f,0.2f };
static float shin[1] = { 54.0f };




void ModeSegSwallowOrgans::DrawScene(
  const EVec3f& cam_pos,
  const EVec3f& cam_center)
{
  const int    frame_idx = formVisParam_getframeI();
  const EVec3f cuboid    = ImageCore::GetCuboid();

  ImageCore::GetInst()->BindAllVolumes();
  
  DrawCrossSectionsNormal();

  if (m_b_draw_cutstroke)
    DrawPolyLine(EVec3f(1, 1, 0), 3, m_stroke, false);

  if (m_b_draw_selectionrect)
    DrawPolyLine(EVec3f(1, 1, 0), 3, 4, m_selectrect, true);


  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);

  //draw cage (handle or normam)
  if (IsShiftKeyOn() && m_meshseq.GetNumSelectedVtx() > 0)
  {
    if (m_meshseq.GetNumSelectedVtx() > 0)
    {
      const EVec3f c  = m_meshseq.GetSelectedVtxCentroid(frame_idx);
      int trans_rot_scale = FormSegSwallowOrgans_ModeTransRotScale();
      DrawHandle(c, m_handle_len, m_handle_wid, trans_rot_scale);
    }
  }

  if (!IsSpaceKeyOn())
  {
    const auto &c = m_meshseq.GetCage(frame_idx);
    const bool vis_all = !FormSegSwallowOrgans_bShowOnlySelectedPts();
    DrawCageWithCPs(c, vis_all, m_cp_radi, m_meshseq.GetSelectedCageVtxVec());
  }
  
  bool draw_bound      = FormSegSwallowOrgans_bVisBound();
  bool draw_surf_trans = FormSegSwallowOrgans_bVisSurfTrans();
  bool draw_surf_solid = FormSegSwallowOrgans_bVisSurfSolid();
  bool draw_vismeshes  = FormSegSwallowOrgans_bVisMeshes();
  const bool b_xy = formVisParam_bPlaneXY();
  const bool b_yz = formVisParam_bPlaneYZ();
  const bool b_zx = formVisParam_bPlaneZX();

  if (!IsSpaceKeyOn() && (draw_bound || draw_surf_trans))
  {
    //draw cage & mesh 
    glDisable(GL_CULL_FACE);
    glDepthMask(false);
    glEnable(GL_BLEND);

    float planexy = b_xy ? CrssecCore::GetInst()->GetPlanePosXY() : -1;
    float planeyz = b_yz ? CrssecCore::GetInst()->GetPlanePosYZ() : -1;
    float planezx = b_zx ? CrssecCore::GetInst()->GetPlanePosZX() : -1;
    if (!draw_bound)
    {
      planexy = planeyz = planezx = -1;
    }

    float opacity = draw_surf_trans ? (float)0.4 : (float)0;
    const auto &m = m_meshseq.GetMesh(frame_idx);
    DrawMeshWithCrossecHL(m, planeyz, planezx, planexy, opacity, cuboid);

    glDepthMask(true);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
  }

  //draw mesh (solid)
  if (!IsSpaceKeyOn() && draw_surf_solid)
  {
    glDisable(GL_CULL_FACE);
    const auto& m = m_meshseq.GetMesh(frame_idx);
    DrawMeshWithCrossecHL(m, -1,-1,-1, 1.0f, cuboid);
  }

  //draw vis meshes
  if (!IsSpaceKeyOn() && draw_vismeshes)
  {
    glEnable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    float planexy = b_xy ? CrssecCore::GetInst()->GetPlanePosXY() : -1;
    float planeyz = b_yz ? CrssecCore::GetInst()->GetPlanePosYZ() : -1;
    float planezx = b_zx ? CrssecCore::GetInst()->GetPlanePosZX() : -1;

    for (int i = 0; i < m_vismeshes.size(); ++i)
    {
      if (frame_idx >= m_vismeshes[i].m_meshes.size()) continue;
      const TMesh &m = m_vismeshes[i].m_meshes[frame_idx];

      if (!m_b_vismesh_transparent)
      {
        const int color_idx = i % VIS_COL_MAX;
        m.Draw(diff_vis[color_idx], ambi_vis[color_idx], spec, shin);
      }
      else
      {
        glDepthMask(false);
        glEnable(GL_BLEND);
        DrawMeshWithCrossecHL(m, planeyz, planezx, planexy, 0.4f, cuboid);
        glDepthMask(true);
      }
    }
  }

  //draw volume 
  if (formVisParam_bRendVol())
  {
    DrawVolumeVisMask(!IsShiftKeyOn(), cam_pos, cam_center);
  }
}



//PointGroupの登録
void ModeSegSwallowOrgans::RegistPointGroup()
{
  //find null group
  int group_idx = -1;
  for (int i = 0; i < NUM_POINT_GROUPS; ++i)
    if (m_groups[i].m_ids.size() == 0)
    {
      group_idx = i;
      break;
    }

  if (group_idx == -1)
  {
    ShowMsgDlg_OK("group is full/グループ数に空きがありません", "message");
    return;
  }

  m_groups[group_idx].m_ids = m_meshseq.GetSelectedCageVtxSet();
  m_groups[group_idx].m_name = "group";

  FormSegSwallowOrgans_UpdateList();
}



//group[idx]をよびだす
void  ModeSegSwallowOrgans::CheckoutPointGroup(int idx)
{
  m_meshseq.ClearSelectedVtx();

  if (0 <= idx && idx < NUM_POINT_GROUPS &&
    m_groups[idx].m_ids.size() != 0)
  {
    m_meshseq.SetCageVtxSelected(m_groups[idx].m_ids);
    FormSegSwallowOrgans_SetNumSelectVtx(m_meshseq.GetNumSelectedVtx());
    FormSegSwallowTimeline_RedrawPanel();

    if (FormSegSwallowOrgans_bFitCrssec()) FitCrssecToSelectedCageVtx();
  }

  formMain_RedrawMainPanel();
}



//group[idx]を削除
void  ModeSegSwallowOrgans::DeletePointGroup(int idx)
{
  m_meshseq.ClearSelectedVtx();

  if (0 <= idx && idx < NUM_POINT_GROUPS &&
    m_groups[idx].m_ids.size() != 0)
  {
    m_groups[idx].m_name = "na";
    m_groups[idx].m_ids.clear();

    FormSegSwallowOrgans_UpdateList();
    formMain_RedrawMainPanel();
  }

}



void  ModeSegSwallowOrgans::LoadPointGroup(std::string fname)
{
  if (m_meshseq.GetNumCageVertex() == 0) return;

  std::ifstream ifs(fname);

  if (ifs.fail())
  {
    std::cout << "error when opening " << fname.c_str() << "\n";
    return;
  }

  std::string str;
  int tmp_numgroups;
  ifs >> str >> tmp_numgroups;

  for (int i = 0; i < tmp_numgroups && i < NUM_POINT_GROUPS; ++i)
  {
    int idx, num_points;
    std::string name;
    ifs >> idx >> name >> num_points;
    m_groups[i].m_name = name;

    for (int j = 0; j < num_points; ++j)
    {
      int point_idx;
      ifs >> point_idx;
      m_groups[i].m_ids.insert(point_idx);
    }
  }
  ifs.close();

  FormSegSwallowOrgans_UpdateList();
  formMain_RedrawMainPanel();
}



void  ModeSegSwallowOrgans::SavePointGroup(std::string fname)
{
  std::ofstream fp(fname);

  if (!fp.is_open())
  {
    std::cout << "error when opening " << fname.c_str() << "\n";
    return;
  }

  fp << "roipainter4d_Swallowingtool_pointgroup " << NUM_POINT_GROUPS << "\n";
  for (int i = 0; i < NUM_POINT_GROUPS; ++i)
  {
    fp << i << " " << m_groups[i].m_name << " "
      << m_groups[i].m_ids.size() << "\n";

    for (auto& it : m_groups[i].m_ids)
      fp << it << " ";

    fp << "\n\n";
  }

  fp.close();
}



std::vector<EVec3f> ModeSegSwallowOrgans::SelectedCageVtx_GetCentroidSeq()
{
  return m_meshseq.GetSelectedVtxCentroidSeq();
}

std::vector<std::vector<EVec3f>> ModeSegSwallowOrgans::SelectedCageVtx_GetSeq()
{
  return m_meshseq.GetSelectedVtxSeq();
}

std::vector<std::vector<EVec3f>> ModeSegSwallowOrgans::SelectedCageVtx_Get1RingSeq()
{
  return m_meshseq.GetSelectVtx1RingSeq();
}



//ここでもm_manip_logに登録を行いたいが、現在はdlgのmouse moveで呼ぶ実装なので
 //この操作はundoできないことにする
void ModeSegSwallowOrgans::SelectedCageVtx_Move(
  int frame_idx,
  int dim_xyz,
  float pos)
{
  m_meshseq.SetSelectedCageVtxPos(frame_idx, dim_xyz, pos);
}


void ModeSegSwallowOrgans::SelectedCageVtx_Smoothing(std::vector<int> trgt_fids)
{
  m_meshseq.SelectedCageVtx_Smoothing(trgt_fids);
}

void ModeSegSwallowOrgans::SelectedCageVtx_CopyLeft(std::vector<int> trgt_fids)
{
  m_meshseq.SelectedCageVtx_CopyFromLeft(trgt_fids);
}

void ModeSegSwallowOrgans::SelectedCageVtx_CopyRight(std::vector<int> trgt_fids)
{
  m_meshseq.SelectedCageVtx_CopyFromRight(trgt_fids);
}


void ModeSegSwallowOrgans::FitCrssecToSelectedCageVtx()
{
  if (!FormSegSwallowOrgans_bFitCrssec()) return;
  if (m_meshseq.GetNumSelectedVtx() <= 0) return;

  const int    frame_idx = formVisParam_getframeI();
  const EVec3i reso = ImageCore::GetInst()->GetReso();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();
  const EVec3f gc = m_meshseq.GetSelectedVtxCentroid(frame_idx);
  CrssecCore::FitCrssecToPosition(reso, pitch, gc, CRSSEC_XY);
  CrssecCore::FitCrssecToPosition(reso, pitch, gc, CRSSEC_YZ);
  CrssecCore::FitCrssecToPosition(reso, pitch, gc, CRSSEC_ZX);
}


void ModeSegSwallowOrgans::UndoCpManipulation()
{
  std::cout << "UndoCpManipulation";
  //m_manip_log[m_undo_piv] という操作をもとに戻す
  if (0 <= m_undo_piv && m_undo_piv < (int)m_manip_log.size())
  {
    m_meshseq.SetCageVertices(m_manip_log[m_undo_piv].m_frame, m_manip_log[m_undo_piv].m_point_prev);
    m_undo_piv -= 1;
  }
}


void ModeSegSwallowOrgans::RedoCpManipulation()
{
  std::cout << "RedoCpManipulation";

  //m_manip_log[m_undo_piv+1] という操作を再度実施する

  if (0 <= m_undo_piv + 1 && m_undo_piv + 1 < (int)m_manip_log.size())
  {
    m_meshseq.SetCageVertices(m_manip_log[m_undo_piv].m_frame, m_manip_log[m_undo_piv + 1].m_point_new);
    m_undo_piv += 1;
  }
}




