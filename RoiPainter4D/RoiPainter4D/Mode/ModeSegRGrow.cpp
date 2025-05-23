#pragma unmanaged
#include "ModeSegRGrow.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "tqueue.h"
#include "tmath.h"

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegRGrow.h"
#include "CliMessageBox.h"
#pragma unmanaged

using namespace RoiPainter4D;


ModeSegRGrow::ModeSegRGrow() 
{
  std::cout << "ModeSegRGrow...\n";
  m_bL = m_bR = m_bM = false;
  m_b_draw_cutstroke = m_b_modified = false;
  std::cout << "ModeSegRGrow DONE\n";
}



ModeSegRGrow::~ModeSegRGrow()
{
}



bool ModeSegRGrow::CanEndMode()
{
  if (!m_b_modified) return true;
  return ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Leaving?");
}



void ModeSegRGrow::FinishSegmentation()
{
  if (!bForeVoxelExist_flg4())
  {
    ShowMsgDlg_OK(MESSAGE_NO_FOREGROUND, "no foreground");
    return;
  }

  //このタイミングでfalseにしないとcanEndModeでダイアログが表示される
  m_b_modified = false;

  ImageCore::GetInst()->mask_storeCurrentForeGround(  );
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  formMain_RedrawMainPanel();
}



void ModeSegRGrow::cancelSegmentation()
{
  if (ShowMsgDlgYesNo("現在の分割結果を破棄してよいですか\nDo you want to cancel current segmentation?", "cancel?"))
  {
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    formMain_RedrawMainPanel();
  }
}


void ModeSegRGrow::StartMode()
{
  //initialize field 
  m_bL = m_bR = m_bM = false;
  m_b_draw_cutstroke = m_b_modified = false;
  m_drag_cpid = -1;

  m_cps.clear();
  m_cp_radius = 2 * ImageCore::GetInst()->GetPitch().x();

  //initialize vFlg
  ImageCore::GetInst()->InitializeFlg4dByMask( formMain_SetProgressValue );
  
  //show dialog
  formSegRGrow_Show();
  EVec2i minmax = ImageCore::GetInst()->GetVolumeMinMax();
  formSegRGrow_InitAllItems(minmax[0], minmax[1]);

  //4D volume (cpu) --> vis volume (gpu)
  UpdateImageCoreVisVolumes();
}



////////////////////////////////////////////////////////////////////////////
//MouseListener/////////////////////////////////////////////////////////////
void ModeSegRGrow::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;

  if (IsCtrKeyOn())
  {
    m_stroke.clear();
    m_b_draw_cutstroke = true;
  }
  else if (IsShiftKeyOn()) 
  {
    EVec3f ray_pos, ray_dir, pos;
    ogl->GetCursorRay(p, ray_pos, ray_dir);
    int pick_cpid = PickControlPoints(ray_pos, ray_dir);

    if (pick_cpid != -1 )
    {
      m_drag_cpid = pick_cpid;
    }
    else if ( PickCrssec( ray_pos, ray_dir, pos) != CRSSEC_NON)  //　Cp配置
    {
      const int frame_idx = formVisParam_getframeI();
      m_cps.push_back( CtrlPt4D(frame_idx, pos) );
      m_drag_cpid  = (int)(m_cps.size()) - 1;
    }
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }
  formMain_RedrawMainPanel();

}



void ModeSegRGrow::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if (m_b_draw_cutstroke)
  {
    EVec3f cube = ImageCore::GetInst()->GetCuboidF();
    CrssecCore::CreateCurvedCrssec(cube, ogl->GetCamPos(), m_stroke);
  }

  m_drag_cpid = -1;
  m_b_draw_cutstroke = false;
  m_bL = false;
  ogl->BtnUp();

  formMain_RedrawMainPanel();
}


void ModeSegRGrow::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;

  if (IsShiftKeyOn()) 
  {
    EVec3f rayP, rayD, pos;
    ogl->GetCursorRay(p, rayP, rayD);
    int pickCpI = PickControlPoints(rayP, rayD);

    if (pickCpI != -1) // Cpがあれば
    {
      m_cps.erase( m_cps.begin() + pickCpI ); // 消去
    }
  }
  else 
  {
    ogl->BtnDown_Rot(p);
  }
  formMain_RedrawMainPanel();
}


void ModeSegRGrow::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeSegRGrow::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}


void ModeSegRGrow::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}



