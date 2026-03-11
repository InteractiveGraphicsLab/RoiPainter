#ifndef MODE_MODERefCurveDeform_H_
#define MODE_MODERefCurveDeform_H_

#pragma unmanaged

#include "ModeInterface.h"
#include <vector>
#include <stack>
#include "DeformationStrokes.h"
#include "LaplacianSurfaceEditing.h"

//-----------------------------------------------
// (*) User Interface 
// + Shift drag --> place cps for new curve 
// + see paper for detail
//  
// (*) vol_flg[i]
// not used
// 
// Refacored by Takashi At 2025/12
// 
//-----------------------------------------------


class ModeRefCurveDeform : public ModeInterface
  {
  // Curves 
  std::vector<std::vector<PlanarCurve>> m_curves; // [framd_idx][curve_idx]
  std::vector<SharedCurves> m_shared_curves; // [curve_idx]

  typedef struct
  {
    int frame_idx;
    std::vector<std::vector<PlanarCurve>> curves;
    std::vector<SharedCurves> shared_curves;
  } SnapShot;

  std::stack<SnapShot> m_history; 


  PlanarCurveSelectionInfo m_select_info;
  float m_cp_rate;
  int m_target_mask_id;
  std::vector<TMesh> m_meshes_def ;
  std::vector<TMesh> m_meshes_orig;

  std::vector<std::vector<int>   > m_debug_matched_vids;
  std::vector<std::vector<EVec3f>> m_debug_matched_trgtpos;

  bool m_b_modified;

  ModeRefCurveDeform();

public:
  ~ModeRefCurveDeform();
  static ModeRefCurveDeform* GetInst()
  {
    static ModeRefCurveDeform p;
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
  void DrawScene(const EVec3f& cam_pos, const EVec3f& cam_cnt);
  // -----------------------------------------------------------------
  
  void FinishSegmentation();
  void CancelSegmentation();

  void DeformCurrentFrame();
  void DeformAllFrame();

  void Do_RecordSnapShot();
  void Undo_LoadSnapShot();

  void ConvertMaskToMesh();
  void ReloadOrigMeshCurrentFrame();
  void CopyFromPrevFrame();
  void CopyStrokesToAllFrame();

  void SaveState(const std::string& fname);
  void LoadState(const std::string& fname);

  void FlipSelectedStrokeNormalSide();
  void MakeSelectedStroke_Shared();
  void MakeSelectedStroke_Unshared();
  //void LockSelectedStroke();
  //void UnlockSelectedStroke();

private:
  void _Deform(const int);
  //void FindClosestPointFromStroke(const int, std::vector<int>&, std::vector<EVec3f>&, std::vector<EVec3f>&);

  PlanarCurveSelectionInfo PickCpAtCurrentFrame(const EVec3f& ray_pos, const EVec3f& ray_dir);

};



#endif