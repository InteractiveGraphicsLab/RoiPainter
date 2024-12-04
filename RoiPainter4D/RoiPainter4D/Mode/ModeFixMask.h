#ifndef MODE_MODEFIXMASK_H_
#define MODE_MODEFIXMASK_H_

#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"
#include "MaskMeshSequence.h"
#include <vector>
#include <stack>
#include "DeformationStrokes.h"
#include "LaplacianSurfaceEditing.h"


class ModeFixMask :
  public ModeInterface
{
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;

  MaskMeshSequence m_mask_mesh;

  int m_cp_size;
  float m_cp_rate;
  int m_prev_frame_idx;
  bool m_draw_surf_trans;
  bool m_exist_mesh;

  // Stroke Mode
  std::vector<DeformationStrokes> m_strokes;
  std::vector<TMesh> m_tmeshes;
  std::vector<LaplacianDeformer> m_laplacian_deformer;
  std::set<int> m_shared_stroke_idxs;
  int m_prev_selected_stroke_idx;
  std::vector<Eigen::Vector3f> m_matched_pos;

  typedef struct
  {
    DeformationStrokes strokes;
  } Action;

  typedef struct
  {
    std::stack<Action> undo;
    std::stack<Action> redo;
  } History;
  std::vector<History> m_histories;

  bool m_show_only_selected_stroke;

  EVec2i m_initpt;
  EVec2i m_prevpt;

  // canEndMode
  bool m_is_not_saved_state;

  ModeFixMask();

public:
  ~ModeFixMask();
  static ModeFixMask* GetInst()
  {
    static ModeFixMask p;
    return &p;
  }

  // overload functions ---------------------------------------------
  MODE_ID GetModeID() { return MODE_VIS_NORMAL; }
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

  void KeyDown(int nChar);
  void KeyUp(int nChar);

  bool CanEndMode();
  void StartMode();

  void DrawScene(const EVec3f& cuboid, const EVec3f& camP, const EVec3f& camF);
  // -----------------------------------------------------------------
  void Deform();

   void Do();
   void Undo();
   void Redo();

   void ConvertMaskToMesh();
   void ConvertMeshToMask();
   void ReloadMesh();
   void ReloadMesh(const int);
   void CopyFromPrevFrame();
   void CopyStrokesToAllFrame();
   void SetShowOnlySelectedStroke();
   void SetCPSize();

   void SaveState(const std::string&, const std::set<int>&);
   void LoadState(const std::string&, const std::set<int>&);

   void ShareSelectedStroke();
   void UnshareSelectedStroke();
   void LockSelectedStroke();
   void UnlockSelectedStroke();
   void UpdateSharedStroke();


private:
  void FindClosestPointFromStroke(std::vector<int>&, std::vector<EVec3f>&, std::vector<EVec3f>&);

  //Add?


};



#endif