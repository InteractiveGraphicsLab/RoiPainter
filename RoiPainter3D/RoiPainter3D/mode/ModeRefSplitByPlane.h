#ifndef MODEREFSPLITBYPLANE_H_
#define MODEREFSPLITBYPLANE_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>
#include "tmesh.h"

class ModeRefSplitByPlane : public ModeInterface
{
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;

  //target mask id 
  int m_trgt_maskid;
  bool m_is_updated;

  //control points (cps)
  TMesh m_cp_sphere;
  float m_cp_radi;
  std::vector<EVec3f> m_cps;
  int m_drag_cpid;

  //cut stroke 
  bool m_b_drawstroke;
  std::vector<EVec3f> m_stroke;

  //plane 
  EVec3f m_plane_pos, m_plane_x, m_plane_y, m_plane_norm;
  TMesh  m_plane;


  ModeRefSplitByPlane();
public:
  ~ModeRefSplitByPlane();

  static ModeRefSplitByPlane* GetInst() {
    static ModeRefSplitByPlane p;
    return &p;
  }

  // overload functions ---------------------------------------------

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
  void MouseWheel(const EVec2i& p, short zDelta, OglForCLI* ogl);
  
  void StartMode();
  bool CanLeaveMode();

  void KeyDown(int nChar);
  void KeyUp(int nChar);
  void DrawScene(const EVec3f& cuboid, const EVec3f& cam_pos, const EVec3f& cam_center);
  void cancelSegmentation();
  void finishSegmentation();

  void GeneratePlaneFromPoints();
  void FlipPlaneNormal();
  void SplitVolumeByPlane();
  void AnalysisWithCurrentPlane(std::string fname);

private:
  int PickCPs(EVec3f &ray_pos, EVec3f &ray_dir);
};


#endif

