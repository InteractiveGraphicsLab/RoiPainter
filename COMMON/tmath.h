#ifndef COMMON_TMATH_H_
#define COMMON_TMATH_H_
#pragma unmanaged

//Eigen 
#include <Dense>
#include <Geometry> 

//stl
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <deque>

typedef Eigen::Vector2i EVec2i;
typedef Eigen::Vector2d EVec2d;
typedef Eigen::Vector2f EVec2f;
typedef Eigen::Vector3i EVec3i;
typedef Eigen::Vector3d EVec3d;
typedef Eigen::Vector3f EVec3f;
typedef Eigen::Vector4i EVec4i;
typedef Eigen::Vector4d EVec4d;
typedef Eigen::Vector4f EVec4f;
typedef Eigen::Matrix2d EMat2d;
typedef Eigen::Matrix3f EMat3f;
typedef Eigen::Matrix3d EMat3d;
typedef Eigen::Matrix4f EMat4f;
typedef Eigen::Matrix4d EMat4d;
typedef Eigen::VectorXf EVecXf;
typedef Eigen::MatrixXf EMatXf;

#ifndef byte
typedef unsigned char byte;
#endif

#ifndef max3
#define max3(a,b,c)      std::max((a), std::max( (b),(c) ))
#endif

#ifndef min3
#define min3(a,b,c)      std::min((a), std::min( (b),(c) ))
#endif

#ifndef max3id
#define max3id(a,b,c)    ((a>=b&&a>=c)?0:(b>=c)?1:2)
#endif

#ifndef min3id
#define min3id(a,b,c)    ((a<=b&&a<=c)?0:(b<=c)?1:2)
#endif


template<class T>
inline T t_crop(const T &minV, const T &maxV, const T &v)
{
  return std::min(maxV, std::max(minV, v));
}


enum CRSSEC_ID
{
  CRSSEC_NON,
  CRSSEC_XY,
  CRSSEC_YZ,
  CRSSEC_ZX,
  CRSSEC_CURVE
};


inline bool t_bInWindow3D(
  const EVec3f &minW, 
  const EVec3f &maxW, 
  const EVec3f &p   , float offset = 0)
{
  return
    minW[0] - offset <= p[0] && p[0] <= maxW[0] + offset &&
    minW[1] - offset <= p[1] && p[1] <= maxW[1] + offset &&
    minW[2] - offset <= p[2] && p[2] <= maxW[2] + offset;
}


inline float t_DistSq(const EVec3f& p1, const EVec3f& p2)
{
  return (p1[0] - p2[0]) * (p1[0] - p2[0]) +
         (p1[1] - p2[1]) * (p1[1] - p2[1]) +
         (p1[2] - p2[2]) * (p1[2] - p2[2]);
}


inline double t_Dist(const EVec3d &p1, const EVec3d &p2)
{
  return sqrt((p1[0] - p2[0]) * (p1[0] - p2[0]) +
              (p1[1] - p2[1]) * (p1[1] - p2[1]) +
              (p1[2] - p2[2]) * (p1[2] - p2[2]));
}


inline float t_Dist(const EVec3f &p1, const EVec3f &p2)
{
  return sqrt((p1[0] - p2[0]) * (p1[0] - p2[0]) +
              (p1[1] - p2[1]) * (p1[1] - p2[1]) +
              (p1[2] - p2[2]) * (p1[2] - p2[2]));
}


//	  | a b | |s|    w1
//    | c d | |t|  = w2
inline bool t_SolveLinearEq2d(
    const double a,  const double b,
    const double c,  const double d, 
    const double w1, const double w2,
    double &s, double &t)
{
  double det = (a*d - b*c);
  if (det == 0) return false;
  det = 1.0 / det;
  s = (d*w1 - b*w2) * det;
  t = (-c*w1 + a*w2) * det;
  return true;
}


inline bool t_SolveLinearEq2f(
  const float a,  const float b,
  const float c,  const float d, 
  const float w1, const float w2,
  float &s, float &t)
{
  float det = (a*d - b*c);
  if (det == 0) return false;
  det = 1.0f / det;
  s = (d*w1 - b*w2) * det;
  t = (-c*w1 + a*w2) * det;
  return true;
}


inline void Trace(const EVec3d &v)
{
  std::cout << v[0] << " " << v[1] << " " << v[2] << "\n";
}

inline void Trace(const EVec3f &v)
{
  std::cout << v[0] << " " << v[1] << " " << v[2] << "\n";
}

inline void Trace(const EVec3i &v)
{
  std::cout << v[0] << " " << v[1] << " " << v[2] << "\n";
}

inline void Trace(const EVec2d &v)
{
  std::cout << v[0] << " " << v[1] << "\n";
}

inline void Trace(const EMat4d &m)
{
  for (int i = 0; i < 4; i++) 
    std::cout << m(i,0) << " " << m(i,1) << " " 
              << m(i,2) << " " << m(i,3) << "\n";
}

inline void Trace(const EMat3d &m)
{
  for (int i = 0; i < 3; i++)
    std::cout << m(i,0) << " " << m(i,1) << " " << m(i,2) << "\n";
}

inline void Trace(const EMat3f &m)
{
  for (int i = 0; i < 3; i++) 
    std::cout << m(i,0) << " " << m(i,1) << " " << m(i,2) << "\n";
}

inline void Trace(const char* text, const Eigen::VectorXd &x)
{
  std::cout << text << "(";
  for ( int i=0; i < (int)x.rows(); ++i) std::cout << x[i] << " ";
  std::cout << ")\n";
}


//distance between
//ray(ray_pos + t * ray_dir) and a point
inline float t_DistRayAndPoint(
    const EVec3f &ray_pos, 
    const EVec3f &ray_dir, 
    const EVec3f &point   ,
    float &t)
{
  t = (point - ray_pos).dot(ray_dir) / ray_dir.dot(ray_dir);
  return (ray_pos + t * ray_dir - point).norm();
}


