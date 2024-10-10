#pragma unmanaged
#include "ModeRefStrokeTrim.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"
#include "time.h"
#include "LogCore.h"

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormRefStrokeTrim.h"
#include "FormMaskIDselection.h"
#include "climessagebox.h"

#pragma unmanaged

using namespace RoiPainter3D;


/*
memo undoの仕組み

1) ストロークが書かれたら
    1) m_undo_idx を 1進める
    2) m_redo_max に m_undo_idx を代入
    3) m_undo_vol 内の m_redo_idx以上の画素値を0に
    4) m_undo_vol 内の 削除された画素に m_undo_idxを代入

2) Undo実行
    1) m_undo_vol 内の m_undo_idx画素を 削除前の状態に
    2) m_undo_idxを1戻す

3) Redo実行
    1) m_undo_idx < m_redo_idxのときのみ実行可
    2) m_undo_idxを1進める
    3) m_undo_vol 内の m_undo_idx画素を 削除
*/






ModeRefStrokeTrim::~ModeRefStrokeTrim()
{
	if( m_undo_vol ) delete[] m_undo_vol;
}


ModeRefStrokeTrim::ModeRefStrokeTrim() :
	  m_volume_shader("shader/volVtx.glsl"   , "shader/volFlg_Seg.glsl"),
	  m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
  m_bL = m_bR = m_bM = m_b_drawingstroke = false;
  m_b_modified = false;

  m_undo_vol = 0;
  m_undo_idx = 0;
  m_redo_max = 0;
}


bool ModeRefStrokeTrim::CanLeaveMode()
{
  if( !m_b_modified ) return true;
	
  if (CLI_MessageBox_YESNO_Show("Current Result is not stored. Do you want to leave?", "message") )
	{
		return true;
	}
	return false;
}


//volFlg
// 0  : locked and never changed, 
// 1  : back  
//255 : fore 
void ModeRefStrokeTrim::StartMode()
{
	m_b_modified = false;
  
  m_trgt_maskid = formMaskIdSelection_DoSingleSelection( 
    ImageCore::GetInst()->GetMaskData(), 
    ImageCore::GetInst()->GetActiveMaskID());

  if( m_trgt_maskid < 0 ){
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }

  if( m_trgt_maskid == 0  ){
    CLI_MessageBox_OK_Show("Cant modify mask ID = 0", "message");
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }

  formRefStrokeTrim_Show();
  CrssecCore::GetInst()->ClearCurvedCrossec();

	const int  nun_voxels = ImageCore::GetInst()->GetNumVoxels();
  const byte *msk3d     = ImageCore::GetInst()->m_vol_mask.GetVolumePtr()  ;
	byte       *flg3d     = ImageCore::GetInst()->m_vol_flag.GetVolumePtr()  ;

	for (int i = 0; i < nun_voxels; ++i) 
    flg3d[i] = ( msk3d[i] == m_trgt_maskid) ? 255 : 0;
	
  ImageCore::GetInst()->m_vol_flag.SetUpdated();

  
  if ( m_undo_vol ) delete[] m_undo_vol;
  m_undo_vol = new unsigned short[nun_voxels];
	memset(m_undo_vol, 0, sizeof(unsigned short)*nun_voxels);
  m_undo_idx = 0;  
  m_redo_max = 0;

  //Lock/Unlock pitch box
  formVisParam_UnlockPitchBox();

  LogCore::GetInst()->StartLogger();
}




void ModeRefStrokeTrim::cancelSegmentation()
{
  if( m_undo_vol ) delete[] m_undo_vol;
  m_undo_vol = 0;

  ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
  RedrawScene();
}




void ModeRefStrokeTrim::finishSegmentation()
{
  //export log
  time_t currenttime = time(NULL);
  struct tm t;
  errno_t err;
  err = localtime_s(&t, &currenttime);
  std::string fname = 
      std::to_string( t.tm_year+1900 )
    + std::to_string( t.tm_mon + 1   )
    + std::string((t.tm_mday < 10 ) ? "0" : "") + std::to_string( t.tm_mday )
    + std::string((t.tm_hour < 10 ) ? "0" : "") + std::to_string( t.tm_hour )
    + std::string((t.tm_min  < 10 ) ? "0" : "") + std::to_string( t.tm_min  )
    + std::string((t.tm_sec  < 10 ) ? "0" : "") + std::to_string( t.tm_sec  )
    + std::string("_trimming_log.txt");
  LogCore::GetInst()->CloseLogger(fname);




	const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
	byte  *msk3d = ImageCore::GetInst()->m_vol_mask.GetVolumePtr();
	byte  *flg3d = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();


	//一旦 トリムされた画素のIDを0に
	for( int i= 0; i < num_voxels; ++i)
	{
		if( msk3d[i] == m_trgt_maskid && flg3d[i] != 255 ) msk3d[i] = 0;
	}
  ImageCore::GetInst()->ClearMaskSurface(m_trgt_maskid);

	//トリムされた領域を新しい領域として追加
  bool bTrimRegionExist = false;
	for( int i= 0; i < num_voxels; ++i) 
  {
    if( flg3d[i] == 1 ) bTrimRegionExist  = true;
    flg3d[i] = ( flg3d[i] == 1 ) ? 255 : 0;
  }
  if(bTrimRegionExist)
	  ImageCore::GetInst()->StoreForegroundAsNewMask();

	ImageCore::GetInst()->m_vol_mask.SetUpdated();
	ImageCore::GetInst()->m_vol_flag.SetUpdated();

	m_b_modified = false;

  if( m_undo_vol ) delete[] m_undo_vol;
  m_undo_vol = 0;

	ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
	RedrawScene();
}



