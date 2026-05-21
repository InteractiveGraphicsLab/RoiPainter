#include "ModeMdlFitMuscleModel.h"
#include "OglForCLI.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "ModeCore.h"
#include "tmarchingcubes.h"
#include "tcolor.h"

#pragma managed
#include "FormMdlFitMuscleModel.h"
#include "../FormVisParam.h"
#include "../FormMain.h"
#pragma unmanaged


using namespace RoiPainter3D;
using namespace marchingcubes;


ModeMdlFitMuscleModel::ModeMdlFitMuscleModel()
{
  m_bL = m_bR = m_bM = false;
  m_flag = false;
}

static int PickLandmark(const EVec3f& ray_pos, const EVec3f& ray_dir, const std::vector<EVec3f>& lmk, const float lmk_radius)
{
  float min_depth = FLT_MAX;
  int min_ID = -1;

  for (int i = 0; i < lmk.size(); i++)
  {
    if (DistRayAndPoint(ray_pos, ray_dir, lmk[i]) > lmk_radius) continue;

    float d = (ray_pos - lmk[i]).norm();
    if (d < min_depth)
    {
      min_ID = i;
      min_depth = d;
    }
  }
  return min_ID;
}


static void PickToEraseLandmark(
  const EVec3f& ray_pos,
  const EVec3f& ray_dir,
  std::vector<EVec3f>& lmk,
  const float lmk_radius)
{
  int id = PickLandmark(ray_pos, ray_dir, lmk, lmk_radius);
  if (id != -1)
    lmk.erase(lmk.begin() + id);
}


// オーバーライド
void ModeMdlFitMuscleModel::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = false;
  ogl->BtnUp();
  m_drag_isosurface_lmk_ID = -1;
  m_drag_model_lmk_ID = -1;
  m_drag_model_flg = false;
  RedrawScene();
}


void ModeMdlFitMuscleModel::RBtnUp(const EVec2i& p, OglForCLI* ogl) 
{
  m_bR = false;
  ogl->BtnUp();
}


void ModeMdlFitMuscleModel::MBtnUp(const EVec2i& p, OglForCLI* ogl) 
{
  m_bM = false;
  ogl->BtnUp();
}


void ModeMdlFitMuscleModel::LBtnDown(const EVec2i& p, OglForCLI* ogl) 
{
  m_bL = true;
  EVec3f ray_pos, ray_dir, iso_pos, model_pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);
  if (IsShiftKeyOn()) 
  {
    m_drag_isosurface_lmk_ID = PickLandmark(ray_pos, ray_dir, m_isosurface_lmk, m_lmk_radius);
    if (m_drag_isosurface_lmk_ID != -1) return;

    if (PickIsoSurface(ray_pos, ray_dir, iso_pos))
    {
      if (PickObjModels(ray_pos, ray_dir, model_pos) && ((model_pos - ray_pos).norm() - (iso_pos - ray_pos).norm()) < 0)
      {
        
      }
      else 
      {
        m_isosurface_lmk.push_back(iso_pos);
        m_drag_isosurface_lmk_ID = (int)m_isosurface_lmk.size() - 1;
      }
      
    }

    m_drag_model_lmk_ID = PickLandmark(ray_pos, ray_dir, m_model_lmk, m_lmk_radius);
    if (m_drag_model_lmk_ID != -1) return;

    if (PickObjModels(ray_pos, ray_dir, model_pos))
    {
      if (PickIsoSurface(ray_pos, ray_dir, iso_pos) && ((iso_pos - ray_pos).norm() - (model_pos - ray_pos).norm()) < 0)
      {
        
      }
      else 
      {
        m_model_lmk.push_back(model_pos);
        m_drag_isosurface_lmk_ID = (int)m_model_lmk.size() - 1;
      }
    }
  }
  else if (IsCtrKeyOn())
  {
    if (PickObjModels(ray_pos, ray_dir, model_pos))
    {
      m_drag_model_flg = true;
    }
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }
}


