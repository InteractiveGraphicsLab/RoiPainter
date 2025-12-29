#include "LaplacianSurfaceEditing.h"

//#define DEBUG_LAPLACIAN_DEFORMER


static const float COEF_FIXED = 1.0f;
static const float COEF_WEAK = 0.0001f;





// Extrat {i, j, coef}
static void ConstructLaplacian(
  const TMesh& mesh,
  std::vector<Eigen::Triplet<float>>& _triplets)
{
  for (int i = 0; i < mesh.m_vSize; ++i) 
  {
    const std::vector<int>& neighbors = mesh.m_vRingVs[i];
    const int degree = static_cast<int>(neighbors.size());
    if (degree == 0) continue;

    for (int neighbor : neighbors) 
    {
      _triplets.emplace_back(i, neighbor, -1.0f / static_cast<float>(degree));
    }
    _triplets.emplace_back(i, i, 1.0f);
  }
}



void LaplacianDeformer::Deform(
  TMesh& mesh,
  const std::vector<int>& const_vids,
  const std::vector<Eigen::Vector3f>& const_trgtpos)
{
  if ( const_vids.size() != const_trgtpos.size() ) return;


  std::cout << "STEP 1 CONSTRUCT LAPLACIAN MATRIX ..\n";
  const int num_verts = mesh.m_vSize;

  std::vector<bool  > verts_fixed(num_verts, false);
  std::vector<EVec3f> verts_trgt (num_verts, EVec3f());

  for (size_t i = 0; i < const_vids.size(); ++i)
  {
    if (const_vids[i] >= num_verts) 
    {
      std::cout << "WARNING: Deform\n";
      continue;
    }
    verts_fixed[const_vids[i]] = true;
    verts_trgt [const_vids[i]] = const_trgtpos[i];
  }

  //Create Laplacian Matrix
  Eigen::SparseMatrix<float> laplacian, laplacian_transpose;
  laplacian = Eigen::SparseMatrix<float>(num_verts, num_verts);
  std::vector<Eigen::Triplet<float>> laplacian_triplets = std::vector<Eigen::Triplet<float>>();
  ConstructLaplacian(mesh, laplacian_triplets);
  laplacian.setFromTriplets(laplacian_triplets.begin(), laplacian_triplets.end());
  laplacian.makeCompressed();
  laplacian_transpose = laplacian.transpose();
  laplacian_transpose.makeCompressed();

  // matA
  Eigen::SparseMatrix<float> matA = laplacian_transpose * laplacian;
  for (int i = 0; i < num_verts; ++i)
  {
    matA.coeffRef(i, i) += (verts_fixed[i]) ? COEF_FIXED : COEF_WEAK;
  }

  matA.makeCompressed();

  std::cout << "STEP 2 PREPARE ORIGINAL LAPLACIAN------------------\n";

  // verts & original laplacian vectors を Nx3 行列にまとめる
  Eigen::MatrixXf verts = Eigen::MatrixXf(num_verts, 3);
  for (int i = 0; i < num_verts; ++i)
    verts.row(i) = mesh.m_vVerts[i].transpose();

  Eigen::MatrixXf delta = laplacian * verts;


  std::cout << "STEP 4 PREPARE Solver------------------\n";
  std::unique_ptr<Eigen::SimplicialLLT<Eigen::SparseMatrix<float>>> solver;
  solver = std::make_unique<Eigen::SimplicialLLT<Eigen::SparseMatrix<float>>>();
  solver->compute(matA);


  std::cout << "STEP 5 SOLVE Ax = b -------------------\n";
  Eigen::MatrixXf b = laplacian_transpose * delta;
  
  for (int i = 0; i < num_verts; ++i)
  {
    if (verts_fixed[i]) b.row(i) += COEF_FIXED * verts_trgt[i].transpose();
    else b.row(i) += COEF_WEAK * verts.row(i);
  }

  // solve
  const Eigen::MatrixXf result = solver->solve(b);

  // update mesh
  for (int i = 0; i < num_verts; ++i)
  {
    mesh.m_vVerts[i] = result.row(i).transpose();
  }
  mesh.UpdateNormal();

  std::cout << "deformed...\n";
}

