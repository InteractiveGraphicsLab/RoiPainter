#pragma unmanaged
#include "./HarmCoord3D.h"


//note : map[i,j,k] corresponds to [ m_map_pos + m_map_pitch * (i+0.5,j+0.5,k+0.5) ]
//note : (x,y,z) in 3d space corresponds to map[i,j,k] 
//       (i.j,k) = (int)( ((x,y,z) - m_map_pos) / m_map_pitch ); 

//init map from cage
void HarmCoord3D::InitMap()
{
  //calc map configuration
  EVec3f bbmin, bbmax;
  m_cage.GetBoundingBox(bbmin, bbmax);
  EVec3f bbsize = bbmax - bbmin;

  int MAX_RESO = m_LATTICE_RESOLUTION;
  m_map_pitch = max3( bbsize[0], bbsize[1], bbsize[2]) / (MAX_RESO - 2);
  m_map_pos  << bbmin[0] - m_map_pitch, 
                bbmin[1] - m_map_pitch, 
                bbmin[2] - m_map_pitch; 
  
  // resolutionでは、切り上げで解像度を決定する
  m_map_reso << (int)(bbsize[0] / m_map_pitch + 0.9999f) + 2,
                (int)(bbsize[1] / m_map_pitch + 0.9999f) + 2,
                (int)(bbsize[2] / m_map_pitch + 0.9999f) + 2 ;

  std::cout << "compute Harmonic coordinate volume configuration\n";

  Trace(m_map_reso);
  Trace(m_map_pos );
  std::cout << "pitch  =  " << m_map_pitch << "\n";

  //calc flag image (0:out,1:inside,2:boundary)
  m_map_flag = new byte[m_map_reso[0]*m_map_reso[1]*m_map_reso[2]];
  
  // cast ray in Y axis ( divide ZX plane )  
  TMesh cage = m_cage;
  cage.Translate( -m_map_pos );
  EVec3f pitch(m_map_pitch, m_map_pitch, m_map_pitch);
  cage.GenBinaryVolume(m_map_reso, pitch, m_map_flag);

  const int W = m_map_reso[0];
  const int H = m_map_reso[1];
  const int D = m_map_reso[2], WH = W * H;

  for ( int z = 1; z < D-1; ++z)
  {
    for ( int y = 1; y < H-1; ++y)
    {
      for ( int x = 1; x < W-1; ++x)
      {
        int i= x + y * W + z * WH;
        if ( m_map_flag[i] != 1 ) continue;

        if( m_map_flag[i-1 ] == 0 || m_map_flag[i+1 ] == 0 || 
            m_map_flag[i-W ] == 0 || m_map_flag[i+W ] == 0 ||
            m_map_flag[i-WH] == 0 || m_map_flag[i+WH] == 0 )
        {
          m_map_flag[i] = 2; // set as boundary 
        }
      }
    }
  }

}


// h = x0 + s(x1-x0) + t(x2-x0)
// (h-p) * (x1-x0) = 0  --> d1d1 s + d1d2 t = d1 (p-x0)
// (h-p) * (x2-x0) = 0  --> d1d2 s + d2d2 t = d2 (p-x0)    
//ただし d1 = (x1-x0)

static bool CalcBarycentricCoordinate(
  const EVec3f &p, 
  const EVec3f &x0, 
  const EVec3f &x1, 
  const EVec3f &x2, 
  float &s, float &t, float& dist)
{
  const EVec3f d1 = x1-x0;
  const EVec3f d2 = x2-x0;
  bool tf = SolveLinearEq2f( 
    d1.dot(d1), d1.dot(d2), 
    d1.dot(d2), d2.dot(d2),  d1.dot(p-x0), 
                             d2.dot(p-x0), s, t);

  if (!tf) return false;
  dist = (x0 + s * d1 + t * d2 - p).norm();
  return true;
}


static void CalcBoundBox3D(
  const EVec3f &x0, 
  const EVec3f &x1, 
  const EVec3f &x2, 
  float  offset, 
  EVec3f &bbmin, 
  EVec3f &bbmax)
{
  bbmin << min3(x0[0], x1[0], x2[0]) - offset, 
           min3(x0[1], x1[1], x2[1]) - offset, 
           min3(x0[2], x1[2], x2[2]) - offset;

  bbmax << max3(x0[0], x1[0], x2[0]) + offset, 
           max3(x0[1], x1[1], x2[1]) + offset, 
           max3(x0[2], x1[2], x2[2]) + offset;
}


