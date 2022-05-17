#pragma unmanaged
#include "tmesh.h"

//3rdparty/glew
#include "gl/glew.h"
#include <gl/gl.h> 
#include <gl/glu.h> 

//stl
#include <iostream>
#include <list>
#include <vector>
#include <set>
#include <map>

//COMMON
#include "tmath.h"
#include "tqueue.h"

#pragma warning(disable : 4996)

void TMesh::SetFieldNull()
{
  m_vSize = 0;
  m_vVerts = nullptr;
  m_vNorms = nullptr;
  m_vTexCd = nullptr;
  m_vRingPs = nullptr;
  m_vRingVs = nullptr;
  m_pSize = 0;
  m_pNorms = nullptr;
  m_pPolys = nullptr;
}


TMesh::TMesh()
{
  SetFieldNull();
}

TMesh::TMesh(const char* fname)
{
  SetFieldNull();
  Initialize(fname);
}

TMesh::TMesh(const TMesh& src)
{
  SetFieldNull();
  Set(src);
}

TMesh &TMesh::operator=(const TMesh& src)
{
  SetFieldNull();
  Set(src);
  return *this;
}

TMesh::~TMesh()
{
  Clear();
}


void TMesh::Clear()
{
  if (m_vVerts  != nullptr) delete[] m_vVerts;
  if (m_vNorms  != nullptr) delete[] m_vNorms;
  if (m_vTexCd  != nullptr) delete[] m_vTexCd;
  if (m_pNorms  != nullptr) delete[] m_pNorms;
  if (m_pPolys  != nullptr) delete[] m_pPolys;
  if (m_vRingPs != nullptr) delete[] m_vRingPs;
  if (m_vRingVs != nullptr) delete[] m_vRingVs;
  SetFieldNull();
}


void TMesh::Set(const TMesh &v)
{
  Clear();
  m_vSize = v.m_vSize;
  m_pSize = v.m_pSize;

  if (m_vSize != 0)
  {
    m_vVerts = new EVec3f[m_vSize];
    m_vNorms = new EVec3f[m_vSize];
    m_vTexCd = new EVec3f[m_vSize];
    memcpy(m_vVerts, v.m_vVerts, sizeof(EVec3f) * m_vSize);
    memcpy(m_vNorms, v.m_vNorms, sizeof(EVec3f) * m_vSize);
    memcpy(m_vTexCd, v.m_vTexCd, sizeof(EVec3f) * m_vSize);

    m_vRingVs = new std::vector<int>[m_vSize];
    m_vRingPs = new std::vector<int>[m_vSize];
    for (int i = 0; i < m_vSize; ++i)
    {
      m_vRingVs[i] = v.m_vRingVs[i];
      m_vRingPs[i] = v.m_vRingPs[i];
    }
  }

  if (m_pSize != 0)
  {
    m_pNorms = new EVec3f[m_pSize];
    m_pPolys = new TPoly[m_pSize];
    memcpy(m_pNorms, v.m_pNorms, sizeof(EVec3f) * m_pSize);
    memcpy(m_pPolys, v.m_pPolys, sizeof(TPoly) * m_pSize);
  }
}


static bool IsSame(const std::vector<TPoly>& Ps, const std::vector<TPoly>& Puv)
{
  if (Ps.size() != Puv.size()) return false;
  for (int i = 0; i < (int)Ps.size(); ++i)
  {
    if (Ps[i].idx[0] != Puv[i].idx[0] ||
      Ps[i].idx[1] != Puv[i].idx[1] ||
      Ps[i].idx[2] != Puv[i].idx[2]) return false;
  }
  return true;
}


