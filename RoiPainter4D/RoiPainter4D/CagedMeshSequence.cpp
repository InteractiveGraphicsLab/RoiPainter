#pragma unmanaged
#include "CagedMeshSequence.h"
#include "HarmCoord3D.h"
#include "./Mode/GlslShader.h"
#include "Mode/ModeInterface.h"
#include "OglForCLI.h"

#pragma managed
#include "CliMessageBox.h"
#pragma unmanaged

#pragma warning(disable : 4996)

///////////////////////////////////////////////////////////////////////////////
//class DeformableMesh/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

using namespace RoiPainter4D;

float CagedMeshSequence::m_HANDLE_WIDTH  = 1.0;
float CagedMeshSequence::m_HANDLE_LENGTH = 1.0;
float CagedMeshSequence::m_CP_RADIUS     = 1.0;


const std::string CagedMeshSequence::m_vtxshader_fname = 
    std::string("shader/cagemeshVtx.glsl");
const std::string CagedMeshSequence::m_frgshader_fname = 
    std::string("shader/cagemeshFrg.glsl");
GLuint  CagedMeshSequence::m_gl2Program = -1;


CagedMeshSequence::~CagedMeshSequence()
{
  Clear();
}


CagedMeshSequence::CagedMeshSequence()
{
  m_harmcoord = 0;
}


CagedMeshSequence::CagedMeshSequence(const CagedMeshSequence &src)
{
  m_harmcoord = 0;
  Copy(src);
}


CagedMeshSequence &CagedMeshSequence::operator=(const CagedMeshSequence &src) 
{ 
  Copy(src); 
  return *this;
}


//clean up field
void CagedMeshSequence::Clear()
{
  if ( m_meshes.size() == 0 ) return;

  int n_meshvtx = m_meshes.front().m_vSize;

  if ( m_harmcoord != 0 )
  {
    for ( int i = 0; i < n_meshvtx; ++i )
      delete[] m_harmcoord[i];
    delete[] m_harmcoord;

    m_harmcoord = 0;
  }

  m_cages.clear();
  m_meshes.clear();
  m_cagevtx_hl.clear();
}


void CagedMeshSequence::Copy( const CagedMeshSequence &src)
{
  Clear();

  m_cages      = src.m_cages; 
  m_meshes     = src.m_meshes; 
  m_cagevtx_hl = src.m_cagevtx_hl; 

  if ( m_meshes.size() != 0 )
  {
    int n_cagevtx = m_cages .front().m_vSize;
    int n_meshvtx = m_meshes.front().m_vSize;
    
    m_harmcoord = new float*[n_meshvtx];

    for ( int i = 0; i < n_meshvtx; ++i )
    {
      m_harmcoord[i] = new float[n_cagevtx];
      memcpy( m_harmcoord[i], src.m_harmcoord[i], sizeof(float) * n_cagevtx);
    }
  }
}


static const char* MESSAGE_REUSE = 
  "前計算結果が利用できます．利用しますか？\n"
  "The precomp data is available. Do you want to use it.";


static const char* MESSAGE_FAILLOADING =
"objファイル読み込みに失敗しました. fails to load Obj file";

bool t_LoadMeshAndCage(
  const std::string &meshname,
  const std::string &cagename,
  const EVec3f &cuboid,
  TMesh& mesh,
  TMesh& cage
)
{
  if (!mesh.Initialize(meshname.c_str())) {
    ShowMsgDlg_OK(MESSAGE_FAILLOADING, "message");
    return false;
  }

  if (cagename.length() == 0 || !cage.Initialize(cagename.c_str()))
  {
    EVec3f bbmin, bbmax;
    mesh.GetBoundingBox(bbmin, bbmax);
    float sx = bbmin[0], sy = bbmin[1], sz = bbmin[2];
    float ex = bbmax[0], ey = bbmax[1], ez = bbmax[2];
    //gen cube 
    std::vector<EVec3f> vs = {
      EVec3f(sx,sy,sz), EVec3f(ex,sy,sz), EVec3f(ex,ey,sz), EVec3f(sx,ey,sz),
      EVec3f(sx,sy,ez), EVec3f(ex,sy,ez), EVec3f(ex,ey,ez), EVec3f(sx,ey,ez) };
    std::vector<TPoly> ps = {
      TPoly(0,2,1), TPoly(0,3,2), TPoly(0,5,4),
      TPoly(0,1,5), TPoly(1,6,5), TPoly(1,2,6),
      TPoly(2,7,6), TPoly(2,3,7), TPoly(3,4,7),
      TPoly(3,0,4), TPoly(4,5,6), TPoly(4,6,7) };
    cage.Initialize(vs, ps);
  }

  //calc scale / trans
  EVec3f bbmin, bbmax;
  cage.GetBoundingBox(bbmin, bbmax);
  int idx = max3id(bbmax[0] - bbmin[0], bbmax[1] - bbmin[1], bbmax[2] - bbmin[2]);
  cage.Scale(0.5f * cuboid[idx] / (bbmax[idx] - bbmin[idx]));
  mesh.Scale(0.5f * cuboid[idx] / (bbmax[idx] - bbmin[idx]));

  EVec3f gc = cage.GetGravityCenter();
  cage.Translate(-gc + cuboid / 2);
  mesh.Translate(-gc + cuboid / 2);

  return true;
}



