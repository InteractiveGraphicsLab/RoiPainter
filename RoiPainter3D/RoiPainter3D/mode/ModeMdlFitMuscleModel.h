#pragma once

#include "ModeInterface.h"
#include "tmesh.h"
#include "ttrianglesoup.h"
#include <vector>
#include <set>
#include <string>
#include <map>
#include <windows.h>

class ModeMdlFitMuscleModel : public ModeInterface
{
private:
	// 等値面
	int m_isovalue;
	TTriangleSoup m_isosurface;

	// Landmark
	int m_drag_iso_lmk_ID;
	int m_drag_model_lmk_ID;
	std::vector <EVec3f> m_iso_lmks;
	std::vector <EVec3f> m_model_lmks;
	float m_lmk_radius;
	TMesh m_lmk_mesh;

	// .obj model
	std::vector<TMesh*> m_models;
	std::map<TMesh*, bool> m_model_visibility_map;


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



	void ModelReset();

	TMesh* ImportObjFile(std::string fname);
	
	bool IsModelVisible(TMesh* mesh);

	void GenIsoSurface(const int isovalue, const bool do_halfen);

	bool PickIsoSurface(const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f& pos);
	bool PickObjModels(const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f& pos);

	void ImportLandmarks(std::string fname);
	void ExportLandmarks(std::string fname);
	
	void ModelPCATranslate();

	//void DrawPrincipalAxis(std::vector<EVec3f> lmks, float length);

	// setter
	void SetModelVisibility(TMesh* mesh, const bool isVisible);
};

#pragma managed
