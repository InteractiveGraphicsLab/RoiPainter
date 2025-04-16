#ifndef MODE_INTERFACE_H_
#define MODE_INTERFACE_H_
#pragma unmanaged

#include "tmath.h"
#include <windows.h> 
#undef min
#undef max

class OglForCLI;

enum MODE_ID
{
	MODE_VIS_NORMAL    , // ModeVizNormal       
	MODE_VIS_MASK      , // ModeVizMask         

	MODE_REF_STRKTRIM,  // ModeRefStrkTrim     
	MODE_REF_CURVEDEFORM,// ModeRefCurveDeform, FormRefCurveDeform (by Yuki Kimura, 2024)

	MODE_SEG_PIXPAINT  , // ModeSegPixPaint
	MODE_SEG_REGGROW   , // ModeSegRegGrow     
	MODE_SEG_RIGIDICP  , // ModeSegRigidICP    (SJTrackerより)
	MODE_SEG_LCLRGROW ,  // ModeSegLocalRGrow (Local Region Growing by Hikaru Shionozaki)
	MODE_SEG_BOLUS    ,  // ModeSegBolus      (by shionzaki )
  MODE_SEG_JTRACKER ,  // ModeJontTracker   (For Hokkaidou Univ)
  MODE_SEG_SWALLOW  ,  // ModeSegSwallowOrgans
  MODE_SEG_BRONCHI  ,  // ModeSegBronchi   (by Takata) FormSegModeBronchi
	MODE_SEG_STROKEFFD,  // ModeSegStrokeFfd, FormSegStrokeFfd (by Yuki Kimura, 20220524)

	MODE_PLC_CPS,        // ModePlaceCps    , FormPlaceCPs     tool for landmark placement

	//MODE_SEG_PARACONT ,// ModeSegParaConts(Parallel Contours by Chika Tomiyama, Removed 20220517)
	//MODE_SEG_GCUT   ,  // ModeSegGGut           
	//MODE_SEG_THRESHPNT,// ModeSegTreshPnt      
};



//refactoring 4/15 
//ModeVisNormal
//ModeVisMask
//ModeSegRGrow
//ModeSegLocalRegionGrow
//ModeSegBolus (Local Region Grow cylinder)
//ModeSegPixelPaint




//red green blue
static float COLOR_R [4] = {1.0f, 0.0f, 0.0f, 0.5f};
static float COLOR_RH[4] = {0.5f, 0.0f, 0.0f, 0.5f};
static float COLOR_G [4] = {0.0f, 1.0f, 0.0f, 0.5f};
static float COLOR_GH[4] = {0.0f, 0.5f, 0.0f, 0.5f};
static float COLOR_B [4] = {0.0f, 0.0f, 1.0f, 0.5f};
static float COLOR_BH[4] = {0.0f, 0.0f, 0.5f, 0.5f};

//yellow , magenda, cyan
static float COLOR_Y [4] = {1.0f, 1.0f, 0.0f, 0.5f};
static float COLOR_YH[4] = {0.5f, 0.5f, 0.0f, 0.5f};
static float COLOR_M [4] = {1.0f, 0.0f, 1.0f, 0.5f};
static float COLOR_MH[4] = {0.5f, 0.0f, 0.5f, 0.5f};
static float COLOR_C [4] = {0.0f, 1.0f, 1.0f, 0.5f};
static float COLOR_CH[4] = {0.0f, 0.5f, 0.5f, 0.5f};

static float COLOR_W   [4] = {1.0f, 1.0f, 1.0f, 0.5f};
static float COLOR_GRAY[4] = {0.5f, 0.5f, 0.5f, 0.5f};

static float COLOR_SHIN64[1] = {64};



class ModeInterface
{
protected:
	bool m_bL, m_bM, m_bR;

