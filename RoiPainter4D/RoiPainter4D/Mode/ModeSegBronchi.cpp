#pragma unmanaged
#include "ModeSegBronchi.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegBronchi.h"
#include "CliMessageBox.h"
#pragma unmanaged

#include <iostream>
#include <queue>
#include <vector>
#include <numeric>
#include <iomanip>

#include <time.h>
#include <windows.h>
#define _USE_MATH_DEFINES
#include <cmath>

//#define DEBUG_MODE 1
#define DEBUG_MODE 0
//#define DEBUG_DRAWSCENE 1
#define DEBUG_DRAWSCENE 0
float TreeObj::m_radius = 0.0f;
float TreeObj::m_path_width = 0.0f;

using namespace RoiPainter4D;

#pragma pack(1)//TODOとる




ModeSegBronchi::ModeSegBronchi() :
  m_volume_shader("shader/volVtx.glsl"   , "shader/volFlg_Seg.glsl"),
  m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
  std::cout << "ModeSegBronchi.......................................!!!!!!!!!!!!\n";
  m_bL = m_bR = m_bM = false;
  std::cout << "ModeSegBronchi done\n";
}


ModeSegBronchi::~ModeSegBronchi()
{
  std::cout << "ModeSegBronchiデストラクタ\n";

  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int num_voxels = ImageCore::GetInst()->GetNumVoxels();


  for (int frame_idx = 0; frame_idx < num_frames; ++frame_idx)
  {
    if (m_min[frame_idx] != nullptr) delete[] m_min[frame_idx];
    if (m_max_m_min[frame_idx] != nullptr) delete[] m_max_m_min[frame_idx];
    if (t_dst4d[frame_idx] != nullptr) delete[] t_dst4d[frame_idx];
    m_min[frame_idx] = nullptr;
    m_max_m_min[frame_idx] = nullptr;
    t_dst4d[frame_idx] = nullptr;
  }
}


void ModeSegBronchi::startMode()
{
  if (DEBUG_MODE) std::cout << "ModeSegBronchi...startMode--------------------------\n";

  const int    num_frames = ImageCore::GetInst()->GetNumFrames();
  const int    num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const EVec3i reso  = ImageCore::GetInst()->GetReso();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();
  //initialize vFlg
  ImageCore::GetInst()->InitializeFlg4dByMask(formMain_SetProgressValue);


  IntializeVarState(num_frames, num_voxels);

  TreeObj::SetRadius(pitch.x() * 4);
  m_cp_mesh.InitializeIcosaHedron(TreeObj::GetRadius());
  TreeObj::SetPathWidth(4.0f);

  InitCenteredness(num_frames, num_voxels, reso);
  LoGFilter();
  ChangeImg(num_frames, num_voxels);

  formSegBronchi_Show();
  EVec2i vol_minmax = ImageCore::GetInst()->GetVolumeMinMax();
  formSegBronchi_InitAllItems(vol_minmax[0], vol_minmax[1]);
  formMain_RedrawMainPanel();

  if (DEBUG_MODE) std::cout << "ModeSegBronchi...startMode DONE-----\n";
}


bool ModeSegBronchi::canEndMode()
{
  return true;
}


void ModeSegBronchi::FinishSegmentation()
{
  const std::vector<byte*>& flg4d = ImageCore::GetInst()->m_flg4d;
  const int        num_frames = ImageCore::GetInst()->GetNumFrames();
  const int        num_voxels = ImageCore::GetInst()->GetNumVoxels();

  bool b_foreexist = false;

  for (int fi = 0; fi < num_frames && !b_foreexist; ++fi)
  {
    for (int vi = 0; vi < num_voxels && !b_foreexist; ++vi)
    {
      if (flg4d[fi][vi] == 255)
      {
        b_foreexist = true;
        break;
      }
    }
  }

  if (!b_foreexist)
  {
    ShowMsgDlg_OK(MESSAGE_NO_FOREGROUND, "No foreground");
    return;
  }

  ChangeImg(num_frames, num_voxels);


  ImageCore::GetInst()->mask_storeCurrentForeGround();
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}


