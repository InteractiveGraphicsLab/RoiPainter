#pragma unmanaged
#include "ModeSegSwallowTempGen.h"
#include "ModeCommonTools.h"

#include <iostream>
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegSwallowTempGen.h"
#pragma unmanaged

using namespace RoiPainter4D;



/*
* 1-2 初期
* 2-3 中期
* 4-5 後期 

+ 1. 食塊が動き始めたフレーム
+ 2. 舌骨が上がり切る
+ 3. 舌骨が手前に出て下がり始たフレーム
+ 4. 舌骨が下がり切る


Input:
Volume読み込み
Cage + Mesh + 動き + timeline

timeline.txt
-------------
  2  5  8  12
-------------

+ TODO 
- Mesh/Cage/動きのロード 
    - zetu OK
    - into OK 
    - koto OK
    - nank OK
- Mesh/Cage/動きの可視化
    - mesh/cage可視化
    - handle可視化 
    
- Cageの変形
　- 平行移動
  - 回転
  - 拡縮
    
- timelineのロード YET
- timelineの指定
  - 4こspinbox作る
  - 適用ボタン作る

- 適用ボタンの挙動作る


*/





ModeSegSwallowTempGen::~ModeSegSwallowTempGen()
{

}



ModeSegSwallowTempGen::ModeSegSwallowTempGen() :
  m_volume_shader("shader/volVtx.glsl", "shader/volFlg.glsl"),
  m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
{
  std::cout << "ModeSegSwallowTempGen...\n";

  m_bL = m_bR = m_bM = false;
  m_b_draw_cutstroke = false;

  std::cout << "ModeSegSwallowTempGen done\n";
}



bool ModeSegSwallowTempGen::CanEndMode()
{
  return true;
}


void ModeSegSwallowTempGen::StartMode()
{
  std::cout << "ModeSegSwallowTempGen...startMode----------\n";

  m_bL = m_bR = m_bM = false;
  FormSegSwallowTempGen_Show();

  auto c = ImageCore::GetInst()->GetCuboidF();
  int cprate = 6;

  m_HANDLE_L = c[0] * 0.03f * cprate;
  m_HANDLE_W = c[0] * 0.0015f * cprate;

  int N = ImageCore::GetInst()->GetNumFrames();
  FormSegSwallowTempGen_InitSpins( N, 2, 3, 4, 7);
  std::cout << "ModeSegSwallowTempGen...startMode DONE-----\n";
}



void ModeSegSwallowTempGen::LBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = true;

  if (IsCtrKeyOn())
  {
    m_stroke.clear();
    m_b_draw_cutstroke = true;
  }
  else if (IsShiftKeyOn())
  {
    m_pre_point = p;
    EVec3f ray_pos, ray_dir;
    ogl->GetCursorRay(p, ray_pos, ray_dir);
    const int frame_idx = formVisParam_getframeI();
    EVec3f gc = GetGravCenterTrgtCages(frame_idx);

    //変形開始 pick handle
    int trans_rot_scale = FormSegSwallowTempGen_isModeTrans() ? 0 :
      FormSegSwallowTempGen_isModeRot() ? 1 : 2;

    if (trans_rot_scale == 0) {
      m_drag_handle_id = PickHandleOrthoArrows( ray_pos, ray_dir , gc, m_HANDLE_L, m_HANDLE_W);
    }
    else if (trans_rot_scale == 1) {
      m_drag_handle_id = PickHandleOrthoCircles(ray_pos, ray_dir, gc, m_HANDLE_L , m_HANDLE_W);
    }
    else if (trans_rot_scale == 2) {
      m_drag_handle_id = PickHandleOrthoArrows( ray_pos, ray_dir , gc, m_HANDLE_L, m_HANDLE_W);
    }
       
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }
}



void ModeSegSwallowTempGen::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  if (m_b_draw_cutstroke)
  {
    EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();
    CrssecCore::CreateCurvedCrssec(cuboid, ogl->GetCamPos(), m_stroke);
  }
  m_drag_handle_id = OHDL_NON;


  m_b_draw_cutstroke = false;
  m_bL = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}



void ModeSegSwallowTempGen::RBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = true;
  ogl->BtnDown_Rot(p);
}


