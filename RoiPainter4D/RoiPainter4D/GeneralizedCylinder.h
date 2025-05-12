#ifndef GENERALIZED_CYLINDER_H_
#define GENERALIZED_CYLINDER_H_

#pragma unmanaged

#include "tmath.h"
#include "tmesh.h"


class OrientedBoundBox
{
public:
  //oriented bounding box
  EMat3f m_rot_mat; //world to local
	EVec3f m_gravity_center;
	EVec3f m_bb_min, m_bb_max; 

	OrientedBoundBox(){}
	OrientedBoundBox(const OrientedBoundBox& src){ Copy( src ); }
	OrientedBoundBox& operator=(const OrientedBoundBox& src){ Copy(src);  return *this;}
	
  void Copy(const OrientedBoundBox& src)
	{
		m_rot_mat = src.m_rot_mat;
		m_gravity_center = src.m_gravity_center;
		m_bb_min = src.m_bb_min; 
		m_bb_max = src.m_bb_max; 
	}

  void InitializeOBB(const EVec3f* vertices, int vertNum);
  bool IsInOBB(const EVec3f& pos) const;
  void Draw(float lineWidth, float R, float G, float B, float A) const;
};








class GeneralizedCylinder
{
  std::vector<EVec3f> m_cps          ;
  std::vector<EVec3f> m_axiscurve    ;
  std::vector<EMat3f> m_rotmat_onaxis;
  float m_radius1;
  float m_radius2;
  float m_twist_angle;
  TMesh m_mesh;
	
  //bounding box
  EVec3f m_bbmin, m_bbmax;

  //oriented bounding boxes
  std::vector<OrientedBoundBox> m_obbs;

public:
	~GeneralizedCylinder(){}
	GeneralizedCylinder( float r1, float r2, float twistangle, std::vector<EVec3f> cps );

	GeneralizedCylinder(const GeneralizedCylinder& src)
	{
		Copy(src);
	}

	GeneralizedCylinder& operator=(const GeneralizedCylinder& src)
	{
		Copy(src);
		return *this;
	}

  // control points
	void AddControlPoint   (const EVec3f& cp);
	void MoveControlPoint  (int cp_idx, const EVec3f& new_pos);
	void RemoveControlPoint(int cp_idx);

  //translation, deformation, set parameters 
  void Translate    (EVec3f trans);
  void MultMatrix   (const EMat4f& mat);
  void SetRadius1   (float radius );
  void SetRadius2   (float radius );
  void SetTwistAngle(const float theta);

public:
	void clear()
	{
    m_cps.clear();
    m_axiscurve.clear();
    m_mesh.Clear();
	}

	const std::vector<OrientedBoundBox>& getOBBs() const { return m_obbs; }
  
  void Draw(bool b_active, bool b_drawobb, float cp_radius, float trans);

private:
	void DrawRadiusDir(EVec4f color_r1, EVec4f color_r2) const;

public:
  std::vector<EVec3f> GetAxisCurve() const
  {
    return m_axiscurve;
  }

  std::vector<EVec3f> GetControlPoints() const 
  { 
    return m_cps; 
  }

  int   GetControlPointNum() const{ return (int) m_cps.size(); }
  float GetRadius1()const { return m_radius1; }
  float GetRadius2()const { return m_radius2; }
	float GetTwistAngle() const{ return m_twist_angle; }

	void  Copy(const GeneralizedCylinder& src);
  int   PickCPs(const EVec3f &ray_pos, const EVec3f &ray_dir, float cp_radius, float max_depth = FLT_MAX);

  void  FillVoxelInCylinder(const EVec3i &reso, const EVec3f &pitch, void (*progressfunc)(float), byte* flg3d);

private:
	void UpdateOrientedBoundinbBox();
  void ParameterModified(bool do_updte_axis, bool do_update_mesh);
};

#endif