void ModeSegBronchi::drawScene(const EVec3f& cuboid, const EVec3f& camP, const EVec3f& camF)
{
  BindAllVolumes();

  if (m_b_draw_cutstroke)
  {
    t_DrawPolyLine(EVec3f(1, 1, 0), 3, m_stroke, false);
  }

  //control points
  const static float diff[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
  const static float ambi[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
  const static float spec[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
  const static float shin[1] = { 56.0f };

  //root control points color
  const static float diff_root[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
  const static float ambi_root[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
  const static float spec_root[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
  const static float shin_root[1] = { 56.0f };

  const int frame_idx = formVisParam_getframeI();



  if (m_roots[frame_idx] != nullptr)
  {
    //control point draw
    glEnable(GL_LIGHTING);
    std::queue<TreeObj*> Q;
    Q.push(m_roots[frame_idx]);
    while (!Q.empty())
    {
      TreeObj*piv_cp = Q.front();
      Q.pop();

      for (const auto& child_cp :piv_cp->GetChildren())
      {
        Q.push(child_cp);
      }

      EVec3f& pos =piv_cp->GetPos();
      glTranslated(pos[0], pos[1], pos[2]);
      if (piv_cp == m_selected_cp) m_cp_mesh.Draw(diff_root, ambi_root, spec_root, shin_root);
      else                     m_cp_mesh.Draw(diff, ambi, spec, shin);
      glTranslated(-pos[0], -pos[1], -pos[2]);
    }
    glDisable(GL_LIGHTING);
    if (DEBUG_DRAWSCENE) std::cout << "cp draw後\n";



    bool is_rend_path = formSegBronchi_GetisRendPath();
    bool is_rend_tree = formSegBronchi_GetisRendTreeStructure();
    if (is_rend_path || is_rend_tree)
    {
      //path draw
      Q.push(m_roots[frame_idx]);
      while (!Q.empty())
      {
        TreeObj* piv_cp = Q.front();
        Q.pop();
        for (const auto& child_cp : piv_cp->GetChildren()) Q.push(child_cp);

        //path
        if (is_rend_path)
          for (const auto& paths : piv_cp->GetPathsToChildren())
            t_DrawPolyLine(EVec3f(0, 0, 1), TreeObj::GetPathWidth(), paths, false);
       
        //tree structure path
        if (is_rend_tree)
          for (const auto& child_cp : piv_cp->GetChildren()){
            std::vector<EVec3f> polyline = { piv_cp->GetPos(), child_cp->GetPos() };
            t_DrawPolyLine(EVec3f(1, 0, 0), TreeObj::GetPathWidth(), polyline, false);
          }
      }
    }
  }



  //draw cross section
  const EVec3i reso = ImageCore::GetInst()->GetReso();
  const bool b_xy = formVisParam_bPlaneXY();
  const bool b_yz = formVisParam_bPlaneYZ();
  const bool b_zx = formVisParam_bPlaneZX();
  m_crssec_shader.bind(0, 1, 2, 3, 6, reso, false, isSpaceKeyOn());
  CrssecCore::DrawCrssec(b_xy, b_yz, b_zx, cuboid);
  m_crssec_shader.unbind();
  if (DEBUG_DRAWSCENE) std::cout << "draw cross section後\n";


  //volume
  if (formVisParam_bRendVol())
  {
    const bool  b_pseudo = formVisParam_bDoPsued();
    const float alpha = formVisParam_getAlpha();
    const bool  b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int   num_slice = GetNumVolumeRenderSlice(b_onmanip);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    m_volume_shader.bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, b_pseudo, !isSpaceKeyOn());
    t_drawSlices(num_slice, camP, camF, cuboid);
    m_volume_shader.unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    if (DEBUG_DRAWSCENE) std::cout << "volume後\n";
  }

  if (DEBUG_DRAWSCENE) std::cout << ".........end drawScene\n";
}



void ModeSegBronchi::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;

  if (isCtrKeyOn())
  {
    m_stroke.clear();
    m_b_draw_cutstroke = true;
  }
  else if (isShiftKeyOn())
  {
    EVec3f ray_pos, ray_dir, pos;
    ogl->GetCursorRay(p, ray_pos, ray_dir);
    if (PickCrssec(ray_pos, ray_dir, pos) == CRSSEC_NON) return;

    const int frame_idx = formVisParam_getframeI();
    if (formSegBronchi_GetIsAddCpMode())      AddNewCp(frame_idx, pos);
    else if(formSegBronchi_GetIsMoveCpMode()) m_pick_cp = PickControlPoints(ray_pos, ray_dir);
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }

  formMain_RedrawMainPanel();
}


void ModeSegBronchi::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if (DEBUG_MODE) std::cout << "start LBtnU\n";
  const int frame_idx = formVisParam_getframeI();

  if ( m_b_draw_cutstroke )
  {
    EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();
    CrssecCore::CreateCurvedCrssec(cuboid, ogl->GetCamPos(), m_stroke);
  }

  if (m_pick_cp != nullptr)
  {
    std::cout << "LbtnUp\n";
    UpdatePath( frame_idx, m_pick_cp );
    m_pick_cp = nullptr;
  }


  m_b_draw_cutstroke = false;
  m_bL = false;
  ogl->BtnUp();
  
  formMain_RedrawMainPanel();

  if (DEBUG_MODE) std::cout << "end LBtnU\n";
}





void ModeSegBronchi::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;

  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);
  TreeObj *piv_cp = PickControlPoints(ray_pos, ray_dir);

  if (isShiftKeyOn() && isCtrKeyOn() &&piv_cp!= nullptr)
  {
    const int frame_idx = formVisParam_getframeI();
    DeleteCp(frame_idx, piv_cp);
  }
  else if (isShiftKeyOn() &&piv_cp!= nullptr)
  {
    //Add Cp Mode時、親切り替え
    if (formSegBronchi_GetIsAddCpMode()) m_selected_cp = piv_cp;
  }
  else
  {
    ogl->BtnDown_Rot(p);
  }
  formMain_RedrawMainPanel();

  if (DEBUG_MODE) std::cout << "end RBtnD\n";
}


void ModeSegBronchi::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_pick_cp = nullptr;
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeSegBronchi::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}


void ModeSegBronchi::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeSegBronchi::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegBronchi::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegBronchi::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


void ModeSegBronchi::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  const int frame_idx = formVisParam_getframeI();
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if (m_pick_cp != nullptr)
  {
    if (PickCrssec(ray_pos, ray_dir, pos) != CRSSEC_NON)
    {
      std::cout << "Mouse Move pos: " << pos[0] << pos[1] << pos[2] << "\n";
      m_pick_cp->SetPos(pos);
    }
  }
  else if (m_b_draw_cutstroke)
  {
    m_stroke.push_back(ray_pos + 0.1f * ray_dir);
  }
  else
  {
    ogl->MouseMove(p);
  }

  formMain_RedrawMainPanel();
}


void ModeSegBronchi::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  if(!WheelingCrssec(p, z_delta, ogl) ) 
  {
    ogl->ZoomCamera(z_delta * 0.1f);
  }
  formMain_RedrawMainPanel();
}


void ModeSegBronchi::keyDown(int nChar) 
{
  if (DEBUG_MODE) std::cout << "start keyD\n";

  //n key
  if (nChar == 'N')
  {
    const int num_frames = ImageCore::GetInst()->GetNumFrames();
    const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
    ChangeImg(num_frames, num_voxels);


    std::cout << "先鋭化可視化トグル\n";
  }

  if (DEBUG_MODE) std::cout << "end keyD\n";
}


void ModeSegBronchi::keyUp(int nChar) {}