void ModeSegSwallowTempGen::RBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeSegSwallowTempGen::MBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}


void ModeSegSwallowTempGen::MBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeSegSwallowTempGen::LBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeSegSwallowTempGen::RBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeSegSwallowTempGen::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}


void ModeSegSwallowTempGen::MouseMove(const EVec2i& p, OglForCLI* ogl)
{
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if (m_b_draw_cutstroke)
  {
    m_stroke.push_back(ray_pos + 0.1f * ray_dir);
    formMain_RedrawMainPanel();
  }
  else if (m_drag_handle_id != OHDL_NON)
  {
    if ( FormSegSwallowTempGen_isModeTrans() ) 
      TranslateTargetCages(m_pre_point, p, m_drag_handle_id, ogl);
    else if (FormSegSwallowTempGen_isModeRot()) 
      RotateTargetCages(m_pre_point, p, m_drag_handle_id, ogl);
    else 
      ScaleTargetCages(m_pre_point, p, m_drag_handle_id, ogl);
    m_pre_point = p;
    formMain_RedrawMainPanel();
  }
  else if (m_bL || m_bR || m_bM)
  {
    ogl->MouseMove(p);
    formMain_RedrawMainPanel();
  }
}



void ModeSegSwallowTempGen::MouseWheel(const EVec2i& p, short z_delta, OglForCLI* ogl)
{
  if (!WheelingCrssec(p, z_delta, ogl)) {
    ogl->ZoomCamera(z_delta * 0.1f);
  }
  formMain_RedrawMainPanel();
}


void ModeSegSwallowTempGen::KeyDown(int nChar) {}
void ModeSegSwallowTempGen::KeyUp(int nChar) {}



void ModeSegSwallowTempGen::DrawScene(const EVec3f& cuboid, const EVec3f& camP, const EVec3f& camF)
{
  const EVec3i reso = ImageCore::GetInst()->GetReso();

  //bind volumes 
  BindAllVolumes();

  //draw cross section
  DrawCrossSections(cuboid, reso, false, m_crssec_shader);



  const int frame_idx = formVisParam_getframeI();
  const bool b_xy = formVisParam_bPlaneXY();
  const bool b_yz = formVisParam_bPlaneYZ();
  const bool b_zx = formVisParam_bPlaneZX();
  float planexy = b_xy ? CrssecCore::GetInst()->GetPlanePosXY() : -1;
  float planeyz = b_yz ? CrssecCore::GetInst()->GetPlanePosYZ() : -1;
  float planezx = b_zx ? CrssecCore::GetInst()->GetPlanePosZX() : -1;

  if( !IsSpaceKeyOn() )
  {
    CagedMeshSequence::SetControlPointRadius(0.3f);
    m_cagedmesh_zetsu.DrawCage(frame_idx, true);
    m_cagedmesh_zetsu.DrawMesh(frame_idx, planeyz, planezx, planexy, 0.1f, cuboid);
    m_cagedmesh_koto.DrawCage(frame_idx, true);
    m_cagedmesh_koto.DrawMesh(frame_idx, planeyz, planezx, planexy, 0.1f, cuboid);
    m_cagedmesh_nank.DrawCage(frame_idx, true);
    m_cagedmesh_nank.DrawMesh(frame_idx, planeyz, planezx, planexy, 0.1f, cuboid);
    m_cagedmesh_into.DrawCage(frame_idx, true);
    m_cagedmesh_into.DrawMesh(frame_idx, planeyz, planezx, planexy, 0.1f, cuboid);
  }

  if (IsShiftKeyOn())
  {
    int trans_rot_scale = FormSegSwallowTempGen_isModeTrans() ? 0 :
      FormSegSwallowTempGen_isModeRot() ? 1 : 2;
    EVec3f gc = GetGravCenterTrgtCages(frame_idx);
    if (trans_rot_scale == 0)
      DrawHandleOrthoArrows(gc, m_HANDLE_L, m_HANDLE_W, COLOR_R, COLOR_G, COLOR_B);
    if (trans_rot_scale == 1)
      DrawHandleOrthoCircles(gc, m_HANDLE_L);
    if (trans_rot_scale == 2)
      DrawHandleOrthoCubes(gc, m_HANDLE_L, m_HANDLE_W, COLOR_R, COLOR_G, COLOR_B);
  }




  //cut stroke
  if (m_b_draw_cutstroke)
  {
    DrawPolyLine(EVec3f(1, 1, 0), 3, m_stroke, false);
  }

  if (formVisParam_bRendVol())
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    const bool  b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolumeSlices(cuboid, reso, camP, camF,
      false, b_onmanip, m_volume_shader);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }
  


}


