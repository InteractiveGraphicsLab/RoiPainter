#ifndef MODE_MODESEGRGROW_H_
#define MODE_MODESEGRGROW_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"

//-----------------------------------------------
// (*) User Interface 
// shift + L click : place control point
// shift + L drag  : move  control point
// shift + R click : remove control point
//
// (*) vol_flg[i]
// 0   : not the target
// 1   : backgroupd
// 255 : foreground (highlighted in Green)
//-----------------------------------------------

class ModeSegRGrow : public ModeInterface
{
  GlslShaderVolume  m_volume_shader;
  GlslShaderCrsSec  m_crssec_shader;

  //control points 
  float m_cp_radius;
  int  m_drag_cpid; //-1 when false
  std::vector<CtrlPt4D>  m_cps;

  //cut stroke
  bool m_b_draw_cutstroke;
  std::vector<EVec3f> m_stroke;

  //others
  bool m_b_modified;

  ModeSegRGrow();
public:
  ~ModeSegRGrow();

  static ModeSegRGrow* GetInst() { static ModeSegRGrow p; return &p; }
  
  // overload functions ---------------------------------------------
  MODE_ID GetModeID() { return MODE_SEG_REGGROW; }

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
  void MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl);
  void KeyDown(int nChar);
  void KeyUp (int nChar);
  bool CanEndMode();
  void StartMode();
  void DrawScene(const EVec3f &cuboid, const EVec3f &cam_pos, const EVec3f &cam_center);
  //-----------------------------------------------------------------

  void RunThresholding(const short minv, const short maxv);
  void RunRegionGrow8 (const short minv, const short maxv);
  void RunDilation3D_EachFrame();
  void RunErosion3D_EachFrame();
  void RunFillHole3D_EachFrame();
  void RunThresholding_OneFrame(const short minv, const short maxv, const int frame_idx);
  void RunRegionGrow6_OneFrame (const short minv, const short maxv, const int frame_idx);

  void FinishSegmentation();
  void cancelSegmentation();

private:
  int PickControlPoints(const EVec3f &ray_pos, const EVec3f &ray_dir);
};

#endif