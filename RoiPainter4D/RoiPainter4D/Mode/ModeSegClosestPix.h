#ifndef MODE_MODESEGCLOSESTPIX_H_
#define MODE_MODESEGCLOSESTPIX_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>

//豆苗プロジェクトからコピーする予定
//前のフレームの最も近いIDをつけていくアルゴリズム
//TODO 未実装

class ModeSegClosestPix : public ModeInterface
{
  GlslShaderVolume m_volumeShader;
  GlslShaderCrsSec m_crssecShader;

  bool                m_bDrawStr;
  std::vector<EVec3f> m_stroke;

  ModeSegClosestPix();
public:
  ~ModeSegClosestPix();

  static ModeSegClosestPix* GetInst() { static ModeSegClosestPix p; return &p; }

  // overload functions ---------------------------------------------
  MODE_ID GetModeID() { return MODE_SEG_CLOSESTPIX; }

  void LBtnUp   (const EVec2i &p, OglForCLI *ogl);
  void RBtnUp   (const EVec2i &p, OglForCLI *ogl);
  void MBtnUp   (const EVec2i &p, OglForCLI *ogl);
  void LBtnDown (const EVec2i &p, OglForCLI *ogl);
  void RBtnDown (const EVec2i &p, OglForCLI *ogl);
  void MBtnDown (const EVec2i &p, OglForCLI *ogl);
  void LBtnDclk (const EVec2i &p, OglForCLI *ogl);
  void RBtnDclk (const EVec2i &p, OglForCLI *ogl);
  void MBtnDclk (const EVec2i &p, OglForCLI *ogl);
  void MouseMove(const EVec2i &p, OglForCLI *ogl);
  void MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl);

  void KeyDown(int nChar);
  void KeyUp(int nChar);

  bool CanEndMode();
  void StartMode();
  void DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);

  void runSegClosestPix(const int &startI, const int &endI);

  void finishSegmentation();
  void cancelSegmentation();
};

#endif

