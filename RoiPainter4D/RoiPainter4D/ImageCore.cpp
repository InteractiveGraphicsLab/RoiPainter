#pragma unmanaged
#include "ImageCore.h"

#pragma managed
#include "CliMessageBox.h"
#include "FormMain.h"
#pragma unmanaged

#include "../tdcmtk.h"
#include "tmarchingcubes.h"
#include <deque>

using namespace RoiPainter4D;
using namespace marchingcubes;


static const int ColPalletN = 14;
static const EVec3i ColPallet[ColPalletN] = {
  EVec3i(255,0  ,0) , EVec3i(0  ,0,255),
  EVec3i(  0,255,255), EVec3i(255,0,255), EVec3i(255,255,0),
  EVec3i(  0,128,128), EVec3i(128,0,128), EVec3i(128,128, 0),
  EVec3i(255,128,0) , EVec3i(0,255,128), EVec3i(128,0,255),
  EVec3i(128,255,0) , EVec3i(0,128,255), EVec3i(255, 0, 128) ,

};



ImageCore::ImageCore()
{
  std::cout << "constructure ImageCore-----\n";
  load4DCT(std::vector<std::string>(), 
           std::vector<std::vector<std::string>>(), FT_NON);
  std::cout << "constructure ImageCore-----DONE\n\n";
}

ImageCore::~ImageCore()
{
}

////////////////////////////////////////////////////////////////////////////////////
// 4d ct loader ////////////////////////////////////////////////////////////////////
bool t_open4DImg_dcm2Ds
(
  std::vector<std::vector<std::string>> filesInDirs,
  std::vector<short*> &img4D,
  EVec3i &reso,
  EVec3f &pitch
)
{
  for (int idx = 0; idx < (int)filesInDirs.size(); ++idx)
  {
    std::cout << "load " << idx << "/" <<filesInDirs.size() << "  " << filesInDirs[idx].size() << "\n";
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
        img4D.push_back(new short[W*H*fileNum]); //initialize
      }

      if (reso[0] != W || reso[1] != H)
      {
        std::cout << "inconsistent error\n";
        return false;
      }

      frame_pitchXY_zPos.push_back(EVec3f((float)tdcmtk.getPitchX(), (float)tdcmtk.getPitchY(), (float)tdcmtk.getZPos()));

      if (!tdcmtk.getPixelsAs<short>(&img4D.back()[i*W*H]))
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
      t_flipVolume3DInZ(reso[0], reso[1], reso[2], img4D.back());
      std::cout << "Done\n";
    }
  }
  return true;

}



bool t_open4DImg_traw3d
(
  const std::vector<std::string> &fnames,
  std::vector<short*> &img4D,
  EVec3i &reso,
  EVec3f &pitch
)
{
  for (int idx = 0; idx < (int)fnames.size(); ++idx)
  {
    std::cout << "load " << idx << "/" << fnames.size();

    FILE *fp = fopen(fnames[idx].c_str(), "rb");
    if (fp == 0) continue;

    int  W, H, D; // resoulution (Width, Height, Depth)
    fread(&W, sizeof(int), 1, fp);
    fread(&H, sizeof(int), 1, fp);
    fread(&D, sizeof(int), 1, fp);
    img4D.push_back(new short[W*H*D]);

    double px, py, pz;
    fread(&px, sizeof(double), 1, fp);
    fread(&py, sizeof(double), 1, fp);
    fread(&pz, sizeof(double), 1, fp);

    if (idx == 0)
    {
      reso << W, H, D;
      pitch << (float)px, (float)py, (float)pz;
    }

    if (fread(img4D.back(), sizeof(short), W*H*D, fp) != W*H*D)
    {
      fclose(fp);
      delete img4D.back();
      img4D.pop_back();
    }

    fclose(fp);
  }
  return true;
}



bool t_open3DImg_traw3d_ub
(
  const std::string &fname,
  EVec3i& reso ,
  EVec3f& pitch,
  byte*   &img // allocated in this function 
)
{
  FILE* fp = fopen(fname.c_str(), "rb");
  if (fp == 0) return false;

  int  W, H, D; // resoulution (Width, Height, Depth)
  fread(&W, sizeof(int), 1, fp);
  fread(&H, sizeof(int), 1, fp);
  fread(&D, sizeof(int), 1, fp);
  double px, py, pz;
  fread(&px, sizeof(double), 1, fp);
  fread(&py, sizeof(double), 1, fp);
  fread(&pz, sizeof(double), 1, fp);

  reso << W, H, D;
  pitch << (float)px, (float)py, (float)pz;

  img = new byte[W*H*D];

  if (fread(img, sizeof(byte), W * H * D, fp) != W * H * D)
  {
    delete[] img;
    return false;
  }
  return true;
}