inline float t_DistRayAndPoint(
  const EVec3f &ray_pos, 
  const EVec3f &ray_dir, 
  const EVec3f &point)
{
  float t;
  return t_DistRayAndPoint(ray_pos, ray_dir, point,t);
}



inline float t_DistPointAndLineSegm_sq(
  const EVec3f &p,
  const EVec3f &line_p0,
  const EVec3f &line_p1, 
  EVec3f &nearestpos)
{
  EVec3f dir = (line_p1 - line_p0);
  if (dir.norm() == 0) 
  {
    nearestpos = line_p0;
  } 
  else 
  {
    float t = dir.dot( p - line_p0 ) / dir.dot( line_p1 - line_p0 );
    nearestpos = (t < 0) ? line_p0 : 
                 (t > 1) ? line_p1 : 
                           line_p0 + t * dir;
  }
  return t_DistSq( p, nearestpos) ;
}


inline float t_DistPointAndLineSegm_sq(
  const EVec3f &p,
  const EVec3f &line_p0,
  const EVec3f &line_p1)
{
  EVec3f tmp;
  return t_DistPointAndLineSegm_sq( p, line_p0, line_p1, tmp);
}


inline float t_DistPointAndLineSegm(const EVec3f &p,
  const EVec3f &line_p0,
  const EVec3f &line_p1)
{
  return sqrt(t_DistPointAndLineSegm_sq(p, line_p0, line_p1));
}


// ray  (ray_pos + t * ray_dir)
// line (line_p0 - line_p1    )
// h0 = ray_pos + t0 * ray_dir
// h1 = line_p0 + t1 * (line_p1 - line_p0)
// idear : (h1-h0)*ray_dir = 0
// idear : (h1-h0)*(line_p1-line_p0) = 0
//
// return (distance, 3D point on line segment) 
inline std::tuple<float,EVec3f> t_DistRayAndLineSegm
(
  const EVec3f &ray_pos, const EVec3f ray_dir,
  const EVec3f &line_p0, const EVec3f line_p1
)
{
  const EVec3f dr = ray_dir;
  const EVec3f dl = line_p1-line_p0;

  float a = dr.dot(dr), b = -dr.dot(dl), b0 = dr.dot(line_p0 - ray_pos);
  float c = dl.dot(dr), d = -dl.dot(dl), b1 = dl.dot(line_p0 - ray_pos);

  float t0,t1;
  t_SolveLinearEq2f( a,b,c,d, b0,b1, t0,t1);

  if(      t1 <= 0 ) {
    float dist = t_DistRayAndPoint( ray_pos, ray_dir, line_p0);
    return std::forward_as_tuple( dist, line_p0 ) ;
  }
  else if( 1  <= t1) {
    float dist = t_DistRayAndPoint( ray_pos, ray_dir, line_p1);
    return std::forward_as_tuple( dist, line_p1 ) ;
  }
  else{
    float dist = (ray_pos + t0*dr - line_p0 - t1*dl).norm();
    return std::forward_as_tuple( dist, line_p0 + t1*dl ) ;
  }
}


inline EVec3f t_Mult( const EMat4d &m, const EVec3f &v)
{
  EMat3d R;
  EVec3d t;
  R(0,0)=m(0,0);  R(0,1)=m(0,1);  R(0,2)=m(0,2);  t(0,0)=m(0,3);  
  R(1,0)=m(1,0);  R(1,1)=m(1,1);  R(1,2)=m(1,2);  t(1,0)=m(1,3);  
  R(2,0)=m(2,0);  R(2,1)=m(2,1);  R(2,2)=m(2,2);  t(2,0)=m(2,3);  
  EVec3d p = R * v.cast<double>() + t;
  return p.cast<float>();
}


inline EVec3f t_MultOnlyRot( const EMat4d &m, const EVec3f &v)
{
  EMat3d R;
  R(0,0)=m(0,0);  R(0,1)=m(0,1);  R(0,2)=m(0,2);
  R(1,0)=m(1,0);  R(1,1)=m(1,1);  R(1,2)=m(1,2); 
  R(2,0)=m(2,0);  R(2,1)=m(2,1);  R(2,2)=m(2,2); 
  EVec3d p = R * v.cast<double>();
  return p.cast<float>();
}


inline bool t_intersectRayToTriangle
(
  const EVec3f &rayP,
  const EVec3f &rayD,
  const EVec3f &x0,
  const EVec3f &x1,
  const EVec3f &x2,
  EVec3f &pos
)
{
  Eigen::Matrix3f A;
  A << x1[0] - x0[0], x2[0] - x0[0], -rayD[0],
       x1[1] - x0[1], x2[1] - x0[1], -rayD[1],
       x1[2] - x0[2], x2[2] - x0[2], -rayD[2];

  EVec3f stu = A.inverse()*(rayP - x0);

  if (0 <= stu[0] && stu[0] <= 1 &&
      0 <= stu[1] && stu[1] <= 1 &&
      0 <= stu[0] + stu[1] && stu[0] + stu[1] <= 1)
  {
    pos = rayP + stu[2] * rayD;
    return true;
  }

  return false;
}


inline bool t_intersectRayToQuad
(
  const EVec3f &rayP,
  const EVec3f &rayD,

  const EVec3f &x0,
  const EVec3f &x1,
  const EVec3f &x2,
  const EVec3f &x3,

  EVec3f &pos
)
{
  if (t_intersectRayToTriangle(rayP, rayD, x0, x1, x2, pos)) return true;
  if (t_intersectRayToTriangle(rayP, rayD, x0, x2, x3, pos)) return true;
  return false;
}