EVec3f ModeSegSwallowTempGen::GetGravCenterTrgtCages(int frame_idx)
{
  int num = 0;
  EVec3f gc(0,0,0);
  if (FormSegSwallowTempGen_TransZetsu())
  {
    auto verts = m_cagedmesh_zetsu.GetCageVertices(frame_idx);
    for (auto& v : verts) {
      num++;
      gc += v;
    }
  }

  if (FormSegSwallowTempGen_TransNank())
  {
    auto verts = m_cagedmesh_nank.GetCageVertices(frame_idx);
    for (auto& v : verts) {
      num++;
      gc += v;
    }
  }

  if (FormSegSwallowTempGen_TransKoto())
  {
    auto verts = m_cagedmesh_koto.GetCageVertices(frame_idx);
    for (auto& v : verts) {
      num++;
      gc += v;
    }
  }

  if (FormSegSwallowTempGen_TransInto())
  {
    auto verts = m_cagedmesh_into.GetCageVertices(frame_idx);
    for (auto& v : verts) {
      num++;
      gc += v;
    }
  }
  
  if (num == 0 ) return EVec3f(0,0,0);
  else  return gc / (float) num; 
}








void ModeSegSwallowTempGen::LoadCagedMesh(
    const std::string& mesh_path,
    const std::string& cage_path,
    const std::string& deform_path,
    const SWALLOW_ORGAN_TYPE id)
{
  CagedMeshSequence &cm_modi = (id == SOT_ZETSU) ? m_cagedmesh_zetsu :
                               (id == SOT_NANK ) ? m_cagedmesh_nank  :
                               (id == SOT_KOTO ) ? m_cagedmesh_koto  : m_cagedmesh_into;
  CagedMeshSequence &cm_orig = (id == SOT_ZETSU) ? m_cagedmesh_zetsu_orig :
                               (id == SOT_NANK ) ? m_cagedmesh_nank_orig :
                               (id == SOT_KOTO ) ? m_cagedmesh_koto_orig : m_cagedmesh_into_orig;

  const int    num_frames = ImageCore::GetInst()->GetNumFrames();
  const EVec3f cuboid     = ImageCore::GetInst()->GetCuboidF()  ;
  cm_modi.Initialize(num_frames, cuboid, mesh_path, cage_path );
   
  //origの方は、num_frameが異なる可能性があるので、それを考慮して読み込み 
  cm_orig = cm_modi;
  cm_orig.ImportCageSequenceFromTxt(deform_path, true);
  
  for (int f = 0; f < num_frames; ++f)
  {
    cm_modi.SetCageVertices(f, cm_orig.GetCageVertices(0));
  }

  formMain_RedrawMainPanel();
}



void ModeSegSwallowTempGen::TranslateTargetCages(
    EVec2i p0, EVec2i p1, ORTHO_HANDLE_ID id, OglForCLI* ogl)
{
  const int frame_idx = formVisParam_getframeI();
  EVec3f gc = GetGravCenterTrgtCages(frame_idx);

  EVec3f ray_p0, ray_p1, ray_d0, ray_d1;
  std::tie(ray_p0, ray_d0) = ogl->GetCursorRay1(p0);
  std::tie(ray_p1, ray_d1) = ogl->GetCursorRay1(p1);

  float coef = Dist(gc, ogl->GetCamPos()) / Dist(ray_p0, ogl->GetCamPos());


  EVec3f trans = coef * (ray_p1 - ray_p0);
  if (id == OHDL_X) trans[1] = trans[2] = 0;
  if (id == OHDL_Y) trans[0] = trans[2] = 0;
  if (id == OHDL_Z) trans[0] = trans[1] = 0;
  
  int trgt_idx = frame_idx;
  if ( FormSegSwallowTempGen_TransAllFrame()) trgt_idx = -1;

  if (FormSegSwallowTempGen_TransZetsu())
    m_cagedmesh_zetsu.TranlateCage(trgt_idx, trans);
  if (FormSegSwallowTempGen_TransNank())
    m_cagedmesh_nank.TranlateCage(trgt_idx, trans);
  if (FormSegSwallowTempGen_TransKoto())
    m_cagedmesh_koto.TranlateCage(trgt_idx, trans);
  if (FormSegSwallowTempGen_TransInto())
    m_cagedmesh_into.TranlateCage(trgt_idx, trans);
}