void ModeSegBronchi::UpControlPointSize()
{
  TreeObj::SetRadius(TreeObj::GetRadius() * 2);
  std::cout << TreeObj::GetRadius() << "\n";
  m_cp_mesh.InitializeIcosaHedron(TreeObj::GetRadius());
  formMain_RedrawMainPanel();
}


void ModeSegBronchi::DownControlPointSize()
{
  float r = TreeObj::GetRadius() / 2.0f;
  if (r != 0.0f)
  {
    std::cout << "r " << r << "\n";
    TreeObj::SetRadius(r);
  }
  m_cp_mesh.InitializeIcosaHedron(TreeObj::GetRadius());
  formMain_RedrawMainPanel();
}


void ModeSegBronchi::UpPathWidth()
{
  TreeObj::SetPathWidth(TreeObj::GetPathWidth() * 2);
  std::cout << "width " << TreeObj::GetPathWidth() << "\n";
  formMain_RedrawMainPanel();
}


void ModeSegBronchi::DownPathWidth()
{
  float w = TreeObj::GetPathWidth() / 2.0f;
  if (w >= 1.0f)
  {
    std::cout << "width " << w << "\n";
    TreeObj::SetPathWidth(w);
  }
  formMain_RedrawMainPanel();
}



void ModeSegBronchi::IntializeVarState(const int num_frames, const int num_voxels)
{
  if (DEBUG_MODE) std::cout << "start initializeVarState\n";
  m_bL = m_bR = m_bM = false;
  m_b_draw_cutstroke = false;
  m_pick_cp = nullptr;
  m_selected_cp = nullptr;

  for (auto r : m_roots) 
    if (r != nullptr) delete r;
  m_roots.clear();
  m_roots.resize(num_frames, nullptr);

  if (DEBUG_MODE) std::cout << "end initializeVarState\n";
}


void ModeSegBronchi::InitCenteredness(const int num_frames, const int num_voxels, const EVec3i reso)
{
  //元画像利用
  //vector< short* >& img4d = t_dst4d;
  //先鋭画像利用
  std::vector< short* >& img4d = ImageCore::GetInst()->m_img4d;
  
  m_min.resize(num_frames);
  m_max_m_min.resize(num_frames);

  clock_t start = clock();

  for (int frame_idx = 0; frame_idx < num_frames; ++frame_idx)
  {
    std::cout << "\nframe_idx " << frame_idx << "   centerliness computing\n";
    m_min[frame_idx]       = new short[num_voxels];
    m_max_m_min[frame_idx] = new short[num_voxels];

    short* img3d = img4d[frame_idx];
    short* min3d = m_min[frame_idx];
    memset(min3d, 0, sizeof(short) * num_voxels);
    short* max_m_min = m_max_m_min[frame_idx];
    memset(max_m_min, 0, sizeof(short) * num_voxels);


    int Z = reso[2];
    int Y = reso[1];
    int X = reso[0];
    byte* vol_lock = ImageCore::GetInst()->m_mask4d[frame_idx];
  
    for (int z = 2; z < Z - 2; ++z)
    {
      if (z % 10 == 0) std::cout << "*";
      for (int y = 2; y < Y - 2; ++y)
      {
        for (int x = 2; x <= X - 2; ++x)
        {
          short t_max = SHRT_MIN, t_min = SHRT_MAX;
          const int ctr_idx = x + y * reso[0] + z * reso[0] * reso[1];
          if (vol_lock[ctr_idx] == 0) continue;

          for (int zi = -2; zi <= 2; ++zi)
          for (int yi = -2; yi <= 2; ++yi)
          for (int xi = -2; xi <= 2; ++xi)
          {
            const int adj_idx = ctr_idx + xi + yi * reso[0] + zi * reso[0] * reso[1];
            if (t_max < img3d[adj_idx]) t_max = img3d[adj_idx];
            if (t_min > img3d[adj_idx]) t_min = img3d[adj_idx];
          }
          min3d[ctr_idx] = t_min;
          max_m_min[ctr_idx] = t_max - t_min;
        }
      }
    }
  }


  clock_t end = clock();
  clock_t sec = (end - start) / CLOCKS_PER_SEC;
  clock_t minute = sec / 60;
  std::cout << "\ntotal centerliness computing time : " << minute << "分" << sec % 60 << "秒\n";

}



