//----------------------------------------------------------
//RoiPainter4D: 4DCT analysis software
//Copyright (C) 2017, Takashi  Ijiri, Tomofumi Narita
//This program is released under GNU GPL v3.
//See README and LICENSE for detail.
//
//class ImageCore :
//CT画像などの画像を管理するシングルトンクラス
//オリジナル画像はshort配列で保持
//GPUへ転送するため8bitにトーンマッピングする
//----------------------------------------------------------
#ifndef IMAGECORE_H_
#define IMAGECORE_H_

#pragma unmanaged
#include "OglImage.h"
#include "tmesh.h"
#include <vector>
#include <tuple>


enum LOAD_FILE_TYPE {
  FT_NON,
  FT_TRAW3D,
  FT_RAW8BIT,
  FT_DCM3D,
  FT_DCM2D,
  FT_MHA
};


class MaskData
{
public:
  std::string name;
  TMesh  surf;
  EVec3i color;
  double alpha;
  bool   bRendSurf;
  bool   lock;

  MaskData(std::string _name, EVec3i _color, double _alpha, bool _bRendSurf, bool _lock = false)
  {
    name = _name;
    color = _color;
    alpha = _alpha;
    bRendSurf = _bRendSurf;
    lock = _lock;
  }

  void Set(const MaskData& v)
  {
    name = v.name;
    surf = v.surf;
    color = v.color;
    alpha = v.alpha;
    bRendSurf = v.bRendSurf;
    lock = v.lock;
  }

  MaskData(const MaskData& v)
  {
    //std::cout << "for debug, MaskData copy const\n";
    Set(v);
  }

  MaskData& operator=(const MaskData& v)
  {
    //std::cout << "for debug, MaskData operator =\n";
    Set(v);
    return *this;
  }
};





class ImageCore
{
private:
  EVec3i m_reso         ; //解像度 x,y,z
  EVec3f m_pitch        ; //pitch  x,y,z
  EVec2i m_volume_minmax;

  int                    m_active_maskid; // selected mask id ( -1:none, 0...:mask_id)
  std::vector<MaskData>  m_mask_data    ;

public:
  //4D volumes (CPUに保持)
  std::vector< short* > m_img4d ; //orig volume
  std::vector< byte*  > m_flg4d ; //flag volume (典型例 0:locked, 1:背景,  255前景)
  std::vector< byte*  > m_mask4d; //mask volume (mask id)

  //3D volume to send GPU (privateにしたい)
  OglImage3D  m_vol     ;
  OglImage3D  m_vol_flg ;
  OglImage3D  m_vol_mask;
  OglImage1D<CH_RGBA>  m_img_maskcolor; // func: maskID    --> color

private:
  ImageCore();
public:
  ~ImageCore();

  static ImageCore* GetInst() {
    static ImageCore p;
    return &p;
  }

  //Image IO
  void load4DCT(std::vector<std::string> &files, std::vector<std::vector<std::string>> &filesInDirs, LOAD_FILE_TYPE type);
  void SaveImg4D_traw3dss(std::string fname);
  void SaveImg4D_mha     (std::string fname);

  //MASK IO -----------------------
  void LoadMask_Msk4  (std::string fname, int timeI);
  void LoadMask_Mha   (std::vector<std::string> fname, int timeI);
  void LoadMask_Trawub(std::vector<std::string> fnames, int frameI);
  void SaveMask_Msk4  (std::string fname);
  void SaveMask_Trawub(std::string fname);
  void SaveMask_Mha   (std::string fname);

  //Mesh (Mask) IO ----------------
  void importObjOne(const std::string& fname, const int frameI);
  void importObjAll(const std::vector<std::string>& fnames);
  void ExportMaskCentroid(const std::string& fname);
  void ExportMaskEigenvalue(const std::string& fname);