void CagedMeshSequence::Initialize(
    const int num_frames, 
    const EVec3f &cuboid,
    const std::string& mesh_path,
    const std::string& cage_path)
{
  Clear();

  TMesh mesh, cage;
  if (!t_LoadMeshAndCage(mesh_path, cage_path, cuboid, mesh, cage))
    return;

  m_cages.resize(num_frames);
  m_meshes.resize(num_frames);
  m_cagevtx_hl.resize(cage.m_vSize, 0);

  for (int i = 0; i < num_frames; ++i)
  {
    m_meshes[i] = mesh;
    m_cages[i] = cage;
  }

  
  const int path_i = (int)mesh_path.find_last_of("\\") + 1;
  const std::string tmp = mesh_path.substr(path_i);
  const std::string precomp_path = cage_path + "." + tmp + ".hmnccdprecomp";


  //compute harmonic coordinate
  m_harmcoord = new float* [mesh.m_vSize];
  for (int i = 0; i < mesh.m_vSize; ++i)
  {
    m_harmcoord[i] = new float[cage.m_vSize];
  }

  //CHECK precompute data
  bool precompfile_was_available = false;
  FILE* fp = fopen(precomp_path.c_str(), "rb");
  if (fp != 0)
  {
    int n_mesh_vtx, n_cage_vtx;
    fread(&n_mesh_vtx, sizeof(int), 1, fp);
    fread(&n_cage_vtx, sizeof(int), 1, fp);

    if (n_mesh_vtx == mesh.m_vSize &&
      n_cage_vtx == cage.m_vSize &&
      ShowMsgDlgYesNo(MESSAGE_REUSE, "Reuse Precomputation?"))
    {

      for (int i = 0; i < mesh.m_vSize; ++i)
        for (int j = 0; j < cage.m_vSize; ++j)
          fread(&(m_harmcoord[i][j]), sizeof(float), 1, fp);

      precompfile_was_available = true;
    }
    fclose(fp);
  }

  if (!precompfile_was_available)
  {
    //calc harmonic coordinates
    //HarmCoord3D hc(cage, mesh.m_vSize, mesh.m_vVerts);
    HarmCoord3D hc(cage, mesh.m_vSize, mesh.m_vVerts);

    for (int i = 0; i < mesh.m_vSize; ++i)
      for (int j = 0; j < cage.m_vSize; ++j)
        m_harmcoord[i][j] = hc.m_verts_hc[i][j];

    FILE* fp = fopen(precomp_path.c_str(), "wb");
    int n_mesh = mesh.m_vSize;
    int n_cage = cage.m_vSize;
    fwrite(&n_mesh, sizeof(int), 1, fp);
    fwrite(&n_cage, sizeof(int), 1, fp);

    for (int i = 0; i < mesh.m_vSize; ++i)
      for (int j = 0; j < cage.m_vSize; ++j)
        fwrite(&(m_harmcoord[i][j]), sizeof(float), 1, fp);
    fclose(fp);
  }
}





void CagedMeshSequence::UpdateMeshShape ( int frame_idx) 
{
  if ( frame_idx < 0 || (int)m_cages.size() <= frame_idx  ) return; 

  const int num_meshvtx = m_meshes[frame_idx].m_vSize;
  const int num_cagevtx = m_cages [frame_idx].m_vSize;
  
  const EVec3f* cagevtx = m_cages [frame_idx].m_vVerts;
  EVec3f* meshvtx       = m_meshes[frame_idx].m_vVerts;

#pragma omp parallel for 
  for ( int i = 0; i < num_meshvtx; ++i)
  {
    float x=0, y=0, z=0;
    for ( int j=0; j < num_cagevtx; ++j) 
    {
      x += m_harmcoord[i][j] * cagevtx[j][0];
      y += m_harmcoord[i][j] * cagevtx[j][1];
      z += m_harmcoord[i][j] * cagevtx[j][2];
    }
    meshvtx[i] << x,y,z;
  }

  m_cages [frame_idx].UpdateNormal();
  m_meshes[frame_idx].UpdateNormal();
}


//
//GETTERs
//
TMesh CagedMeshSequence::GetMesh( int frame_idx){ 
  if ( frame_idx < 0 || (int)m_meshes.size() <= frame_idx) return TMesh();
  return m_meshes[frame_idx]; 
}

