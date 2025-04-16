#ifndef MODE_MODSEGRIGIDICP_H_
#define MODE_MODSEGRIGIDICP_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "ttrianglesoup.h"
#include <vector>
#include <set>

//-----------------------------------------------
// (*) User Interface 
// shift + L click : 
// shift + L drag  : translate/rotate src_surface or rot_pivot  
// shift + R click :  
//
// (*) vol_flg[i]
// 0   : not the target
// 1   : backgroupd
// 255 : foreground (highlighted in Green)
//-----------------------------------------------


class ModeSegRigidICP : public ModeInterface
{
  //iso surfaces 等値面
  int m_isovalue;
  std::vector<TTriangleSoup> m_isosurfaces;

  //surface for fitting (各frameにfittingする面と行列)
  TTriangleSoup       m_source_surface;
  std::vector<EMat4d> m_icpmats;

  //Cut Stroke
  bool m_b_draw_stroke;
  std::vector<EVec3f> m_stroke;

  //source surface の移動
  EVec3f m_handle_pivot;
  EVec2i m_pre_mouse_point;
  bool   m_b_trans_pivot  ;
  bool   m_b_trans_srcsurf;
  bool   m_b_rot_srcsurf  ;

  bool   m_b_show_source_and_iso_surface;
  bool   m_b_modified;

  ModeSegRigidICP();
public:
  ~ModeSegRigidICP();

  static ModeSegRigidICP* GetInst() { static ModeSegRigidICP p; return &p; }

  // overload functions ---------------------------------------------
  MODE_ID GetModeID() { return MODE_SEG_RIGIDICP; }
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
  void KeyDown(int nChar);
  void KeyUp  (int nChar);
  bool CanEndMode();
  void StartMode();
  void DrawScene(const EVec3f &cam_pos, const EVec3f &cam_cnt);
  // ---------------------------------------------------------------

  void GenIsoSurface(const int isovalue, const bool do_all_frame, const int frame_index = -1);
  void LoadSrcSurface ( const std::string file_name);
  void PerformTracking( const float icpRejectionScale,
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