void ModeSegRGrow::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if (m_drag_cpid != -1)
  {
    if ( PickCrssec(ray_pos, ray_dir, pos) != CRSSEC_NON ) 
    {
      m_cps[m_drag_cpid].pos = pos;
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



void ModeSegRGrow::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  if(!WheelingCrssec(p, z_delta, ogl) ) 
  {
    ogl->ZoomCamera(z_delta * 0.1f);
  }
  formMain_RedrawMainPanel();
}


void ModeSegRGrow::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegRGrow::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegRGrow::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegRGrow::KeyDown(int nChar) {}
void ModeSegRGrow::KeyUp  (int nChar) {}


int ModeSegRGrow::PickControlPoints(const EVec3f &ray_pos, const EVec3f &ray_dir)
{
  const int frame_idx = formVisParam_getframeI();

  for (int i = 0; i < (int)m_cps.size(); ++i)
  {
    if (m_cps[i].frameI == frame_idx)
    {
      if (DistRayAndPoint(ray_pos, ray_dir, m_cps[i].pos) < m_cp_radius) return i;
    }
  }
  return -1;
}



void ModeSegRGrow::DrawScene(const EVec3f &cam_pos, const EVec3f &cam_cnt)
{
  ImageCore::GetInst()->BindAllVolumes();
  DrawCrossSectionsVisFore(!IsSpaceKeyOn());

  if (m_b_draw_cutstroke)
  {
    DrawPolyLine( EVec3f(1,1,0), 3, m_stroke, false);
  }

  if ( formVisParam_bRendVol() )
  {
    DrawVolumeVisFore(!IsSpaceKeyOn(), cam_pos, cam_cnt);
  }

  //control points
  const int frame_idx = formVisParam_getframeI();

  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , COLOR_W     );
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, COLOR_SHIN64);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , COLOR_R );
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , COLOR_R );

  glEnable(GL_LIGHTING);
  for (const auto& it : m_cps) if (frame_idx == it.frameI)
  {
    TMesh::DrawSphere(it.pos, m_cp_radius);
  }
  glDisable(GL_LIGHTING);

}



template<class T>
static bool IsInRange(const T &val, const T &minv, const T &maxv) {
  return minv <= val && val <= maxv;
}



///////////////////////////////////////////////////////////////////////////
//Segmentation/////////////////////////////////////////////////////////////
void ModeSegRGrow::RunThresholding_AllFrame(const short minv, const short maxv)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int num_voxels = ImageCore::GetInst()->GetNumVoxels();

#pragma omp parallel for
  for (int f = 0; f < num_frames; ++f)
  {
    const short *img3d = ImageCore::GetInst()->m_img4d[f];
    byte* flg3d        = ImageCore::GetInst()->m_flg4d[f];

    for (int i = 0; i < num_voxels; ++i) if (flg3d[i] != 0)
    {
      flg3d[i] = IsInRange(img3d[i], minv, maxv) ? 255 : 1;
    }
  }

  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
  m_b_modified = true;
}



void ModeSegRGrow::RunThresholding_OneFrame(const short minv, const short maxv, const int frameI)
{
  const int   num_frames = ImageCore::GetInst()->GetNumFrames();
  const int   num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const short *img3d     = ImageCore::GetInst()->m_img4d[frameI];
  byte* flg3d            = ImageCore::GetInst()->m_flg4d[frameI];

  if (frameI < 0 || num_frames <= frameI)
  {
    std::cout << "strange input!\n";
    return;
  }

  for (int i = 0; i < num_voxels; ++i) if (flg3d[i] != 0)
  {
    flg3d[i] = IsInRange(img3d[i], minv, maxv) ? 255 : 1;
  }

  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
  m_b_modified = true;
}



