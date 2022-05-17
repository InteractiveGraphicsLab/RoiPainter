#ifndef MODECORE_H_
#define MODECORE_H_

#pragma unmanaged
#include "tmath.h"
#include "Mode/ModeInterface.h"

class OglForCLI;

class ModeCore
{
  ModeInterface* m_mode;

  ModeCore();
public:
  ~ModeCore();

  static ModeCore* GetInst() {
    static ModeCore p;
    return &p;
  }

  void ModeSwitch(MODE_ID m);
  MODE_ID getCurrentMode() { return m_mode->getModeID(); }

  void LBtnUp(const EVec2i& p, OglForCLI* ogl) { m_mode->LBtnUp(p, ogl); }
  void RBtnUp(const EVec2i& p, OglForCLI* ogl) { m_mode->RBtnUp(p, ogl); }
  void MBtnUp(const EVec2i& p, OglForCLI* ogl) { m_mode->MBtnUp(p, ogl); }
  void LBtnDown(const EVec2i& p, OglForCLI* ogl) { m_mode->LBtnDown(p, ogl); }
  void RBtnDown(const EVec2i& p, OglForCLI* ogl) { m_mode->RBtnDown(p, ogl); }
  void MBtnDown(const EVec2i& p, OglForCLI* ogl) { m_mode->MBtnDown(p, ogl); }
  void LBtnDblClk(const EVec2i& p, OglForCLI* ogl) { m_mode->LBtnDclk(p, ogl); }
  void RBtnDblClk(const EVec2i& p, OglForCLI* ogl) { m_mode->RBtnDclk(p, ogl); }
  void MBtnDblClk(const EVec2i& p, OglForCLI* ogl) { m_mode->MBtnDclk(p, ogl); }

  void MouseMove(const EVec2i& p, OglForCLI* ogl) { m_mode->MouseMove(p, ogl); }
  void MouseWheel(const EVec2i& p, short zDelta, OglForCLI* ogl) { m_mode->MouseWheel(p, zDelta, ogl); }

  void keyUp(int nChar) { m_mode->keyUp(nChar); }
  void keyDown(int nChar) { m_mode->keyDown(nChar); }

  void drawScene(const EVec3f& cuboid, const EVec3f& camP, const EVec3f& camF) {
    m_mode->drawScene(cuboid, camP, camF);
  }
};

#endif