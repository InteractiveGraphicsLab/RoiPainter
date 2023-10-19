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


ModePlaceCPs::ModePlaceCPs():
  m_volume_shader("shader/volVtx.glsl", "shader/volFlg.glsl"),
  m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
{
  std::cout << "ModePlaceCPs...\n";
  m_bL = m_bR = m_bM = false;
  m_b_draw_stroke = false;

  std::cout << "ModePlaceCPs done\n";
}


bool ModePlaceCPs::canEndMode() 
{ 
  return true; 
}


void ModePlaceCPs::startMode() 
{
  m_bL = m_bR = m_bM = false;
  m_drag_cpid = -1;
  m_drag_tmpcpid = -1;

  //show dialog
  FormPlaceCPs_InitParam();
  FormPlaceCPs_Show();
  
  //initialize vFlg
  ImageCore::GetInst()->InitializeFlg4dByMask(formMain_SetProgressValue);

  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  m_isosurfaces.clear();
  m_isosurfaces.resize(num_frames);
  
  m_cp_rad = ImageCore::GetInst()->GetPitch()[0] * 2;
  m_cps.clear();
  m_cps.resize(num_frames);
  m_cp_mesh.InitializeAsSphere(m_cp_rad, 10, 10);

  m_template_rottrans.resize(num_frames);

  //4D volume (cpu) --> vis volume (gpu)
  UpdateImageCoreVisVolumes();
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
    mesh.Rotate(m_template_rottrans[f].first);
    mesh.Translate(m_template_rottrans[f].second);
    GenBinaryVolumeFromMesh_Y(reso, pitch, mesh, flgInOut);

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
bool ModePlaceCPs::PickPlanesIsosurf(const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f& pos)
{
  const int fidx = formVisParam_getframeI();
  if (fidx < 0 || m_isosurfaces.size() <= fidx ) return false;

  EVec3f p1, p2;
  bool pick1 = PickCrssec(ray_pos, ray_dir, p1);
  bool pick2 = FormPlaceCPs_VisIsoSurface() && m_isosurfaces[fidx].PickByRay(ray_pos, ray_dir, p2);

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
    if (t_DistRayAndPoint(ray_pos, ray_dir, cps[i]) > cp_rad) continue;

    float d = (ray_pos - cps[i]).norm();
    if (d < min_depth)
    {
      min_idx = i;
      min_depth = d;
    }
  }
  return min_idx;
}

bool pick_to_erase_cps(
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
  std::cout << "Down__";

  m_bL = true;
  

  if (isCtrKeyOn())
  {
    m_stroke.clear();
    m_b_draw_stroke = true;
  }
  else if (isShiftKeyOn())
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
  std::cout << "Up\n";
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

  if (isShiftKeyOn())
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


void ModePlaceCPs::keyDown(int nChar)
{
  formMain_RedrawMainPanel();
}

void ModePlaceCPs::keyUp(int nChar) {}


template<class TMESH_TRIANGLESOUP>
static void DrawTransparentMesh(
    const TMESH_TRIANGLESOUP &m,
    const float diff[4], 
    const float ambi[4],
    const float spec[4],
    const float shin[1])
{

  glDepthMask(false);
  glEnable(GL_BLEND);
  m.Draw(diff, ambi, spec, shin);
  glDisable(GL_BLEND);
  glDepthMask(true);
}

static void DrawTranslatedMesh(
  const TMesh  &m,
  const EVec3f &t,
  const float diff[4],
  const float ambi[4],
  const float spec[4],
  const float shin[1])
{
  glPushMatrix();
  glTranslated(t[0], t[1], t[2]);
  m.Draw(diff,ambi, spec, shin);
  glPopMatrix();

}


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
  static float SPEC[4] = { 0.9f,0.9f,0.9f,0.3f };
  static float SHIN[1] = { 54.0f };

  for (int i = 0; i < cps.size(); ++i)
  {
    float* color = COLOR[i % NUM_COL];
    DrawTranslatedMesh(cp_mesh, cps[i], color, color, SPEC, SHIN);
  }
}



