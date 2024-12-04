#ifndef COMMON_KCURVES_OPEN_H_
#define COMMON_KCURVES_OPEN_H_
#pragma unmanaged

#include <Core>
#include <iostream>
#include <vector>


namespace KCURVES_OPEN
{
  typedef Eigen::Vector2f EVec2f;
  typedef Eigen::VectorXf EVecXf;
  typedef Eigen::MatrixXf EMatXf;

  static const int NUMBER_ITERATION = 15;
  static const int BEZIER_SAMPLING_INTERVAL = 15;


  // prototype declaration

  inline float calcArea(const EVec2f&, const EVec2f&, const EVec2f&);
  inline float solveFormula(const float&, const float&, const float&, const float&);
  inline bool IsInsideTriangle(const EVec2f&, const EVec2f&, const EVec2f&, const EVec2f&);
  inline std::vector<EVec2f> calcBezierCPs(const std::vector<EVec2f>&);
  inline std::vector<EVec2f> calcBezierCurves(const std::vector<EVec2f>&, const std::vector<EVec2f>&);
  inline std::vector<EVec2f> calcKCurvesOpen(const std::vector<EVec2f>&);
  //inline void show_step(const int&);
  //inline void show_vector_EVec2f(const std::string&, const std::vector<EVec2f>&);
  //inline void show_vector_float(const std::string&, const std::vector<float>&);
  //inline void show_EMatXf(const std::string&, const EMatXf&);
  //inline void show_EVecXf(const std::string&, const EVecXf&);
  //inline void show_comment(const std::string& _comment);


  // calculate triangle area
  inline float calcArea(const EVec2f& _a, const EVec2f& _b, const EVec2f&_c)
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
  inline float solveFormula(const float& _a, const float& _b, const float& _c, const float& _d)
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
  inline std::vector<EVec2f> calcBezierCPs(const std::vector<EVec2f>& _kcurves_cps)
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
        const float a = sqrtf(calcArea(bezier_cps[2 * i - 2], bezier_cps[2 * i - 1], bezier_cps[2 * i + 1]));
        const float b = sqrtf(calcArea(bezier_cps[2 * i - 1], bezier_cps[2 * i + 1], bezier_cps[2 * i + 2]));
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
        t[i] = solveFormula(a, b, c, d);
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

      // debug
      //show_EMatXf("mat_A", mat_A);
      //show_EVecXf("vec_bx", vec_bx);
      //show_EVecXf("vec_by", vec_by);

      const EVecXf vec_xx = mat_A.fullPivLu().solve(vec_bx);
      const EVecXf vec_xy = mat_A.fullPivLu().solve(vec_by);
      for (int i = 1; i < number_segments + 1; ++i)
      {
        bezier_cps[2 * i - 1][0] = vec_xx[i + 1];
        bezier_cps[2 * i - 1][1] = vec_xy[i + 1];
      }

      // debug
      //show_EVecXf("vec_xx", vec_xx);
      //show_EVecXf("vec_xy", vec_xy);

      // debug
      //show_step(step + 1);
      //show_vector_EVec2f("Bezier CPs", bezier_cps);
    }

    // debug
    //show_comment("\n>>FINISH CALC\n");

    return bezier_cps;
  }


  // calculate Bezier curves from Bezier control points
  inline std::vector<EVec2f> calcBezierCurves(const std::vector<EVec2f>& _bezier_cps, const std::vector<EVec2f>& _kcurves_cps)
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
  inline std::vector<EVec2f> calcKCurvesOpen(const std::vector<EVec2f>& _kcurves_cps)
  {
    const std::vector<EVec2f> bezier_cps = calcBezierCPs(_kcurves_cps);
    const std::vector<EVec2f> curves = calcBezierCurves(bezier_cps, _kcurves_cps);

    return curves;
  }


  // for debug
  //inline void show_step(const int& _step, const bool& _show_debug)
  //{
  //  std::cout << "\nSTEP: " << _step << std::endl;
  //}


  // for debug
  //inline void show_vector_EVec2f(const std::string& name, const std::vector<EVec2f>& _vec)
  //{
  //  const int size = static_cast<int>(_vec.size());
  //  std::cout << name << "[" << size << "]:" << std::endl;

  //  for (int i = 0; i < size; ++i)
  //  {
  //    std::cout << "  " << _vec[i][0] << ", " << _vec[i][1] << std::endl;
  //  }
  //}


  // for debug
  //inline void show_vector_float(const std::string& name, const std::vector<float>& _vec)
  //{
  //  const int size = static_cast<int>(_vec.size());
  //  std::cout << name << "[" << size << "]:" << std::endl;

  //  for (int i = 0; i < size; ++i)
  //  {
  //    std::cout << "  " << _vec[i] << std::endl;
  //  }
  //}


  // for debug
  //inline void show_EMatXf(const std::string& name, const EMatXf& _mat)
  //{
  //  std::cout << name << ": \n" << _mat << std::endl;
  //}


  // for debug
  //inline void show_EVecXf(const std::string& name, const EVecXf& _vec)
  //{
  //  const int size = static_cast<int>(_vec.size());
  //  std::cout << name << "[" << size << "]:" << std::endl;

  //  for (int i = 0; i < _vec.size(); ++i)
  //  {
  //    std::cout << "  " << _vec[i] << std::endl;
  //  }
  //}

  // for debug
  //inline void show_comment(const std::string& _comment)
  //{
  //  std::cout << _comment << std::endl;
  //}
}


#endif
