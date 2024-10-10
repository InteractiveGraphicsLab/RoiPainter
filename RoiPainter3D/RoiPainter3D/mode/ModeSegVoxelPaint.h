#ifndef MODE_MODESEGVOXELPAINT_H_
#define MODE_MODESEGVOXELPAINT_H_
#pragma unmanaged

// class ModeSegVoxelPaint
// support 
//   1. Segmentation Voxel paint  : paint modeである領域を生成する 
//   2. Refinement   Voxel paint  : paint modeである領域を修正する
//
// 二種類のペイントモード（直接ペイント・らっそ）をサポートする

#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>


class ModeSegVoxelPaint : public ModeInterface
{
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;

  // field for MODE_REF_VOXEL_PAINT
  bool m_b_refinementmode;
  int  m_refine_maskid   ; 

  //mouse manipuration
	bool       m_b_paintmode;
	bool       m_b_lassomode ;
  CRSSEC_ID  m_trgt_crssecid;
	std::vector<EVec3f>  m_lasso;
	std::vector<EVec4i>  m_paint_voxels;

  ModeSegVoxelPaint();
public:
  ~ModeSegVoxelPaint();

  static ModeSegVoxelPaint* GetInst() { 
    static ModeSegVoxelPaint p; 
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

  void FinishSegmentation();
  void CancelSegmentation();
};

#endif