// intersection h = v0 + s(v1-v0) + t(v2-v0) = (x,y,z)    
// two of x,y,z are known , v0,v1,v2 are known 
inline bool t_IntersectRayXToTriangle(
  const EVec3f& v0,
  const EVec3f& v1,
  const EVec3f& v2,
  const double y,
  const double z,
  double& x //output 
)
{
  //pre-check
  if ((y < v0[1] && y < v1[1] && y < v2[1]) ||
      (y > v0[1] && y > v1[1] && y > v2[1])) return false;
  if ((z < v0[2] && z < v1[2] && z < v2[2]) ||
      (z > v0[2] && z > v1[2] && z > v2[2])) return false;

  double s, t;
  if (!t_SolveLinearEq2d(v1[1] - v0[1], v2[1] - v0[1],
    v1[2] - v0[2], v2[2] - v0[2], y - v0[1], z - v0[2], s, t)) return false;
  if (s < 0 || t < 0 || s + t > 1) return false;

  x = (1 - s - t) * v0[0] + s * v1[0] + t * v2[0];
  return true;
}


inline bool t_IntersectRayYToTriangle(
  const EVec3f& v0,
  const EVec3f& v1,
  const EVec3f& v2,
  const double x,
  const double z,
  double& y //output 
)
{
  //pre-check
  if ((x < v0[0] && x < v1[0] && x < v2[0]) ||
      (x > v0[0] && x > v1[0] && x > v2[0])) return false;
  if ((z < v0[2] && z < v1[2] && z < v2[2]) ||
      (z > v0[2] && z > v1[2] && z > v2[2])) return false;

  double s, t;
  if (!t_SolveLinearEq2d(v1[0] - v0[0], v2[0] - v0[0],
    v1[2] - v0[2], v2[2] - v0[2], x - v0[0], z - v0[2], s, t)) return false;
  if (s < 0 || t < 0 || s + t > 1) return false;

  y = (1 - s - t) * v0[1] + s * v1[1] + t * v2[1];
  return true;
}

inline bool t_intersectRayToPlane(
    const EVec3f &ray_pos,
    const EVec3f &ray_dir,
    const EVec3f &plane_pos, 
    const EVec3f &plane_norm,
    EVec3f &pos)
{
  float d_n = ray_dir.dot(plane_norm);
  if (abs(d_n) <= 0.000001f)
  {
    pos << 0, 0, 0;
    return false;
  }

  float t = -plane_norm.dot(ray_pos - plane_pos) / d_n;
  pos = ray_pos + t * ray_dir;
  return true;
}





template<class T>
void t_getMaxMinOfArray(const int N, const T* src, T& minV, T& maxV)
{
  if (N == 0) return;

  minV = src[0];
  maxV = src[0];
  for (int i = 0; i < N; ++i)
  {
    if (src[i] < minV) minV = src[i];
    if (src[i] > maxV) maxV = src[i];
  }

}


template<class T>
inline T t_CalcCentroid(const std::vector<T> &vs)
{
  T gc(0, 0, 0);
  if (vs.size() == 0) return gc;

  for (const auto &p : vs) gc += p;
  gc /= (float)vs.size();
  return gc;
}


inline EVec3f t_CalcGravityCenter(const std::vector<EVec3f>& vs)
{
  EVec3f gc(0, 0, 0);
  if (vs.size() == 0) return gc;

  for (const auto& p : vs) gc += p;
  gc /= (float)vs.size();
  return gc;
}


inline EMat3f t_CalcCovMatrix(const std::vector<EVec3f>& vs)
{
  EVec3f c = t_CalcGravityCenter(vs);
  EMat3f A;
  A << 0, 0, 0, 0, 0, 0, 0, 0, 0;

  for (auto &p : vs)
  {
    float x = p[0] - c[0];
    float y = p[1] - c[1];
    float z = p[2] - c[2];
    A(0, 0) += x * x;  A(0, 1) += x * y;  A(0, 2) += x * z;
    A(1, 0) += y * x;  A(1, 1) += y * y;  A(1, 2) += y * z;
    A(2, 0) += z * x;  A(2, 1) += z * y;  A(2, 2) += z * z;
  }
  A /= (float)vs.size();
  return A;
}


///////////////////////////////////////////////////////////////////////
//functions for vertices std::vector<EVec3f> verts/////////////////////

//calc length of a polyline
inline float t_verts_Length(
    const std::vector<EVec3f> &verts, 
    bool bClosed = false)
{
  const int N = (int)verts.size();
  float d = 0;
  if (bClosed && N >= 2) d += t_Dist(verts.back(), verts.front());

  for (int i = 1; i < N; ++i) d += t_Dist(verts[i], verts[i - 1]);

  return d;
}


inline void t_verts_Translate(const EVec3f vec, std::vector<EVec3f> &verts)
{
  for (auto& it : verts) it += vec;
}


inline void t_verts_Rotate(
    const Eigen::AngleAxis<float> &R, 
    const EVec3f &center, 
    std::vector<EVec3f> &verts)
{
  for (auto& it : verts) it -= center;
  for (auto& it : verts) it = R * it;
  for (auto& it : verts) it += center;
}


//----------------------------------------------------------------------
// resampling a polyline into "n" section with equal interval
// This function returns n+1 points
// ---------------------------------------------------------------------
inline void t_verts_ResampleEqualInterval
(
  const int n,
  const std::vector<EVec3f> &verts,
  std::vector<EVec3f> &result
)
{
  result.clear();
  if (verts.size() < 2) { result.resize(n + 1); return; }

  const float stepD = t_verts_Length(verts) / (float)n;

  if (stepD == 0) {
    result.resize(n + 1);
    return;
  }

  result.push_back(verts[0]);
  float distance = 0;

  EVec3f vec, pivot = verts[0];

  for (int index = 1; index < (int)verts.size();)
  {
    distance += t_Dist(verts[index], pivot);

    if (distance >= stepD)//steo over
    {
      vec = pivot - verts[index];
      vec *= (distance - stepD) / vec.norm();

      pivot = verts[index] + vec;
      result.push_back(pivot);
      distance = 0;
    }
    else
    {
      pivot = verts[index];
      ++index;
    }
  }
  if (result.size() != n + 1) result.push_back(verts.back());
}


