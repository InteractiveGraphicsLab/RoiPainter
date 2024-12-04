#pragma once

#include <Core>
#include <Sparse>
#include <Dense>
#include <vector>
#include "tmesh.h"


public class LaplacianDeformer
{
public:
  TMesh& m_mesh;

  LaplacianDeformer(TMesh&);
  void Initialize(TMesh&);
  void Deform(const std::vector<int>&, const std::vector<Eigen::Vector3f>&);

private:
  int m_num_verts;
  int m_num_fixed_verts;
  int m_num_free_verts;
  int m_times;
  bool m_is_set_solver;

  std::vector<int> m_fixed_verts_idxs;

  float m_coef;
  float m_coef_weak;
  Eigen::MatrixXf m_verts;
  Eigen::MatrixXf m_delta;
  Eigen::SparseMatrix<float> m_laplacian;
  Eigen::SparseMatrix<float> m_laplacian_transpose;
  std::unique_ptr<Eigen::SimplicialLLT<Eigen::SparseMatrix<float>>> m_solver;

  void SetSolver(const std::vector<int>&);
  void Solve(const std::vector<Eigen::Vector3f>&);
  void ConstructLaplacian(std::vector<Eigen::Triplet<float>>&);
};
