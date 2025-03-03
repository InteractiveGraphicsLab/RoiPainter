#pragma unmanaged 
#include "ModeSegPixPaint.h"
#include "ModeCommonTools.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"
#include <iostream>

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegPixPaint.h"
#include "CliMessageBox.h"
#pragma unmanaged 

using namespace RoiPainter4D;




ModeSegPixPaint::ModeSegPixPaint()
{
  std::cout << "ModeSegPixPaint...\n";

  m_bL = m_bR = m_bM = false;
  m_b_paintpixels = false;
  m_b_drawlasso   = false;
  m_b_modified    = false;

  std::cout << "ModeSegPixPaint DONE\n";
}

ModeSegPixPaint::~ModeSegPixPaint()
{
}


bool ModeSegPixPaint::CanEndMode()
{
  if (!m_b_modified) return true;
  return ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Leaving?");
}



void ModeSegPixPaint::FinishSegmentation()
{
  std::vector<byte*> &flg4d = ImageCore::GetInst()->m_flg4d;
  const EVec3i   reso  = ImageCore::GetInst()->GetReso();
  const int  num_frame = (int)flg4d.size();
  const int  num_voxel = reso[0] * reso[1] * reso[2];

  bool b_roiexist = false;

  for (int fi = 0; fi < num_frame && !b_roiexist; ++fi)
  {
    for (int i = 0; i < num_voxel && !b_roiexist; ++i)
    {
      if (flg4d[fi][i] == 255) b_roiexist = true;
    }
  }

  if (!b_roiexist)
  {
    ShowMsgDlg_OK(MESSAGE_NO_FOREGROUND, "no foreground");
    return;
  }

  ImageCore::GetInst()->mask_storeCurrentForeGround();

  //このタイミングでisModified = false (しないとcanEndModeでダイアログが表示される)
  m_b_modified = false;
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  formMain_RedrawMainPanel();
}



void ModeSegPixPaint::CancelSegmentation()
{
  if (!m_b_modified || ShowMsgDlgYesNo("現在の分割結果を破棄してよいですか\nDo you want to cancel current segmentation?", "cancel?"))
  {
    m_b_modified = false;
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    formMain_RedrawMainPanel();
  }
}



void ModeSegPixPaint::StartMode()
{
  //initialize field
  m_bL = m_bR = m_bM = false;
  m_b_paintpixels = m_b_drawlasso = m_b_modified = false;

  //show dialog
  formSegPixPaint_Show();

  //initialzie vFlg
  ImageCore::GetInst()->InitializeFlg4dByMask( formMain_SetProgressValue );

  //4D volume (cpu) --> vis volume (gpu)
  UpdateImageCoreVisVolumes();

  std::cout << "finish ModeSegPixPaint::startMode!!!!\n";
}




static void c_DrawCubes
(
  const std::vector<EVec4i> &pixs,
  const EVec3f &pitch,
  const bool   &tf
)
{
  if (tf) glColor3d(0.2, 0.8, 1.0);
  else    glColor3d(0.9, 0.9, 0.9);

  const float x = pitch[0];
  const float y = pitch[1];
  const float z = pitch[2];

  for (const auto itr : pixs)
  {
    glPushMatrix();
    glTranslated(itr[0]*x, itr[1]*y, itr[2]*z);
    glDisable(GL_CULL_FACE);
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(0, 0, z); glVertex3f(0, y, z); glVertex3f(0, y, 0);
    glNormal3f(0, -1, 0); glVertex3f(0, 0, 0); glVertex3f(x, 0, 0); glVertex3f(x, 0, z); glVertex3f(0, 0, z);
    glNormal3f(0, 0, -1); glVertex3f(0, 0, 0); glVertex3f(0, y, 0); glVertex3f(x, y, 0); glVertex3f(y, 0, 0);
    glNormal3f(1, 0, 0 ); glVertex3f(x, 0, 0); glVertex3f(x, 0, z); glVertex3f(x, y, z); glVertex3f(x, y, 0);
    glNormal3f(0, 1, 0 ); glVertex3f(0, y, 0); glVertex3f(x, y, 0); glVertex3f(x, y, z); glVertex3f(0, y, z);
    glNormal3f(0, 0, 1 ); glVertex3f(0, 0, z); glVertex3f(0, y, z); glVertex3f(x, y, z); glVertex3f(y, 0, z);
    glEnd();
    glEnable(GL_CULL_FACE);
    glPopMatrix();
  }
}