void ModeSegBronchi::LoGFilter(const int ksize, const double sigma)
{
  if (DEBUG_MODE) std::cout << "start LoGFilter\n";
  clock_t start_pre_filter = clock();

  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
  std::vector<short*>& img4d = ImageCore::GetInst()->m_img4d;

  t_dst4d.resize(num_frames);
  for (int frame_idx = 0; frame_idx < num_frames; ++frame_idx)
  {
    short* img3d = img4d[frame_idx];
    t_dst4d[frame_idx] = new short[num_voxels];
    short* dst3d = t_dst4d[frame_idx];
    for (int voxel_idx = 0; voxel_idx < num_voxels; ++voxel_idx) dst3d[voxel_idx] = img3d[voxel_idx];
  }
  std::cout << "LoGFilterコピー後\n";

  //LoGカーネル作成
  //sigma=0.48
  //vector<int> elem{ 0, 0, 0, 0, 0, 1, 1, 2, 4, -62 };
  //sigma=0.54
  //vector<int> elem{ 0, 0, 0, 0, 0, 1, 1, 2, 1, -44 };
  //sigma=0.55
  //vector<int> elem{ 0, 0, 0, 0, 0, 1, 1, 1, 1, -32 };
  //sigma=0.50
  //vector<int> elem{ 0, 0, 0, 0, 0, 0, 1, 2, 3, -50 };
  //sigma=0.63
  //vector<int> elem{ 0, 0, 0, 0, 0, 1, 1, 1, -1, -20 };
  //sigma=0.73
  //vector<int> elem{ 0, 0, 0, 0, 0, 1, 1, 1, -3, -8 };
  //sigma=1.0
  std::vector<int> elem{ 0, 0, 0, 0, 1, 1, 0, -1, -2, -6 };

  std::vector<std::vector<std::vector<int>>> kernel
  { {{elem[0], elem[1], elem[2], elem[1], elem[0]},
     {elem[1], elem[3], elem[4], elem[3], elem[1]},
     {elem[2], elem[4], elem[5], elem[4], elem[2]},
     {elem[1], elem[3], elem[4], elem[3], elem[1]},
     {elem[0], elem[1], elem[2], elem[1], elem[0]}},
    {{elem[1], elem[3], elem[4], elem[3], elem[1]},
     {elem[3], elem[6], elem[7], elem[6], elem[3]},
     {elem[4], elem[7], elem[8], elem[7], elem[4]},
     {elem[3], elem[6], elem[7], elem[6], elem[3]},
     {elem[1], elem[3], elem[4], elem[3], elem[1]}},
    {{elem[2], elem[4], elem[5], elem[4], elem[2]},
     {elem[4], elem[7], elem[8], elem[7], elem[4]},
     {elem[5], elem[8], elem[9], elem[8], elem[5]},
     {elem[4], elem[7], elem[8], elem[7], elem[4]},
     {elem[2], elem[4], elem[5], elem[4], elem[2]}},
    {{elem[1], elem[3], elem[4], elem[3], elem[1]},
     {elem[3], elem[6], elem[7], elem[6], elem[3]},
     {elem[4], elem[7], elem[8], elem[7], elem[4]},
     {elem[3], elem[6], elem[7], elem[6], elem[3]},
     {elem[1], elem[3], elem[4], elem[3], elem[1]}},
    {{elem[0], elem[1], elem[2], elem[1], elem[0]},
     {elem[1], elem[3], elem[4], elem[3], elem[1]},
     {elem[2], elem[4], elem[5], elem[4], elem[2]},
     {elem[1], elem[3], elem[4], elem[3], elem[1]},
     {elem[0], elem[1], elem[2], elem[1], elem[0]}} };


  for (int z = 0; z < ksize; ++z)
  {
    for (int y = 0; y < ksize; ++y)
    {
      for (int x = 0; x < ksize; ++x)
      {
        std::cout << kernel[z][y][x] << " ";
      }
      std::cout << "\n";
    }
    std::cout << "-----------------------------------\n";
  }


  //LoGフィルタ処理
  const EVec3i reso = ImageCore::GetInst()->GetReso();
  const int X = reso[0];
  const int Y = reso[1];
  const int Z = reso[2];
  for (int frame_idx = 0; frame_idx < num_frames; ++frame_idx)
  {
    short* img3d = img4d[frame_idx];
    short* dst3d = t_dst4d[frame_idx];
    //画像ラスタスキャン
    for (int z = ksize / 2; z < Z - ksize / 2; ++z)
    {
      for (int y = ksize / 2; y < Y - ksize / 2; ++y)
      {
        for (int x = ksize / 2; x < X - ksize / 2; ++x)
        {
          //畳み込み計算
          double t_LoG = 0;
          for (int zi = -ksize / 2; zi <= ksize / 2; zi++)
          {
            for (int yi = -ksize / 2; yi <= ksize / 2; yi++)
            {
              for (int xi = -ksize / 2; xi <= ksize / 2; xi++)
              {
                int Ki = (x + xi) + (y + yi) * X + (z + zi) * X * Y;
                double t_intensity = img3d[Ki] * kernel[ksize / 2 + zi][ksize / 2 + yi][ksize / 2 + xi];

                t_LoG += t_intensity;
              }
            }
          }
          //double omega = (t_LoG > 0) ? (0.05) : (0.3);
          double omega = (t_LoG > 0) ? (0.05) : (0.5);
          double wLoG = omega * t_LoG;
          int K = x + y * X + z * X * Y;

          dst3d[K] = (short)(img3d[K] - wLoG);
        }
      }
    }
    std::cout << "compute " << frame_idx << " frame LoG filter\n";
  }


  clock_t end_pre_filter = clock();
  clock_t sec_pre_filter = (end_pre_filter - start_pre_filter) / CLOCKS_PER_SEC;
  clock_t minute_pre_filter = sec_pre_filter / 60;
  std::cout << "\npre filter computing time : " << minute_pre_filter << "分" << sec_pre_filter % 60 << "秒\n";


  const int fi = formVisParam_getframeI();
  const int winlv_min = formVisParam_getWinLvMin();
  const int winlv_max = formVisParam_getWinLvMax();
  ImageCore::GetInst()->UpdateVisVolume(winlv_min, winlv_max, fi);

  formMain_RedrawMainPanel();

  if (DEBUG_MODE) std::cout << "end LoGFilter\n";
}


void ModeSegBronchi::ChangeImg(const int num_frames, const int num_voxels)
{
  std::vector<short*>& img4d = ImageCore::GetInst()->m_img4d;
  for (int frame_idx = 0; frame_idx < num_frames; ++frame_idx)
  {
    short* img3d = img4d[frame_idx];
    short* dst3d = t_dst4d[frame_idx];
    for (int voxel_idx = 0; voxel_idx < num_voxels; ++voxel_idx)
    {
      short tmp = img3d[voxel_idx];
      img3d[voxel_idx] = dst3d[voxel_idx];
      dst3d[voxel_idx] = tmp;
    }
  }
  const int fi = formVisParam_getframeI();
  const int winlv_min = formVisParam_getWinLvMin();
  const int winlv_max = formVisParam_getWinLvMax();
  ImageCore::GetInst()->UpdateVisVolume(winlv_min, winlv_max, fi);
}






