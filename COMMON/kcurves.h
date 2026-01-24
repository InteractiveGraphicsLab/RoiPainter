#ifndef COMMON_KCURVES_H_
#define COMMON_KCURVES_H_
#pragma unmanaged


//Eigen 
#include <Dense>
#include <Geometry>
#include <Sparse>

#include <Core>
#include <iostream>
#include <vector>

namespace KCurves
{
  typedef Eigen::Vector2f EVec2f;
  typedef Eigen::SparseMatrix<float> ESpMat;
  typedef Eigen::Triplet<float>      ETrip;

  static const int NUMBER_ITERATION = 15;


	inline void Trace(EVec2f& p)
	{
		std::cout << p[0] << "" << p[1] << std::endl;
	}


	/*
	//実数解を一つ返す
	//solve x^3 + a x^2 + b x + c
	inline float SolveCubicFunction
	(
		const float a,
		const float b,
		const float c
	)
	{
		const float p = b - a * a / 3.0f;
		const float q = 2 * a * a * a / 27.0f - a * b / 3.0f + c;

		const float tmp = q * q / 4.0f + p * p * p / 27.0f;
		if (tmp < 0) return 0;

		const float mTmp = -q / 2.0f + sqrt(tmp);
		const float nTmp = -q / 2.0f - sqrt(tmp);
		const float m = (mTmp < 0) ? -pow(-mTmp, 1 / 3.0f) : pow(mTmp, 1 / 3.0f);
		const float n = (nTmp < 0) ? -pow(-nTmp, 1 / 3.0f) : pow(nTmp, 1 / 3.0f);

		return -a / 3.0f + m + n;
	}
	*/	


	inline float SolveCubic01(float a, float b, float c)
	{
		auto f = [&](float x)
			{
				return x * x * x + a * x * x + b * x + c;
			};

		// Appendix A により符号反転は保証されている
		float left = 0.0f, f_left = f(left);
		float right = 1.0f, f_right = f(right);
		if (f_left * f_right >= 0.0f) return 0.5;

		for (int i = 0; i < 20; ++i)
		{
			float mid = 0.5f * (left + right);
			float f_mid = f(mid);

			if (f_left * f_mid <= 0)
			{
				right = mid;
				f_right = f_mid;
			}
			else
			{
				left = mid;
				f_left = f_mid;
			}
		}
		return 0.5f * (left + right);
	}



	inline float TriangleArea(
		const EVec2f& p0,
		const EVec2f& p1,
		const EVec2f& p2)
	{
		EVec2f v1 = p1 - p0;
		EVec2f v2 = p2 - p0;
		const float cross = v1[0] * v2[1] - v1[1] * v2[0];
		return 0.5f * std::abs(cross);
	}


	// input : cps        2D control points,        size:N)
	// output: out_cps    2D Bezier control points, size:3 * N, each 3 construct quad bezier)
	// output: out_points 2D curve
	//
	// note
	// 各制御点 cps[i] に対して、2次ベジエ制御点 Ci0, Ci1, Ci2を生成する
	//
	// ただし，
	// C_i1 は上記の C1[i]に対応
	// C_i0, C_i+1,0 については、『C_i0 = C_i+1,0 = (1-λi) Ci + λi Ci+1 』と計算できるのでλiとC1[i]のみ保持する
	//
	//https://qiita.com/Rijicho_nl/items/05ee4c8d77e99e29daa5
	//

