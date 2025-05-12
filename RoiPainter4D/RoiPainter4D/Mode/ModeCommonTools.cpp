#pragma unmanaged
#include "ModeCommonTools.h"

#pragma managed
#include "../FormVisParam.h"
#include "../FormMain.h"
#pragma unmanaged

#include "ModeInterface.h"
#include "OglForCLI.h"
#include "CrsSecCore.h"
#include "ImageCore.h"

using namespace RoiPainter4D;




CRSSEC_ID PickCrssec(const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f& pos)
{
  return CrssecCore::PickCrssec(
    formVisParam_bPlaneXY(),
    formVisParam_bPlaneYZ(),
    formVisParam_bPlaneZX(),
    ImageCore::GetInst()->GetCuboidF(), ray_pos, ray_dir, pos);
}



CRSSEC_ID PickCrssec(
  const CRSSEC_ID trgt_id,
  const EVec3f& ray_pos,
  const EVec3f& ray_dir,
  EVec3f& pos)
{
  const EVec3f c = ImageCore::GetInst()->GetCuboidF();

  bool bxy = false, byz = false, bzx = false;
  if (trgt_id == CRSSEC_XY) bxy = true;
  else if (trgt_id == CRSSEC_YZ) byz = true;
  else if (trgt_id == CRSSEC_ZX) bzx = true;
  else return CRSSEC_NON;

  return CrssecCore::PickCrssec(bxy, byz, bzx, c, ray_pos, ray_dir, pos);
}



bool WheelingCrssec(const EVec2i& p, short z_delta, OglForCLI* ogl)
{
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);
  CRSSEC_ID id = PickCrssec(ray_pos, ray_dir, pos);

  if (id != CRSSEC_NON)
  {
    CrssecCore::MoveCrssec(
      ImageCore::GetInst()->GetReso(),
      ImageCore::GetInst()->GetPitch(), id, z_delta);
  }
  return id != CRSSEC_NON;
}



void UpdateImageCoreVisVolumes()
{
  //4D volume (cpu) --> vis volume (gpu)
  const int fi = formVisParam_getframeI();
  const int winlv_min = formVisParam_getWinLvMin();
  const int winlv_max = formVisParam_getWinLvMax();
  ImageCore::GetInst()->UpdateVisVolume(winlv_min, winlv_max, fi);
}



//Compute erosion for ImageCore::m_flg4
//m_flg4: 0:background (neverchange), 
//        1:background, 
//      255:foreground
void RunErosion3D_OneFrame_flg4(int frame_idx)
{
  std::vector<byte*>& flg4d = ImageCore::GetInst()->m_flg4d;
  const EVec3i reso         = ImageCore::GetInst()->GetReso();
  Erode3D(reso[0], reso[1], reso[2], flg4d[frame_idx]);
}

void RunErosion3D_AllFrame_flg4()
{
  std::vector<byte*>& flg4d = ImageCore::GetInst()->m_flg4d;
  const EVec3i reso         = ImageCore::GetInst()->GetReso();
  const int num_frames      = ImageCore::GetInst()->GetNumFrames();
#pragma omp parallel for 
  for (int fi = 0; fi < num_frames; ++fi)
    Erode3D(reso[0], reso[1], reso[2], flg4d[fi]);
}


//Compute Dilation for ImageCore::m_flg4
//m_flg4: 0:background (neverchange), 
//        1:background, 
//      255:foreground
void RunDilation3D_OneFrame_flg4(int frame_idx)
{
  std::vector<byte*>& flg4d = ImageCore::GetInst()->m_flg4d;
  const EVec3i reso = ImageCore::GetInst()->GetReso();
  Dilate3D(reso[0], reso[1], reso[2], flg4d[frame_idx]);
}

void RunDilation3D_AllFrame_flg4()
{
  std::vector<byte*>& flg4d = ImageCore::GetInst()->m_flg4d;
  const EVec3i reso = ImageCore::GetInst()->GetReso();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
#pragma omp parallel for 
  for (int fi = 0; fi < num_frames; ++fi)
    Dilate3D(reso[0], reso[1], reso[2], flg4d[fi]);
}



//Compute FillHole for ImageCore::m_flg4
//m_flg4: 0:background (neverchange), 
//        1:background, 
//      255:foreground
void RunFillHole3D_AllFrame_flg4()
{
  const EVec3i    reso = ImageCore::GetInst()->GetReso();
  const int  num_frames = ImageCore::GetInst()->GetNumFrames();
  const int  num_voxels = ImageCore::GetInst()->GetNumVoxels();

  std::cout << "RunFillHole3D_EachFrame...\n";

  byte* flgs = new byte[num_voxels];

  for (int fi = 0; fi < num_frames; fi++)
  {
    byte* flg3d = ImageCore::GetInst()->m_flg4d[fi];

#pragma omp parallel for 
    for (int vi = 0; vi < num_voxels; ++vi)
      flgs[vi] = (flg3d[vi] == 255) ? 255 : 0;

    FillHole3D(reso[0], reso[1], reso[2], flgs);

#pragma omp parallel for 
    for (int i = 0; i < num_voxels; ++i)
      if (flg3d[i] == 1)
        flg3d[i] = (flgs[i] == 255) ? 255 : 1;

    formMain_SetProgressValue(fi / float(num_frames));
  }
  formMain_SetProgressValue(0);

  delete[] flgs;

  std::cout << "...DONE...\n";
}


bool bForeVoxelExist_flg4()
{
  const std::vector<byte*>& flg4d = ImageCore::GetInst()->m_flg4d;
  const int  num_frame = ImageCore::GetInst()->GetNumFrames();
  const int  num_voxel = ImageCore::GetInst()->GetNumVoxels();

  bool b_foreexist = false;

  for (int fi = 0; fi < num_frame && !b_foreexist; ++fi)
  {
    for (int i = 0; i < num_voxel && !b_foreexist; ++i)
    {
      if (flg4d[fi][i] == 255) b_foreexist = true;
    }
  }
  return b_foreexist;
}