TreeObj* ModeSegBronchi::PickControlPoints(
  const EVec3f& ray_pos,
  const EVec3f& ray_dir)
{
  const int frame_idx = formVisParam_getframeI();

  if (m_roots[frame_idx] == nullptr)
    return nullptr;

  std::queue<TreeObj*> Q;
  Q.push(m_roots[frame_idx]);

  while (!Q.empty())
  {
    TreeObj* piv = Q.front();
    Q.pop();

    // TODO Planeの座標が取れたら、Plane付近・手前のみ掴めるようにしたい（カメラPosとPlanePos？の間なら、掴める）
    if (t_DistRayAndPoint(ray_pos, ray_dir, piv->GetPos()) < TreeObj::GetRadius())
      return piv;

    for (const auto& child_cp : piv->GetChildren())
      Q.push(child_cp);
  }
  return nullptr;
}





void ModeSegBronchi::SaveCp(std::string fname)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int frame_digit = (int)std::to_string(num_frames).length();
  const EVec3i reso = ImageCore::GetInst()->GetReso();
  const EVec3i int_dgt = EVec3i((int)std::to_string(reso[0]).length(),
                                (int)std::to_string(reso[1]).length(),
                                (int)std::to_string(reso[2]).length());
  const int frac_dgt = 3;

  std::ofstream ofs(fname.c_str());
  if (!ofs.is_open())
  {
    std::cout << "error when opening " << fname << "\n";
    return;
  }

  for (int frm = 0; frm < num_frames; ++frm)
  {
    ofs << "frame_idx " << frm << "\n";
    TreeObj *root = m_roots[frm];
    int cp_cnt = 0;

    if (root == nullptr ) {
      ofs << "0\n";
      std::cout <<"frame " << frm << ", cp num " << cp_cnt << "\n";
      continue;
    }

    ofs << "1\n";

    std::queue<TreeObj*> Q;
    Q.push(root);

    while (!Q.empty())
    {
      TreeObj*piv_cp= Q.front();
      Q.pop();
      cp_cnt++;

      ofs << std::setw(frame_digit) << std::setfill(' ') << std::left << (int)piv_cp->GetChildren().size() << " ";
      auto p = piv_cp->GetPos();
      ofs << std::setw(int_dgt[0] + frac_dgt + 1) << std::setfill(' ') << std::fixed << std::setprecision(frac_dgt) << p.x() << " "
          << std::setw(int_dgt[1] + frac_dgt + 1) << std::setfill(' ') << std::fixed << std::setprecision(frac_dgt) << p.y() << " "
          << std::setw(int_dgt[2] + frac_dgt + 1) << std::setfill(' ') << std::fixed << std::setprecision(frac_dgt) << p.z() << "\n";

      for (const auto& child_cp : piv_cp->GetChildren()) Q.push(child_cp);
    }
    std::cout << "frame " << frm << ", cp num " << cp_cnt << "\n";
  }
  std::cout << "save終了\n";
}



void ModeSegBronchi::LoadCp(std::string fname)
{ 
  const int num_frames = ImageCore::GetInst()->GetInst()->GetNumFrames();
  
  for (int f = 0; f < num_frames; ++f)
  {
    if (m_roots[f] != nullptr)
      delete m_roots[f];
    m_roots[f] = nullptr;
  }
    
  std::ifstream ifs(fname.c_str());

  if (ifs.fail())
  {
    std::cout << "error when opening " << fname << "\n";
    return;
  }

  char buf[1024];
  int  tmp_fidx, tmp_tf;
  for (int frm = 0; frm < num_frames; ++frm)
  {
    ifs >> buf >> tmp_fidx >> tmp_tf;
    std::cout << buf << " " << tmp_fidx << " " << tmp_tf << "\n";

    if (tmp_tf == 0) continue;
    
    m_roots[frm] = new TreeObj(frm, nullptr);
     
    std::queue<TreeObj*> Q;
    Q.push(m_roots[frm]);

    while (!Q.empty())
    {
      TreeObj*piv_cp= Q.front();
      Q.pop();
          
      int num_ch;
      ifs >> num_ch;
      for (int i = 0; i < num_ch; ++i)
      {
        TreeObj* c = new TreeObj(frm, piv_cp);
        piv_cp->AddChild(c);
        Q.push(c);
      }

      float x,y,z;
      ifs >> x >> y >> z;
      piv_cp->SetPos( EVec3f(x, y, z) );
    }
  }

  ComputePathAllFrameAllCp();
}



void ModeSegBronchi::AddNewCp(const int frame_idx, const EVec3f pos)
{
  //root  
  if (m_roots[frame_idx] == nullptr)
  {
    std::cout << "root \n";
    m_roots[frame_idx] = new TreeObj(frame_idx, nullptr, pos);
    m_selected_cp = m_roots[frame_idx];
    m_pick_cp = m_roots[frame_idx];
    return;
  }

  //child
  if (m_selected_cp == nullptr || m_selected_cp->GetFrameIdx() != frame_idx)
  {
    std::cout << "親CPを選択してください\n";
    return;
  }
 
  //non root
  TreeObj * child = new TreeObj(frame_idx, m_selected_cp, pos);
  m_selected_cp->AddChild(child);
  m_pick_cp = child;
}



void ModeSegBronchi::DeleteCp(const int frame_idx, TreeObj* cp)
{
  if (cp->GetParent() == nullptr)
  {
    m_roots[frame_idx] = nullptr;
    delete cp;
  }
  else
  {
    cp->GetParent()->DeleteChild(cp);
  }
  m_selected_cp = nullptr;
  m_pick_cp = nullptr;
  std::cout << "Delete Cp\n";
}