void ModeMdlFitMuscleModel::RBtnDown(const EVec2i& p, OglForCLI* ogl) 
{
  m_bR = true;
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);
  if (IsShiftKeyOn())
  {
    PickToEraseLandmark(ray_pos, ray_dir, m_isosurface_lmk, m_lmk_radius);
    PickToEraseLandmark(ray_pos, ray_dir, m_model_lmk, m_lmk_radius);
    RedrawScene();
  }
  else
  {
    ogl->BtnDown_Rot(p);
  }
}


void ModeMdlFitMuscleModel::MBtnDown(const EVec2i& p, OglForCLI* ogl) 
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}


void ModeMdlFitMuscleModel::LBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeMdlFitMuscleModel::RBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeMdlFitMuscleModel::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}


void ModeMdlFitMuscleModel::MouseMove(const EVec2i& p, OglForCLI* ogl) 
{
  if (!m_bL && !m_bR && !m_bM) return;
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if (m_drag_isosurface_lmk_ID != -1)
  {
    if (PickIsoSurface(ray_pos, ray_dir, pos))
      m_isosurface_lmk[m_drag_isosurface_lmk_ID] = pos;
  }

  if (m_drag_model_lmk_ID != -1)
  {
    if (PickObjModels(ray_pos, ray_dir, pos))
      m_model_lmk[m_drag_model_lmk_ID] = pos;
  }

  if (m_drag_model_flg)
  {
  }

  ogl->MouseMove(p);
  RedrawScene();
}


void ModeMdlFitMuscleModel::MouseWheel(const EVec2i& p, short z_delta, OglForCLI* ogl) {}


void ModeMdlFitMuscleModel::KeyDown(int nChar) {}
void ModeMdlFitMuscleModel::KeyUp(int nChar) {}


bool ModeMdlFitMuscleModel::CanLeaveMode() 
{
  return true;
}



void ModeMdlFitMuscleModel::StartMode() 
{
  ImageCore::GetInst()->InitializeVolFlgByLockedMask();

  m_bL = m_bR = m_bM = false;
  formMdlFitMuscle_Show();

  m_drag_isosurface_lmk_ID = -1;
  m_drag_model_lmk_ID = -1;
  m_lmk_radius = ImageCore::GetInst()->GetPitch()[0];
  m_lmk_mesh.InitializeAsSphere(m_lmk_radius, 10, 10);

  formVisParam_UnlockPitchBox();

}

static float COLOR_HY[4] = { 0.3f, 0.4f, 0.1f, 0.3f };

static void DrawColoredLandmark(const TMesh& lmk_mesh, const std::vector<EVec3f>& lmk)
{
  static const int NUM_COL = 9;
  static float COLOR[9][4] = {
    {1.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f, 0.5f},
    {1.0f, 0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.5f, 0.5f}, {0.5f, 0.0f, 1.0f, 0.5f},
    {0.5f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.5f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.5f, 0.5f}
  };

  for (int i = 0; i < lmk.size(); i++)
  {
    float* c = COLOR[i % NUM_COL];
    glPushMatrix();
    glTranslated(lmk[i][0], lmk[i][1], lmk[i][2]);

    lmk_mesh.Draw(c, c, COLOR_W, COLOR_SHIN64);
    glPopMatrix();
  }
}

void ModeMdlFitMuscleModel::DrawLandmarks()
{ 
  for (EVec3f lmk : m_isosurface_lmk)
  {
    glPushMatrix();
    glTranslated(lmk[0], lmk[1], lmk[2]);

    m_lmk_mesh.Draw(COLOR_R, COLOR_R, COLOR_W, COLOR_SHIN64);
    glPopMatrix();
  }

  for (EVec3f lmk : m_model_lmk)
  {
    glPushMatrix();
    glTranslated(lmk[0], lmk[1], lmk[2]);

    m_lmk_mesh.Draw(COLOR_B, COLOR_B, COLOR_W, COLOR_SHIN64);
    glPopMatrix();
  }
}