static bool t_open4DImg_raw8bit(
    const std::vector<std::string>& fnames,
    std::vector<short*>& img4D,
    EVec3i& reso,
    EVec3f& pitch)
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

  reso  << W,H,D;
  pitch << 1.0, 1.0, 1.0;

  byte* vol = new byte[WHD];

  for (int idx = 0; idx < (int)fnames.size(); ++idx)
  {
    std::cout << "load " << idx << "/" << fnames.size();

    FILE *fp = fopen(fnames[idx].c_str(), "rb");
    if (fp == 0) continue;
   
    if (fread(vol, sizeof(byte), WHD, fp) != WHD)
    {
      std::cout << "failed to read " << idx << "volume\n";
      continue;
    }
    fclose(fp);

    img4D.push_back(new short[WHD]);
    short *trgt = img4D.back();
    for (int i = 0; i < WHD; ++i)
      trgt[i] = vol[i];
  }

  delete[] vol;
  return true;
}





bool t_open4DImg_dcm3Ds
(
  const std::vector<std::string> &fnames,
  std::vector< short* > &img4D,
  EVec3i &reso,
  EVec3f &pitch
)
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
      const double pX= (dcmtk.getPitchX()<0.0)? 1.0: dcmtk.getPitchX();
      const double pY= (dcmtk.getPitchY()<0.0)? 1.0: dcmtk.getPitchY();
      const double pZ= (dcmtk.getSliceThickness()<0.0)? 1.0: dcmtk.getSliceThickness();
      pitch << (float)pX, (float)pY, (float)pZ;
    }

    if (reso[0] != W || reso[1] != H || reso[2] != fNum)
    {
      std::cout << "inconsistent error\n";
      return false;
    }

    short *img = new short[W * H * fNum];
    dcmtk.getPixels3DAs<short>(img);
    img4D.push_back(img);
  }
  
  return true;
}







static bool t_LoadDefaultVolume
(
  EVec3i &reso,
  EVec3f &pitch, 
  float   trans_x,
  float   theta  ,
  short* &img
)
{
  const int N = 120, R = N / 5;
  reso << N - 5, N, N + 5;
  pitch << 1, 1, 1;
  img = new short[reso[0] * reso[1] * reso[2]];

  EVec3f c1(2 * R, 2 * R, 2 * R), c2(3 * R, 3 * R, 3 * R);
  c1 = Eigen::AngleAxisf(theta, EVec3f(0,0,1)) * c1;
  c2 = Eigen::AngleAxisf(theta, EVec3f(0,0,1)) * c2;
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
        float v = 3000 * exp(-(d1 - R)*(d1 - R) / 10) + 3000 * exp(-(d2 - R)*(d2 - R) / 10);
        img[x + y * reso[0] + z * reso[1] * reso[0]] = (short)(v - 1500.0f);
      }
    }
  }
  return true;
}



// 上の画像を作成する（ファントム画像） 
// cpを配置するUIを
// shortest pathを解く　（C. Dについては考える）







static void t_calcMinMaxValOfImg4D(
    const std::vector<short*> &img4d, 
    const int num_voxel, 
    short &min_val, 
    short &max_val)
{
  const int num_frames = (int) img4d.size();
  std::vector<short> min_vals(num_frames, SHRT_MAX );
  std::vector<short> max_vals(num_frames, SHRT_MIN );

#pragma omp parallel for
  for (int fi = 0; fi < num_frames; ++fi)
  {
    short* img = img4d[fi];
    short& minv = min_vals[fi];
    short& maxv = max_vals[fi];

    for (int i = 0; i < num_voxel; ++i)
    {
      if(minv > img[i] ) minv = img[i];
      if(maxv < img[i] ) maxv = img[i];
    }
  }

  min_val = min_vals[0];
  max_val = max_vals[0];
  for (int fi = 1; fi < num_frames; ++fi)
  {
    if(min_val > min_vals[fi] ) min_val = min_vals[fi];
    if(max_val < max_vals[fi] ) max_val = max_vals[fi];
  }
}







