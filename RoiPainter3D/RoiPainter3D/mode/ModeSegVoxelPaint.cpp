#pragma unmanaged 
#include "ModeSegVoxelPaint.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"
#include <algorithm>
#include "OglForCLI.h"

#pragma managed 
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegVoxelPaint.h"
#include "FormMaskIDselection.h"
#include "climessagebox.h"

#pragma unmanaged 

using namespace RoiPainter3D;

//volFlg
//  0 : locked and never modified 
//  1 : background
// 255: forreground
ModeSegVoxelPaint::~ModeSegVoxelPaint()
{

}


ModeSegVoxelPaint::ModeSegVoxelPaint() :
	m_volume_shader("shader/volVtx.glsl"   , "shader/volFlg_Seg.glsl"   ),
	m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
  m_bR = m_bL = m_bM = false;
  m_b_refinementmode = false;
  m_b_lassomode = false;
  m_b_paintmode = false;
}


bool ModeSegVoxelPaint::CanLeaveMode()
{
  return true;
}


void ModeSegVoxelPaint::StartMode()
{
  formSegVoxelPaint_Show();

  m_bR = m_bL = m_bM = false;
  m_b_refinementmode = false;
	m_b_lassomode = false;
  m_b_paintmode = false;


  if( ModeCore::GetInst()->GetCurrentModeId() == MODE_SEG_VOXPAINT)
  {
    ImageCore::GetInst()->InitializeVolFlgByLockedMask();
  }
  else if( ModeCore::GetInst()->GetCurrentModeId() == MODE_REF_VOXPAINT )
  {
    //REFINEMENT mode
    m_b_refinementmode = true;
    m_refine_maskid = formMaskIdSelection_DoSingleSelection(
      ImageCore::GetInst()->GetMaskData(), 
      ImageCore::GetInst()->GetActiveMaskID());

    if( m_refine_maskid  == -1)
    {
      ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
      return;
    }
    if( m_refine_maskid  == 0 )
    {
      CLI_MessageBox_OK_Show("MASK_id = 0 is not editable here ", "message");
      ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
      return;
    }

    ImageCore::GetInst()->InitializeVolFlgByLockedMask( m_refine_maskid );
  }
  else
  {
    //never comes here 
    ModeCore::GetInst()->ModeSwitch( MODE_VIS_NORMAL );
    return;
  }

  CrssecCore::GetInst()->ClearCurvedCrossec();

  //Lock/Unlock pitch box 
  formVisParam_UnlockPitchBox();
  
}






void ModeSegVoxelPaint::FinishSegmentation()
{
  const int   num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const byte *vFlg       = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();


  if ( m_b_refinementmode )
  {
    //refinement mode
    byte* mask3d = ImageCore::GetInst()->m_vol_mask.GetVolumePtr();

    for (int i = 0; i < num_voxels; ++i) 
    {
      if ( vFlg[i] == 255 ){
        mask3d[i] = m_refine_maskid ;
      }
      else if( vFlg[i] == 1 && mask3d[i] == m_refine_maskid ) {
        mask3d[i] = 0 ;
      }
    }
    ImageCore::GetInst()->m_vol_mask.SetUpdated();
    ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
  }
  else 
  {
    //segmentation mode
    if (!ImageCore::GetInst()->bForeVoxelsExistInVolFlg()) 
    {
      CLI_MessageBox_OK_Show("No foreground pixel exist", "caution");
      return;
    }
    ImageCore::GetInst()->StoreForegroundAsNewMask();
    ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
  }

}


void ModeSegVoxelPaint::CancelSegmentation()
{
    ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
    RedrawScene();
}





///////////////////////////////////////////////////////////////////////////////
//Functions for Lasso /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////





///////////////////////////////////////////////////////////////////////////////
//Mouse Listener //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ModeSegVoxelPaint::LBtnDown( const EVec2i &p, OglForCLI *ogl)
{

  m_bL = true;
  m_paint_voxels.clear();
  m_lasso.clear();
		
	if ( IsShiftKeyOn() )
	{
		if ( formSegVoxelPaint_bLassoMode() ) 
    {
      m_b_lassomode  = false;
      EVec3f rayP, rayD, pos;
      ogl->GetCursorRay(p, rayP, rayD);
      m_trgt_crssecid = PickCrssec(rayP, rayD, &pos);

      if(m_trgt_crssecid != CRSSEC_NON) m_b_lassomode = true;
    }
		else 
    {
      m_b_paintmode = true;
    }
	}
	else
	{
		ogl->BtnDown_Trans( p );
	}
}


