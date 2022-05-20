#ifndef MODECOMMONTOOLS_H_
#define MODECOMMONTOOLS_H_
#pragma unmanaged

#include "tmath.h"
#include "Mode/GlslShader.h"

class OglForCLI;

void BindAllVolumes();

void DrawCrossSections(
  const EVec3f& cuboid,
  const EVec3i& reso,
  GlslShaderCrsSec& shader);

CRSSEC_ID PickCrssec(const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f* pos);
CRSSEC_ID PickCrsSec(const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f* pos, const bool b_xy, const bool b_yz, const bool b_zx);
CRSSEC_ID PickCrsSec(const CRSSEC_ID trgt_id, const EVec3f& ray_pos, const EVec3f& ray_dir, EVec3f* pos);

bool PickToMoveCrossSecByWheeling(const EVec2i& p, OglForCLI* ogl, short z_delta);


#endif