#ifndef MODECOMMONTOOLS_H_
#define MODECOMMONTOOLS_H_
#pragma unmanaged

#include "tmath.h"
#include "Mode/GlslShader.h"

class OglForCLI;
class TMesh;

void BindAllVolumes();

void DrawCrossSections( GlslShaderCrsSec& shader);

void DrawCrsSec_Standard();
void DrawCrsSec_Segmentation();
void DrawCrsSec_Mask();
void DrawCrsSec_LocalRegionGrow();
void DrawSuface_Segmenation(const TMesh& mesh);

void DrawVolume_Standard    (const EVec3f &cam_pos, const EVec3f &cam_cnt, bool b_coarse_render);
void DrawVolume_Segmentation(const EVec3f& cam_pos, const EVec3f& cam_cnt, bool b_coarse_render);
void DrawVolume_Mask(const EVec3f& cam_pos, const EVec3f& cam_cnt, bool b_coarse_render);
void DrawVolume_LocalRegionGrow(const EVec3f& cam_pos, const EVec3f& cam_cnt, bool b_coarse_render);


CRSSEC_ID PickCrssec(const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f* pos);
CRSSEC_ID PickCrsSec(const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f* pos, const bool b_xy, const bool b_yz, const bool b_zx);
CRSSEC_ID PickCrsSec(const CRSSEC_ID trgt_id, const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f* pos);

bool PickToMoveCrossSecByWheeling(const EVec2i& p, OglForCLI* ogl, short z_delta);





#endif