bool TMesh::Initialize(const char *fName)
{
  FILE* fp = fopen(fName, "r");
  if (!fp) return false;

  std::list<EVec3f>  vList;
  std::list<EVec2f>  uvList;
  std::list<TPoly >  pList;
  std::list<TPoly >  pUvList;

  char buf[512];
  while (fgets(buf, 255, fp))
  {
    char* bkup = _strdup(buf);
    char* token = strtok(buf, " \t");

    if (stricmp(token, "vt") == 0)
    {
      double u, v;
      sscanf(bkup, "vt %lf %lf", &u, &v);
      uvList.push_back(EVec2f((float)u, (float)v));
    }
    else if (stricmp(token, "v") == 0)
    {
      double x, y, z;
      sscanf(bkup, "v %lf %lf %lf", &x, &y, &z);
      vList.push_back(EVec3f((float)x, (float)y, (float)z));
    }
    else if (stricmp(token, "f") == 0)
    {
      //sscanfの返り値は正常に読めた数: / が入ったら2文字しか読めない

      int v[3], t[3], s;
      int vtxnum = sscanf(bkup, "f %d %d %d %d", &v[0], &v[1], &v[2], &s);
      if (vtxnum < 3) vtxnum = sscanf(bkup, "f %d/%d %d/%d %d/%d %d/%d"            , &v[0], &t[0], &v[1], &t[1], &v[2], &t[2], &s, &s) / 2;
      if (vtxnum < 3) vtxnum = sscanf(bkup, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v[0], &t[0], &s, &v[1], &t[1], &s, &v[2], &t[2], &s, &s, &s, &s) / 3;
      if (vtxnum < 3) vtxnum = sscanf(bkup, "f %d//%d %d//%d %d//%d %d//%d"        , &v[0], &s, &v[1], &s, &v[2], &s, &s, &s) / 2;
      pList  .push_back(TPoly(v[0] - 1, v[1] - 1, v[2] - 1));
      pUvList.push_back(TPoly(t[0] - 1, t[1] - 1, t[2] - 1));

    }
    free(bkup);
  }
  fclose(fp);

  std::vector<EVec3f> Vs { std::begin(vList  ), std::end(vList  ) };
  std::vector<EVec2f> Ts { std::begin(uvList ), std::end(uvList ) };
  std::vector<TPoly>  Ps { std::begin(pList  ), std::end(pList  ) };
  std::vector<TPoly>  Puv{ std::begin(pUvList), std::end(pUvList) };
  Initialize(Vs, Ps);

  //1頂点につき1 texCdのときのみ，入力されたTexCdを利用
  if (Ts.size() == Vs.size() && Ps.size() == Puv.size() && IsSame(Ps, Puv))
  {
    for (int i = 0; i < m_vSize; ++i) m_vTexCd[i] << Ts[i][0], Ts[i][1], 0;
  }

  std::cout << "Obj File Loaded:" << m_vSize << " " << m_pSize << "\n";
  return true;
}



void TMesh::Initialize(const std::vector<EVec3f> &Vs, const std::vector<TPoly> &Ps)
{
  Clear();

  m_vSize = (int)Vs.size();
  if (m_vSize != 0)
  {
    m_vVerts = new EVec3f[m_vSize];
    m_vNorms = new EVec3f[m_vSize];
    m_vTexCd = new EVec3f[m_vSize];
    m_vRingVs = new std::vector<int>[m_vSize];
    m_vRingPs = new std::vector<int>[m_vSize];
    for (int i = 0; i < m_vSize; ++i) m_vVerts[i] = Vs[i];
  }

  m_pSize = (int)Ps.size();
  if (m_pSize != 0)
  {
    m_pPolys = new TPoly[m_pSize];
    m_pNorms = new EVec3f[m_pSize];
    for (int i = 0; i < m_pSize; ++i) m_pPolys[i] = Ps[i];
  }

  UpdateNormal();
  UpdateRingInfo();

  //for debug
  std::cout << "for debug : check data\n";
  for (int i = 0; i < m_pSize; ++i)
  {
    int *idx = m_pPolys[i].idx;
    if (idx[0] < 0 || idx[0] >= m_vSize) std::cout << "error 1";
    if (idx[1] < 0 || idx[1] >= m_vSize) std::cout << "error 2";
    if (idx[2] < 0 || idx[2] >= m_vSize) std::cout << "error 3";
  }
}

  


void TMesh::InitializeIcosaHedron(const double r)
{
  float a = (float)(r * 0.525731);
  float b = (float)(r * 0.850651);
  std::vector<EVec3f> Vs = {
    EVec3f(0,-a, b), EVec3f(b, 0, a), EVec3f(b, 0,-a),
    EVec3f(-b, 0,-a), EVec3f(-b, 0, a), EVec3f(-a, b, 0),
    EVec3f(a, b, 0), EVec3f(a,-b, 0), EVec3f(-a,-b, 0),
    EVec3f(0,-a,-b), EVec3f(0, a,-b), EVec3f(0, a, b) };
  std::vector<TPoly> Ps = {
    TPoly(1, 2, 6), TPoly(1, 7, 2), TPoly(3, 4, 5), TPoly( 4, 3, 8),
    TPoly(6, 5,11), TPoly(5, 6,10), TPoly(9,10, 2), TPoly(10, 9, 3),
    TPoly(7, 8, 9), TPoly(8, 7, 0), TPoly(11,0, 1), TPoly( 0,11, 4),
    TPoly(6, 2,10), TPoly(1, 6,11), TPoly(3, 5,10), TPoly( 5, 4,11),
    TPoly(2, 7, 9), TPoly(7, 1, 0), TPoly(3, 9, 8), TPoly( 4, 8, 0) };
  Initialize(Vs, Ps);
}



