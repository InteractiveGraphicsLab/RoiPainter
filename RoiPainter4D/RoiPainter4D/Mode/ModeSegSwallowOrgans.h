#ifndef MODE_MODSEGSWALLOWORGANS_H_
#define MODE_MODSEGSWALLOWORGANS_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "CagedMeshSequence.h"
#include "GlslShader.h"
#include "tmesh.h"

#include <tuple>

#define NUM_POINT_GROUPS 10

//�����̓�g�D�e���v���[�g�𐧌�_�ړ��ɂ��ό`���̈敪�����郂�[�h
//�ό`�ɂ̓P�[�W�x�[�X�̎�@ (harmonic coordinate)���̗p


class CagePointGroup
{
public : 
  std::string   m_name;
  std::set<int> m_ids;
  CagePointGroup(){}
  CagePointGroup(const CagePointGroup &src){
    Set(src);
  }
  CagePointGroup &operator=(const CagePointGroup &src){
    Set(src);
    return *this;
  }
  void Set(const CagePointGroup &src){
    m_name = src.m_name;
    m_ids = src.m_ids;
  }
  void Set(const std::string &name, const std::set<int> &ids){
    m_name = name;
    m_ids = ids;
  }
};


// deformation log
class CageManipLog
{
public:
  int m_frame;
  std::vector<EVec3f> m_point_prev;
  std::vector<EVec3f> m_point_new ;
  
  CageManipLog(
      const int framei, 
      std::vector<EVec3f> &prept,
      std::vector<EVec3f> &defpt){
    m_frame = framei;
    m_point_prev = prept;
    m_point_new = defpt;
  }
  CageManipLog(const CageManipLog& src) {
    Set(src);
  }
  CageManipLog& operator=(const CageManipLog& src) {
    Set(src);
    return *this;
  }
  void Set(const CageManipLog& src) {
    m_frame = src.m_frame;
    m_point_prev = src.m_point_prev;
    m_point_new  = src.m_point_new;
  }
};





class ModeSegSwallowOrgans : public ModeInterface
{
  //shaders
  GlslShaderVolume  m_volume_shader;
  GlslShaderCrsSec  m_crssec_shader;

  std::string m_fpath_cage;
  std::string m_fpath_mesh;

  CagedMeshSequence m_meshseq;

  //support cut stroke 
  bool m_b_draw_cutstroke;
  std::vector<EVec3f> m_stroke;

  //user interface (����_�I���C����_�ړ�)
  ORTHO_HANDLE_ID m_draghandle_id;
  std::vector<EVec3f> m_cagevtx_prev;

  bool   m_b_draw_selectionrect; 
  EVec3f m_selectrect[4];

  //cursor point
  EVec2i m_initpt;
  EVec2i m_prevpt; 
  CagePointGroup m_groups[NUM_POINT_GROUPS];  

  //deformation log for undo (frame)
  int m_undo_piv;
  std::vector<CageManipLog> m_manip_log;

  std::vector<TMeshSequence > m_vismeshes;

  ModeSegSwallowOrgans();
public:
  ~ModeSegSwallowOrgans();

  static ModeSegSwallowOrgans* GetInst(){ static ModeSegSwallowOrgans p; return &p; }

  // overload functions ---------------------------------------------
  MODE_ID getModeID() { return MODE_SEG_SWALLOW; }

  void LBtnUp    (const EVec2i &p, OglForCLI *ogl);
  void RBtnUp    (const EVec2i &p, OglForCLI *ogl);
  void MBtnUp    (const EVec2i &p, OglForCLI *ogl);
  void LBtnDown  (const EVec2i &p, OglForCLI *ogl);
  void RBtnDown  (const EVec2i &p, OglForCLI *ogl);
  void MBtnDown  (const EVec2i &p, OglForCLI *ogl);
  void LBtnDclk  (const EVec2i &p, OglForCLI *ogl);
  void RBtnDclk  (const EVec2i &p, OglForCLI *ogl);
  void MBtnDclk  (const EVec2i &p, OglForCLI *ogl);
  void MouseMove (const EVec2i &p, OglForCLI *ogl);
  void MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl);

  void keyDown(int nChar);
  void keyUp (int nChar);

  bool canEndMode();
  void startMode();
  void drawScene(const EVec3f &cuboid, const EVec3f &cam_pos, const EVec3f &cam_center);
  //-----------------------------------------------------------------

  // IO for mesh and cage data   
  void LoadNewMeshAndCage( std::string meshname, std::string cagename);
  void LoadCageSequence  ( std::string fname);
  void SaveCageMeshSequenceObj(std::string fname, bool b_cage);
  void SaveCageMeshSequenceTxt(std::string fname, bool b_cage);

  void DeformImportedCage(std::string fname_cage, std::string fname_output);


  // Finish/Cancel Segmentation
  void FinishSegmentation();
  void CancelSegmentation();

  //Cage Vtx Grouping
  CagePointGroup GetPointGroup(int idx) { return m_groups[idx]; }
  void  LoadPointGroup(std::string fname);
  void  SavePointGroup(std::string fname);
  void  RegistPointGroup  (); //���I������Points��group�ɒǉ�
  void  DeletePointGroup  (int idx); //group[idx]���폜
  void  CheckoutPointGroup(int idx); //group[idx]����т���

  //Current Selection 
  std::vector<EVec3f> SelectedCageVtx_GetCentroidSeq();
  std::vector<std::vector<EVec3f>> SelectedCageVtx_GetSeq();
  std::vector<std::vector<EVec3f>> SelectedCageVtx_Get1RingSeq();
  void SelectedCageVtx_Clear();
  void SelectedCageVtx_Move(int frame_idx, int dim_xyz, float pos);
  void SelectedCageVtx_Smoothing(std::vector<int> trgt_fids);
  void SelectedCageVtx_CopyLeft (std::vector<int> trgt_fids);
  void SelectedCageVtx_CopyRight(std::vector<int> trgt_fids);

  void CopyCurrentMeshToOtherFrames();
  void FitCrssecToSelectedCageVtx();
  void UndoCpManipulation();
  void RedoCpManipulation();

  void LoadVisObjs (std::vector<std::string> fnames){
    m_vismeshes.push_back(TMeshSequence(fnames));
  }
  void ClearVisObjs() { 
    m_vismeshes.clear();
  }
private:
  void  FillInMesh();
};

#endif

