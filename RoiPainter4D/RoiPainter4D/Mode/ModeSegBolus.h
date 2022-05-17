#ifndef MODE_MODESEGBOLUS_H_
#define MODE_MODESEGBOLUS_H_

#pragma unmanaged
#include "ModeInterface.h"
#include "GlslShader.h"
#include "GeneralizedCylinder.h"

// 3������ԓ��� cylinder�ƃV�[�h�������z�u�����
//  - cylinder�̒��S���͗̈�g���̃V�[�h�ɂȂ�
//  - ����ȊO�ɂ������I�ɗ̈�g���̃V�[�h��z�u�ł���

//user interface
// -- Cylinder mode -- 
// shift + L dblclick --> �I����ԂȂ� cp�ǉ�
// shift + L dblclick --> �I�𖳂��Ȃ� cylinder�ǉ�
// shift + R dblclick --> �I������cylinder��cp���폜 �i�Ō��cp�Ȃ�cylinder�폜�j
// -- seed mode -- 
// shift + L dblclick --> seed �ǉ� 
// shift + R dblclick --> seed �폜 
// --����--
// shift + M click --> cylinder �I��
// shift + L drag  --> seed���ړ� or active cylinder��cp���ړ�
//
// ctl + L drag --> cylinder�𕽍s�ړ�


class ModeSegBolus : public ModeInterface
{
private:
	GlslShaderVolume m_volume_shader_segm;
	GlslShaderVolume m_volume_shader_norm;
	GlslShaderCrsSec m_crssec_shader;
  
  //support cut stroke 
  bool m_b_draw_cutstroke;
  std::vector<EVec3f> m_stroke;

  HANDLE m_sub_thread;

	float m_cp_radius;
	std::vector< GeneralizedCylinder > m_cylinders;

  //manipuration
  int  m_active_cid ;     // -1 when false
	int  m_drag_cpid  ;     // -1 when false
  bool m_b_drag_cylinder; //active cylinder�S�̂��h���b�O

  EVec2i m_pre_pos;
	bool m_b_modified;

  //flg volume (3D)
  byte* m_vol_cyl;
	short m_thresh_max;
	short m_thresh_min;
  
  //fillvoxel�v�Z���false, cylinder���ҏW���ꂽ��true
	bool m_b_updated_cylinder;

private:
	ModeSegBolus();

public:
	static ModeSegBolus* getInst() { static ModeSegBolus p; return &p; }

public:   
	// overload functions ---------------------------------------------
	MODE_ID getModeID() { return MODE_SEG_BOLUS; }

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
	void keyDown(int nChar);
	void keyUp(int nChar){}
	bool canEndMode();
	void startMode();
	void drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);

	void FinishSegmentation();
	void cancelSegmentation();
	
	// ---------------------------------------------------------------
	void SetRadius1   ( float axis  );
	void SetRadius2   ( float axis  );
  void SetTwistAngle( float theta );

	void UpdateFormItemsForCylinder();

	void AddNewCylinder();
	void FillInCylinder();

  //3D/4D �摜����
	void RunThresholdingInClyinderOneFrame ( const short minv, const short maxv);
	void RunThresholdingInCylinderAllFrame ( const short minv, const short maxv);
	void RunRegionGrowingInCylinderOneFrame( const short minv, const short maxv);
  void RunRegionGrowingInCylinderAllFrame( const short minv, const short maxv);
	void RunRegionGrowingInCylinder4D      ( const short minv, const short maxv);
  void RunErosionOneFrame ();
  void RunErosionAllFrame ();
  void RunDilationOneFrame();
  void RunDilationAllFrame();

  //cylinder IO
	void ExportCylinderInfoByText( std::string filePath );
  void LoadCylinderInfoFromFile( std::string filePath, bool isFitting = true);

private:
	EVec2i PickCPs    (const EVec3f &ray_pos, const EVec3f &ray_dir, const float max_depth);
	
};

#endif
