#pragma unmanaged

#include "GeneralizedCylinder.h"
#include "OglForCLI.h"
#include <algorithm>
#include "tmesh.h"


#define NUM_VERTS_AXISSEGM 5
#define NUM_VERTS_CIRCLE 8


//3次のspline curveによりcontrol pointsを補間する
//
// cp[i]-cp[i+1] の間を3次のspline segmentで補完する
//
// fi(t) = ai + bi t + ci t^2 + di t^3 ,  t \in [0,1] 
//
// ai, bi, ci, diは以下の制約から計算できる 
//
// 1.  fi(0) = ai = cp[i], 
// 2.  fi(1) = ai + bi + ci + di = cp[i+1]
//
// 次のspline segmentが存在するときは以下の2式を利用
// 3a. fi' (1) = fi+1' (0)  --> bi + 2 ci + 3 di = b i+1  
// 4a. fi''(1) = fi+1''(0)  --> 2 ci + 6 di = 2 ci+1  　　
// 最後のspline segmentでは以下の2式を利用
// 3b. f0''(0) = 0  --> 2 c0 = 0  
// 4b. fn''(1) = 0  --> 2 cn + 6 dn = 0  　　
// 
// この制約式をまとめると A x = bという線型方程式の形になる
//
void t_GenerateSplineCurve(
  const std::vector<EVec3f> &cps,
  std::vector<EVec3f> &curve
)
{
  if ( cps.size() < 2 ) 
  {
    curve = cps;
    return;
  }
  
  //generate linear equation Ax = b
  const int num_cps = (int) cps.size();

	EMatXf mat = EMatXf::Zero( (num_cps -1) * 4, (num_cps -1) * 4 );
	EVecXf bx  = EVecXf::Zero( (num_cps- 1) * 4 );
	EVecXf by  = EVecXf::Zero( (num_cps- 1) * 4 );
	EVecXf bz  = EVecXf::Zero( (num_cps- 1) * 4 );

	for (int i = 0; i < num_cps - 1; ++i)
	{
    const int i4 = i * 4;
    
    //fi(0) = pi and fi(1) = pi+1
		mat( i4, i4) = 1.0f;
		mat( i4+1, i4  ) = 1.0f;
    mat( i4+1, i4+1) = 1.0f;
    mat( i4+1, i4+2) = 1.0f;
    mat( i4+1, i4+3) = 1.0f;
    bx [ i4 ] = cps[i][0];
    by [ i4 ] = cps[i][1];
    bz [ i4 ] = cps[i][2];
    bx [ i4+1] = cps[i+1][0];
    by [ i4+1] = cps[i+1][1];
    bz [ i4+1] = cps[i+1][2];
		
    if( i < num_cps - 2 )
    {
      //fi'(1) = fi+1'(0) and fi''(1) = fi+1''(0)
		  mat( i4+2, i4+1 ) =  1.0f;
		  mat( i4+2, i4+2 ) =  2.0f;
		  mat( i4+2, i4+3 ) =  3.0f;
		  mat( i4+2, i4+5 ) = -1.0f;
		  mat( i4+3, i4+2 ) =  2.0f;
		  mat( i4+3, i4+3 ) =  6.0f;
		  mat( i4+3, i4+6 ) = -2.0f;
    }
    else
    {
      //f0''(0) = 0 and fn-1''(1) = 0
	    mat( i4+2, 2    ) = 1.0f;
	    mat( i4+3, i4+2 ) = 2.0f;
	    mat( i4+3, i4+3 ) = 6.0f;
    }
    bx[ i4+2 ] = by[ i4+2 ] = bz[ i4+2 ] = 0;
    bx[ i4+3 ] = by[ i4+3 ] = bz[ i4+3 ] = 0;
	}

	EVecXf coefX, coefY, coefZ;
	try{ 
    Eigen::FullPivLU<EMatXf> lu( mat );
    coefX = lu.solve(bx); 
    coefY = lu.solve(by); 
    coefZ = lu.solve(bz); 
  }
	catch (...){ 
    std::cout << "eigen exception\n"; 
    curve = cps;
    return;
  }
  

  //resampling !!
	curve.resize( (num_cps - 1) * NUM_VERTS_AXISSEGM + 1 );

	for ( int segm_i = 0; segm_i < num_cps - 1; ++segm_i)
  { 
    int i4 = segm_i * 4;
    float ax = coefX[i4+0], ay = coefY[i4+0], az = coefZ[i4+0];
    float bx = coefX[i4+1], by = coefY[i4+1], bz = coefZ[i4+1];
    float cx = coefX[i4+2], cy = coefY[i4+2], cz = coefZ[i4+2];
    float dx = coefX[i4+3], dy = coefY[i4+3], dz = coefZ[i4+3];

	  for ( int vert_i = 0; vert_i < NUM_VERTS_AXISSEGM; ++vert_i) 
    {
		  float t = (float)vert_i / NUM_VERTS_AXISSEGM;
      float t2 = t*t, t3 = t*t*t;
      
      const int idx = vert_i + segm_i * NUM_VERTS_AXISSEGM ;
      curve[ idx ] = EVec3f( ax + bx * t + cx * t2 + dx * t3,
                             ay + by * t + cy * t2 + dy * t3, 
                             az + bz * t + cz * t2 + dz * t3); 
    }
  }
  
	//最後の頂点だけ個別対応
  curve.back() = cps.back();

}