/*
3D画像の読み込み

FT_TRAW3D         : 複数指定されたTRAW3D_FILEを読み込む
FT_3D_DICOM_FLIES : 複数指定されたDicom File(3D)を読み込む
FT_2D_DICOM_FLIES : slice(連番・複数)が入ったフォルダを、複数用意しこれを4DCTとして読み込む，
                    filesには複数フォルダがはいったひとつのフォルダを指定する

※ この関数はコンストラクタから呼ばれるため、外のシングルトンにはアクセスしないこと
   (初期化時のping pongを避ける)
*/
void ImageCore::load4DCT(
  std::vector<std::string>              &files,       //used for  FT_TRAW3D/FT_3D_DICOM_FLIES
  std::vector<std::vector<std::string>> &filesInDirs, //used for  FT_2D_DICOM_FLIES
  LOAD_FILE_TYPE type)
{
  //clear 4d ct volume 
  for (int i = 0; i < m_img4d.size(); ++i) delete m_img4d[i];
  for (int i = 0; i < m_flg4d.size(); ++i) delete m_flg4d[i];
  for (int i = 0; i < m_mask4d.size(); ++i) delete m_mask4d[i];
  m_img4d.clear();
  m_mask4d.clear();
  m_flg4d.clear();

  m_pitch << 1, 1, 1;
  m_reso << 0, 0, 0;


  if (     type == FT_2D_DICOM_FLIES) 
    t_open4DImg_dcm2Ds(filesInDirs, m_img4d, m_reso, m_pitch);
  else if (type == FT_3D_DICOM_FLIES) 
    t_open4DImg_dcm3Ds(files, m_img4d, m_reso, m_pitch);
  else if (type == FT_TRAW3D        ) 
    t_open4DImg_traw3d(files, m_img4d, m_reso, m_pitch);
  else if (type == FT_RAW8BIT)
    t_open4DImg_raw8bit(files, m_img4d, m_reso, m_pitch);


  std::cout <<  " m_img4d.size() " << m_img4d.size() << "\n";

  bool is_defoult_voxel = false;
  if (m_img4d.size() == 0 || m_reso[0] == 0 || m_reso[1] == 0 || m_reso[2] == 0)
  {
    //load default file
    for (int i = 0; i < 10; ++i)
    {
      short *img;
      float theta = (float)(i*M_PI / 50.0);
      t_LoadDefaultVolume(m_reso, m_pitch, i*2.0f, theta,  img);
      m_img4d.push_back(img);
    }
    is_defoult_voxel = true;
  }


  //get min and max (for Window level range)
  const int W = m_reso[0];
  const int H = m_reso[1];
  const int D = m_reso[2], WHD = W*H*D;

  short min_val, max_val;
  t_calcMinMaxValOfImg4D(m_img4d, WHD, min_val, max_val);
  m_volume_minmax[0] = min_val; 
  m_volume_minmax[1] = max_val;
  std::cout << "Window Level is (" <<m_volume_minmax[0] << "," << m_volume_minmax[1] << ")\n";


  //allocate other fields 
  for (int i = 0; i < (int)m_img4d.size(); ++i)
  {
    m_flg4d .push_back(new byte[WHD]);
    m_mask4d.push_back(new byte[WHD]);
    memset(m_flg4d .back(), 0, sizeof(byte)*WHD);
    memset(m_mask4d.back(), 0, sizeof(byte)*WHD);
  }

  m_vol.Allocate(W, H, D);
  m_vol_flg.Allocate(W, H, D);
  m_vol_mask.Allocate(W, H, D);
  m_vol.SetValue(m_img4d[0], (short)m_volume_minmax[0], (short)m_volume_minmax[1]);
  m_vol_flg.SetValue(m_flg4d[0]);
  m_vol_mask.SetValue(m_mask4d[0]);

  //set mask[0]
  m_mask_data.clear();
  m_mask_data.push_back(MaskData("bckGrnd", EVec3i(0, 0, 0), 0, 0));

  //transfer functions
  m_img_maskcolor.Allocate(256);
  m_img_maskcolor[0] = m_img_maskcolor[1] = m_img_maskcolor[2] = m_img_maskcolor[3] = 0;
  for (int i = 1; i < 256; ++i)
  {
    m_img_maskcolor[4 * i + 0] = (i % 7) * 64;
    m_img_maskcolor[4 * i + 1] = (i % 2) * 255;
    m_img_maskcolor[4 * i + 2] = (i % 5) * 128;
    m_img_maskcolor[4 * i + 3] = 255;
  }
  
  //message on pich
  if( !is_defoult_voxel )
  {
    if(m_pitch[0] == 1 && m_pitch[1] == 1 && m_pitch[2] == 1)
    {
      std::cout << "Please check dialog message...\n";
      ShowMsgDlg_OK("Opened successfully.\n\nPlease check Pitch!\nピッチが正しいか（上下前後左右に歪んでいないか）確認してください。", "Message");
    }
    else if(type == FT_2D_DICOM_FLIES || type == FT_3D_DICOM_FLIES)
    {
      std::cout << "Please check dialog message...\n";
      ShowMsgDlg_OK("Pitch情報をdicomより読み込みました．念のため正しいか(画像がゆがんでないか)を確認してください．", "Message");
    }
  }
}


void ImageCore::SelectedMsk_SetColor ( const EVec3i color ){
  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;
  m_mask_data[m_active_maskid].color = color;
}

void ImageCore::SelectedMsk_SetLocked( const bool   tf    ){
  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;
  m_mask_data[m_active_maskid].lock = tf;
}

void ImageCore::SelectedMsk_SetAlpha ( const float  alpha ){
  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;
  m_mask_data[m_active_maskid].alpha = alpha;
} 

