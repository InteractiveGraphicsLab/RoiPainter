#pragma unmanaged
#include "ttrianglesoup.h"

//3rdparty/glew
#include "gl/glew.h"
#include <gl/gl.h> 
#include <gl/glu.h> 

//COMMON
#include <map>
#include "tmath.h"
#include "tmesh.h"
#include "tqueue.h"

void TTriangleSoup::Clear()
{
  if( m_triangles != nullptr ) delete[] m_triangles; 
  if( m_normals   != nullptr) delete[] m_normals  ;
  m_num_triangles = 0;
  m_triangles = nullptr;
  m_normals   = nullptr;
}

void TTriangleSoup::Set(const TTriangleSoup &src)
{
  Clear();
    
  m_num_triangles = src.m_num_triangles;
  if( m_num_triangles == 0 ) return;

  m_triangles = new TTriangle[m_num_triangles];
  m_normals   = new EVec3f   [m_num_triangles];

  for( int i = 0; i < m_num_triangles; ++i)
  {
    m_triangles[i] = src.m_triangles[i];
    m_normals[i] = src.m_normals[i];
  }
}


TTriangleSoup::~TTriangleSoup()
{
  Clear();
}

TTriangleSoup::TTriangleSoup()
{
  m_num_triangles = 0;
  m_triangles = nullptr;
  m_normals = nullptr;
}

TTriangleSoup::TTriangleSoup( const TTriangleSoup &src)
{
  m_num_triangles = 0;
  m_triangles = nullptr;
  m_normals = nullptr;
  Set(src);
}

TTriangleSoup& TTriangleSoup::operator= (const TTriangleSoup  &v) 
{
  Set(v); 
  return *this; 
}


void TTriangleSoup::Allocate(int num_triangles, EVec3f *triangle_vertex_array)
{
  Clear();
  m_num_triangles = num_triangles;
  m_triangles = new TTriangle[m_num_triangles];
  m_normals   = new EVec3f   [m_num_triangles];

  if( triangle_vertex_array == 0) return;
    
  for(int i = 0; i <  m_num_triangles; ++i )
  {
    m_triangles[i].m_verts[0] = triangle_vertex_array[3*i+0];
    m_triangles[i].m_verts[1] = triangle_vertex_array[3*i+1];
    m_triangles[i].m_verts[2] = triangle_vertex_array[3*i+2];
  }

  UpdateNormal();
}

void TTriangleSoup::UpdateNormal()
{
#pragma omp parallel for
  for (int i = 0; i < m_num_triangles ; ++i) m_normals[i].setZero();

  for (int i = 0; i < m_num_triangles ; ++i)
  {
    TTriangle &t = m_triangles[i];
    m_normals[i] = (t.m_verts[1] - t.m_verts[0]).cross(t.m_verts[2] - t.m_verts[0]);
    float l = m_normals[i].norm();
    if( l != 0 ) m_normals[i] /= l;
  }
}
  
  
bool TTriangleSoup::ExportAsStl(const char *fname)
{
  FILE* fp = fopen(fname, "w");
  if (!fp) return false;

  fprintf(fp, "solid ttrianglesoup\n");
  for (int i = 0; i < m_num_triangles; ++i)
  {
    const TTriangle &t = m_triangles[i];
    fprintf(fp, "facet normal %f %f %f\n", m_normals[i][0], m_normals[i][1], m_normals[i][2]);
    fprintf(fp, "  outer loop\n");
    fprintf(fp, "    vertex %f %f %f\n", t.m_verts[0][0], t.m_verts[0][1], t.m_verts[0][2]);
    fprintf(fp, "    vertex %f %f %f\n", t.m_verts[1][0], t.m_verts[1][1], t.m_verts[1][2]);
    fprintf(fp, "    vertex %f %f %f\n", t.m_verts[2][0], t.m_verts[2][1], t.m_verts[2][2]);
    fprintf(fp, "  endloop\n");
    fprintf(fp, "endfacet\n");
  }
  fprintf(fp, "endsolid tmesh\n");
  fclose(fp);

  return true;
}


void TTriangleSoup::Draw() const
{
  if (m_num_triangles == 0 ) return;
  glBegin(GL_TRIANGLES);
  for( int i=0; i < m_num_triangles; ++i) 
  {
    const TTriangle& t = m_triangles[i];
    glNormal3fv( m_normals[i].data() );
    glVertex3fv( t.m_verts[0].data() );
    glVertex3fv( t.m_verts[1].data() );
    glVertex3fv( t.m_verts[2].data() );
  }
  glEnd();
}


//only for surface where each vertex has unique texture coordinate
void TTriangleSoup::Draw(
    const float *diff, 
    const float *ambi, 
    const float *spec, 
    const float *shin) const
{
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  diff);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  ambi);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,shin);
  Draw();
}

