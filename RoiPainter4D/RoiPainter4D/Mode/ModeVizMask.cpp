#pragma unmanaged 
#include "ModeVizMask.h"
#include "OglForCLI.h"
#include "ImageCore.h"
#include "ModeCommonTools.h"

#pragma managed 
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormVisMask.h"
#pragma unmanaged 


using namespace RoiPainter4D;


ModeVizMask::ModeVizMask() :
  m_volumeShader("shader/volVtx.glsl", "shader/volFlg_Msk.glsl"),
  m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg_Msk.glsl")
{
  std::cout << "ModeVizMask const...----------------------\n";

  m_bL = m_bR = m_bM = false;
 
  std::cout << "ModeVizMask DONE -------------------------\n";
}

ModeVizMask::~ModeVizMask()
{
}


bool ModeVizMask::CanEndMode()
{
  return true;
}


void ModeVizMask::StartMode()
{
  m_bL = m_bR = m_bM = false;
  m_mask_mesh = MaskMeshSequence();
  m_tmeshes = std::vector<TMesh>(ImageCore::GetInst()->GetNumFrames());
  const int scale = 2;
  if (!m_mask_mesh.LoadMask(scale))
  {
      std::cout << "Failed to load mask." << "\n";
  }
  for (int i = 0; i < ImageCore::GetInst()->GetNumFrames(); ++i) {
      m_tmeshes[i] = m_mask_mesh.GetMesh(i);
      std::cout << "got meshes." << "\n";
  }
  
  UpdateImageCoreVisVolumes();
  formVisMask_Show();
  formVisMask_updateList();
}


////////////////////////////////////////////////////////////////////////////
//MouseListener/////////////////////////////////////////////////////////////
void ModeVizMask::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;
  ogl->BtnDown_Trans(p);
}

void ModeVizMask::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeVizMask::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
  ogl->BtnDown_Rot(p);
}

void ModeVizMask::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeVizMask::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}

void ModeVizMask::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeVizMask::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizMask::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizMask::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}

void ModeVizMask::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  ogl->MouseMove(p);
  formMain_RedrawMainPanel();
}



void ModeVizMask::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  if(!WheelingCrssec(p, z_delta, ogl) ) {
    ogl->ZoomCamera(z_delta * 0.1f);
  }
  formMain_RedrawMainPanel();
}


void ModeVizMask::KeyDown(int nChar) {}
void ModeVizMask::KeyUp(int nChar) {}


void ModeVizMask::DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
  const EVec3i reso = ImageCore::GetInst()->GetReso(); //‰ð‘œ“xŽæ“¾

  ImageCore::GetInst()->UpdateImgMaskColor();

  BindAllVolumes();

  DrawCrossSections( cuboid, reso, !IsSpaceKeyOn(), m_crssecShader);

  if ( formVisParam_bRendVol() )
  {
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    bool b_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolumeSlices( cuboid, reso, camP, camF, 
                    !IsSpaceKeyOn(), b_manip, m_volumeShader);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }
  
}