inline void t_verts_Smoothing(std::vector<EVec3f> &verts)
{
  const int N = (int)verts.size();
  if (N < 2) return;

  std::vector<EVec3f> result = verts;

  for (int i = 1; i < N - 1; ++i) result[i] = 0.5 * verts[i] + 0.25 * verts[i - 1] + 0.25 * verts[i + 1];
  verts = result;
}


inline void t_verts_Smoothing(const int times, std::vector<EVec3f> &verts)
{
  for (int i = 0; i < times; ++i) t_verts_Smoothing(verts);
}


inline void t_verts_GetNearestPoint(
    const int     vSize, 
    const EVec3f *verts, 
    const EVec3f  &pos, 
    int &idx, float &dist_sq)
{
  dist_sq = FLT_MAX;
  idx  = -1;

  for ( int i = 0; i < vSize; ++i ) 
  { 
    float d = t_DistSq( pos, verts[i] );
    if( d < dist_sq ) { 
      dist_sq = d; 
      idx = i;
    }
  }
}



inline void t_CalcBoundingBox(
  const EVec3f& v0,
  const EVec3f& v1,
  const EVec3f& v2,
  EVec3f& bbMin,
  EVec3f& bbMax)
{
  bbMin << min3(v0[0], v1[0], v2[0]),
    min3(v0[1], v1[1], v2[1]),
    min3(v0[2], v1[2], v2[2]);
  bbMax << max3(v0[0], v1[0], v2[0]),
    max3(v0[1], v1[1], v2[1]),
    max3(v0[2], v1[2], v2[2]);
}


inline void t_CalcBoundingBox(
  const int N,
  const EVec3f* verts,
  EVec3f& BBmin,
  EVec3f& BBmax)
{
  BBmin << FLT_MAX, FLT_MAX, FLT_MAX;
  BBmax << -FLT_MAX, -FLT_MAX, -FLT_MAX;

  for (int i = 0; i < N; ++i)
  {
    BBmin[0] = std::min(BBmin[0], (float)verts[i][0]);
    BBmin[1] = std::min(BBmin[1], (float)verts[i][1]);
    BBmin[2] = std::min(BBmin[2], (float)verts[i][2]);
    BBmax[0] = std::max(BBmax[0], (float)verts[i][0]);
    BBmax[1] = std::max(BBmax[1], (float)verts[i][1]);
    BBmax[2] = std::max(BBmax[2], (float)verts[i][2]);
  }
}


inline void t_CalcBoundingBox(
  const std::vector<EVec2i>& verts,
  EVec2i& BBmin,
  EVec2i& BBmax)
{
  BBmin << INT_MAX, INT_MAX;
  BBmax << INT_MIN, INT_MIN;

  for (int i = 0; i < (int)verts.size(); ++i)
  {
    BBmin[0] = std::min(BBmin[0], verts[i][0]);
    BBmin[1] = std::min(BBmin[1], verts[i][1]);
    BBmax[0] = std::max(BBmax[0], verts[i][0]);
    BBmax[1] = std::max(BBmax[1], verts[i][1]);
  }
}


inline void t_CalcBoundingBox
(
  const std::vector<EVec3f>& verts,
  EVec3f& BBmin,
  EVec3f& BBmax
)
{
  BBmin << FLT_MAX, FLT_MAX, FLT_MAX;
  BBmax << -FLT_MAX, -FLT_MAX, -FLT_MAX;

  for (const auto& v : verts) {
    BBmin[0] = std::min(BBmin[0], v[0]);
    BBmin[1] = std::min(BBmin[1], v[1]);
    BBmin[2] = std::min(BBmin[2], v[2]);
    BBmax[0] = std::max(BBmax[0], v[0]);
    BBmax[1] = std::max(BBmax[1], v[1]);
    BBmax[2] = std::max(BBmax[2], v[2]);
  }
}


//calculate the angle of two 3D vectors
//axisに対して右ねじ方向が正
inline double t_CalcAngle(
  const EVec3f& v1,
  const EVec3f& v2,
  const EVec3f& axis
)
{
  double v1_len = v1.norm();
  double v2_len = v2.norm();
  if (v1_len == 0 || v2_len == 0) return 0;
  double cosT = v1.dot(v2) / v1_len / v2_len;
  if (cosT >= 1.0) cosT = 1;
  if (cosT <= -1.0) cosT = -1;

  if (axis.dot(v1.cross(v2)) >= 0) return acos(cosT);
  else return -acos(cosT);
}


//2D座標系において左回りが正（右ねじ座標系でz(0,0,1)を軸にして右回りが正）
inline double t_CalcAngle(const EVec2d& d1, const EVec2d& d2)
{
  double l = d1.norm() * d2.norm();
  if (l == 0) return 0;

  double cosT = t_crop<double>(-1., 1., (d1.dot(d2)) / l);

  if (d1[0] * d2[1] - d1[1] * d2[0] >= 0)
    return  acos(cosT);
  else
    return -acos(cosT);
}



inline EVec3f t_projectPointToPlane(
  const EVec3f &plane_pos, 
  const EVec3f &plane_norm,
  const EVec3f &point)
{
  float n2 = plane_norm.squaredNorm();
  if (n2 == 0) return EVec3f(0, 0, 0);
  float t = plane_norm.dot(plane_pos - point) / n2;
  return point + t * plane_norm;
}