void t_GenerateCurvedCylinder(
    const std::vector<EVec3f> &axis,
    const float radius1,
    const float radius2,
    const float twist_angle,
    const int NUM_CIRCLE_VERTS,
    
    std::vector<EVec3f> &verts,
    std::vector<TPoly > &polys,
    std::vector<EMat3f> &rotmat_on_axis
)
{
  if( axis.size() < 2 ) {
    verts.clear();
    polys.clear();
    rotmat_on_axis.clear();
    return;
  }

	int num_axis_verts = (int)axis.size();

  polys.clear();
	verts.resize( num_axis_verts * NUM_CIRCLE_VERTS );
  rotmat_on_axis.resize( num_axis_verts );

  EVec3f pre_tangent_dir( 0, 0, 1);
  EMat3f pre_rot_mat = Eigen::AngleAxisf( twist_angle, EVec3f(0,0,1) ).matrix();

	for ( int axis_idx  = 0; axis_idx < num_axis_verts; ++axis_idx)
  {
    //calc rotation matrix (pre_tangent_dir --> tangent_dir)
		EVec3f tangent_dir(0,0,0);
    if( axis_idx > 0                  ) tangent_dir += axis[ axis_idx   ] - axis[ axis_idx -1];
    if( axis_idx < num_axis_verts - 1 ) tangent_dir += axis[ axis_idx+1 ] - axis[ axis_idx   ];
		tangent_dir.normalize();
    
	  float dotprod = std::min( 1.0f, std::max(-1.0f, tangent_dir.dot( pre_tangent_dir ) ) );

	  EVec3f rotaxis = pre_tangent_dir.cross( tangent_dir );
	  if (rotaxis.norm() == 0.0) rotaxis = EVec3f(0.0f, 1.0f, 0.0f); 
	  rotaxis.normalize();

	  EMat3f rot_mat = Eigen::AngleAxisf( acos( dotprod ), rotaxis).matrix();
    EMat3f m = rot_mat * pre_rot_mat;
   
    for ( int ci = 0; ci < NUM_CIRCLE_VERTS; ++ci )
    {
      const float theta = float(2 * M_PI * ci / NUM_CIRCLE_VERTS);
      EVec3f p( radius1*cosf(theta), radius2*sinf(theta), 0.0f);
      verts[ ci + axis_idx * NUM_CIRCLE_VERTS ] = m * p + axis[ axis_idx ];

      if( axis_idx != num_axis_verts - 1) 
      {
        int i0 = (axis_idx  ) * NUM_CIRCLE_VERTS + ci;
        int i1 = (axis_idx  ) * NUM_CIRCLE_VERTS + (ci+1)%NUM_CIRCLE_VERTS;
        int i2 = (axis_idx+1) * NUM_CIRCLE_VERTS + ci;
        int i3 = (axis_idx+1) * NUM_CIRCLE_VERTS + (ci+1)%NUM_CIRCLE_VERTS;
        polys.push_back(TPoly(i0,i1,i3));
        polys.push_back(TPoly(i0,i3,i2));
      }
    }

    rotmat_on_axis[axis_idx] = m;
    pre_rot_mat = m;
    pre_tangent_dir = tangent_dir;
  }

  for (int ci = 0; ci < NUM_CIRCLE_VERTS - 2; ++ci)
	{
		int i0 = 0;
		int i1 = (ci + 2) % NUM_CIRCLE_VERTS;
		int i2 = (ci + 1) % NUM_CIRCLE_VERTS;
    polys.push_back( TPoly(i0,i1,i2) );

		const int piv_idx = NUM_CIRCLE_VERTS * (num_axis_verts - 1) ;

		int i3 = piv_idx;
		int i4 = piv_idx + (ci + 1) % NUM_CIRCLE_VERTS;
		int i5 = piv_idx + (ci + 2) % NUM_CIRCLE_VERTS;
    polys.push_back( TPoly(i3,i4,i5) );
	}

}







