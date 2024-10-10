#pragma unmanaged
#include "ModeCommonTools.h"

#pragma managed
#include "../FormVisParam.h"
#pragma unmanaged

#include "ModeInterface.h"
#include "OglForCLI.h"
#include "CrsSecCore.h"
#include "ImageCore.h"
#include "GlslShader.h"


#define ONMOVE_SLICE_RATE 0.2

using namespace RoiPainter4D;


void BindAllVolumes()
{
  glActiveTextureARB(GL_TEXTURE0);
  ImageCore::GetInst()->m_vol.BindOgl();
  //glActiveTextureARB(GL_TEXTURE1);
  //ImageCore::getInst()->m_volGmag.bindOgl();
  glActiveTextureARB(GL_TEXTURE2);
  ImageCore::GetInst()->m_vol_flg.BindOgl(false);
  glActiveTextureARB(GL_TEXTURE3);
  ImageCore::GetInst()->m_vol_mask.BindOgl(false);
  glActiveTextureARB(GL_TEXTURE4);
  formVisParam_bindTfImg();
  glActiveTextureARB(GL_TEXTURE5);
  formVisParam_bindPsuImg();
  glActiveTextureARB(GL_TEXTURE6);
  ImageCore::GetInst()->m_img_maskcolor.BindOgl(false);
}


void DrawCrossSections(
  const EVec3f& cuboid,
  const EVec3i& reso,
  const bool& do_hilight,
  GlslShaderCrsSec& shader)
{
  glColor3d(1, 1, 1);
  shader.bind(0, 1, 2, 3, 6, reso, false, do_hilight);
  CrssecCore::DrawCrssec(
    formVisParam_bPlaneXY(),
    formVisParam_bPlaneYZ(),
    formVisParam_bPlaneZX(), cuboid);
  shader.unbind();
}

void DrawVolumeSlices(
  const EVec3f& cuboid,
  const EVec3i& reso,
  const EVec3f& cam_pos,
  const EVec3f& cam_focus,
  const bool& do_hilight,
  const bool& is_manipulation,
  GlslShaderVolume& shader
)
{
  const bool  b_pseudo = formVisParam_bDoPsued();
  const float alpha = formVisParam_getAlpha();
  const int   num_slice = GetNumVolumeRenderSlice(is_manipulation);
  shader.bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, cam_pos, b_pseudo, !IsSpaceKeyOn());
  t_drawSlices(num_slice, cam_pos, cam_focus, cuboid);
  shader.unbind();
}







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



int  GetNumVolumeRenderSlice(bool b_during_manipuration)
{
  if (b_during_manipuration)
    return (int)(ONMOVE_SLICE_RATE * formVisParam_getSliceNum());
  else
    return formVisParam_getSliceNum();
}



