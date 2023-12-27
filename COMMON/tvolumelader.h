#ifndef COMMON_VOLUMELOADER_H_
#define COMMON_VOLUMELOADER_H_
#pragma unmanaged

#include "tmath.h"


bool ReadMha3d(
  const std::string &fname,
  EVec3i& reso,
  EVec3f& pitch,
  std::unique_ptr<byte[]>& volume_b,
  std::unique_ptr<short[]>& volume_s
);

bool WriteMha3d(
  const std::string &fname,
  const EVec3i& reso,
  const EVec3f& pitch,
  const byte* volume_b,// 0 when use volume_short
  const short* volume_s // 0 when use volume_byte
);

template<class T>
bool ReadTraw3d(
  const std::string &fname,
  EVec3i& reso,
  EVec3f& pitch,
  T*& img);

template<class T>
bool WriteTraw3d(
  const std::string &fname,
  const EVec3i& reso,
  const EVec3f& pitch,
  const T* img);


void GenDefaultVolume(
  const float trans_x,
  const float theta,
  EVec3i& reso,
  EVec3f& pitch,
  short*& img
);




//////////////////////////////////////////////
// 4DCT Loaders ///////////////////////////////
bool Open4DImg_traw3d(
  const std::vector<std::string>& fnames,

  EVec3i& reso,
  EVec3f& pitch,
  std::vector<short*>& img4D
);

bool Open4DImg_mha(
  const std::vector<std::string>& fnames,
  EVec3i& reso,
  EVec3f& pitch,
  std::vector<short*>& img4D
);

bool Open4DImg_raw8bit(
  const std::vector<std::string>& fnames,
  EVec3i& reso,
  EVec3f& pitch,
  std::vector<short*>& img4D
);

bool Open4DImg_dcm2Ds(
  std::vector<std::vector<std::string>> filesInDirs,
  EVec3i& reso,
  EVec3f& pitch,
  std::vector<short*>& img4D
);

bool Open4DImg_dcm3Ds(
  const std::vector<std::string>& fnames,
  EVec3i& reso,
  EVec3f& pitch,
  std::vector< short* >& img4D
);



#endif