void OrientedBoundBox::InitializeOBB(const EVec3f* vertices, int vertNum)
{
  //calc gravity center
  m_gravity_center = EVec3f::Zero();
	for (int i = 0; i < vertNum; ++i) m_gravity_center += vertices[i];
	m_gravity_center /= (float)vertNum;

  //calc rotation (orientation) by PCA
	EMatXf data_mat(3, vertNum);
	for (int i = 0; i < vertNum; ++i)
		data_mat.col(i) = vertices[i] - m_gravity_center;

	EMat3f cov = data_mat * data_mat.transpose() / (float)vertNum;

	Eigen::SelfAdjointEigenSolver<EMatXf> es( cov );
	if (es.info() != Eigen::Success)
	{
		m_rot_mat = Eigen::Matrix3f::Identity();
		m_bb_max  = m_bb_min = EVec3f(0,0,0);
		return ;
	}

	m_rot_mat = es.eigenvectors().transpose();

	m_bb_max = EVec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	m_bb_min = EVec3f( FLT_MAX,  FLT_MAX,  FLT_MAX);
	for (int i = 0; i < vertNum; ++i)
	{
		EVec3f p = m_rot_mat * (vertices[i] - m_gravity_center) + m_gravity_center;

		if (p[0] < m_bb_min[0]) m_bb_min[0] = p[0];
		if (p[1] < m_bb_min[1]) m_bb_min[1] = p[1];
		if (p[2] < m_bb_min[2]) m_bb_min[2] = p[2];
		if (p[0] > m_bb_max[0]) m_bb_max[0] = p[0];
		if (p[1] > m_bb_max[1]) m_bb_max[1] = p[1];
		if (p[2] > m_bb_max[2]) m_bb_max[2] = p[2];
	}

}



//pos is in world coordinate
bool OrientedBoundBox::IsInOBB(const EVec3f& pos) const
{
	EVec3f pos_local = m_rot_mat * ( pos - m_gravity_center) + m_gravity_center;

  return pos_local[0] >= m_bb_min[0] && pos_local[0] <= m_bb_max[0] && 
         pos_local[1] >= m_bb_min[1] && pos_local[1] <= m_bb_max[1] && 
         pos_local[2] >= m_bb_min[2] && pos_local[2] <= m_bb_max[2] ;
}