void TMesh::InitializeAsSphere(
  const float r,
  const int reso_hori,
  const int reso_verti)
{
  std::vector<EVec3f> vs;
  std::vector<TPoly>  ps;

  const float step_phi = (float)(M_PI / (reso_verti + 1.0));
  const float step_theta = (float)(2 * M_PI / (reso_hori));

  vs.push_back(EVec3f(0, 0, -1)); //南極
  for (int phi_i = 0; phi_i < reso_verti; ++phi_i)
  {
    //std::cout << "\n";
    for (int theta_i = 0; theta_i < reso_hori; ++theta_i)
    {
      float phi = step_phi * (phi_i + 1) - (float)(0.5 * M_PI);
      float theta = step_theta * theta_i;
      //std::cout << "(" <<phi << "," << theta << ")";

      vs.push_back(EVec3f(r * cos(theta) * cos(phi),
        r * sin(theta) * cos(phi),
        r * sin(phi)));
    }
  }
  vs.push_back(EVec3f(0, 0, 1)); //北極

  //vertex size == reso_hori * reso_verti + 2

  //蓋
  for (int theta_i = 0; theta_i < reso_hori; ++theta_i)
  {
    ps.push_back(TPoly(0, 1 + (theta_i + 1) % reso_hori, 1 + theta_i));
  }

  for (int phi_i = 0; phi_i < reso_verti - 1; ++phi_i)
  {
    for (int theta_i = 0; theta_i < reso_hori; ++theta_i)
    {
      int i0 = 1 + phi_i * reso_hori + theta_i;
      int i1 = 1 + phi_i * reso_hori + (theta_i + 1) % reso_hori;
      int i2 = 1 + (phi_i + 1) * reso_hori + (theta_i + 1) % reso_hori;
      int i3 = 1 + (phi_i + 1) * reso_hori + theta_i;
      ps.push_back(TPoly(i0, i1, i2));
      ps.push_back(TPoly(i0, i2, i3));
    }
  }
  const int n = (int)vs.size() - 1;
  for (int theta_i = 0; theta_i < reso_hori; ++theta_i)
  {
    ps.push_back(TPoly(n, n - reso_hori +  theta_i, 
                          n - reso_hori + (theta_i + 1) % reso_hori));
  }
  Initialize(vs, ps);
}





void TMesh::Smoothing(int n)
{
  EVec3f *vs = new EVec3f[m_vSize];

  for (int k = 0; k < n; ++k)
  {
#pragma omp parallel for
    for (int i = 0; i < m_vSize; ++i)
    {
      vs[i] << 0, 0, 0;
      for (const auto &it : m_vRingVs[i]) vs[i] += m_vVerts[it];
      vs[i] /= (float)m_vRingVs[i].size();
    }
    std::swap(vs, m_vVerts);
  }
  delete[] vs;
  UpdateNormal();
}


void TMesh::UpdateNormal()
{
#pragma omp parallel for
  for (int i = 0; i < m_vSize; ++i) 
    m_vNorms[i].setZero();

  for (int i = 0; i < m_pSize; ++i)
  {
    int *idx = m_pPolys[i].idx;

    // ※ゼロ割に気を付ける
    m_pNorms[i] = (m_vVerts[idx[1]] - m_vVerts[idx[0]]).cross(m_vVerts[idx[2]] - m_vVerts[idx[0]]);
    float l = m_pNorms[i].norm();
    if( l != 0 ) m_pNorms[i] /= l;

    m_vNorms[idx[0]] += m_pNorms[i];
    m_vNorms[idx[1]] += m_pNorms[i];
    m_vNorms[idx[2]] += m_pNorms[i];
  }

#pragma omp parallel for
  for (int i = 0; i < m_vSize; ++i) 
  {
    float n = m_vNorms[i].norm(); 
    if ( n > 0)  m_vNorms[i] /= n;
  }
}


void TMesh::UpdateRingInfo()
{
  for (int i = 0; i < m_vSize; ++i) m_vRingPs[i].clear();
  for (int i = 0; i < m_vSize; ++i) m_vRingVs[i].clear();

  for (int i = 0; i < m_pSize; ++i)
  {
    int *idx = m_pPolys[i].idx;
    m_vRingPs[idx[0]].push_back(i);
    m_vRingPs[idx[1]].push_back(i);
    m_vRingPs[idx[2]].push_back(i);
    m_vRingVs[idx[0]].push_back(idx[1]);
    m_vRingVs[idx[0]].push_back(idx[2]);
    m_vRingVs[idx[1]].push_back(idx[0]);
    m_vRingVs[idx[1]].push_back(idx[2]);
    m_vRingVs[idx[2]].push_back(idx[0]);
    m_vRingVs[idx[2]].push_back(idx[1]);
  }

  for (int i = 0; i < m_vSize; ++i)
  {
    sort(m_vRingVs[i].begin(), m_vRingVs[i].end());
    auto it = unique(m_vRingVs[i].begin(), m_vRingVs[i].end());
    m_vRingVs[i].erase(it, m_vRingVs[i].end());
  }
}

