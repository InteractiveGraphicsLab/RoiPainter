#pragma unmanaged
#include "OglImage.h"
#include "tqueue.h"
#include "timageloader.h"





// voxel value (input/output)
//  0:never change 
//  1:background
//255:foreground
void Erode3D(const int& W, const int& H, const int& D, byte* vol)
{
  const int WH = W * H, WHD = W * H * D;

  for (int z = 0; z < D; ++z)
  {
    for (int y = 0; y < H; ++y)
    {
      for (int x = 0; x < W; ++x)
      {
        int idx = x + y * W + z * WH;
        if (vol[idx] != 255) continue;

        if (x == 0 || y == 0 || z == 0 || x == W - 1 || y == H - 1 || z == D - 1 ||
          vol[idx - 1] <= 1 || vol[idx - W] <= 1 || vol[idx - WH] <= 1 ||
          vol[idx + 1] <= 1 || vol[idx + W] <= 1 || vol[idx + WH] <= 1)
        {
          vol[idx] = 2;
        }
      }
    }
  }

  for (int i = 0; i < WHD; ++i) if (vol[i] == 2) vol[i] = 1;
}


//voxel value (input/output)
//  0:never change 
//  1:background
//255:foreground
inline void Dilate3D(const int& W, const int& H, const int& D, byte* vol)
{
  const int WH = W * H, WHD = W * H * D;

  for (int z = 0; z < D; ++z)
  {
    for (int y = 0; y < H; ++y)
    {
      for (int x = 0; x < W; ++x)
      {
        int idx = x + y * W + z * WH;
        if (vol[idx] != 1) continue;

        if ((x > 0 && vol[idx - 1] == 255) || (x < W - 1 && vol[idx + 1] == 255) ||
          (y > 0 && vol[idx - W] == 255) || (y < H - 1 && vol[idx + W] == 255) ||
          (z > 0 && vol[idx - WH] == 255) || (z < D - 1 && vol[idx + WH] == 255))
        {
          vol[idx] = 2;
        }
      }
    }
  }
  for (int i = 0; i < WHD; ++i) if (vol[i] == 2) vol[i] = 255;
}


//voxel value (input/output)
//  0 : background
//255 : foreground
inline void FillHole3D(const int& W, const int& H, const int& D, byte* vol)
{
  const int WH = W * H, WHD = W * H * D;
  TQueue<EVec3i> Q(WHD / 3);

  for (int y = 0; y < H; ++y)
    for (int x = 0; x < W; ++x)
    {
      if (vol[x + y * W + 0 * WH] == 0) Q.push_back(EVec3i(x, y, 0));
      if (vol[x + y * W + (D - 1) * WH] == 0) Q.push_back(EVec3i(x, y, (D - 1)));
    }

  for (int z = 0; z < D; ++z)
    for (int y = 0; y < H; ++y)
    {
      if (vol[0 + y * W + z * WH] == 0) Q.push_back(EVec3i(0, y, z));
      if (vol[(W - 1) + y * W + z * WH] == 0) Q.push_back(EVec3i(W - 1, y, z));
    }

  for (int z = 0; z < D; ++z)
    for (int x = 0; x < W; ++x)
    {
      if (vol[x + 0 * W + z * WH] == 0) Q.push_back(EVec3i(x, 0, z));
      if (vol[x + (H - 1) * W + z * WH] == 0) Q.push_back(EVec3i(x, H - 1, z));
    }

  for (int i = 0; i < Q.size(); ++i) vol[Q[i].x() + Q[i].y() * W + Q[i].z() * WH] = 2;

  //region growing for background
  while (!Q.empty())
  {
    const int x = Q.front().x();
    const int y = Q.front().y();
    const int z = Q.front().z();
    const int I = x + y * W + z * WH;
    Q.pop_front();

    if (x != 0 && !vol[I - 1]) { vol[I - 1] = 2; Q.push_back(EVec3i(x - 1, y, z)); }
    if (x != W - 1 && !vol[I + 1]) { vol[I + 1] = 2; Q.push_back(EVec3i(x + 1, y, z)); }
    if (y != 0 && !vol[I - W]) { vol[I - W] = 2; Q.push_back(EVec3i(x, y - 1, z)); }
    if (y != H - 1 && !vol[I + W]) { vol[I + W] = 2; Q.push_back(EVec3i(x, y + 1, z)); }
    if (z != 0 && !vol[I - WH]) { vol[I - WH] = 2; Q.push_back(EVec3i(x, y, z - 1)); }
    if (z != D - 1 && !vol[I + WH]) { vol[I + WH] = 2; Q.push_back(EVec3i(x, y, z + 1)); }
  }
  for (int i = 0; i < WHD; ++i) vol[i] = (vol[i] == 2) ? 0 : 255;
}


