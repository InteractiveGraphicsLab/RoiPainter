#ifndef MODE_MODSEGSWALLOWTEMPGEN_H_
#define MODE_MODSEGSWALLOWTEMPGEN_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"
#include "tmath.h"
#include "tmesh.h"
#include "CagedMeshSequence.h"

enum SWALLOW_ORGAN_TYPE {
  SOT_ZETSU,
  SOT_NANK ,
  SOT_INTO,
  SOT_KOTO
};


class ModeSegSwallowTempGen : public ModeInterface
{
private:
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;
  

  float m_HANDLE_L, m_HANDLE_W;

  EVec2i m_pre_point;
  ORTHO_HANDLE_ID  m_drag_handle_id;
  
  
  bool                 m_b_draw_cutstroke;
  std::vector<EVec3f>  m_stroke;

  //input template 
  CagedMeshSequence m_cagedmesh_zetsu_orig; //ê„
  CagedMeshSequence m_cagedmesh_into_orig ; //àÙì™
  CagedMeshSequence m_cagedmesh_koto_orig ; //å˚ì™
  CagedMeshSequence m_cagedmesh_nank_orig ; //ìÓå˚äW

  //ïœå`å„
  CagedMeshSequence m_cagedmesh_zetsu; //ê„
  CagedMeshSequence m_cagedmesh_into ; //àÙì™
  CagedMeshSequence m_cagedmesh_koto ; //å˚ì™
  CagedMeshSequence m_cagedmesh_nank ; //ìÓå˚äW

  ModeSegSwallowTempGen();

public:
  ~ModeSegSwallowTempGen();

  static ModeSegSwallowTempGen* getInst() { 
    static ModeSegSwallowTempGen p; 
    return &p; 
  }

  // overload functions ---------------------------------------------
  MODE_ID getModeID() { return MODE_VIS_NORMAL; }

  void LBtnUp    (const EVec2i& p, OglForCLI* ogl);
  void RBtnUp    (const EVec2i& p, OglForCLI* ogl);
  void MBtnUp    (const EVec2i& p, OglForCLI* ogl);
  void LBtnDown  (const EVec2i& p, OglForCLI* ogl);
  void RBtnDown  (const EVec2i& p, OglForCLI* ogl);
  void MBtnDown  (const EVec2i& p, OglForCLI* ogl);
  void LBtnDclk  (const EVec2i& p, OglForCLI* ogl);
  void RBtnDclk  (const EVec2i& p, OglForCLI* ogl);
  void MBtnDclk  (const EVec2i& p, OglForCLI* ogl);
  void MouseMove (const EVec2i& p, OglForCLI* ogl);
  void MouseWheel(const EVec2i& p, short zDelta, OglForCLI* ogl);

  void keyDown(int nChar);
  void keyUp(int nChar);

  bool canEndMode();
  void startMode();
  void drawScene(const EVec3f& cuboid, const EVec3f& camP, const EVec3f& camF);
  
  void LoadCagedMesh( const std::string &mesh_path, 
                      const std::string &cage_path, 
                      const std::string &deform_path,
                      const SWALLOW_ORGAN_TYPE id);
  void UpdateCageByTiming(std::string fname_time_orig, int f1, int f2, int f3, int f4);
  void ExportCageMotionData(std::string fname);

private:
  EVec3f GetGravCenterTrgtCages(int frame_idx) ;
  void TranslateTargetCages(EVec2i p0, EVec2i p1, ORTHO_HANDLE_ID id, OglForCLI* ogl);
  void RotateTargetCages   (EVec2i p0, EVec2i p1, ORTHO_HANDLE_ID id, OglForCLI* ogl);
  void ScaleTargetCages    (EVec2i p0, EVec2i p1, ORTHO_HANDLE_ID id, OglForCLI* ogl);
};

#endif