void TTriangleSoup::DrawEdges()
{
  glBegin(GL_LINES);
  for (int i = 0; i < m_num_triangles; ++i)
  {
    const TTriangle& t = m_triangles[i];
    glVertex3fv( t.m_verts[0].data() ); glVertex3fv( t.m_verts[1].data() );
    glVertex3fv( t.m_verts[1].data() ); glVertex3fv( t.m_verts[2].data() );
    glVertex3fv( t.m_verts[2].data() ); glVertex3fv( t.m_verts[0].data() );
  }
  glEnd();
}

void TTriangleSoup::DrawEdges(int width, double r, double g, double b)
{
  glLineWidth((float)width);
  glColor3d(r, g, b);
  DrawEdges();
}


void TTriangleSoup::ImportObj( const char* fname )
{
  TMesh mesh;
  mesh.Initialize(fname);
    
  Allocate(mesh.m_pSize);

  if( m_num_triangles == 0 ) return;

  for( int t = 0; t < mesh.m_pSize; ++t)
  {
    int* idx = mesh.m_pPolys[t].idx;
    m_triangles[t].m_verts[0] = mesh.m_vVerts[idx[0]];
    m_triangles[t].m_verts[1] = mesh.m_vVerts[idx[1]];
    m_triangles[t].m_verts[2] = mesh.m_vVerts[idx[2]];
  }
  UpdateNormal();
}
  

EVec3f TTriangleSoup::GetGravityCenter()
{
  EVec3f gc(0,0,0);

  for( int t = 0; t < m_num_triangles; ++t)
  {
    gc += m_triangles[t].m_verts[0];
    gc += m_triangles[t].m_verts[1];
    gc += m_triangles[t].m_verts[2];
  }
  return gc / (float)(3 * m_num_triangles);
}


void TTriangleSoup::Translate(const EVec3f &trans)
{
  for( int t = 0; t < m_num_triangles; ++t)
  {
    m_triangles[t].m_verts[0] += trans;
    m_triangles[t].m_verts[1] += trans;
    m_triangles[t].m_verts[2] += trans;
  }
}


void TTriangleSoup::MultMat(const EMat4f M)
{
  EMat3f R;
  R <<  M(0, 0), M(0, 1), M(0, 2),
        M(1, 0), M(1, 1), M(1, 2),
        M(2, 0), M(2, 1), M(2, 2);
  EVec3f t(M(0, 3), M(1, 3), M(2, 3));

  Trace(R);
  Trace(t);

  for (int i = 0; i < m_num_triangles; ++i) 
  {
    m_triangles[i].m_verts[0] = R * m_triangles[i].m_verts[0] + t;
    m_triangles[i].m_verts[1] = R * m_triangles[i].m_verts[1] + t;
    m_triangles[i].m_verts[2] = R * m_triangles[i].m_verts[2] + t;
  }
  UpdateNormal();
}

void TTriangleSoup::CalcBoundBox(EVec3f& BBmin, EVec3f& BBmax) const
{
  BBmin <<  FLT_MAX,  FLT_MAX,  FLT_MAX;
  BBmax << -FLT_MAX, -FLT_MAX, -FLT_MAX;

  for (int i = 0; i < m_num_triangles; ++i)
  {
    const TTriangle& t = m_triangles[i];
    for (int k = 0; k < 3; ++k)
    {
      BBmin[0] = std::min(BBmin[0], t.m_verts[k][0]);
      BBmin[1] = std::min(BBmin[1], t.m_verts[k][1]);
      BBmin[2] = std::min(BBmin[2], t.m_verts[k][2]);
      BBmax[0] = std::max(BBmax[0], t.m_verts[k][0]);
      BBmax[1] = std::max(BBmax[1], t.m_verts[k][1]);
      BBmax[2] = std::max(BBmax[2], t.m_verts[k][2]);
    }
  }
}


bool TTriangleSoup::PickByRay(
    const EVec3f& rayP, 
    const EVec3f& rayD, 
    EVec3f& pos, 
    int& pid) const
{
  float depth = FLT_MAX;
  EVec3f tmpPos;
  pid = -1;

  for (int t = 0; t < m_num_triangles; ++t)
  {
    if (t_intersectRayToTriangle(rayP, rayD, 
                                  m_triangles[t].m_verts[0], 
                                  m_triangles[t].m_verts[1], 
                                  m_triangles[t].m_verts[2], tmpPos))
    {
      float d = (tmpPos - rayP).norm();
      if (d < depth)
      {
        depth = d;
        pos = tmpPos;
        pid = t;
      }
    }
  }
  return depth != FLT_MAX;
}




