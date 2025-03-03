#pragma unmanaged
#include "ModeCommonTools.h"

#pragma managed
#include "../FormVisParam.h"
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