static double t_CalcTriangleArea(
  const EVec3f &x0,
  const EVec3f &x1,
  const EVec3f &x2
)
{
  EVec3f v1 = x1 - x0;
  EVec3f v2 = x2 - x0;
  double v1_len = v1.norm();
  double v2_len = v2.norm();
  if (v1_len == 0 || v2_len == 0) return 0;
  double cosT = v1.dot(v2) / v1_len / v2_len;
  return v1.norm() * v2.norm() * sqrt(1 - cosT*cosT);
}


inline void WriteToFstream(std::ofstream& fs, const EVec3f& v)
{
  fs << v[0] << " " << v[1] << " " << v[2] << "\n";
}


inline void WriteToFstream(std::ofstream& fs, const EMat4d& m)
{
  fs << m(0, 0) << " " << m(0, 1) << " " << m(0, 2) << " " << m(0, 3) << "\n";
  fs << m(1, 0) << " " << m(1, 1) << " " << m(1, 2) << " " << m(1, 3) << "\n";
  fs << m(2, 0) << " " << m(2, 1) << " " << m(2, 2) << " " << m(2, 3) << "\n";
  fs << m(3, 0) << " " << m(3, 1) << " " << m(3, 2) << " " << m(3, 3) << "\n";
}


inline void ReadFromFstream(std::ifstream& fs, EMat4d& m)
{
  fs >> m(0, 0) >> m(0, 1) >> m(0, 2) >> m(0, 3);
  fs >> m(1, 0) >> m(1, 1) >> m(1, 2) >> m(1, 3);
  fs >> m(2, 0) >> m(2, 1) >> m(2, 2) >> m(2, 3);
  fs >> m(3, 0) >> m(3, 1) >> m(3, 2) >> m(3, 3);
}


inline void ReadFromFstream(std::ifstream& fs, EVec3f& v)
{
  fs >> v[0] >> v[1] >> v[2];
}


///////////////////////////////////////////////////////////////////////
//---CONTOUR MATCHING ALGORITHM--------------------------------------//
// paper : Optimal surface reconstruction from planar contours
// implemented by Chika Tomiyama and Takashi Ijiri Oct. 2017

// get shortest path in (2*pN x (qN+1) ) graph  
// from (pivPi, 0) to (pivPi + pN, qN)
// 
// edgeCostV and edgeCostH store edge capacity
//
static double c_findShortestPathByDP(
  const int pivPi,
  const int pN,
  const int qN,
  double **edgeCostV, //2pN x (qN+1) NotChanged
  double **edgeCostH, //2pN x (qN+1) NotChanged
  double **costArray, //2pN x (qN+1)
  int    **fromArray, //2pN x (qN+1)
  std::list<std::pair<int, int>> &path)
{
  for (int p = 0; p < 2 * pN; ++p)
    for (int q = 0; q <= qN; ++q)
    {
      costArray[p][q] = DBL_MAX;
      fromArray[p][q] = -1;
    }

  path.clear();
  costArray[pivPi][0] = 0.0;
  fromArray[pivPi][0] = 0;

  for (int p = pivPi; p <= pivPi + pN; ++p)
  {
    for (int q = 0; q <= qN; ++q)
    {
      if (p == pivPi && q == 0) continue;
      double costH = (q == 0) ? DBL_MAX : costArray[p][q - 1] + edgeCostH[p][q - 1];
      double costV = (p == pivPi) ? DBL_MAX : costArray[p - 1][q] + edgeCostV[p - 1][q];
      fromArray[p][q] = (costV < costH) ? 1 : 2;
      costArray[p][q] = (costV < costH) ? costV : costH;
    }
  }

  int p = pivPi + pN;
  int q = qN;
  path.push_front(std::make_pair(p, q));

  while (true)
  {
    if (fromArray[p][q] == 1) p--;
    else q--;
    path.push_front(std::make_pair(p, q));

    if (p == pivPi && q == 0) break;
  }

  return costArray[pivPi + pN][qN];
}


enum POLY_MACH_METRIC
{
  PMM_AREA,   //triangle area
  PMM_ARCLEN  //distance between matched vertices of P and Q
};


inline void c_polylineMatching
(
  const std::vector<EVec3f>   &P,
  const std::vector<EVec3f>   &Q,
  const POLY_MACH_METRIC metric,
  std::list<std::pair<int, int>>   &matchedIds
)
{
  matchedIds.clear();
  const int pNum = (int)P.size();
  const int qNum = (int)Q.size();
  if (qNum < 3 || pNum < 3) return;

  //initialzie edgeCost
  double  **edgeCostV = new double*[2 * pNum];
  double  **edgeCostH = new double*[2 * pNum]; //-1:init, 0:leftmost, 1:from Left, 2:from top
  double  **costArray = new double*[2 * pNum];
  int     **fromArray = new    int*[2 * pNum]; //-1:init, 0:start, 1:top, 2:from left  

  for (int p = 0; p < 2 * pNum; ++p)
  {
    edgeCostV[p] = new double[qNum + 1];
    edgeCostH[p] = new double[qNum + 1];
    costArray[p] = new double[qNum + 1];
    fromArray[p] = new int[qNum + 1];

    for (int q = 0; q <= qNum; ++q)
    {
      if (metric == PMM_AREA) {
        edgeCostV[p][q] = t_CalcTriangleArea(P[p % pNum], Q[q % qNum], P[(p + 1) % pNum]);
        edgeCostH[p][q] = t_CalcTriangleArea(P[p % pNum], Q[q % qNum], Q[(q + 1) % qNum]);
      }
      else
      {
        //edgeCostV[p][q] = (Q[q % qNum] - 0.5*P[p % pNum] - 0.5*P[(p + 1) % pNum]).norm(); 
        //edgeCostH[p][q] = (P[p % pNum] - 0.5*Q[q % qNum] - 0.5*Q[(q + 1) % qNum]).norm();
        edgeCostV[p][q] = (P[p % pNum] - Q[q % qNum]).norm() + (P[(p + 1) % pNum] - Q[q % qNum]).norm();
        edgeCostH[p][q] = (P[p % pNum] - Q[q % qNum]).norm() + (P[p % pNum] - Q[(q + 1) % qNum]).norm();
      }
      costArray[p][q] = DBL_MAX;
      fromArray[p][q] = -1;
    }
  }

  double minCost = DBL_MAX;

  for (int pivPi = 0; pivPi < pNum; ++pivPi)
  {
    std::list< std::pair<int, int> > pivPiPath;
    double pivPiCost = c_findShortestPathByDP(pivPi, pNum, qNum, edgeCostV, edgeCostH, costArray, fromArray, pivPiPath);
    if (pivPiCost < minCost)
    {
      matchedIds = pivPiPath;
      minCost = pivPiCost;
    }
  }

  for (int i = 0; i < pNum * 2; i++)
  {
    delete[] edgeCostV[i];
    delete[] edgeCostH[i];
    delete[] costArray[i];
    delete[] fromArray[i];
  }
  delete[] edgeCostV;
  delete[] edgeCostH;
  delete[] costArray;
  delete[] fromArray;
}