void ModeSegBronchi::CopyPrevFrameCp()
{
  if (DEBUG_MODE) std::cout << "start copyPrevFrameCP\n";

  clock_t start = clock();


  const int frame_idx = formVisParam_getframeI();

  if (m_roots[frame_idx] != nullptr)
    delete m_roots[frame_idx];
  m_roots[frame_idx] = nullptr;


  std::queue<TreeObj*> Q_pre, Q_piv;
  m_roots[frame_idx] = new TreeObj(frame_idx, nullptr, m_roots[frame_idx - 1]->GetPos());
  Q_pre.push(m_roots[frame_idx - 1]); Q_piv.push(m_roots[frame_idx]);
  while (!Q_pre.empty() && !Q_piv.empty())
  {
    TreeObj* pre_piv_cp = Q_pre.front();
    TreeObj* piv_cp = Q_piv.front();
    Q_pre.pop(); Q_piv.pop();

    for (const auto& pre_child : pre_piv_cp->GetChildren())
    {
      TreeObj* child = new TreeObj(frame_idx, piv_cp, pre_child->GetPos());
      piv_cp->AddChild(child);
      Q_pre.push(pre_child); Q_piv.push(child);
    }
  }

  ComputePathOneFrameAllCp(frame_idx);

  clock_t end = clock();
  clock_t sec = (end - start) / CLOCKS_PER_SEC;
  clock_t minute = sec / 60;
  std::cout << "\ncopy time : " << minute << "分" << sec % 60 << "秒\n";

  if (DEBUG_MODE) std::cout << "end copyPrevFrameCP\n";
}





/////////////////////////////////////////////////////////////////////
//SHORTEST PATH ALGORITHM////////////////////////////////////////////


class ShortestPathGraph
{
public:
  unsigned short* m_dist;
  byte*           m_flg; // 0未訪問, 1訪問中, 2確定 0で初期化
  int*            m_from;
  int             m_num;

  ShortestPathGraph() {
    m_dist = nullptr;
    m_flg  = nullptr;
    m_from = nullptr;
    m_num  = 0;
  }

  void Set(const ShortestPathGraph& src)
  {
    Initialize(src.m_num);
    memcpy(m_dist, src.m_dist, sizeof(unsigned short) * m_num);
    memcpy(m_flg , src.m_flg , sizeof(byte)           * m_num);
    memcpy(m_from, src.m_from, sizeof(int)            * m_num);
    std::cout << sizeof(unsigned short) << " " << sizeof(byte) << " " << sizeof(int) << "\n";
  }

  ShortestPathGraph(const ShortestPathGraph& src) { Set(src); }
  ShortestPathGraph& ShortestPathGraph::operator=(const ShortestPathGraph& src) { Set(src); return *this;}

  ~ShortestPathGraph() {
    if (m_dist != nullptr) delete[] m_dist;
    if (m_flg  != nullptr) delete[] m_flg;
    if (m_from != nullptr) delete[] m_from;
    m_dist = nullptr;
    m_flg  = nullptr;
    m_from = nullptr;
  }


  void Initialize(int n) {
    if (m_dist != nullptr) delete[] m_dist;
    if (m_flg  != nullptr) delete[] m_flg;
    if (m_from != nullptr) delete[] m_from;
    m_dist = new unsigned short[n];
    m_flg  = new byte[n];
    m_from = new int[n];
    memset(m_dist, 0, sizeof(unsigned short) * n);
    memset(m_flg , 0, sizeof(byte)           * n);
    memset(m_from, 0, sizeof(int)            * n);
    m_num = n;
  }


  void SetValueForAll(const unsigned short dist, const byte flg, const int from)
  {
    for (int i = 0; i < m_num; ++i)
    {
      m_dist[i] = dist;
      m_flg[i]  = flg;
      m_from[i] = from;
    }
  }

  std::vector<EVec3f> backTrack(
    int   W, int H, int D,
    int   ix, int iy, int iz,
    float px, float py, float pz)
  {
    std::vector<EVec3f> path;

    int p_index = m_from[ix + iy * W + iz * W * H];
    while (p_index != -1)
    {
      int z = p_index / (W * H);
      int y = p_index / W - z * H;
      int x = p_index % W;
      path.push_back(EVec3f( (x + 0.5f) * px, (y + 0.5f) * py, (z + 0.5f) * pz));
      p_index = m_from[p_index];
    }
    return path;
  }