void TMesh::Translate(const EVec3f t)
{ 
  for (int i = 0; i < m_vSize; ++i) m_vVerts[i] += t; 
}

void TMesh::Scale(const float  s)
{ 
  for (int i = 0; i < m_vSize; ++i) m_vVerts[i] *= s; 
}

void TMesh::Rotate(Eigen::AngleAxis<float> &R)
{ 
  for (int i = 0; i < m_vSize; ++i) m_vVerts[i] = R * m_vVerts[i]; 
}

void TMesh::Rotate(const EMat3f &R)
{ 
  for (int i = 0; i < m_vSize; ++i) m_vVerts[i] = R * m_vVerts[i]; 
}

void TMesh::MultMat(const EMat4f M)
{
  EMat3f R;
  R << M(0, 0), M(0, 1), M(0, 2),
        M(1, 0), M(1, 1), M(1, 2),
        M(2, 0), M(2, 1), M(2, 2);
  EVec3f t(M(0, 3), M(1, 3), M(2, 3));
  for (int i = 0; i < m_vSize; ++i) m_vVerts[i] = R * m_vVerts[i] + t;
  UpdateNormal();
}


void TMesh::NormalizeByUniformScaling()
{
  EVec3f minV, maxV;
  GetBoundBox(minV, maxV);
  EVec3f a = maxV - minV;
  float  s = std::max(a[0], std::max(a[1], a[2]));

  Translate(-minV);
  Scale(1.0f / s);
}



EVec3f TMesh::GetGravityCenter() const
{
  EVec3f p(0,0,0);
  for (int i = 0; i < m_vSize; ++i) p += m_vVerts[i];
  return p / (float)m_vSize;
}

void TMesh::GetBoundBox(EVec3f &BBmin, EVec3f &BBmax) const
{
  t_CalcBoundingBox(m_vSize, m_vVerts, BBmin, BBmax);
}



//rendering 


void TMesh::Draw() const
{
  if (m_vSize == 0 || m_pSize == 0) return;

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glNormalPointer(GL_FLOAT, 0, m_vNorms);
  glTexCoordPointer(3, GL_FLOAT, 0, m_vTexCd);
  glVertexPointer(3, GL_FLOAT, 0, m_vVerts);
  glDrawElements(GL_TRIANGLES, m_pSize * 3, GL_UNSIGNED_INT, m_pPolys);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


//only for surface where each vertex has unique texture coordinate
void TMesh::Draw(
    const float *diff, 
    const float *ambi, 
    const float *spec, 
    const float *shin ) const
{
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambi);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shin);
  Draw();
}

void TMesh::DrawEdges() const
{
  glBegin(GL_LINES);
  for (int i = 0; i < m_pSize; ++i)
  {
    int* idx = m_pPolys[i].idx;
    glVertex3fv(m_vVerts[idx[0]].data());
    glVertex3fv(m_vVerts[idx[1]].data());
    glVertex3fv(m_vVerts[idx[1]].data());
    glVertex3fv(m_vVerts[idx[2]].data());
    glVertex3fv(m_vVerts[idx[2]].data());
    glVertex3fv(m_vVerts[idx[0]].data());
  }
  glEnd();
}

void TMesh::DrawEdges(int width, double r, double g, double b) const
{
  glLineWidth((float)width);
  glColor3d(r, g, b);
  DrawEdges();
}

//draw edge [i,j] if vert_flg[i] || vert_flg[j] 
void TMesh::DrawEdges( 
    int width, 
    double r, 
    double g, 
    double b, 
    const std::vector<int> &vert_flg ) const
{
  glLineWidth((float)width);
  glColor3d(r, g, b);

  glBegin(GL_LINES);
  for (int i = 0; i < m_vSize; ++i)
  {
    if (!vert_flg[i]) continue;
    for ( const auto &vi : m_vRingVs[i] )
    {
      glVertex3fv(m_vVerts[ i].data());
      glVertex3fv(m_vVerts[vi].data());
    }
  }
  glEnd();
}


void TMesh::ExportObjNoTexCd(const char *fname) const
{
  FILE* fp = fopen(fname, "w");
  if (!fp) return;

  fprintf(fp, "#Obj exported from tmesh\n");

  for (int i = 0; i < m_vSize; ++i)
    fprintf(fp, "v %f %f %f\n", m_vVerts[i][0], 
                                m_vVerts[i][1], 
                                m_vVerts[i][2]);

  for (int i = 0; i < m_pSize; ++i)
    fprintf(fp, "f %d %d %d\n", m_pPolys[i].idx[0] + 1, 
                                m_pPolys[i].idx[1] + 1, 
                                m_pPolys[i].idx[2] + 1);
  fclose(fp);
}


