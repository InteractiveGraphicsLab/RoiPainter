#pragma unmanaged
#include "MaskMeshSequence.h"
#include "ImageCore.h"
#include "tmarchingcubes.h"
#include "./Mode/GlslShader.h"


const std::string MaskMeshSequence::m_vtxshader_fname = std::string("shader/cagemeshVtx.glsl");
const std::string MaskMeshSequence::m_frgshader_fname = std::string("shader/cagemeshFrg.glsl");
GLuint MaskMeshSequence::m_gl2Program = -1;


MaskMeshSequence::MaskMeshSequence():
  is_initialized(false)
{

}


MaskMeshSequence::~MaskMeshSequence()
{

}



bool MaskMeshSequence::LoadMask(const int _scale = 1)
{
  const int scale_num = (int)pow(_scale, 3);

  const int active_maskid = ImageCore::GetInst()->GetSelectMaskIdx();
  const std::vector<MaskData>& mask_data = ImageCore::GetInst()->GetMaskData();

  if (active_maskid < 0 || mask_data.size() <= active_maskid) return false;

  m_num_frames = ImageCore::GetInst()->GetNumFrames();
  m_num_voxels = ImageCore::GetInst()->GetNumVoxels();
  m_reso = ImageCore::GetInst()->GetReso();
  m_pitch = ImageCore::GetInst()->GetPitch();

  const int w = m_reso[0] / _scale;
  const int h = m_reso[1] / _scale;
  const int d = m_reso[2] / _scale;

  std::cout << "m_num_voxels: " << m_num_voxels << "\n";
  std::cout << "m_reso: " << m_reso << "\n";

  m_meshes = std::vector<TMesh>(m_num_frames);
  #pragma omp parallel for
  for (int frame_idx = 0; frame_idx < m_num_frames; ++frame_idx)
  {
    std::unique_ptr<short[]> v = std::make_unique<short[]>(m_num_voxels / scale_num);
    for (int z = 0; z < d; ++z) {
      for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
          float ave = 0;
          for (int c = 0; c < _scale; ++c) {
            for (int b = 0; b < _scale; ++b) {
              for (int a = 0; a < _scale; ++a) {
                const int idx = ((z * _scale + c) * m_reso[1] * m_reso[0]) + ((y * _scale + b) * m_reso[0]) + (x * _scale + a);
                ave += (ImageCore::GetInst()->m_mask4d[frame_idx][idx] == active_maskid) ? 1.0f : 0.0f;
              }
            }
          }
          ave /= powf(static_cast<float>(_scale), 3.0f);
          v[z * h * w + y * w + x] = ave >= 0.49f ? 255 : 0;

        }
      }
    }

    TMesh mesh;
    marchingcubes::MarchingCubes(m_reso / _scale, m_pitch, v.get(), 128, 0, 0, mesh);
    mesh.Scale(static_cast<float>(_scale));
    mesh.Smoothing(3);
    m_meshes[frame_idx] = mesh;
    v.reset();
  }

  is_initialized = true;
  return true;
}


void MaskMeshSequence::UpdateMask(const int _current_frame_idx)
{
  const int active_maskid = ImageCore::GetInst()->GetSelectMaskIdx();
  const std::vector<MaskData>& mask_data = ImageCore::GetInst()->GetMaskData();

  byte mask_locked[256] = {};
  for (int i = 0; i < (int)mask_data.size(); ++i)
  {
    mask_locked[i] = mask_data[i].lock ? 1 : 0;
  }
  mask_locked[active_maskid] = 0;

  #pragma omp parallel for
  for (int frame_idx = 0; frame_idx < m_num_frames; ++frame_idx)
  {
    byte* mask = ImageCore::GetInst()->m_mask4d[frame_idx];
    std::unique_ptr<byte[]> img = std::make_unique<byte[]>(m_num_voxels);

    m_meshes[frame_idx].GenBinaryVolume(m_reso, m_pitch, img.get());

    for (int i = 0; i < m_num_voxels; ++i)
    {
      if (!mask_locked[mask[i]])
      {
        if (img[i])
        {
          mask[i] = active_maskid;
        }
        else
        {
          if (mask[i] == active_maskid)
          {
            mask[i] = 0;
          }
        }
      }
    }
    img.reset();
  }

  ImageCore::GetInst()->m_vol_mask.SetValue(ImageCore::GetInst()->m_mask4d[_current_frame_idx]);
}


TMesh& MaskMeshSequence::GetMesh(const int _frame_idx)
{
  return m_meshes[_frame_idx];
}


bool MaskMeshSequence::DrawMesh(const int _frame_idx)
{
  if (_frame_idx < 0 || (int)m_meshes.size() <= _frame_idx) return false;

  if (m_gl2Program == -1)
  {
    t_InitializeShader(
      m_vtxshader_fname.c_str(),
      m_frgshader_fname.c_str(),
      m_gl2Program);
  }

  glUseProgram(m_gl2Program);
  //glUniform1f(glGetUniformLocation(m_gl2Program, "u_crssec_x_01"), planex01);
  //glUniform1f(glGetUniformLocation(m_gl2Program, "u_crssec_y_01"), planey01);
  //glUniform1f(glGetUniformLocation(m_gl2Program, "u_crssec_z_01"), planez01);
  //glUniform1f(glGetUniformLocation(m_gl2Program, "u_cuboid_w"), cuboid[0]);
  //glUniform1f(glGetUniformLocation(m_gl2Program, "u_cuboid_h"), cuboid[1]);
  //glUniform1f(glGetUniformLocation(m_gl2Program, "u_cuboid_d"), cuboid[2]);
  //glUniform1f(glGetUniformLocation(m_gl2Program, "u_opacity"), opacity);

  m_meshes[_frame_idx].Draw();
  glUseProgram(0);

  return true;
}
