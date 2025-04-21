#pragma unmanaged
#include "ModePlaceCPs.h"
#include "ModeCommonTools.h"
#include "ModeCore.h"
#include "ImageCore.h"
#include "CrsSecCore.h"

#pragma managed
#include "FormPlaceCPs.h"
#include "FormMain.h"
#include "FormVisParam.h"
#include "CliMessageBox.h"
#pragma unmanaged

#include "tmath.h"
#include "tmesh.h"
#include "thandle3d.h"
#include "tmarchingcubes.h"

using namespace RoiPainter4D;
using namespace marchingcubes;


ModePlaceCPs::~ModePlaceCPs() 
{
}


ModePlaceCPs::ModePlaceCPs()
{
  std::cout << "ModePlaceCPs...\n";
  m_bL = m_bR = m_bM = false;
  std::cout << "ModePlaceCPs done\n";
}


bool ModePlaceCPs::CanEndMode() 
{ 
  return true; 
}


void ModePlaceCPs::StartMode() 
{
  m_bL = m_bR = m_bM = false;
  m_b_draw_stroke = false;
  m_drag_cpid = -1;
  m_drag_tmpcpid = -1;

  //show dialog
  FormPlaceCPs_InitParam();
  FormPlaceCPs_Show();
  
  //initialize vFlg & 4D volume (cpu) --> vis volume (gpu)
  ImageCore::GetInst()->InitializeFlg4dByMask(formMain_SetProgressValue);
  UpdateImageCoreVisVolumes();

  m_cp_rad = ImageCore::GetInst()->GetPitch()[0] * 2;
  m_cp_mesh.InitializeAsSphere(m_cp_rad, 10, 10);

  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  m_isosurfaces       = std::vector<TTriangleSoup      >(num_frames);
  m_cps               = std::vector<std::vector<EVec3f>>(num_frames);
  m_template_rottrans = std::vector<EMat4f             >(num_frames);

}



void ModePlaceCPs::FinishSegmentation() 
{
  const int num_frame = ImageCore::GetInst()->GetNumFrames();
  const int num_voxel = ImageCore::GetInst()->GetNumVoxels();
  const EVec3i  reso  = ImageCore::GetInst()->GetReso();
  const EVec3f  pitch = ImageCore::GetInst()->GetPitch();

  std::vector<byte*>& flg4d = ImageCore::GetInst()->m_flg4d;


  byte* flgInOut = new byte[num_voxel];

  for (int f = 0; f < num_frame; ++f)
  {
    TMesh mesh = m_template;
    mesh.MultMat(m_template_rottrans[f]);
    mesh.GenBinaryVolume(reso, pitch, flgInOut);

    byte* flg3d = flg4d[f];
#pragma omp parallel for
    for (int i = 0; i < num_voxel; ++i)
    {
      flg3d[i] = (flg3d[i]    == 0) ? 0   :
                 (flgInOut[i] == 1) ? 255 : 1;
    }

    //progressbar
    formMain_SetProgressValue((float) f / num_frame);
    std::cout << "fillInMesh " << f << "\n";
  }
  delete[] flgInOut;
  formMain_SetProgressValue(0);

  ImageCore::GetInst()->mask_storeCurrentForeGround();

  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  formMain_RedrawMainPanel();
}




void ModePlaceCPs::CancelSegmentation() 
{
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  RoiPainter4D::formMain_RedrawMainPanel();
}



//表示されているplaneと isosurfaceをピッキングして、近い点を返す
bool ModePlaceCPs::PickPlanesIsosurf(
    const EVec3f& ray_pos, 
    const EVec3f& ray_dir, 
    EVec3f& pos)
{
  const int fidx = formVisParam_getframeI();
  if (fidx < 0 || m_isosurfaces.size() <= fidx ) return false;

  EVec3f p1, p2;
  bool pick1 = PickCrssec(ray_pos, ray_dir, p1);
  bool pick2 = FormPlaceCPs_VisIsoSurface() && 
               m_isosurfaces[fidx].PickByRay(ray_pos, ray_dir, p2);

  if (!pick1 && !pick2) return false;

  if ( pick1 && !pick2) pos = p1;
  if (!pick1 &&  pick2) pos = p2;
  if ( pick1 &&  pick2)
    pos = (ray_pos - p1).norm() < (ray_pos - p2).norm() ? p1 : p2;

  return true;

}


