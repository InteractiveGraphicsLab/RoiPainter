#include "ModeMdlFitMuscleModel.h"

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
void ModeMdlFitMuscleModel::LBtnUp(const EVec2i& p, OglForCLI* ogl){}
void ModeMdlFitMuscleModel::RBtnUp(const EVec2i& p, OglForCLI* ogl) {}
void ModeMdlFitMuscleModel::MBtnUp(const EVec2i& p, OglForCLI* ogl) {}
void ModeMdlFitMuscleModel::LBtnDown(const EVec2i& p, OglForCLI* ogl) {}
void ModeMdlFitMuscleModel::RBtnDown(const EVec2i& p, OglForCLI* ogl) {}
void ModeMdlFitMuscleModel::MBtnDown(const EVec2i& p, OglForCLI* ogl) {}
void ModeMdlFitMuscleModel::LBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeMdlFitMuscleModel::RBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeMdlFitMuscleModel::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeMdlFitMuscleModel::MouseMove(const EVec2i& p, OglForCLI* ogl) 
{
  if (!m_bL && !m_bR && !m_bM) return;
  RedrawScene();
}
void ModeMdlFitMuscleModel::MouseWheel(const EVec2i& p, short z_delta, OglForCLI* ogl) {}

void ModeMdlFitMuscleModel::KeyDown(int nChar) {}
void ModeMdlFitMuscleModel::KeyUp(int nChar) {}

bool ModeMdlFitMuscleModel::CanLeaveMode() {return true;}
void ModeMdlFitMuscleModel::StartMode() 
{
  formMdlFitMuscle_Show();

}

void ModeMdlFitMuscleModel::DrawScene(const EVec3f& cam_pos, const EVec3f& cam_center) {}
//

void ModeMdlFitMuscleModel::FinishSegmentation() {}
