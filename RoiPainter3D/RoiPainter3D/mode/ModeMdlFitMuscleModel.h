#pragma once

#include "ModeInterface.h"
#include "tmesh.h"
#include <vector>
#include <set>
#include <string>
#include <windows.h>

class ModeMdlFitMuscleModel : public ModeInterface
{
private:
	std::vector<TMesh*> m_models;
	std::vector<std::vector<float>> m_colorList;

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

	void InitColorList();


	void ImportObjFile(std::string fname);

	void ImportAllObjInFolder(std::string folderPath);

	void ModelReset();
};

#pragma managed
