#ifndef MODE_MODESEGSTROKEFFD_H_
#define MODE_MODESEGSTROKEFFD_H_

#pragma unmanaged

#include "ModeInterface.h"
#include "CagedMeshSequence.h"
#include "GlslShader.h"
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
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;

  CagedMeshSequence m_meshseq;

  enum class E_Mode
  {
    Stroke,
    Cage
  };
  E_Mode m_mode_deform;

  int   m_mode_transform;
  int   m_cp_size;
  float m_cp_rate;
  int   m_prev_frame_idx;
  bool  m_draw_surf_trans;

  // Stroke Mode
  std::vector<DeformationStrokes> m_strokes;
  std::set<int> m_shared_stroke_idxs;
  int m_prev_selected_stroke_idx;

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

  bool _show_only_selected_stroke;

  // Cage Mode
  ORTHO_HANDLE_ID m_draghandle_id;

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
  void SetDeformMode();
  void SetTransformMode();
  void SetCPSize();
  void SetShowOnlySelectedStroke();
  void ClearSelectedStrokes();

private:


};



#endif