TMesh CagedMeshSequence::GetCage(int frame_idx) {
  if (frame_idx < 0 || (int)m_cages.size() <= frame_idx) return TMesh();
  return m_cages[frame_idx];
}

int  CagedMeshSequence::GetNumSelectedVtx() const
{
  int sum = 0;
  for ( const auto &it : m_cagevtx_hl ) if( it ) ++sum; 
  return sum;
}



int CagedMeshSequence::GetNumMeshVertex() const {
  if ( (int)m_meshes.size() <= 0) return 0;
  return m_meshes[0].m_vSize;
}

int CagedMeshSequence::GetNumCageVertex() const {
  return (int)m_cagevtx_hl.size();
}



EVec3f CagedMeshSequence::GetMeshVertex( 
    const int frame_idx, 
    const int vtx_idx)
{
  bool tf = frame_idx < 0 || (int)m_meshes.size() <= frame_idx || 
            vtx_idx   < 0 || (int)m_meshes[frame_idx].m_vSize <= vtx_idx;
    
  if ( tf ) 
    return EVec3f(0,0,0);
  else
    return m_meshes[frame_idx].m_vVerts[vtx_idx];
}


EVec3f CagedMeshSequence::GetCageVertex(const int frame_idx, const int vtx_idx)
{
  bool tf = frame_idx < 0 || (int)m_cages.size() <= frame_idx || 
            vtx_idx   < 0 || (int)m_cages[frame_idx].m_vSize <= vtx_idx;
    
  if ( tf ) 
    return EVec3f(0,0,0);
  else
    return m_cages[frame_idx].m_vVerts[vtx_idx];
}


std::vector<EVec3f> CagedMeshSequence::GetMeshVertices(const int frame_idx) const
{
  if (frame_idx < 0 || (int)m_meshes.size() <= frame_idx)
    return std::vector<EVec3f>();

  int n = m_meshes[frame_idx].m_vSize;
  std::vector<EVec3f> verts(n);

  for (int i = 0; i < n; ++i)
    verts[i] = m_meshes[frame_idx].m_vVerts[i];

  return verts;
}


std::vector<EVec3f> CagedMeshSequence::GetCageVertices(const int frame_idx) const
{
  if (frame_idx < 0 || (int)m_cages.size() <= frame_idx) 
    return std::vector<EVec3f>();
  
  int n = m_cages[frame_idx].m_vSize;
  std::vector<EVec3f> verts(n);

  for (int i = 0; i < n; ++i) 
    verts[i] = m_cages[frame_idx].m_vVerts[i];

  return verts;
}


void CagedMeshSequence::SetCageVertices(
  const int frame_idx,
  const std::vector<EVec3f>& verts,
  const bool doUpdateShape)
{
  if ( frame_idx < 0 || (int)m_cages.size() <= frame_idx ) return;

  int n = m_cages[frame_idx].m_vSize;
  if ( n != (int)verts.size()) return;

  for (int i = 0; i < n; ++i) 
    m_cages[frame_idx].m_vVerts[i] = verts[i];

  if (doUpdateShape) UpdateMeshShape(frame_idx);
}





void  CagedMeshSequence::SetCageVertex ( 
    const int frame_idx, 
    const int vtx_idx, 
    EVec3f pos, 
    bool doUpdateShape
    )
{
  bool tf = frame_idx < 0 || (int)m_cages.size() <= frame_idx || 
            vtx_idx   < 0 || (int)m_cages[frame_idx].m_vSize <= vtx_idx;
  if ( tf ) 
    return;

  m_cages[frame_idx].m_vVerts[vtx_idx] = pos;
  if( doUpdateShape ) UpdateMeshShape( frame_idx );
}



void  CagedMeshSequence::SetSelectedCageVtxPos(const int frame_idx, const int dim_xyz, float pos)
{
  const int num_frames = (int) m_cages.size();
  
  if ( frame_idx < 0 || num_frames <= frame_idx ) return;
  if ( dim_xyz < 0 || 3 <= dim_xyz ) return;

  EVec3f c = GetSelectedVtxCentroid( frame_idx );
  float ofst = pos - c[dim_xyz];
    
  for ( int i = 0; i < (int)m_cagevtx_hl.size(); ++i )
  {
    if ( m_cagevtx_hl[i] ) 
    {
      m_cages[frame_idx].m_vVerts[i][dim_xyz] += ofst;
    }
  }
  UpdateMeshShape( frame_idx );
}




// 1 2 3 4 5 6 7   
// 1 ----- 5
//   1/4 2/4 3/4

