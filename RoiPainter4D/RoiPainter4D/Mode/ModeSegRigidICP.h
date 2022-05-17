#ifndef MODE_MODSEGRIGIDICP_H_
#define MODE_MODSEGRIGIDICP_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"
#include "ttrianglesoup.h"
#include <vector>
#include <set>


class ModeSegRigidICP : public ModeInterface
{
  //shaders
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;

  //等値面情報
  int                        m_isovalue;
  std::vector<TTriangleSoup> m_isosurfaces;

  //各フレームにフィッテイングするsurface
  TTriangleSoup       m_source_surface;
  std::vector<EMat4d> m_icpmats;
  EVec3f m_handle_pivot;
  //
  bool m_b_show_source_and_iso_surface;

  //Cut Stroke
  bool m_b_draw_stroke;
  std::vector<EVec3f> m_stroke;

  //source surface の移動
  EVec2i m_pre_mouse_point;
  bool   m_b_trans_pivot      ;
  bool   m_b_trans_srcsurf;
  bool   m_b_rot_srcsurf;
  EMat3f m_rotate_handle_r;
  bool   m_b_modified;

  ModeSegRigidICP();
public:
  ~ModeSegRigidICP();

  static ModeSegRigidICP* GetInstance() { static ModeSegRigidICP p; return &p; }

  // overload functions ---------------------------------------------
  MODE_ID getModeID() { return MODE_SEG_RIGIDICP; }
  void LBtnUp     (const EVec2i &p, OglForCLI *ogl);
  void RBtnUp     (const EVec2i &p, OglForCLI *ogl);
  void MBtnUp     (const EVec2i &p, OglForCLI *ogl);
  void LBtnDown   (const EVec2i &p, OglForCLI *ogl);
  void RBtnDown   (const EVec2i &p, OglForCLI *ogl);
  void MBtnDown   (const EVec2i &p, OglForCLI *ogl);
  void LBtnDclk   (const EVec2i &p, OglForCLI *ogl);
  void RBtnDclk   (const EVec2i &p, OglForCLI *ogl);
  void MBtnDclk   (const EVec2i &p, OglForCLI *ogl);
  void MouseMove  (const EVec2i &p, OglForCLI *ogl);
  void MouseWheel (const EVec2i &p, short zDelta, OglForCLI *ogl);
  void keyDown(int nChar);
  void keyUp  (int nChar);
  bool canEndMode();
  void startMode();
  void drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);
  // ---------------------------------------------------------------

  void LoadSourceSurfaceObj      (const std::string file_name);
  void IsosurfaceGenerateOneFrame(const int    isovalue, const int frame_index);
  void IsosurfaceGenerateAllFrame(const int    isovalue);
  void PerformTracking           (const float icpRejectionScale, 
                                  const int   icpNumLevels, 
                                  const int   start_index, 
                                  const int   end_index, 
                                  const bool  b_parallelTracking);
  void FinishSegmentation(const bool b_storeallframes);
  void CancelSegmentation();
  void ExportMatrixSequence(std::string fname); 
  void ImportMatrixSequence(std::string fname);
private:
  void FillInMesh( int start_frame, int end_frame);
};


#endif