#pragma once
#pragma unmanaged

#include <Core>
#include <iostream>
#include <vector>
#include "CagedMeshSequence.h"

#define NUM_P (int)100


namespace DeformByStroke
{
  typedef Eigen::Vector3f EVec3f;
  typedef Eigen::VectorXf EVecXf;
  typedef Eigen::MatrixXf EMatXf;


  // prototype declaration
  inline void Deform(CagedMeshSequence&, const std::vector<std::vector<EVec3f>>&, const int&, const float& _alpha = 1.0f, const float& _beta = 1.0f, const float& _gamma = 5.0f);
  inline void CalcA(CagedMeshSequence&, const std::vector<EVec3f>&, const int&, EMatXf&, EVecXf&, EVecXf&, EVecXf&);
  inline void CalcB(CagedMeshSequence&, const int&, EMatXf&, EVecXf&, EVecXf&, EVecXf&);
  inline void CalcC(CagedMeshSequence&, const int&, EMatXf&, EVecXf&, EVecXf&, EVecXf&);
  inline bool CalcClosestPointBetweenPointAndTriangle(const EVec3f&, const EVec3f&, const EVec3f&, const EVec3f&, std::vector<float>&);
  inline EMatXf CalcPro(const EMatXf&, const EMatXf&);


  // deform by stroke
  inline void Deform(
    CagedMeshSequence& _meshseq,
    const std::vector<std::vector<EVec3f>>& _strokes,
    const int& _frame_idx,
    const float& _alpha,
    const float& _beta,
    const float& _gamma)
  {

    std::vector<EVec3f> p0;
    const int strokes_size = static_cast<const int>(_strokes.size());

    if (strokes_size == 0) // exception
    {
      std::cout << "stroke size is too small." << std::endl;
      return;
    }

    const int num = NUM_P / strokes_size;
    const int num_tail = NUM_P - num * (strokes_size - 1);
    for (int i = 0; i < strokes_size - 1; ++i)
    {
      std::vector<EVec3f> vec;
      VertsResampleEqualInterval(num - 1, _strokes[i], vec);
      p0.insert(p0.end(), vec.begin(), vec.end());
    }
    std::vector<EVec3f> vec;
    VertsResampleEqualInterval(num_tail - 1, _strokes[strokes_size - 1], vec);
    p0.insert(p0.end(), vec.begin(), vec.end());

    const int numCageVtx = _meshseq.GetNumCageVertex();

    // calc matrix a, b, c
    EMatXf matA, matB, matC;
    EVecXf vecAx, vecAy, vecAz;
    EVecXf vecBx, vecBy, vecBz;
    EVecXf vecCx, vecCy, vecCz;
    CalcA(_meshseq, p0, _frame_idx, matA, vecAx, vecAy, vecAz);
    CalcB(_meshseq, _frame_idx, matB, vecBx, vecBy, vecBz);
    CalcC(_meshseq, _frame_idx, matC, vecCx, vecCy, vecCz);

    // solve formula
    const EMatXf matAtrans = matA.transpose();
    const EMatXf matBtrans = matB.transpose();
    const EMatXf matCtrans = matC.transpose();
    //const EMatXf mat  = alpha * matAtrans * matA  + beta * matBtrans * matB  + gamma * matCtrans * matC ;
    //const EVecXf vecx = alpha * matAtrans * vecAx + beta * matBtrans * vecBx + gamma * matCtrans * vecCx;
    //const EVecXf vecy = alpha * matAtrans * vecAy + beta * matBtrans * vecBy + gamma * matCtrans * vecCy;
    //const EVecXf vecz = alpha * matAtrans * vecAz + beta * matBtrans * vecBz + gamma * matCtrans * vecCz;
    const EMatXf mat = _alpha * CalcPro(matAtrans, matA) + _beta * CalcPro(matBtrans, matB) + _gamma * CalcPro(matCtrans, matC);
    const EVecXf vecx = _alpha * CalcPro(matAtrans, vecAx) + _beta * CalcPro(matBtrans, vecBx) + _gamma * CalcPro(matCtrans, vecCx);
    const EVecXf vecy = _alpha * CalcPro(matAtrans, vecAy) + _beta * CalcPro(matBtrans, vecBy) + _gamma * CalcPro(matCtrans, vecCy);
    const EVecXf vecz = _alpha * CalcPro(matAtrans, vecAz) + _beta * CalcPro(matBtrans, vecBz) + _gamma * CalcPro(matCtrans, vecCz);

    const EVecXf solx = mat.fullPivLu().solve(vecx);
    const EVecXf soly = mat.fullPivLu().solve(vecy);
    const EVecXf solz = mat.fullPivLu().solve(vecz);

    // deform cage
    std::vector<EVec3f> verts = std::vector<EVec3f>(numCageVtx);
    for (int i = 0; i < numCageVtx; ++i)
    {
      verts[i] = { solx[i], soly[i], solz[i] };
    }
    _meshseq.SetCageVertices(_frame_idx, verts, true);
  }


