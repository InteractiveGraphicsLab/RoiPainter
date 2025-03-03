#ifndef MODE_MODESEGJOINTTRACKER_H_
#define MODE_MODESEGJOINTTRACKER_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "tmesh.h"

#include <vector>
#include <map>

//-----------------------------------------------
// (*) User Interface 
// step1 
//   閾値指定 --> iso surface 生成
// step2 tracking  
//  step 2-1. shift - double click --> iso surface の一部をハイライト
//  step 2-2. double click --> cp 配置 
//  step 2-3 run tracking ボタン --> ICPにより，2個の関節の位置あわせ
//  2-1, 2-2は frameidx == 0 に限定
// step3 
//   step3-1 外部からobj読み込み
//   step3-2 ドラッグにより位置あわせ
//   step3-3 位置あわせと距離計算
// manipuration mode
//   - cp placement bone 1 
//   - cp placement bone 2
//   - rot / trans bone 1
//   - rot / trans bone 2
//   - rot / trans fitting surface 
//
// (*) vol_flg[i]
// flg volume is not used for this 
//-----------------------------------------------

class TVtxDistInfo
{
public:
	int     m_idx ; // 元surface上の頂点id
	double  m_dist; // 対応先surfaceへの最小距離
	EVec3f  m_pSrc; // 対応元の頂点位置
	EVec3f  m_pTgt; // 対応先surface上の位置

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
  MODE_ID GetModeID() { return MODE_SEG_JTRACKER; }

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

  void KeyDown(int nChar);
  void KeyUp(int nChar);

  bool CanEndMode();
  void StartMode();
  void DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);
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