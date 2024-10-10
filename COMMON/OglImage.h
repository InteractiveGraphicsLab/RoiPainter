#ifndef COMMON_OGLIMAGE_H_
#define COMMON_OGLIMAGE_H_
#pragma unmanaged

#include "gl/glew.h"
#include <gl/gl.h> 
#include <gl/glu.h> 
#include <iostream> 
#include "tmath.h"

enum OGL_IMAGE_CH
{
  CH_INTES = 1,
  CH_RGBA = 4
};

/*
template<class T>
void t_FlipVolumeInZ(const int W, const int H, const int D, T* vol)
{
  const int WH = W * H;

  T* tmp = new T[WH];

  for (int z = 0; z < D / 2; ++z)
  {
    memcpy(tmp         , &vol[z * WH]          , sizeof(T) * WH);
    memcpy(&vol[z * WH], &vol[(D - 1 - z) * WH], sizeof(T) * WH);
    memcpy(&vol[(D - 1 - z) * WH], tmp         , sizeof(T) * WH);
  }
  delete[] tmp;
}
*/


//voxel value (input/output)
//  0:never change 
//  1:background
//255:foreground
void Erode3D(const int& W, const int& H, const int& D, byte* vol);
void Dilate3D(const int& W, const int& H, const int& D, byte* vol);
void Erode3D(const EVec3i& reso, byte* vol);
void Dilate3D(const EVec3i& reso, byte* vol);


//voxel value (input/output)
//  0 : background
//255 : foreground
void FillHole3D(const int& W, const int& H, const int& D, byte* vol);
void FillHole3D(const EVec3i& reso, byte* vol);




// single channel volume image
// Note oglBindについて
//   - bindのタイミングで bUpdated == trueなら unbindする
//   - unbindを忘れるとGRAMでleakするので注意
class OglImage3D
{
private:
  GLuint   m_name_gpu;
  EVec3i   m_resolution;
  bool     m_is_updated;
  GLubyte *m_volume;

public:
  ~OglImage3D()
  {
    if (m_volume != nullptr) delete[] m_volume;
  }

  OglImage3D()
  {
    m_volume = nullptr;
    m_name_gpu   = -1;
    m_is_updated = true;
    m_resolution = EVec3i(0, 0, 0);
  }

  OglImage3D(const OglImage3D &src)
  {
    m_volume = nullptr;
    Set(src);
  }

  OglImage3D &operator=(const OglImage3D &src)
  {
    Set(src);
    return *this;
  }

private:
  void Set(const OglImage3D &src)
  {
    m_resolution = src.m_resolution;
    if (src.m_volume != nullptr)
    {
      Allocate(m_resolution);
      int N = m_resolution[0] * m_resolution[1] * m_resolution[2];
      memcpy(m_volume, src.m_volume, sizeof(GLubyte) * N);
    }
  }

public:
  void Allocate(const int W, const int H, const int D)
  {
    if (m_volume != nullptr) delete[] m_volume;
    m_resolution = EVec3i(W, H, D);
    m_volume     = new GLubyte[m_resolution[0] * m_resolution[1] * m_resolution[2]];
    m_is_updated = true;
  }

  void Allocate(const EVec3i reso)
  {
    Allocate(reso[0], reso[1], reso[2]);
  }