	inline void compute_kCurves
	(
		const std::vector<EVec2f>& cps,

		int  num_sample, //sampling number of each curve segment
		std::vector<EVec2f>& out_cps,
		std::vector<EVec2f>& out_points
	)
	{
		if (cps.size() < 4) return;
		if (num_sample < 3) num_sample = 3;

		const int N = (int)cps.size();
		std::vector<float> lambda(N, 0.5f), ti(N);
		std::vector<EVec2f> Ci0(N), Ci1 = cps, Ci2(N);

		for (int iter = 0; iter < NUMBER_ITERATION; ++iter)
		{
			//1. update lambda (use 0.5 for the first iteration )
			// iter < ITER_NUM / 2 は、安定化のためのtrick
			if (iter != 0 && iter < NUMBER_ITERATION / 2)
			{
				for (int i = 0; i < N; ++i)
				{
					int next_i = (i + 1) % N;
					float A = sqrt(TriangleArea(Ci0[i], Ci1[i], Ci1[next_i]));
					float B = sqrt(TriangleArea(Ci0[i], Ci1[i], Ci1[next_i]));
					float C = sqrt(TriangleArea(Ci1[i], Ci1[next_i], Ci2[next_i]));
					lambda[i] = A / (B + C);
				}
			}

			//2. update Ci2, Ci+10
			for (int i = 0; i < N; ++i)
			{
				int next_i = (i + 1) % N;
				Ci2[i] = Ci0[next_i] = (1 - lambda[i]) * Ci1[i] +
					lambda[i] * Ci1[next_i];
			}

			//3. update ti
			for (int i = 0; i < N; ++i)
			{
				//solve eq(4)
				EVec2f Ci2_Ci0 = Ci2[i] - Ci0[i];
				EVec2f Ci0_pi = Ci0[i] - cps[i];
				float a = Ci2_Ci0.squaredNorm();
				float b = 3 * Ci2_Ci0.dot(Ci0_pi);
				float c = (3 * Ci0[i] - 2 * cps[i] - Ci2[i]).dot(Ci0_pi);
				float d = -Ci0_pi.squaredNorm();

				ti[i] = (a == 0) ? 0.5f : SolveCubic01(b / a, c / a, d / a);
			}

			//4. update C1
			ESpMat A(N, N);
			Eigen::VectorXf b1(N), b2(N);
			std::vector< Eigen::Triplet<float> > entries; //{row, col, val}

			for (int i = 0; i < N; ++i)
			{
				int nextI = (i + 1) % N;
				int prevI = (i - 1) < 0 ? N - 1 : i - 1;
				float t = ti[i];
				float a = (1 - lambda[prevI]) * (1 - t) * (1 - t);
				float b = lambda[i] * t * t;
				float c = lambda[prevI] * (1 - t) * (1 - t) + (2 - (1 + lambda[i]) * t) * t;

				entries.push_back(ETrip(i, prevI, a));
				entries.push_back(ETrip(i, i, c));
				entries.push_back(ETrip(i, nextI, b));
				b1[i] = cps[i][0];
				b2[i] = cps[i][1];
			}

			A.setFromTriplets(entries.begin(), entries.end());
			// solve Ax = b
			Eigen::SparseLU<ESpMat> LU(A);
			Eigen::VectorXf x = LU.solve(b1);
			Eigen::VectorXf y = LU.solve(b2);

			for (int i = 0; i < N; ++i) Ci1[i] << x[i], y[i];

		}

		out_cps.clear();
		for (int i = 0; i < N; ++i)
		{
			out_cps.push_back(Ci0[i]);
			out_cps.push_back(Ci1[i]);
			out_cps.push_back(Ci2[i]);
		}

		out_points.clear();
		for (int i = 0; i < N; ++i)
		{
			const EVec2f& C0 = Ci0[i];
			const EVec2f& C1 = Ci1[i];
			const EVec2f& C2 = Ci2[i];

			for (int j = 0; j < num_sample; ++j)
			{
				float t = j * 1.0f / num_sample;
				EVec2f p = (1 - t) * (1 - t) * C0 + 2 * t * (1 - t) * C1 + t * t * C2;
				out_points.push_back(p);
			}
		}
	}

