#ifndef MODE_MODESEGPIXPAINT_H_
#define MODE_MODESEGPIXPAINT_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"

//UI 
// ここにUIの概要を書く
// shift + L drag : paint voxel / drawa lasso for foreground
// shift + R drag : paint voxel / drawa lasso for background
//
//

//vol_flg
//0   : 対象外
//1   : 背景
//255 : 緑色ハイライト


//TODO
//used for refinement

class ModeSegPixPaint : public ModeInterface
{

  GlslShaderVolume    m_volume_shader;
  GlslShaderCrsSec    m_crssec_shader;

  //lasso
  CRSSEC_ID  m_trgt_crssecid;
  bool m_b_drawlasso;
  std::vector<EVec3f>  m_lasso;

  //cut stroke
  bool m_b_paintpixels;
  std::vector<EVec4i> m_paintvoxels;

  //others
  bool m_b_modified;


  ModeSegPixPaint();
public:
  ~ModeSegPixPaint();

  static ModeSegPixPaint* GetInst() { static ModeSegPixPaint p; return &p; }

  // overload functions ---------------------------------------------
  MODE_ID GetModeID() { return MODE_SEG_PIXPAINT; }

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
  //-----------------------------------------------------------------

  void FinishSegmentation();
  void CancelSegmentation();

};

#endif