////////////////////////////////////////////////////////////////////////
//Mouse Listener ///////////////////////////////////////////////////////

void ModeRefStrokeTrim::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeRefStrokeTrim::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeRefStrokeTrim::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}

void ModeRefStrokeTrim::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  LogCore::GetInst()->Add("LBtnDown");

  m_bL = true;
	m_stroke2d.clear();
	m_stroke3d.clear();

	if (IsShiftKeyOn())
	{
		m_b_drawingstroke = true;
		EVec3f rayP, rayD;
		ogl->GetCursorRay(p,rayP,rayD);

		m_stroke2d.push_back( p );
		m_stroke3d.push_back( rayP + 0.1f * rayD );
	}
	else 
  {
		ogl->BtnDown_Trans(p);
  }
}


void ModeRefStrokeTrim::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if( m_b_drawingstroke ) {
    UpdateVolFlgByStroke(ogl);
    LogCore::GetInst()->Add("DRAW_TRIMSTROKE");
  }

	ogl->BtnUp();
	m_bL = false;
	m_b_drawingstroke = false;
	m_stroke2d.clear();
	m_stroke3d.clear();
  RedrawScene();
}


void ModeRefStrokeTrim::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  LogCore::GetInst()->Add("RBtnDown");


  if (m_bL && m_b_drawingstroke)
	{
    //cancel current trimming stroke
		m_b_drawingstroke = false;
		m_bL = false;
		m_stroke2d.clear();
		m_stroke3d.clear();
		return;
	}
  else
  {
	  ogl->BtnDown_Rot (p); 
	  m_bR = true ;   
  }
}

void ModeRefStrokeTrim::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnUp();
  m_bR = false;
  RedrawScene();
}

void ModeRefStrokeTrim::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  LogCore::GetInst()->Add("MBtnDown");

  ogl->BtnDown_Zoom(p); 
  m_bM = true ; 
}

void ModeRefStrokeTrim::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnUp();
  m_bM = false;
  RedrawScene();
}

void ModeRefStrokeTrim::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if( !m_bL && !m_bR && !m_bM ) return;

	if (m_b_drawingstroke)
	{
		EVec3f rayP, rayD;
		ogl->GetCursorRay(p,rayP,rayD);
		m_stroke2d.push_back( p);
		m_stroke3d.push_back( rayP + 0.1f * rayD );
	}
	else
	{
		ogl->MouseMove( p );
	}
  RedrawScene( false );
}



void ModeRefStrokeTrim::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
  if( PickToMoveCrossSecByWheeling(p, ogl, zDelta ) )
  {
    LogCore::GetInst()->Add("WHEELCRSSEC"); 
  }
  else
  {
    LogCore::GetInst()->Add("WHEELZOOM"); 
    ogl->ZoomCamera(zDelta * 0.1f);
  }
  RedrawScene();
}



void ModeRefStrokeTrim::KeyDown(int nChar) 
{
  LogCore::GetInst()->Add("KeyDown");

  if ( nChar != 'Z' && nChar != 'Y' ) return;

  byte* flg3d    = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();
  int num_voxels = ImageCore::GetInst()->GetNumVoxels();


  if (nChar == 'Z' && m_undo_idx > 0)
	{
    std::cout<< "undo!!\n";
    LogCore::GetInst()->Add("UNDO");

#pragma omp parallel for 
    for ( int i=0; i < num_voxels; ++i )
      if ( m_undo_vol[i] == m_undo_idx ) flg3d[i] = 255;

    m_undo_idx--;
  }
  else if (nChar == 'Y' && m_undo_idx < m_redo_max)
  {
    std::cout<< "Redo!!\n";
    LogCore::GetInst()->Add("REDO");

    m_undo_idx++;    
#pragma omp parallel for 
    for ( int i=0; i < num_voxels; ++i )
      if ( m_undo_vol[i] == m_undo_idx ) flg3d[i] = 1;

  }
 
	ImageCore::GetInst()->m_vol_flag.SetUpdated();
  RedrawScene();
}


void ModeRefStrokeTrim::KeyUp(int nChar) {}