static bool IsInBoundingBox(
  const EVec3f &p, 
  const EVec3f &bbmin, 
  const EVec3f &bbmax 
)
{
  return bbmin[0] <= p[0] && p[0] <= bbmax[0] && 
         bbmin[1] <= p[1] && p[1] <= bbmax[1] && 
         bbmin[2] <= p[2] && p[2] <= bbmax[2] ;
}



static void SetBoundaryCondition
(
    const int W, 
    const int H, 
    const int D,
    const float   pitch,
    const EVec3f &map_pos  ,
    const byte   *map_flg, // 0:out, 1:in, 2:boundary
    const EVec3f &x0, 
    const EVec3f &x1, 
    const EVec3f &x2,

    float *map_hc
)
{
  EVec3f bbmin, bbmax;
  CalcBoundBox3D(x0,x1,x2, pitch*2.5f, bbmin, bbmax);


  //set boundary condition value
  const int WH = W*H;

  for ( int z = 0; z < D; ++z)
  {
    for ( int y = 0; y < H; ++y)
    {
      for ( int x = 0; x < W; ++x)
      {
        int i = x + y * W + z * WH;
        if ( map_flg[i] == 1 ) continue; // consider boundary and background

        const EVec3f p( (x + 0.5f) * pitch + map_pos[0], 
                        (y + 0.5f) * pitch + map_pos[1], 
                        (z + 0.5f) * pitch + map_pos[2] );
      
        if ( !IsInBoundingBox(p, bbmin, bbmax) ) continue;

        //bacycentric coordinateを計算し三角形の中に入っていたら値 1-s-tを登録
        float s,t, dist;
        if( CalcBarycentricCoordinate(p,x0,x1,x2,s,t, dist) && 
            dist < 3 * pitch && 
            -0.01f <= s && s <= 1.01f && -0.01f <= t && t <= 1.01f && 
            s + t <= 1.00f) 
        {
          map_hc[i] = 1-s-t;
        }
      }
    }
  }

}


static void SetBoundaryCondition
(
  const int W, const int H, const int D,
  const float   pitch,
  const EVec3f &map_pos  ,
  const byte   *map_flg, // 0:out, 1:in, 2:boundary
  
  const TMesh &cage,
  const int piv_cage_vi,
  float *map_hc
)
{
  //init map_hc
  memset( map_hc, 0, sizeof(float) * W*H*D );

  //対象となる三角形を境界制約として描画
  for( int pi = 0; pi < cage.m_pSize; ++pi )
  {
    int* poly = cage.m_pPolys[pi].idx;
    EVec3f &x0 = cage.m_vVerts[poly[0]]; 
    EVec3f &x1 = cage.m_vVerts[poly[1]]; 
    EVec3f &x2 = cage.m_vVerts[poly[2]]; 

    if ( poly[0] == piv_cage_vi ) {
      SetBoundaryCondition(W,H,D, pitch, map_pos, map_flg, x0,x1,x2, map_hc);
    }else if( poly[1] == piv_cage_vi ) {
      SetBoundaryCondition(W,H,D, pitch, map_pos, map_flg, x1,x2,x0, map_hc);
    }else if( poly[2] == piv_cage_vi ) {
      SetBoundaryCondition(W,H,D, pitch, map_pos, map_flg, x2,x0,x1, map_hc);
    }else {
      continue;
    }
  }
}




static void LaplacianSmoothing(
  const int W, const int H, const int D, 
  const byte *map_flg , // 0:out, 1:in, 2:boundary
  const int NUN_ITER, 
  float *map_hc         //contains values only at boundary
)
{
  const int WH = W*H;

  float *tmp = new float[W*H*D];
  memcpy(tmp, map_hc, sizeof(float)*W*H*D);

  for ( int iter = 0; iter < NUN_ITER; ++iter)
  {
    for ( int z = 1; z < D-1; ++z)
      for ( int y = 1; y < H-1; ++y)
        for ( int x = 1; x < W-1; ++x)
        {
          int i = x + y * W + z * WH;
          if ( map_flg[i] != 1 ) continue;
          
          tmp[i] =  (map_hc[i- 1] + map_hc[i+ 1] + 
                     map_hc[i- W] + map_hc[i+ W] + 
                     map_hc[i-WH] + map_hc[i+WH]) / 6.0f;

        }
    memcpy(map_hc, tmp, sizeof(float)*W*H*D);
  }
  
  delete[] tmp;
  std::cout << "-*-";
}



