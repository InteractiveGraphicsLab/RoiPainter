#ifndef COMMON_TRIANGLESOUP_H_
#define COMMON_TRIANGLESOUP_H_

#pragma unmanaged
#include "tmath.h"

// triangle soup class
// checked 2022/5/09
class TTriangle
{
public:
  EVec3f m_verts[3];

  TTriangle(){
    m_verts[0] = m_verts[1] = m_verts[2] = EVec3f(0, 0, 0);
  }

  TTriangle& operator= (const TTriangle  &v) { 
    Set(v); 
    return *this; 
  }

  TTriangle(const TTriangle &src){
    Set(src);
  }

  void Set(const TTriangle &src){
    m_verts[0] = src.m_verts[0]; 
    m_verts[1] = src.m_verts[1]; 
    m_verts[2] = src.m_verts[2];   
  }
};




class TTriangleSoup 
{
public:
  int        m_num_triangles;
  TTriangle *m_triangles   ;
  EVec3f    *m_normals  ;

  void Clear();
  void Set(const TTriangleSoup& src);

  ~TTriangleSoup();
  TTriangleSoup();
  TTriangleSoup(const TTriangleSoup& src);
  TTriangleSoup& operator= (const TTriangleSoup& v);
 
  void Allocate(int num_triangles, EVec3f* triangle_vertex_array = 0);
  
  void Draw() const;
  void Draw(const float* diff, const float* ambi,
            const float* spec, const float* shin) const;
  void DrawEdges();
  void DrawEdges(int width, double r, double g, double b);

  void ImportObj(const char* fname);
  bool ExportAsStl(const char* fname);

  void Translate(const EVec3f& trans);
  void MultMat(const EMat4f M);
  void UpdateNormal();
  
  void CalcBoundBox(EVec3f& BBmin, EVec3f& BBmax) const;
  EVec3f GetGravityCenter();

  bool TTriangleSoup::PickByRay(const EVec3f& rayP, const EVec3f& rayD, EVec3f& pos, int& pid) const;
  bool TTriangleSoup::PickByRay(const EVec3f& rayP, const EVec3f& rayD, EVec3f& pos) const
  {
    int pidx;
    return PickByRay(rayP, rayD, pos, pidx);
  }
};


// cast ray in Y axis ( divide ZX plane )  
void GenBinaryVolumeFromMesh_Y
(
  const EVec3i& reso,
  const EVec3f& pitch,
  const TTriangleSoup& mesh,
  byte* binVol //allocated[WxHxD], 0:out, 1:in
);

#endif