///////////////////////////////////////////////////////////////////////////////
//Functions for Lasso /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//注目している点が lasso の内側であるか判定
static bool t_IsInsideLasso
(
	const EVec3f         &pos, 
	const std::vector<EVec3f> &lasso_stroke,
	const CRSSEC_ID      &id
) 
{ 
  const int N = (int)lasso_stroke.size();

	EVec3f axis = (id == CRSSEC_XY) ? EVec3f(0, 0, 1) :
		            (id == CRSSEC_YZ) ? EVec3f(1, 0, 0) : EVec3f(0, 1, 0);

	double sum = 0;
  for( int i=0; i < N; ++i)
  {
		sum += CalcAngle(lasso_stroke[ i ]-pos, lasso_stroke[ (i==N-1)?0:i+1 ]-pos, axis);
  }

	return fabs(2 * M_PI - fabs(sum)) < M_PI * 0.5;
}



//lassoの内側にあるvoxelを fore/backに変更
//b_fore = true  --> vFlg 1   --> 255に
//b_fore = false --> vFlg 255 --> 1に
static void t_addPixsInsideLasso
(
	const CRSSEC_ID       id   ,
	const EVec3i          reso ,
	const EVec3f          pitch,
	const std::vector<EVec3f> &lasso_stroke,
	const bool            b_fore,

   		  byte* vol_flg
)
{
	const int W = reso[0];
	const int H = reso[1];
	const int D = reso[2], WH = W*H;

  std::vector<EVec3f> lasso;
  const int new_num = std::max( 10, std::min( (int)lasso_stroke.size(), (int)( VertsLength(lasso_stroke, true)/pitch[0]) ));

  VertsResampleEqualInterval( new_num, lasso_stroke, lasso);

  //compute bounding box
  EVec3f tmp_min, tmp_max;
  CalcBoundingBox( lasso, tmp_min, tmp_max);
  EVec3i bb_min ( (int) (tmp_min[0]/pitch[0] ), (int) (tmp_min[1]/pitch[1] ), (int) (tmp_min[2]/pitch[2] ) );
  EVec3i bb_max ( (int) (tmp_max[0]/pitch[0] ), (int) (tmp_max[1]/pitch[1] ), (int) (tmp_max[2]/pitch[2] ) );
  bb_min[0] = std::max( 0 ,bb_min[0] - 1);
  bb_min[1] = std::max( 0 ,bb_min[1] - 1);
  bb_min[2] = std::max( 0 ,bb_min[2] - 1);
  bb_max[0] = std::min(W-1,bb_max[0] + 1);
  bb_max[1] = std::min(H-1,bb_max[1] + 1);
  bb_max[2] = std::min(D-1,bb_max[2] + 1);
  
	if (id == CRSSEC_XY) 
  {
		const float zpos = lasso.front()[2];
		const int   zi   = (int)(zpos / pitch[2]);

#pragma omp parallel for
		for (int yi = bb_min[1]; yi < bb_max[1]; ++yi)
    {
		  for (int xi = bb_min[0]; xi < bb_max[0]; ++xi)
		  {
        int idx = xi + yi * W + zi * WH;
        if( vol_flg[idx] == 0 ) continue;
			  if (  b_fore && vol_flg[idx] ==255 ) continue;
			  if ( !b_fore && vol_flg[idx] ==1   ) continue;
      
			  EVec3f p((xi + 0.5f) * pitch[0], (yi + 0.5f) * pitch[1], zpos);
        if ( t_IsInsideLasso(p, lasso, id) ) vol_flg[idx] = b_fore ? 255 : 1;
		  }
    }
	}
	
	if (id == CRSSEC_YZ) 
  {
		const float xpos = lasso.front()[0];
		const int   xi   = (int)(xpos / pitch[0]);

#pragma omp parallel for
		for (int zi = bb_min[2]; zi < bb_max[2]; ++zi)
    {
		  for (int yi = bb_min[1]; yi < bb_max[1]; ++yi)
		  {
        int idx = xi + yi * W + zi * WH;
        if( vol_flg[idx] == 0 ) continue;
        if (  b_fore && vol_flg[idx] ==255 ) continue;
			  if ( !b_fore && vol_flg[idx] ==1   ) continue;

			  EVec3f p(xpos, (yi + 0.5f) * pitch[1], (zi + 0.5f) * pitch[2]);
        if ( t_IsInsideLasso(p, lasso, id) ) vol_flg[idx] = b_fore ? 255 : 1;
	    }
    }
  }

	if (id == CRSSEC_ZX) 
  {
		const float ypos = lasso.front()[1];
		const int   yi   = (int)(ypos / pitch[1]);

#pragma omp parallel for
		for (int zi = bb_min[2]; zi < bb_max[2]; ++zi)
    {
		  for (int xi = bb_min[0]; xi < bb_max[0]; ++xi)
		  {
        int idx = xi + yi * W + zi * WH;
        if( vol_flg[idx] == 0 ) continue;
        if (  b_fore && vol_flg[idx] ==255 ) continue;
			  if ( !b_fore && vol_flg[idx] ==1   ) continue;
      
			  EVec3f p((xi + 0.5f) * pitch[0], ypos, (zi + 0.5f) * pitch[2]);
        if ( t_IsInsideLasso(p, lasso, id) ) vol_flg[idx] = b_fore ? 255 : 1;
		  }
    }
	}
}







