#ifndef CAGEDMESHSEQUENCE_H_
#define CAGEDMESHSEQUENCE_H_
#pragma unmanaged

#include "tmesh.h"
#include "thandle3d.h"

class OglForCLI;
//-----------------------------------------------------------------------------
//class CagedMeshSequence
//
// this class maintains a sequence of caged mesh
// a pair of cage and mesh exits for each frame 
// this class also has selection flag for vertices of the cage 
// 
//-----------------------------------------------------------------------------
class CagedMeshSequence
{
private:
  std::vector< TMesh > m_cages ;
  std::vector< TMesh > m_meshes;
  std::vector< int   > m_cagevtx_hl; //cage vtxの選択状況 flg 

  //harmonic coodinate : size [n_meshvtx,  n_cagevtx]
  float **m_harmcoord; 

public:
  CagedMeshSequence();
  ~CagedMeshSequence() { Clear(); }

  CagedMeshSequence(const CagedMeshSequence &src){
    m_harmcoord = 0;
    Copy(src);
  }
  CagedMeshSequence &operator=(const CagedMeshSequence &src){
    Copy(src);
    return *this;
  }
  void Copy( const CagedMeshSequence &src);
  void Clear();

  void Initialize( const int num_frames, 
                   const EVec3f      &cuboid, 
                   const std::string &mesh_path, 
                   const std::string &cage_path );

  bool IsInitialized(){ return m_cages.size() != 0; }

  //Getter & Setter 
  TMesh GetMesh(int frame_idx);
  TMesh GetCage(int frame_idx);
  int   GetNumSelectedVtx() const;
  int   GetNumMeshVertex () const ;
  int   GetNumCageVertex () const ;
  float** GetHarmCoord() const { return m_harmcoord; }
  EVec3f GetMeshVertex( const int frame_idx, const int vtx_idx);
  EVec3f GetCageVertex( const int frame_idx, const int vtx_idx);
  void   SetCageVertex( const int frame_idx, const int vtx_idx, EVec3f pos, bool doUpdateShape = true);
  std::vector<EVec3f> GetMeshVertices(const int frame_idx) const;
  std::vector<EVec3f> GetCageVertices(const int frame_idx) const;

  void SetCageVertices(const int frame_idx, const std::vector<EVec3f> &verts, const bool doUpdateShape = true);
  void SetSelectedCageVtxPos(const int frame_idx, const int dim_xyz, float pos);

  void SelectedCageVtx_Smoothing    (const std::vector<int> trgt_fids);
  void SelectedCageVtx_CopyFromLeft (const std::vector<int> trgt_fids);
  void SelectedCageVtx_CopyFromRight(const std::vector<int> trgt_fids);
  void UpdateMeshShape ( int frame_idx) ;

  //Selection----------------------------------------------------------------
  void SelectCageVtxByRect( 
    const int frame_idx, 
    const EVec2i p0, 
    const EVec2i p1, 
    OglForCLI *ogl,
    const int select_mode = -1); 

  void SelectCageVtxByPick( 
    const int frame_idx, 
    const EVec3f &ray_pos, 
    const EVec3f &ray_dir,
    const float  cp_rad,
    const int select_mode = -1);

  void SetCageVtxSelected( std::set<int> &ids);
  std::set<int>    GetSelectedCageVtxSet();
  std::vector<int> GetSelectedCageVtxVec(){ return m_cagevtx_hl;}

  void ClearSelectedVtx();

  //Modification---------------------------------------------------------------
  void TranslateSelectedVerts(const int frame_idx, const EVec2i &p0, const EVec2i &p1, ORTHO_HANDLE_ID handle_id, OglForCLI* ogl);
  void RotateSelectedVerts   (const int frame_idx, const EVec2i &p0, const EVec2i &p1, ORTHO_HANDLE_ID handle_id, float handle_len, OglForCLI* ogl);
  void ScaleSelectedVerts    (const int frame_idx, const EVec2i &p0, const EVec2i &p1, OglForCLI* ogl);

  // if frame_idx == -1 --> deformation is applied for all frames
  void TranlateCage(const int frame_idx, const EVec3f& trans);
  void ScaleCage   (const int frame_idx, const EVec3f& center, const float s);
  void RotateCage  (const int frame_idx, const EVec3f& center, const EMat3f& R);

  void CopyOneFrameToTheOtherAllFrames( const int frame_idx );
 
  EVec3f GetSelectedVtxCentroid( const int frame_idx ) const;
  std::vector<EVec3f> GetSelectedVtxCentroidSeq();
  std::vector<std::vector<EVec3f>> GetSelectedVtxSeq();  
  std::vector<std::vector<EVec3f>> GetSelectVtx1RingSeq();
  
  void ImportCageSequenceFromTxt(std::string fname, bool modify_num_frame = false);
  void ExportCageMeshSequenceAsTxt(std::string fname, bool b_cage);
  void ExportCageMeshSequenceAsObj(std::string fname, bool b_cage);
};


//rendering --------------------------------------------

//Meshをレンダリング + xy yz zx断面付近の帯を描画
void DrawMeshWithCrossecHL(
  const TMesh &mesh,
  float crssec_x_01,
  float crssec_y_01,
  float crssec_z_01,
  float surface_opacity,
  const EVec3f& cuboid);

//Meshをレンダリング + 頂点Sphereを描画
void DrawCageWithCPs(
  const TMesh &mesh,
  bool  allmesh    , //true --> 全頂点を描画 
  float cp_radius ,
  const std::vector<int> &vtx_hl //ハイライトする頂点id
  );




#endif