//////////////////////////////////////////////////////////////////////////
// Distance Transform ////////////////////////////////////////////////////
// 
// grow distans transform volume (vol_dist)
// return minimum value of grown voxel
inline static float __DistTransGrowFronteer
(
    const int W, const int H, const int D,
    const EVec3f &pitch, 

    std::deque<EVec4i> &Q1, //input : current fronteer 
    std::deque<EVec4i> &Q2, //output: new fronteer

    float *vol_dist,
    byte  *vol_flg  
)
{
	const int WH = W*H, WHD = W*H*D;
	float pX   = pitch[0];
  float pY   = pitch[1];
  float pZ   = pitch[2];
	float pXY  = sqrt(pX*pX + pY*pY        );
	float pYZ  = sqrt(        pY*pY + pZ*pZ);
	float pXZ  = sqrt(pX*pX         + pZ*pZ);
	float pXYZ = sqrt(pX*pX + pY*pY + pZ*pZ);
	
	float min_dist = FLT_MAX; 

	while(!Q1.empty())
	{
		const int x = Q1.front()[0];
		const int y = Q1.front()[1];
		const int z = Q1.front()[2];
		const int I = Q1.front()[3];
		Q1.pop_front();

		int K ;
	  
    //1. vol[I]の26近傍を見て，距離が最小となるものを入れる
    //2. one ring分growさせる
		//calc Dist for voxel (x,y,z) 
    //unisotropic volumeで、seedが曲線状なのでこの実装のほうが多少正確
		K = I-1-W-WH; if(x>0  &&y>0  &&z>0	&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I  -W-WH; if(       y>0  &&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pYZ  );
		K = I+1-W-WH; if(x<W-1&&y>0  &&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I-1  -WH; if(x>0         &&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXZ  );
		K = I    -WH; if(              z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pZ   );
		K = I+1  -WH; if(x<W-1       &&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXZ  );
		K = I-1+W-WH; if(x>0  &&y<H-1&&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I  +W-WH; if(       y<H-1&&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pYZ  );
		K = I+1+W-WH; if(x<W-1&&y<H-1&&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		
		K = I-1-W   ; if(x>0  &&y>0         && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXY );
		K = I  -W   ; if(       y>0         && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pY  );
		K = I+1-W   ; if(x<W-1&&y>0         && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXY );
		K = I-1     ; if(x>0                && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pX  );
		K = I+1     ; if(x<W-1              && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pX  );
		K = I-1+W   ; if(x>0  &&y<H-1       && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXY );
		K = I  +W   ; if(       y<H-1       && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pY  );
		K = I+1+W   ; if(x<W-1&&y<H-1       && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXY );
                                                                     
		K = I-1-W+WH; if(x>0  &&y>0  &&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I  -W+WH; if(       y>0  &&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pYZ  );
		K = I+1-W+WH; if(x<W-1&&y>0  &&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I-1  +WH; if(x>0         &&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXZ  );
		K = I    +WH; if(              z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pZ   );
		K = I+1  +WH; if(x<W-1       &&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXZ  );
		K = I-1+W+WH; if(x>0  &&y<H-1&&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I  +W+WH; if(       y<H-1&&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pYZ  );
		K = I+1+W+WH; if(x<W-1&&y<H-1&&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );

		//grow fronteer to deque
		K = I-1-W-WH; if(x>0  &&y>0  &&z>0	&&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y-1, z-1, K)); vol_flg[K] = 1;}	
		K = I  -W-WH; if(       y>0  &&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y-1, z-1, K)); vol_flg[K] = 1;}	
		K = I+1-W-WH; if(x<W-1&&y>0  &&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y-1, z-1, K)); vol_flg[K] = 1;}	
		K = I-1  -WH; if(x>0         &&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y  , z-1, K)); vol_flg[K] = 1;}	
		K = I    -WH; if(              z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y  , z-1, K)); vol_flg[K] = 1;}
		K = I+1  -WH; if(x<W-1       &&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y  , z-1, K)); vol_flg[K] = 1;}
		K = I-1+W-WH; if(x>0  &&y<H-1&&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y+1, z-1, K)); vol_flg[K] = 1;}
		K = I  +W-WH; if(       y<H-1&&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y+1, z-1, K)); vol_flg[K] = 1;}
		K = I+1+W-WH; if(x<W-1&&y<H-1&&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y+1, z-1, K)); vol_flg[K] = 1;}
		
		K = I-1-W   ; if(x>0  &&y>0         &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y-1, z  , K)); vol_flg[K] = 1;}
		K = I  -W   ; if(       y>0         &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y-1, z  , K)); vol_flg[K] = 1;}
		K = I+1-W   ; if(x<W-1&&y>0         &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y-1, z  , K)); vol_flg[K] = 1;}
		K = I-1     ; if(x>0                &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y  , z  , K)); vol_flg[K] = 1;}
		K = I+1     ; if(x<W-1              &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y  , z  , K)); vol_flg[K] = 1;}
		K = I-1+W   ; if(x>0  &&y<H-1       &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y+1, z  , K)); vol_flg[K] = 1;}
		K = I  +W   ; if(       y<H-1       &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y+1, z  , K)); vol_flg[K] = 1;}
		K = I+1+W   ; if(x<W-1&&y<H-1       &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y+1, z  , K)); vol_flg[K] = 1;}

		K = I-1-W+WH; if(x>0  &&y>0  &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y-1, z+1, K)); vol_flg[K] = 1;}
		K = I  -W+WH; if(       y>0  &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y-1, z+1, K)); vol_flg[K] = 1;}
		K = I+1-W+WH; if(x<W-1&&y>0  &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y-1, z+1, K)); vol_flg[K] = 1;}
		K = I-1  +WH; if(x>0         &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y  , z+1, K)); vol_flg[K] = 1;}
		K = I    +WH; if(              z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y  , z+1, K)); vol_flg[K] = 1;}
		K = I+1  +WH; if(x<W-1       &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y  , z+1, K)); vol_flg[K] = 1;}
		K = I-1+W+WH; if(x>0  &&y<H-1&&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y+1, z+1, K)); vol_flg[K] = 1;}
		K = I  +W+WH; if(       y<H-1&&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y+1, z+1, K)); vol_flg[K] = 1;}
		K = I+1+W+WH; if(x<W-1&&y<H-1&&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y+1, z+1, K)); vol_flg[K] = 1;}

		min_dist = std::min( min_dist, vol_dist[I] );
	}
	return min_dist;
}


