#ifndef MODE_MODESEGBOLUS_H_
#define MODE_MODESEGBOLUS_H_

#pragma unmanaged
#include "ModeInterface.h"
#include "GlslShader.h"
#include "GeneralizedCylinder.h"

// 3次元空間内に cylinderとシードが複数配置される
//  - cylinderの中心軸は領域拡張のシードになる
//  - それ以外にも明示的に領域拡張のシードを配置できる

//user interface
// -- Cylinder mode -- 
// shift + L dblclick --> 選択状態なら cp追加
// shift + L dblclick --> 選択無しなら cylinder追加
// shift + R dblclick --> 選択したcylinderのcpを削除 （最後のcpならcylinder削除）
// -- seed mode -- 
// shift + L dblclick --> seed 追加 
// shift + R dblclick --> seed 削除 
// --両方--
// shift + M click --> cylinder 選択
// shift + L drag  --> seedを移動 or active cylinderのcpを移動
//
// ctl + L drag --> cylinderを平行移動


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
  bool m_b_drag_cylinder; //active cylinder全体をドラッグ

  EVec2i m_pre_pos;
	bool m_b_modified;

  //flg volume (3D)
  byte* m_vol_cyl;
	short m_thresh_max;
	short m_thresh_min;
  
  //fillvoxel計算後はfalse, cylinderが編集されたらtrue
	bool m_b_updated_cylinder;

private:
	ModeSegBolus();

public:
	static ModeSegBolus* getInst() { static ModeSegBolus p; return &p; }

public:   
	// overload functions ---------------------------------------------
	MODE_ID GetModeID() { return MODE_SEG_BOLUS; }

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
	void KeyUp(int nChar){}
	bool CanEndMode();
	void StartMode();
	void DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);

	void FinishSegmentation();
	void cancelSegmentation();
	
	// ---------------------------------------------------------------
	void SetRadius1   ( float axis  );
	void SetRadius2   ( float axis  );
  void SetTwistAngle( float theta );

	void UpdateFormItemsForCylinder();

	void AddNewCylinder();
	void FillInCylinder();

  //3D/4D 画像処理
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
