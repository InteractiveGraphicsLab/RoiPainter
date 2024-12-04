#pragma unmanaged

#include "tvolumelader.h"
#include "../tdcmtk.h"
#include <zlib.h>


//mha header
//ObjectType = Image
//NDims = 3
//BinaryData = True
//BinaryDataByteOrderMSB = False
//CompressedData = False
//TransformMatrix = 1 0 0 0 1 0 0 0 1
//Offset = 0 0 0
//CenterOfRotation = 0 0 0
//AnatomicalOrientation = RAI
//ElementSpacing = 0.46800000000000003 0.46800000000000003 0.5
//DimSize = 512 512 320
//ElementType = MET_UCHAR
//ElementDataFile = LOCAL
bool ReadMha3d(
    const std::string &fname,
    EVec3i& reso,
    EVec3f& pitch,
    std::unique_ptr<byte[]>& volume_b,
    std::unique_ptr<short[]>& volume_s
)
{
  std::ifstream ifs(fname, std::ios_base::binary);
  if (!ifs)
  {
    std::cout << "can't open mha file " << fname << "\n";
    return false;
  }

  int dim = 0;
  std::string line;
  std::string elementType;
  bool isCompressed = false;

  while (std::getline(ifs, line))
  {
    std::istringstream iss(line);
    std::string key, equal;
    iss >> key >> equal;

    if (key == "DimSize") {
      iss >> reso[0] >> reso[1] >> reso[2];
    }
    else if (key == "ElementType") {
      iss >> elementType;
    }
    else if (key == "ElementSpacing") {
      iss >> pitch[0] >> pitch[1] >> pitch[2];
    }
    else if (key == "NDims") {
      iss >> dim;
      if (dim != 3) {
        std::cout << "ERROR this is not 3D volume data\n";
        ifs.close();
        return false;
      }
    }
    else if (line.find("ElementDataFile = LOCAL") != std::string::npos) {
      break;
    }
    else if (line.find("CompressedData = True") != std::string::npos) {
      isCompressed = true;
    }
  }

  const int WHD = reso[0] * reso[1] * reso[2];
  bool tf = false;

  if (isCompressed) {
    std::cout << "Compressed Mha.\n";
    std::vector<byte> compressedData;
    std::copy(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), std::back_inserter(compressedData));

    uLongf uncompressedSize = WHD * sizeof(byte);
    volume_b.reset(new byte[WHD]);

    int result = uncompress(volume_b.get(), &uncompressedSize, compressedData.data(), compressedData.size());
    tf = (result == Z_OK);
  }
  else
  {
    if (elementType == "MET_UCHAR") {
      volume_b.reset(new byte[WHD]);
      tf = static_cast<bool>(ifs.read(reinterpret_cast<char*>(volume_b.get()), sizeof(byte) * WHD));
    }
    else if (elementType == "MET_SHORT") {
      volume_s.reset(new short[WHD]);
      tf = static_cast<bool>(ifs.read(reinterpret_cast<char*>(volume_s.get()), sizeof(short) * WHD));
    }
  }
  ifs.close();
  return tf;
}