bool TMesh::ExportStl(const char *fname)
{
  FILE* fp = fopen(fname, "w");
  if (!fp) return false;

  fprintf(fp, "solid tmesh\n");
  for (int i = 0; i < m_pSize; ++i)
  {
    fprintf(fp, "facet normal %f %f %f\n", m_pNorms[i][0], 
                                            m_pNorms[i][1], 
                                            m_pNorms[i][2]);
    fprintf(fp, "  outer loop\n");

    int *p = m_pPolys[i].idx;
    fprintf(fp, "    vertex %f %f %f\n", m_vVerts[p[0]][0], 
                                          m_vVerts[p[0]][1], 
                                          m_vVerts[p[0]][2]);
    fprintf(fp, "    vertex %f %f %f\n", m_vVerts[p[1]][0], 
                                          m_vVerts[p[1]][1], 
                                          m_vVerts[p[1]][2]);
    fprintf(fp, "    vertex %f %f %f\n", m_vVerts[p[2]][0], 
                                          m_vVerts[p[2]][1], 
                                          m_vVerts[p[2]][2]);
    fprintf(fp, "  endloop\n");
    fprintf(fp, "endfacet\n");
  }
  fprintf(fp, "endsolid tmesh\n");
  fclose(fp);
  return true;
}




bool TMesh::PickByRay(const EVec3f &rayP, const EVec3f &rayD, EVec3f &pos, int &pid) const
{
  float depth = FLT_MAX;
  EVec3f tmpPos;
  pid = -1;

  for (int pi = 0; pi < m_pSize; ++pi)
  {
    const int *p = m_pPolys[pi].idx;
    if (t_intersectRayToTriangle(rayP, rayD, m_vVerts[p[0]], m_vVerts[p[1]], m_vVerts[p[2]], tmpPos))
    {
      float d = (tmpPos - rayP).norm();
      if (d < depth)
      {
        depth = d;
        pos = tmpPos;
        pid = pi;
      }
    }
  }
  return depth != FLT_MAX;
}


bool TMesh::PickByRay(const EVec3f &rayP, const EVec3f &rayD, EVec3f &pos) const
{
  int pid;
  return PickByRay(rayP, rayD, pos, pid);
}


//extract Verts and Polys connected to vertex[vi]
void TMesh::ExtractLinkedVertsPolys( 
  const int vid,
  std::vector<int> &vert_ids, 
  std::vector<int> &poly_ids) const
{
  if( vid < 0 || m_vSize - 1 < vid ) return;

  //flg 0:not visited, 1:visited
  byte *flg = new byte[ m_vSize ]; 
  memset( flg, 0, sizeof( byte ) * m_vSize );

  TQueue< int > Q( m_vSize );
  Q.push_back( vid );
  flg[ vid ] = 1;

  while( !Q.empty() )
  {
    const std::vector<int> &ring = m_vRingVs[Q.front()];
    Q.pop_front();
    for ( auto it : ring )
    {
      if ( flg[it] == 0 ) 
      {
        flg[it] = 1;
        Q.push_back(it);
      }
    }
  }

  //verts
  vert_ids.clear();
  vert_ids.reserve(m_vSize);
  for ( int i = 0; i < m_vSize; ++i )
    if ( flg[i] ) 
      vert_ids.push_back( i );
      
  //polys 
  poly_ids.clear();
  poly_ids.reserve(m_pSize);
  for ( int i = 0; i < m_pSize; ++i )
    if ( flg[ m_pPolys[i].idx[0]] ) 
      poly_ids.push_back( i );

  delete[] flg;
}


void TMesh::ExtractRingNeighborVerts( 
  const int vid,
  const int ringsize, 
  std::set<int> &output_ids) const
{
  if( vid < 0 || m_vSize - 1 < vid ) return;

	output_ids.clear();
	std::set<int> border_verts; 
	border_verts.insert( vid );

	for( int count = -1; count < ringsize; ++count)
	{
		std::set<int> new_border;
		for( auto piv : border_verts)
		{
			output_ids.insert( piv );
			for( auto ringidx : m_vRingVs[piv] )
			{
				if( output_ids  .find( ringidx ) != output_ids  .end() ) continue;
        if( border_verts.find( ringidx ) != border_verts.end() ) continue;
        new_border.insert( ringidx );
			}
		}
		border_verts = new_border;
	}
}


int TMesh::GetNearestVertexIdx ( const EVec3f &p ) const
{
  int idx;
  float distsq;
  t_verts_GetNearestPoint(m_vSize, m_vVerts, p, idx, distsq);
  return idx;
}


