#pragma unmanaged
#include "ModeSegThreshfieldPaint.h"
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormVisNorm.h"

#pragma unmanaged

using namespace RoiPainter3D;

ModeSegThreshfieldPaint::~ModeSegThreshfieldPaint()
{

}

ModeSegThreshfieldPaint::ModeSegThreshfieldPaint() :
  m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg.glsl"),
  m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
{
}




bool ModeSegThreshfieldPaint::CanLeaveMode()
{
  return true;
}

void ModeSegThreshfieldPaint::StartMode()
{
  //Lock/Unlock pitch box 
  formVisParam_UnlockPitchBox();
}


void ModeSegThreshfieldPaint::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegThreshfieldPaint::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegThreshfieldPaint::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegThreshfieldPaint::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegThreshfieldPaint::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegThreshfieldPaint::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegThreshfieldPaint::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{


}


void ModeSegThreshfieldPaint::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegThreshfieldPaint::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


void ModeSegThreshfieldPaint::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegThreshfieldPaint::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
}



void ModeSegThreshfieldPaint::KeyDown(int nChar) {}
void ModeSegThreshfieldPaint::KeyUp(int nChar) {}


void ModeSegThreshfieldPaint::DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
}