void CagedMeshSequence::SelectedCageVtx_Smoothing(
    const std::vector<int> trgt_fids
)
{
  if ( trgt_fids.size() == 0 ) return;

  const int num_frames = (int) m_cages.size();
  int fid_start = trgt_fids.front() - 1;
  int fid_end   = trgt_fids.back () + 1;
  if ( fid_start < 0 ) return;
  if ( fid_start >= num_frames ) return;

  for ( auto fid : trgt_fids )
  {
    float t = (fid - fid_start ) / (float)(fid_end - fid_start);
    for ( int i = 0; i < (int)m_cagevtx_hl.size(); ++i )
    {
      if ( m_cagevtx_hl[i] ) 
      {
        m_cages[fid].m_vVerts[i] = 
          (1-t) * m_cages[fid_start].m_vVerts[i] + 
           t * m_cages[fid_end  ].m_vVerts[i];
      }
    }
    UpdateMeshShape( fid );
  }
}


void CagedMeshSequence::SelectedCageVtx_CopyFromLeft(
    const std::vector<int> trgt_fids
)
{
  if ( trgt_fids.size() == 0 ) return;

  int trgt_fid = trgt_fids.front() - 1;
  if ( trgt_fid < 0 ) return;

  for ( auto fid : trgt_fids )
  {
    for ( int i = 0; i < (int)m_cagevtx_hl.size(); ++i )
    {
      if ( m_cagevtx_hl[i] ) 
        m_cages[fid].m_vVerts[i] = m_cages[trgt_fid].m_vVerts[i];
    }
    UpdateMeshShape( fid );
  }

}


void CagedMeshSequence::SelectedCageVtx_CopyFromRight(const std::vector<int> trgt_fids)
{
  if ( trgt_fids.size() == 0 ) return;

  const int num_frames = (int) m_cages.size();
  int trgt_fid = trgt_fids.back() + 1;
  if ( trgt_fid >= num_frames ) return;

  for ( auto fid : trgt_fids )
  {
    for ( int i = 0; i < (int)m_cagevtx_hl.size(); ++i )
    {
      if ( m_cagevtx_hl[i] ) 
        m_cages[fid].m_vVerts[i] = m_cages[trgt_fid].m_vVerts[i];
    }
    UpdateMeshShape( fid );
  }
}






void CagedMeshSequence::SetCageVtxSelected( std::set<int> &ids){
  for( auto &it : ids ) 
    if( 0 <= it && it < m_cagevtx_hl.size() ) 
      m_cagevtx_hl[it] = true;
}



std::set<int> CagedMeshSequence::GetSelectedCageVtx()
{
  std::set<int> s;
  for ( int i=0; i< (int) m_cagevtx_hl.size(); ++i)
    if ( m_cagevtx_hl[i] )
      s.insert(i);
  return s;
}






// 2D screen上に描かれたrectangle内部の頂点を選択状態にする
// rectの外の頂点については選択状態を解除しない
void CagedMeshSequence::SelectCageVtxByRect(
    const int frame_idx, 
    const EVec2i p0, 
    const EVec2i p1, 
    OglForCLI *ogl,
    const int select_mode)
{
  if ( frame_idx < 0 || (int)m_cages.size() <= frame_idx  ) return; 

  if (!ogl->IsDrawing()) ogl->oglMakeCurrent();

  double mat_mv[16], mat_proj[16];
	int mat_viewport[4];
	glGetDoublev (GL_MODELVIEW_MATRIX , mat_mv);
	glGetDoublev (GL_PROJECTION_MATRIX, mat_proj);
	glGetIntegerv(GL_VIEWPORT, mat_viewport);

  const int num_cagevtx = m_cages[frame_idx].m_vSize;
  const int minx = std::min(p0[0], p1[0]), maxx = std::max(p0[0], p1[0]);
  const int miny = std::min(p0[1], p1[1]), maxy = std::max(p0[1], p1[1]);

  for ( int i = 0; i < num_cagevtx; ++i )
  {
    const EVec3f &v = m_cages[frame_idx].m_vVerts[i];
    double x,y,z;

		gluProject(v[0], v[1], v[2], mat_mv, mat_proj, mat_viewport, &x, &y, &z);
    y = mat_viewport[3] - y;
		
    if ( minx <= x && x <= maxx && miny <= y && y <= maxy )
		{
      if ((select_mode == 0) || (select_mode == 1))
      {
        m_cagevtx_hl[i] = select_mode;
      }
      else
      {
        m_cagevtx_hl[i] = (m_cagevtx_hl[i] == 1) ? 0 : 1;
      }
    }
  }

	if (!ogl->IsDrawing()) wglMakeCurrent(NULL, NULL);
}





// pick to select cage vertex
void CagedMeshSequence::SelectCageVtxByPick( 
    const int    frame_idx, 
    const EVec3f &ray_pos, 
    const EVec3f &ray_dir,
    const int select_mode)
{
  if ( frame_idx < 0 || m_cages.size() <= frame_idx  ) return; 

  const TMesh &cage = m_cages[frame_idx];
  
  float mindist = FLT_MAX;
  int   minidx  = -1;

  for ( int i = 0; i < cage.m_vSize; ++i)
  {
    const EVec3f &vertex = cage.m_vVerts[i];
    if ( DistRayAndPoint( ray_pos, ray_dir, vertex) > m_CP_RADIUS )
      continue;
    
    float d = Dist(vertex, ray_pos);
    if ( d < mindist )
    {
      mindist = d;
      minidx = i;
    }   
  }

  if (minidx != -1)
  {
    if ((select_mode == 0) || (select_mode == 1))
    {
      m_cagevtx_hl[minidx] = select_mode;
    }
    else
    {
      m_cagevtx_hl[minidx] = (m_cagevtx_hl[minidx] == 1) ? 0 : 1;
    }
  }
}