	//memo 
	// 
	// 参照論文: "Kappa-Curves: Interpolating B-splines with Local Curvature Control" (2017)
	// 
	// Step 4の線形システムについて
	// 
	// 論文中の式 pi = α c(i-1,1) + β c(i,1) + γ c(i+1,1) は以下のように導出される。
	//
	// ★ 2次元ベジエの式3より
	// pi = (1 - ti)^2 * c(i,0) + 2 ti (1 - ti) * c(i,1) + ti^2 * c(i,2)
	// 
	// ★接続条件の定義 (Eq.5)
	// c(i,2) =     (1 - λi) * c( i ,1) +     λi * c(i+1,1)
	// c(i,0) = (1 - λ(i-1)) * c(i-1,1) + λ(i-1) * c( i ,1),  c(i,0) = c(i-1,2)
	//
	// 上記 Eq.5 を Eq.3 へ代入して整理 
	// pi = [ (1-λ(i-1))(1-ti)^2 ]                       * c(i-1,1)  <-- α(i)
	//    + [ λ(i-1)(1-ti)^2 + 2ti(1-ti) + (1-λi)ti^2 ] * c( i ,1)  <-- β(i)
	//    + [ λi * ti^2 ]                                * c(i+1,1)  <-- γ(i)
	//
	//
	// ◯ 開いた曲線の場合 p0, p1, ..., pN に対して、
	// (1)p0, pN は端点としてよけ、p1～p(N-1)のそれぞれに2Dベジエ曲線を作る
	// (2) Step 4の線形システムは、以下のようになる
	// 
	// 
	// (*)p1 において  c(1,0),c(1,1), c(1,2)によりベジエができるが、c(1,0)が端点（p0) になるので、、、
	// 
	// pi = (1 - ti)^2 * c(i,0) + 2 ti (1 - ti) * c(i,1) + ti^2 * c(i,2)
	// c(i,2) =     (1 - λi) * c( i ,1) +     λi * c(i+1,1)
	// c(i,0) = p0
	// これを代入すると、、 
	// pi = (1 - ti)^2 * p0 
	//    + [ 2ti(1 - ti) + (1 - λi)ti^2 ] * c( i ,1)  
	//    + [ λi * ti^2 ]                  * c(i+1,1)
	// 
	// (*)p(N-1)では c(N-1,0),c(N-1,1), c(N-1,2)によりベジエができるが、c(N-1,2)が端点（pN) になるので、、、
	// 
	// pi = (1 - ti)^2 * c(i,0) + 2 ti (1 - ti) * c(i,1) + ti^2 * c(i,2)
	// c(i,0) = (1 - λ(i-1)) * c(i-1,1) + λ(i-1) * c( i ,1)
	// c(i,2) = pN
	// これを代入すると、、 
	// pi = [ (1 - λ(i-1))(1 - ti)^2 ]         * c(i-1,1)
	//    + [ λ(i-1)(1 - ti)^2 + 2ti(1 - ti) ] * c( i ,1)
	//    + [ ti^2 ]                            * pN
	// 
	// N=1の時の対応
	// pi = (1 - ti)^2 * start_cp + 2 ti (1 - ti) * c(1,1) + ti^2 * end_cp
	// c(1,1) = ( pi - (1 - ti)^2 * start_cp - ti^2 * end_cp ) / ( 2 ti (1 - ti) )



	//
	// input : cps        2D control points,        size: N 
	// output: out_cps    2D Bezier control points, size: 3 * (N-2), each 3 construct quad bezier
	// output: out_points 2D curve
	//
	// note
	// 各制御点 cps[i] に対して、2次ベジエ制御点 Ci0, Ci1, Ci2を生成する
	//
	// ただし，
	// cps[1  ]に対する曲線は、cps[0]=C10, C11, C12で決定される
	// cps[N-2]に対する曲線は、CN-2,0, CN-2,1, CN-2,2=cps[N-1]で決定される