static void LaplacianSmoothing_multilevel(
  const int  W, 
  const int  H, 
  const int  D, 
  const int smoothing_iter_n,
  const int max_lv          ,
  const byte *map_flg , // 0:out, 1:in, 2:boundary

  float      *map_hc    //contains values only at boundary
)
{
  std::vector<EVec3i> maps_reso;
  std::vector<byte* > maps_flag;
  std::vector<float*> maps_val ;
  maps_reso.push_back( EVec3i(W,H,D) );
  maps_flag.push_back((byte*)map_flg);
  maps_val .push_back( map_hc );

  //down sampling
  for ( int lv = 1; lv < max_lv; ++lv)
  {
    const EVec3i p_reso = maps_reso[lv-1];
    const byte  *p_flg  = maps_flag[lv-1];
    const float *p_val  = maps_val [lv-1];

    //calc resolution
    int pW = p_reso[0], pH = p_reso[1], pD = p_reso[2], pWH = pW * pH;
    int w  = (pW+1)/2 ,  h = (pH+1)/2 ,  d = (pD+1)/2,  wh  = w*h;

    //gen flg map
    byte  *flg = new byte [ w*h*d ]; 
    float *val = new float[ w*h*d ];
    byte  *sum = new byte [ w*h*d ];
    memset(flg, 0, sizeof(byte )*w*h*d);
    memset(val, 0, sizeof(float)*w*h*d);
    memset(sum, 0, sizeof(byte )*w*h*d);
    maps_reso.push_back( EVec3i(w,h,d) );
    maps_flag.push_back( flg );
    maps_val .push_back( val );

    //8個のchild cellsをまわる...
    //flag: ひとつでもboundary ならboundaryに
    //val : boundary cellの平均を入れる（in/out cellには0を入れる）

    //pre lv --> new lv
    for ( int z = 0; z < pD; ++z)
      for ( int y = 0; y < pH; ++y)
        for ( int x = 0; x < pW; ++x)
        {
          int p_idx = x + y*pW + z*pWH;
          if ( p_flg[p_idx] == 2 ) {
            int i = (x/2) + (y/2)*w + (z/2)*wh;
            flg[i]  = 2;
            sum[i] += 1;
            val[i] += p_val[p_idx];
          }
        }

    //boundary valueを平均値に
    for ( int z = 0; z < d; ++z)
      for ( int y = 0; y < h; ++y)
        for ( int x = 0; x < w; ++x)
        {
          int i= x + y * w + z * w*h;
          if ( flg[i] == 2 ) {
            val[i] /= (float) sum[i];
          }else {
            flg[i] = p_flg[ 2*x + 2*y*pW +2*z*pWH ];
          }
        }
    
    delete[] sum;
  }

  // smooting and upsampling
  for ( int lv = max_lv-1; lv >= 0 ; --lv)
  {
    EVec3i reso = maps_reso[lv];
    byte  *flg  = maps_flag[lv];
    float *val  = maps_val [lv];

    const int w = reso[0], h = reso[1], d = reso[2];

    //updampling copy value from coarse layer
    if( lv != max_lv-1)
    {
      const int pW = maps_reso[lv+1][0], pH = maps_reso[lv+1][1], pD = maps_reso[lv+1][2];
      const float *p_val = maps_val [lv+1];

      for ( int z = 0; z < d; ++z)
        for ( int y = 0; y < h; ++y)
          for ( int x = 0; x < w; ++x)
          {
            int i = x + y*w + z*w*h;
            if ( flg[i] != 1 ) continue;
            val[i] = p_val[(x/2) + (y/2)*pW + (z/2)*pW*pH];
          }
    }

    //smoothing 
    LaplacianSmoothing(reso[0],reso[1],reso[2], flg, smoothing_iter_n, val);
  }

  //creanup 
  for ( int lv = 1; lv<max_lv; ++lv) 
  {
    delete[] maps_val [lv];
    delete[] maps_flag[lv];
  }
  
}





