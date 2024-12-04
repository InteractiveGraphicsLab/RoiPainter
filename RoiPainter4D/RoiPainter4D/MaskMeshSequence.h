#pragma once
#pragma unmanaged

#include <memory>
#include <GL/glew.h>
#include "tmesh.h"
#include "thandle3d.h"
#include "ImageCore.h"


class MaskMeshSequence
{
private:
  //std::vector<byte*>& m_masks;
  //std::vector<std::unique_ptr<short[]>> m_masks;
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
  //MaskMeshSequence(const TMesh&);
  ~MaskMeshSequence();

  //MaskMeshSequence& operator=(const MaskMeshSequence&);

  bool LoadMask(const int);
  void UpdateMask(const int);
  //bool SaveMask();
  //bool LoadState();
  //bool SaveState();

  TMesh& GetMesh(const int);

  bool DrawMesh(const int);
};
