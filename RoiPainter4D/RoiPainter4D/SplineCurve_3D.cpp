#pragma unmanaged

#include "SplineCurve_3D.h"
#include "gl/glew.h"
#include <gl/gl.h> 
#include <gl/glu.h> 
#include <iostream>



#define DEFAULT_SAMPLING_NUM 6


SplineCurve_3D::SplineCurve_3D(){ sampling_num_onesegment_ = DEFAULT_SAMPLING_NUM; }


SplineCurve_3D::SplineCurve_3D(const std::vector<EVec3f>& ctrlPoints)
{
	sampling_num_onesegment_ = DEFAULT_SAMPLING_NUM;
	
	for(int i = 0; i < (int)ctrlPoints.size(); ++i)
		control_points_.push_back(ctrlPoints[i]);
	
	createSpline();
}


SplineCurve_3D::SplineCurve_3D(const EVec3f* points, int pointNum)
{
	sampling_num_onesegment_ = DEFAULT_SAMPLING_NUM;
	
	for (int i = 0; i < pointNum; ++i)
		control_points_.push_back(points[i]);
	
	createSpline();
}


SplineCurve_3D::~SplineCurve_3D(){ }


void SplineCurve_3D::copyCurve(const SplineCurve_3D& curve)
{
	this->clear();

	verts_ = curve.verts_;
	control_points_ = curve.control_points_;
	sampling_num_onesegment_ = curve.sampling_num_onesegment_;
}


void SplineCurve_3D::addControlPoint(const EVec3f& point)
{
	control_points_.push_back(point);   
  createSpline();
}



void SplineCurve_3D::moveControlPoint(const EVec3f& coordinate, int pointI)
{
	if ( pointI < 0 || pointI >= (int)control_points_.size() ) return;

	control_points_[pointI] = coordinate;   createSpline();
}


void SplineCurve_3D::removeControlPoint(int pointI)
{
	if ( (int)control_points_.size() <= pointI || 0 > pointI ) return;

	control_points_.erase( control_points_.begin() + pointI );   createSpline();
}



void SplineCurve_3D::createSpline()
{
	verts_.clear();
	if (control_points_.size() < 2) return ;

	EMatXf leftSideMat = makeLeftSideMatrix();

	EVecXf solves_xyz[3];//Ax = b の解を記憶。xyzの3次元分を記憶。
	for (int xyz = 0; xyz < 3; ++xyz)
		solves_xyz[xyz] = calcSplineCoefficient(leftSideMat, makeRightSideVector(xyz));

	sampling(solves_xyz); //得られたスプライン曲線を折れ線近似
}

//スプライン曲線を折れ線近似。折れ線の頂点がすべてのcontrol_pointを通り、かつ、頂点の重複が無いようにする
void SplineCurve_3D::sampling(const EVecXf* const splineCoefficients)
{
	if (control_points_.size() < 2) return ;

	verts_.resize( (control_points_.size() - 1) * sampling_num_onesegment_ + 1 );

	for(int segmentI = 0; segmentI < (int)control_points_.size() - 1; ++segmentI)
	for (int vertI = 0; vertI < sampling_num_onesegment_; ++vertI) //セグメントごとにサンプリング
	for (int xyz = 0; xyz < 3; ++xyz) //xyz座標
	{
		float a = splineCoefficients[xyz][4 * segmentI + 0]; // 定数項
		float b = splineCoefficients[xyz][4 * segmentI + 1]; // 1次の係数
		float c = splineCoefficients[xyz][4 * segmentI + 2]; // 2次の係数
		float d = splineCoefficients[xyz][4 * segmentI + 3]; // 3次の係数


		float t = (float)vertI / sampling_num_onesegment_;
		verts_[vertI + segmentI * sampling_num_onesegment_][xyz] = a + b * t + c * t * t + d * t * t * t;
	}

	//最後の頂点だけ個別対応
	int segmentI = (int)control_points_.size() - 2;
	for (int xyz = 0; xyz < 3; ++xyz) //xyz座標
	{
		float a = splineCoefficients[xyz][4 * segmentI + 0]; // 定数項
		float b = splineCoefficients[xyz][4 * segmentI + 1]; // 1次の係数
		float c = splineCoefficients[xyz][4 * segmentI + 2]; // 2次の係数
		float d = splineCoefficients[xyz][4 * segmentI + 3]; // 3次の係数

		float t = 1.0f;
		verts_[sampling_num_onesegment_ + segmentI * sampling_num_onesegment_][xyz] = a + b * t + c * t * t + d * t * t * t;
	}
}


//Ax = bを解く
EVecXf SplineCurve_3D::calcSplineCoefficient(const EMatXf& A, const EVecXf& b) const
{
	if ( (int)control_points_.size() < 2 ) return EVecXf();

	EVecXf solve;
	try{ solve = A.fullPivLu().solve(b); }
	catch (...){ std::cout << "eigen exception\n"; throw std::exception(); }

	return solve;
}


