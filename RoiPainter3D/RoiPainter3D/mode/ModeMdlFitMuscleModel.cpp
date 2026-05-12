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
  m_drag_lmk_ID = -1;
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

  if (IsShiftKeyOn()) 
  {
    EVec3f rayPos, rayDir, pos;
    ogl->GetCursorRay(p, rayPos, rayDir);

    m_drag_lmk_ID = PickLandmark(rayPos, rayDir, m_lmk, m_lmk_radius);
    if (m_drag_lmk_ID != -1) return;

    if (PickIsoSurface(rayPos, rayDir, pos))
    {
      m_lmk.push_back(pos);
      m_drag_lmk_ID = (int)m_lmk.size() - 1;
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
  if (IsShiftKeyOn())
  {
    EVec3f ray_pos, ray_dir, pos;
    ogl->GetCursorRay(p, ray_pos, ray_dir);
    PickToEraseLandmark(ray_pos, ray_dir, m_lmk, m_lmk_radius);
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

  if (m_drag_lmk_ID != -1)
  {
    if (PickIsoSurface(ray_pos, ray_dir, pos))
      m_lmk[m_drag_lmk_ID] = pos;
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

void ModeMdlFitMuscleModel::InitColorList()
{
  m_colorList = {
        {1.0f, 0.0f, 0.0f, 1.0f}, // 赤
        {0.0f, 1.0f, 0.0f, 1.0f}, // 緑
        {0.0f, 0.0f, 1.0f, 1.0f}, // 青
        {1.0f, 1.0f, 0.0f, 1.0f}, // 黄
        {0.0f, 1.0f, 1.0f, 1.0f}, // シアン（水色）
        {1.0f, 0.0f, 1.0f, 1.0f}, // マゼンタ（紫）
        {1.0f, 0.5f, 0.0f, 1.0f}, // オレンジ
        {0.5f, 1.0f, 0.0f, 1.0f}, // 黄緑
        {0.0f, 0.5f, 1.0f, 1.0f}, // 空色
        {0.5f, 0.0f, 1.0f, 1.0f}, // 紫
        {1.0f, 1.0f, 1.0f, 1.0f}, // 白
        {0.7f, 0.7f, 0.7f, 1.0f}  // グレー
  };
}

void ModeMdlFitMuscleModel::StartMode() 
{
  ImageCore::GetInst()->InitializeVolFlgByLockedMask();

  m_bL = m_bR = m_bM = false;
  InitColorList();
  formMdlFitMuscle_Show();

  m_drag_lmk_ID = -1;
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

void ModeMdlFitMuscleModel::DrawScene(const EVec3f& cam_pos, const EVec3f& cam_center) 
{
  BindAllVolumes();
  
  DrawCrsSec_Standard();
  glEnable(GL_LIGHTING);
  DrawColoredLandmark(m_lmk_mesh, m_lmk);
  m_isosurface.Draw(COLOR_HY, COLOR_HY, COLOR_W, COLOR_SHIN64);

  int count = 0;
  int numColors = static_cast<int>(m_colorList.size());
  glDisable(GL_LIGHTING);
  for (TMesh* mesh : m_models) {
    int colorIdx = count % numColors;
    glColor3f(m_colorList[colorIdx][0], m_colorList[colorIdx][1], m_colorList[colorIdx][2]);
    mesh->Draw();
    count++;
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


void ModeMdlFitMuscleModel::ImportObjFile(std::string fname)
{
  TMesh* newMesh = new TMesh(fname.c_str());
  if (newMesh->m_vSize > 0) {
    m_models.push_back(newMesh);
  }
  else 
  {
    delete newMesh;
  }
}

void ModeMdlFitMuscleModel::ImportAllObjInFolder(std::string folder_path)
{
  std::string search_path = folder_path + "\\*";
  WIN32_FIND_DATAA find_data;
  HANDLE hFind = FindFirstFileA(search_path.c_str(), &find_data);

  if (hFind == INVALID_HANDLE_VALUE) return;

  do
  {
    std::string file_name = find_data.cFileName;

    if (file_name == "." || file_name == "..") continue;

    std::string full_path = folder_path + "\\" + file_name;
    if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      ImportAllObjInFolder(full_path);
    }
    else
    {
      if (full_path.size() > 4 &&
        _stricmp(full_path.substr(full_path.size() - 4).c_str(), ".obj") == 0)
      {
        this->ImportObjFile(full_path);
      }
    }
  } while (FindNextFileA(hFind, &find_data));

  FindClose(hFind);

}

void ModeMdlFitMuscleModel::ModelReset()
{
  for (TMesh* mesh : m_models)
  {
    if (mesh != nullptr)
    {
      delete mesh;
    }
  }

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


void ModeMdlFitMuscleModel::ImportLandmarks(std::string fname)
{
  std::ifstream ifs(fname);
  if (ifs.fail())
  {
    std::cout << "error when opening " << "\n";
    return;
  }

  m_lmk.clear();
  int num_lmk;

  ifs >> num_lmk;
  m_lmk.resize(num_lmk);
  for (int i = 0; i < m_lmk.size(); i++)
    ReadFromFstream(ifs, m_lmk[i]);
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
  ofs << (int)m_lmk.size() << "\n";
  for (int i = 0; i < m_lmk.size(); i++)
    WriteToFstream(ofs, m_lmk[i]);
  ofs.close();
}