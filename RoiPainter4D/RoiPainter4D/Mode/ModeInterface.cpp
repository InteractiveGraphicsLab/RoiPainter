#pragma unmanaged
#include "ModeInterface.h"

#pragma managed
#include "../FormVisParam.h"
#pragma unmanaged

#include "OglForCLI.h"
#include "CrsSecCore.h"
#include "ImageCore.h"
#include "GlslShader.h"

#define ONMOVE_SLICE_RATE 0.2

using namespace RoiPainter4D;



void ModeInterface::BindAllVolumes()
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



///////////////////////////////////////////////////////////////////////////////
//Draw Cross Sections//////////////////////////////////////////////////////////
static void DrawCrossSections(GlslShaderCrsSec &shader, bool do_hilight)
{
  glColor3d(1, 1, 1);
  shader.bind(0, 1, 2, 3, 6, ImageCore::GetReso(), false, do_hilight);
  CrssecCore::DrawCrssec(formVisParam_bPlaneXY(),
                         formVisParam_bPlaneYZ(),
                         formVisParam_bPlaneZX(),
                         ImageCore::GetCuboid());
  shader.unbind();
}


void ModeInterface::DrawCrossSectionsNormal()
{
  static GlslShaderCrsSec shader("shader/crssecVtx.glsl", 
                                 "shader/crssecFlg.glsl");
  DrawCrossSections(shader, false);
}

//Visualize mask color 
void ModeInterface::DrawCrossSectionsVisMask(bool do_hilight)
{
  static GlslShaderCrsSec shader("shader/crssecVtx.glsl", 
                                 "shader/crssecFlg_Msk.glsl");
  DrawCrossSections(shader, do_hilight);
}


//highlight foreground voxel (flg[i] = 255)
void ModeInterface::DrawCrossSectionsVisFore(bool do_hilight)
{
  static GlslShaderCrsSec shader("shader/crssecVtx.glsl", 
                                 "shader/crssecFlg_Seg.glsl");
  DrawCrossSections(shader, do_hilight);
}



///////////////////////////////////////////////////////////////////////////////
//Draw Volumes ////////////////////////////////////////////////////////////////


static void DrawVolumeSlices(
  const bool    is_manipulation,
  const bool    do_hilight,
  const EVec3f& cam_pos,
  const EVec3f& cam_cnt,
  GlslShaderVolume& shader
)
{
  bool  b_pseudo  = formVisParam_bDoPsued();
  float alpha     = formVisParam_getAlpha();
  int   num_slice = formVisParam_getSliceNum();
  if (is_manipulation) num_slice = (int) (num_slice * ONMOVE_SLICE_RATE);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  shader.bind(0, 1, 2, 3, 4, 5, 6, alpha, 
              ImageCore::GetReso(), 
              cam_pos, b_pseudo, !IsSpaceKeyOn());

  t_drawSlices(num_slice, cam_pos, cam_cnt, ImageCore::GetCuboid());

  shader.unbind();

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
}


void ModeInterface::DrawVolumeNormal(
  bool is_manip,
  const EVec3f& cam_pos,
  const EVec3f& cam_cnt)
{
  static GlslShaderVolume shader("shader/volVtx.glsl", "shader/volFlg.glsl");
  DrawVolumeSlices(is_manip, false, cam_pos, cam_cnt, shader);
}

void ModeInterface::DrawVolumeVisMask(
  bool is_manip, 
  bool do_hilight,
  const EVec3f& cam_pos,
  const EVec3f& cam_cnt)
{
  static GlslShaderVolume shader("shader/volVtx.glsl", "shader/volFlg_Msk.glsl");
  DrawVolumeSlices(is_manip, do_hilight, cam_pos, cam_cnt, shader);
}

void ModeInterface::DrawVolumeVisFore(
  bool is_manip,
  bool do_hilight,
  const EVec3f& cam_pos,
  const EVec3f& cam_cnt)
{
  static GlslShaderVolume shader("shader/volVtx.glsl", "shader/volFlg_Seg.glsl");
  DrawVolumeSlices(is_manip, do_hilight, cam_pos, cam_cnt, shader);
}