void ImageCore::SetSelectMaskId      ( const int    idx   ){
  if (idx < 0 || m_mask_data.size() <= idx) return;
  m_active_maskid = idx;
}


void ImageCore::SelectedMsk_delete() {

  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;

  if (m_active_maskid == 0)
  {
    ShowMsgDlg_OK("0th region (background) cannot be removed", "caution");
    return;
  }

  if (ShowMsgDlgYesNo("Do you want to DELETE the selected mask?", "caution") == false) return;


  const int num_voxels = GetNumVoxels();
  const int num_frames = GetNumFrames();

  for (int fi = 0; fi < num_frames; ++fi)
  {
    byte* msk3d = m_mask4d[fi];

#pragma omp parallel for
    for (int i = 0; i < num_voxels; ++i)
    {
      if      (msk3d[i] >  m_active_maskid) msk3d[i] --;
      else if (msk3d[i] == m_active_maskid) msk3d[i] = 0;
    }

    formMain_SetProgressValue(fi/(float)num_frames);
  }
  formMain_SetProgressValue(0);

  m_mask_data.erase(m_mask_data.begin() + m_active_maskid);
  --m_active_maskid;
}






void ImageCore::SelectedMsk_marge(const int &trgtMaskID) {

  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;

  if (m_active_maskid == 0 || trgtMaskID == 0)
  {
    ShowMsgDlg_OK("0th region (background) cannot be marged", "caution");
    return;
  }

  if (trgtMaskID == m_active_maskid)
  {
    ShowMsgDlg_OK("Cannot marge to it self", "caution");
    return;
  }

  std::cout << "active " << m_active_maskid << " trgt " << trgtMaskID << "\n";

  const int num_frames = GetNumVoxels();

  for (int fi = 0; fi < m_mask4d.size(); ++fi)
  {
    byte *msk3d = m_mask4d[fi];

#pragma omp parallel for
    for (int i = 0; i < num_frames; ++i) 
      if (msk3d[i] == trgtMaskID)
        msk3d[i] = m_active_maskid;

#pragma omp parallel for
    for (int i = 0; i < num_frames; ++i) 
      if (msk3d[i] > trgtMaskID)
        --msk3d[i];
    
    formMain_SetProgressValue(fi/(float)num_frames);

  }
  
  formMain_SetProgressValue(0);

  if (m_active_maskid > trgtMaskID) --m_active_maskid;

  m_mask_data.erase(m_mask_data.begin() + trgtMaskID);
  m_mask_data[m_active_maskid].surf.Clear();
  m_mask_data[m_active_maskid].bRendSurf = false;
}





static void t_ErodeTargetMask(
  const EVec3i resolution, 
  const int    trgtmaskid,
  byte* mask3d
){
  const int num_voxels = resolution[0] * resolution[1] * resolution[2];
  byte* flgs = new byte[num_voxels];

#pragma omp parallel for
  for (int i = 0; i < num_voxels; ++i)
  {
    flgs[i] = (mask3d[i] == trgtmaskid) ? 255 : 1;
  }

  t_Erode3D( resolution[0], resolution[1], resolution[2], flgs);

#pragma omp parallel for
  for (int i = 0; i < num_voxels; ++i) if (mask3d[i] == trgtmaskid)
  {
    if (flgs[i] == 1) mask3d[i] = 0;
  }

  delete[] flgs;
}

static void t_DilateTargetMask(
  const EVec3i resolution,
  const int    trgtmaskid,
  const std::vector<MaskData> &maskdata,
  byte* mask3d
)
{
  const int num_voxels = resolution[0] * resolution[1] * resolution[2];

  //maskへのアクセスが遅いので配列を利用して高速化
  byte mask_locked[256] ={};//0初期化
  for( int i=0; i < (int)maskdata.size(); ++i) mask_locked[i] = maskdata[i].lock ? 1:0;


  byte* flgs = new byte[num_voxels];

#pragma omp parallel for
  for (int i = 0; i < num_voxels; ++i) 
  {
    flgs[i] = (mask3d[i] == trgtmaskid  ) ? 255 :
              (mask_locked[mask3d[i]]   ) ? 0   : 1;
  }

  t_Dilate3D(resolution[0], resolution[1], resolution[2], flgs);

#pragma omp parallel for
  for (int i = 0; i < num_voxels; ++i) {
    if (flgs[i] == 255) mask3d[i] = trgtmaskid;
  }

  delete[] flgs;
}