  //should be wglMakeCurrent
  void BindOgl(bool bInterpolate = true)
  {
    if (m_is_updated)
    {
      UnbindOgl();
      m_is_updated = false;
    }

    if (m_name_gpu == -1 || !glIsTexture(m_name_gpu))
    {
      //generate textrue
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      glGenTextures(1, &m_name_gpu);

      glBindTexture(GL_TEXTURE_3D, m_name_gpu);
      glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE8, 
                    m_resolution[0], m_resolution[1], m_resolution[2], 0, 
                    GL_LUMINANCE,  GL_UNSIGNED_BYTE, m_volume);
    }
    else
    {
      //use the previous texture on GPU
      glBindTexture(GL_TEXTURE_3D, m_name_gpu);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    GLint param = bInterpolate ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, param);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, param);
  }

  //should be wglMakeCurrent
  void UnbindOgl()
  {
    if (m_name_gpu != -1 && glIsTexture(m_name_gpu)) 
      glDeleteTextures(1, &m_name_gpu);
    m_name_gpu = -1;
  }

  // should be allocated & the size of v is samge as m_res
  template<class T>
  void SetValue(const T* v, const T minV, const T maxV)
  {
    if (m_volume == nullptr) return;

    float rate = 255.0f / (maxV - minV);
    int N = m_resolution[0] * m_resolution[1] * m_resolution[2];

#pragma omp parallel for 
    for (int i = 0; i < N; ++i) 
      m_volume[i] = (byte)std::max(0.f, std::min(255.f, (v[i] - minV) * rate));

    m_is_updated = true;
  }

  void SetValue(byte *v)
  {
    if (m_volume == nullptr) return;
    const int N = m_resolution[0] * m_resolution[1] * m_resolution[2];
    memcpy(m_volume, v, sizeof(byte) * N);
    m_is_updated = true;
  }

  void SetValue(float** slices, const float minV, const float maxV)
  {
    if (m_volume == nullptr) return;

    float rate = 255.0f / (maxV - minV);
    int WH = m_resolution[0] * m_resolution[1];

    for (int z = 0; z < m_resolution[2]; ++z)
    {
      for (int i = 0; i < WH; ++i)
      {
        m_volume[z * WH + i] = 
          (byte) std::max(0.f, std::min(255.f, (slices[z][i] - minV) * rate));
      }
    }

    m_is_updated = true;
  }

  void SetAllZero()
  {
    const int N = m_resolution[0] * m_resolution[1] * m_resolution[2];
    memset(m_volume, 0, sizeof(GLubyte) * N);
    m_is_updated = true;
  }

  inline GLubyte& operator[](int i) { return m_volume[i]; }
  inline GLubyte  operator[](int i) const { return m_volume[i]; }

  inline GLubyte GetV(const int x, const int y, const int z) const
  {
    return m_volume[x + y * m_resolution[0] + z * m_resolution[0] * m_resolution[1]];
  }

  inline GLubyte SetV(const int x, const int y, const int z, GLubyte v)
  {
    m_volume[x + y * m_resolution[0] + z * m_resolution[0] * m_resolution[1]] = v;
    m_is_updated = true;
  }

  inline void SetUpdated() { m_is_updated = true; }

  const int GetW() const { return m_resolution[0]; }
  const int GetH() const { return m_resolution[1]; }
  const int GetD() const { return m_resolution[2]; }
  GLubyte* GetVolumePtr() { return m_volume; }

  void FlipInZ() {
    FlipVolumeInZ<GLubyte>(m_resolution, m_volume);
  }
};




//2D image 
template <OGL_IMAGE_CH CH>
class OglImage2D
{
protected:
  GLubyte *m_image;
  GLuint   m_name_gpu;
  EVec2i   m_resolution;
  bool     m_is_updated;

public:
  ~OglImage2D()
  {
    if (m_image != nullptr) delete[] m_image;
    m_image = nullptr;
  }

  OglImage2D()
  {
    m_resolution << 0, 0;
    m_image = nullptr;
    m_name_gpu   = -1;
    m_is_updated = true;
  }

  OglImage2D(const OglImage2D &src)
  {
    m_image = nullptr;
    Set(src);
  }

  OglImage2D &operator=(const OglImage2D &src)
  {
    Set(src);
    return *this;
  }
  inline GLubyte& operator[](int i) { return m_image[i]; }
  inline GLubyte  operator[](int i) const { return m_image[i]; }

  inline int GetW() const { return m_resolution[0]; }
  inline int GetH() const { return m_resolution[1]; }

private:
  void Set(const OglImage2D &src)
  {
    m_resolution = src.m_resolution;
    //m_oglName = src.m_oglName; コピーしない
    if (src.m_image != nullptr)
    {
      Allocat(m_resolution);
      int N = m_resolution[0] * m_resolution[1] * CH;
      memcpy(m_image, src.m_image, sizeof(GLubyte) * N);
    }
  }

public:
  void Allocate(const int W, const int H)
  {
    if (m_image != nullptr) delete[] m_image;
    m_resolution << W, H;
    m_image = new GLubyte[m_resolution[0] * m_resolution[1] * CH];
    m_is_updated = true;
  }