//最近傍点検索
void TMesh::GetDistToPoint(
    const EVec3f &p, 
    const int nearest_vid, 

    EVec3f &pos_onsurf, 
    double &dist) const
{
	dist = DBL_MAX;
  std::set<int> trgt_ps;
  for ( const auto& vid : m_vRingVs[nearest_vid] )
    for ( const auto& pid : m_vRingPs[vid] )
      trgt_ps.insert(pid);

  //edge
	if ( trgt_ps.size() == 0)
	{ 
    std::cout << "strange input !!!!!!! " << nearest_vid << "\n";
		Trace( m_vVerts[ nearest_vid] );
	}

  //check on edges
	for (const auto &it : m_vRingVs[nearest_vid])
	{
		const EVec3f &x0 = m_vVerts[ nearest_vid ];
		const EVec3f &x1 = m_vVerts[ it  ];
    EVec3f tmppos;
    float  tmpdist = t_DistPointAndLineSegm_sq( p, x0, x1, tmppos);
    if ( tmpdist < dist ) {
      dist = tmpdist;
      pos_onsurf = tmppos;
    }
	}
  
  //check on polyton 
	for (auto it : trgt_ps)
	{
		// d1 = x1-x0, d2 = x2-x0, h = x0 + s d1 + t d2
		// d1*(h-p) = 0
		// d2*(h-p) = 0 よりstを計算
		const EVec3f &x0 = m_vVerts[ m_pPolys[it].idx[0] ];
		EVec3f d1 = m_vVerts[ m_pPolys[it].idx[1] ] - x0;
		EVec3f d2 = m_vVerts[ m_pPolys[it].idx[2] ] - x0;
		double s,t;
		t_SolveLinearEq2d( d1.dot(d1), d1.dot(d2), 
			                          d2.dot(d1), d2.dot(d2), d1.dot(p-x0),    
			                                                  d2.dot(p-x0), s,t);
		if (0 <= s && 0 <= t && s + t <= 1)
		{
			EVec3f h = x0 + (float)s * d1 + (float)t * d2;
			double d = t_DistSq( p, h);
			if( d < dist ) {
				dist = d; 
				pos_onsurf = h;
			}
		}
	}

	if( dist == DBL_MAX )
	{
    std::cout << "never comes here error !!\n" ;
		for( auto i : trgt_ps ) 
			std::cout << i << "--" << i ;
	}
}




#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

void TMesh::DrawSphere(
    const EVec3f &position, 
    const float radius )
{
  static int polygon_size;
  static GLuint m_buffers[3] = {0,0,0};
  if ( m_buffers[0] == 0 )
  {
    const int M = 12;
    const int N = 12;
    TMesh s;
    s.InitializeAsSphere( 1.0f, M, N);
    polygon_size = s.m_pSize;
    
    //generat vertex buffer object 
    float  *vs = new float[9*s.m_pSize]; // 3*pize * 3
    float  *ns = new float[9*s.m_pSize]; // 3*pize * 3
    GLuint *id = new GLuint[3*s.m_pSize];
    for( int p = 0; p < s.m_pSize; ++p)
    {
      const int *vidx = s.m_pPolys[p].idx;
      for ( int i=0; i < 3; ++i ) vs[9*p+i+0] = s.m_vVerts[vidx[0]][i];
      for ( int i=0; i < 3; ++i ) vs[9*p+i+3] = s.m_vVerts[vidx[1]][i];
      for ( int i=0; i < 3; ++i ) vs[9*p+i+6] = s.m_vVerts[vidx[2]][i];
      for ( int i=0; i < 3; ++i ) ns[9*p+i+0] = s.m_vNorms[vidx[0]][i];
      for ( int i=0; i < 3; ++i ) ns[9*p+i+3] = s.m_vNorms[vidx[1]][i];
      for ( int i=0; i < 3; ++i ) ns[9*p+i+6] = s.m_vNorms[vidx[2]][i];
      id[p*3+0] = p*3+0;
      id[p*3+1] = p*3+1;
      id[p*3+2] = p*3+2;
    }
    //init vbo
    glGenBuffers(3, m_buffers);
    //１つ目のバッファオブジェクトに頂点データ配列を転送する 
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*s.m_pSize*9, vs, GL_STATIC_DRAW);
    //２つ目のバッファオブジェクトに法線データ配列を転送する 
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*s.m_pSize*9, ns, GL_STATIC_DRAW);
    //3つ目のバッファオブジェクトに頂点のインデックスを転送する 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*s.m_pSize*3, id, GL_STATIC_DRAW);
    std::cout << radius << " " << polygon_size << " " << m_buffers[0] << "\n";
    delete[] vs;
    delete[] ns;
    delete[] id;
  }

  //rendering 
  glEnable(GL_NORMALIZE);
  glPushMatrix();
  glTranslated(position[0], position[1], position[2]);
  glScalef(radius, radius, radius);

  //頂点データ，法線データ，テクスチャ座標の配列を有効にする 
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  
  //頂点/法線/indexデータの場所を指定する 
  glBindBuffer(GL_ARRAY_BUFFER, m_buffers[0]);
  glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
  glBindBuffer(GL_ARRAY_BUFFER, m_buffers[1]);
  glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(0));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[2]);
  glDrawElements(GL_TRIANGLES, polygon_size * 3, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

  //頂点データ，法線データ
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
 
  glPopMatrix();
  glDisable(GL_NORMALIZE);
}