void OrientedBoundBox::Draw(float line_width, float r, float g, float b, float a) const
{
	glDisable(GL_LIGHTING);

	std::vector<EVec3f> verts;
	verts.emplace_back(m_bb_min[0], m_bb_max[1], m_bb_min[2]);
	verts.emplace_back(m_bb_max[0], m_bb_max[1], m_bb_min[2]);
	verts.emplace_back(m_bb_max[0], m_bb_max[1], m_bb_max[2]);
	verts.emplace_back(m_bb_min[0], m_bb_max[1], m_bb_max[2]);
	verts.emplace_back(m_bb_min[0], m_bb_min[1], m_bb_min[2]);
	verts.emplace_back(m_bb_max[0], m_bb_min[1], m_bb_min[2]);
	verts.emplace_back(m_bb_max[0], m_bb_min[1], m_bb_max[2]);
	verts.emplace_back(m_bb_min[0], m_bb_min[1], m_bb_max[2]);
  
  for (unsigned int i = 0; i < 8; ++i)
		verts[i] = m_rot_mat.transpose()*(verts[i] - m_gravity_center) + m_gravity_center;

	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < 4; ++i)
	{
		indices.push_back( i             );
		indices.push_back((i + 1) % 4    );
		indices.push_back( i          + 4);
    indices.push_back((i + 1) % 4 + 4);
    indices.push_back(i);
    indices.push_back(i + 4);
	}

	glLineWidth( line_width );
	glEnable(GL_LINE_SMOOTH);
	glColor4f(r,g,b,a);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, verts.data());
	glDrawElements(GL_LINES, (int)indices.size(), GL_UNSIGNED_INT, indices.data());
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_LINE_SMOOTH);
	glEnable(GL_LIGHTING);
}










void GeneralizedCylinder::AddControlPoint   (const EVec3f& cp)
{
  m_cps.push_back( cp );
  ParameterModified(true, true);
}

void GeneralizedCylinder::MoveControlPoint  (int cp_idx, const EVec3f& new_pos)
{
  if( cp_idx < 0 || m_cps.size() <= cp_idx) return;
  m_cps[ cp_idx ] = new_pos;
  ParameterModified(true, true);
}

void GeneralizedCylinder::RemoveControlPoint(int cp_idx)
{
  if( cp_idx < 0 || m_cps.size() <= cp_idx) return;
  m_cps.erase( m_cps.begin() + cp_idx );
  ParameterModified(true, true);
}

void GeneralizedCylinder::Translate( EVec3f trans)
{
  for( int i=0; i < m_cps.size(); ++i ) m_cps[i] += trans;
  for( int i=0; i < m_axiscurve.size(); ++i ) m_axiscurve[i] += trans;
  ParameterModified(false, true);

}

void GeneralizedCylinder::MultMatrix(const EMat4f& mat)
{
  for( int i=0; i < m_cps.size(); ++i ) {
    EVec4f p( m_cps[i][0], m_cps[i][1], m_cps[i][2], 1.0f);
    p = mat * p;
    m_cps[i] << p[0] / p[3], p[1] / p[3], p[2] / p[3];
  }
  ParameterModified( true, true);
}

void GeneralizedCylinder::SetTwistAngle(float twist_angle)
{
  m_twist_angle = twist_angle;
  ParameterModified( false, true);

}

void GeneralizedCylinder::SetRadius1( float radius)
{
  if (radius <= 0.0001f) return ;
	m_radius1 = radius;
  ParameterModified( false, true);
}

void GeneralizedCylinder::SetRadius2(float radius)
{
  if (radius <= 0.0001f) return;
  m_radius2 = radius;
  ParameterModified( false, true);
}

void GeneralizedCylinder::ParameterModified(bool do_update_axis, bool do_update_mesh)
{
  if( do_update_axis )
  {
    t_GenerateSplineCurve( m_cps, m_axiscurve );
  }

  if( do_update_mesh )
  {
    std::vector<EVec3f> verts;
    std::vector<TPoly > polys;
    t_GenerateCurvedCylinder(
        m_axiscurve, m_radius1, m_radius2, m_twist_angle, NUM_VERTS_CIRCLE, 
        verts, polys, m_rotmat_onaxis );
    m_mesh.Initialize( verts, polys );
  }

  m_mesh.GetBoundBox(m_bbmin, m_bbmax);
	UpdateOrientedBoundinbBox();
}