void ModeMdlFitMuscleModel::DrawScene(const EVec3f& cam_pos, const EVec3f& cam_center) 
{
  BindAllVolumes();
  
  DrawCrsSec_Standard();
  glEnable(GL_LIGHTING);
  DrawLandmarks();

  m_isosurface.Draw(COLOR_HY, COLOR_HY, COLOR_W, COLOR_SHIN64);

  
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (TMesh* mesh : m_models) {
    if (IsModelVisible(mesh))
      mesh->Draw(COLOR_W, COLOR_W, COLOR_W, COLOR_SHIN64);
  }
  //glDisable(GL_BLEND);
  if (m_flag)
  {
    DrawPrincipalAxis(m_model_lmk, 100);
    DrawPrincipalAxis(m_isosurface_lmk, 100);
  }

  if (formVisParam_bRendVol()) 
  {
    bool on_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolume_Standard(cam_pos, cam_center, on_manip);
  }
}
//

void ModeMdlFitMuscleModel::FinishSegmentation() 
{
  ImageCore::GetInst()->StoreForegroundAsNewMask();
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  RedrawScene();
}


static EVec3f CalcLMKGravityCenter(const std::vector<EVec3f>& lmks)
{
  EVec3f pos_sum = EVec3f(0, 0, 0);
  for (int i = 0; i < lmks.size(); i++)
    pos_sum += lmks[i];
  return pos_sum / (float)lmks.size();
  
}

static EMatXf CreateLandmarkMatrix(const std::vector<EVec3f>& lmks)
{
  EMatXf lmk_mat(lmks.size(), 3);
  for (int i = 0; i < lmks.size(); i++)
  {
    for (int j = 0; j < 3; j++)
    {
      lmk_mat(i, j) = lmks[i][j];
    }
  }
  return lmk_mat;
}


static EMat3f PCA(std::vector<EVec3f>& lmks)
{
  std::vector<EVec3f> center_lmks;
  EVec3f gravity_center = CalcLMKGravityCenter(lmks);
  for (int i = 0; i < lmks.size(); i++)
    center_lmks.push_back(lmks[i] - gravity_center);

  EMatXf lmk_mat = CreateLandmarkMatrix(center_lmks);

  EMat3f covariance_mat = (lmk_mat.transpose() * lmk_mat) / (float)center_lmks.size();

  Eigen::SelfAdjointEigenSolver<EMat3f> solver(covariance_mat);

  EMat3f principal_mat = solver.eigenvectors();
  
  return principal_mat;
}

void ModeMdlFitMuscleModel::TranslateModel()
{
  EMat3f iso_principal_mat = PCA(m_isosurface_lmk);
  EMat3f model_principal_mat = PCA(m_model_lmk);

  if (iso_principal_mat.col(2).dot(model_principal_mat.col(2)) < 0)
  {
    iso_principal_mat.col(2) *= -1;
  }

  EMat3f rotation = iso_principal_mat * model_principal_mat.transpose();

  if (rotation.determinant() < 0)
  {
    iso_principal_mat.col(0) *= -1;

    rotation = iso_principal_mat * model_principal_mat.transpose();
  }

  EVec3f translate = CalcLMKGravityCenter(m_isosurface_lmk) - rotation * CalcLMKGravityCenter(m_model_lmk);

  for (TMesh* model : m_models)
  {
    model->Rotate(rotation);
    model->Translate(translate);
  }
}

void ModeMdlFitMuscleModel::DrawPrincipalAxis(std::vector<EVec3f> lmks, float length)
{
  EVec3f center = CalcLMKGravityCenter(lmks);
  EMat3f vertex_mat = PCA(lmks);
  EVec3f axis = vertex_mat.col(2);

  EVec3f p0 = center;
  EVec3f p1 = center + axis.normalized() * length;

  glBegin(GL_LINES);

  glVertex3f(p0[0], p0[1], p0[2]);
  glVertex3f(p1[0], p1[1], p1[2]);

  glEnd();
}

void ModeMdlFitMuscleModel::ModelReset()
{
  for (TMesh* mesh : m_models)
  {
    if (mesh != nullptr)
    {
      delete mesh;
      mesh = nullptr;
    }
  }
  
  m_model_visibility_map.clear();
  m_models.clear();
}


