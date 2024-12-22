#include "LaplacianSurfaceEditing.h"

//#define DEBUG_LAPLACIAN_DEFORMER


LaplacianDeformer::LaplacianDeformer(
  TMesh& _mesh) :
  m_mesh(_mesh),
  m_num_verts(_mesh.m_vSize),
  m_is_set_solver(false)
{

}


void LaplacianDeformer::Initialize(
  TMesh& _mesh)
{
  m_mesh = _mesh;
  m_num_verts = _mesh.m_vSize;
  m_is_set_solver = false;
}


void LaplacianDeformer::Deform(
  const std::vector<int>& _fixed_verts_idxs,
  const std::vector<Eigen::Vector3f>& _fixed_positions)
{
  std::cout << "start deform...\n";

  SetSolver(_fixed_verts_idxs);
  Solve(_fixed_positions);

  std::cout << "deformed...\n";
}


void LaplacianDeformer::SetSolver(
  const std::vector<int>& _fixed_verts_idxs)
{
  #ifdef DEBUG_LAPLACIAN_DEFORMER
    std::cout << "start SetSolver.\n";
  #endif

  m_coef = 1.0f;
  m_coef_weak = 0.0001f;

  m_fixed_verts_idxs = _fixed_verts_idxs;
  m_num_fixed_verts = static_cast<int>(m_fixed_verts_idxs.size());
  m_num_free_verts = m_num_verts - m_num_fixed_verts;

  m_verts = Eigen::MatrixXf(m_num_verts, 3);
  for (int i = 0; i < m_num_verts; ++i)
  {
    m_verts.row(i) = m_mesh.m_vVerts[i].transpose();
  }

  // laplacian
  m_laplacian = Eigen::SparseMatrix<float>(m_num_verts, m_num_verts);
  std::vector<Eigen::Triplet<float>> laplacian_triplets = std::vector<Eigen::Triplet<float>>();
  ConstructLaplacian(laplacian_triplets);
  m_laplacian.setFromTriplets(laplacian_triplets.begin(), laplacian_triplets.end());
  m_laplacian.makeCompressed();

  m_laplacian_transpose = m_laplacian.transpose();
  m_laplacian_transpose.makeCompressed();

  // delta
  m_delta = m_laplacian * m_verts;

  // matA
  Eigen::SparseMatrix<float> matA = m_laplacian_transpose * m_laplacian;
  int count = 0;
  for (int i = 0; i < m_num_verts; ++i)
  {
    if (i == m_fixed_verts_idxs[count])
    {
      matA.coeffRef(i, i) += m_coef;
      ++count;
    }
    else
    {
      matA.coeffRef(i, i) += m_coef_weak;
    }
  }
  matA.makeCompressed();

  // set solver
#ifdef DEBUG_LAPLACIAN_DEFORMER
  std::cout << "start compute.\n";
#endif
  m_solver = std::make_unique<Eigen::SimplicialLLT<Eigen::SparseMatrix<float>>>();
  m_solver->compute(matA);

  #ifdef DEBUG_LAPLACIAN_DEFORMER
    std::cout << "solver set.\n";
  #endif

  m_is_set_solver = true;
}


void LaplacianDeformer::Solve(
  const std::vector<Eigen::Vector3f>& _fixed_positions)
{
  if (!m_is_set_solver) {
    std::cout << "solver is not set.\n";
    return;
  }

  #ifdef DEBUG_LAPLACIAN_DEFORMER
    std::cout << "start Solve.\n";
  #endif

  // vecb
  Eigen::MatrixXf vecb = m_laplacian_transpose * m_delta;
  int count = 0;
  for (int i = 0; i < m_num_verts; ++i)
  {
    if (i == m_fixed_verts_idxs[count])
    {
      vecb.row(i) += m_coef * _fixed_positions[count].transpose();
      ++count;
    }
    else
    {
      vecb.row(i) += m_coef_weak * m_verts.row(i);
    }
  }

  // solve
  const Eigen::MatrixXf result = m_solver->solve(vecb);

#ifdef DEBUG_LAPLACIAN_DEFORMER
  std::cout << "solver info: " << m_solver->info() << "\n";
#endif

  // update mesh
  for (int i = 0; i < m_num_verts; ++i)
  {
    m_mesh.m_vVerts[i] = result.row(i).transpose();
  }
  m_mesh.UpdateNormal();

  #ifdef DEBUG_LAPLACIAN_DEFORMER
    std::cout << "solved.\n";
  #endif
}


void LaplacianDeformer::ConstructLaplacian(
  std::vector<Eigen::Triplet<float>>& _triplets)
{
  #ifdef DEBUG_LAPLACIAN_DEFORMER
    std::cout << "start ConstructLaplacian.\n";
  #endif

  for (int i = 0; i < m_num_verts; ++i) {
    const std::vector<int>& neighbors = m_mesh.m_vRingVs[i];
    const int degree = static_cast<int>(neighbors.size());
    for (int neighbor : neighbors) {
      _triplets.emplace_back(i, neighbor, -1.0f / static_cast<float>(degree));
    }
    _triplets.emplace_back(i, i, 1.0f);
  }
}
