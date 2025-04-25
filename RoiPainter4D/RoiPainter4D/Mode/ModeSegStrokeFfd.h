#ifndef MODE_MODESEGSTROKEFFD_H_
#define MODE_MODESEGSTROKEFFD_H_

#pragma unmanaged

#include "ModeInterface.h"
#include "CagedMeshSequence.h"
#include <vector>
#include <stack>

#include "DeformationStrokes.h" // 曲線制約によるモデル変形

//-----------------------------------------------
// By YUKI KIMURA published in PLoS ONE 
// 
// (*) User Interface 
// TODO 木村くんのコードを読んで確認
//
// (*) vol_flg[i]
// not used
//-----------------------------------------------


class ModeSegStrokeFfd : public ModeInterface
{
  CagedMeshSequence m_meshseq;

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

  float m_cp_size;
  float m_handle_len, m_handle_wid;

  // Stroke Mode
  std::vector<DeformationStrokes> m_strokes;
  std::set<int> m_shared_stroke_idxs;
  int m_prev_selected_stroke_idx;

  // Cage Mode
  ORTHO_HANDLE_ID m_draghandle_id;

  int   m_prev_frame_idx;

  bool   m_b_draw_selectionrect;
  EVec3f m_selectrect[4];

  EVec2i m_initpt;
  EVec2i m_prevpt;

  // canEndMode
  bool m_is_not_saved_state;
  bool m_debug;

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

  void Do();
  void Undo();
  void Redo();

  void CopyFromPrevFrame();
  void CopyCageToAllFrames();
  void ShareSelectedStroke();
  void UnshareSelectedStroke();
  void LockSelectedStroke();
  void UnlockSelectedStroke();
  void UpdateSharedStroke();
  void SetCPSize(int size);
  void ClearSelectedStrokes();
};



#endif