  // Mask Operation ----------------
  void SelectedMsk_delete();
  void SelectedMsk_marge(const int& trgtMaskID);
  void SelectedMsk_erodeOne(const int frameI);
  void SelectedMsk_dilateOne(const int frameI);
  void SelectedMsk_fillHoleOne(const int frameI);
  void SelectedMsk_erodeAll();
  void SelectedMsk_dilateAll();
  void SelectedMsk_fillHoleAll();
  void SelectedMsk_expObjOne(const std::string& fname, const int& frameI);
  void SelectedMsk_expObjAll(const std::string& fname);
  void SelectedMsk_SetColor(const EVec3i color);
  void SelectedMsk_SetLocked(const bool   tf);
  void SelectedMsk_SetAlpha(const float  alpha);
  void SetSelectMaskId(const int    idx);

  const std::vector<MaskData>& GetMaskData() { return m_mask_data; }
  int   GetSelectMaskIdx() { return m_active_maskid; }

  // 4D Image Get Info -------------------------
  EVec2i GetVolumeMinMax() { 
    if (m_volume_minmax[0] < -2500) m_volume_minmax[0] = -2500; 
    return m_volume_minmax; 
  }
  EVec3i GetReso()      { return m_reso; }
  EVec3f GetPitch()     { return m_pitch; }
  int    GetNumVoxels() { return m_reso[0] * m_reso[1] * m_reso[2]; }
  int    GetNumFrames() { return (int)m_img4d.size(); }

  //return tapple {width, height, depth, num_voxel}
  std::tuple<int, int, int> GetResolution3() {
    return std::forward_as_tuple(m_reso[0], m_reso[1], m_reso[2]);
  }

  //return tapple {width, height, depth, width*height, width*height*depth}
  std::tuple<int, int, int, int, int> GetResolution5() {
    int WH = m_reso[0] * m_reso[1];
    int WHD = m_reso[0] * m_reso[1] * m_reso[2];
    return std::forward_as_tuple(m_reso[0], m_reso[1], m_reso[2], WH, WHD);
  }

  EVec3f GetCuboidF() {
    return EVec3f(m_pitch[0] * m_reso[0], m_pitch[1] * m_reso[1], m_pitch[2] * m_reso[2]);
  }

  static EVec3f GetCuboid() { return GetInst()->GetCuboidF(); }

  void SetPitch(const EVec3f& pitch) {
    m_pitch = pitch;
  }

  inline int GetVoxelIndex(const EVec3f& position) {
    const int x = Crop(0, m_reso[0] - 1, (int)(position[0] / m_pitch[0]));
    const int y = Crop(0, m_reso[1] - 1, (int)(position[1] / m_pitch[1]));
    const int z = Crop(0, m_reso[2] - 1, (int)(position[2] / m_pitch[2]));
    return x + y * m_reso[0] + z * m_reso[0] * m_reso[1];
  }

  inline EVec4i GetVoxelIndexXYZI(const EVec3f& position) {
    const int x = Crop(0, m_reso[0] - 1, (int)(position[0] / m_pitch[0]));
    const int y = Crop(0, m_reso[1] - 1, (int)(position[1] / m_pitch[1]));
    const int z = Crop(0, m_reso[2] - 1, (int)(position[2] / m_pitch[2]));
    return EVec4i(x, y, z, x + y * m_reso[0] + z * m_reso[0] * m_reso[1]);
  }

  short GetVoxelValue(const int frameI, const EVec3f& position) {
    return m_img4d[frameI][GetVoxelIndex(position)];
  }



  void UpdateVisVolume(int winlv_min, int winlv_max, int frame_idx);
  void UpdateImgMaskColor();


  // m_flg4d Operation ----------------------
 
  // mask[f][i] is locked     --> flg4d[f][i] =  0 
  // mask[f][i] is not locked --> flg4d[f][i] =  1
  void InitializeFlg4dByMask(void (*progressfunc)(float));

  // m_flg4d[i]==255 --> m_mask4d[i] = new_mask_id
  void mask_storeCurrentForeGround();
    
 };


#endif


