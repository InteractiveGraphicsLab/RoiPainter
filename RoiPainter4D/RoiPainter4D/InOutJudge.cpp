#include "InOutJudge.h"
#include <numeric>


//メッシュの各三角形を単位球面上に投影し、符号付きで面積を計算
//総和が2π以上なら内、以下なら外
template <typename VertType>
bool InOutJudge
( const vector<VertType> meshVertices
, const vector<int>& faceIndices
, const VertType& queryPoint 
)
{
	vector<double> triAreas_OnUnitSphereArea((int)faceIndices.size() / 3);
#pragma omp parallel for
	for (int faceI = 0; faceI < (int)faceIndices.size(); faceI += 3)
	{
		VertType vec0_1 = meshVertices[faceIndices[faceI + 1]] - meshVertices[faceIndices[faceI]];
		VertType vec0_2 = meshVertices[faceIndices[faceI + 2]] - meshVertices[faceIndices[faceI]];

		VertType normal = vec0_1.cross(vec0_2);
		double area = normal.norm()/2;
		normal.normalize();

		VertType triGravCenter = 
												 ( meshVertices[faceIndices[faceI]]
												 + meshVertices[faceIndices[faceI + 1]]
												 + meshVertices[faceIndices[faceI + 2]]
												 ) / 3;

		triGravCenter = (triGravCenter - queryPoint);

		double r = triGravCenter.norm();
		triGravCenter.normalize();
		if (r == 0.0) triAreas_OnUnitSphereArea[faceI / 3] = 100000.0;
		else          triAreas_OnUnitSphereArea[faceI / 3] = area * triGravCenter.dot(normal) / (r*r);
	}

	return ( accumulate(triAreas_OnUnitSphereArea.begin(), triAreas_OnUnitSphereArea.end(), 0.0 )
	       ) >= 2.0*M_PI;
}



//メッシュの各三角形を単位球面上に投影し、符号付きで面積を計算
//総和が2π以上なら内、以下なら外
//多くのクエリをまとめて処理するならこっちのほうが効率的
template <typename VertType>
void InOutJudge
( const vector<VertType>  meshVertices
, const vector<int>&    faceIndices
, const vector<VertType>& queryPoints
, vector<bool>&         results
)
{
	results.clear();
	results.resize( (int)queryPoints.size() );

	vector<float> triAreas( (int)faceIndices.size() / 3 );
	vector<VertType> triGravCenters( (int)faceIndices.size() / 3 );
	vector<VertType> triNormals( (int)faceIndices.size() / 3 );

#pragma omp parallel for
	for (int faceI = 0; faceI < (int)faceIndices.size(); faceI += 3)
	{
		VertType vec0_1 = meshVertices[ faceIndices[faceI + 1] ] - meshVertices[ faceIndices[faceI] ];
		VertType vec0_2 = meshVertices[ faceIndices[faceI + 2] ] - meshVertices[ faceIndices[faceI] ];
		triNormals [ faceI / 3 ] = vec0_1.cross(vec0_2);
		triAreas[ faceI / 3 ] = triNormals[faceI / 3].norm() / 2;
    triNormals[faceI / 3].normalize();

		triGravCenters[faceI / 3] = ( meshVertices[ faceIndices[faceI  ] ] 
																+ meshVertices[ faceIndices[faceI+1] ] 
																+ meshVertices[ faceIndices[faceI+2] ]
																)/3;
	}

#pragma omp parallel for
	for (int queryI = 0; queryI < (int)queryPoints.size() ; ++queryI)
	{
		vector<double> triAreas_OnUnitSphereArea( (int)faceIndices.size() / 3 );
#pragma omp parallel for
		for (int faceI  = 0; faceI  < (int)faceIndices.size(); faceI+=3)
		{
			const VertType& queryPoint = queryPoints[queryI];

			double area = triAreas[faceI / 3];

			const VertType& cross = triNormals[ faceI / 3 ];
			VertType triGravCenter = triGravCenters[faceI / 3];
			triGravCenter = (triGravCenter - queryPoint);

			double r = triGravCenter.norm();
      triGravCenter.normalize();
			if (r == 0.0) triAreas_OnUnitSphereArea[faceI / 3] = 100000.0;
			else          triAreas_OnUnitSphereArea[faceI / 3] = area * triGravCenter.dot(cross) / (r*r);
		
		}	

		results[queryI] = ( accumulate
											( triAreas_OnUnitSphereArea.begin()
											, triAreas_OnUnitSphereArea.end()
											, 0.0
											) 
											) >= 2.0*M_PI;
	}
}