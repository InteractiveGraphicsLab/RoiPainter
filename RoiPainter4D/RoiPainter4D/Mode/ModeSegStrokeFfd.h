#ifndef MODE_MODESEGSTROKEFFD_H_
#define MODE_MODESEGSTROKEFFD_H_

#pragma unmanaged

#include "ModeInterface.h"
#include "CagedMeshSequence.h"
#include <vector>
#include <stack>

#include "DeformationStrokes.h"

//-----------------------------------------------
// By YUKI KIMURA published in PLoS ONE 
// Refactored by Takashi (かなり大幅に変更を実施 : 2026 Jan)
// (*) User Interface 
//
// (*) vol_flg[i]
// not used
// 
//-----------------------------------------------


class ModeSegStrokeFfd : public ModeInterface
{
  float m_cp_size;
  float m_handle_len;
  float m_handle_wid;

  CagedMeshSequence m_meshseq;

  // Curve Mode
  std::vector<std::vector<PlanarCurve>> m_curves       ; // [framd_idx][curve_idx]
  std::vector<SharedCurves>             m_shared_curves; // [curve_idx]

  PlanarCurveSelectionInfo m_select_info;

  // Cage Mode
  ORTHO_HANDLE_ID m_draghandle_id;
  bool   m_b_draw_selectionrect; 
  EVec3f m_selectrect[4];

  typedef struct
  {
    int frame_idx;
    std::vector<std::vector<PlanarCurve>> curves;
    std::vector<SharedCurves> shared_curves;
    std::vector<EVec3f> cage_verts;
  } SnapShot;

  std::stack<SnapShot> m_history;
  
  EVec2i m_initpt; //drag開始位置
  EVec2i m_prevpt; //dragの直前位置


  ModeSegStrokeFfd();

public:
  ~ModeSegStrokeFfd();
  static ModeSegStrokeFfd* GetInst()
  {
    static ModeSegStrokeFfd p;
    return &p;
  }

  // overload functions ---------------------------------------------
  MODE_ID GetModeID() { return MODE_VIS_NORMAL; }
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

  void KeyDown(int nChar);
  void KeyUp(int nChar);
  bool CanEndMode();
  void StartMode();
  void DrawScene(const EVec3f& cam_pos, const EVec3f& cam_cnt);
  // -----------------------------------------------------------------

  void Deform();

  void LoadMeshAndCage(const std::string&, const std::string&);
  void SaveMeshAndCage(const std::string&, const std::string&);
  void SaveState(const std::string&, const std::set<int>&);
  void LoadState(const std::string&, const std::set<int>&);


  void CopyFromPrevFrame();
  void CopyCageToAllFrames();
  void ShareSelectedStroke();
  void UnshareSelectedStroke();
  void LockSelectedStroke();
  void UnlockSelectedStroke();
  void UpdateSharedStroke();
  void SetCPSize(int size);
  void ClearSelectedStrokes();

  void FinishSegmentation();


  void Do_RecordSnapShot();
  void Undo_LoadSnapShot();

  private:
    PlanarCurveSelectionInfo PickCpAtCurrentFrame(const EVec3f& ray_pos, const EVec3f& ray_dir);
};



#endif



/*
typedef struct
{
  DeformationStrokes strokes;
  std::vector<EVec3f> verts;
} Action;

typedef struct
{
  std::stack<Action> undo;
  std::stack<Action> redo;
} History;

std::vector<History> m_histories;
std::vector<DeformationStrokes> m_strokes;
std::set<int> m_shared_stroke_idxs;

*/