static float SPEC[4] = { 0.9f,0.9f,0.9f,0.3f };
static float SHIN[1] = { 54.0f };
static float DIFF1[4] = { 0.3f,0.4f,0.1f,0.3f };
static float AMBI1[4] = { 0.3f,0.4f,0.1f,0.3f };
static float DIFF2[4] = { 0.8f,0.2f,0.8f,0.3f };
static float AMBI2[4] = { 0.8f,0.2f,0.8f,0.3f };


void ModePlaceCPs::drawScene(const EVec3f& cuboid, const EVec3f& camP, const EVec3f& camF) 
{
  const int    frame_idx = formVisParam_getframeI();
  const EVec3i reso      = ImageCore::GetInst()->GetReso();
  const EVec3f pitch     = ImageCore::GetInst()->GetPitch();

  if (frame_idx < 0 || m_isosurfaces.size() <= frame_idx) 
  {
    std::cout << "something wrong 1...\n";
    return;
  }
  if (frame_idx < 0 || m_cps.size() <= frame_idx)
  {
    std::cout << "something wrong 2...\n";
    return;
  }

  BindAllVolumes();
  DrawCrossSections(cuboid, reso, false, m_crssec_shader);
  
  if (m_b_draw_stroke)
  {
    t_DrawPolyLine(EVec3f(1, 1, 0), 3, m_stroke, false);
  }

  glEnable(GL_LIGHTING);
  if (!isSpaceKeyOn())
  {
    //draw control points
    DrawColoredCPs(m_cp_mesh, m_cps[frame_idx]);
    DrawColoredCPs(m_cp_mesh, m_template_cps);

    //draw isosurface
    if (FormPlaceCPs_VisIsoSurface())
    {
      m_isosurfaces[frame_idx].Draw(DIFF1, AMBI1, SPEC, SHIN);
      //DrawTransparentMesh(m_isosurfaces[frame_idx], DIFF1, AMBI1, SPEC, SHIN);
    }

    m_template.Draw(DIFF2, AMBI2, SPEC, SHIN);

    if (FormPlaceCPs_VisFitTemplate())
    {
      float m[16];
      const EMat3f& r = m_template_rottrans[frame_idx].first;
      const EVec3f& t = m_template_rottrans[frame_idx].second;
      m[0]=r(0,0); m[4]=r(0,1); m[ 8]=r(0,2); m[12] = t[0];
      m[1]=r(1,0); m[5]=r(1,1); m[ 9]=r(1,2); m[13] = t[1];
      m[2]=r(2,0); m[6]=r(2,1); m[10]=r(2,2); m[14] = t[2];
      m[3]=  0 ; m[7]=  0 ; m[11]=  0 ; m[15] =    1;
      glPushMatrix();
      glMultMatrixf(m);
      m_template.Draw(DIFF2, AMBI2, SPEC, SHIN);
      DrawColoredCPs(m_cp_mesh, m_template_cps);
      glPopMatrix();
    }
    
  }


  if (formVisParam_bRendVol())
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    const bool b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolumeSlices(cuboid, reso, camP, camF,
      !isSpaceKeyOn(), b_onmanip, m_volume_shader);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }
  
}