//return -1 if fails
static int pick_cps(
  const EVec3f& ray_pos,
  const EVec3f& ray_dir,
  const std::vector<EVec3f> &cps,
  const float cp_rad)
{
  float min_depth = FLT_MAX;
  int min_idx = -1;

  for (int i = 0; i < (int)cps.size(); ++i)
  {
    if (DistRayAndPoint(ray_pos, ray_dir, cps[i]) > cp_rad) continue;

    float d = (ray_pos - cps[i]).norm();
    if (d < min_depth)
    {
      min_idx = i;
      min_depth = d;
    }
  }
  return min_idx;
}


static bool pick_to_erase_cps(
    const EVec3f& ray_pos,
    const EVec3f& ray_dir,
    const float cp_rad,
    std::vector<EVec3f>& cps)
{
  int cpid = pick_cps(ray_pos, ray_dir, cps, cp_rad);
  if (cpid != -1)
  {
    cps.erase(cps.begin() + cpid);
    return true;
  }
  return false;
}



void ModePlaceCPs::LBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = true;
  
  if (IsCtrKeyOn())
  {
    m_stroke.clear();
    m_b_draw_stroke = true;
  }
  else if (IsShiftKeyOn())
  {
    const int f = formVisParam_getframeI();

    EVec3f ray_pos, ray_dir, pos;
    ogl->GetCursorRay(p, ray_pos, ray_dir);
    
    //pick cps to drag
    m_drag_cpid = pick_cps(ray_pos, ray_dir, m_cps[f], m_cp_rad);
    if (m_drag_cpid != -1) return;

    m_drag_tmpcpid = pick_cps(ray_pos, ray_dir, m_template_cps, m_cp_rad);
    if (m_drag_tmpcpid != -1) return;
    
    //pick template --> pick planes/isosurface
    if (m_template.PickByRay(ray_pos, ray_dir, pos))
    {
      m_template_cps.push_back(pos);
      m_drag_tmpcpid = (int)m_template_cps.size()-1;
    }
    else if (PickPlanesIsosurf(ray_pos, ray_dir, pos))
    {
      m_cps[f].push_back(pos);
      m_drag_cpid = (int)m_cps[f].size() - 1;
    }
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }

  formMain_RedrawMainPanel();
}



void ModePlaceCPs::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  if (m_b_draw_stroke)
  {
    EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();
    CrssecCore::CreateCurvedCrssec(cuboid, ogl->GetCamPos(), m_stroke);
  }

  m_bL = false;
  m_b_draw_stroke = false;
  m_drag_cpid = -1;
  m_drag_tmpcpid = -1;

  ogl->BtnUp();
  formMain_RedrawMainPanel();
}



void ModePlaceCPs::RBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = true;

  if (IsShiftKeyOn())
  {
    const int f = formVisParam_getframeI();
    EVec3f ray_pos, ray_dir;
    ogl->GetCursorRay(p, ray_pos, ray_dir);

    if (pick_to_erase_cps(ray_pos, ray_dir, m_cp_rad, m_cps[f]))
    {
      formMain_RedrawMainPanel();
    }
    else if (pick_to_erase_cps(ray_pos, ray_dir, m_cp_rad, m_template_cps))
    {
      formMain_RedrawMainPanel();
    }
  }
  else
  {
    ogl->BtnDown_Rot(p);
  }
}


void ModePlaceCPs::RBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = false;
  ogl->BtnUp();
  RoiPainter4D::formMain_RedrawMainPanel();
}

void ModePlaceCPs::MBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}

void ModePlaceCPs::MBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = false;
  ogl->BtnUp();
  RoiPainter4D::formMain_RedrawMainPanel();
}

void ModePlaceCPs::LBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModePlaceCPs::RBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModePlaceCPs::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}


void ModePlaceCPs::MouseMove(const EVec2i& p, OglForCLI* ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if (m_b_draw_stroke)
  {
    m_stroke.push_back(ray_pos + 0.1f * ray_dir);
  }
  else if (m_drag_cpid != -1)
  {
    if (PickPlanesIsosurf(ray_pos, ray_dir, pos))
    {
      m_cps[formVisParam_getframeI()][m_drag_cpid] = pos;
    }
  }
  else if (m_drag_tmpcpid != -1)
  {
    if (m_template.PickByRay(ray_pos, ray_dir, pos))
    {
      m_template_cps[m_drag_tmpcpid] = pos;
    }
  }
  else
  {
    ogl->MouseMove(p);
  }

  formMain_RedrawMainPanel();
}


