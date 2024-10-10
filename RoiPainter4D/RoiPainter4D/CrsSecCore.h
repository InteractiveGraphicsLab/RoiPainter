#ifndef CRSSECCORE_H_
#define CRSSECCORE_H_

//------------------------------------
//class CrsSecCore
//
//date 2016/08/20
//written by Takashi Ijiri
//
//This class manages cross sections to visualize volumes.
//-------------------------------------

#pragma unmanaged
#include "tmath.h"
#include "tmesh.h"


class CrssecCore
{
  float m_plane_xy; //[0,1]
  float m_plane_yz; //[0,1]
  float m_plane_zx; //[0,1]

  EVec3f m_curve_crssec_norm;
  TMesh  m_curve_crssec;
  std::vector<EVec2i> m_curve_crssec_edge;

  CrssecCore();
public:
  ~CrssecCore();

  static CrssecCore *GetInst()
  {
    static CrssecCore p;
    return &p;
  }

  float GetPlanePosXY(){ return m_plane_xy; }
  float GetPlanePosYZ(){ return m_plane_yz; }
  float GetPlanePosZX(){ return m_plane_zx; }
  void SetPlanePosXY(const float& pos_z, const EVec3f& cuboid) { m_plane_xy = pos_z / cuboid[2]; }
  void SetPlanePosYZ(const float& pos_x, const EVec3f& cuboid) { m_plane_yz = pos_x / cuboid[0]; }
  void SetPlanePosZX(const float& pos_y, const EVec3f& cuboid) { m_plane_zx = pos_y / cuboid[1]; }


private:
  void CreateCurvedCrssec_(
      const EVec3f &cuboid, 
      const EVec3f &cam_pos, 
      const std::vector<EVec3f> &stroke
  );
  
  CRSSEC_ID PickCrssec_(
      bool bXY, 
      bool bYZ, 
      bool bZX, 
      const EVec3f &cuboid  , 
      const EVec3f &ray_pos, 
      const EVec3f &ray_dir, 
      EVec3f &pos
  );

  void FitCrssecToPosition_(
    const EVec3i &reso, 
    const EVec3f &pitch, 
    const EVec3f &pos, 
    CRSSEC_ID fixId)
  {
    if (fixId != CRSSEC_XY) m_plane_xy = pos[2] / (pitch[2] * reso[2]);
    if (fixId != CRSSEC_YZ) m_plane_yz = pos[0] / (pitch[0] * reso[0]);
    if (fixId != CRSSEC_ZX) m_plane_zx = pos[1] / (pitch[1] * reso[1]);
  }
  void DrawCrssec_(bool bXY, bool bYZ, bool bZX, const EVec3f &cuboid);
  void MoveCrssec_(EVec3i reso, EVec3f pitch, CRSSEC_ID id, short delta);

public:
  static void CreateCurvedCrssec(
      const EVec3f &cuboid, 
      const EVec3f &cam_pos, 
      const std::vector<EVec3f> &stroke)
  {
    GetInst()->CreateCurvedCrssec_(cuboid, cam_pos, stroke);
  }

  static CRSSEC_ID PickCrssec(
      bool bXY, bool bYZ, bool bZX, 
      const EVec3f &cuboid  , 
      const EVec3f &ray_pos, 
      const EVec3f &ray_dir, 
      EVec3f &pos)
  {
    return GetInst()->PickCrssec_(bXY,bYZ,bZX,cuboid,ray_pos,ray_dir,pos);
  }

  static void FitCrssecToPosition(
    const EVec3i &reso, 
    const EVec3f &pitch, 
    const EVec3f &pos, 
    CRSSEC_ID fixId)
  {
    GetInst()->FitCrssecToPosition_(reso,pitch,pos,fixId);
  }

  static void DrawCrssec(bool bXY, bool bYZ, bool bZX, const EVec3f &cuboid)
  {
    GetInst()->DrawCrssec_(bXY,bYZ,bZX,cuboid);
  }

  static void MoveCrssec(EVec3i reso, EVec3f pitch, CRSSEC_ID id, short delta)
  {
    GetInst()->MoveCrssec_(reso,pitch,id,delta);
  }

};


void t_drawAxis();
void t_DrawFrame(const EVec3f &cuboid);
void t_drawSlices(const int slineNum, const EVec3f &camP, const EVec3f &camF, const EVec3f &cuboid);

#endif