static void t_FillHoleTargetMask(
  const EVec3i resolution,
  const int    trgtmaskid,
  const std::vector<MaskData> &maskdata,
  byte* mask3d
)
{
  //maskへのアクセスが遅いので配列を利用して高速化
  byte mask_locked[256] ={};//0初期化
  for( int i=0; i < (int)maskdata.size(); ++i) mask_locked[i] = maskdata[i].lock ? 1:0;

  const int num_voxels = resolution[0] * resolution[1] * resolution[2];
  byte* flgs = new byte[num_voxels];

#pragma omp parallel for
  for (int i = 0; i < num_voxels; ++i)
  {
    flgs[i] =(mask3d[i] == trgtmaskid ) ? 255 : 0;
  }

  t_FillHole3D(resolution[0], resolution[1], resolution[2], flgs);

#pragma omp parallel for
  for (int i = 0; i < num_voxels; ++i)
  {
    if (flgs[i] == 255 && !mask_locked[mask3d[i]] ) mask3d[i] = trgtmaskid;
  }

  delete[] flgs;
}




void ImageCore::SelectedMsk_erodeOne(const int frame_idx) 
{
  if (m_active_maskid == 0 && !ShowMsgDlgYesNo("mask0番にこの処理を行ないますか？", "message") ) return;
  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;

  t_ErodeTargetMask( m_reso, m_active_maskid, m_mask4d[frame_idx]);
}


void ImageCore::SelectedMsk_dilateOne(const int frame_idx) 
{
  if (m_active_maskid == 0 && !ShowMsgDlgYesNo("mask0番にこの処理を行ないますか？", "message") ) return;
  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;

  t_DilateTargetMask( m_reso, m_active_maskid, m_mask_data, m_mask4d[frame_idx] );
}


void ImageCore::SelectedMsk_fillHoleOne(const int frame_idx) 
{
  if (m_active_maskid == 0 && !ShowMsgDlgYesNo("mask0番にこの処理を行ないますか？", "message") ) return;
  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;

  t_FillHoleTargetMask( m_reso, m_active_maskid, m_mask_data, m_mask4d[frame_idx]);
}


void ImageCore::SelectedMsk_erodeAll() 
{
  if (m_active_maskid == 0 && !ShowMsgDlgYesNo("mask0番にこの処理を行ないますか？", "message") ) return;
  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;

  const int num_frames = GetNumFrames();
  for (int i = 0; i < num_frames; ++i){
    t_ErodeTargetMask( m_reso, m_active_maskid, m_mask4d[i]); 
    formMain_SetProgressValue(i/(float)num_frames);
  }
  formMain_SetProgressValue(0);
}


void ImageCore::SelectedMsk_dilateAll() 
{
  if (m_active_maskid == 0 && !ShowMsgDlgYesNo("mask0番にこの処理を行ないますか？", "message") ) return;
  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;

  const int num_frames = GetNumFrames();
  for (int i = 0; i < num_frames; ++i){
    t_DilateTargetMask( m_reso, m_active_maskid, m_mask_data, m_mask4d[i] );
    formMain_SetProgressValue(i/(float)num_frames);
  }
  formMain_SetProgressValue(0);
}


void ImageCore::SelectedMsk_fillHoleAll() 
{
  if (m_active_maskid == 0 && !ShowMsgDlgYesNo("mask0番にこの処理を行ないますか？", "message") ) return;
  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;

  const int num_frames = GetNumFrames();
  for (int fI = 0; fI < num_frames; ++fI){
    t_FillHoleTargetMask( m_reso, m_active_maskid, m_mask_data, m_mask4d[fI]);
    formMain_SetProgressValue(fI/(float)num_frames);
  }
  formMain_SetProgressValue(0);
}




void ImageCore::SelectedMsk_expObjOne(
    const std::string &fname, 
    const int &frame_idx)
{
  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;

  const int num_voxels = GetNumVoxels();

  short *v = new short[num_voxels];
  for (int i = 0; i < num_voxels; ++i) v[i] = (m_mask4d[frame_idx][i] == m_active_maskid) ? 255 : 0;

  TMesh mesh;
  marchingcubes::t_MarchingCubes(m_reso, m_pitch, v, 128, 0, 0, mesh);
  mesh.Smoothing(2);
  mesh.ExportObjNoTexCd(fname.c_str());

  delete[] v;
};



void ImageCore::SelectedMsk_expObjAll(const std::string &fname) 
{
  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;

  const int num_frames = GetNumFrames();
  const int num_voxels = GetNumVoxels();

  short *v = new short[num_voxels];

  for (int fi = 0; fi < num_frames; fi++) 
  {
    for (int vi = 0; vi < num_voxels; ++vi) v[vi] = (m_mask4d[fi][vi] == m_active_maskid) ? 255 : 0;

    TMesh mesh;
    marchingcubes::t_MarchingCubes(m_reso, m_pitch, v, 128, 0, 0, mesh);
    mesh.Smoothing(2);

    std::string nameStr = fname.substr(0, fname.find_last_of("."));

    std::ostringstream ss;  //3桁になるよう0を付加 edit by kikuchi
    #ifndef _DEBUG
    ss << std::setw(3) << std::setfill('0') << std::to_string(fi);
    nameStr += ss.str()+ ".obj";
    mesh.ExportObjNoTexCd(nameStr.c_str());
    #endif
  }

  delete[] v;
};