bool WriteMha3d(
    const std::string &fname,
    const EVec3i& reso,
    const EVec3f& pitch,
    const byte* volume_b,// 0 when use volume_short
    const short* volume_s // 0 when use volume_byte
)
{
  std::ofstream ofs(fname, std::ios_base::binary);
  if (!ofs)
  {
    std::cout << "can't open mha file " << fname << "\n";
    return false;
  }

  ofs << "ObjectType = Image\n";
  ofs << "NDims = 3\n";
  ofs << "BinaryData = True\n";
  ofs << "BinaryDataByteOrderMSB = False\n";
  ofs << "CompressedData = False\n";
  ofs << "TransformMatrix = 1 0 0 0 1 0 0 0 1\n";
  ofs << "Offset = 0 0 0\n";
  ofs << "CenterOfRotation = 0 0 0\n";

  ofs << "AnatomicalOrientation = RAI\n";
  ofs << "ElementSpacing  = " << pitch[0] << " " << pitch[1] << " " << pitch[2] << "\n";
  ofs << "DimSize = " << reso[0] << " " << reso[1] << " " << reso[2] << "\n";
  if (volume_b != 0) ofs << "ElementType = MET_UCHAR\n";
  else if (volume_s != 0) ofs << "ElementType = MET_SHORT\n";
  ofs << "ElementDataFile = LOCAL\n";

  const int WHD = reso[0] * reso[1] * reso[2];
  if (volume_b != 0)
  {
    ofs.write((char*)(volume_b), sizeof(byte) * WHD);
  }
  else if (volume_s != 0)
  {
    ofs.write((char*)(volume_s), sizeof(short) * WHD);
  }
  ofs.close();
  return true;
}


template<class T>
bool ReadTraw3d(
    const std::string &fname,
    EVec3i& reso,
    EVec3f& pitch,
    T*& img)
{
  FILE* fp = fopen(fname.c_str(), "rb");
  if (fp == 0) return false;

  int  W, H, D;
  double px, py, pz;
  fread(&W, sizeof(int), 1, fp);
  fread(&H, sizeof(int), 1, fp);
  fread(&D, sizeof(int), 1, fp);
  fread(&px, sizeof(double), 1, fp);
  fread(&py, sizeof(double), 1, fp);
  fread(&pz, sizeof(double), 1, fp);
  reso << W, H, D;
  pitch << (float)px, (float)py, (float)pz;

  img = new T[W * H * D];
  if (fread(img, sizeof(T), W * H * D, fp) != W * H * D)
  {
    fclose(fp);
    delete[] img;
    return false;
  }

  fclose(fp);
  return true;
}

//明示的インスタンス化
template bool ReadTraw3d(const std::string &fname, EVec3i& reso, EVec3f& pitch, short*& img);
template bool ReadTraw3d(const std::string &fname, EVec3i& reso, EVec3f& pitch, byte*& img);


template<class T>
bool WriteTraw3d(
    const std::string &fname,
    const EVec3i& reso,
    const EVec3f& pitch,
    const T* img)
{
  const int W = reso[0];
  const int H = reso[1];
  const int D = reso[2];
  const double px = pitch[0];
  const double py = pitch[1];
  const double pz = pitch[2];

  FILE* fp = fopen(fname.c_str(), "wb");
  if (fp == 0) return false;
  fwrite(&W, sizeof(int), 1, fp);
  fwrite(&H, sizeof(int), 1, fp);
  fwrite(&D, sizeof(int), 1, fp);
  fwrite(&px, sizeof(double), 1, fp);
  fwrite(&py, sizeof(double), 1, fp);
  fwrite(&pz, sizeof(double), 1, fp);
  fwrite(img, sizeof(T), W * H * D, fp);
  fclose(fp);
  return true;
}


//明示的インスタンス化
template bool WriteTraw3d(const std::string &fname, const EVec3i& reso, const EVec3f& pitch, const short* img);
template bool WriteTraw3d(const std::string &fname, const EVec3i& reso, const EVec3f& pitch, const byte* img);




bool Open4DImgTraw3D(
    const std::vector<std::string>& fnames,
    EVec3i& reso,
    EVec3f& pitch,
    std::vector<short*>& img4D)
{
  for (int idx = 0; idx < (int)fnames.size(); ++idx)
  {
    std::cout << "load " << idx << "/" << fnames.size();

    short* img = 0;
    EVec3i r;
    EVec3f p;
    if (!ReadTraw3d<short>(fnames[idx].c_str(), r, p, img)) continue;

    if (idx == 0)
    {
      reso << r;
      pitch << p;
    }
    img4D.push_back(img);
  }
  return true;
}