void CagedMeshSequence::ClearSelectedVtx()
{
  for ( auto& it : m_cagevtx_hl) it = 0;
}




EVec3f CagedMeshSequence::GetSelectedVtxCentroid( const int frame_idx ) const
{
  EVec3f gc(0,0,0);
  if ( frame_idx < 0 || (int)m_cages.size() <= frame_idx  ) return gc; 

  int num = 0;
  for ( int i = 0; i < (int)m_cagevtx_hl.size(); ++i )
  {
    if ( m_cagevtx_hl[i] ) 
    {
      gc += m_cages[frame_idx].m_vVerts[i];
      num ++;
    }
  }

  if ( num == 0 ) return gc;
  else return gc/(float)num;
}




std::vector<EVec3f> CagedMeshSequence::GetSelectedVtxCentroidSeq()
{
  std::vector<EVec3f> centroids;

  for ( int i = 0; i < (int) m_cages.size(); ++i)
  {
    centroids.push_back( GetSelectedVtxCentroid(i) );
  }
  return centroids;
}




std::vector<std::vector<EVec3f>> CagedMeshSequence::GetSelectedVtxSeq()
{
  std::vector<std::vector<EVec3f>> vert_seq(m_cages.size());

  for ( int fi = 0; fi < (int) m_cages.size(); ++fi)
  {
    for ( int i = 0; i < (int)m_cagevtx_hl.size(); ++i )
    {
      if ( !m_cagevtx_hl[i] ) continue;
      vert_seq[fi].push_back( m_cages[fi].m_vVerts[i] );
    }
  }
  return vert_seq;
}


std::vector<std::vector<EVec3f>> CagedMeshSequence::GetSelectVtx1RingSeq()
{
  std::vector<std::vector<EVec3f>> vert_seq( m_cages.size() );
  if ( m_cages.size() == 0 ) return vert_seq;

  std::set<int> vid;
  for ( int i = 0; i < (int)m_cagevtx_hl.size(); ++i )
  {
    if ( !m_cagevtx_hl[i] ) continue;
    for ( auto &it : m_cages.front().m_vRingVs[i] )
      vid.insert(it); 
  }
  for ( int fi = 0; fi < (int) m_cages.size(); ++fi)
  {
    for ( auto it : vid ) 
      vert_seq[fi].push_back( m_cages[fi].m_vVerts[it] );
  }
  return vert_seq;
}









//Move all selected vertices---------------------------------------------------
//translate, rotate, or scaling with respect to the gravity center
void CagedMeshSequence::TranslateSelectedVerts(
    const int frame_idx, 
    const EVec2i &p0, 
    const EVec2i &p1, 
    ORTHO_HANDLE_ID handle_id,
    OglForCLI* ogl)
{
  if ( frame_idx < 0 || (int)m_cages.size() <= frame_idx  ) return; 


  const EVec3f gc = GetSelectedVtxCentroid( frame_idx );
  
  EVec3f ray_p0, ray_p1, ray_d0, ray_d1;
  std::tie(ray_p0, ray_d0) = ogl->GetCursorRay1(p0);
  std::tie(ray_p1, ray_d1) = ogl->GetCursorRay1(p1);

  float coef = Dist( gc    , ogl->GetCamPos() ) / 
               Dist( ray_p0, ogl->GetCamPos() );
  
  EVec3f trans = coef * (ray_p1 - ray_p0);
  if ( handle_id == OHDL_X ) trans[1] = trans[2] = 0;
  if ( handle_id == OHDL_Y ) trans[0] = trans[2] = 0;
  if ( handle_id == OHDL_Z ) trans[0] = trans[1] = 0;

  TMesh &mesh = m_cages[frame_idx];

  for ( int i = 0; i < (int)m_cagevtx_hl.size(); ++i )
  {
    if ( m_cagevtx_hl[i] ) mesh.m_vVerts[i] += trans;
  }

  UpdateMeshShape( frame_idx );
}