//Ax = b のAをcontrol_points_から生成
EMatXf SplineCurve_3D::makeLeftSideMatrix() const
{
	if ( control_points_.size() < 2 ) EMatXf();

	EMatXf leftSideMat = EMatXf::Zero( (control_points_.size() - 1) * 4, (control_points_.size() - 1) * 4 );

#pragma omp parallel for
	for (int curveI = 0; curveI < (int)control_points_.size() - 2; ++curveI)
	{
		//start point
		leftSideMat( 4 * curveI, 4 * curveI ) = 1.0;

		//end point		
		for (int i = 0; i < 4; ++i) 		
			leftSideMat(4 * curveI + 1, 4 * curveI + i) =  1.0f;
		
		leftSideMat( 4 * curveI + 2, 4 * curveI + 1 ) =  1.0f;
		leftSideMat( 4 * curveI + 2, 4 * curveI + 2 ) =  2.0f;
		leftSideMat( 4 * curveI + 2, 4 * curveI + 3 ) =  3.0f;
		leftSideMat( 4 * curveI + 2, 4 * curveI + 5 ) = -1.0f;

		leftSideMat( 4 * curveI + 3, 4 * curveI + 2 ) =  2.0f;
		leftSideMat( 4 * curveI + 3, 4 * curveI + 3 ) =  6.0f;
		leftSideMat( 4 * curveI + 3, 4 * curveI + 6 ) = -2.0f;
	}

	size_t curveN = control_points_.size() - 2;
	//c_0 = 0
	leftSideMat( 4 * curveN, 2 ) = 1.0f;

	//start point
	leftSideMat( 4 * curveN + 1, 4 * curveN ) = 1.0f;

	//end point
	leftSideMat( 4 * curveN + 2, 4 * curveN ) = 1.0f;
	leftSideMat( 4 * curveN + 2, 4 * curveN + 1 ) = 1.0f;
	leftSideMat( 4 * curveN + 2, 4 * curveN + 2 ) = powf(1.0f, 2);
	leftSideMat( 4 * curveN + 2, 4 * curveN + 3 ) = powf(1.0f, 3);

	//2次微分 = 0
	leftSideMat( 4 * curveN + 3, 4 * curveN + 2 ) = 2.0f;
	leftSideMat( 4 * curveN + 3, 4 * curveN + 3 ) = 6.0f;

	return leftSideMat;
}

// Ax = b のbを1次元分のみ生成。x: 0  y: 1  z: 2 
EVecXf SplineCurve_3D::makeRightSideVector(int xyz) const
{
	if ( control_points_.size() < 2 ) return EVecXf();

	EVecXf rightSideVec = EVecXf( (control_points_.size() - 1) * 4 );

	for (int curveI = 0; curveI < (int)control_points_.size() - 2; ++curveI)
	{
		rightSideVec[4 * curveI + 0] = control_points_[curveI    ][xyz];
		rightSideVec[4 * curveI + 1] = control_points_[curveI + 1][xyz];
		
		rightSideVec[4 * curveI + 2] = 
		rightSideVec[4 * curveI + 3] = 0.0;
	}


	size_t curveN = control_points_.size() - 2;
	//c_0 = 0
	rightSideVec[4 * curveN] = 0;

	rightSideVec[4 * curveN + 1] = control_points_[curveN    ][xyz];
	rightSideVec[4 * curveN + 2] = control_points_[curveN + 1][xyz];

	rightSideVec[4 * curveN + 3] = 0;

	return rightSideVec;
}


void SplineCurve_3D::rotation(const EMat3f& rotmat)
{
	for (int pointI = 0; pointI < (int)control_points_.size(); ++pointI)
		moveControlPoint(rotmat * control_points_[pointI], pointI);
}

void SplineCurve_3D::translation(const EVec3f& translation)
{
	for (int pointI = 0; pointI < (int)control_points_.size(); ++pointI)
		moveControlPoint(control_points_[pointI] + translation, pointI);
}

void SplineCurve_3D::convertAffine(
		const std::vector<EMat3f>& mats, 
		const std::vector<EVec3f>& translations)
{
	if (mats.size() != translations.size()) return;

	for (int pointI = 0; pointI < (int)control_points_.size(); ++pointI)
	{ 
		for (int i = 0; i < (int)mats.size(); ++i)
			control_points_[pointI] = mats[i] * control_points_[pointI] + translations[i];

		moveControlPoint(control_points_[pointI], pointI);
	}
}

void SplineCurve_3D::convertHomography(const EMat4f& mat)
{
  for (size_t pointI = 0; pointI < control_points_.size(); ++pointI)
  {
    EVec4f HomogeneousCoord(control_points_[pointI][0], control_points_[pointI][1], control_points_[pointI][2], 1.0f);

    HomogeneousCoord = mat * HomogeneousCoord;

    EVec3f targetPoint( HomogeneousCoord[0] / HomogeneousCoord[3]
                      , HomogeneousCoord[1] / HomogeneousCoord[3]
                      , HomogeneousCoord[2] / HomogeneousCoord[3]);

    moveControlPoint(targetPoint, (int)pointI);
  }
}





void SplineCurve_3D::drawCurve(const EVec3f& rgb, const float lineWidth) const
{
	drawCurve(EVec4f(rgb[0], rgb[1], rgb[2], 1.0), lineWidth);
}

void SplineCurve_3D::drawCurve(const EVec4f& rgba, const float lineWidth) const
{
  if ((int)control_points_.size() < 2) return;

  glDisable(GL_LIGHTING);
  glLineWidth(lineWidth);
  glColor4f(rgba[0], rgba[1], rgba[2], rgba[3]);
  glBegin(GL_LINE_STRIP);

  for (const auto v : verts_)
    glVertex3fv(v.data());
  
	glEnd();
  glEnable(GL_LIGHTING);
}


#pragma managed

