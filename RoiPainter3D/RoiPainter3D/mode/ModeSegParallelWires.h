#ifndef MODE_MODESEGPARALLELWIRES_H_
#define MODE_MODESEGPARALLELWIRES_H_
#pragma unmanaged

// class ModeSegVoxelPaint
// 
// Segmetation by placing multiple wires (spline curve)

// User interface 
// 
// Browsing : 
//  select plane (xy, yz, or zx) by dialog 
//  mouse wheel (or up/down key) to move the plane
//
// Wire placement
//  Shift L-click to add CP
//  Shift L-drag  to move CP
//  # click on a curve --> insert point
//  # otherwise --> add cp at the end
//
//  Shift M/R click to remove CP
//  
//  If the user places more than three control points --> the system generates a closed curve 


#include "ModeInterface.h"
#include "GlslShader.h"
#include "kcurves.h"
#include <vector>

class SplineWire
{
public:
  enum PLANE_ID {
    PLANE_XY, PLANE_YZ, PLANE_ZX
  };

private:
  const PLANE_ID m_plane_id;
  std::vector<EVec3f> m_cps;
  std::vector<EVec3f> m_curve;

  static float m_cp_radius;

public:
  SplineWire( PLANE_ID plane_id );
  
  SplineWire(const SplineWire& src) : m_plane_id (src.m_plane_id){
    Copy(src);
  }
  SplineWire &operator=(const SplineWire &src)
  {
    Copy(src);
    return *this;
  }
  void Copy(const SplineWire& src){
    m_cps = src.m_cps;
    m_curve = src.m_curve;
    if ( m_plane_id != src.m_plane_id )
      std::cout << "!!!!!!!!!!!!!!!!!!! strange copy at SplineWire !!!!!!!!!!!!\n"; 
  }
  
  //control point manipulation
  int  AddCtrlPt (const EVec3f &p);
  void MoveCtrlPt(const int idx, const EVec3f &p);

  int  PickCtrlPt( const EVec3f &ray_pos, const EVec3f &ray_dir);
  void PickToEraseCtrlPt( const EVec3f &ray_pos, const EVec3f &ray_dir);
  
  //rendering
  void DrawCtrlPt() const;
  void DrawWire  (const EVec3f &offset, const EVec3f &color, float width) const;

  int GetNumCtrlPts() const { return (int) m_cps.size(); }
  const std::vector<EVec3f> &GetCurve() const { return m_curve; }


  static void SetCtrlPtRadius(float r){
    m_cp_radius = r;
  }
  
  void exportCtrlPtInfo(std::ofstream &ofs) const;
  void importCtrlPtInfo(std::ifstream &ifs) ;
  
private:
  void UpdateCurveFromCPs( );
};




class ModeSegParallelWires : public ModeInterface
{
private:
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;
  
  CRSSEC_ID m_trgtplane;

  //wires
  std::vector<SplineWire> m_wires_xy;
  std::vector<SplineWire> m_wires_yz;
  std::vector<SplineWire> m_wires_zx;
  
  //dragging wire id , cp id
	EVec2i m_draging_cpid;
  
  int m_planexy_idx, m_planeyz_idx, m_planezx_idx; 
  
private:
  ModeSegParallelWires();

public:
  ~ModeSegParallelWires();


  static ModeSegParallelWires* GetInst() { 
    static ModeSegParallelWires p; 
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
  //------------------------------------------------------------------------------

  void ImportWireInfo(std::string fname);
  void ExportWireInfo(std::string fname);

  void SetControlPointSize(float size_01);

private:
  CRSSEC_ID GetCurrentTrgtPlane();
};


#endif

