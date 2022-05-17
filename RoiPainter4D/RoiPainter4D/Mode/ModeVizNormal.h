#ifndef MODE_MODEVISNORM_H_
#define MODE_MODEVISNORM_H_

#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>

class ModeVizNormal : public ModeInterface
{
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;

  bool m_b_draw_cutstroke;
  std::vector<EVec3f> m_stroke;

  ModeVizNormal();
public:
  ~ModeVizNormal();
  static ModeVizNormal* GetInst() 
  { 
    static ModeVizNormal p; 
    return &p; 
  }

  // overload functions ---------------------------------------------
  MODE_ID getModeID() { return MODE_VIS_NORMAL; }
  void LBtnUp  (const EVec2i &p, OglForCLI *ogl);
  void RBtnUp  (const EVec2i &p, OglForCLI *ogl);
  void MBtnUp  (const EVec2i &p, OglForCLI *ogl);
  void LBtnDown(const EVec2i &p, OglForCLI *ogl);
  void RBtnDown(const EVec2i &p, OglForCLI *ogl);
  void MBtnDown(const EVec2i &p, OglForCLI *ogl);
  void LBtnDclk(const EVec2i &p, OglForCLI *ogl);
  void RBtnDclk(const EVec2i &p, OglForCLI *ogl);
  void MBtnDclk(const EVec2i &p, OglForCLI *ogl);
  void MouseMove(const EVec2i &p, OglForCLI *ogl);
  void MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl);
  void keyDown(int nChar);
  void keyUp(int nChar);
  bool canEndMode();
  void startMode();
  void drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);
};

#endif