	inline void compute_kCurves_open
	(
		const std::vector<EVec2f>& _cps,

		int  num_sample, //sampling number of each curve segment
		std::vector<EVec2f>& out_cps,
		std::vector<EVec2f>& out_points
	)
	{
		if (num_sample < 3) num_sample = 3;
	
		if (_cps.size() <= 1) return;
	
		if (_cps.size() == 2)
		{
			//直線
			out_cps.clear();
			out_cps.push_back(_cps[0]);
			out_cps.push_back(_cps[1]);
			out_points.clear();
			for (int j = 0; j < num_sample; ++j)
			{
				float t = j * 1.0f / num_sample;
				EVec2f p = (1 - t) * _cps[0] + t * _cps[1];
				out_points.push_back(p);
			}
			return;
    }

		//N 本の 2次ベジエを生成する
		const int N = (int)_cps.size() - 2;
		const EVec2f start_cp = _cps.front();
		const EVec2f end_cp = _cps.back();

		std::vector<EVec2f> cps(_cps.begin() + 1, _cps.end() - 1);
		std::vector<float>  lambda(N, 0.5f), ti(N);
		std::vector<EVec2f> Ci0(N), Ci1 = cps, Ci2(N);

		Ci0[0] = start_cp;
		Ci2[N - 1] = end_cp;

		for (int iter = 0; iter < NUMBER_ITERATION; ++iter)
		{
			//1. update lambda (use 0.5 for the first iteration )
			// iter < ITER_NUM / 2 は、安定化のためのtrick
			// lambda[N-1]は不要なことに注意
			if (iter != 0 && iter < NUMBER_ITERATION / 2)
			{
				for (int i = 0; i < N - 1; ++i)
				{
					float A = sqrt(TriangleArea(Ci0[i], Ci1[i], Ci1[i + 1]));
					float B = sqrt(TriangleArea(Ci0[i], Ci1[i], Ci1[i + 1]));
					float C = sqrt(TriangleArea(Ci1[i], Ci1[i + 1], Ci2[i + 1]));
					lambda[i] = A / (B + C);
				}
			}

			//2. update Ci2, Ci+1,0
			for (int i = 0; i < N - 1; ++i)
			{
				Ci2[i] = Ci0[i + 1] = (1 - lambda[i]) * Ci1[i] + lambda[i] * Ci1[i + 1];
			}
			Ci0[0] = start_cp;
			Ci2[N - 1] = end_cp;

			//3. update ti
			for (int i = 0; i < N; ++i)
			{
				//solve eq(4)
				EVec2f Ci2_Ci0 = Ci2[i] - Ci0[i];
				EVec2f Ci0_pi = Ci0[i] - cps[i];
				float a = Ci2_Ci0.squaredNorm();
				float b = 3 * Ci2_Ci0.dot(Ci0_pi);
				float c = (3 * Ci0[i] - 2 * cps[i] - Ci2[i]).dot(Ci0_pi);
				float d = -Ci0_pi.squaredNorm();
				ti[i] = (a == 0) ? 0.5f : SolveCubic01(b / a, c / a, d / a);
				if (std::isnan(ti[i])) ti[i] = 0.5f;
				ti[i] = std::max(1e-5f, std::min(1.0f - 1e-5f, ti[i]));
			}

			//4. update C1 
			ESpMat A(N, N);
			Eigen::VectorXf b1(N), b2(N);
			std::vector< Eigen::Triplet<float> > entries; //{row, col, val}

			for (int i = 0; i < N; ++i)
			{
				const float t = ti[i];
				if (i == 0 && i == N - 1) // N =1 のとき
				{
					entries.push_back(ETrip(i, i, 2 * t * (1 - t)));
					b1[i] = cps[i][0] - (1 - t) * (1 - t) * start_cp[0] - t * t * end_cp[0];
					b2[i] = cps[i][1] - (1 - t) * (1 - t) * start_cp[1] - t * t * end_cp[1];
				}
				else if (i == 0)
				{
					const float b = 2 * t * (1 - t) + (1 - lambda[i]) * t * t;
					const float c = lambda[i] * t * t;
					entries.push_back(ETrip(i, i, b));
					entries.push_back(ETrip(i, i + 1, c));
					b1[i] = cps[i][0] - (1 - t) * (1 - t) * start_cp[0];
					b2[i] = cps[i][1] - (1 - t) * (1 - t) * start_cp[1];
				}
				else if (i == N - 1)
				{
					const float a = (1 - lambda[i - 1]) * (1 - t) * (1 - t);
					const float b = lambda[i - 1] * (1 - t) * (1 - t) + 2 * t * (1 - t);
					entries.push_back(ETrip(i, i - 1, a));
					entries.push_back(ETrip(i, i, b));
					b1[i] = cps[i][0] - t * t * end_cp[0];
					b2[i] = cps[i][1] - t * t * end_cp[1];
				}
				else
				{
					const float a = (1 - lambda[i - 1]) * (1 - t) * (1 - t);
					const float b = lambda[i - 1] * (1 - t) * (1 - t) + (2 - (1 + lambda[i]) * t) * t;
					const float c = lambda[i] * t * t;
					entries.push_back(ETrip(i, i - 1, a));
					entries.push_back(ETrip(i, i, b));
					entries.push_back(ETrip(i, i + 1, c));
					b1[i] = cps[i][0];
					b2[i] = cps[i][1];
				}
			}

			A.setFromTriplets(entries.begin(), entries.end());
			// solve Ax = b
			Eigen::SparseLU<ESpMat> LU(A);
			Eigen::VectorXf x = LU.solve(b1);
			Eigen::VectorXf y = LU.solve(b2);

			for (int i = 0; i < N; ++i) Ci1[i] << x[i], y[i];
		}

		out_cps.clear();
		for (int i = 0; i < N; ++i)
		{
			out_cps.push_back(Ci0[i]);
			out_cps.push_back(Ci1[i]);
			out_cps.push_back(Ci2[i]);
		}

		out_points.clear();
		for (int i = 0; i < N; ++i)
		{
			const EVec2f& C0 = Ci0[i];
			const EVec2f& C1 = Ci1[i];
			const EVec2f& C2 = Ci2[i];

			for (int j = 0; j < num_sample; ++j)
			{
				float t = j * 1.0f / num_sample;
				EVec2f p = (1 - t) * (1 - t) * C0 + 2 * t * (1 - t) * C1 + t * t * C2;
				out_points.push_back(p);
			}
		}
	}

}