void CagedMeshSequence::RotateSelectedVerts(
    const int frame_idx, 
    const EVec2i &p0, 
    const EVec2i &p1, 
    ORTHO_HANDLE_ID handle_id,
    OglForCLI* ogl)
{

  if ( frame_idx < 0 || (int)m_cages.size() <= frame_idx  ) return; 
  // std::cout << "RotateSelectedVerts\n";

  const EVec3f gc = GetSelectedVtxCentroid( frame_idx );
  
  EVec3f ray_p0, ray_p1, ray_d0, ray_d1;
  std::tie(ray_p0, ray_d0) = ogl->GetCursorRay1(p0);
  std::tie(ray_p1, ray_d1) = ogl->GetCursorRay1(p1);

  float coef = Dist( gc    , ogl->GetCamPos() ) / 
               Dist( ray_p0, ogl->GetCamPos() );
  
  EVec3f eye_ray = (ogl->GetCamCnt() - ogl->GetCamPos()).normalized();
  EVec3f trans = coef * (ray_p1 - ray_p0);
  
  EMat3f dR; // = CalcObjectRotationMatrixByMouseDragF(ogl, dx, dy, 0.007f);

  if ( handle_id == OHDL_XY ) 
  {
    trans[2] = 0;
    float theta = trans.norm() / m_HANDLE_LENGTH; 
    if( trans.cross(eye_ray).z() < 0 ) theta *= -1;
    dR = Eigen::AngleAxisf(theta, EVec3f(0,0,1));
  }
  if ( handle_id == OHDL_YZ ) 
  {
    trans[0] = 0;
    float theta = trans.norm() / m_HANDLE_LENGTH; 
    if( trans.cross(eye_ray).x() < 0 ) theta *= -1;
    dR = Eigen::AngleAxisf(theta, EVec3f(1,0,0));
  }
  if ( handle_id == OHDL_ZX ) 
  {
    trans[1] = 0;
    float theta = trans.norm() / m_HANDLE_LENGTH; 
    if( trans.cross(eye_ray).y() < 0 ) theta *= -1;
    dR = Eigen::AngleAxisf(theta, EVec3f(0,1,0));
  }
  
  TMesh &c = m_cages[frame_idx];
 
  for ( int i = 0; i < (int)m_cagevtx_hl.size(); ++i )
  {
    if ( m_cagevtx_hl[i] ) c.m_vVerts[i] = dR * (c.m_vVerts[i] - gc) + gc ;
  }
    
  UpdateMeshShape( frame_idx );
}



void CagedMeshSequence::ScaleSelectedVerts(
    const int frame_idx, 
    const EVec2i &p0, 
    const EVec2i &p1, 
    OglForCLI* ogl)
{
  if ( frame_idx < 0 || (int)m_cages.size() <= frame_idx  ) return; 

  TMesh  &c = m_cages[frame_idx];
  const EVec3f gc = GetSelectedVtxCentroid( frame_idx );
  const int dx = p1.x() - p0.x();
  const int dy = p1.y() - p0.y();

  const float scale = 1.0f + dy * 0.01f;

  for ( int i = 0; i < (int)m_cagevtx_hl.size(); ++i )
  {
    if ( m_cagevtx_hl[i] ) c.m_vVerts[i] = scale * (c.m_vVerts[i] - gc) + gc;
  }
  UpdateMeshShape(frame_idx);
}



void CagedMeshSequence::TranlateCage(const int frame_idx, const EVec3f& trans)
{
  if (frame_idx == -1)
  {
    for (int i = 0; i < (int)m_cages.size(); ++i) {
      m_cages[i].Translate(trans);
      UpdateMeshShape(i);
    }
  }
  else if ( 0 <= frame_idx && frame_idx < (int)m_cages.size() )
  {
    m_cages[frame_idx].Translate(trans);
    UpdateMeshShape(frame_idx);
  }
}


void CagedMeshSequence::RotateCage(const int frame_idx, const EVec3f &center, const EMat3f& R)
{
  if (frame_idx == -1)
  {
    for (int i = 0; i < (int)m_cages.size(); ++i) {
      m_cages[i].Translate(-center);
      m_cages[i].Rotate(R);
      m_cages[i].Translate( center);
      UpdateMeshShape(i);
    }
  }
  else if (0 <= frame_idx && frame_idx < (int)m_cages.size())
  {
    m_cages[frame_idx].Translate(-center);
    m_cages[frame_idx].Rotate(R);
    m_cages[frame_idx].Translate(center);
    UpdateMeshShape(frame_idx);
  }
}


void CagedMeshSequence::ScaleCage(const int frame_idx, const EVec3f& center, const float s)
{
  if (frame_idx == -1)
  {
    for (int i = 0; i < (int)m_cages.size(); ++i) {
      m_cages[i].Translate(-center);
      m_cages[i].Scale(s);
      m_cages[i].Translate(center);
      UpdateMeshShape(i);
    }
  }
  else if (0 <= frame_idx && frame_idx < (int)m_cages.size())
  {
    m_cages[frame_idx].Translate(-center);
    m_cages[frame_idx].Scale(s);
    m_cages[frame_idx].Translate(center);
    UpdateMeshShape(frame_idx);
  }
}








