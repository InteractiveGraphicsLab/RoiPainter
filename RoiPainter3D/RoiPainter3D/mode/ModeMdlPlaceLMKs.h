#pragma once

#include "ModeInterface.h"
#include <vector>
#include <set>
#include "tmesh.h"
#include "ttrianglesoup.h"

#pragma unmanaged


class ModeMdlPlaceLMKs : public ModeInterface
{
private:
	// 等値面
	int m_isovalue;
	TTriangleSoup m_isosurface;

	// landmark
	int m_dragLmkIdx = -1;
	std::vector <EVec3f> m_lmk;
	float m_lmkRad;
	TMesh m_lmkMesh;

	ModeMdlPlaceLMKs();
public:
	static ModeMdlPlaceLMKs* GetInst() {
		static ModeMdlPlaceLMKs p;
		return &p;
	}

	// オーバーライド
	void LBtnUp(const EVec2i& p, OglForCLI* ogl) ;
	void RBtnUp(const EVec2i& p, OglForCLI* ogl) ;
	void MBtnUp(const EVec2i& p, OglForCLI* ogl) ;
	void LBtnDown(const EVec2i& p, OglForCLI* ogl) ;
	void RBtnDown(const EVec2i& p, OglForCLI* ogl) ;
	void MBtnDown(const EVec2i& p, OglForCLI* ogl) ;
	void LBtnDclk(const EVec2i& p, OglForCLI* ogl) ;
	void RBtnDclk(const EVec2i& p, OglForCLI* ogl) ;
	void MBtnDclk(const EVec2i& p, OglForCLI* ogl) ;
	void MouseMove(const EVec2i& p, OglForCLI* ogl) ;
	void MouseWheel(const EVec2i& p, short z_delta, OglForCLI* ogl);

	void KeyDown(int nChar) ;
	void KeyUp(int nChar) ;

	bool CanLeaveMode() ;

	void StartMode() ;
	void DrawScene(const EVec3f& cam_pos, const EVec3f& cam_center) ;
	//

	void GenIsoSurFace(const int isovalue);

	bool PickIsosurface(const EVec3f& rayPos, const EVec3f& rayDir, EVec3f& pos);


};

#pragma managed