void ModePlaceCPs::MouseWheel(const EVec2i& p, short z_delta, OglForCLI* ogl)
{
  if (!WheelingCrssec(p, z_delta, ogl))
    ogl->ZoomCamera(z_delta * 0.1f);
  formMain_RedrawMainPanel();
}


void ModePlaceCPs::KeyDown(int nChar)
{
  formMain_RedrawMainPanel();
}

void ModePlaceCPs::KeyUp(int nChar) {}



static void DrawColoredCPs(
    const TMesh &cp_mesh,  
    const std::vector<EVec3f> &cps)
{
  static const int NUM_COL = 9;
  static float COLOR[9][4] =
  {
    {1.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f, 0.5f},
    {1.0f, 0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.5f, 0.5f}, {0.5f, 0.0f, 1.0f, 0.5f},
    {0.5f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.5f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.5f, 0.5f}
  };

  for (int i = 0; i < cps.size(); ++i)
  {
    float* c = COLOR[i % NUM_COL];
    glPushMatrix();
    glTranslated(cps[i][0], cps[i][1], cps[i][2]);
    cp_mesh.Draw(c, c, COLOR_W, COLOR_SHIN64);
    glPopMatrix();
  }
}


static float COLOR_HY[4] = { 0.3f, 0.4f, 0.1f, 0.3f };
static float COLOR_RB[4] = { 0.8f, 0.2f, 0.8f, 0.3f };


void ModePlaceCPs::DrawScene(const EVec3f& cam_pos, const EVec3f& cam_cnt) 
{
  const int frame_idx = formVisParam_getframeI();

  if (frame_idx < 0 || m_isosurfaces.size() <= frame_idx || m_cps.size() <= frame_idx)
  {
    std::cout << "something wrong ...\n";
    return;
  }

  ImageCore::GetInst()->BindAllVolumes();
  DrawCrossSectionsNormal();
  
  if (m_b_draw_stroke)
  {
    DrawPolyLine(EVec3f(1, 1, 0), 3, m_stroke, false);
  }

  glEnable(GL_LIGHTING);
  if (!IsSpaceKeyOn())
  {
    //draw control points
    DrawColoredCPs(m_cp_mesh, m_cps[frame_idx]);
    DrawColoredCPs(m_cp_mesh, m_template_cps);

    //draw isosurface
    if (FormPlaceCPs_VisIsoSurface())
    {
      m_isosurfaces[frame_idx].Draw(COLOR_HY, COLOR_HY, COLOR_W, COLOR_SHIN64);
    }

    m_template.Draw(COLOR_RB, COLOR_RB, COLOR_W, COLOR_SHIN64);

    if (FormPlaceCPs_VisFitTemplate())
    {
      glPushMatrix();
      glMultMatrixf(m_template_rottrans[frame_idx].data());
      m_template.Draw(COLOR_RB, COLOR_RB, COLOR_W, COLOR_SHIN64);
      DrawColoredCPs(m_cp_mesh, m_template_cps);
      glPopMatrix();
    }
  }

  if (formVisParam_bRendVol())
  {
    DrawVolumeNormal(cam_pos, cam_cnt);
  }
}



static void GenIsoSurfHalf(
  const EVec3i reso  ,
  const EVec3f pitch ,
  const short  *volume,
  const int  isovalue,
  TTriangleSoup &mesh
)
{
  const int W = reso[0], H = reso[1], D = reso[2];
  const int hW = W / 2, hH = H / 2, hD = D / 2;
  EVec3i rh(hW, hH, hD);
  EVec3f ph(pitch[0] * 2, pitch[1] * 2, pitch[2] * 2);

  short* vh = new short[hW * hH * hD];
  for (int z = 0; z < hD; ++z)
    for (int y = 0; y < hH; ++y)
      for (int x = 0; x < hW; ++x)
        vh[z * hW * hH + y * hW + x] = volume[2 * z * W * H + 2 * y * W + 2 * x];

  MarchingCubesPolygonSoup(rh, ph, vh, isovalue, 0, 0, mesh);
  delete[] vh;
}


