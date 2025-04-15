#ifndef MODE_COMMONTOOLS_H_
#define MODE_COMMONTOOLS_H_

#pragma unmanaged
// common tool used by Mode[Ref/Seg/Std]* 
// include only from CPP

#include "OglForCLI.h"




CRSSEC_ID PickCrssec(const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f& pos);
CRSSEC_ID PickCrssec(const CRSSEC_ID trgt_id,
  const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f& pos);

//pick and move cross section by wheeling
bool WheelingCrssec(const EVec2i& p, short z_delta, OglForCLI* ogl);
void UpdateImageCoreVisVolumes();


inline EMat3f CalcObjectRotationMatrixByMouseDragF(
  OglForCLI* ogl,
  int cursor_dx, //mouse move distance
  int cursor_dy, //mouse move distance
  float coef = 0.007f
)
{
  EVec3f cam_pos = ogl->GetCamPos();
  EVec3f cam_cnt = ogl->GetCamCnt();
  EVec3f cam_up = ogl->GetCamUp();
  EVec3f xdir = (cam_cnt - cam_pos).cross(cam_up);
  xdir.normalize();

  float dx = coef * cursor_dx;
  float dy = coef * cursor_dy;
  EMat3f R1(Eigen::AngleAxisf(dy, xdir));
  EMat3f R2(Eigen::AngleAxisf(dx, cam_up));
  return R1 * R2;
}


inline EMat3d CalcObjectRotationMatrixByMouseDrag(
  OglForCLI* ogl,
  int cursor_dx, //mouse move distance
  int cursor_dy, //mouse move distance
  double coef = 0.007
)
{
  EVec3f cam_pos = ogl->GetCamPos();
  EVec3f cam_cnt = ogl->GetCamCnt();
  EVec3f cam_up = ogl->GetCamUp();
  EVec3f xdir = (cam_cnt - cam_pos).cross(cam_up);
  xdir.normalize();

  double dx = coef * cursor_dx;
  double dy = coef * cursor_dy;
  EMat3d R1(Eigen::AngleAxisd(dy, xdir.cast<double>()));
  EMat3d R2(Eigen::AngleAxisd(dx, cam_up.cast<double>()));
  return R1 * R2;
}



inline EVec3d CalcObjectTransVectorByMouseDrag(
  OglForCLI* ogl,
  int cursor_dx, //mouse move distance
  int cursor_dy, //mouse move distance
  double coef = 0.1
)
{
  EVec3f cam_pos = ogl->GetCamPos();
  EVec3f cam_cnt = ogl->GetCamCnt();
  EVec3f cam_up = ogl->GetCamUp();
  EVec3f xdir = (cam_cnt - cam_pos).cross(cam_up);
  xdir.normalize();

  double dx = coef * cursor_dx;
  double dy = -coef * cursor_dy;
  return dx * xdir.cast<double>() + dy * cam_up.cast<double>();
}


inline EVec3f CalcObjectTransVectorByMouseDragF(
  OglForCLI* ogl,
  int cursor_dx, //mouse move distance
  int cursor_dy, //mouse move distance
  float coef = 0.1f
)
{
  EVec3f cam_pos = ogl->GetCamPos();
  EVec3f cam_cnt = ogl->GetCamCnt();
  EVec3f cam_up = ogl->GetCamUp();
  EVec3f xdir = (cam_cnt - cam_pos).cross(cam_up);
  xdir.normalize();

  float dx = coef * cursor_dx;
  float dy = -coef * cursor_dy;
  return dx * xdir + dy * cam_up;
}


//ImageCore::m_flg4D に対する処理
void RunErosion3D_AllFrame_flg4();
void RunFillHole3D_AllFrame_flg4();
void RunDilation3D_AllFrame_flg4();

bool bForeVoxelExist_flg4();







static const char* MESSAGE_NO_FOREGROUND =
"前景領域がありません\n No foreground voxel exist\n";

static const char* MESSAGE_TRUSH_LEAVE =
"現在の分割結果を破棄してモードを変更しますか？\n Do you want to trush the current data and leave this mode?";

#endif