void ModePlaceCPs::IsosurfaceGenerateOneFrame(
    const int isovalue, 
    const int frame_idx) 
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  if (m_isosurfaces.size() != num_frames)
  {
    m_isosurfaces.clear();
    m_isosurfaces.resize(num_frames);
  }
  m_isovalue = isovalue;

  const EVec3i reso = ImageCore::GetInst()->GetReso();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();
  const short* volume = ImageCore::GetInst()->m_img4d[frame_idx];
  
  if (reso[0] % 2 == 0 && reso[2] % 2 == 0 && reso[2] % 2 == 0)
  {
    //halfen volume 
    const int W = reso[0], H = reso[1], D = reso[2];
    const int hW = W / 2, hH = H / 2, hD = D / 2;
    EVec3i rh( hW, hH, hD);
    EVec3f ph(pitch[0] * 2, pitch[1] * 2, pitch[2] * 2);
    short* vh = new short[hW * hH * hD];
    for (int z = 0; z < hD; ++z)
      for (int y = 0; y < hH; ++y)
        for (int x = 0; x < hW; ++x)
          vh[z * hW * hH + y * hW + x] = volume[2*z*W*H + 2*y*W + 2*x];

    t_MarchingCubes_PolygonSoup(rh, ph, vh, isovalue, 0, 0, m_isosurfaces[frame_idx]);
    delete[] vh;

  }
  else
  {
    t_MarchingCubes_PolygonSoup(reso, pitch, volume, isovalue, 0, 0, m_isosurfaces[frame_idx]);
  }


  std::cout << "isoSurfGen1Frame " << frame_idx << " / " << num_frames << "\n";
}


void ModePlaceCPs::IsosurfaceGenerateAllFrame(const int isovalue) 
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  m_isosurfaces.clear();
  m_isosurfaces.resize(num_frames);
  m_isovalue = isovalue;

  const EVec3i reso = ImageCore::GetInst()->GetReso();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();

  const int W = reso[0], H = reso[1], D = reso[2];
  const int hW = W / 2, hH = H / 2, hD = D / 2;
  EVec3i rh(hW, hH, hD);
  EVec3f ph(pitch[0] * 2, pitch[1] * 2, pitch[2] * 2);
  short* vh = new short[hW * hH * hD];

  for (int i = 0; i < num_frames; ++i)
  {
    short* volume = ImageCore::GetInst()->m_img4d[i];
    if (W % 2 == 0 && H % 2 == 0 && D % 2 == 0)
    {
      //halfen volume 
      for (int z = 0; z < hD; ++z)
        for (int y = 0; y < hH; ++y)
          for (int x = 0; x < hW; ++x)
            vh[z * hW * hH + y * hW + x] = volume[2 * z * W * H + 2 * y * W + 2 * x];
      t_MarchingCubes_PolygonSoup(rh, ph, vh, isovalue, 0, 0, m_isosurfaces[i]);
    }
    else
    {
      t_MarchingCubes_PolygonSoup(reso, pitch, volume, isovalue, 0, 0, m_isosurfaces[i]);
    }
    std::cout << "isoSurfGen " << i << "/" << num_frames << "\n";
  }

  delete[] vh;

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
  EVec3f cuboid = ImageCore::GetInst()->GetCuboid();
  m_template_cps.clear();
  m_template.Initialize(fname.c_str());
  
  EVec3f minbb, maxbb;
  m_template.GetBoundBox(minbb, maxbb);
  m_template.Translate(EVec3f(-maxbb[0]*1.2f,0,0));
}



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



//given (x0, x1, x2) and (y0, y1, y2) 
// Σ||R (x0-c0) - (x1-c1)|| 
// x0' = R x0 + (R c0 + c)
// t = 
static void CalcTriangleMatching(
  const std::vector<EVec3f>& x,      
  const std::vector<EVec3f>& y,
  std::pair<EMat3f, EVec3f>& rottrans //OUTPUT : rotation, transpose
)
{
  rottrans.first = EMat3f::Identity();
  rottrans.second << 0, 0, 0;

  const int N = std::min((int)x.size(), (int)y.size());
  if (N != 3) return; 

  //calc gc 
  EVec3f cx = (x[0] + x[1] + x[2]) / 3.0f;
  EVec3f cy = (y[0] + y[1] + y[2]) / 3.0f;

  //calc origin center point 
  EVec3f x0 = x[0]-cx, x1 = x[1]-cx, x2 = x[2]-cx;
  EVec3f y0 = y[0]-cy, y1 = y[1]-cy, y2 = y[2]-cy;

  //calc normalized normal and rot axis
  EVec3f nx = ((x1 - x0).cross(x2 - x0));
  EVec3f ny = ((y1 - y0).cross(y2 - y0));  
  EVec3f axis_nxny = nx.cross(ny);

  float len_nx = nx.norm();
  float len_ny = ny.norm();
  float len_axis_nxny = axis_nxny.norm();
  if (len_nx < 0.00001f || len_ny < 0.00001f || len_axis_nxny < 0.00001f ) return;
  nx /= len_nx;
  ny /= len_ny;
  axis_nxny /= len_axis_nxny;

  EMat3f Rot1 = t_CalcRotationMat(nx, ny, axis_nxny);
  x0 = Rot1 * x0;
  x1 = Rot1 * x1;
  x2 = Rot1 * x2;

  float t0 = t_CalcAngle(x0, y0, ny);
  float t1 = t_CalcAngle(x1, y1, ny);
  float t2 = t_CalcAngle(x2, y2, ny);
  float t = t_CalcAverageAngle3(t0, t1, t2);

  EMat3f Rot2 = Eigen::AngleAxis<float>(t, ny).matrix();

  //R = Apq * (V sqrt(U) Vt) ^ -1
  rottrans.first = Rot2 * Rot1;
  rottrans.second = -rottrans.first * cx + cy;
}