  std::vector<EVec3f> ComputeShortestPath(
      const EVec3i& reso,
      const EVec3f& pitch,
      const short* img3d,
      const short* min3d,
      const short* max_m_min_3d,
      const byte*  vol_lock,
      EVec3f pos_start,
      EVec3f pos_end)
  {
    int vox_idx_start = ImageCore::GetInst()->GetVoxelIndex(pos_start);
    int vox_idx_end   = ImageCore::GetInst()->GetVoxelIndex(pos_end  );

    if (vol_lock[vox_idx_start] == 0 || vol_lock[vox_idx_end] == 0)
    {
      std::cout << "マスク外のため、計算スキップ\n";
      return std::vector<EVec3f>();
    }

    const int W = reso[0];
    const int H = reso[1];
    const int D = reso[2];
    this->SetValueForAll(SHRT_MAX, 0, -1);

    //(dist, voxel_idx) 
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>> > q;
    

    //1 set start point
    m_dist[vox_idx_start] = 0;
    m_flg[vox_idx_start] = 1;
    m_from[vox_idx_start] = -1;
    q.push(std::make_pair(m_dist[vox_idx_start], vox_idx_start) );


    //最短路計算
    while (!q.empty())
    {
      //最短距離ノード確定＋flg更新
      const int piv_idx = q.top().second;
      q.pop();
      if ( m_flg[piv_idx]     == 2 ) continue;
      if ( m_flg[vox_idx_end] == 2 ) break;

      m_flg[piv_idx] = 2;

      //index->xyz if()  true->index
      int zidx = piv_idx / (W * H);
      int yidx = piv_idx / W - zidx * H;
      int xidx = piv_idx % W;

      //26近傍で計算, neighbor(v_index)

      for (int zi = std::max(0, zidx - 1), z_min = std::min(zidx + 1, D - 1); zi <= z_min; ++zi)
      for (int yi = std::max(0, yidx - 1), y_min = std::min(yidx + 1, H - 1); yi <= y_min; ++yi)
      for (int xi = std::max(0, xidx - 1), x_min = std::min(xidx + 1, W - 1); xi <= x_min; ++xi)
      {
        int idx = xi + yi * W + zi * W * H;

        //mask外 or pivot自身 or すでに確定済み
        if (vol_lock[idx] == 0 || idx == piv_idx || m_flg[idx] == 2) continue;


        //https://www.cse.wustl.edu/~taoju/research/interactive.pdf
        //中心らしさ参考論文
        //float centeredness = (max_m_min_3d[piv_idx] == 0) ? 0 : (float)(100 * (img3d[piv_idx] - min3d[piv_idx]) / max_m_min_3d[piv_idx]);
        float centeredness = (max_m_min_3d[piv_idx] == 0) ? 0 : (float)(100 * fabs(img3d[piv_idx] - img3d[idx]) / (max_m_min_3d[piv_idx]));

        //EdgeCost計算
        float newdist = m_dist[piv_idx]
                      + centeredness
                      + (float)(100 * sqrt( (xidx-xi) * (xidx - xi) + (yidx - yi) * (yidx - yi) + (zidx - zi) * (zidx - zi)));
        if (zi % 200 == 0 && centeredness < 0)
        {
          std::cout << "centeredness : " << centeredness << "\n";
          std::cout << "distance     : " << (float)(100 * sqrt((xidx - xi) * (xidx - xi) + (yidx - yi) * (yidx - yi) + (zidx - zi) * (zidx - zi))) << "\n";
          std::cout << "(min3d[piv_idx] - img3d[piv_idx]) : " << (min3d[piv_idx] - img3d[piv_idx]) << "\n";
          std::cout << "(min3d[piv_idx]                 ) : " << min3d[piv_idx] << "\n";
          std::cout << "(                 img3d[piv_idx]) : " << img3d[piv_idx] << "\n";
          std::cout << "max_m_min_3d[piv_idx]             : " << max_m_min_3d[piv_idx] << "\n";
          std::cout << "-------\n";
        }


        if (m_flg[idx] == 0)
        {
          //未訪問
          m_dist[idx] = (unsigned short) newdist;
          m_flg[idx] = 1;
          m_from[idx] = piv_idx;
          q.push(std::make_pair(newdist, idx));
        }
        else if (m_dist[idx] > newdist)
        {
          //訪問中
          m_dist[idx] = (unsigned short) newdist;
          m_from[idx] = piv_idx;
          q.push(std::make_pair(newdist, idx));
        }
      }
    }

    int iz = vox_idx_end / (W * H);
    int iy = vox_idx_end / W - iz * H;
    int ix = vox_idx_end % W;
    auto path = backTrack(W, H, D, ix, iy, iz, pitch[0], pitch[1], pitch[2]);
    path.push_back(pos_start);
    path.insert(path.begin(), pos_end);
    return path;
  }
};










void ModeSegBronchi::UpdatePath(const int frame_idx, TreeObj* trgt_cp, bool path_to_parent)
{
  EVec3i reso  = ImageCore::GetInst()->GetReso();
  EVec3f pitch = ImageCore::GetInst()->GetPitch();

  //元画像利用
  short* t_dst3d = t_dst4d[frame_idx];
  byte*    msk3d = ImageCore::GetInst()->m_mask4d[frame_idx];
  short*   min3d = m_min[frame_idx];
  short* max_m_min_3d = m_max_m_min[frame_idx];

  //0 gen graph
  ShortestPathGraph graph;
  graph.Initialize(reso[0] * reso[1] * reso[2]);

  TreeObj* parent = trgt_cp->GetParent();
  if (parent != nullptr && path_to_parent)
  {
    //元画像利用
    auto path = graph.ComputeShortestPath(reso, pitch, t_dst3d, min3d, max_m_min_3d, msk3d,
                                          parent->GetPos(), trgt_cp->GetPos() );

    parent->SetPathToChild(trgt_cp, path);
  }

  for (auto c : trgt_cp->GetChildren())
  {
    auto path = graph.ComputeShortestPath(reso, pitch, t_dst3d, min3d, max_m_min_3d, msk3d,
                                          trgt_cp->GetPos(), c->GetPos() );

    trgt_cp->SetPathToChild(c, path);
  }
}



void ModeSegBronchi::ComputePathOneFrameAllCp(const int frame_idx)
{
  std::cout << "ComputePathOneFrameAllCp\n";
  if (m_roots[frame_idx] == nullptr) return;

  std::queue<TreeObj*> Q;
  Q.push(m_roots[frame_idx]);
  while (!Q.empty())
  {
    TreeObj* piv_cp = Q.front();
    Q.pop();
    //cout << "cp adress " << piv_cp << "\n";

    for (const auto& child_cp : piv_cp->GetChildren())
    {
      Q.push(child_cp);
    }

    UpdatePath(frame_idx, piv_cp, false);
  }

  formMain_RedrawMainPanel();
}


void ModeSegBronchi::ComputePathAllFrameAllCp()
{
  for (int frame_idx = 0; frame_idx < m_roots.size(); ++frame_idx)
  {
    std::cout << frame_idx << " frame is computing\n";
    ComputePathOneFrameAllCp(frame_idx);
    std::cout << frame_idx << " frame is computed\n";
  }
}


template<class T, class M>
static bool t_inRange(const M& num, const T& min, const T& max) {
  if (min > num) return false;
  if (max < num) return false;
  return true;
}


class ElemQ
{
public:
  float m_x;
  float m_y;
  float m_z;
  int m_K;
  int m_age;
  bool m_is_former;

  ElemQ(float x, float y, float z, int K, int age, bool is_former)
  {
    m_x = x;
    m_y = y;
    m_z = z;
    m_K = K;
    m_age = age;
    m_is_former = is_former;
  }
};


