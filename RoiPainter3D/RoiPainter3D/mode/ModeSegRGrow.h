#ifndef MODE_MODESEGRGROW_H_
#define MODE_MODESEGRGROW_H_
#pragma unmanaged

//-----------------------------------------------------------------------------
// class ModeSegRGrow 
// Region growing / thresholding によるsegmentationを行なうモード
// 
//-----------------------------------------------------------------------------

#include "ModeInterface.h"
#include "tmesh.h"
#include <vector>

class ModeSegRGrow : public ModeInterface
{
  bool  m_b_drawstroke;
  bool  m_b_roi_update;
  int   m_drag_cp_id; // -1 : not dragging 

  std::vector<EVec3f> m_stroke;
	std::vector<EVec3f> m_cps;
  float m_cp_radius;

  ModeSegRGrow();
public:
  ~ModeSegRGrow();

  static ModeSegRGrow* GetInst() 
  { 
    static ModeSegRGrow p; 
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
  void MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl);
  void KeyDown(int nChar);
  void KeyUp  (int nChar);
  bool CanLeaveMode();
  void StartMode   ();
  void DrawScene( const EVec3f &cam_pos, const EVec3f &cam_center);

public:
  void RunThresholding(short minV, short maxV);
  void RunRegionGrow6 (short minV, short maxV);
  void RunRegionGrow26(short minV, short maxV);
  void RunErosion();
  void RunDilation();
  void RunFillHole();

  void FinishSegmentation();

};


#endif