void ImageCore::SaveMask(std::string fname)
{
  FILE *fp = fopen(fname.c_str(), "wb");

  //1. version info 
  int version = 0;
  fwrite(&version, sizeof(int), 1, fp);

  //2. reso info
  const int W = m_reso[0];
  const int H = m_reso[1];
  const int D = m_reso[2];
  const int F = (int)m_img4d.size();
  fwrite(&W, sizeof(int), 1, fp);
  fwrite(&H, sizeof(int), 1, fp);
  fwrite(&D, sizeof(int), 1, fp);
  fwrite(&F, sizeof(int), 1, fp);

  //3. mask data
  int maskN = (int)m_mask_data.size();
  fwrite(&maskN, sizeof(int), 1, fp);

  for (const auto &it : m_mask_data)
  {
    const int nLen = (int)it.name.length();
    fwrite(&nLen, sizeof(int), 1, fp);
    fwrite(it.name.c_str(), sizeof(char), nLen + 1, fp);

    const int iLock = it.lock;
    fwrite(&it.alpha, sizeof(double), 1, fp);
    fwrite(it.color.data(), sizeof(int), 3, fp);
    fwrite(&iLock, sizeof(int), 1, fp);

    std::cout << nLen << " " << it.name.c_str() << "\n";
  }


  //4. mask volume 
  for (int f = 0; f < F; ++f)
  {
    std::cout << "Saved " << f << " / " << F << " frame.\n";
    fwrite(m_mask4d[f], sizeof(byte), W*H*D, fp);
  }

  fclose(fp);
  std::cout << "Finished save mask.\n";
}




void ImageCore::SaveMaskAsTRawFiles(std::string fname)
{
  const int W = m_reso[0];
  const int H = m_reso[1];
  const int D = m_reso[2];
  const double px = m_pitch[0];
  const double py = m_pitch[1];
  const double pz = m_pitch[2];

  const int F = (int)m_img4d.size();

  for (int i = 0; i < F; ++i)
  {
    std::string f = (i < 10) ? std::string("0") + std::to_string(i) : std::to_string(i);
    f = fname + f + std::string(".traw3D_ub");

    FILE* fp = fopen(f.c_str(), "wb");
    if (fp == 0) return;

    fwrite(&W, sizeof(int), 1, fp);
    fwrite(&H, sizeof(int), 1, fp);
    fwrite(&D, sizeof(int), 1, fp);
    fwrite(&px, sizeof(double), 1, fp);
    fwrite(&py, sizeof(double), 1, fp);
    fwrite(&pz, sizeof(double), 1, fp);

    fwrite(m_mask4d[i], sizeof(byte), W*H*D, fp);

    fclose(fp);
  }

}





void ImageCore::SaveImg4DAsTRawFiles(std::string fname)
{
  const int W = m_reso[0];
  const int H = m_reso[1];
  const int D = m_reso[2];
  const double px = m_pitch[0];
  const double py = m_pitch[1];
  const double pz = m_pitch[2];

  const int F = (int)m_img4d.size();

  for (int i = 0; i < F; ++i)
  {
    std::string f = (i < 10) ? std::string("0") + std::to_string(i) : std::to_string(i);
    f = fname + f + std::string(".traw3D_ss");

    FILE* fp = fopen(f.c_str(), "wb");
    if (fp == 0) return;

    fwrite(&W, sizeof(int), 1, fp);
    fwrite(&H, sizeof(int), 1, fp);
    fwrite(&D, sizeof(int), 1, fp);
    fwrite(&px, sizeof(double), 1, fp);
    fwrite(&py, sizeof(double), 1, fp);
    fwrite(&pz, sizeof(double), 1, fp);
    
    fwrite(m_img4d[i], sizeof(short), W * H * D, fp);

    fclose(fp);
  }

}



