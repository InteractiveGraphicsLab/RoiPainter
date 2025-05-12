#ifndef MODE_MODREFSTROKETRIM_H_
#define MODE_MODREFSTROKETRIM_H_
#pragma unmanaged 

#include "ModeInterface.h"

//-----------------------------------------------
// (*) User Interface 
// L drag  : draw trim stroke
// R click : cancel drawn stroke
// 
// (*) vol_flg
// 0   : not the target
// 1   : backgroupd
// 255 : foreground (highlighted in Green)
//-----------------------------------------------

class ModeRefStrokeTrim: public ModeInterface
{
	//strokes 
	bool m_b_draw_cutstroke;
	bool m_b_draw_trimstroke;
	std::vector<EVec2i> m_stroke2d;
	std::vector<EVec3f> m_stroke3d;

	bool m_b_modified;
	bool m_b_draw_crssec;

	byte *m_msk3D_pre;
	int   m_msk3D_pre_fi;

	ModeRefStrokeTrim();
public:
	~ModeRefStrokeTrim();
	static ModeRefStrokeTrim* GetInst(){ static ModeRefStrokeTrim p; return &p; }

	// overload functions ---------------------------------------------
	MODE_ID GetModeID() { return MODE_REF_STRKTRIM; }
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
	bool CanEndMode();
	void StartMode ();
	void DrawScene(const EVec3f &cam_pos, const EVec3f &cam_center);
	void finishTrim();
	void cancelBtnClick();
	//-----------------------------------------------------------------

private:
	void UpdateVolFlgByStroke(OglForCLI &ogl);
};

#endif