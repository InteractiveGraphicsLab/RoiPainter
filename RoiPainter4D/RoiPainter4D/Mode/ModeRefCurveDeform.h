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


class ModeRefCurveDeform :
  public ModeInterface
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


  //Selected CurveCP Info 
  class SelectionInfo
  {
  public: 
    bool selected = false;
    bool is_shared = false;
    int curve_idx = -1;
    int cp_idx = -1;
    CRSSEC_ID crssec_id  = CRSSEC_XY;
    float     crssec_pos = 0.0f;
    EVec3f pos;

    SelectionInfo(){ Clear(); }
    void Set(bool _selected, bool _shared, int  _curve_id, int _cpid, EVec3f p, 
             CRSSEC_ID _crssec_id, float _crssec_pos)
    {
      selected = _selected;
      is_shared = _shared;
      curve_idx = _curve_id;
      cp_idx = _cpid;
      pos = p;
      crssec_id  = _crssec_id;
      crssec_pos = _crssec_pos;
    }
    void Clear(){
      Set(false, false, -1, -1, EVec3f(0, 0, 0), CRSSEC_XY, 0.0f);
    }
    bool IsStdCurveSelect(int _curve_idx){
      return selected && !is_shared && curve_idx == _curve_idx;  
    }
    bool IsSharedCurveSelect(int _curve_idx){
      return selected && is_shared && curve_idx == _curve_idx;  
    }
  };
  
  SelectionInfo m_select_info;
  float m_cp_rate;
  int m_target_mask_id;
  std::vector<TMesh> m_meshes_def ;
  std::vector<TMesh> m_meshes_orig;
  std::vector<LaplacianDeformer> m_laplacian_deformer;
  std::vector<Eigen::Vector3f>   m_matched_pos;

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

   void SaveState(const std::string&);
   void LoadState(const std::string&);

   void FlipSelectedStrokeNormalSide();
   void MakeSelectedStroke_Shared();
   void MakeSelectedStroke_Unshared();
   //void LockSelectedStroke();
   //void UnlockSelectedStroke();

private:
  void _Deform(const int);
  void FindClosestPointFromStroke(const int, std::vector<int>&, std::vector<EVec3f>&, std::vector<EVec3f>&);

  ModeRefCurveDeform::SelectionInfo PickCpAtCurrentFrame(const EVec3f& ray_pos, const EVec3f& ray_dir);

};



#endif