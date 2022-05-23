
#include "ModeSegGCut.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormVisNorm.h"


using namespace RoiPainter3D;

#pragma unmanaged


ModeSegGCut::~ModeSegGCut()
{

}

ModeSegGCut::ModeSegGCut() :
  m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg.glsl"),
  m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
{
}




bool ModeSegGCut::canEndMode()
{
}

void ModeSegGCut::startMode()
{
}


void ModeSegGCut::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegGCut::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegGCut::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegGCut::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegGCut::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegGCut::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegGCut::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{


}


void ModeSegGCut::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegGCut::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


void ModeSegGCut::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegGCut::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
}



void ModeSegGCut::keyDown(int nChar) {}
void ModeSegGCut::keyUp(int nChar) {}


void ModeSegGCut::drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
}

#pragma managed