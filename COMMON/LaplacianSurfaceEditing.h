#pragma once

#include <Core>
#include <Sparse>
#include <Dense>
#include <vector>
#include "tmesh.h"


public class LaplacianDeformer
{

public:
  LaplacianDeformer(){}
  void Deform(
    TMesh &mesh,
    const std::vector<int>&    const_vids, 
    const std::vector<EVec3f>& const_trgtpos);

};

//int m_num_verts;
//int m_num_fixed_verts;
//int m_num_free_verts;
//bool m_is_set_solver;

//std::vector<int> m_fixed_verts_idxs;

//Eigen::MatrixXf m_verts;
//Eigen::MatrixXf m_delta;
//Eigen::SparseMatrix<float> m_laplacian;
//Eigen::SparseMatrix<float> m_laplacian_transpose;
//std::unique_ptr<Eigen::SimplicialLLT<Eigen::SparseMatrix<float>>> m_solver;
  //void SetSolver(const std::vector<int>&);
  //void Solve(const std::vector<Eigen::Vector3f>&);
  //void ConstructLaplacian(std::vector<Eigen::Triplet<float>>&);