inline void CalcDistanceTransform(
  const EVec3f &pitch,
  const EVec3i &reso ,
  const float  max_dist, //この値以上の領域は計算しない
  const std::vector<EVec4i> &seed_voxels, //xyzI
  
  float* vol_dt //should be allocated
)
{
  const int W = reso[0];
  const int H = reso[1];
  const int D = reso[2];
	const int WHD = reso[0] * reso[1] * reso[2];
	
  //vol_flg -- 0:yet, 1:visited. 
	byte *vol_flg = new byte[WHD];

	for(int i = 0; i < WHD; ++i)
	{
		vol_dt [i] = FLT_MAX;
		vol_flg[i] = 0;
	}
	
	std::deque<EVec4i> Q1, Q2;

	for (const auto &p : seed_voxels)
	{
		Q1.push_back(p);
		vol_dt [ p[3] ] = 0.0;
		vol_flg[ p[3] ] = 1  ;
	}

	while( !Q1.empty() || !Q2.empty() )
	{
		float min_dist;
		min_dist = __DistTransGrowFronteer(W,H,D,pitch, Q1,Q2, vol_dt, vol_flg);
		if( min_dist > max_dist) break;
		min_dist = __DistTransGrowFronteer(W,H,D,pitch, Q2,Q1, vol_dt, vol_flg);			
		if( min_dist > max_dist) break;
	}

	delete[] vol_flg;
  std::cout << "...Distance Transform Done\n";
}



///////////////////////////////////////////////////////////////////////////////
// Active Set Method //////////////////////////////////////////////////////////

//step1 solve 
//
//  |Q     A_eq^T A_ls'^T|    |x     |      |c    |
//  |A_eq  0      0      |  x |lambda|   =  |b_eq |
//  |A_ls' 0      0      |    |mu    |      |b_ls'|
//
//  A_ls / b_ls / mu consists of active set
//  size of result_mu is same as b_ls(full) and 0 for non active
//
static void SolveQP_ActiveSetMethod_step1
(
  const Eigen::MatrixXd &Q     ,
  const Eigen::VectorXd &c     , 
  const Eigen::MatrixXd &A_eq  , 
  const Eigen::VectorXd &b_eq  , 
  const Eigen::MatrixXd &A_ls  , 
  const Eigen::VectorXd &b_ls  ,
  const std::vector<int>     &b_active, 
  
  Eigen::VectorXd &result_x     ,
  Eigen::VectorXd &result_lambda,
  Eigen::VectorXd &result_mu
)
{
  const int rows_Q   = (int)Q   .rows();
  const int rows_Aeq = (int)A_eq.rows();  
  const int rows_Als = (int)A_ls.rows();

  int rows_Als_active = 0;
  for ( const auto &it : b_active )
    if ( it ) ++ rows_Als_active;  

  const int matrix_size = rows_Q + rows_Aeq + rows_Als_active;

  Eigen::MatrixXd M = Eigen::MatrixXd::Zero( matrix_size, matrix_size);
  Eigen::VectorXd b = Eigen::VectorXd::Zero( matrix_size );

  for ( int i = 0; i < rows_Q; ++i )
  {
    for ( int j = 0; j < rows_Q; ++j )
    {
      M(i,j) = Q(i,j);
    }
    b[i] = c[i];
  }

  for ( int i = 0; i < rows_Aeq; ++i ) 
  {
    for ( int j = 0; j < rows_Q; ++j ) 
    {
      M( rows_Q + i, j ) = M( j, rows_Q + i ) = A_eq(i,j);
    }
    b[ rows_Q+i ] = b_eq[i];
  }

  for ( int i = 0, idx = 0; i < rows_Als; ++i ) 
  {
    if ( !b_active[i] ) continue;
    
    for ( int j = 0; j < rows_Q; ++j ) 
    {
      M( rows_Q + rows_Aeq + idx, j ) = M( j, rows_Q + rows_Aeq + idx ) = A_ls( i, j);
    }
    b[ rows_Q + rows_Aeq + idx] = b_ls[i];
    ++idx;
  }


  //solve
  Eigen::VectorXd x = M.householderQr().solve(b);
  
  //output
  result_x      = Eigen::VectorXd::Zero( rows_Q   );
  result_lambda = Eigen::VectorXd::Zero( rows_Aeq );
  result_mu     = Eigen::VectorXd::Zero( rows_Als );
  
  for ( int i = 0; i < rows_Q  ; ++i ) 
    result_x[i]      = x[i       ];

  for ( int i = 0; i < rows_Aeq; ++i ) 
    result_lambda[i] = x[i+rows_Q];

  for ( int i = 0, idx = 0; i < rows_Als; ++i ) 
  {
    result_mu[i] = (b_active[i] ) ? x[ rows_Q + rows_Aeq + idx] : 0;
    if ( b_active[i] ) ++idx;
  }

}