void ModeSegRGrow::RunRegionGrow4D(const short minv, const short maxv)
{
  std::cout << "runRegionGrow8...\n";

  int W, H, D, WH, WHD;
  std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();
  const int num_frames   = ImageCore::GetInst()->GetNumFrames();
  const int num_voxels   = ImageCore::GetInst()->GetNumVoxels();

  const std::vector<short *> &img4d = ImageCore::GetInst()->m_img4d;
  std::vector <byte*> &flg4d = ImageCore::GetInst()->m_flg4d;


  //volFlg : 0:never change, 1:back, 255:fore
#pragma omp parallel for
  for (int fi = 0; fi < num_frames; ++fi)
  {
    byte* flg3d = flg4d[fi];
    for (int vi = 0; vi < num_voxels; ++vi)
    {
      flg3d[vi] = (flg3d[vi] == 0) ? 0 : 1;
    }
  }

  //CP --> pixel id
  TQueue<PixIdx4D> Q;
  for (const auto cp : m_cps)
  {
    const int vi = ImageCore::GetInst()->GetVoxelIndex( cp.pos );

    if (flg4d[cp.frameI][vi] != 0 && IsInRange(img4d[cp.frameI][vi], minv, maxv))
    {
      Q.push_back(PixIdx4D(vi, cp.frameI));
      flg4d[cp.frameI][vi] = 255;
    }
  }

  //growing
  while (!Q.empty())
  {
    const int I = Q.front().idx;
    const int x = I%W;
    const int y = (I / W) % H;
    const int z = I / WH;
    const int f = Q.front().frameI;
    Q.pop_front();

    short *img3dF = img4d[f];  
    byte  *flg3dF = flg4d[f];
    //grow to 8 neighbors
    int K;
    K = I - 1;  if (x > 0 && flg3dF[K] == 1 && IsInRange(img3dF[K], minv, maxv)) { flg3dF[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I - W;  if (y > 0 && flg3dF[K] == 1 && IsInRange(img3dF[K], minv, maxv)) { flg3dF[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I - WH; if (z > 0 && flg3dF[K] == 1 && IsInRange(img3dF[K], minv, maxv)) { flg3dF[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I + 1;  if (x <W-1&& flg3dF[K] == 1 && IsInRange(img3dF[K], minv, maxv)) { flg3dF[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I + W;  if (y <H-1&& flg3dF[K] == 1 && IsInRange(img3dF[K], minv, maxv)) { flg3dF[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I + WH; if (z <D-1&& flg3dF[K] == 1 && IsInRange(img3dF[K], minv, maxv)) { flg3dF[K] = 255; Q.push_back(PixIdx4D(K, f)); }

    K = I;      if (f > 0           && flg4d[f-1][K] == 1 && IsInRange(img4d[f-1][K], minv, maxv)) { flg4d[f-1][K] = 255; Q.push_back(PixIdx4D(K, f-1)); }
    K = I;      if (f <num_frames-1 && flg4d[f+1][K] == 1 && IsInRange(img4d[f+1][K], minv, maxv)) { flg4d[f+1][K] = 255; Q.push_back(PixIdx4D(K, f+1)); }
  }

  std::cout << "runRegionGrow8...DONE\n\n";

  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
  m_b_modified = true;

}



void ModeSegRGrow::RunRegionGrow3D_OneFrame(const short minv, const short maxv, const int frameI)
{
  std::cout << "runRegionGrow6...\n";

  int W, H, D, WH, WHD;
  std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();
  const int num_frames   = ImageCore::GetInst()->GetNumFrames();
  const int num_voxels   = ImageCore::GetInst()->GetNumVoxels();
  const short *img3d = ImageCore::GetInst()->m_img4d[frameI];
  byte        *flg3d = ImageCore::GetInst()->m_flg4d[frameI];

  if (frameI < 0 || num_frames <= frameI)
  {
    std::cout << "strange input!\n";
    return;
  }

  //volFlg : 0:never change, 1:back, 255:fore
  for (int i = 0; i < WHD; ++i)
  {
    flg3d[i] = (flg3d[i] == 0) ? 0 : 1;
  }

  //CP --> pixel id
  TQueue<PixIdx4D> Q;
  for (const auto cp : m_cps)
  {
    const int I = ImageCore::GetInst()->GetVoxelIndex( cp.pos );
    
    if (cp.frameI != frameI) continue;

    if ( flg3d[I] != 0 && IsInRange(img3d[I], minv, maxv))
    {
      Q.push_back(PixIdx4D(I, frameI));
      flg3d[I] = 255;
    }
  }

  //growing
  while (!Q.empty())
  {
    const int I = Q.front().idx;
    const int x = I%W;
    const int y = (I / W) % H;
    const int z = I / WH;
    const int f = Q.front().frameI;
    Q.pop_front();

    //grow to 8 neighbors
    int K;
    K = I - 1;  if (x >  0  && flg3d[K] == 1 && IsInRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I - W;  if (y >  0  && flg3d[K] == 1 && IsInRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I - WH; if (z >  0  && flg3d[K] == 1 && IsInRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I + 1;  if (x < W-1 && flg3d[K] == 1 && IsInRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I + W;  if (y < H-1 && flg3d[K] == 1 && IsInRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    K = I + WH; if (z < D-1 && flg3d[K] == 1 && IsInRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
  }

  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
  m_b_modified = true;
  std::cout << "runRegionGrow6...DONE\n\n";
}


void ModeSegRGrow::RunErosion3D_AllFrame()
{
  RunErosion3D_AllFrame_flg4();
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}


void ModeSegRGrow::RunDilation3D_AllFrame()
{
  RunDilation3D_AllFrame_flg4();
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}


void ModeSegRGrow::RunFillHole3D_AllFrame()
{
  RunFillHole3D_AllFrame_flg4();
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}