void GeneralizedCylinder::DrawAxisCurve( const EVec3f& color, float lineWidth) const
{
  t_DrawPolyLine( color, lineWidth, m_axiscurve);
}




GeneralizedCylinder::GeneralizedCylinder()
{
  m_cps.clear();
  m_axiscurve.clear();
  m_twist_angle = 0.0f;
  
	m_radius1 = 10.0f;
	m_radius2 = 10.0f;
}

GeneralizedCylinder::GeneralizedCylinder( 
    float r1, 
    float r2, 
    float twistangle, 
    std::vector<EVec3f>& cps )
{
	m_radius1 = r1;
	m_radius2 = r2;
  m_cps = cps;
  m_twist_angle = 0.0f;

  ParameterModified(true, true);
}



void GeneralizedCylinder::Copy(const GeneralizedCylinder& src)
{
	std::cout << "copy cylinder" << std::endl;
  
  m_cps           = src.m_cps;
  m_axiscurve     = src.m_axiscurve;
  m_rotmat_onaxis = src.m_rotmat_onaxis;
  m_twist_angle   = src.m_twist_angle;
	
  m_radius1 = src.m_radius1;
	m_radius2 = src.m_radius2;
  m_mesh    = src.m_mesh;
	m_obbs    = src.m_obbs;
  m_bbmin   = src.m_bbmin;
  m_bbmax   = src.m_bbmax;
}





void GeneralizedCylinder::UpdateOrientedBoundinbBox()
{
  m_obbs.clear();
	if ( m_cps.size() < 2) return;

	m_obbs.resize( m_cps.size() - 1);

	//区間ごとにOBBを計算
#pragma omp parallel for
	for (int cp_i = 0; cp_i < m_cps.size() - 1; ++cp_i )
	{
    //区間 cp_i - cp_i+1 の間に対応する頂点は，以下の通り．
    //各区間を NUM_RESAMPLE_AXISCURVE等分して，各点にNUM_VERTS_CRSSECCIRCLE 分の頂点がある．
    //(5, 8)なら... cp_i=0: 0-47，co_i=1: 40-87, ...
    int start_idx =  cp_i      * NUM_VERTS_AXISSEGM     * NUM_VERTS_CIRCLE;
    int end_idx   = (cp_i + 1) * (NUM_VERTS_AXISSEGM +1)* NUM_VERTS_CIRCLE;
    int num_verts = (NUM_VERTS_AXISSEGM +1)* NUM_VERTS_CIRCLE;
    m_obbs[cp_i].InitializeOBB( &m_mesh.m_vVerts[start_idx], num_verts ); 
    
    std::cout << "gen_obb... " << start_idx << " " << end_idx << " " << m_mesh.m_vSize << "\n";    
	}
}



void GeneralizedCylinder::DrawOBBs() const
{
  for (const auto& obb : m_obbs) 
    obb.Draw( 5.0f, 1,0,0,1);
}







