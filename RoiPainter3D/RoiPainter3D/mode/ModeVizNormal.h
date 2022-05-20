#ifndef MODE_MODEVISNORMAL_H_
#define MODE_MODEVISNORMAL_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>

class ModeVizNormal : public ModeInterface
{
  GlslShaderVolume m_volumeShader;
  GlslShaderCrsSec m_crssecShader;

  bool m_bDrawStr;
  std::vector<EVec3f> m_stroke;

  ModeVizNormal();
public:
  ~ModeVizNormal();

  static ModeVizNormal* getInst() { 
    static ModeVizNormal p; 
    return &p; 
  }

  // overload functions ---------------------------------------------

  void LBtnUp    (const EVec2i &p, OglForCLI *ogl);
  void RBtnUp    (const EVec2i &p, OglForCLI *ogl);
  void MBtnUp    (const EVec2i &p, OglForCLI *ogl);
  void LBtnDown  (const EVec2i &p, OglForCLI *ogl);
  void RBtnDown  (const EVec2i &p, OglForCLI *ogl);
  void MBtnDown  (const EVec2i &p, OglForCLI *ogl);
  void LBtnDclk  (const EVec2i &p, OglForCLI *ogl);
  void RBtnDclk  (const EVec2i &p, OglForCLI *ogl);
  void MBtnDclk  (const EVec2i &p, OglForCLI *ogl);
  void MouseMove (const EVec2i &p, OglForCLI *ogl);
  void MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl);

  void KeyDown(int nChar);
  void KeyUp  (int nChar);

  bool CanLeaveMode();
  void StartMode ();
  void DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);

};

#endif