//given two sets pf points 
//compute translation t and rotation R to minimize 
// Σ||R (x0-c0) - (x1-c1)||
// c0 and c1 are gravty centers of x0 and x1
// t = - R c0 + c
static void CalcShapeMatching(
    const std::vector<EVec3f> &x0,
    const std::vector<EVec3f> &x1,
    std::pair<EMat3f, EVec3f> &rottrans //OUTPUT : rotation, transpose
)
{
  rottrans.first  = EMat3f::Identity();
  rottrans.second << 0, 0, 0;

  const int N = std::min((int)x0.size(), (int)x1.size());
  if (N<=3) return; //TODO N == 3の時は別の処理

  //calc gc 
  EVec3f c0(0,0,0), c1(0,0,0);
  for (int i = 0; i < N; ++i )
  {
    c0 += x0[i];
    c1 += x1[i];
  }
  c0 /= (float)N;
  c1 /= (float)N;

  //calc Apq = Σ((x1i-c1))(x0i-c0)-T
  EMat3f Apq = EMat3f::Zero();
  for (int i = 0; i < N; ++i)
  {
    EVec3f p = x1[i] - c1, q = x0[i] - c0;
    Apq(0,0) += p[0]*q[0];  Apq(0,1) += p[0]*q[1];  Apq(0,2) += p[0]*q[2];
    Apq(1,0) += p[1]*q[0];  Apq(1,1) += p[1]*q[1];  Apq(1,2) += p[1]*q[2];
    Apq(2,0) += p[2]*q[0];  Apq(2,1) += p[2]*q[1];  Apq(2,2) += p[2]*q[2];
  }
  
  //AtA = V U Vt --> sqrt(AtA) = V sqrt(U) Vt 
  EMat3f AtA = Apq.transpose() * Apq;
  Eigen::SelfAdjointEigenSolver<EMat3f> eigensolver(AtA);
  if (eigensolver.info() != Eigen::Success) return;
  EMat3f V = eigensolver.eigenvectors();
  EVec3f d = eigensolver.eigenvalues();

  EMat3f D = EMat3f::Identity();
  D(0, 0) = (d[0] > 0) ? sqrt(d[0]) : 0.000001f;
  D(1, 1) = (d[1] > 0) ? sqrt(d[1]) : 0.000001f;
  D(2, 2) = (d[2] > 0) ? sqrt(d[2]) : 0.000001f;
  
  //D --> D^-1
  D(0, 0) = 1.0f / D(0, 0);
  D(1, 1) = 1.0f / D(1, 1);
  D(2, 2) = 1.0f / D(2, 2);
  EMat3f sqrtAtA_inv = V * D * V.transpose();

  //R = Apq * (V sqrt(U) Vt) ^ -1
  rottrans.first = Apq * sqrtAtA_inv;
  rottrans.second = -rottrans.first * c0 + c1;
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