//voxel value 0:never change, 1:background, 255:foreground
void Erode3D(OglImage3D& v)
{
  Erode3D(v.GetW(), v.GetH(), v.GetD(), v.GetVolumePtr());
  v.SetUpdated();
}

//voxel value 0:never cahnge, 1:background, 255:foreground
void Dilate3D(OglImage3D& v)
{
  Dilate3D(v.GetW(), v.GetH(), v.GetD(), v.GetVolumePtr());
  v.SetUpdated();
}

//voxel value 0: background, 255:foreground
void FillHole3D(OglImage3D& v)
{
  FillHole3D(v.GetW(), v.GetH(), v.GetD(), v.GetVolumePtr());
  v.SetUpdated();
}


void Erode3D(const EVec3i& reso, byte* vol)
{
  Erode3D(reso[0], reso[1], reso[2], vol);
}

void Dilate3D(const EVec3i& reso, byte* vol)
{
  Dilate3D(reso[0], reso[1], reso[2], vol);
}

void FillHole3D(const EVec3i& reso, byte* vol)
{
  FillHole3D(reso[0], reso[1], reso[2], vol);
}





void OglImage2D<CH_RGBA>::SendImageToGPU()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution[0], m_resolution[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image);
}

void OglImage2D<CH_INTES>::SendImageToGPU()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, m_resolution[0], m_resolution[1], 0, GL_LUMINANCE8, GL_UNSIGNED_BYTE, m_image);
}

bool OglImage2D<CH_RGBA>::Allocate(const char* fname)
{
  if (m_image != nullptr) delete[] m_image;
  m_name_gpu = -1;
  m_image = nullptr;
  m_is_updated = true;
  return LoadImage(fname, m_resolution[0], m_resolution[1], m_image);
}


bool OglImage2D<CH_RGBA>::SaveAs(const char* fname)
{
  SaveImage(fname, m_resolution[0], m_resolution[1], m_image);
  return true;
}

bool OglImage2D<CH_INTES>::SaveAs(const char* fname)
{
  SaveImageGray(fname, m_resolution[0], m_resolution[1], m_image);
  return true;
}


void OglImage2D<CH_RGBA>::SetGrayValue_normalize(float* image)
{
  const int W = m_resolution[0];
  const int H = m_resolution[1];
  int N = W * H;
  float minV = +FLT_MAX;
  float maxV = -FLT_MAX;

  for (int i = 0; i < N; ++i) {
    minV = std::min(minV, image[i]);
    maxV = std::max(maxV, image[i]);
  }
  for (int i = 0; i < N; ++i) {
    float v = (image[i] - minV) / (maxV - minV);
    m_image[4 * i + 0] = m_image[4 * i + 1] =
      m_image[4 * i + 2] = m_image[4 * i + 3] = (byte)(255 * v);
  }
}

void OglImage2D<CH_RGBA>::SetIntensity(
  const int x,
  const int y,
  byte c)
{
  int i = 4 * (x + y * m_resolution[0]);
  m_image[i] = m_image[i + 1] = m_image[i + 2] = c;
}










void OglImage1D<CH_RGBA>::SendImageToGPU()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, m_resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image);
}

void OglImage1D<CH_INTES>::SendImageToGPU()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE8, m_resolution, 0, GL_LUMINANCE8, GL_UNSIGNED_BYTE, m_image);
}

void OglImage1D<CH_INTES>::SetZero()
{
  m_is_updated = true;
  memset(m_image, 0, sizeof(byte) * m_resolution);
}

void OglImage1D<CH_RGBA>::SetZero()
{
  m_is_updated = true;
  memset(m_image, 0, sizeof(byte) * m_resolution * 4);
}


void OglImage1D<CH_RGBA>::Allocate(const char* fname)
{
  if (m_image != nullptr) delete[] m_image;
  m_image = nullptr;
  m_resolution = 0;

  int W, H;
  GLubyte* rgba;
  if (LoadImage(fname, W, H, rgba))
  {
    Allocate(W);
    for (int i = 0; i < W; ++i) 
      memcpy(&m_image[CH_RGBA * i], &rgba[CH_RGBA * i], sizeof(byte) * CH_RGBA);
    delete[] rgba;
  }
}
