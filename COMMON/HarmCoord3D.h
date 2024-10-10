#ifndef COMMON_HARMONICCOORD_H_
#define COMMON_HARMONICCOORD_H_
#pragma unmanaged

#include "tmesh.h"


class HarmCoord3D
{
public:
  //original cage 
  const TMesh m_cage  ;
  const int m_LATTICE_RESOLUTION; 
  const int m_SMOOTHING_LEVEL   ; 
  const int m_SMOOTHING_ITER_N  ; 
 
  // -- 3D map configuration -- 
  //等方性ボクセル
  //少なくとも周囲1ボクセル分は背景に
  //cage vertexごとの計算結果を保持するのは現実的ではないので頂点ごとに計算したあと破棄する
  EVec3i  m_map_reso ; // 3d map resolution 
  float   m_map_pitch; // cubic voxel pitch
  EVec3f  m_map_pos  ; // origin position of the map
  byte   *m_map_flag ; // 0:out,1:inside,2:boundary

  //Store harmonic coordinate map（cost memory）
  std::vector<float*> m_map_hc; //harmonic coordinate map for each cage vertex

  //harmonic coordinate for internal vertices
  std::vector<std::vector<float>> m_verts_hc;

public:
  ~HarmCoord3D();
  HarmCoord3D(const TMesh &cage, const int num_verts, const EVec3f *verts, 
    const int LATTICE_RESOLUTION = 160, 
    const int SMOOTHING_LEVEL    = 5  ,
    const int Smoothing_ITER_N   = 400);

private:
  void InitMap();
  void CalcHarmonicCoordinateMap();
  std::vector<float> GetHarmonicCoordinate(const EVec3f &v);
  void RefineHarmonicCoordinate(const int vsize, const EVec3f *verts);
  //std::vector<float> RefineHarmonicCoordinates(const EVec3f &v, std::vector<float> hc);

  void Debug_showDebugInfo(const int vsize, const EVec3f *verts);
};

#endif