void HarmCoord3D::CalcHarmonicCoordinateMap()
{
  const int N = m_cage.m_vSize;
  const int W = m_map_reso[0];
  const int H = m_map_reso[1];
  const int D = m_map_reso[2], WH = W * H;

  for( int ci = 0; ci < N; ++ci)
  {
    float *map_hc = new float[W*H*D];
    m_map_hc.push_back(map_hc);
    SetBoundaryCondition(W,H,D, m_map_pitch, m_map_pos, m_map_flag, m_cage, ci, map_hc);
  }

  std::cout << "calc harmonic Coodinate map\n----------" << N << "\n";

#pragma omp parallel for
  for( int ci = 0; ci < N; ++ci)
  {
    std::cout << "start LaplacianSmoothing_multilevel -- " << ci << "\n";

    LaplacianSmoothing_multilevel( 
        W,H,D, 
        m_SMOOTHING_ITER_N, 
        m_SMOOTHING_LEVEL , 
        m_map_flag, m_map_hc[ci]);

    std::cout << " -- " << ci << "DONE \n";
  }

    
  //boundaryの 2 ring外側にはすでに値をセット済み

  /*
  //後処理で総和を1にするので，外側はハーモニック関数ではないが気にしないf
  for( int ci = 0; ci < N; ++ci)
  {
    float* hc = m_map_hc[ci];

    for (int z = 0; z < D; ++z)
    for (int y = 0; y < H; ++y)
    for (int x = 0; x < W; ++x)
    {
      // m_map_flag : 0:out,1:inside,2:boundary

      int i = x + y * W + z * W * H;
      if (m_map_flag[i] != 2) continue;

      for (int zz = z - 6; zz <= z + 6; ++zz) if (0 <= zz && zz < D)
      for (int yy = y - 6; yy <= y + 6; ++yy) if (0 <= yy && yy < H)
      for (int xx = x - 6; xx <= x + 6; ++xx) if (0 <= xx && xx < W)
      {
        int ii = xx + yy * W + zz * W * H;
        if (m_map_flag[ii] == 0) 
          hc[ii] = hc[i];
      }
    }
  }
  */
}



// note : 画素の中心 (0.5)の位置に画素値があるとする
// 
// x = v[0] をはさむ2つの画素を求めるなら...
// posx =  (v[0] - m_map_pos[0]) / m_map_pitch で、画素格子上における 画素位置が得られる
// posx = posx-0.5 で下図のように，ちょうど0,1,2に画素値がある空間における位置に変換できる　
//
//              posx
//  0    1    2 |  3    4
//  +----+----+----+----+
//  pitch = 1のとき v[0] = 2.3なら  x0 = 1, x1 = 2, t = 0.8
//  pitch = 1のとき v[0] = 2.7なら  x0 = 2, x1 = 3, t = 0.2

std::vector<float> HarmCoord3D::GetHarmonicCoordinate(const EVec3f &v)
{
  const int N = m_cage.m_vSize;
  const int W = m_map_reso[0];
  const int H = m_map_reso[1];
  const int D = m_map_reso[2], WH = W * H;

  const float posx = (v[0] - m_map_pos[0]) / m_map_pitch - 0.5f;
  const float posy = (v[1] - m_map_pos[1]) / m_map_pitch - 0.5f;
  const float posz = (v[2] - m_map_pos[2]) / m_map_pitch - 0.5f;
  const int   x    = (int) posx;
  const int   y    = (int) posy;
  const int   z    = (int) posz;
  const float xt = posx - x;
  const float yt = posy - y;
  const float zt = posz - z;

  float sum = 0;
  std::vector<float> hc(N, 0);

  //tri linear補間を実施する（cageは十分大きく，はみ出す事は無い前提で実装する）

  if ( x < 0 || y < 0 || z < 0 ||  W-1 <= x || H-1 <= y || D-1 <= z ) {
    std::cout << "ERROR ERROR ERROR cage is too small\n";
    return hc;
  }

  const int map_idx = x + y * W + z * W * H;
  
  for( int i=0; i < N; ++i)
  {
    const float* val = m_map_hc[i];
    float v0 = (1-yt) * ( (1-xt) * val[map_idx     ] + xt * val[map_idx+1     ] ) +
                 yt   * ( (1-xt) * val[map_idx+W   ] + xt * val[map_idx+1+W   ] );
    float v1 = (1-yt) * ( (1-xt) * val[map_idx  +WH] + xt * val[map_idx+1  +WH] ) + 
                 yt   * ( (1-xt) * val[map_idx+W+WH] + xt * val[map_idx+1+W+WH] );

    hc[i] = (1-zt) * v0 + zt * v1;
    sum += hc[i];
  }

  //std::cout << "sum of hc value (should be 1.0)" << sum << "\n";
  return hc;

}