void GeneralizedCylinder::DrawCylinder(float transparency) const
{
  static float diffCylinder[4] = { 0.0f, 0.0f, 0.5f, 0.8f };
  static float specCylinder[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float ambiCylinder[4] = { 0.1f, 0.1f, 0.2f, 0.8f };
  static float shinCylinder[1] = { 128.0f };
  diffCylinder[3] = transparency;
  specCylinder[3] = transparency;
  m_mesh.Draw(diffCylinder, ambiCylinder, specCylinder, shinCylinder);
}


void GeneralizedCylinder::DrawRadiusDir(EVec4f color_r1, EVec4f color_r2) const
{
	if ( m_axiscurve.size() < 2) return;
  EMat3f mat_front = m_rotmat_onaxis.front(); 
  EMat3f mat_tail  = m_rotmat_onaxis.back (); 

  EVec3f front_rad1_0 = mat_front * EVec3f( m_radius1, 0, 0) + m_axiscurve.front();
  EVec3f front_rad1_1 = mat_front * EVec3f(-m_radius1, 0, 0) + m_axiscurve.front();
  EVec3f front_rad2_0 = mat_front * EVec3f(0, -m_radius2, 0) + m_axiscurve.front();
  EVec3f front_rad2_1 = mat_front * EVec3f(0,  m_radius2, 0) + m_axiscurve.front();
  EVec3f tail_rad1_0  = mat_tail  * EVec3f( m_radius1, 0, 0) + m_axiscurve.back();
  EVec3f tail_rad1_1  = mat_tail  * EVec3f(-m_radius1, 0, 0) + m_axiscurve.back();
  EVec3f tail_rad2_0  = mat_tail  * EVec3f(0, -m_radius2, 0) + m_axiscurve.back();
  EVec3f tail_rad2_1  = mat_tail  * EVec3f(0,  m_radius2, 0) + m_axiscurve.back();

	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(5);
	glBegin(GL_LINES);
	glColor4fv(color_r1.data());
	  glVertex3fv(front_rad1_0.data());
	  glVertex3fv(front_rad1_1.data());
	  glVertex3fv(tail_rad1_0.data());
	  glVertex3fv(tail_rad1_1.data());

	glColor4fv(color_r2.data());
	  glVertex3fv(front_rad2_0.data());
	  glVertex3fv(front_rad2_1.data());
	  glVertex3fv(tail_rad2_0.data());
	  glVertex3fv(tail_rad2_1.data());

	glEnd();
	glEnable(GL_LIGHTING);
	glDisable(GL_LINE_SMOOTH);
}



int GeneralizedCylinder::PickCPs(const EVec3f &ray_pos, const EVec3f &ray_dir, float cp_radius, float max_depth)
{
  for (int cp_idx = 0; cp_idx < (int)m_cps.size(); ++cp_idx)
	{
		float dist  = t_DistRayAndPoint(ray_pos, ray_dir, m_cps[cp_idx]);
		float depth = (ray_pos - m_cps[cp_idx]).norm();

		if (dist < cp_radius && depth < max_depth + cp_radius)
		{ 
			return cp_idx;
		}
	}
  return -1;
}




//fill inside voxel 
//input : flg3d 0:back, 255:inside (既に255のvoxelには何もしない)
//output: flg3d 0:back, 255:inside 
void GeneralizedCylinder::FillVoxelInCylinder(
    const EVec3i &reso, 
    const EVec3f &pitch, 
    void (*progressfunc)(float), 
    byte* flg3d)
{
  //calc distance transform
  std::vector<EVec4i> dist_tf_seeds;
  
  std::vector<EVec3f> resample_curve;
  int   num_sample = int( t_verts_Length(m_axiscurve, false) / ( (pitch[0]+pitch[1]+pitch[2])/3.0 ) );
  t_verts_ResampleEqualInterval(num_sample, m_axiscurve, resample_curve);

  for ( int i=0; i < resample_curve.size(); ++i)
  {
    EVec3f &p = resample_curve[i];
    const int x = t_crop( 0, reso[0] - 1, (int)(p[0] / pitch[0]));
    const int y = t_crop( 0, reso[1] - 1, (int)(p[1] / pitch[1]));
    const int z = t_crop( 0, reso[2] - 1, (int)(p[2] / pitch[2]));
    const int I = x + y * reso[0] + z * reso[0] * reso[1];
    dist_tf_seeds.push_back(EVec4i(x,y,z,I));
  }

  const float min_r1r2 = std::min(m_radius1, m_radius2);
  const float max_r1r2 = std::max(m_radius1, m_radius2);
  float *vol_dist = new float[reso[0]*reso[1]*reso[2]];
  CalcDistanceTransform(pitch, reso, max_r1r2, dist_tf_seeds, vol_dist);
  

  //index by using bounding box
  const int xs = t_crop(0, reso[0]-1, (int)( m_bbmin[0]/pitch[0] ) );
  const int ys = t_crop(0, reso[1]-1, (int)( m_bbmin[1]/pitch[1] ) );
  const int zs = t_crop(0, reso[2]-1, (int)( m_bbmin[2]/pitch[2] ) );
  const int xe = t_crop(0, reso[0]-1, (int)( m_bbmax[0]/pitch[0] ) );
  const int ye = t_crop(0, reso[1]-1, (int)( m_bbmax[1]/pitch[1] ) );
  const int ze = t_crop(0, reso[2]-1, (int)( m_bbmax[2]/pitch[2] ) );
  const int W  = reso[0];
  const int WH = reso[0] * reso[1];
  const int num_obb = (int)m_obbs.size();

  //prepare polygon (gc/norm/verts)
  const int num_polys = m_mesh.m_pSize;
  const EVec3f *verts = m_mesh.m_vVerts;
  const TPoly  *polys = m_mesh.m_pPolys;
  const EVec3f *poly_norms = m_mesh.m_pNorms;

  float  *poly_areas   = new float [num_polys];
  EVec3f *poly_gcenter = new EVec3f[num_polys];
  
  for (int pi = 0; pi < num_polys; ++pi)
	{
    const int *idx = polys[pi].idx;
		EVec3f v01 = verts[idx[1]] - verts[idx[0]];
		EVec3f v02 = verts[idx[2]] - verts[idx[0]];
		poly_areas  [pi] = (v01.cross(v02)).norm() / 2;
		poly_gcenter[pi] = ( verts[idx[0]] + verts[idx[1]] + verts[idx[2]] ) / 3.0;
	}

  if( progressfunc != 0) progressfunc(0);
  const int total_z = ze-zs;
  int num_z = 0;

  for( int z = zs; z < ze; ++z)
  {

    //zを並列化すると progress barの更新に課題が残るので yを並列化
#pragma omp parallel for
    for( int y = ys; y < ye; ++y)
    {
      for( int x = xs; x < xe; ++x)
      {
        const int idx = x + y * W + z * WH;
        if( flg3d[idx] != 0 ) continue;
        
        if( vol_dist[idx] <= min_r1r2 ) {//前景確定
          flg3d[idx] = 255;
          continue;
        }
        if( vol_dist[idx] > max_r1r2 ) {//背景確定
          continue;
        }

        const EVec3f p( (x+0.5f)*pitch[0], (y+0.5f)*pitch[1], (z+0.5f)*pitch[2] ); 
        
        //1. coarse check obb
        bool isInObb = false;
        for( int i = 0; i < num_obb && !isInObb; ++i) 
        {
          isInObb = m_obbs[i].IsInOBB(p);
        } 
        if ( !isInObb ) continue;
        
        //2. check in/out m_mesh by 立体角 （説明は下に）
        double area = 0.0;
        for (int poly_i = 0; poly_i < num_polys; ++poly_i)
        {
			    EVec3f ofst = poly_gcenter[poly_i] - p;
			    const float r = ofst.squaredNorm();
			    if (r <= 0.00000001) { 
            //面上に乗っているときは内部判定 
            area = 1000000.0; 
            break; 
          }
			    ofst.normalize();
          area += poly_areas[ poly_i ] * ofst.dot( poly_norms[poly_i] ) / r;
		    }
        if( area > 2 * M_PI ) flg3d[idx] = 255;
      }
    }
    
    num_z++;
    if( progressfunc != 0) progressfunc( (float) num_z / total_z);
    std::cout << z << "done\n";
  }


  if( progressfunc != 0) progressfunc(0);

  delete[] poly_areas;
  delete[] poly_gcenter;
  delete[] vol_dist;
  //ある面Sの立体角とは，これを半径1の球に射影した面積で定義される．
  //ある点pが 閉じた図形Sの内部にある場合，点pを中心とする単位球にSを射影した際の立体角は4πになる (r=1の球の面積)
  //ある点pが 閉じた図形Sの外部にある場合，点pを中心とする単位球にSを射影した際の立体角は0になる
  //
  //立体角の単位はsr ステラディアン 
  //
  //半径rの球上にある図形Sの面積がsの時，この図形を半径1の球に射影すると面積は s/r^2になる
  //fill in cylinder
}