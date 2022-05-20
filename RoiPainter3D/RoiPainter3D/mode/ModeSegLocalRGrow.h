#ifndef MODE_MODESEGLOCALRGROW_H_
#define MODE_MODESEGLOCALRGROW_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>

class TMesh;


class LRGSeed
{
private:
  static TMesh m_cp_mesh;
  static float m_cp_radius;
public:
  static void  SetCpRadius( float radius );
  static float GetCpRadius( );

public:

  //m_pos.size() == 1: sphere, > 1: cylinder 
	std::vector < EVec3f > m_cps;
	short  m_min_v, m_max_v;
  float  m_radius;
	int    m_flg_fore;

	LRGSeed(
    EVec3f position  = EVec3f(0,0,0), 
    short  min_val = 0, 
    short  max_val = 0, 
    int    b_fore = 0, 
    float radius = 0)
	{
    m_cps.clear();
		m_cps.push_back(position);
		m_min_v     = min_val;
		m_max_v     = max_val;
		m_flg_fore  = b_fore;
		m_radius    = radius;
	}

  //copy constructure and =operator
  LRGSeed(const LRGSeed &src)
  {
    Set(src);
  }

  LRGSeed &operator=(const LRGSeed &src)
  {
    Set(src);
    return *this;
  }

  void Set(const LRGSeed &src)
  {
    m_cps = src.m_cps;
		m_min_v    = src.m_min_v;
		m_max_v    = src.m_max_v;
		m_flg_fore = src.m_flg_fore;
		m_radius   = src.m_radius;
  }

  void ModifyRadius(float delta){
    m_radius += delta;
    if( m_radius < 0.1 ) m_radius  = 0.1f; 
  }

	void Draw        () const;
	void DrawAsActive() const;
};






class ModeSegLocalRGrow : public ModeInterface
{
private:
  // shader 
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;

  //seed id for moving (seed_id, cp_id)
	EVec2i m_drag_seedcp_id;

	//cut stroke
	bool m_b_drawstroke;
	std::vector<EVec3f> m_stroke;

public:
  // seeds and index of activated Seed (when -1 all seeds are non-activated)
  int m_activeseed_idx; 
	std::vector<LRGSeed> m_seeds;

private:
  ModeSegLocalRGrow();
public:
  ~ModeSegLocalRGrow();

  static ModeSegLocalRGrow* GetInst() { 
    static ModeSegLocalRGrow p; 
    return &p; 
  }

  // overload functions ---------------------------------------------

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
  void MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl);

  void KeyDown(int nChar);
  void KeyUp  (int nChar);

  bool CanLeaveMode();
  void StartMode ();
  void DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);

  void FinishSegmentation();
  void CancelSegmentation();
  void RunLocalRegionGrow();
  void AddNewSeed(bool bForeBack);
  void AddNewSeed(bool bForeBack, EVec3f &pos);

  void  ImportSeedInfo(std::string fname);
  void  ExportSeedInfo(std::string fname);

private:
  bool   PickActiveSeed    (const EVec3f &rayP, const EVec3f &rayD, EVec3f &pos);
  EVec2i PickSeedCP        (const EVec3f &rayP, const EVec3f &rayD);
  bool   PickToRemoveSeedCP(const EVec3f &rayP, const EVec3f &rayD);
  bool   PickCrssecToaddCP (const EVec3f &rayP, const EVec3f &rayD, const bool b_fore);


  int   GetLatestAddedSeedIdx(bool tf);
};


#endif