void ModeSegVoxelPaint::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
  m_paint_voxels.clear();
  m_lasso.clear();
		
	if ( IsShiftKeyOn() )
	{
		if ( formSegVoxelPaint_bLassoMode() ) 
    {
      m_b_lassomode  = true;
      EVec3f rayP, rayD, pos;
      ogl->GetCursorRay(p, rayP, rayD);
      m_trgt_crssecid = PickCrssec(rayP, rayD, &pos);

      if(m_trgt_crssecid != CRSSEC_NON) m_b_lassomode = true;
    }
		else 
    {
      m_b_paintmode = true;
    }
	}
	else
	{
		ogl->BtnDown_Rot( p );
	}
}

void ModeSegVoxelPaint::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnDown_Zoom(p);
	m_bM = true;
}



void ModeSegVoxelPaint::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  byte* vol_flg = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();

	if (m_b_paintmode) 
	{
		for (auto itr : m_paint_voxels) if( vol_flg[itr[3]] == 1 ) vol_flg[itr[3]] = 255;
		ImageCore::GetInst()->m_vol_flag.SetUpdated();
	}
	if (m_b_lassomode)
	{
    const EVec3i reso  = ImageCore::GetInst()->GetResolution();
    const EVec3f pitch = ImageCore::GetInst()->GetPitch();
    const EVec3f cube  = ImageCore::GetInst()->GetCuboid();

		t_AddPixsInsideLasso( m_trgt_crssecid, reso, pitch, m_lasso, m_bL, vol_flg);
		ImageCore::GetInst()->m_vol_flag.SetUpdated();
	}

	m_lasso.clear();
	m_paint_voxels.clear();
	m_bL = m_b_paintmode = m_b_lassomode = false;
	ogl->BtnUp();
	RedrawScene();
}



void ModeSegVoxelPaint::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  byte* vol_flg = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();

	if (m_b_paintmode) 
	{
		for (auto itr : m_paint_voxels) if( vol_flg[itr[3]] == 255) vol_flg[itr[3]] = 1;
		ImageCore::GetInst()->m_vol_flag.SetUpdated();
	}
	if (m_b_lassomode)
	{
    const EVec3i reso  = ImageCore::GetInst()->GetResolution();
    const EVec3f pitch = ImageCore::GetInst()->GetPitch();
    const EVec3f cube  = ImageCore::GetInst()->GetCuboid();

		t_AddPixsInsideLasso(m_trgt_crssecid, reso, pitch, m_lasso, m_bL, vol_flg);
		ImageCore::GetInst()->m_vol_flag.SetUpdated();
	}

	m_lasso.clear();
	m_paint_voxels.clear();
	m_bR = m_b_paintmode = m_b_lassomode = false;
	ogl->BtnUp();
	RedrawScene();
}

void ModeSegVoxelPaint::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  RedrawScene();
}




void ModeSegVoxelPaint::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if (!m_bL && !m_bM && !m_bR) return;

	EVec3f ray_pos, ray_dir, pos;
	ogl->GetCursorRay(p, ray_pos, ray_dir);

	const OglImage3D &vFlg = ImageCore::GetInst()->m_vol_flag    ;

	if ( m_b_paintmode )
	{
		if ( PickCrssec(ray_pos, ray_dir, &pos) == CRSSEC_NON) return;
    EVec4i vi = ImageCore::GetInst()->GetVoxelIndex4i(pos);

		if( m_bL && vFlg[vi[3]] == 1 ) m_paint_voxels.push_back( vi );
		if(!m_bL && vFlg[vi[3]] ==255) m_paint_voxels.push_back( vi );
	}
	else if (m_b_lassomode) 
	{
		if ( PickCrsSec( m_trgt_crssecid, ray_pos, ray_dir, &pos) != CRSSEC_NON){
			m_lasso.push_back(pos);
		}
	}
	else
	{
		ogl->MouseMove(p);
	}	
  RedrawScene(true);
 }



void ModeSegVoxelPaint::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  if( !PickToMoveCrossSecByWheeling(p, ogl, z_delta) )
  {
    ogl->ZoomCamera(z_delta * 0.1f);
  }

  RedrawScene();
}