void ImageCore::LoadMask(std::string fname, int timeI)
{
  FILE* fp = fopen(fname.c_str(), "rb");

  int version, W, H, D, F;

  //1. version
  fread(&version, sizeof(int), 1, fp);

  //2. resolution 
  fread(&W, sizeof(int), 1, fp);
  fread(&H, sizeof(int), 1, fp);
  fread(&D, sizeof(int), 1, fp);
  fread(&F, sizeof(int), 1, fp);

  if (W != m_reso[0] || H != m_reso[1] || D != m_reso[2] || F != (int)m_img4d.size())
  {
    Trace(m_reso);
    std::cout << W << " " << H << " " << D << " " << F << "\n";
  
    ShowMsgDlg_OK("caution", "strange volume size");
    fclose(fp);
    return;
  }

  //3. mask data
  int maskN;
  fread(&maskN, sizeof(int), 1, fp);


  m_mask_data.clear();

  for (int i = 0; i < maskN; ++i)
  {
    int nLen;
    fread(&nLen, sizeof(int), 1, fp);

    char *name = new char[nLen + 1];
    fread(name, sizeof(char), nLen + 1, fp);

    int lock, col[3];
    double alpha;
    fread(&alpha, sizeof(double), 1, fp);
    fread(col, sizeof(int), 3, fp);
    fread(&lock, sizeof(int), 1, fp);

    m_mask_data.push_back(MaskData(std::string(name), EVec3i(col[0], col[1], col[2]), alpha, 0, lock ? true : false));

    delete[] name;
  }

  //4. mask volume 
  for (int f = 0; f < F; ++f)
  {
    fread(m_mask4d[f], sizeof(byte), W * H * D, fp);
  }
  fclose(fp);

  //flip??	
  //DlgSetStackOrder dlg;
  //if (IDOK != dlg.DoModal()) m_volMsk.flipVolumeInZ();

  m_vol_mask.SetValue(m_mask4d[timeI]);

}


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
static bool ReadMha(
  const char* fname,
  EVec3i &res  ,
  EVec3f &pitch, 
  std::unique_ptr<byte[]>& msk_data
  )
{
  std::ifstream ifs(fname, std::ios::binary);
  if (!ifs)
  {
    std::cout << "can't open mha file " << fname << "\n";
    return false;
  }
  
  //Read header 
  int rx, ry, rz;
  float px, py, pz;
  int n_dim, m0, m1, m2, m3, m4, m5, m6, m7, m8, o0, o1, o2, r0, r1, r2;
  byte b1[256], b2[256], b3[256], obj_type[256];
  
  ifs >> b1 >> b2 >> obj_type;      // ObjectType = Image
  ifs >> b1 >> b2 >> n_dim;         // NDims = 3
  ifs >> b1 >> b2 >> b3;            // BinaryData = True
  ifs >> b1 >> b2 >> b3;            // BinaryDataByteOrderMSB = False
  ifs >> b1 >> b2 >> b3;            // CompressedData = False
  ifs >> b1 >> b2 >> m0 >> m1 >> m2 
                  >> m3 >> m4 >> m5
                  >> m6 >> m7 >> m8;//TransformMatrix = 1 0 0 0 1 0 0 0 1
  ifs >> b1 >> b2 >> o0 >> o1 >> o2;//Offset = 0 0 0
  ifs >> b1 >> b2 >> r0 >> r1 >> r2;//CenterOfRotation = 0 0 0


  ifs >> b1 >> b2 >> b3;            //AnatomicalOrientation = RAI
  ifs >> b1 >> b2 >> px >> py >> pz;//ElementSpacing = 0.46800000000000003 0.46800000000000003 0.5
  ifs >> b1 >> b2 >> rx >> ry >> rz;//DimSize = 512 512 320
  ifs >> b1 >> b2 >> b3;  //ElementType = MET_UCHAR
  ifs >> b1 >> b2 >> b3;  //ElementDataFile = LOCAL

  std::cout << "resolution " << rx << " " << ry << " " << rz << "\n";
  std::cout << "pitch      " << px << " " << py << " " << pz << "\n";
  res << rx, ry, rz;  
  pitch << px, py, pz;

  msk_data.reset(new byte[rx * ry * rz]);
  if (!ifs.read(reinterpret_cast<char*>(msk_data.get()), rx * ry * rz))
  {
    return false;
  }
  return true;
} 




void ImageCore::LoadMaskMha(std::vector<std::string> fnames, int timeI)
{
  int W, H, D, WH, WHD;
  std::tie(W, H, D, WH, WHD) = GetResolution5();
  int NUM_FRM = GetNumFrames();
 
  if (NUM_FRM != fnames.size())
  {
    ShowMsgDlg_OK("strange frame count", "caution");
    return;
  }

  std::vector<std::unique_ptr<byte[]>> masks;
  //check resolution 
  for (auto f : fnames)
  {
    EVec3f pitch;
    EVec3i reso ;
    std::unique_ptr<byte[]> d;
    if (!ReadMha(f.c_str(), reso, pitch, d))
    {
      std::cout << "Fail to read mha files\n";
    }
    if (reso[0] != W || reso[1] != H || reso[2] != D)
    {
      ShowMsgDlg_OK("strange resolution", "caution");
      return;
    }
    masks.push_back(std::move(d)); //所有権を masksに渡す
  }
  
  int max_mask_id = 0;
  for (int f = 0; f < NUM_FRM; ++f)
  {
    memcpy(m_mask4d[f], masks[f].get(), sizeof(byte) * WHD);
    t_FlipVolumeInZ<byte>(W, H, D, m_mask4d[f]);

    byte *m = m_mask4d[f];
    for( int i=0; i < WHD; ++i) max_mask_id = max(max_mask_id, m[i]);
  }
  
  //generate mask 
  m_mask_data.clear();
  m_mask_data.push_back(MaskData("bckGrnd", EVec3i(0, 0, 0), 0, 0));
  for( int c = 1; c < max_mask_id; ++c)
    m_mask_data.push_back(MaskData("region", ColPallet[c % ColPalletN], 0.1, false, true));

  m_vol_mask.SetValue(m_mask4d[timeI]);
}



