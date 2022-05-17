#ifndef MODE_MODESEGJOINTTRACKER_H_
#define MODE_MODESEGJOINTTRACKER_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"
#include "tmesh.h"

#include <vector>
#include <map>

//UI 
// step1 
//   臒l�w�� --> iso surface ����
// step2 tracking  
//  step 2-1. shift - double click --> iso surface �̈ꕔ���n�C���C�g
//  step 2-2. double click --> cp �z�u 
//  step 2-3 run tracking �{�^�� --> ICP�ɂ��C2�̊֐߂̈ʒu���킹
//  2-1, 2-2�� frameidx == 0 �Ɍ���

// step3 
//   step3-1 �O������obj�ǂݍ���
//   step3-2 �h���b�O�ɂ��ʒu���킹
//   step3-3 �ʒu���킹�Ƌ����v�Z
//
// manipuration mode
//   - cp placement bone 1 
//   - cp placement bone 2
//   - rot / trans bone 1
//   - rot / trans bone 2
//   - rot / trans fitting surface 
//
//vol_flg
// flg volume is not used for this 

//
// OK gen form
// OK gen mouse event 
// OK gen drawscene
// OK step1
// OK step2
// OK step3
//
// OK CPs io
// OK vis flg in dlg
// OK test VIS 
// OK test cp i/o 
// OK test accracy  
// 



class TVtxDistInfo
{
public:
	int     m_idx ; // ��surface��̒��_id
	double  m_dist; // �Ή���surface�ւ̍ŏ�����
	EVec3f  m_pSrc; // �Ή����̒��_�ʒu
	EVec3f  m_pTgt; // �Ή���surface��̈ʒu

	~TVtxDistInfo(){}
	TVtxDistInfo(){}
	void Copy ( const TVtxDistInfo &src){
		m_idx  = src.m_idx;
		m_dist = src.m_dist;
		m_pSrc  = src.m_pSrc;
		m_pTgt  = src.m_pTgt;
	}

	TVtxDistInfo( const TVtxDistInfo &src){ Copy(src); } 
	TVtxDistInfo& operator=(const TVtxDistInfo& src){ Copy(src); return *this;}

	TVtxDistInfo( int idx, double dist, EVec3f pSrc, EVec3f pTgt){
		m_idx  = idx;
		m_dist = dist;
		m_pSrc = pSrc;
		m_pTgt = pTgt;
	}

};







class ModeSegJointTracker : public ModeInterface
{
  //shaders
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;
  
  //iso surfaces (by step1)
  int                m_isovalue    ;
  std::vector<TMesh> m_isosurfaces ;
  std::vector<byte*> m_isosurf_pflg; //polygon flg 0:non, 1:bone1, 2:bone2

  //CPs / fitting surface / fitting matrix (by step2)
  std::vector<EVec3f>   m_cps_bone1 , m_cps_bone2 ; 
  std::vector<std::pair<EVec3f,EVec3f>> m_srcbone1, m_srcbone2;
  std::vector<int> m_srcbone1_ids, m_srcbone2_ids;
  std::vector<EMat4d>   m_mats_bone1, m_mats_bone2; //fnum

  //for error evaluation 
  TMesh m_evalobj;
  std::vector<EMat4d> m_mats_evalobj;  
  std::vector<std::vector<TVtxDistInfo>> m_evalinfo; // evalinfo[framidx][vtxidx] 


  //mouse manipulation 
  bool   m_b_transrot;
  EVec2i m_prevpoint ;
  EVec2i m_dragcp    ; // (cp1/cp2, cpidx) 
  EMat3f m_handle_rot;


  ModeSegJointTracker();
public:
  ~ModeSegJointTracker();

  static ModeSegJointTracker* GetInst() { 
    static ModeSegJointTracker p; return &p; 
  }

  // overload functions ---------------------------------------------
  MODE_ID getModeID() { return MODE_SEG_JTRACKER; }

  void LBtnUp   (const EVec2i &p, OglForCLI *ogl);
  void RBtnUp   (const EVec2i &p, OglForCLI *ogl);
  void MBtnUp   (const EVec2i &p, OglForCLI *ogl);
  void LBtnDown (const EVec2i &p, OglForCLI *ogl);
  void RBtnDown (const EVec2i &p, OglForCLI *ogl);
  void MBtnDown (const EVec2i &p, OglForCLI *ogl);
  void LBtnDclk (const EVec2i &p, OglForCLI *ogl);
  void RBtnDclk (const EVec2i &p, OglForCLI *ogl);
  void MBtnDclk (const EVec2i &p, OglForCLI *ogl);
  void MouseMove(const EVec2i &p, OglForCLI *ogl);
  void MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl);

  void keyDown(int nChar);
  void keyUp(int nChar);

  bool canEndMode();
  void startMode();
  void drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);
  //-----------------------------------------------------------------

  void FinishSegmentation();
  void CancelSegmentation();


public:
  void RunICP_all(int icplevel, float icpreject);
  void RunICP_currenrt(int icplevel, float icpreject);
  void ExportCPsInfo(std::string fname);
  void ImportCPsInfo(std::string fname);

  //evaluation (evaluate the quality of 4DCT)
public: 
  void EvalObj_Load(std::string fname);
  void EvalObj_RunAnalysis( std::string fname, int icplv, float icpreject);
private:
  void EvalObj_Translate(int frameidx, EVec3f trans);
  void EvalObj_Rotate   (int frameidx, EMat3f rot  );
  void EvalObj_DrawDiff (int frameidx );
  void EvalObj_DrawSurf (int frameidx );


private: 
  void   CPs_Draw  (int frameidx );

  //only for frameidx == 0
  EVec2i CPs_Pick  (EVec3f ray_p, EVec3f ray_d);
  void   CPs_Add   (const int boneid   , const EVec3f pos); //boneid 1 or 2
  void   CPs_Move  (const EVec2i cpid  , const EVec3f pos); //cpid (1or2, index)
  void   CPs_Remove(const EVec2i cpid );

  //iso surface
  void IsoSurf_Draw( int frame_idx );
  //for all frame
  void IsoSurf_PickToActivate( const EVec3f rayp, const EVec3f rayd );
  //only for frameidx == 0
  bool IsoSurf_PickActiveSurf( const EVec3f rayp, const EVec3f rayd, EVec3f &pos);
public:
  void IsoSurf_Generate(short isovalue );

private:
  //registration info
  void DrawRegistSurfaces(int frameidx);
  void DrawCpTrajectory(int frameidx, int cpidx);
};

#endif