bool Open4DImgMha(
    const std::vector<std::string>& fnames,
    EVec3i& reso,
    EVec3f& pitch,
    std::vector<short*>& img4D)
{
  for (int idx = 0; idx < fnames.size(); ++idx)
  {
    EVec3f p;
    EVec3i r;
    std::unique_ptr<byte[]> data_b;
    std::unique_ptr<short[]> data_s;
    if (!ReadMha3d(fnames[idx].c_str(), r, p, data_b, data_s))
    {
      std::cout << "Fail to read mha files\n";
      return false;
    }

    if (idx == 0)
    {
      reso = r;
      pitch = p;
    }
    else if (reso[0] != r[0] || reso[1] != r[1] || reso[2] != r[2])
    {
      std::cout << "strange resolution\n";
      return false;
    }

    const int WHD = reso[0] * reso[1] * reso[2];
    short* img3d = new short[WHD];
    if (data_b != nullptr)
    {
      for (int i = 0; i < WHD; ++i) img3d[i] = (short)data_b[i];
    }
    else if (data_s != nullptr)
    {
      for (int i = 0; i < WHD; ++i) img3d[i] = data_s[i];
    }
    img4D.push_back(img3d);
  }

  return true;
}



bool Open4DImgRaw8bit(
    const std::vector<std::string>& fnames,
    EVec3i& reso,
    EVec3f& pitch,
    std::vector<short*>& img4D)
{
  // load 0th file to get file size
  long long int file_size = 0;
  {
    FILE* fp = fopen(fnames[0].c_str(), "rb");
    if (fp == NULL)
      return false;

    if (fseek(fp, 0L, SEEK_END) == 0) {
      fpos_t pos;
      if (fgetpos(fp, &pos) == 0)
        file_size = (long long int) pos;
    }
    fclose(fp);
  }

  int W = (int)cbrt(file_size);
  int H = W;
  int D = W;
  const int WHD = W * H * D;

  std::cout << "SYTEM LOAD FILE AS " << W << "x" << H << "x" << D << "volume\n\n";

  reso << W, H, D;
  pitch << 1.0, 1.0, 1.0;

  byte* vol = new byte[WHD];

  for (int idx = 0; idx < (int)fnames.size(); ++idx)
  {
    std::cout << "load " << idx << "/" << fnames.size();

    FILE* fp = fopen(fnames[idx].c_str(), "rb");
    if (fp == 0) continue;

    if (fread(vol, sizeof(byte), WHD, fp) != WHD)
    {
      std::cout << "failed to read " << idx << "volume\n";
      continue;
    }
    fclose(fp);

    img4D.push_back(new short[WHD]);
    short* trgt = img4D.back();
    for (int i = 0; i < WHD; ++i)
      trgt[i] = vol[i];
  }

  delete[] vol;
  return true;
}