////////////////////////////////////////////////////////////////////////////
//MouseListener/////////////////////////////////////////////////////////////

void ModeSegPixPaint::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;

  if (IsShiftKeyOn())
  {
    if (formSegPixPaint_bLassoMode() || formSegPixPaint_bLassoAllMode())
    {
      m_b_drawlasso = false;

      EVec3f ray_pos, ray_dir, pos;
      ogl->GetCursorRay(p, ray_pos, ray_dir);
      m_trgt_crssecid = PickCrssec(ray_pos, ray_dir, pos);

      if (m_trgt_crssecid != CRSSEC_NON) m_b_drawlasso = true;
    }
		else 
    {
      m_b_paintpixels = true;
    }
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }
}



void ModeSegPixPaint::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
  if (IsShiftKeyOn())
  {
    if ( formSegPixPaint_bLassoMode() || formSegPixPaint_bLassoAllMode())
    {
      m_b_drawlasso = false;

      EVec3f ray_pos, ray_dir, pos;
      ogl->GetCursorRay(p, ray_pos, ray_dir);
      m_trgt_crssecid = PickCrssec(ray_pos, ray_dir, pos);

      if(m_trgt_crssecid != CRSSEC_NON) m_b_drawlasso = true;
    }
		else 
    {
      m_b_paintpixels = true;
    }
  }
  else
  {
    ogl->BtnDown_Rot(p);
  }
}




void ModeSegPixPaint::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  std::vector<byte*> &flg4d = ImageCore::GetInst()->m_flg4d;
  const EVec3i   reso  = ImageCore::GetInst()->GetReso();
  const EVec3f  pitch  = ImageCore::GetInst()->GetPitch();
  const int  frame_idx = formVisParam_getframeI();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  if ( m_b_paintpixels )
  {
    for ( auto itr : m_paintvoxels) 
    {
      if ( flg4d[frame_idx][ itr[3] ] == 1) flg4d[frame_idx][ itr[3] ] = 255;
    }
    m_b_modified = true;
    UpdateImageCoreVisVolumes();
  }
  else if ( m_b_drawlasso && formSegPixPaint_bLassoMode())
  {
    t_addPixsInsideLasso(m_trgt_crssecid, reso, pitch, m_lasso, m_bL, flg4d[frame_idx] );
    m_b_modified = true;
    UpdateImageCoreVisVolumes();
  }
  else if ( m_b_drawlasso && formSegPixPaint_bLassoAllMode())
  {
    for (int i = 0; i < num_frames; i++) {
      t_addPixsInsideLasso(m_trgt_crssecid, reso, pitch, m_lasso, m_bL, flg4d[i]);
      m_b_modified = true;
      UpdateImageCoreVisVolumes();
    }
  }

  m_lasso.clear();
  m_paintvoxels.clear();
  m_bL = false;
  m_b_paintpixels = m_b_drawlasso = false;

  ogl->BtnUp();
  formMain_RedrawMainPanel();
}