void ModeSegSwallowTempGen::RotateTargetCages(EVec2i p0, EVec2i p1, ORTHO_HANDLE_ID id, OglForCLI* ogl)
{
  const int frame_idx = formVisParam_getframeI();
  EVec3f gc = GetGravCenterTrgtCages(frame_idx);

  EVec3f ray_p0, ray_p1, ray_d0, ray_d1;
  std::tie(ray_p0, ray_d0) = ogl->GetCursorRay1(p0);
  std::tie(ray_p1, ray_d1) = ogl->GetCursorRay1(p1);

  float coef = Dist(gc, ogl->GetCamPos()) / Dist(ray_p0, ogl->GetCamPos());
  EVec3f eye_ray = (ogl->GetCamCnt() - ogl->GetCamPos()).normalized();
  EVec3f trans = coef * (ray_p1 - ray_p0);

  EMat3f dR; // = CalcObjectRotationMatrixByMouseDragF(ogl, dx, dy, 0.007f);

  if (id == OHDL_XY)
  {
    trans[2] = 0;
    float theta = trans.norm() / m_HANDLE_L;
    if (trans.cross(eye_ray).z() < 0) theta *= -1;
    dR = Eigen::AngleAxisf(theta, EVec3f(0, 0, 1));
  }
  if (id == OHDL_YZ)
  {
    trans[0] = 0;
    float theta = trans.norm() / m_HANDLE_L;
    if (trans.cross(eye_ray).x() < 0) theta *= -1;
    dR = Eigen::AngleAxisf(theta, EVec3f(1, 0, 0));
  }
  if (id == OHDL_ZX)
  {
    trans[1] = 0;
    float theta = trans.norm() / m_HANDLE_L;
    if (trans.cross(eye_ray).y() < 0) theta *= -1;
    dR = Eigen::AngleAxisf(theta, EVec3f(0, 1, 0));
  }
  
  int trgt_idx = frame_idx;
  if (FormSegSwallowTempGen_TransAllFrame()) trgt_idx = -1;

  if (FormSegSwallowTempGen_TransZetsu())
    m_cagedmesh_zetsu.RotateCage(trgt_idx, gc, dR);
  if (FormSegSwallowTempGen_TransNank())
    m_cagedmesh_nank.RotateCage(trgt_idx, gc, dR);
  if (FormSegSwallowTempGen_TransKoto())
    m_cagedmesh_koto.RotateCage(trgt_idx, gc, dR);
  if (FormSegSwallowTempGen_TransInto())
    m_cagedmesh_into.RotateCage(trgt_idx, gc, dR);
}


void ModeSegSwallowTempGen::ScaleTargetCages(EVec2i p0, EVec2i p1, ORTHO_HANDLE_ID id, OglForCLI* ogl)
{
  const int frame_idx = formVisParam_getframeI();
  EVec3f gc = GetGravCenterTrgtCages(frame_idx);

  const int dx = p1.x() - p0.x();
  const int dy = p1.y() - p0.y();
  const float scale = 1.0f + dy * 0.01f;

  int trgt_idx = frame_idx;
  if (FormSegSwallowTempGen_TransAllFrame()) trgt_idx = -1;

  if (FormSegSwallowTempGen_TransZetsu())
    m_cagedmesh_zetsu.ScaleCage(trgt_idx, gc, scale);
  if (FormSegSwallowTempGen_TransNank())
    m_cagedmesh_nank.ScaleCage(trgt_idx, gc, scale);
  if (FormSegSwallowTempGen_TransKoto())
    m_cagedmesh_koto.ScaleCage(trgt_idx, gc, scale);
  if (FormSegSwallowTempGen_TransInto())
    m_cagedmesh_into.ScaleCage(trgt_idx, gc, scale);
}




