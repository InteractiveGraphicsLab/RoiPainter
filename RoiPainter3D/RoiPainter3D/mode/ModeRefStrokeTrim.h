#ifndef MODE_MODEREFSTROKETRIM_H_
#define MODE_MODEREFSTROKETRIM_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>

class ModeRefStrokeTrim : public ModeInterface
{
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;

	bool m_b_modified ;
	bool m_b_drawingstroke;
	int  m_trgt_maskid ;

  int  m_undo_idx, m_redo_max;
  unsigned short *m_undo_vol;

	std::vector<EVec2i> m_stroke2d;
	std::vector<EVec3f> m_stroke3d;

  ModeRefStrokeTrim();
public:
  ~ModeRefStrokeTrim();

  static ModeRefStrokeTrim* GetInst() { 
    static ModeRefStrokeTrim p; 
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
  void DrawScene(const EVec3f &cuboid, const EVec3f &cam_pos, const EVec3f &cam_center);

  void cancelSegmentation();
  void finishSegmentation();

private:
  void UpdateVolFlgByStroke( OglForCLI *ogl);
};


#endif