/*
std::vector<float> HarmCoord3D::RefineHarmonicCoordinates(const EVec3f &v, std::vector<float> hc)
{
  const int N = m_cage.m_vSize;
  EVec3f current_v(0,0,0);

  for( int i=0; i < N; ++i) current_v += hc[i] * m_cage.m_vVerts[i];

  EVec3f diff = v - current_v;

  //最も係数の大きな３個のベクトルの係数を調整してdiffをキャンセルさせる（結構アドホックな実装）．
  int   top_ids[3] = {-1,-1,-1};
  float top_vals[3] = {-1,-1,-1};

  for( int i=0; i < N; ++i)
  {
    if ( hc[i] > top_vals[0] )
    {
      top_vals[2] = top_vals[1];
      top_vals[1] = top_vals[0];
      top_vals[0] = hc[i];
      top_ids[2] = top_ids[1];
      top_ids[1] = top_ids[0];
      top_ids[0] = i;
    }
    else if ( hc[i] > top_vals[1] )
    {
      top_vals[2] = top_vals[1];
      top_vals[1] = hc[i];
      top_ids[2] = top_ids[1];
      top_ids[1] = i;
    }
    else if ( hc[i] > top_vals[2] )
    {
      top_vals[2] = hc[i];
      top_ids[2] = i;
    }
  }

  //diff = a x0 + b x1 + c x2
  EVec3f x0 = m_cage.m_vVerts[top_ids[0]];
  EVec3f x1 = m_cage.m_vVerts[top_ids[1]];
  EVec3f x2 = m_cage.m_vVerts[top_ids[2]];

  EMat3f m;
  m << x0[0], x1[0], x2[0],
       x0[1], x1[1], x2[1],
       x0[2], x1[2], x2[2];

  EVec3f coef = m.householderQr().solve(diff);

  hc[top_ids[0] ] += coef[0];
  hc[top_ids[1] ] += coef[1];
  hc[top_ids[2] ] += coef[2];
  if( coef.norm() > 1.0 ){
    cout << "\n--------------- DIFF -------------------- " << diff[0] << " " << diff[1] << " " << diff[2] << "\n";
    cout << "--------------- COEF -------------------- " << top_vals[0] << " " << top_vals[1] << " " << top_vals[2] << "\n";
    cout << "--------------- COEF -------------------- " << coef[0] << " " << coef[1] << " " << coef[2] << "\n";
  }
  return hc;
}

*/