void ModePlaceCPs::GenIsoSurface(
    const int  isovalue, 
    const bool do_all_frame, 
    const int  frame_idx)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  if (m_isosurfaces.size() != num_frames)
  {
    m_isosurfaces.clear();
    m_isosurfaces.resize(num_frames);
  }
  m_isovalue = isovalue;

  const EVec3i reso  = ImageCore::GetInst()->GetReso();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();
  bool do_halfen = reso[0] % 2 == 0 && reso[1] % 2 == 0 && reso[2] % 2 == 0;

  if (do_all_frame)
  {
    for (int i = 0; i < num_frames; ++i)
    {
      short* volume = ImageCore::GetInst()->m_img4d[i];

      if (do_halfen)
        GenIsoSurfHalf(reso, pitch, volume, isovalue, m_isosurfaces[i]);
      else
        MarchingCubesPolygonSoup(reso, pitch, volume, isovalue, 0, 0, m_isosurfaces[i]);
      
      std::cout << "isoSurfGen " << i << "/" << num_frames << "\n";
    }
  }
  else
  {
    const short* volume = ImageCore::GetInst()->m_img4d[frame_idx];

    if (do_halfen)
      GenIsoSurfHalf(reso, pitch, volume, isovalue, m_isosurfaces[frame_idx]);
    else
      MarchingCubesPolygonSoup(reso, pitch, volume, isovalue, 0, 0, m_isosurfaces[frame_idx]);

    std::cout << "isoSurfGen1Frame " << frame_idx << " / " << num_frames << "\n";
  }

}




void ModePlaceCPs::ExportControlPoints(std::string fname) 
{
  std::ofstream ofs(fname.c_str());
  if (!ofs.is_open())
  {
    std::cout << "error when opening " << fname << "\n";
    return;
  }

  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  ofs << "ControlPoins " << num_frames << "\n";

  for (int f = 0; f < num_frames; ++f)
  {
    ofs << "frame " << f << " " << (int)m_cps[f].size() << "\n";
    for (int i = 0; i < m_cps[f].size(); ++i)
      WriteToFstream(ofs, m_cps[f][i]);
  }
  ofs.close();
}



void ModePlaceCPs::ImportControlPoints(std::string fname) 
{
  std::ifstream ifs(fname);

  if (ifs.fail())
  {
    std::cout << "file open error";
    return;
  }

  std::string str;
  int num_frame;
  ifs >> str >> num_frame;
  
  if (ImageCore::GetInst()->GetNumFrames() != num_frame)
  {
    std::cout << "wrong num of frames...";
    ifs.close();
    return;
  }
  
  m_cps.clear();
  m_cps.resize(num_frame);
  for (int f = 0; f < num_frame; ++f)
  {
    int fidx, num_cps;
    ifs >> str >> fidx >> num_cps;
    std::cout << "frame " << fidx << " " << num_cps << "\n";

    m_cps[f].resize(num_cps);
    for (int i = 0; i < m_cps[f].size(); ++i)
      ReadFromFstream(ifs, m_cps[f][i]);
  }
  ifs.close();

  formMain_RedrawMainPanel();
}



void ModePlaceCPs::LoadTemplateMesh(std::string fname)
{
  m_template_cps.clear();
  m_template.Initialize(fname.c_str());
  
  EVec3f minbb, maxbb;
  m_template.GetBoundingBox(minbb, maxbb);
  m_template.Translate(EVec3f(-maxbb[0]*1.2f,0,0));
}




void ModePlaceCPs::FitTemplateUsingCPs(bool modify_scale)
{
  const int num_frame = ImageCore::GetInst()->GetNumFrames();
  m_template_rottrans.resize(num_frame);


  for (int f = 0; f < num_frame; ++f)
  {
    const int N = std::min((int)m_template_cps.size(), (int)m_cps[f].size());
    if (N == 3)
    {
      CalcTriangleMatching(m_template_cps, m_cps[f], m_template_rottrans[f]);
    }
    else if (N >= 4)
    {
      CalcShapeMatching(m_template_cps, m_cps[f], m_template_rottrans[f]);
    }
  }
}




/*

//Eigen solver test
static void EigenSolverTest()
{
  EMat3f M;
  M << 1, 2, 3,
    2, -1, 5,
    3, 5, 2;

  Eigen::SelfAdjointEigenSolver<EMat3f> eigensolver(M);

  if (eigensolver.info() == Eigen::Success)
  {
    EMat3f V = eigensolver.eigenvectors();
    EVec3f d = eigensolver.eigenvalues();

    EMat3f D;
    D << d[0], 0, 0,
      0, d[1], 0,
      0, 0, d[2];

    std::cout << "vis\n";
    std::cout << V * V.transpose() << " -- \n\n"; //matrix (v0,v1,v2)
    std::cout << V.transpose() * V << " -- \n\n"; //matrix (v0,v1,v2)
    std::cout << V * D * V.transpose() << "\n\n";
  }
}

*/