  // calc matA and set vecA
  inline void CalcA(
    CagedMeshSequence& _meshseq,
    const std::vector<EVec3f>& _p0,
    const int& _frame_idx,
    EMatXf& _mat,
    EVecXf& _vecx,
    EVecXf& _vecy,
    EVecXf& _vecz)
  {
    // initialize
    const int numCageVtx = _meshseq.GetNumCageVertex();
    _mat = EMatXf(NUM_P, numCageVtx);
    _vecx = EVecXf(NUM_P);
    _vecy = EVecXf(NUM_P);
    _vecz = EVecXf(NUM_P);

    // set p
    const TMesh& mesh = _meshseq.GetMesh(_frame_idx);
    for (int i = 0; i < NUM_P; ++i)
    {
      // find the closest point on the triangle mesh containing p_i
      const int vtxIdx = mesh.GetNearestVertexIdx(_p0[i]);
      const std::vector<int> vRingPs = mesh.m_vRingPs[vtxIdx];
      int* pIdx;
      bool flag = false;
      std::vector<float> barycentric;
      for (const auto j : vRingPs)
      {
        pIdx = mesh.m_pPolys[j].idx;
        const EVec3f& x0 = mesh.m_vVerts[pIdx[0]];
        const EVec3f& x1 = mesh.m_vVerts[pIdx[1]];
        const EVec3f& x2 = mesh.m_vVerts[pIdx[2]];
        flag = CalcClosestPointBetweenPointAndTriangle(_p0[i], x0, x1, x2, barycentric);
        if (flag) break;
      }

      // calc matA
      if (flag)
      {
        for (int j = 0; j < numCageVtx; ++j)
        {
          float sum = 0.0f;
          for (int k = 0; k < 3; ++k)
          {
            sum += barycentric[k] * _meshseq.GetHarmCoord()[pIdx[k]][j];
          }
          _mat(i, j) = sum;
        }
      }
      else
      {
        for (int j = 0; j < numCageVtx; ++j)
        {
          _mat(i, j) = _meshseq.GetHarmCoord()[vtxIdx][j];
        }
      }
    }

    // calc vecA
    for (int i = 0; i < NUM_P; ++i)
    {
      _vecx[i] = _p0[i][0];
      _vecy[i] = _p0[i][1];
      _vecz[i] = _p0[i][2];
    }
  }


  // calc matB and vecB
  inline void CalcB(
    CagedMeshSequence& _meshseq,
    const int& _frame_idx,
    EMatXf& _mat,
    EVecXf& _vecx,
    EVecXf& _vecy,
    EVecXf& _vecz)
  {
    const int numCageVtx = _meshseq.GetNumCageVertex();
    const std::vector<EVec3f>& cageVertices = _meshseq.GetCageVertices(_frame_idx);
    _mat = EMatXf(numCageVtx, numCageVtx).setIdentity();
    _vecx = EVecXf(numCageVtx);
    _vecy = EVecXf(numCageVtx);
    _vecz = EVecXf(numCageVtx);
    for (int i = 0; i < numCageVtx; ++i)
    {
      _vecx[i] = cageVertices[i][0];
      _vecy[i] = cageVertices[i][1];
      _vecz[i] = cageVertices[i][2];
    }
  }