void ModeRefStrokeTrim::DrawScene(const EVec3f &cuboid, const EVec3f &cam_pos, const EVec3f &cam_center)
{
  //renderingに必用なパラメータを集めておく

  if (m_b_drawingstroke && m_stroke3d.size() > 1)
	{
    DrawPolyLine( EVec3f(1,0,1), 4, m_stroke3d, true );
	}
	
	//bind volumes ---------------------------------------
  BindAllVolumes();
		
 //render cross sections ----------------------------------
  const EVec3i reso  = ImageCore::GetInst()->GetResolution();
  DrawCrossSections(cuboid, reso, m_crssec_shader);

	//volume rendering ---------------------------------------
	if ( formVisParam_bRendVol() && !IsSpaceKeyOn())
	{
    const float alpha    = formVisParam_getAlpha();
    const bool b_otherroi= formVisParam_GetOtherROI();
    const bool b_manip   = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int  slice_num = (int)( (b_manip ? ONMOVE_SLICE_RATE : 1.0 ) 
                           * formVisParam_getSliceNum() );

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		m_volume_shader.Bind(0, 1, 2, 3, 4, 5, 6, alpha * 0.1f, reso, cam_pos, false, b_otherroi);
		t_DrawCuboidSlices( slice_num, cam_pos, cam_center, cuboid);
		m_volume_shader.Unbind();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
}






static bool IsInClosedStroke(double x, double y, const std::vector<EVec2i> &stroke)
{
	//stroke.size() > 3 
	EVec2d d1,d2;
	double sum = 0;

	d1 << stroke.back ()[0] - x, stroke.back() [1] - y;
	d2 << stroke.front()[0] - x, stroke.front()[1] - y;
	sum = CalcAngle(d1,d2);
	
	for (int i = 1; i < (int)stroke.size(); ++i)
	{
		d1 << stroke[i-1][0] - x, stroke[i-1][1] - y;
		d2 << stroke[ i ][0] - x, stroke[ i ][1] - y;
		sum += CalcAngle(d1,d2);
	}

	return fabs(sum) > M_PI * 1.5;
}



void ModeRefStrokeTrim::UpdateVolFlgByStroke( OglForCLI *ogl)
{
	if( m_stroke2d.size() < 3 ) return;

  int W,H,D,WH,WHD;
  std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();

	byte *flg3d = ImageCore::GetInst()->m_vol_flag.GetVolumePtr() ;
  
  //prepare undo 
  m_undo_idx += 1;
  m_redo_max  = m_undo_idx;

#pragma omp parallel for
  for ( int i = 0; i < WHD; ++i)
  {
    if ( m_undo_vol[i] >= m_undo_idx)
      m_undo_vol[i] = 0;
  }

  //get projection information
	if( !ogl->IsDrawing() ) ogl->oglMakeCurrent();

	double model_mat[16], projection_mat[16];
	int viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX , model_mat      );
	glGetDoublev(GL_PROJECTION_MATRIX, projection_mat );
	glGetIntegerv(GL_VIEWPORT        , viewport       );
  

  //gen screen_img (binary inside/outside image) 
  std::cout << "get 2D foreground image\n";

	byte *screen_img = new byte[ viewport[2] * viewport[3] ];
	memset( screen_img, 0, sizeof(byte) * viewport[2] * viewport[3] );

	for( auto &p : m_stroke2d) p[1] = viewport[3] - p[1];

	EVec2i bb_min, bb_max;
	CalcBoundingBox(m_stroke2d, bb_min, bb_max);
	bb_min[0] = std::max( 0           , bb_min[0] - 1);
	bb_min[1] = std::max( 0           , bb_min[1] - 1);
	bb_max[0] = std::min(viewport[2]-1, bb_max[0] + 1);
	bb_max[1] = std::min(viewport[3]-1, bb_max[1] + 1);

#pragma omp parallel for
	for ( int y = bb_min[1]; y < bb_max[1]; ++y)
	{
		for ( int x = bb_min[0]; x < bb_max[0]; ++x)
		{
			if ( IsInClosedStroke( x, y, m_stroke2d ) ) 
        screen_img[x + y * viewport[2] ] = 255;
		}
	}


  //compute projection of all foreground voxels
  std::cout << "compute projection of all voxels\n";
	const EVec3f pitch = ImageCore::GetInst()->GetPitch();

#pragma omp parallel for
	for ( int z = 0; z < D; ++z)
	{
		for( int y = 0 ; y < H; ++y)
		{
			for ( int x = 0; x < W; ++x)
			{
				const int I = x + y * W + z * WH;
				if( flg3d[I] != 255 ) continue;

        double posx = (x + 0.5) * pitch[0];
        double posy = (y + 0.5) * pitch[1];
        double posz = (z + 0.5) * pitch[2];
				double px, py, pz;
				gluProject(	posx,  posy, posz,  model_mat, projection_mat, viewport, &px, &py, &pz);

				if( 0<= px && px <= viewport[2]-1 && 
					  0<= py && py <= viewport[3]-1 && 
            screen_img[ (int)px + (int)py*viewport[2] ] == 255 ) 
        {
          flg3d[I] = 1;
          m_undo_vol[I] = m_undo_idx;
        }
			}	
		}
	}

	delete[] screen_img;

	if( !ogl->IsDrawing() ) wglMakeCurrent(NULL,NULL);

	ImageCore::GetInst()->m_vol_flag.SetUpdated();
  m_b_modified = true;
	
  std::cout << "...done\n";
}




