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
  std::vector< int   > m_cagevtx_hl; //cage vtxÇÃëIëèÛãµ flg 
  float **m_harmcoord; //size is [n_meshvtx,  n_cagevtx]


  //shader for mesh rendering
  static const std::string m_vtxshader_fname;
  static const std::string m_frgshader_fname;
  static GLuint  m_gl2Program;
public:
  
  ~CagedMeshSequence();
  CagedMeshSequence ();

  CagedMeshSequence(const CagedMeshSequence &src);
  CagedMeshSequence &operator=(const CagedMeshSequence &src);
  void Copy( const CagedMeshSequence &src);
  void Clear();

  void Initialize( const int num_frames, 
                   const EVec3f      &cuboid, 
                   const std::string &mesh_path, 
                   const std::string &cage_path );


  bool IsInitialized(){ return m_cages.size() != 0; }


  //GETTERs
  TMesh GetMesh( int frame_idx);
  int   GetNumSelectedVtx() const;
  int   GetNumMeshVertex() const ;
  int   GetNumCageVertex() const ;
  int   GetNumFrames()const { return (int)m_cages.size(); }

  EVec3f GetMeshVertex( const int frame_idx, const int vtx_idx);
  EVec3f GetCageVertex( const int frame_idx, const int vtx_idx);
  void   SetCageVertex( const int frame_idx, const int vtx_idx, EVec3f pos, bool doUpdateShape = true);
  std::vector<EVec3f> GetCageVertices(const int frame_idx)const;
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
    OglForCLI *ogl); 

  void SelectCageVtxByPick( 
    const int frame_idx, 
    const EVec3f &ray_pos, 
    const EVec3f &ray_dir); 

  void SetCageVtxSelected( std::set<int> &ids);
  std::set<int> GetSelectedCageVtx();

  void ClearSelectedVtx();

  //Modification---------------------------------------------------------------
  void TranslateSelectedVerts(
      const int frame_idx, 
      const EVec2i &p0, 
      const EVec2i &p1, 
      ORTHO_HANDLE_ID handle_id,
      OglForCLI* ogl);

  void RotateSelectedVerts(
      const int frame_idx, 
      const EVec2i &p0, 
      const EVec2i &p1, 
      ORTHO_HANDLE_ID handle_id,
      OglForCLI* ogl);

  void ScaleSelectedVerts(
      const int frame_idx, 
      const EVec2i &p0, 
      const EVec2i &p1, 
      OglForCLI* ogl);

  // if frame_idx == -1 --> deformation is applied for all frames
  void TranlateCage(const int frame_idx, const EVec3f& trans);
  void ScaleCage   (const int frame_idx, const EVec3f& center, const float s);
  void RotateCage  (const int frame_idx, const EVec3f& center, const EMat3f& R);

  ORTHO_HANDLE_ID PickCageHandle(
    const int frame_idx, 
    const EVec3f ray_pos, 
    const EVec3f ray_dir, 
    const int trans_rot_scale);
  

  //rendering --------------------------------------------
  void DrawMesh ( int frame_idx, 
                  float crssec_x_01, 
                  float crssec_y_01, 
                  float crssec_z_01,
                  float surface_opacity, 
                  const EVec3f &cuboid ) const;
  void DrawCage  ( int frame_idx, bool allmesh ) const;
  void DrawHandle(int frame_idx, int trans_rot_scale /*0,1,2*/) const;
  void DrawHandleCenter(int frame_idx, int trans_rot_scale /*0,1,2*/) const;

  void CopyOneFrameToTheOtherAllFrames( const int frame_idx );

  //handle size info 
  static float m_HANDLE_WIDTH ;
  static float m_HANDLE_LENGTH;
  static float m_CP_RADIUS;
  static void SetHandleLength(float handle_length){
    m_HANDLE_LENGTH = handle_length;
    m_HANDLE_WIDTH  = handle_length * 0.04f;
  }
  static void SetControlPointRadius(float r) {
    m_CP_RADIUS = r;
  }
 

  EVec3f GetSelectedVtxCentroid( const int frame_idx ) const;
  std::vector<EVec3f> GetSelectedVtxCentroidSeq();
  std::vector<std::vector<EVec3f>> GetSelectedVtxSeq();  
  std::vector<std::vector<EVec3f>> GetSelectVtx1RingSeq();
  
  
  void ImportCageSequenceFromTxt(std::string fname, bool modify_num_frame = false);
  void ExportCageMeshSequenceAsTxt(std::string fname, bool b_cage);
  void ExportCageMeshSequenceAsObj(std::string fname, bool b_cage);

};

#endif
