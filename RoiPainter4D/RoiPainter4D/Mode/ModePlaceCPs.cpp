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

  //show dialog
  FormPlaceCPs_InitParam();
  FormPlaceCPs_Show();
  
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  m_isosurfaces.clear();
  m_isosurfaces.resize(num_frames);
  
  m_cp_rad = ImageCore::GetInst()->GetPitch()[0] * 2;
  m_cps.clear();
  m_cps.resize(num_frames);
  m_cp_mesh.InitializeAsSphere(m_cp_rad, 10, 10);


  //4D volume (cpu) --> vis volume (gpu)
  UpdateImageCoreVisVolumes();
}


void ModePlaceCPs::FinishSegmentation() 
{
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  RoiPainter4D::formMain_RedrawMainPanel();
}


void ModePlaceCPs::CancelSegmentation() 
{
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  RoiPainter4D::formMain_RedrawMainPanel();
}


//表示されているplaneと isosurfaceをピッキングして、近い点を返す
bool ModePlaceCPs::pick_planes_isosurf(const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f& pos)
{
  const int fidx = formVisParam_getframeI();
  if (fidx < 0 || m_isosurfaces.size() <= fidx ) return false;

  EVec3f p1, p2;
  bool pick1 = PickCrssec(ray_pos, ray_dir, p1);
  bool pick2 = m_isosurfaces[fidx].PickByRay(ray_pos, ray_dir, p2);

  
  if (!pick1 && !pick2) return false;

  if ( pick1 && !pick2) pos = p1;
  if (!pick1 &&  pick2) pos = p2;
  if ( pick1 &&  pick2)
    pos = (ray_pos - p1).norm() < (ray_pos - p2).norm() ? p1 : p2;

  return true;

}


//return -1 if fails
int ModePlaceCPs::pick_cps(const EVec3f& ray_pos, const EVec3f& ray_dir)
{
  const int fidx = formVisParam_getframeI();
  const std::vector<EVec3f> &cps = m_cps[fidx];
  
  float min_depth = FLT_MAX;
  int min_idx = -1;

  for (int i = 0; i < (int)cps.size(); ++i)
  {
    if (t_DistRayAndPoint(ray_pos, ray_dir, cps[i]) > m_cp_rad) continue;
    
    float d = (ray_pos - cps[i]).norm();
    if (d < min_depth)
    {
      min_idx = i;
      min_depth = d;
    }
  }
  return min_idx;
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
    m_drag_cpid = pick_cps(ray_pos, ray_dir);
    if (m_drag_cpid != -1)
      return;

    //pick planes/isosurface to place
    if (pick_planes_isosurf(ray_pos, ray_dir, pos))
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

  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModePlaceCPs::RBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = true;

  if (isShiftKeyOn())
  {
    EVec3f ray_pos, ray_dir;
    ogl->GetCursorRay(p, ray_pos, ray_dir);
    int cpid = pick_cps(ray_pos, ray_dir);
    
    if (cpid != -1)
    {
      const int f = formVisParam_getframeI();
      m_cps[f].erase(m_cps[f].begin() + cpid);
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
    if (pick_planes_isosurf(ray_pos, ray_dir, pos))
    {
      const int f = formVisParam_getframeI();
      m_cps[f][m_drag_cpid] = pos;
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


static const int NUM_COL = 9;
static float COLOR[9][4] = 
{
  {1.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f, 0.5f},
  {1.0f, 0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.5f, 0.5f}, {0.5f, 0.0f, 1.0f, 0.5f},
  {0.5f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.5f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.5f, 0.5f}
};

void ModePlaceCPs::drawScene(const EVec3f& cuboid, const EVec3f& camP, const EVec3f& camF) 
{
  const int    frame_idx = formVisParam_getframeI();
  const EVec3i reso      = ImageCore::GetInst()->GetReso();
  const EVec3f pitch     = ImageCore::GetInst()->GetPitch();

  if (frame_idx < 0 || m_isosurfaces.size() <= frame_idx) 
  {
    std::cout << "somthing wrong 1...\n";
    return;
  }
  if (frame_idx < 0 || m_cps.size() <= frame_idx)
  {
    std::cout << "somthing wrong 2...\n";
    return;
  }

  //bind volumes
  BindAllVolumes();
  DrawCrossSections(cuboid, reso, false, m_crssec_shader);

  if (m_b_draw_stroke)
  {
    t_DrawPolyLine(EVec3f(1, 1, 0), 3, m_stroke, false);
  }

  //draw source and iso surfaces
  static float diff1[4] = { 0.2f,0.8f,0.2f,0.3f };
  static float ambi1[4] = { 0.2f,0.8f,0.2f,0.3f };
  static float diff2[4] = { 0.4f,0.1f,0.8f,1.0f };
  static float ambi2[4] = { 0.4f,0.1f,0.8f,1.0f };
  static float spec[4] = { 0.9f,0.9f,0.9f,0.3f };
  static float shin[1] = { 54.0f };

  glEnable(GL_LIGHTING);

  if (!isSpaceKeyOn())
  {
    //draw control points
    const auto& cps = m_cps[frame_idx];
    for (int i = 0; i < cps.size(); ++i)
    {
      glPushMatrix();
      glTranslated(cps[i][0], cps[i][1], cps[i][2]);
      m_cp_mesh.Draw(COLOR[i%NUM_COL], COLOR[i % NUM_COL], spec, shin);
      glPopMatrix();
    }

    //draw isosurface
    glDepthMask(false);
    glEnable(GL_BLEND);
    m_isosurfaces[frame_idx].Draw(diff1, ambi1, spec, shin);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDepthMask(true);
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
  t_MarchingCubes_PolygonSoup(reso, pitch, volume, isovalue, 0, 0, m_isosurfaces[frame_idx]);

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

  for (int i = 0; i < num_frames; ++i)
  {
    short* volume = ImageCore::GetInst()->m_img4d[i];
    t_MarchingCubes_PolygonSoup(reso, pitch, volume, isovalue, 0, 0, m_isosurfaces[i]);
    std::cout << "isoSurfGen " << i << "/" << num_frames << "\n";
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