static void CopyCageByTimingInfo
(
  const int _time_orig[4],
  const int _time_trgt[4],
  const CagedMeshSequence &cm_orig,
  CagedMeshSequence &cm_trgt
)
{
  const int num_orig = cm_orig.GetNumFrames();
  const int num_trgt = cm_trgt.GetNumFrames();
  if (num_orig == 0) return;

  //compute mapping 
  std::vector<int> time_orig = { 0, _time_orig[0], _time_orig[1], _time_orig[2], _time_orig[3], num_orig - 1 };
  std::vector<int> time_trgt = { 0, _time_trgt[0], _time_trgt[1], _time_trgt[2], _time_trgt[3], num_trgt - 1 };
  std::vector<float> map_trgt2orig(num_trgt, 0);
  
  
  for (int i = 0; i < 6; ++i) std::cout << time_orig[i] << " "; std::cout << "\n";
  for (int i = 0; i < 6; ++i) std::cout << time_trgt[i] << " "; std::cout << "\n";


  std::cout << num_orig << " aaaaa " << num_trgt << "\n";


  for (int j = 0; j < 4 + 1; ++j)
  {
    float f0_orig = (float)time_orig[ j ];
    float f1_orig = (float)time_orig[j+1];
    float f0_trgt = (float)time_trgt[ j ];
    float f1_trgt = (float)time_trgt[j+1];
    
    for (int i = (int)f0_trgt; i < (int)f1_trgt; ++i)
    {
      map_trgt2orig[i] = (i - f0_trgt) / (f1_trgt - f0_trgt) * (f1_orig - f0_orig) + f0_trgt;
    }
    map_trgt2orig[num_trgt-1] = (float)(num_orig - 1);
  }

  for (int i = 0; i < map_trgt2orig.size(); ++i)
  {
    std::cout << i << " --- " << map_trgt2orig[i] << "\n";

    int idx = (int)(map_trgt2orig[i] + 0.5);
    if (idx >= num_orig) idx = num_orig-1;
    cm_trgt.SetCageVertices(i, cm_orig.GetCageVertices(idx), true);
  }
  
}






void ModeSegSwallowTempGen::UpdateCageByTiming(
    std::string fname_time_orig, 
    int f1, int f2, int f3, int f4
    )
{
  //オリジナルタイムラインファイル読み込み
  std::ifstream ifs(fname_time_orig.c_str());
  int orig_f1, orig_f2, orig_f3, orig_f4;
  ifs >> orig_f1 >> orig_f2 >> orig_f3 >> orig_f4;

  int time_orig[4] = { orig_f1, orig_f2, orig_f3, orig_f4 };
  int time_trgt[4] = { f1, f2, f3, f4 };
  
  CopyCageByTimingInfo(time_orig, time_trgt, m_cagedmesh_zetsu_orig, m_cagedmesh_zetsu);
  CopyCageByTimingInfo(time_orig, time_trgt, m_cagedmesh_nank_orig , m_cagedmesh_nank);
  CopyCageByTimingInfo(time_orig, time_trgt, m_cagedmesh_koto_orig , m_cagedmesh_koto);
  CopyCageByTimingInfo(time_orig, time_trgt, m_cagedmesh_into_orig , m_cagedmesh_into);
  
}

void ModeSegSwallowTempGen::ExportCageMotionData(std::string fname)
{   
  if ( m_cagedmesh_zetsu.IsInitialized() )
    m_cagedmesh_zetsu.ExportCageMeshSequenceAsTxt(fname+"zetucage.txt", true);

  if (m_cagedmesh_nank.IsInitialized())
    m_cagedmesh_nank.ExportCageMeshSequenceAsTxt(fname + "nankcage.txt", true);

  if (m_cagedmesh_koto.IsInitialized())
    m_cagedmesh_koto.ExportCageMeshSequenceAsTxt(fname + "kotocage.txt", true);

  if (m_cagedmesh_into.IsInitialized())
    m_cagedmesh_into.ExportCageMeshSequenceAsTxt(fname + "intocage.txt", true);
}

