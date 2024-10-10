#ifndef COMMON_TIMAGELOADER_H_
#define COMMON_TIMAGELOADER_H_

#pragma managed

inline bool LoadImage(
  const char *fname,
  int &W,
  int &H,
  unsigned char* &rgba)
{

  System::String ^s = gcnew System::String(fname);
  System::Drawing::Image^ img;

  try
  {
    img = System::Drawing::Image::FromFile(s);
  }
  catch ( System::IO::FileNotFoundException ^e)
  {
    e->ToString();
    return false;
  }

  System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(img);
  if (bmp->Width == 0 || bmp->Height == 0) return false;

  W = bmp->Width;
  H = bmp->Height;

  System::Drawing::Imaging::BitmapData ^bmpData =
    bmp->LockBits(System::Drawing::Rectangle(0, 0, W, H),
      System::Drawing::Imaging::ImageLockMode::WriteOnly,
      bmp->PixelFormat);

  rgba = new unsigned char[W*H * 4];

  unsigned char* pBuf = (unsigned char*)bmpData->Scan0.ToPointer();

  //�P�x�l�̎擾�A�ݒ� 
  if (bmp->PixelFormat == System::Drawing::Imaging::PixelFormat::Format8bppIndexed)
  {
    for (int y = 0; y < H; y++)
    {
      for (int x = 0; x < W; x++)
      {
        const int I = 4 * (x + y*W);
        rgba[I + 0] = rgba[I + 1] = rgba[I + 2] = (unsigned char)pBuf[x + y * bmpData->Stride];
      }
    }
  }
  else if (bmp->PixelFormat == System::Drawing::Imaging::PixelFormat::Format24bppRgb ||
    bmp->PixelFormat == System::Drawing::Imaging::PixelFormat::Format32bppArgb)
  {
    //24bit BGRBGRBGR...   32bit BGRA BGRABGRA...

    int BitCount = bmp->GetPixelFormatSize(bmp->PixelFormat);
    int Step = BitCount / 8;

    for (int y = 0; y < H; y++)
    {
      for (int x = 0; x < W; x++)
      {
        const int I = 4 * (x + y*W);
        rgba[I + 2] = (unsigned char)pBuf[x * Step + 0 + y * bmpData->Stride]; //B 
        rgba[I + 1] = (unsigned char)pBuf[x * Step + 1 + y * bmpData->Stride]; //G 
        rgba[I + 0] = (unsigned char)pBuf[x * Step + 2 + y * bmpData->Stride]; //R
      }
    }
  }
  else
  {
    std::cout << "-------------------------error unknown format \n";
    return false;
  }


  bmp->UnlockBits(bmpData);
  delete(bmp);
  delete(img);
  delete(s);

  return true;
}



inline void SaveImage(
  const char *fname,
  const int &W,
  const int &H,
  const unsigned char* rgba)
{

  System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(W, H, System::Drawing::Imaging::PixelFormat::Format32bppRgb);

  System::Drawing::Imaging::BitmapData ^bmpData = bmp->LockBits(
      System::Drawing::Rectangle(0, 0, W, H),
      System::Drawing::Imaging::ImageLockMode::WriteOnly,
      bmp->PixelFormat);

  unsigned char* pBuf = (unsigned char*)bmpData->Scan0.ToPointer();


  //32bit BGRA BGRABGRA...

  int BitNum = bmp->GetPixelFormatSize(bmp->PixelFormat);
  int Step = BitNum / 8;

  for (int y = 0; y < H; y++)
  {
    for (int x = 0; x < W; x++)
    {
      const int I = 4 * (x + y*W);
      pBuf[x * Step + 0 + y * bmpData->Stride] = rgba[I + 2]; //B 
      pBuf[x * Step + 1 + y * bmpData->Stride] = rgba[I + 1]; //G 
      pBuf[x * Step + 2 + y * bmpData->Stride] = rgba[I + 0]; //R
    }
  }

  System::String ^fname_str = gcnew System::String(fname);
  System::String ^ext = System::IO::Path::GetExtension(fname_str)->ToLower();

  if (ext == ".bmp") bmp->Save(fname_str, System::Drawing::Imaging::ImageFormat::Bmp);
  else if (ext == ".jpg") bmp->Save(fname_str, System::Drawing::Imaging::ImageFormat::Jpeg);
  else if (ext == ".png") bmp->Save(fname_str, System::Drawing::Imaging::ImageFormat::Png);
  else if (ext == ".tif") bmp->Save(fname_str, System::Drawing::Imaging::ImageFormat::Tiff);

  delete(bmp);
  delete(fname_str);
}


inline void SaveImageGray(
  const char *fname,
  const int &W,
  const int &H,
  const unsigned char* intensity)
{

  System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(W, H, System::Drawing::Imaging::PixelFormat::Format32bppRgb);

  System::Drawing::Imaging::BitmapData ^bmpData = bmp->LockBits(
      System::Drawing::Rectangle(0, 0, W, H),
      System::Drawing::Imaging::ImageLockMode::WriteOnly,
      bmp->PixelFormat);

  unsigned char* pBuf = (unsigned char*)bmpData->Scan0.ToPointer();


  //32bit BGRA BGRABGRA...

  int BitNum = bmp->GetPixelFormatSize(bmp->PixelFormat);
  int Step = BitNum / 8;

  for (int y = 0; y < H; y++)
  {
    for (int x = 0; x < W; x++)
    {
      const int I = x + y*W;
      pBuf[x * Step + 0 + y * bmpData->Stride] = intensity[I]; //B 
      pBuf[x * Step + 1 + y * bmpData->Stride] = intensity[I]; //G 
      pBuf[x * Step + 2 + y * bmpData->Stride] = intensity[I]; //R
    }
  }

  System::String ^fname_str = gcnew System::String(fname);
  System::String ^ext = System::IO::Path::GetExtension(fname_str)->ToLower();

  if (ext == ".bmp") bmp->Save(fname_str, System::Drawing::Imaging::ImageFormat::Bmp);
  else if (ext == ".jpg") bmp->Save(fname_str, System::Drawing::Imaging::ImageFormat::Jpeg);
  else if (ext == ".png") bmp->Save(fname_str, System::Drawing::Imaging::ImageFormat::Png);
  else if (ext == ".tif") bmp->Save(fname_str, System::Drawing::Imaging::ImageFormat::Tiff);

  delete(bmp);
  delete(fname_str);
}



#endif