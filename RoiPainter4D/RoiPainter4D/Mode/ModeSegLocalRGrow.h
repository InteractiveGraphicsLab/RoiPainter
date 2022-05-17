#ifndef MODE_MODESEGLOCALRGROW_H_
#define MODE_MODESEGLOCALRGROW_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"
#include "tmesh.h"
#include <vector>

class LocalSeed
{
  //vectorでframe数分だけ管理する
  std::vector<float > m_radius  ;
  std::vector<EVec3f> m_position; // 位置
  std::vector<EVec2i> m_thresh  ; // 閾値
  std::vector<bool  > m_b_edit  ; // [i] = 1 なら i-th frameにおいて半径・閾値・位置のどれかがユーザにより編集された

public:

  LocalSeed(
    const int    num_frames,
    const int    edit_frame_idx,
    const EVec3f posision,
    const float  radius,
    const EVec2i threshold)
  {
    for (int i = 0; i < num_frames; ++i)
    {
      m_position.push_back(posision );
      m_radius  .push_back(radius   );
      m_b_edit  .push_back( i == edit_frame_idx );
      m_thresh  .push_back(threshold);
    }
  }

  LocalSeed( 
    int   num_frames, 
    std::vector<float > radius, 
    std::vector<EVec3f> position, 
    std::vector<EVec2i> thresh  , 
    std::vector<bool  > bedit )
  {
    m_position = position;
    m_radius   = radius  ;
    m_thresh   = thresh  ;
    m_b_edit   = bedit   ;
  }

  void Set(const LocalSeed& src)
  {
    m_position  = src.m_position;
    m_radius    = src.m_radius;
    m_b_edit    = src.m_b_edit;
    m_thresh    = src.m_thresh;
  }

  LocalSeed& operator=(const LocalSeed& src)
  {
    Set(src);
    return *this;
  }

  LocalSeed(const LocalSeed& src)
  {
    Set(src);
  }

  //setter
  void SetPosition ( int frame_idx, EVec3f pos   ){ m_position[frame_idx]    = pos;  }
  void SetRadius   ( int frame_idx, float  rad   ){ m_radius  [frame_idx]    = (rad <= 0.0f ) ? 0.000001f : rad;}
  void SetThreshMin( int frame_idx, int    minv  ){ m_thresh  [frame_idx][0] = minv;   }
  void SetThreshMax( int frame_idx, int    maxv  ){ m_thresh  [frame_idx][1] = maxv;   }
  void SetThresh   ( int frame_idx, EVec2i thresh){ m_thresh  [frame_idx]    = thresh; }
  void SetIsEdit   ( int frame_idx, bool   tf    ){ m_b_edit  [frame_idx]    = tf  ;   }

  EVec3f GetPosition(int frame_idx) const
  {
	  if ( frame_idx < 0 || frame_idx >= m_position.size()) return EVec3f(0,0,0);
	  else return m_position[frame_idx];
  }

  float GetRadius(int frame_idx) const
  {
    if ( frame_idx < 0 || frame_idx >= m_radius.size()) return -1.0f;
    else return m_radius[frame_idx];
  }

  EVec2i GetThreshold(int frame_idx) const
  {
	  if ( frame_idx < 0 || frame_idx >= m_thresh.size() ) return EVec2i(0,0);
	  else return m_thresh[frame_idx];
  }

  bool GetIsEdit( int frame_idx ) const
  {
	  if ( frame_idx < 0 || frame_idx >= m_b_edit.size() ) return false;
	  else return m_b_edit[frame_idx];
  }

};





//sphere のみのLocal Region Growing
// curved cylinderを領するのは ModeSegBolus 


class ModeSegLocalRGrow : public ModeInterface
{
  GlslShaderVolume  m_volume_shader;
  GlslShaderCrsSec  m_crssec_shader;

  std::vector<LocalSeed> m_seeds;
  
  TMesh m_unitsphere;
  float m_cp_radius;

  int   m_active_seed_id; //半径や閾値を変更する対象のシードid
  bool  m_is_drag_activeseed;
  bool  m_is_resize_activeseed;

  bool  m_is_modified;

  ModeSegLocalRGrow();

public:
  ~ModeSegLocalRGrow();
  static ModeSegLocalRGrow* getInst() {
    static ModeSegLocalRGrow p;
    return &p;
  }

  MODE_ID getModeID() { return MODE_SEG_LCLRGROW; }
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

  void keyDown( int nChar );
  void keyUp  ( int nChar );

  //this function is called before switch the mode (if return false, the mode will not be switched)
  bool canEndMode();

  //this function is called just after switch the mode
  void startMode();
  void drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);


  void FinishSegmentation();
  void CancelSegmentation();

  //IO seed info
  void LoadSeedInfo(std::string fname);
  void SaveSeedInfo(std::string fname);

  //リストで選択しているシードの設定
  void SetSelectedSeedId(int id) { if (id < 0) return; m_active_seed_id = id; }
  int  GetSelectedSeedId() { return m_active_seed_id; }

  void SelectSeed_SetPosition (EVec3f new_pos);
  void SelectSeed_SetRadius   (float new_radius);
  void SelectSeed_SetThreshMax(int maxv);
  void SelectSeed_SetThreshMin(int minv);
  void SelectSeed_RunInterpolation();

  const std::vector<LocalSeed>& getSeedList() { return m_seeds; }
  
  //todo 
  void RunSeedInterpolation(int trgt_seed_id);
  
  void RunLocalRegionGrow_Allframe();
  void RunLocalRegionGrow_OnlyCurrentFrame();

private:
  int  PickSeeds(const EVec3f &rayP, const EVec3f &rayD);//シードをつまんで動かす関数
  void RunLocalRegionGrow_OnlyTrgtFrame(int trgt_frame_idx);
};


#endif
