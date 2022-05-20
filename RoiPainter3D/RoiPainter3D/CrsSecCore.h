#ifndef CRSSECCORE_H_
#define CRSSECCORE_H_
#pragma unmanaged


//------------------------------------
//class CrsSecCore
//
//date 2018/10/31
//written by Takashi Ijiri
//
//This class manages cross sections to visualize volumes.
//-------------------------------------

#include "tmath.h"
#include "tmesh.h"


class CrssecCore
{
  //plane
  float  m_plane_xy; //[0,1]
  float  m_plane_yz; //[0,1]
  float  m_plane_zx; //[0,1]

  //curved cross section 
  EVec3f  m_curve_crssec_norm;
  TMesh   m_curve_crssec     ;
  std::vector<EVec2i> m_curve_crssec_edge;

  CrssecCore();
public:
  ~CrssecCore();

  static CrssecCore *GetInst()
  {
    static CrssecCore p;
    return &p;
  }

  void DrawCrssec(bool b_draw_xy, bool b_draw_yz, bool b_draw_zx, const EVec3f &cuboid);


  //picking cross sections
  CRSSEC_ID PickCrssec(
    bool b_pick_xy, bool b_pick_yz, bool b_pick_zx, bool b_pick_curvecrssec, 
    const EVec3f &cuboid, const EVec3f &ray_pos, const EVec3f &ray_dir);

  CRSSEC_ID PickCrssec(
    bool b_pick_xy, bool b_pick_yz, bool b_pick_zx, bool b_pick_curvecrssec, 
    const EVec3f &cuboid, const EVec3f &ray_pos, const EVec3f &ray_dir,
    EVec3f &picked_position);

  void MoveCrssec(EVec3i reso, EVec3f pitch, CRSSEC_ID id, short delta);

  //move three planes to fit to the picked 3D position
  void FitCrssecToPosition(const EVec3i &reso, const EVec3f &pitch, const EVec3f &pos, CRSSEC_ID fixId)
  {
    if (fixId != CRSSEC_XY) m_plane_xy = pos[2] / (pitch[2] * reso[2]);
    if (fixId != CRSSEC_YZ) m_plane_yz = pos[0] / (pitch[0] * reso[0]);
    if (fixId != CRSSEC_ZX) m_plane_zx = pos[1] / (pitch[1] * reso[1]);
  }

  void  GenerateCurvedCrssec(const EVec3f &cuboid, const EVec3f &camP, const std::vector<EVec3f> &stroke);
  void  ClearCurvedCrossec(){ m_curve_crssec.Clear(); m_curve_crssec_edge.clear(); }

  float GetPlaneXyPosition() { return m_plane_xy; }
  float GetPlaneYzPosition() { return m_plane_yz; }
  float GetPlaneZxPosition() { return m_plane_zx; }

  void SetPlaneXyPosition(float plane_xy ){ m_plane_xy = plane_xy; }
  void SetPlaneYzPosition(float plane_yz ){ m_plane_yz = plane_yz; }
  void SetPlaneZxPosition(float plane_zx ){ m_plane_zx = plane_zx; }

};



void t_DrawCoordinateAxis();
void t_DrawCuboidFrame(const EVec3f &cuboid);
void t_DrawCuboidSlices(const int num_slice, const EVec3f &cam_pos, const EVec3f &cam_center, const EVec3f &cuboid);



//lasso‚Ì“à‘¤‚É‚ ‚évoxel‚ð fore/back‚É•ÏX
//b_fore = true  --> vFlg 1   --> 255‚É
//b_fore = false --> vFlg 255 --> 1‚É
void t_AddPixsInsideLasso
(
	const CRSSEC_ID id   ,
	const EVec3i    reso ,
	const EVec3f    pitch,
	const std::vector<EVec3f> &lasso_stroke,
	const bool      b_fore,

   		  byte*  vFlg
);


#endif