	ModeInterface() { m_bL = m_bR = m_bM = false; }

public:
	virtual MODE_ID GetModeID() = 0;
	virtual void LBtnUp		 (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void RBtnUp		 (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void MBtnUp		 (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void LBtnDown	 (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void RBtnDown	 (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void MBtnDown	 (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void LBtnDclk	 (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void RBtnDclk	 (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void MBtnDclk	 (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void MouseMove (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl) = 0;

	virtual void KeyDown(int nChar) = 0;
	virtual void KeyUp  (int nChar) = 0;

	//this function is called before switch the mode (if return false, the mode will not be switched)
	virtual bool CanEndMode() = 0;
	
  //this function is called just after switch the mode
	virtual void StartMode() = 0;

	virtual void DrawScene(const EVec3f &cam_pos, const EVec3f &cam_center) = 0;

protected:

	enum REND_MODE 
	{
		RM_NORMAL , 
		RM_HILIGHT, 
		RM_MASK
	};

	//Render Cross sections (should be called acter BindAllVolumes)
	void DrawCrossSectionsNormal (); 
	void DrawCrossSectionsVisMask(bool do_hilight); 
	void DrawCrossSectionsVisFore(bool do_hilight);

	//Render Volumes
	void DrawVolumeNormal (
		const EVec3f& cam_pos, 
		const EVec3f& cam_cnt,
		bool do_coarse_rend_onmanip = true);

	void DrawVolumeVisMask(
		bool do_hilight, //mask可視化 on/off
		const EVec3f& cam_pos, 
		const EVec3f& cam_cnt, 
		bool do_coarse_rend_onmanip = true);

	void DrawVolumeVisFore(
		bool is_hilight, //fore可視化 on/off
		const EVec3f& cam_pos, 
		const EVec3f& cam_cnt, 
		bool do_coarse_rend_onmanip = true);


private:

};



class CtrlPt4D
{
public:
	EVec3f pos;
	int    frameI;

	CtrlPt4D() {
	}

	CtrlPt4D(const int _frameI, const EVec3f &_pos) {
		frameI = _frameI;
		pos    = _pos;
	}

	CtrlPt4D(const CtrlPt4D &src) {
		pos = src.pos;
		frameI = src.frameI;
	}

	CtrlPt4D& operator=(const CtrlPt4D& src) {
		pos = src.pos;
		frameI = src.frameI;
		return *this;
	}
};



class PixIdx4D {
public:
	int idx   ;
	int frameI;

	PixIdx4D() {
	}

	PixIdx4D(const int &_idx, const int &_frame) {
		idx = _idx;
		frameI = _frame;
	}

	PixIdx4D(const PixIdx4D &src) {
		idx = src.idx;
		frameI = src.frameI;
	}

	PixIdx4D& operator=(const PixIdx4D& src) {
		frameI = src.frameI;
		idx = src.idx;
		return *this;
	}
};


inline bool IsCtrKeyOn  () { return GetKeyState(VK_CONTROL) < 0; }
inline bool IsSpaceKeyOn() { return GetKeyState(VK_SPACE  ) < 0; }
inline bool IsShiftKeyOn() { return GetKeyState(VK_SHIFT  ) < 0; }
inline bool IsAltKeyOn  () { return GetKeyState(VK_MENU   ) < 0; }
//inline bool IsAKeyOn  ()   { return GetKeyState(0x41) < 0; }
//inline bool IsBKeyOn  ()   { return GetKeyState(0x42) < 0; }
inline bool IsCKeyOn  ()   { return GetKeyState(0x43) < 0; }
//inline bool IsDKeyOn  ()   { return GetKeyState(0x44) < 0; }
//inline bool IsEKeyOn  ()   { return GetKeyState(0x45) < 0; }
//inline bool IsFKeyOn  ()   { return GetKeyState(0x46) < 0; }
//inline bool IsGKeyOn  ()   { return GetKeyState(0x47) < 0; }
//inline bool IsHKeyOn  ()   { return GetKeyState(0x48) < 0; }
//inline bool IsIKeyOn  ()   { return GetKeyState(0x49) < 0; }
//inline bool IsJKeyOn  ()   { return GetKeyState(0x4A) < 0; }
//inline bool IsKKeyOn  ()   { return GetKeyState(0x4B) < 0; }
//inline bool IsLKeyOn  ()   { return GetKeyState(0x4C) < 0; }
inline bool IsMKeyOn  ()   { return GetKeyState(0x4D) < 0; }
//inline bool IsNKeyOn  ()   { return GetKeyState(0x4E) < 0; }
//inline bool IsOKeyOn  ()   { return GetKeyState(0x4F) < 0; }
//inline bool IsPKeyOn  ()   { return GetKeyState(0x50) < 0; }
//inline bool IsQKeyOn  ()   { return GetKeyState(0x51) < 0; }
//inline bool IsRKeyOn  ()   { return GetKeyState(0x52) < 0; }
inline bool IsSKeyOn  ()   { return GetKeyState(0x53) < 0; }
//inline bool IsTKeyOn  ()   { return GetKeyState(0x54) < 0; }
//inline bool IsUKeyOn  ()   { return GetKeyState(0x55) < 0; }
//inline bool IsVKeyOn  ()   { return GetKeyState(0x56) < 0; }
//inline bool IsWKeyOn  ()   { return GetKeyState(0x57) < 0; }
//inline bool IsXKeyOn  ()   { return GetKeyState(0x58) < 0; }
//inline bool IsYKeyOn  ()   { return GetKeyState(0x59) < 0; }
//inline bool IsZKeyOn  ()   { return GetKeyState(0x5A) < 0; }
//inline bool IsTabKeyOn  (){ return GetKeyState( VK_TAB     ) < 0 ; }

// Common functions for Mode*.cpp


#endif

