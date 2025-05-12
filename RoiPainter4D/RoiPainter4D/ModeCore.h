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
  MODE_ID getCurrentMode() { return m_mode->GetModeID(); }

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

  void KeyUp(int nChar) { m_mode->KeyUp(nChar); }
  void KeyDown(int nChar) { m_mode->KeyDown(nChar); }

  void DrawScene(const EVec3f& cam_pos, const EVec3f& cam_center) {
    m_mode->DrawScene(cam_pos, cam_center);
  }
};

#endif