void HarmCoord3D::RefineHarmonicCoordinate(const int vsize, const EVec3f *verts)
{
  //各頂点のharmonic coordinate (hc_i)に対して，
  //argmin ||hc_i - hc_i'  ||^2 (現在のものとなるべく近くなるように，) 
  //subject to
  //  ∑hc_i - 1     = 0 (総和が１になるように) 
  //  ∑hc_i x_i - v = 0 (計算結果がターゲットと一致するように) 
  //   hc_i >= 0         (非負)
  //
  //　二次計画法の有効制約法を利用
  // Active Set Method
  //
  //ただし誤差により Σ hc_i < 1.0 となる点について，単純に上記を計算すると，
  //全体に少しずつ値が付き，すべての頂点に少しずつ依存する座標系ができる．これは望ましくないので
  //最大となる hc_*iに対して，hc_*i += 1 - Σ hc_i とする

  //ad-hocな前処理
  for (int vtx_idx = 0; vtx_idx < (int)m_verts_hc.size(); ++vtx_idx)
  { 
    std::vector<float>& hc0 = m_verts_hc[vtx_idx];
    float sum    = 0;
    int   maxid  = 0;
    float maxval = hc0[0];
    for (int i = 0; i < (int)hc0.size(); ++i)
    {
      sum += hc0[i];
      if (maxval < hc0[i]) 
      {
        maxval = hc0[i];
        maxid  = i;
      }
    }

    if (sum < 1.0)
    {
      hc0[maxid] += 1.0f - sum;
    }
  }


  //最適化
  for ( int vtx_idx = 0; vtx_idx < (int) m_verts_hc.size(); ++vtx_idx )
  {
    const std::vector<float> &hc0 = m_verts_hc[vtx_idx];

    const int N = m_cage.m_vSize;
    Eigen::MatrixXd Q    = Eigen::MatrixXd::Zero(N, N);
    Eigen::MatrixXd A_eq = Eigen::MatrixXd::Zero(4, N);
    Eigen::MatrixXd A_ls = Eigen::MatrixXd::Zero(N, N);
    Eigen::VectorXd c    = Eigen::VectorXd::Zero(N);
    Eigen::VectorXd b_eq = Eigen::VectorXd::Zero(4);
    Eigen::VectorXd b_ls = Eigen::VectorXd::Zero(N);

    for ( int i = 0; i < N; ++i)
    {
      Q(i,i) = 2; 
      c[i] = 2 * hc0[i];
      A_eq(0,i) = 1;
      A_eq(1,i) = m_cage.m_vVerts[i][0];
      A_eq(2,i) = m_cage.m_vVerts[i][1];
      A_eq(3,i) = m_cage.m_vVerts[i][2];
      A_ls(i,i) = -1;
    }
    b_eq << 1.0, verts[vtx_idx][0], verts[vtx_idx][1], verts[vtx_idx][2];

    Eigen::VectorXd x_init = Eigen::VectorXd::Zero(N);
    for ( int i = 0; i < N; ++i) x_init[i] = std::max( 0.f, hc0[i]);

    Eigen::VectorXd new_hc = 
      SolveQpActiveSetMethod(Q,c,A_eq, b_eq,A_ls, b_ls, x_init);

    for ( int i = 0; i < N; ++i) m_verts_hc[vtx_idx][i] = (float)new_hc[i];
  }
}



HarmCoord3D::HarmCoord3D(
    const TMesh &cage, 
    const int num_verts, 
    const EVec3f *verts,
    const int LATTICE_RESOLUTION,
    const int SMOOTHING_LEVEL   ,
    const int Smoothing_ITER_N  ) : 
  m_cage(cage) , 
  m_LATTICE_RESOLUTION(LATTICE_RESOLUTION),
  m_SMOOTHING_LEVEL  (SMOOTHING_LEVEL),
  m_SMOOTHING_ITER_N (Smoothing_ITER_N)
{
  std::cout << "System computes Hamonic Coordinate\n";
  InitMap();
  CalcHarmonicCoordinateMap();

  m_verts_hc.clear();
  for ( int i = 0; i < num_verts; ++i)
    m_verts_hc.push_back( GetHarmonicCoordinate(verts[i]) );
 
  std::cout << "System refine Hamonic Coordinate\n";
  RefineHarmonicCoordinate(num_verts, verts);

  Debug_showDebugInfo(num_verts, verts);
}


HarmCoord3D::~HarmCoord3D()
{
  for ( int i = 0; i < (int) m_map_hc.size(); ++i )
    delete[] m_map_hc[i];
}


void HarmCoord3D::Debug_showDebugInfo(const int vsize, const EVec3f *verts)
{
  std::cout << "Debug_showDebugInfo\n";

  const int n_cagevtx = m_cage.m_vSize;

  for ( int i=0; i < vsize; ++i)
  {
    float sum = 0;
    EVec3f p(0,0,0);
    bool isAllSemiPositive = true;
    for ( int j = 0; j < n_cagevtx; ++j)
    {
      p += m_verts_hc[i][j] * m_cage.m_vVerts[j];
      sum += m_verts_hc[i][j];
      if ( m_verts_hc[i][j] < -0.0000001f ) isAllSemiPositive = false;
    }
    std::cout << "cagevtx[" << i << "](sum,diff,allposi)"
              << sum << " " << Dist(p, verts[i]) 
              << " posi:"   << isAllSemiPositive << "\n"; 
  }
}