  //bmp/png/tif/jpeg (only 24 bit color image) are supported
  //画像左上が原点となるので、OpenGL利用の際はflipする
  bool Allocate(const char *fname);
  bool SaveAs(const char *fname);


  //should be wglMakeCurrent
  void UnbindOgl()
  {
    if (m_name_gpu != -1 && glIsTexture(m_name_gpu)) glDeleteTextures(1, &m_name_gpu);
    m_name_gpu = -1;
  }


  //should be wglMakeCurrent
  void BindOgl(bool bInterpolate = true)
  {
    if (m_is_updated)
    {
      UnbindOgl();
      m_is_updated = false;
    }

    if (m_name_gpu == -1 || !glIsTexture(m_name_gpu))
    {
      //generate textrue
      glGenTextures(1, &m_name_gpu);
      glBindTexture(GL_TEXTURE_2D, m_name_gpu);
      SendImageToGPU();
    }
    else
    {
      //use the previous texture on GPU
      glBindTexture(GL_TEXTURE_2D, m_name_gpu);
    }

    //CHに異存、ここにあったらだめ　glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    GLint param = bInterpolate ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
  }

  void FlipInY()
  {
    const int W = m_resolution[0];
    const int H = m_resolution[1];
    byte *tmp = new byte[W * H * CH];
    for (int y = 0; y < H; ++y) memcpy(&tmp[CH*(H - 1 - y)*W], &m_image[4 * y* W], sizeof(byte) * CH * W);
    memcpy(m_image, tmp, sizeof(byte) * W * H * CH);
    delete[] tmp;
    m_is_updated = true;
  }

  void SetGrayValue_normalize( float* image );
  void SetIntensity(const int x, const int y, byte c);

  bool IsAllocated() { return m_image == nullptr ? false : true; }
private:
  void SendImageToGPU();
};


// rgba 1D image 
template <OGL_IMAGE_CH CH>
class OglImage1D
{
protected:
  GLubyte *m_image;
  GLuint   m_name_gpu;
  int      m_resolution;
  bool     m_is_updated;

public:
  ~OglImage1D()
  {
    if (m_image != nullptr) delete[] m_image;
  }

  OglImage1D()
  {
    m_resolution = 0;
    m_image = nullptr;
    m_name_gpu   = -1;
    m_is_updated = true;
  }

  OglImage1D(const OglImage1D &src)
  {
    m_image = nullptr;
    Set(src);
  }

  OglImage1D &operator=(const OglImage1D &src)
  {
    Set(src);
    return *this;
  }

private:
  void Set(const OglImage1D &src)
  {
    m_resolution = src.m_resolution;
    // m_oglName = src.m_oglName;
    if (src.m_image != nullptr)
    {
      Allocate(m_resolution);
      memcpy(m_image, src.m_image, sizeof(GLubyte) * m_resolution * CH);
    }
  }

public:
  void Allocate(const int N)
  {
    if (m_image != nullptr) delete[] m_image;
    m_resolution = N;
    m_image = new GLubyte[m_resolution * CH];
  }

  void Allocate(const char* fname);

  void AllocateHeuImg(const int N)
  {
    const float S = N / 6.0f;
    Allocate(N);
    for (int i = 0; i < N; ++i)
    {
      EVec3f C;
      if (i < 1 * S) C << 1, (i - 0 * S) / S, 0;
      else if (i < 2 * S) C << 1 - (i - 1 * S) / S, 1, 0;
      else if (i < 3 * S) C << 0, 1, (i - S * 2) / S;
      else if (i < 4 * S) C << 0, 1 - (i - S * 3) / S, 1;
      else if (i < 5 * S) C << (i - 4 * S) / S, 0, 1;
      else if (i < 6 * S) C << 1, 0, 1 - (i - S * 3) / S;
      m_image[4 * i] = (byte)(C[0] * 255);
      m_image[4 * i + 1] = (byte)(C[1] * 255);
      m_image[4 * i + 2] = (byte)(C[2] * 255);
    }

  }

