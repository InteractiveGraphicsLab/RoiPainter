#ifndef MODE_MODEVISMASK_H_
#define MODE_MODEVISMASK_H_

#include "ModeInterface.h"
#include "GlslShader.h"
#include "MaskMeshSequence.h"
#include "tmesh.h"


class ModeVizMask : public ModeInterface
{
  GlslShaderVolume m_volumeShader;
  GlslShaderCrsSec m_crssecShader;
  
  MaskMeshSequence m_mask_mesh;
  std::vector<TMesh> m_tmeshes;

  ModeVizMask();
public:
  ~ModeVizMask();
  static ModeVizMask* GetInst() { static ModeVizMask p; return &p; }

  // overload functions ---------------------------------------------
  MODE_ID GetModeID() { return MODE_VIS_MASK; }
  void LBtnUp(const EVec2i &p, OglForCLI *ogl);
  void RBtnUp(const EVec2i &p, OglForCLI *ogl);
  void MBtnUp(const EVec2i &p, OglForCLI *ogl);
  void LBtnDown(const EVec2i &p, OglForCLI *ogl);
  void RBtnDown(const EVec2i &p, OglForCLI *ogl);
  void MBtnDown(const EVec2i &p, OglForCLI *ogl);
  void LBtnDclk(const EVec2i &p, OglForCLI *ogl);
  void RBtnDclk(const EVec2i &p, OglForCLI *ogl);
  void MBtnDclk(const EVec2i &p, OglForCLI *ogl);
  void MouseMove(const EVec2i &p, OglForCLI *ogl);
  void MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl);
  void KeyDown(int nChar);
  void KeyUp(int nChar);
  bool CanEndMode();
  void StartMode();
  void DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);
};

#endif