void CagedMeshSequence::CopyOneFrameToTheOtherAllFrames( const int frame_idx )
{
  if ( frame_idx < 0 || (int)m_cages.size() <= frame_idx  ) return; 

  for ( int i=0; i < (int) m_cages.size(); ++i)
  {
    if ( i == frame_idx ) continue;
    m_cages[i] = m_cages[frame_idx];
    UpdateMeshShape(i);
  }
}





///////////////////////////////////////////////////////////
//rendering ///////////////////////////////////////////////
///////////////////////////////////////////////////////////

// crssec_*の付近のみ不透明にでレンダリング
// crssec_*は[0,1]に正規化されているものとする
void CagedMeshSequence::DrawMesh(
  int frame_idx, 
  float planex01, //plane position (normalized into [0,1])
  float planey01, //plane position (normalized into [0,1])
  float planez01, //plane position (normalized into [0,1])
  float opacity , //surface opacity   
  const EVec3f &cuboid) const
{
  if ( frame_idx < 0 || (int)m_meshes.size() <= frame_idx  ) return; 

  if ( m_gl2Program == -1  )
  {
    t_InitializeShader(
        m_vtxshader_fname.c_str(), 
        m_frgshader_fname.c_str(), 
        m_gl2Program);
  }

  //bind shader
  glUseProgram(m_gl2Program);
  glUniform1f(glGetUniformLocation(m_gl2Program, "u_crssec_x_01"), planex01);
  glUniform1f(glGetUniformLocation(m_gl2Program, "u_crssec_y_01"), planey01);
  glUniform1f(glGetUniformLocation(m_gl2Program, "u_crssec_z_01"), planez01);
  glUniform1f(glGetUniformLocation(m_gl2Program, "u_cuboid_w"   ), cuboid[0]);
  glUniform1f(glGetUniformLocation(m_gl2Program, "u_cuboid_h"   ), cuboid[1]);
  glUniform1f(glGetUniformLocation(m_gl2Program, "u_cuboid_d"   ), cuboid[2]);
  glUniform1f(glGetUniformLocation(m_gl2Program, "u_opacity"    ), opacity);

  m_meshes[frame_idx].Draw(); 
  glUseProgram(0);
}





void CagedMeshSequence::DrawCage(
    int frame_idx, 
    bool allmesh) const
{
  if ( frame_idx < 0 || (int)m_cages.size() <= frame_idx  ) return; 
  const TMesh &c = m_cages[frame_idx];

  //cage edges
  glDisable( GL_LIGHTING );
  if( allmesh ) 
    c.DrawEdges(3, 0.3, 0.5, 1);
  else
    c.DrawEdges(3, 0.3, 0.5, 1, m_cagevtx_hl );

  //cage vtx
  glEnable ( GL_LIGHTING );
  glEnable(GL_CULL_FACE);
  glCullFace( GL_BACK );
  for ( int i = 0; i < c.m_vSize; ++i)
  {
    if( !allmesh && !m_cagevtx_hl[i] ) continue;

    if ( m_cagevtx_hl[i] )
      TMesh::DrawSphere( c.m_vVerts[i], m_CP_RADIUS, 
        COLOR_R, COLOR_R, COLOR_W, COLOR_SHIN64);
    else
      TMesh::DrawSphere( c.m_vVerts[i], m_CP_RADIUS, 
        COLOR_GRAY, COLOR_GRAY, COLOR_W, COLOR_SHIN64);
  }
}



void CagedMeshSequence::DrawHandle(int frame_idx, int trans_rot_scale) const
{
  if ( frame_idx < 0 || (int)m_cages.size() <= frame_idx  ) return; 
  if ( GetNumSelectedVtx() <= 0 ) return;
  const EVec3f c = GetSelectedVtxCentroid( frame_idx );
  const float len = m_HANDLE_LENGTH;
  const float wid = m_HANDLE_WIDTH;
  //translation, rotation, scaling
  if ( trans_rot_scale == 0) 
    DrawHandleOrthoArrows( c, len, wid, COLOR_R, COLOR_G, COLOR_B);
  if ( trans_rot_scale == 1) 
    DrawHandleOrthoCircles(c, m_HANDLE_LENGTH                    );
  if ( trans_rot_scale == 2)
    DrawHandleOrthoCubes  (c, len, wid, COLOR_R, COLOR_G, COLOR_B);
}



