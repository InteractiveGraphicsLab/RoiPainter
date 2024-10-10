#ifndef MODE_MODPLACECPS_H_
#define MODE_MODPLACECPS_H_

#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"
#include "tmesh.h"
#include "ttrianglesoup.h"
#include <vector>
#include <set>



class ModePlaceCPs : public ModeInterface
{
  //shaders
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;

  //等値面情報
  int                        m_isovalue;
  std::vector<TTriangleSoup> m_isosurfaces;

  //Cut Stroke
  bool m_b_draw_stroke;
  std::vector<EVec3f> m_stroke;

  //control points
  int m_drag_cpid; //-1 if nothing 
  std::vector<std::vector<EVec3f>> m_cps;
  float m_cp_rad;
  TMesh m_cp_mesh;

  //template mesh
  std::vector<EVec3f> m_template_cps;
  int m_drag_tmpcpid; //-1 if nothing 
  TMesh m_template;

  std::vector<std::pair<EMat3f, EVec3f>> m_template_rottrans;//rigid transform


  ModePlaceCPs();
public:
  ~ModePlaceCPs();

  static ModePlaceCPs* GetInst() { static ModePlaceCPs p; return &p; }

  // overload functions ---------------------------------------------
  MODE_ID GetModeID() { return MODE_SEG_RIGIDICP; }
  void LBtnUp   (const EVec2i& p, OglForCLI* ogl);
  void RBtnUp   (const EVec2i& p, OglForCLI* ogl);
  void MBtnUp   (const EVec2i& p, OglForCLI* ogl);
  void LBtnDown (const EVec2i& p, OglForCLI* ogl);
  void RBtnDown (const EVec2i& p, OglForCLI* ogl);
  void MBtnDown (const EVec2i& p, OglForCLI* ogl);
  void LBtnDclk (const EVec2i& p, OglForCLI* ogl);
  void RBtnDclk (const EVec2i& p, OglForCLI* ogl);
  void MBtnDclk (const EVec2i& p, OglForCLI* ogl);
  void MouseMove(const EVec2i& p, OglForCLI* ogl);
  void MouseWheel(const EVec2i& p, short zDelta, OglForCLI* ogl);
  void KeyDown(int nChar);
  void KeyUp(int nChar);
  bool CanEndMode();
  void StartMode();
  void DrawScene(const EVec3f& cuboid, const EVec3f& camP, const EVec3f& camF);
  // ---------------------------------------------------------------

  void IsosurfaceGenerateOneFrame(const int    isovalue, const int frame_index);
  void IsosurfaceGenerateAllFrame(const int    isovalue);
  void FinishSegmentation();
  void CancelSegmentation();

  void ExportControlPoints(std::string fname);
  void ImportControlPoints(std::string fname);


  void LoadTemplateMesh(std::string fname);
  void FitTemplateUsingCPs(bool modify_scale);


private:
  bool PickPlanesIsosurf(const EVec3f &ray_pos, const EVec3f &ray_dir, EVec3f &pos);
};


#endif

