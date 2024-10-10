#ifndef SPLINE_CURVE_3D_H_
#define SPLINE_CURVE_3D_H_

#pragma unmanaged
#include "tmath.h"
#include <vector>

//0<= t <=N-1  で等間隔パラメトライズ (Nはデータの個数)
//折れ線で表現
class SplineCurve_3D
{
private:
	int sampling_num_onesegment_;
	std::vector<EVec3f> control_points_;
	std::vector<EVec3f> verts_;

public:
	SplineCurve_3D();
	SplineCurve_3D(const std::vector<EVec3f>&);
	SplineCurve_3D(const EVec3f*, int pointNum);

	SplineCurve_3D(const SplineCurve_3D& curve) { copyCurve(curve); }
	SplineCurve_3D& operator=(const SplineCurve_3D& curve) { copyCurve(curve); return *this; }
	
	void copyCurve(const SplineCurve_3D& curve);


	~SplineCurve_3D();


	void addControlPoint(const EVec3f& point);
	void moveControlPoint(const EVec3f& coordinate, int pointI);
	void removeControlPoint(int pointI);

	void rotation(const EMat3f& rotmat);
	void translation(const EVec3f& translation);
	void convertAffine(const std::vector<EMat3f>& mats, const std::vector<EVec3f>& translations);
  void convertHomography(const EMat4f& mat);

	void drawCurve(const EVec3f& rgb , const float lineWidth) const;
  void drawCurve(const EVec4f& rgba, const float lineWidth) const;

	void drawCurve(const float lineWidth) const { drawCurve( EVec3f(0,0,1), lineWidth ); }
	void drawCurve() const { drawCurve( EVec3f(0, 0, 1), 10.0f ); }

	void clear()
	{
		verts_.clear();
		control_points_.clear();
	}

	int                        getControlPointNum() const { return (int)control_points_.size(); }
  const std::vector<EVec3f>& getControlPoints () const { return control_points_; }
	int                        getCurveReso() const { return sampling_num_onesegment_; }
	const std::vector<EVec3f>& getVerts() const { return verts_; }

	void setCurveReso(int reso) 
	{
		if (reso < 2) return ;
		sampling_num_onesegment_ = reso;
		createSpline();
	}

private:
	//control_points_をつなぐ3次スプライン曲線(折れ線で表現する)を生成
	void createSpline();
	
	EMatXf makeLeftSideMatrix()const ;   //Ax = b のAを生成
	EVecXf makeRightSideVector(int xyz) const;  //Ax = b のbを生成
	EVecXf calcSplineCoefficient(const EMatXf& A, const EVecXf& b) const;//Ax = b を解くことで3次スプライン曲線の係数を計算

	//3次スプライン曲線から折れ線を形成
	void sampling(const EVecXf* const splineCoefficients);
};

#endif
