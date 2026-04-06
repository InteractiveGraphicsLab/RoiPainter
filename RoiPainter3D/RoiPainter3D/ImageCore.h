#ifndef MODE_IMAGECORE_H_
#define MODE_IMAGECORE_H_

//------------------------------------
//class ImageCore
//
//2018/10/31 
//written by Takashi Ijiri
//
//This class manages following images,
//+ volumetric image
//+ maskId     image
//+ transfer functions (as images)
//-------------------------------------

#pragma unmanaged
#include "OglForCLI.h"
#include "OglImage.h"
#include "tmath.h"
#include "tmesh.h"
#include "MaskData.h"
#include <vector>
#include <string>
#include <iostream>


class ImageCore
{  
  //singleton
private:
  ImageCore();
public:
  static ImageCore* GetInst() { static ImageCore p; return &p; }

private:
	//volume info 
	EVec3i      m_reso ;
	EVec3f      m_pitch;
  EVec2i      m_vol_minmax ;
	std::string m_filepath   ;

  std::vector<MaskData> m_mask_data     ;
	int m_active_maskid; // -1:none, 0...:maskID

public:
	//volume images
	short      *m_vol_orig  ; // original volume (1D array representation)
	float      *m_vol_origgm; // gradiente magnitude volume  (1D array representation)
	OglImage3D  m_vol       ; // 8 bit bolume (tone mapping) 
	OglImage3D  m_vol_flag  ; // 8 bit flag volume 
	OglImage3D  m_vol_mask  ; // 8 bit mask volume  
	OglImage3D  m_vol_gm    ; // 8 bit gradient magnitude volume

	OglImage1D<CH_RGBA> m_img_maskcolor ; // func: maskID    --> color


  //I/O Loaders volume 
  bool  LoadVolume(std::vector<std::string> fnames, std::string fext);
  bool  LoadVolume(std::string fname, std::string fext);
  void  LoadMask(const char* fname);
  void  SaveMask(const char* fname) const ;
  void  SaveVolumeAsTraw3dss(const char* fname) const;


  //update opengl volumes by linear tone mapping 
	void UpdateOGLVolume( short windowlv_min,  short windowlv_max);
  void UpdateOGLMaskColorImg();


	//getter for Volume Info (resolution/pitch/cuboid)
	EVec3f GetCuboid() const { 
    return EVec3f( (float)(m_reso[0] * m_pitch[0]), 
                   (float)(m_reso[1] * m_pitch[1]), 
                   (float)(m_reso[2] * m_pitch[2])); 
  }

	EVec3i GetResolution() const { return m_reso; } 
  
  std::tuple<int, int, int> GetResolution3() const {
    return std::forward_as_tuple( m_reso[0], m_reso[1], m_reso[2] ) ;
  }

  //return reso (W, H, D, WH, WHD)
  std::tuple<int, int, int, int, int> GetResolution5() const  
  {
    const int WH = m_reso[0] * m_reso[1];
    const int WHD = WH * m_reso[2];
    return std::forward_as_tuple(m_reso[0], m_reso[1], m_reso[2], WH, WHD); 
  }

  std::string GetFilePath () const { return m_filepath; }
  int         GetNumVoxels() const { return m_reso[0] * m_reso[1] * m_reso[2]; }
  EVec2i      GetVolMinMax() const { return m_vol_minmax; }
  EVec3f      GetPitch    () const { return m_pitch; }

  void   SetPitch(const EVec3f pitch) { m_pitch = pitch; }


  //position --> voxel index
  EVec4i GetVoxelIndex4i(const EVec3f& position) const
  {
    const int x = Crop<int>(0, m_reso[0] - 1, (int)(position[0] / m_pitch[0]));
    const int y = Crop<int>(0, m_reso[1] - 1, (int)(position[1] / m_pitch[1]));
    const int z = Crop<int>(0, m_reso[2] - 1, (int)(position[2] / m_pitch[2]));
    return EVec4i(x, y, z, x + y * m_reso[0] + z * m_reso[0] * m_reso[1]);
  }

  int GetVoxelIndex(const EVec3f& position) const
  {
    return GetVoxelIndex4i(position)[3];
  }

  short GetVoxelValue(const EVec3f& position) const
  {
    return m_vol_orig[ GetVoxelIndex(position) ];
  }

  //generate new region by using all voxels with (m_volFlg[i] == 255) 
  void StoreForegroundAsNewMask();


  ///////////////////////////////////////////////////////
  //mask manipuration
  int  GetActiveMaskID() const{ return m_active_maskid; }
  void SetActiveMaskID(int maskid) { m_active_maskid = maskid; }
  const std::vector<MaskData> &GetMaskData(){ return m_mask_data; }
  void ClearMaskSurface(int trgtid);
  void DrawMaskSurfaces();
  
  // manipuration for active (user-selected) mask id
  void ActiveMask_SetLocked  (const bool   tf    );
  void ActiveMask_SetRendSurf(const bool   tf    );
  void ActiveMask_SetAlpha   (const double alpha );
  void ActiveMask_SetColor   (const EVec3i &c    );
  void ActiveMask_Delete   ( );
  void ActiveMask_Erode    ( );
  void ActiveMask_Dilate   ( );
  void ActiveMask_ExportObj(const std::string &fname);

  void FillHole( std::set<int> &ids, int dilation_size);
  void MargeMaskIDs ( std::set<int> &ids );


  void InitializeVolFlgByLockedMask(int fore_maskid = -1);
  bool bForeVoxelsExistInVolFlg();
  void ExportMaskIDsAsOneMesh   ( std::set<int> mask_ids, const char *fname );
  void ExportAllMaskIdAsBmpSlice( const char *fname );
  void ExportOneMaskIdAsBmpSlice( int mask_id, const char *fname);
  void ExportVolumeAsBmpSlice   ( const char* fname, int xy_yz_zx /*0,1,2*/ );

private:
	void UpdateGradMagnituteVolume();
  void GetFlgVolByMask_0_1_255(const int trgt_maskid, byte* flgvol);
};

#endif