void CagedMeshSequence::DrawHandleCenter(int frame_idx, int trans_rot_scale) const
{
  if (frame_idx < 0 || (int)m_cages.size() <= frame_idx) return;
  
  EVec3f gc(0, 0, 0);
  for (int i = 0; i < (int)m_cagevtx_hl.size(); ++i)
  {
      gc += m_cages[frame_idx].m_vVerts[i];
  }
  if (m_cages[frame_idx].m_vSize != 0) gc /= (float) m_cages[frame_idx].m_vSize;

  const EVec3f c = gc;
  const float len = m_HANDLE_LENGTH;
  const float wid = m_HANDLE_WIDTH;
  //translation, rotation, scaling
  if (trans_rot_scale == 0)
    DrawHandleOrthoArrows(c, len, wid, COLOR_R, COLOR_G, COLOR_B);
  if (trans_rot_scale == 1)
    DrawHandleOrthoCircles(c, m_HANDLE_LENGTH);
  if (trans_rot_scale == 2)
    DrawHandleOrthoCubes(c, len, wid, COLOR_R, COLOR_G, COLOR_B);
}





ORTHO_HANDLE_ID CagedMeshSequence::PickCageHandle(
    const int    frame_idx, 
    const EVec3f ray_pos  , 
    const EVec3f ray_dir  , 
    const int trans_rot_scale)
{
  if ( frame_idx < 0 || (int)m_cages.size() <= frame_idx  ) return OHDL_NON;
  if ( GetNumSelectedVtx() <= 0 ) return OHDL_NON;

  const float length = m_HANDLE_LENGTH;
  const float radius = m_HANDLE_WIDTH ;
  const EVec3f c = GetSelectedVtxCentroid( frame_idx );

  if ( trans_rot_scale == 0) {
    return PickHandleOrthoArrows (ray_pos, ray_dir, c, length, radius);
  }
  else if ( trans_rot_scale == 1){
    return PickHandleOrthoCircles(ray_pos, ray_dir, c, length, radius);
  }
  else if ( trans_rot_scale == 2){
    return PickHandleOrthoArrows (ray_pos, ray_dir, c, length, radius);
  }
  return OHDL_NON;
}




void CagedMeshSequence::ImportCageSequenceFromTxt(
    std::string fname,
    bool modify_num_frame // if true --> modify frame num 
    )
{
  if (!IsInitialized()) return;
  int num_frames = (int)m_cages.size();
  int num_verts  = m_cages[0].m_vSize;

  std::ifstream ifs(fname);

  if (ifs.fail())
  {
    ShowMsgDlg_OK("file open error", "err");
    return ;
  }
  
  std::string str;
  int tmp_numframe, tmp_numverts;
  ifs >> str >> tmp_numframe >> tmp_numverts;

  
  if (modify_num_frame && tmp_numframe != num_frames)
  {
    // frame����ύX����
    TMesh c = m_cages[0];
    TMesh m = m_meshes[0];
    m_cages.resize(tmp_numframe);
    m_meshes.resize(tmp_numframe);
    m_cagevtx_hl.resize(c.m_vSize, 0);

    for (int i = 0; i < tmp_numframe; ++i)
    {
      m_meshes[i] = m;
      m_cages[i] = c;
    }
    num_frames = (int)m_cages.size();

  }


  if ( tmp_numframe != num_frames || tmp_numverts != num_verts )
  {
    ShowMsgDlg_OK("sizes of frames or verts are strange", "err");
    ifs.close();
    return ;
  }

  for ( int f = 0; f < num_frames; ++f)
  {
    int tmp_i;
    ifs >> str >> tmp_i;

    for ( int j = 0; j < num_verts; ++j)
    {
      EVec3f &p = m_cages[f].m_vVerts[j];
      ifs >> p[0] >> p[1] >> p[2];
    }
    UpdateMeshShape( f );
  }
  ifs.close();
}





void CagedMeshSequence::ExportCageMeshSequenceAsTxt(std::string fname, bool b_cage)
{
  if (!IsInitialized()) return;

  std::ofstream ofs(fname.c_str());
  if (!ofs.is_open())
  {
    std::cout << "error when opening " << fname << "\n";
    return;
  }

  const std::vector<TMesh>& objs = b_cage ? m_cages : m_meshes;
  const int num_frames = (int)objs.size();
  const int num_verts  = objs[0].m_vSize;
  ofs << "cagemesh_vtx_info " << num_frames << " " << num_verts << "\n";

  for (int f = 0; f < num_frames; ++f)
  {
    ofs << "frame " << f << "\n";
    for (int j = 0; j < num_verts; ++j)
    {
      WriteToFstream(ofs, objs[f].m_vVerts[j]);
    }
  }
  ofs.close();

}





void CagedMeshSequence::ExportCageMeshSequenceAsObj(std::string fname, bool b_cage)
{
  if ( !IsInitialized() ) return;

  const std::vector<TMesh> &objs = b_cage ? m_cages : m_meshes;

  for (int fi = 0, size= (int)objs.size(); fi < size; ++fi)
  {
    std::string name_str = fname.substr(0, fname.find_last_of("."));
    std::string ss = fi < 10 ? "00" : fi < 100 ? "0" : "";
    name_str = name_str + ss + std::to_string(fi) + ".obj";

    objs[fi].ExportObjNoTexCd(name_str.c_str());
  }
}





#pragma managed