void TMesh::DrawSphere
(
  const EVec3f &position, 
  const float radius,
  const float *diff, 
  const float *ambi, 
  const float *spec, 
  const float *shin) 
{
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , spec);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , diff);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , ambi);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shin);
  DrawSphere(position, radius);
}




///////////////////////////////////////////////////////////////////////////////
//Mesh filling ////////////////////////////////////////////////////////////////

void GenBinaryVolumeFromMesh_Y
(
  const EVec3i& reso,
  const EVec3f& pitch,
  const TMesh& mesh,

  byte* binVol //allocated[WxHxD], 0:out, 1:in
)
{
  const int    W = reso[0];
  const int    H = reso[1];
  const int    D = reso[2];
  const double px = pitch[0];
  const double py = pitch[1];
  const double pz = pitch[2];
  const int     vSize = mesh.m_vSize;
  const int     pSize = mesh.m_pSize;
  const EVec3f* verts = mesh.m_vVerts;
  const EVec3f* vNorm = mesh.m_vNorms;
  const TPoly* polys = mesh.m_pPolys;
  const EVec3f* pNorm = mesh.m_pNorms;

  //clock_t t0 = clock();
  const int WH = W * H, WHD = W * H * D;
  const EVec3f cuboid((float)(W * px), (float)(H * py), (float)(D * pz));

  EVec3f BBmin, BBmax;
  t_CalcBoundingBox(vSize, verts, BBmin, BBmax);

  memset(binVol, 0, sizeof(byte) * WHD);


  // insert triangles in BINs -- divide yz space into (BIN_SIZE x BIN_SIZE)	
  const int BIN_SIZE = 20;
  std::vector< std::vector<int> > polyID_Bins(BIN_SIZE * BIN_SIZE, std::vector<int>());

  for (int p = 0; p < pSize; ++p)
  {
    EVec3f bbMin, bbMax;
    t_CalcBoundingBox(verts[polys[p].idx[0]],
      verts[polys[p].idx[1]],
      verts[polys[p].idx[2]], bbMin, bbMax);

    int xS = t_crop(0, BIN_SIZE - 1, (int)(bbMin[0] / cuboid[0] * BIN_SIZE));
    int zS = t_crop(0, BIN_SIZE - 1, (int)(bbMin[2] / cuboid[2] * BIN_SIZE));
    int xE = t_crop(0, BIN_SIZE - 1, (int)(bbMax[0] / cuboid[0] * BIN_SIZE));
    int zE = t_crop(0, BIN_SIZE - 1, (int)(bbMax[2] / cuboid[2] * BIN_SIZE));
    for (int z = zS; z <= zE; ++z)
      for (int x = xS; x <= xE; ++x)
        polyID_Bins[z * BIN_SIZE + x].push_back(p);
  }

  //clock_t t1 = clock();

  // ray casting along x axis to fill inside the mesh 
#pragma omp parallel for
  for (int zI = 0; zI < D; ++zI) if (BBmin[2] <= (0.5 + zI) * pz && (0.5 + zI) * pz <= BBmax[2])
    for (int xI = 0; xI < W; ++xI) if (BBmin[0] <= (0.5 + xI) * px && (0.5 + xI) * px <= BBmax[0])
    {
      double x = (0.5 + xI) * px;
      double z = (0.5 + zI) * pz;
      int bin_xi = std::min((int)(x / cuboid[0] * BIN_SIZE), BIN_SIZE - 1);
      int bin_zi = std::min((int)(z / cuboid[2] * BIN_SIZE), BIN_SIZE - 1);
      std::vector<int>& trgtBin = polyID_Bins[bin_zi * BIN_SIZE + bin_xi];

      std::multimap<double, double> blist;// (xPos, normInXdir);

      for (const auto pi : trgtBin) if (pNorm[pi][1] != 0)
      {
        const EVec3f& V0 = verts[polys[pi].idx[0]];
        const EVec3f& V1 = verts[polys[pi].idx[1]];
        const EVec3f& V2 = verts[polys[pi].idx[2]];
        double y;
        if (t_IntersectRayYToTriangle(V0, V1, V2, x, z, y))
          blist.insert(std::make_pair(y, pNorm[pi][1])); //(y 座標, normal)
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
        for (; yI <= pivYi && yI < H; ++yI) binVol[xI + yI * W + zI * WH] = flag;
        flag = !flag;
      }
      if (flag == true) std::cout << "error double check here!";
    }

  //clock_t t2 = clock();
  //std::cout << "compute time : " << (t1-t0)/ (double) CLOCKS_PER_SEC << " " << (t2-t1)/ (double) CLOCKS_PER_SEC) << "\n";
}



