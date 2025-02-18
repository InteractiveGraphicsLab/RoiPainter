#ifndef MODE_MODREFSTROKETRIM_H_
#define MODE_MODREFSTROKETRIM_H_
#pragma unmanaged 

#include "ModeInterface.h"
#include "GlslShader.h"

//-----------------------------------------------
// (*) User Interface 
// L double click : place / remove control point
// shift + L drag : move control point
//
// (*) vol_flg
//0 対象外
//1   : 背景
//255 : 前景
//-----------------------------------------------

class ModeRefStrokeTrim: public ModeInterface
{
	GlslShaderVolume    m_volumeShader;	
	GlslShaderCrsSec    m_crssecShader;
	std::vector<EVec3f> m_CutStroke;
	std::vector<EVec2i> m_TrimStroke;

	bool m_bModified;
	bool m_bDrawTrimStr;
    
  //cut stroke
  bool m_b_draw_cutstroke;
	bool m_bDrawCrssec;
	std::vector<EVec3f> m_stroke;

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
	void DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);
	void finishTrim();
	void cancelBtnClick();
	//-----------------------------------------------------------------

private:
	void updateVolFlgByStroke(OglForCLI &ogl);
};

#endif