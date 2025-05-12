#pragma once
#pragma unmanaged

#include <memory>
#include <GL/glew.h>
#include "tmesh.h"
#include "thandle3d.h"


class MaskMeshSequence
{
private:
  int m_num_frames;
  int m_num_voxels;
  EVec3i m_reso;
  EVec3f m_pitch;

  std::vector<TMesh> m_meshes;

  static const std::string m_vtxshader_fname;
  static const std::string m_frgshader_fname;
  static GLuint m_gl2Program;

public:
  bool is_initialized;

  MaskMeshSequence();
  ~MaskMeshSequence();

  bool LoadMask(const int);
  void UpdateMask();

  TMesh& GetMesh(const int);

  bool DrawMesh(const int);

};