  // calc matC and vecC
  inline void CalcC(
    CagedMeshSequence& _meshseq,
    const int& _frame_idx,
    EMatXf& _mat,
    EVecXf& _vecx,
    EVecXf& _vecy,
    EVecXf& _vecz)
  {
    // initialize
    const int numCageVtx = _meshseq.GetNumCageVertex();
    const std::vector<EVec3f>& cageVertices = _meshseq.GetCageVertices(_frame_idx);
    _mat = EMatXf(numCageVtx, numCageVtx).setIdentity();
    _vecx = EVecXf(numCageVtx);
    _vecy = EVecXf(numCageVtx);
    _vecz = EVecXf(numCageVtx);

    // calc matC and vecC
    const TMesh& cage = _meshseq.GetCage(_frame_idx);
    for (int i = 0; i < numCageVtx; ++i)
    {
      // get ring
      const std::vector<int> vRingVs = cage.m_vRingVs[i];
      const int vRingVsSize = (int)vRingVs.size();

      // calc matC and vecC
      EVec3f LapVec = cageVertices[i];
      for (const auto j : vRingVs)
      {
        _mat(i, j) = -1.0f / vRingVsSize;
        LapVec -= cage.m_vVerts[j] / static_cast<float>(vRingVsSize);
      }
      _vecx[i] = LapVec[0];
      _vecy[i] = LapVec[1];
      _vecz[i] = LapVec[2];
    }
  }


  // calc the closest point between point to triangle
  // output barycentric ratio
  // return is the closest point on triangle
  inline bool CalcClosestPointBetweenPointAndTriangle(
    const EVec3f& _p,
    const EVec3f& _x0,
    const EVec3f& _x1,
    const EVec3f& _x2,
    std::vector<float>& _barycentric)
  {
    // initialize
    _barycentric = std::vector<float>(3);

    // calc mat
    EMatXf mat(2, 2);
    const EVec3f d1 = _x1 - _x0;
    const EVec3f d2 = _x2 - _x0;
    const float d1_d1 = d1.dot(d1);
    const float d1_d2 = d1.dot(d2);
    const float d2_d2 = d2.dot(d2);
    mat << d1_d1, d1_d2, d1_d2, d2_d2;

    // calc vec
    EVecXf vec(2);
    const EVec3f x0p = _p - _x0;
    const float x0p_d1 = x0p.dot(d1);
    const float x0p_d2 = x0p.dot(d2);
    vec << x0p_d1, x0p_d2;

    // solve formula
    const EVecXf sol = mat.fullPivLu().solve(vec);

    // output and return
    const float a = 1.0f - sol[0] - sol[1];
    const float b = sol[0];
    const float c = sol[1];
    if ((a > 0.0f) && (b > 0.0f) && (c > 0.0f))
    {
      _barycentric[0] = a;
      _barycentric[1] = b;
      _barycentric[2] = c;
      return true;
    }
    return false;
  }


  // calc Matrix product
  inline EMatXf CalcPro(const EMatXf& _a, const EMatXf& _b)
  {
    const int a_rows = static_cast<const int>(_a.rows());
    const int a_cols = static_cast<const int>(_a.cols());
    const int b_rows = static_cast<const int>(_b.rows());
    const int b_cols = static_cast<const int>(_b.cols());

    if (a_cols != b_rows) exit(1);

    EMatXf c = EMatXf(a_rows, b_cols).setZero();
#pragma omp parallel for 
    for (int i = 0; i < a_rows; ++i)
    {
      for (int j = 0; j < b_cols; ++j)
      {
        for (int k = 0; k < a_cols; ++k)
        {
          c(i, j) += _a(i, k) * _b(k, j);
        }
      }
    }

    return c;
  }
}