/*
// for test
// cast ray in X axis
static void genBinaryVolumeInTriangleMeshX
(
  const int    W,  const int    H,  const int    D,
  const double px, const double py, const double pz,
  const int vSize,
  const int pSize,
  const EVec3f *verts,
  const EVec3f *vNorm,
  const TPoly  *polys,
  const EVec3f *pNorm,

  byte *binVol //allocated[WxHxD], 0:out, 1:in
)
{
  //clock_t t0 = clock();
  const int WH = W*H, WHD = W*H*D;
  const EVec3f cuboid((float)(W*px), (float)(H*py), (float)(D*pz));

  EVec3f BBmin, BBmax;
  t_CalcBoundBox(vSize, verts, BBmin, BBmax);

  memset(binVol, 0, sizeof(byte) * WHD);

  // insert triangles in BINs -- divide yz space into (BIN_SIZE x BIN_SIZE)
  const int BIN_SIZE = 100;
  std::vector< std::vector<int> > polyID_Bins(BIN_SIZE * BIN_SIZE, std::vector<int>());

  for (int p = 0; p < pSize; ++p)
  {
    EVec3f bbMin, bbMax;
    t_CalcBoundBox( verts[polys[p].idx[0]],
                    verts[polys[p].idx[1]],
                    verts[polys[p].idx[2]], bbMin, bbMax);
    int yS = std::min((int)(bbMin[1] / cuboid[1] * BIN_SIZE), BIN_SIZE - 1);
    int zS = std::min((int)(bbMin[2] / cuboid[2] * BIN_SIZE), BIN_SIZE - 1);
    int yE = std::min((int)(bbMax[1] / cuboid[1] * BIN_SIZE), BIN_SIZE - 1);
    int zE = std::min((int)(bbMax[2] / cuboid[2] * BIN_SIZE), BIN_SIZE - 1);
    for (int z = zS; z <= zE; ++z) for (int y = yS; y <= yE; ++y) polyID_Bins[z*BIN_SIZE + y].push_back(p);
  }

  //clock_t t1 = clock();

  // ray casting along x axis to fill inside the mesh
#pragma omp parallel for
  for (int zI = 0; zI < D; ++zI) if (BBmin[2] <= (0.5 + zI) * pz && (0.5 + zI) * pz <= BBmax[2])
    for (int yI = 0; yI < W; ++yI) if (BBmin[1] <= (0.5 + yI) * py && (0.5 + yI) * px <= BBmax[1])
    {
      double y = (0.5 + yI) * px;
      double z = (0.5 + zI) * pz;
      int bin_yi = std::min((int)(y / cuboid[1] * BIN_SIZE), BIN_SIZE - 1);
      int bin_zi = std::min((int)(z / cuboid[2] * BIN_SIZE), BIN_SIZE - 1);
      std::vector<int> &trgtBin = polyID_Bins[bin_zi * BIN_SIZE + bin_yi];

      std::multimap<double, double> blist;// (xPos, normInXdir);

      for (const auto pi : trgtBin) if (pNorm[pi][1] != 0)
      {
        const EVec3f &V0 = verts[polys[pi].idx[0]];
        const EVec3f &V1 = verts[polys[pi].idx[1]];
        const EVec3f &V2 = verts[polys[pi].idx[2]];
        double x;
        if (t_IntersectRayXToTriangle(V0, V1, V2, y, z, x)) blist.insert(std::make_pair(x, pNorm[pi][0])); //(x 座標, normal[0])
      }

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
      int xI = 0;

      //int pivIdx = xI ;
      for (auto it = blist.begin(); it != blist.end(); ++it)
      {
        int pivXi = (int)(it->first / py);
        for (; xI <= pivXi && xI < W; ++xI) binVol[xI + yI * W + zI*WH] = flag;
        flag = !flag;
      }
      if (flag == true) std::cout << "error double check here!\n";
    }

  //clock_t t2 = clock();
  //std::cout << "compute time : " << (t1-t0)/ (double) CLOCKS_PER_SEC << " " << (t2-t1)/ (double) CLOCKS_PER_SEC) << "\n";
}

*/