  //should be wglMakeCurrent
  void UnbindOgl()
  {
    if (m_name_gpu != -1 && glIsTexture(m_name_gpu)) glDeleteTextures(1, &m_name_gpu);
    m_name_gpu = -1;
  }

  //should be wglMakeCurrent
  void BindOgl(bool bInterpolate = true)
  {
    if (m_is_updated)
    {
      UnbindOgl();
      m_is_updated = false;
    }

    if (m_name_gpu == -1 || !glIsTexture(m_name_gpu))
    {
      //generate textrue
      glGenTextures(1, &m_name_gpu);
      glBindTexture(GL_TEXTURE_1D, m_name_gpu);
      SendImageToGPU();
    }
    else
    {
      //use the previous texture on GPU
      glBindTexture(GL_TEXTURE_1D, m_name_gpu);
    }

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    GLint param = bInterpolate ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, param);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, param);
  }

  inline GLubyte& operator[](int i) { return m_image[i]; }
  inline GLubyte  operator[](int i) const { return m_image[i]; }

  void SetZero();
  void SetUpdated() { m_is_updated = true; }
  int GetResolution() { return m_resolution; }
private:
  void SendImageToGPU();
};



typedef OglImage1D<CH_RGBA > OGLImage1D4;
typedef OglImage1D<CH_INTES> OGLImage1D1;
typedef OglImage2D<CH_RGBA > OGLImage2D4;
typedef OglImage2D<CH_INTES> OGLImage2D1;



//voxel value 0:never change, 1:background, 255:foreground
void Erode3D(OglImage3D& v);

//voxel value 0:never cahnge, 1:background, 255:foreground
void Dilate3D(OglImage3D& v);

//voxel value 0: background, 255:foreground
void FillHole3D(OglImage3D& v);



template<class T1, class T2>
void Sobel3D(const int W, const int H, const int D, const T1* vol, T2* res)
{
  const T1 sblX[3][3][3] = {
    { {-1,  0, +1},
      {-2,  0, +2},
      {-1,  0, +1} }, { {-2,  0,  2},
                        {-4,  0,  4},
                        {-2,  0,  2} }, { {-1,  0,  1},
                                          {-2,  0,  2},
                                          {-1,  0,  1}} };
  static T1 sblY[3][3][3] = {
    {{-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}},   {{-2, -4, -2},
                     { 0,  0,  0},
                     { 2,  4,  2}}, {{-1, -2, -1},
                                    { 0,  0,  0},
                                    { 1,  2,  1}} };
  static T1 sblZ[3][3][3] = {
    {{-1, -2, -1},
    {-2, -4, -2},
    {-1, -2, -1}},  {{ 0,  0,  0},
                     { 0,  0,  0},
                     { 0,  0,  0}}, {{ 1,  2,  1},
                                     { 2,  4,  2},
                                     { 1,  2,  1}} };

  const int WH = W * H, WHD = WH * D;

#pragma omp parallel for
  for (int i = 0; i < WHD; ++i)
  {
    const int z = (i) / WH;
    const int y = (i - z * WH) / W;
    const int x = (i - z * WH - y * W);

    double gx = 0, gy = 0, gz = 0;

    for (int zz = -1; zz < 2; ++zz) if (0 <= z + zz && z + zz < D)
      for (int yy = -1; yy < 2; ++yy) if (0 <= y + yy && y + yy < H)
        for (int xx = -1; xx < 2; ++xx) if (0 <= x + xx && x + xx < W)
        {
          int I = i + xx + yy * W + zz * WH;
          gx += sblX[zz + 1][yy + 1][xx + 1] * vol[I];
          gy += sblY[zz + 1][yy + 1][xx + 1] * vol[I];
          gz += sblZ[zz + 1][yy + 1][xx + 1] * vol[I];
        }

    //for boundary voxels
    if (x == 0 || x == W - 1) gx = 0;
    if (y == 0 || y == H - 1) gy = 0;
    if (z == 0 || z == D - 1) gz = 0;
    res[i] = (T2)sqrt(gx * gx + gy * gy + gz * gz) / 16.0f;

    //if (res[i] > 40000) std::cout << x << " " << y << " " << z << "\n";
  }
}



#endif