static void t_GetMinIdxAndValue(const Eigen::VectorXd &x, int &idx, double &value)
{
  idx = -1;
  value = DBL_MAX;
  for ( int i = 0; i < x.rows(); ++i) if ( x[i] < value)
  {
    value = x[i];
    idx = i;
  }
}


//solve
//
// argmin_{x}  1/2 <x,Qx> - cx  
// subject to 
//    A_eq   x - b_eq   =  0
//    A_less x - b_less <= 0  
// 
// x    : N - dimentional vector
// Q    : N x N
// A_ls : N_ls x N
// b_ls : N_ls - dimentional
// A_eq : N_eq x N
// b_eq : N_eq - dimebtional vector
//
// step1  solve linear system 
// step2  check all constraints (A_less x - b_less <= 0  )
//   step 3-1. (if yes)
//      if ( all mu >= 0 ) --> finish
//      else inactivate i (argmin_i mu_i)  
//   step 3-2. (if no )  
//      perform projection
//
static Eigen::VectorXd SolveQp_ActiveSetMethod
(
    const Eigen::MatrixXd &Q     ,
    const Eigen::VectorXd &c     , 
    const Eigen::MatrixXd &A_eq  , 
    const Eigen::VectorXd &b_eq  , 
    const Eigen::MatrixXd &A_ls  , 
    const Eigen::VectorXd &b_ls  ,
    const Eigen::VectorXd &x_init, bool b_vis_debug = false
)
{
  std::cout << "-------------------------------------\n";
  std::cout << "-------SolveQp_ActiveSetMethod-------\n";

  //precheck the constraint (A_less * x <= b_less)
  bool is_xinit_in_const = true;
  for ( int i = 0; i < (int)A_ls.rows(); ++i )
    if ( A_ls.row(i) * x_init - b_ls[i] > 0 ) is_xinit_in_const = false;

  if ( !is_xinit_in_const )
    std::cout << "x_init does not satisfy the constraints \n";
  
  std::vector<int> b_active( A_ls.rows(), 0);
  int iteration = 0;

  Eigen::VectorXd curr_x = x_init;

  while ( true )
  {
    if( b_vis_debug ) std::cout << "-start iteration[" << iteration << "]-\n";

    //step1 solve KKT only with active set
    Eigen::VectorXd res_x, res_lambda, res_mu;
    SolveQP_ActiveSetMethod_step1( Q, c, A_eq, b_eq, A_ls, b_ls,
                                   b_active, res_x,res_lambda, res_mu); 
    
    if( b_vis_debug ) Trace( "step1. ", res_x);

    //step2 check termination condition
    bool b_passallconsts = true;

    for ( int i = 0; i < (int)A_ls.rows() && b_passallconsts; ++i )
    {
      if ( !b_active[i] && A_ls.row(i) * res_x - b_ls[i] > 0 ) 
        b_passallconsts = false;
    }

    if ( b_passallconsts )
    {
      //step 3.1 check all mu
      double min_mu;
      int    min_mu_idx;
      t_GetMinIdxAndValue ( res_mu, min_mu_idx, min_mu);
      
      if( b_vis_debug ) Trace("step2. mu = ", res_mu);
      if( b_vis_debug ) std::cout << "(min_mu, min_mu_idx)=" 
                                  << min_mu << " " <<  min_mu_idx << "\n";

      if ( min_mu >= 0 ) 
        return res_x; //finish
      else
        b_active[min_mu_idx] = 0; //deactivate min_mu_idx - th contraint
    }
    else
    {
      //step 3.2 projection 
      // - keep the solution in A_less x <= b_less
      // - project onto the closest constraint
      int    projection_idx  = -1     ;
      double projection_dist = DBL_MAX;
 
      for ( int i = 0, s = (int)A_ls.rows(); i < s; ++i )
      {
        if ( b_active[i] ) continue;
        double ai_d = A_ls.row(i) * (res_x - curr_x);
        if ( ai_d <= 0 ) continue;

        double dist = (b_ls[i] - A_ls.row(i) * curr_x) / ai_d;
        if ( dist < projection_dist )
        {
          projection_dist = dist;
          projection_idx  = i;
        }
      }
      
      if ( projection_dist < 1.0 ) 
        b_active[projection_idx] = 1;
      else 
        projection_dist = 1.0;

      if( b_vis_debug ) std::cout << "projection dist " << projection_dist << "\n";

      res_x = curr_x + projection_dist * (res_x - curr_x);
    }
    curr_x = res_x;
    
    //stdoutput for debug
    if( b_vis_debug ) Trace( "updated solution x= ", curr_x);
    ++iteration;
    if ( iteration > A_ls.rows()*2 )
      return curr_x;
  }
}

#endif