void ModeSegPixPaint::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  std::vector<byte*> &flg4d = ImageCore::GetInst()->m_flg4d;
  const EVec3i   reso  = ImageCore::GetInst()->GetReso();
  const EVec3f  pitch  = ImageCore::GetInst()->GetPitch();

  const int frame_idx = formVisParam_getframeI();

  const int num_frames = ImageCore::GetInst()->GetNumFrames();


  if (m_b_paintpixels)
  {
    for (auto itr : m_paintvoxels)
    {
      if (flg4d[frame_idx][itr[3]] == 255) flg4d[frame_idx][itr[3]] = 1;
    }
    UpdateImageCoreVisVolumes();
    m_b_modified = true;
  }
  else if (m_b_drawlasso && formSegPixPaint_bLassoMode())
  {
    t_addPixsInsideLasso(m_trgt_crssecid, reso, pitch, m_lasso, m_bL, flg4d[frame_idx] );
    UpdateImageCoreVisVolumes();
    m_b_modified = true;
  }
  else if (m_b_drawlasso && formSegPixPaint_bLassoAllMode())
  {
    for (int i = 0; i < num_frames; i++) {
      t_addPixsInsideLasso(m_trgt_crssecid, reso, pitch, m_lasso, m_bL, flg4d[i]);
      UpdateImageCoreVisVolumes();
      m_b_modified = true;
    }
  }

  m_lasso.clear();
  m_paintvoxels.clear();

  m_bR = m_b_paintpixels = m_b_drawlasso = false;
  ogl->BtnUp();

  formMain_RedrawMainPanel();
}



void ModeSegPixPaint::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}

void ModeSegPixPaint::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}




void ModeSegPixPaint::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if ( m_b_paintpixels )
  {
    if (PickCrssec(ray_pos, ray_dir, pos) == CRSSEC_NON) return;
    std::vector<byte*>  &flg4d = ImageCore::GetInst()->m_flg4d;
    const EVec4i      vox = ImageCore::GetInst()->GetVoxelIndexXYZI(pos);
    const int   frame_idx = formVisParam_getframeI();

    if ( m_bL && flg4d[frame_idx][ vox[3] ] == 1  ) m_paintvoxels.push_back( vox );
    if (!m_bL && flg4d[frame_idx][ vox[3] ] == 255) m_paintvoxels.push_back( vox );
  }
  else if (m_b_drawlasso)
  {
    if ( PickCrssec(m_trgt_crssecid, ray_pos, ray_dir, pos ) != CRSSEC_NON)
    {
      m_lasso.push_back( pos );
    }
  }
  else
  {
    ogl->MouseMove(p);
  }

  formMain_RedrawMainPanel();
}


void ModeSegPixPaint::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  if(!WheelingCrssec(p, z_delta, ogl) ) {
    ogl->ZoomCamera(z_delta * 0.1f);
  }
  formMain_RedrawMainPanel();
}


void ModeSegPixPaint::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegPixPaint::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegPixPaint::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}

void ModeSegPixPaint::KeyDown(int nChar) {}
void ModeSegPixPaint::KeyUp(int nChar) {}



void ModeSegPixPaint::DrawScene(const EVec3f &cuboid, const EVec3f &cam_pos, const EVec3f &cam_cnt)
{
  const EVec3i reso = ImageCore::GetInst()->GetReso();

  BindAllVolumes();
  DrawCrossSectionsVisFore(!IsSpaceKeyOn());

  if (formVisParam_bRendVol())
  {
    const bool b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolumeVisFore(b_onmanip, !IsSpaceKeyOn(), cam_pos, cam_cnt);
  }

  const EVec3f pitch = ImageCore::GetInst()->GetPitch();
  if (m_b_paintpixels) c_DrawCubes(m_paintvoxels, pitch, m_bL);

  if (m_b_drawlasso) 
  {
    EVec3f ofset = (cam_pos - cam_cnt).normalized() * 0.5;
    glLineWidth(2);
    glColor3d(0.2, 0.4, 1.0);

    glPushMatrix();
    glTranslated(ofset[0], ofset[1], ofset[2]);
    glBegin(GL_LINE_LOOP);
    for (auto itr : m_lasso) glVertex3f(itr.x(), itr.y(), itr.z());
    glEnd();
    glPopMatrix();
  }
}



#pragma managed 