#endif











/*
* OLD : kcurves.h
* 
typedef Eigen::Vector2f EVec2f;
typedef Eigen::VectorXf EVecXf;
typedef Eigen::MatrixXf EMatXf;

// prototype declaration

inline float CalcArea(const EVec2f&, const EVec2f&, const EVec2f&);
inline float SolveFormula(const float&, const float&, const float&, const float&);
inline bool IsInsideTriangle(const EVec2f&, const EVec2f&, const EVec2f&, const EVec2f&);
inline std::vector<EVec2f> CalcBezierCPs(const std::vector<EVec2f>&);
inline std::vector<EVec2f> CalcBezierCurves(const std::vector<EVec2f>&, const std::vector<EVec2f>&);
inline std::vector<EVec2f> CalcKCurvesOpen(const std::vector<EVec2f>&);


// calculate triangle area
inline float CalcArea(const EVec2f& _a, const EVec2f& _b, const EVec2f& _c)
{
  const EVec2f vec_a = _b - _a;
  const EVec2f vec_b = _c - _a;
  const float product_norm_ab = vec_a.norm() * vec_b.norm();;
  const float theta = vec_a.dot(vec_b) / product_norm_ab;
  const float area = product_norm_ab * sqrt(1.0f - theta * theta);

  return area;
}


// solve cubic formula
// only real solution in the range [0, 1]
// https://ja.wikipedia.org/wiki/%E4%B8%89%E6%AC%A1%E6%96%B9%E7%A8%8B%E5%BC%8F
// 係数をきれいにするため、よく見かける(q, p)ではなく (q/2, p/3)を計算する
inline float SolveFormula(const float& _a, const float& _b, const float& _c, const float& _d)
{
  const float b = _b / _a / 3;
  const float c = _c / _a;
  const float d = _d / _a;

  const float p = c / 3 - b * b;
  const float q = b * b * b - (b * c - d) / 2;
  const float D = q * q + p * p * p;

  if (fabsf(D) < FLT_MIN)
  {
    const float i = cbrtf(q) - b;
    if (i >= 0.0f)
    {
      return fminf(i, 1.0f);
    }
    else
    {
      return fminf(i * -2.0f, 1.0f);
    }
  }
  else if (D > 0.0f)
  {
    const float u = cbrtf(-q + sqrtf(D));
    const float v = cbrtf(-q - sqrtf(D));
    const float i = u + v - b;
    if (i < 0.0f) return 0.0f;
    if (i > 1.0f) return 1.0f;
    return i;
  }
  else
  {
    const float n = 2 * sqrtf(-p);
    const float arg = atan2f(sqrtf(-D), -q) / 3;
    const float pi2d3 = 2 * static_cast<float>(M_PI) / 3;
    const float i = n * cosf(arg) - b;
    if (0 <= i && i <= 1) return i;
    const float j = n * cosf(arg + pi2d3) - b;
    if (0 <= j && j <= 1) return j;
    const float k = n * cosf(arg - pi2d3) - b;
    if (0 <= k && k <= 1) return k;
  }

  return 0.50001f;
}

// is the point inside or outside the triangle
inline bool IsInsideTriangle(const EVec2f& _p, const EVec2f& _a, const EVec2f& _b, const EVec2f& _c)
{
  const EVec3f ab = { (_b - _a)[0], (_b - _a)[1], 0 };
  const EVec3f bc = { (_c - _b)[0], (_c - _b)[1], 0 };
  const EVec3f ca = { (_a - _c)[0], (_a - _c)[1], 0 };

  const EVec3f bp = { (_p - _b)[0], (_p - _b)[1], 0 };
  const EVec3f cp = { (_p - _c)[0], (_p - _c)[1], 0 };
  const EVec3f ap = { (_p - _a)[0], (_p - _a)[1], 0 };

  const float abp = (ab.cross(bp))[2];
  const float bcp = (bc.cross(cp))[2];
  const float cap = (ca.cross(ap))[2];

  if (((abp > 0) && (bcp > 0) && (cap > 0)) || ((abp < 0) && (bcp < 0) && (cap < 0)))
  {
    return true;
  }

  return false;
}


// calculate Bezier control points from k-curves control points
inline std::vector<EVec2f> CalcBezierCPs(const std::vector<EVec2f>& _kcurves_cps)
{
  const int number_segments = static_cast<int>(_kcurves_cps.size()) - 2;
  std::vector<EVec2f> bezier_cps(2 * number_segments + 1);
  std::vector<float> lambda(number_segments + 1);
  std::vector<float> t(number_segments + 2);
  EMatXf mat_A(number_segments + 4, number_segments + 4);

  // debug
  //show_comment("\n>>START CALC\n");
  //show_vector_EVec2f("kcurves CPs", _kcurves_cps);

  // initialize
  bezier_cps[0] = _kcurves_cps[0];
  bezier_cps[2 * number_segments] = _kcurves_cps[number_segments + 1];
  lambda[0] = 0.0f;
  lambda[number_segments] = 1.0f;
  t[0] = 0.0f;
  t[number_segments + 1] = 1.0f;
  for (int i = 0; i < number_segments; ++i)
  {
    bezier_cps[2 * i + 1] = _kcurves_cps[i + 1];
    t[i + 1] = 0.5f;
  }
  for (int i = 0; i < number_segments - 1; ++i)
  {
    bezier_cps[2 * i + 2] = (bezier_cps[2 * i + 1] + bezier_cps[2 * i + 3]) / 2;
    lambda[i + 1] = 0.5f;
  }
  for (int i = 0; i < number_segments + 4; ++i)
  {
    for (int j = 0; j < number_segments + 4; ++j)
    {
      mat_A(i, j) = 0.0f;
    }
  }
  mat_A(0, 0) = 1.0f;
  mat_A(1, 1) = 1.0f;
  mat_A(number_segments + 2, number_segments + 2) = 1.0f;
  mat_A(number_segments + 3, number_segments + 3) = 1.0f;

  // debug
  //show_step(0);
  //show_vector_EVec2f("Bezier CPs", bezier_cps);

  // steps
  for (int step = 0; step < NUMBER_ITERATION; ++step)
  {
    // estimate lambda
    for (int n = 0; n < number_segments - 1; ++n)
    {
      const int i = n + 1;
      const float a = sqrtf(CalcArea(bezier_cps[2 * i - 2], bezier_cps[2 * i - 1], bezier_cps[2 * i + 1]));
      const float b = sqrtf(CalcArea(bezier_cps[2 * i - 1], bezier_cps[2 * i + 1], bezier_cps[2 * i + 2]));
      lambda[i] = a / (a + b);
    }

    // debug
    //show_vector_float("lambda", lambda);

    // update c_(i,0), c_(i,2)
    for (int n = 0; n < number_segments - 1; ++n)
    {
      const int i = n + 1;
      bezier_cps[2 * i] = (1 - lambda[i]) * bezier_cps[2 * i - 1] + lambda[i] * bezier_cps[2 * i + 1];
    }

    // calculate t
    for (int n = 0; n < number_segments; ++n)
    {
      const int i = n + 1;
      const float a = (bezier_cps[2 * i] - bezier_cps[2 * i - 2]).squaredNorm();
      const float b = -3 * (bezier_cps[2 * i] - bezier_cps[2 * i - 2]).dot(_kcurves_cps[i] - bezier_cps[2 * i - 2]);
      const float c = (-3 * bezier_cps[2 * i - 2] + 2 * _kcurves_cps[i] + bezier_cps[2 * i]).dot(_kcurves_cps[i] - bezier_cps[2 * i - 2]);
      const float d = -(_kcurves_cps[i] - bezier_cps[2 * i - 2]).squaredNorm();
      t[i] = SolveFormula(a, b, c, d);
    }

    // debug
    //show_vector_float("t", t);

    // update c_(i, 1)
    EVecXf vec_bx(number_segments + 4), vec_by(number_segments + 4);
    vec_bx[0] = bezier_cps[2 * number_segments][0];
    vec_by[0] = bezier_cps[2 * number_segments][1];
    vec_bx[1] = _kcurves_cps[0][0];
    vec_by[1] = _kcurves_cps[0][1];
    vec_bx[number_segments + 2] = _kcurves_cps[number_segments + 1][0];
    vec_by[number_segments + 2] = _kcurves_cps[number_segments + 1][1];
    vec_bx[number_segments + 3] = bezier_cps[0][0];
    vec_by[number_segments + 3] = bezier_cps[0][1];
    for (int n = 0; n < number_segments; ++n)
    {
      const int i = n + 1;
      const float a = (1 - lambda[i - 1]) * powf(1 - t[i], 2);
      const float b = lambda[i - 1] * powf(1 - t[i], 2) + (2 - (1 + lambda[i]) * t[i]) * t[i];
      const float c = lambda[i] * powf(t[i], 2);
      mat_A(i + 1, i) = a;
      mat_A(i + 1, i + 1) = b;
      mat_A(i + 1, i + 2) = c;
      vec_bx[i + 1] = _kcurves_cps[i][0];
      vec_by[i + 1] = _kcurves_cps[i][1];
    }


    const EVecXf vec_xx = mat_A.fullPivLu().solve(vec_bx);
    const EVecXf vec_xy = mat_A.fullPivLu().solve(vec_by);
    for (int i = 1; i < number_segments + 1; ++i)
    {
      bezier_cps[2 * i - 1][0] = vec_xx[i + 1];
      bezier_cps[2 * i - 1][1] = vec_xy[i + 1];
    }

  }

  // debug
  //show_comment("\n>>FINISH CALC\n");

  return bezier_cps;
}


// calculate Bezier curves from Bezier control points
inline std::vector<EVec2f> CalcBezierCurves(const std::vector<EVec2f>& _bezier_cps, const std::vector<EVec2f>& _kcurves_cps)
{
  const int number_segments = static_cast<int>(_bezier_cps.size()) / 2;
  std::vector<EVec2f> curves;

  for (int i = 0; i < number_segments; ++i)
  {
    // check getting the right result
    if (!IsInsideTriangle(_kcurves_cps[i + 1], _bezier_cps[i * 2], _bezier_cps[i * 2 + 1], _bezier_cps[i * 2 + 2]))
    {
      //std::cout << "wrong result" << std::endl; // debug
      return std::vector<EVec2f>();
    }

    // calc curve
    for (int j = 0; j <= BEZIER_SAMPLING_INTERVAL; ++j)
    {
      const float t = (float)j / BEZIER_SAMPLING_INTERVAL;
      const EVec2f p = powf(1 - t, 2) * _bezier_cps[2 * i] + 2 * (1 - t) * t * _bezier_cps[2 * i + 1] + powf(t, 2) * _bezier_cps[2 * i + 2];
      curves.push_back(p);
    }
  }

  return curves;
}


// calculate open k-curves from k-curves control points
inline std::vector<EVec2f> CalcKCurvesOpen(const std::vector<EVec2f>& _kcurves_cps)
{
  const std::vector<EVec2f> bezier_cps = CalcBezierCPs(_kcurves_cps);
  const std::vector<EVec2f> curves = CalcBezierCurves(bezier_cps, _kcurves_cps);

  return curves;
}
*/