// cast ray in Y axis ( divide ZX plane )  
void GenBinaryVolumeFromMesh_Y
(
  const EVec3i        &reso,
  const EVec3f        &pitch,
  const TTriangleSoup &mesh,

  byte *binVol //allocated[WxHxD], 0:out, 1:in
)
{
  const int    W  = reso[0];
  const int    H  = reso[1];
  const int    D  = reso[2];
  const double px = pitch[0];
  const double py = pitch[1];
  const double pz = pitch[2];
  const int num_tris = mesh.m_num_triangles;
  const TTriangle *tris  = mesh.m_triangles;
  const EVec3f    *norms = mesh.m_normals;


  //clock_t t0 = clock();
  const int WH = W*H, WHD = W*H*D;
  const EVec3f cuboid((float)(W*px), (float)(H*py), (float)(D*pz));

  EVec3f BBmin, BBmax;
  mesh.CalcBoundBox(BBmin, BBmax);

  memset(binVol, 0, sizeof(byte) * WHD);


  // insert triangles in BINs -- divide yz space into (BIN_SIZE x BIN_SIZE)	
  const int BIN_SIZE = 100;
  std::vector< std::vector<int> > polyID_Bins(BIN_SIZE * BIN_SIZE, std::vector<int>());

  for (int p = 0; p < num_tris; ++p)
  {
    const TTriangle &t = tris[p];

    EVec3f bbMin, bbMax;
    t_CalcBoundingBox( t.m_verts[0], t.m_verts[1], t.m_verts[2], bbMin, bbMax);
    int xS = t_crop<int>(0, BIN_SIZE - 1, (int)(bbMin[0] / cuboid[0] * BIN_SIZE));
    int zS = t_crop<int>(0, BIN_SIZE - 1, (int)(bbMin[2] / cuboid[2] * BIN_SIZE));
    int xE = t_crop<int>(0, BIN_SIZE - 1, (int)(bbMax[0] / cuboid[0] * BIN_SIZE));
    int zE = t_crop<int>(0, BIN_SIZE - 1, (int)(bbMax[2] / cuboid[2] * BIN_SIZE));

    for (int z = zS; z <= zE; ++z) for (int x = xS; x <= xE; ++x) polyID_Bins[z*BIN_SIZE + x].push_back(p);
  }

  // ray casting along x axis to fill inside the mesh 
#pragma omp parallel for
  for (int zI = 0; zI < D; ++zI) if (BBmin[2] <= (0.5 + zI) * pz && (0.5 + zI) * pz <= BBmax[2])
  {
    for (int xI = 0; xI < W; ++xI) if (BBmin[0] <= (0.5 + xI) * px && (0.5 + xI) * px <= BBmax[0])
    {
      double x = (0.5 + xI) * px;
      double z = (0.5 + zI) * pz;
      int bin_xi = t_crop<int>( 0, BIN_SIZE-1, (int)(x / cuboid[0] * BIN_SIZE) );
      int bin_zi = t_crop<int>( 0, BIN_SIZE-1, (int)(z / cuboid[2] * BIN_SIZE) );
      std::vector<int> &trgtBin = polyID_Bins[bin_zi * BIN_SIZE + bin_xi];

      std::multimap<double, double> blist;// (xPos, normInXdir);

      for (const auto pi : trgtBin) if ( norms[pi][1] != 0)
      {
        const TTriangle &t = tris[pi];
        double y;
        if (t_IntersectRayYToTriangle(t.m_verts[0], t.m_verts[1], t.m_verts[2], x, z, y))
          blist.insert( std::make_pair(y, norms[pi][1] )); //(y 座標, normal)
      }

      if (blist.size() == 0) continue;

      //clean blist (edge上で起こった交差重複を削除)
      while (blist.size() != 0)
      {
        if (blist.size() == 1) { blist.clear(); break; }

        bool found = false;
        auto it0 = blist.begin();
        auto it1 = blist.begin(); it1++;

        for (; it1 != blist.end(); ++it0, ++it1) if (it0->second * it1->second > 0)
        {
          blist.erase(it1);
          found = true;
          break;
        }
        if (!found) break;
      }

      bool flag = false;
      int yI = 0;

      //int pivIdx = xI ;
      for (auto it = blist.begin(); it != blist.end(); ++it)
      {
        int pivYi = (int)(it->first / py);
        for (; yI <= pivYi && yI < H; ++yI) binVol[xI + yI * W + zI*WH] = flag;
        flag = !flag;
      }
      if (flag == true) std::cout << "error double check here!";
    }
  }
  //clock_t t2 = clock();
  //std::cout << "compute time : " << (t1-t0)/ (double) CLOCKS_PER_SEC << " " << (t2-t1)/ (double) CLOCKS_PER_SEC) << "\n";
}