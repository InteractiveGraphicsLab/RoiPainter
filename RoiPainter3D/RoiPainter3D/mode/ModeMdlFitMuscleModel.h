#pragma once

#include "ModeInterface.h"

class ModeMdlFitMuscleModel : public ModeInterface
{

	ModeMdlFitMuscleModel();
public:
	static ModeMdlFitMuscleModel* GetInst()
	{
		static ModeMdlFitMuscleModel p;
		return &p;
	}

	// オーバーライド
	void LBtnUp(const EVec2i& p, OglForCLI* ogl);
	void RBtnUp(const EVec2i& p, OglForCLI* ogl);
	void MBtnUp(const EVec2i& p, OglForCLI* ogl);
	void LBtnDown(const EVec2i& p, OglForCLI* ogl);
	void RBtnDown(const EVec2i& p, OglForCLI* ogl);
	void MBtnDown(const EVec2i& p, OglForCLI* ogl);
	void LBtnDclk(const EVec2i& p, OglForCLI* ogl);
	void RBtnDclk(const EVec2i& p, OglForCLI* ogl);
	void MBtnDclk(const EVec2i& p, OglForCLI* ogl);
	void MouseMove(const EVec2i& p, OglForCLI* ogl);
	void MouseWheel(const EVec2i& p, short z_delta, OglForCLI* ogl);

	void KeyDown(int nChar);
	void KeyUp(int nChar);

	bool CanLeaveMode();
	void StartMode();

	void DrawScene(const EVec3f& cam_pos, const EVec3f& cam_center);
	//

	void FinishSegmentation();

};

#pragma managed