bool Open4DImgDcm2Ds(
    std::vector<std::vector<std::string>> filesInDirs,
    EVec3i& reso,
    EVec3f& pitch,
    std::vector<short*>& img4D)
{
  for (int idx = 0; idx < (int)filesInDirs.size(); ++idx)
  {
    std::cout << "load " << idx << "/" << filesInDirs.size() << "  " << filesInDirs[idx].size() << "\n";
    std::cout << filesInDirs[idx][0].c_str() << "\n";


    std::vector<EVec3f> frame_pitchXY_zPos; // {(x,y,zSlizePos)}
    const int fileNum = (int)filesInDirs[idx].size();

    if (fileNum < 2) return false;

    for (int i = 0; i < fileNum; ++i)
    {
      //load dicoms (note fNum should be 1)
      Tdcmtk tdcmtk(filesInDirs[idx][i].c_str());

      int W, H, fNum;
      tdcmtk.getSize(W, H, fNum);

      if (i == 0)
      {
        reso << W, H, fileNum;
        img4D.push_back(new short[W * H * fileNum]); //initialize
      }

      if (reso[0] != W || reso[1] != H)
      {
        std::cout << "inconsistent error\n";
        return false;
      }

      frame_pitchXY_zPos.push_back(EVec3f((float)tdcmtk.getPitchX(), (float)tdcmtk.getPitchY(), (float)tdcmtk.getZPos()));

      if (!tdcmtk.getPixelsAs<short>(&img4D.back()[i * W * H]))
      {
        std::cout << "error = " << filesInDirs[idx][i].c_str() << "\n";
      }
    }

    pitch[0] = (frame_pitchXY_zPos[0][0] == -1.0f) ? 1 : frame_pitchXY_zPos[0][0];
    pitch[1] = (frame_pitchXY_zPos[0][1] == -1.0f) ? 1 : frame_pitchXY_zPos[0][1];
    pitch[2] = fabs(frame_pitchXY_zPos[0][2] - frame_pitchXY_zPos[1][2]);


    if (frame_pitchXY_zPos[1][2] - frame_pitchXY_zPos[0][2] < 0)
    {
      std::cout << "to Flipping volumes in stack direction! ";
      FlipVolumeInZ<short>(reso, img4D.back());
      std::cout << "Done\n";
    }
  }
  return true;

}



bool Open4DImgDcm3Ds(
    const std::vector<std::string>& fnames,
    EVec3i& reso,
    EVec3f& pitch,
    std::vector< short* >& img4D)
{
  for (int i = 0; i < (int)fnames.size(); ++i)
  {
    std::cout << "load: " << fnames[i].c_str() << "\n";

    Tdcmtk dcmtk(fnames[i].c_str());
    int W, H, fNum;
    dcmtk.getSize(W, H, fNum);

    if (i == 0)
    {
      reso << W, H, fNum;
      const double pX = (dcmtk.getPitchX() < 0.0) ? 1.0 : dcmtk.getPitchX();
      const double pY = (dcmtk.getPitchY() < 0.0) ? 1.0 : dcmtk.getPitchY();
      const double pZ = (dcmtk.getSliceThickness() < 0.0) ? 1.0 : dcmtk.getSliceThickness();
      pitch << (float)pX, (float)pY, (float)pZ;
    }

    if (reso[0] != W || reso[1] != H || reso[2] != fNum)
    {
      std::cout << "inconsistent error\n";
      return false;
    }

    short* img = new short[W * H * fNum];
    dcmtk.getPixels3DAs<short>(img);
    img4D.push_back(img);
  }

  return true;
}




void GenDefaultVolume(
    const float trans_x,
    const float theta,

    EVec3i &reso,
    EVec3f &pitch,
    short* &img)
{
  const int N = 120, R = N / 5;
  reso << N - 5, N, N + 5;
  pitch << 1, 1, 1;
  img = new short[reso[0] * reso[1] * reso[2]];

  EVec3f c1(2 * R, 2 * R, 2 * R), c2(3 * R, 3 * R, 3 * R);
  c1 = Eigen::AngleAxisf(theta, EVec3f(0, 0, 1)) * c1;
  c2 = Eigen::AngleAxisf(theta, EVec3f(0, 0, 1)) * c2;
  c1[0] += trans_x;
  c2[0] += trans_x;

#pragma omp parallel for 
  for (int z = 0; z < reso[2]; ++z)
  {
    for (int y = 0; y < reso[1]; ++y)
    {
      for (int x = 0; x < reso[0]; ++x)
      {
        EVec3f p((float)x, (float)y, (float)z);
        float d1 = (p - c1).norm();
        float d2 = (p - c2).norm();
        float v = 3000 * exp(-(d1 - R) * (d1 - R) / 10) + 3000 * exp(-(d2 - R) * (d2 - R) / 10);
        img[x + y * reso[0] + z * reso[1] * reso[0]] = (short)(v - 1500.0f);
      }
    }
  }
}