void ModeSegBronchi::RunOneFrameRegionGrow(
    const EVec3i& thresh_minmax_fl, 
    const int frame_idx, 
    int former_age, int latter_age, 
    int Growth_width_layer, int RGrow_layer)
{
  if (DEBUG_MODE) std::cout << "start RunPathRegionGrow\n";

  //1フレームごとに領域分割する
  //元画像利用
  short* img3d = t_dst4d[frame_idx];
  //先鋭画像利用
  //short* img3d = ImageCore::GetInst()->m_img4d[frame_idx];


  byte*  flg3d = ImageCore::GetInst()->m_flg4d[frame_idx];
  const EVec3i reso = ImageCore::GetInst()->GetReso();
  const int W  = reso[0];
  const int H  = reso[1];
  const int D  = reso[2];
  const int WH = W * H;
  const int WHD = W * H * D;
  const int v_min = thresh_minmax_fl[0];
  const int v_max_former = thresh_minmax_fl[1];
  const int v_max_latter = thresh_minmax_fl[2];
  std::cout << "v_max_former = " << v_max_former << "v_max_latter = " << v_max_latter << "\n";

  for (int i = 0; i < WHD; ++i) flg3d[i] = 1;


  if (m_roots[frame_idx] == nullptr)
    return;


  std::queue<ElemQ> Q_seeds;
  std::queue<std::pair<TreeObj*, int>>Q_cps; //auto [piv_cp, num_layer] 
  int init_layer_idx = 1;
  m_roots[frame_idx]->SetAge(former_age);
  Q_cps.push(std::make_pair(m_roots[frame_idx], init_layer_idx));
  while (!Q_cps.empty())
  {
    TreeObj* piv_cp = Q_cps.front().first;
    int layer_idx = Q_cps.front().second;
    Q_cps.pop();

    for (const auto& child_cp : piv_cp->GetChildren())
    {
      int child_age = (layer_idx < Growth_width_layer) ? std::max(1, (int)(piv_cp->GetAge() / sqrt(2))) : latter_age;
      child_cp->SetAge(child_age);
      Q_cps.push(std::make_pair(child_cp, layer_idx + 1));
    }

    for (const auto& path : piv_cp->GetPathsToChildren())
    {
      const int N = (int)path.size();
      std::cout << "path size : " << N << "\n";
      int parent_age = piv_cp->GetAge();
      int child_age = (layer_idx < Growth_width_layer) ? std::max(1, (int)(piv_cp->GetAge() / sqrt(2))) : latter_age;

      for (int id = N - 1; id >= 0; --id)
      {
        //backTrackで(子->親)の順に格納されているのを(N - 1 -> 0)で(親->子)の順で処理
        const int pathVoxel_idx = ImageCore::GetInst()->GetVoxelIndex(path[id]);
        float t = (float)id / N;
        int age_seed = (int)( parent_age * t + child_age * (1 - t));
        //cout << "age_seed " << age_seed << "\n";

        flg3d[pathVoxel_idx] = 255;
        Q_seeds.push(ElemQ(path[id][0], path[id][1], path[id][2], pathVoxel_idx, age_seed, layer_idx <= RGrow_layer));
        if (id == 0)
        {
          std::cout << "(layer_idx    " << layer_idx << "\n";
          std::cout << "latter_layerB " << RGrow_layer << "\n";

          std::cout << "(layer_idx <= RGrow_layer) " << (layer_idx <= RGrow_layer) << "\n";
        }
      }
    }
  }

  //region growing
  while (!Q_seeds.empty())
  {
    const ElemQ it = Q_seeds.front();

    const float    x = it.m_x;
    const float    y = it.m_y;
    const float    z = it.m_z;
    const int  index = it.m_K;
    int age          = it.m_age;
    bool is_former   = it.m_is_former;
    Q_seeds.pop();
    
    //6近傍分
    int K;
    
    if (age == 0) continue;
    const int v_max = is_former ? v_max_former : v_max_latter;
    K = index - 1;  if (x > 0     && flg3d[K] == 1 && t_inRange(img3d[K], v_min, v_max)) { flg3d[K] = 255; Q_seeds.push(ElemQ(x - 1, y    , z    , K, age - 1, is_former)); }
    K = index - W;  if (y > 0     && flg3d[K] == 1 && t_inRange(img3d[K], v_min, v_max)) { flg3d[K] = 255; Q_seeds.push(ElemQ(x    , y - 1, z    , K, age - 1, is_former)); }
    K = index - WH; if (z > 0     && flg3d[K] == 1 && t_inRange(img3d[K], v_min, v_max)) { flg3d[K] = 255; Q_seeds.push(ElemQ(x    , y    , z - 1, K, age - 1, is_former)); }
    K = index + 1;  if (x < W - 1 && flg3d[K] == 1 && t_inRange(img3d[K], v_min, v_max)) { flg3d[K] = 255; Q_seeds.push(ElemQ(x + 1, y    , z    , K, age - 1, is_former)); }
    K = index + W;  if (y < H - 1 && flg3d[K] == 1 && t_inRange(img3d[K], v_min, v_max)) { flg3d[K] = 255; Q_seeds.push(ElemQ(x    , y + 1, z    , K, age - 1, is_former)); }
    K = index + WH; if (z < D - 1 && flg3d[K] == 1 && t_inRange(img3d[K], v_min, v_max)) { flg3d[K] = 255; Q_seeds.push(ElemQ(x    , y    , z + 1, K, age - 1, is_former)); }
  }

  std::cout << "Finish Region Grow\n";

  UpdateImageCoreVisVolumes();

  if (frame_idx == formVisParam_getframeI()) formMain_RedrawMainPanel();


  if (DEBUG_MODE) std::cout << "end RunPathRegionGrow\n";

}