void ModeSegVoxelPaint::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegVoxelPaint::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegVoxelPaint::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}



void ModeSegVoxelPaint::KeyDown(int nChar) {
  RedrawScene();
}

void ModeSegVoxelPaint::KeyUp(int nChar) {
  RedrawScene();
}







static void t_drawCubes
(
	const std::vector<EVec4i> &pixIds,
	const EVec3f &pitch,
	const bool   &tf
) 
{
  static const float spec[4] = { 1,1,1,0.3f };
  static const float shin[1] = { 64.0f };
  static const float diff_f[4] = { 0.3f, 1.0f, 0.3f, 0.3f };
  static const float ambi_f[4] = { 0.3f, 1.0f, 0.3f, 0.3f };
  static const float diff_b[4] = { 1.0f, 0.3f, 0.3f, 0.3f };
  static const float ambi_b[4] = { 1.0f, 0.3f, 0.3f, 0.3f };

  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , tf ? diff_f : diff_b);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , tf ? ambi_f : ambi_b);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shin);

  const float x = pitch[0];
	const float y = pitch[1];
	const float z = pitch[2];

	for (const auto itr : pixIds) 
	{
		glPushMatrix();
      glEnable(GL_LIGHTING);
			glTranslated(itr.x()*x,  itr.y()*y, itr.z()*z);
			glDisable(GL_CULL_FACE);
			glBegin(GL_QUADS);
				glNormal3f(-1,0, 0); glVertex3f(0, 0, 0); glVertex3f(0, 0, z); glVertex3f(0, y, z); glVertex3f(0, y, 0);
				glNormal3f(0,-1, 0); glVertex3f(0, 0, 0); glVertex3f(x, 0, 0); glVertex3f(x, 0, z); glVertex3f(0, 0, z);
				glNormal3f(0, 0,-1); glVertex3f(0, 0, 0); glVertex3f(0, y, 0); glVertex3f(x, y, 0); glVertex3f(y, 0, 0);
				glNormal3f(1, 0, 0); glVertex3f(x, 0, 0); glVertex3f(x, 0, z); glVertex3f(x, y, z); glVertex3f(x, y, 0);
				glNormal3f(0, 1, 0); glVertex3f(0, y, 0); glVertex3f(x, y, 0); glVertex3f(x, y, z); glVertex3f(0, y, z);
				glNormal3f(0, 0, 1); glVertex3f(0, 0, z); glVertex3f(0, y, z); glVertex3f(x, y, z); glVertex3f(y, 0, z);
			glEnd();
			glEnable(GL_CULL_FACE);
      glDisable(GL_LIGHTING);
		glPopMatrix();
	}
}



void ModeSegVoxelPaint::DrawScene
(
    const EVec3f &cuboid, 
    const EVec3f &camP  ,
    const EVec3f &camF
)
{
  const bool   bDrawVol = formVisParam_bRendVol();
  const EVec3i reso     = ImageCore::GetInst()->GetResolution();
  const EVec3f pitch    = ImageCore::GetInst()->GetPitch();

  //bind volumes ---------------------------------------
  BindAllVolumes();

  //draw planes
  DrawCrossSections(cuboid, reso, m_crssec_shader);

  
  if (bDrawVol && !IsSpaceKeyOn())
  {
    const bool  b_pse   = formVisParam_bDoPsued();
    const float alpha   = formVisParam_getAlpha();
    const bool  b_roi   = formVisParam_GetOtherROI();
    const bool  b_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int   n_slice = (int)((b_manip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());


    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    m_volume_shader.Bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, b_pse, b_roi);
    t_DrawCuboidSlices(n_slice, camP, camF, cuboid);
    m_volume_shader.Unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }


	if ( m_b_paintmode )
  {
    t_drawCubes( m_paint_voxels, pitch, m_bL);
  }

	if (m_b_lassomode) 
  {
		EVec3f ofset = (camP - camF).normalized() * 0.5;
		glLineWidth(2);

    if( m_bL ) 
      glColor3d(0.3, 1.0, 0.3);
    else       		
      glColor3d(1.0, 0.3, 0.3);

		glPushMatrix();
		glTranslated(ofset[0], ofset[1], ofset[2]);	
		glBegin(GL_LINE_LOOP);
		for (auto itr : m_lasso) glVertex3f(itr.x(), itr.y(), itr.z());
		glEnd();
		glPopMatrix();
	}
}




#pragma managed