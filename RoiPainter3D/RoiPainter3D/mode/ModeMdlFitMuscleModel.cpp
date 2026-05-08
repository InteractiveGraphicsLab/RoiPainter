#include "ModeMdlFitMuscleModel.h"
#include "OglForCLI.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "ModeCore.h"

#pragma managed
#include "FormMdlFitMuscleModel.h"
#include "../FormVisParam.h"
#include "../FormMain.h"
#pragma unmanaged


using namespace RoiPainter3D;


ModeMdlFitMuscleModel::ModeMdlFitMuscleModel()
{
  m_bL = m_bR = m_bM = false;
}


// オーバーライド
void ModeMdlFitMuscleModel::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = false;
  ogl->BtnUp();
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

  formVisParam_UnlockPitchBox();

}



void ModeMdlFitMuscleModel::DrawScene(const EVec3f& cam_pos, const EVec3f& cam_center) 
{
  BindAllVolumes();
  
  DrawCrsSec_Standard();

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