#ifndef COMMON_TMESH_H_
#define COMMON_TMESH_H_
#pragma unmanaged


//stl
#include <vector>
#include <set>

//COMMON
#include "tmath.h"


class TPoly
{
public:
  int idx[3];

  TPoly(int v0 = 0, int v1 = 0, int v2 = 0) 
  {
    idx[0] = v0; idx[1] = v1; idx[2] = v2;
  }

  TPoly(const TPoly &p) { Set(p); }
  TPoly& operator= (const TPoly& v) { Set(v); return *this; }

  void Set(const TPoly &p) 
  {
    memcpy(idx, p.idx, sizeof(int) * 3);
  }
};



// simple mesh model
class TMesh
{
public:
  //Vertex Info (num, position, TexCoord, Normal)
  int      m_vSize;
  EVec3f  *m_vVerts;
  EVec3f  *m_vTexCd; //(u,v,w) 
  EVec3f  *m_vNorms;
  std::vector<int> *m_vRingPs;
  std::vector<int> *m_vRingVs;

  //Polygon Info (num, normal, polygon_idx)
  int     m_pSize;
  EVec3f *m_pNorms;
  TPoly  *m_pPolys;

private:
  void SetFieldNull();

public:
  ~TMesh();
  TMesh();
  TMesh(const char* fname);
  TMesh(const TMesh& src) ;
  TMesh &operator=(const TMesh& src);
  void Clear();
  void Set(const TMesh& v);

  bool Initialize(const char* fName);
  void Initialize(const std::vector<EVec3f>& Vs, const std::vector<TPoly>& Ps);
  void InitializeIcosaHedron(const double r);
  void InitializeAsSphere(const float r, const int reso_hori, const int reso_verti);

  //modification
  void Smoothing(int n);
  void UpdateNormal();
  void UpdateRingInfo();
  void Translate(const EVec3f t);
  void Scale(const float  s);
  void Rotate(Eigen::AngleAxis<float>& R);
  void Rotate(const EMat3f& R);
  void MultMat(const EMat4f M);
  void NormalizeByUniformScaling();

  //ger info
  EVec3f GetGravityCenter() const;
  void GetBoundBox(EVec3f& BBmin, EVec3f& BBmax) const;
  
  BoundingBox GetBoundingBox() const;
  BoundingBox GetBoundingBox_OnePoly(const int pid) const;

  //rendering
  void Draw() const;
  void Draw( const float* diff, const float* ambi,
             const float* spec, const float* shin ) const;
  void DrawEdges() const;
  void DrawEdges(int width, double r, double g, double b) const;

  static void DrawSphere(const EVec3f& position, const float radius);
  static void DrawSphere(const EVec3f& position, const float radius,
                        const float* diff, const float* ambi,
                        const float* spec, const float* shin);


  //draw edge [i,j] if vert_flg[i] || vert_flg[j] 
  void DrawEdges(int width, double r, double g, double b,
    const std::vector<int>& vert_flg) const;
  
  //export 
  void ExportObjNoTexCd(const char* fname) const;
  bool ExportStl(const char* fname);

  //picking 
  bool PickByRay(const EVec3f& rayP, const EVec3f& rayD, EVec3f& pos, int& pid) const;
  bool PickByRay(const EVec3f& rayP, const EVec3f& rayD, EVec3f& pos) const;
  
  //other tools 
  void ExtractLinkedVertsPolys( const int vid,
                                std::vector<int>& vert_ids,
                                std::vector<int>& poly_ids) const;
  void ExtractRingNeighborVerts(const int vid,
                                const int ringsize,
                                std::set<int>& output_ids) const;
  int GetNearestVertexIdx(const EVec3f& p) const;

  //最近傍点検索
  void GetDistToPoint( const EVec3f& p, 
                       const int nearest_vid,
                       EVec3f& pos_onsurf, 
                       double& dist) const;

  // Convert Mesh to voxel by filling its inside
  // reso & pitch : resolution and pitch of volume
  // binVol       : allocated[WxHxD], 0:out, 1:in
  void GenBinaryVolume( const EVec3i& reso, 
                        const EVec3f& pitch, 
                                byte* bin_vol) const; 
  



  static void DrawIcosaHedron(
    const float r,
    const float* diff,
    const float* ambi,
    const float* spec,
    const float* shin);

  static void DrawIcosaHedron(
    const EVec3f& p,
    const float r,
    const float* diff,
    const float* ambi,
    const float* spec,
    const float* shin);
};


class TMeshSequence
{

public:
  std::vector<TMesh> m_meshes;

  TMeshSequence() {}
  TMeshSequence(std::vector<std::string> fnames) {
    LoadFromFiles(fnames);
  }
  TMeshSequence(const TMeshSequence& src) {
    Set(src);
  }
  TMeshSequence& operator=(const TMeshSequence& src) {
    Set(src);
    return *this;
  }
  void Set(const TMeshSequence& src) {
    m_meshes = src.m_meshes;
  }
  void LoadFromFiles(std::vector<std::string> fnames) {
    m_meshes.clear();
    for (auto i : fnames)
      m_meshes.push_back(TMesh(i.c_str()));
  }
};





#endif