static void GenIsoSurfaceHalf(const EVec3i reso, const EVec3f pitch, const short* volume, const int isovalue, TTriangleSoup& mesh)
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


void ModeMdlFitMuscleModel::GenIsoSurface(const int isovalue, const bool do_halfen)
{
  m_isovalue = isovalue;

  const EVec3i reso = ImageCore::GetInst()->GetResolution();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();
  bool can_halfen = reso[0] % 2 == 0 && reso[1] % 2 == 0 && reso[2] % 2 == 0;

  short* volume = ImageCore::GetInst()->m_vol_orig;

  if (do_halfen && can_halfen)
    GenIsoSurfaceHalf(reso, pitch, volume, isovalue, m_isosurface);
  else
    MarchingCubesPolygonSoup(reso, pitch, volume, isovalue, 0, 0, m_isosurface);
}


bool ModeMdlFitMuscleModel::PickIsoSurface(const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f& pos)
{
  EVec3f p;
  bool pick = m_isosurface.PickByRay(ray_pos, ray_dir, p);

  pos = p;
  return pick;
}

bool ModeMdlFitMuscleModel::PickObjModels(const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f& pos)
{
  for (TMesh* model : m_models)
  {
    if (model->PickByRay(ray_pos, ray_dir, pos) && m_model_visibility_map[model])
      return true;
  }
  return false;
}


void ModeMdlFitMuscleModel::ImportLandmarks(std::string fname)
{
  std::ifstream ifs(fname);
  if (ifs.fail())
  {
    std::cout << "error when opening " << "\n";
    return;
  }

  m_isosurface_lmk.clear();
  m_model_lmk.clear();
  int num_isosurface_lmk;
  int num_model_lmk;
  std::string str;

  ifs >> str >> num_isosurface_lmk;
  m_isosurface_lmk.resize(num_isosurface_lmk);
  for (int i = 0; i < m_isosurface_lmk.size(); i++)
    ReadFromFstream(ifs, m_isosurface_lmk[i]);

  ifs >> str >> num_model_lmk;
  m_model_lmk.resize(num_model_lmk);
  for (int i = 0; i < m_model_lmk.size(); i++)
    ReadFromFstream(ifs, m_model_lmk[i]);
  ifs.close();
  RedrawScene();
}


void ModeMdlFitMuscleModel::ExportLandmarks(std::string fname)
{
  std::ofstream ofs(fname.c_str());
  if (!ofs.is_open())
  {
    std::cout << "error when opening " << fname << "\n";
    return;
  }
  ofs << "IsosurfaceLandMarks " << (int)m_isosurface_lmk.size() << "\n";
  for (int i = 0; i < m_isosurface_lmk.size(); i++)
    WriteToFstream(ofs, m_isosurface_lmk[i]);

  ofs << "ModelLandMarks " << (int)m_model_lmk.size() << "\n";
  for (int i = 0; i < m_model_lmk.size(); i++)
    WriteToFstream(ofs, m_model_lmk[i]);
  ofs.close();
}


TMesh* ModeMdlFitMuscleModel::ImportObjFile(std::string fname)
{
  TMesh* newMesh = new TMesh(fname.c_str());
  if (newMesh->m_vSize > 0) {
    m_models.push_back(newMesh);
    m_model_visibility_map[newMesh] = true;
    return newMesh; 
  }
  else
  {
    delete newMesh;
    return nullptr; 
  }
}


void ModeMdlFitMuscleModel::SetModelVisibility(TMesh* mesh, bool isVisible)
{
  m_model_visibility_map[mesh] = isVisible;
}

void ModeMdlFitMuscleModel::SetPCAAxis(bool flag)
{
  m_flag = flag;
}

bool ModeMdlFitMuscleModel::IsModelVisible(TMesh* mesh)
{
  if (m_model_visibility_map.find(mesh) != m_model_visibility_map.end())
  {
    return m_model_visibility_map[mesh];
  }
  return true;
}