void ImageCore::LoadMaskTRawFiles(
  std::vector<std::string> fnames, 
  int frameI)
{ 
  const int  num_frames = (int)m_img4d.size();
  const byte num_masks  = (byte)m_mask_data.size();

  int num_new_mask = 0;
  
  for(int fi=0; fi < fnames.size(); ++fi)
  { 
    EVec3i reso;
    EVec3f pitch;
    byte *img;
    if (!t_open3DImg_traw3d_ub(fnames[fi], reso, pitch, img))
    {
      std::cout << "failed reading " << fi << " files\n";
      delete[] img;
      continue;
    }

    // write mask data 
    byte* mask = m_mask4d[fi];
    const int WHD = reso[0] * reso[1] * reso[2];
    for (int i = 0; i < WHD; ++i)
    {
      if(img[i] == 0) continue;
      if(mask[i]>0 && m_mask_data[mask[i]].lock) continue;

      if (img[i] > num_new_mask) num_new_mask = img[i];
      mask[i] = img[i] + num_masks - 1;
    }

    delete[] img;
  }

  //generate mask 
  for (int c = 0; c < num_new_mask; ++c)
    m_mask_data.push_back(MaskData("import", ColPallet[(num_masks + c) % ColPalletN], 0.1, false, true));
  m_vol_mask.SetValue(m_mask4d[frameI]);
}





void ImageCore::UpdateVisVolume(int winlv_min, int winlv_max, int frame_idx)
{
  if (frame_idx < 0 || m_img4d.size() - 1 < frame_idx) return;
  m_vol     .SetValue( m_img4d[frame_idx], (short)winlv_min, (short)winlv_max);
  m_vol_flg .SetValue( m_flg4d[frame_idx] );
  m_vol_mask.SetValue( m_mask4d[frame_idx]);
}

void ImageCore::UpdateImgMaskColor()
{
  m_img_maskcolor.SetZero();
  for (int i = 0; i < (int)m_mask_data.size(); ++i)
  {
    m_img_maskcolor[4 * i + 0] = m_mask_data[i].color[0];
    m_img_maskcolor[4 * i + 1] = m_mask_data[i].color[1];
    m_img_maskcolor[4 * i + 2] = m_mask_data[i].color[2];
    m_img_maskcolor[4 * i + 3] = (int)(m_mask_data[i].alpha * 255);

    if ( m_mask_data[i].bRendSurf) m_img_maskcolor[4 * i + 3] = 0;
  }
  m_img_maskcolor.SetUpdated();
}










//------------------------------------------------------------------------
// generate new region by using all voxels with (m_volFlg[i] == 255) 
// 現在の m_img4D_mask の 前景領域（255）をマスクとして登録
// IDは現在の登録マスク数 
//------------------------------------------------------------------------
void ImageCore::mask_storeCurrentForeGround()
{
  const int newid     = (int)m_mask_data.size();
  const int num_frame = GetNumFrames();
  const int num_voxel = GetNumVoxels();

  for (int fi = 0; fi < num_frame; ++fi)
  {
    byte* flg3d  = m_flg4d[fi];
    byte* mask3d = m_mask4d[fi];
#pragma omp parallel for
    for (int i = 0; i < num_voxel; ++i)
      if (flg3d[i] == 255)  mask3d[i] = newid ;

    formMain_SetProgressValue(fi/(float)num_frame);
  }
  formMain_SetProgressValue(0);

  //initial color
  static int c = 0;
  //store new region
  m_mask_data.push_back(MaskData("region", ColPallet[c%ColPalletN], 0.1, false, true));
  c++;
}




void ImageCore::InitializeFlg4dByMask( void (*progressfunc)(float) )
{
  //maskへのアクセスが遅いので配列を利用して高速化
  byte mask_locked[256] ={};
  for( int i=0; i < (int)m_mask_data.size(); ++i) mask_locked[i] = m_mask_data[i].lock ? 1:0;

  const int num_frames = GetNumFrames();
  const int num_voxels = GetNumVoxels();

  for (int f = 0; f < num_frames; ++f)
  {
    byte* flg3d = m_flg4d [f];
    byte* msk3d = m_mask4d[f];

#pragma omp parallel for
    for (int i = 0; i < num_voxels; ++i)
    {
      flg3d[i] = (mask_locked[msk3d[i]] ) ? 0 : 1;
    }
    
    if( progressfunc != 0) progressfunc(f/float(num_frames));
  }

  if( progressfunc != 0) progressfunc(0);
}







#pragma managed

