#pragma unmanaged
#include "ModeSegBolus.h"
#include "ModeCommonTools.h"
#include "CrsSecCore.h"
#include "ImageCore.h"
#include "ModeCore.h"

#pragma managed
#include "FormSegBolus.h"
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSelectMskId.h"
#include "CliMessageBox.h"
#pragma unmanaged

#include <chrono>
#include <fstream>
#include "tqueue.h"

//openCV
#include "opencv/cv.h"
#include "opencv2/surface_matching.hpp"
#include "opencv2/surface_matching/ppf_helpers.hpp"
#include "opencv2/core/utility.hpp"

#pragma comment(lib,"opencv_core310.lib")
#pragma comment(lib,"opencv_surface_matching310.lib")

using namespace RoiPainter4D;
#pragma warning(disable : 4996)

//flg4dの使い方
// 0 : never change
// 1 : background
//255: foreground
ModeSegBolus::ModeSegBolus() :
	m_volume_shader_norm("shader/volVtx.glsl", "shader/volFlg.glsl"     ),
	m_volume_shader_segm("shader/volVtx.glsl", "shader/volFlg_Seg.glsl" ),
	m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
  m_vol_cyl = 0;
}



//////start and finish seg///////////
void ModeSegBolus::startMode()
{
  //mouse manipuration
	m_bL = m_bR = m_bM = false;
  m_b_draw_cutstroke = false;
  
  m_b_modified  = false;
	m_drag_cpid   = -1;
	m_active_cid  = -1;

  //cylinder and control points
  m_cylinders.clear();
  m_cp_radius = 5 * ImageCore::GetInst()->GetPitch().x();

  m_thresh_max = m_thresh_min = 0;

  //以下，用途不明field
  m_b_updated_cylinder = false;
	m_sub_thread = 0;


	const EVec3f pitch      = ImageCore::GetInst()->GetPitch();
	const EVec3i resolution = ImageCore::GetInst()->GetReso ();
	const int    num_voxels  = resolution[0] * resolution[1] * resolution[2];
  
  //init flg
  ImageCore::GetInst()->InitializeFlg4dByMask( formMain_SetProgressValue );

  float rad_max   = float( max3(resolution[0], resolution[1], resolution[2]) * max3(pitch[0], pitch[1], pitch[2]) );

  EVec2i minmax = ImageCore::GetInst()->GetVolumeMinMax();
	formSegBolus_InitAllItems( minmax[0], minmax[1], 0.0001f, rad_max);
	formSegBolus_Show();
	formMain_RedrawMainPanel();
  UpdateFormItemsForCylinder();

  UpdateImageCoreVisVolumes();

  if( m_vol_cyl != 0 ) delete[] m_vol_cyl;
  m_vol_cyl = new byte[num_voxels];
  memset(m_vol_cyl, 0, sizeof(byte)*num_voxels );
}


bool ModeSegBolus::canEndMode()
{
	if (!m_b_modified) return true;

	if (ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Leaving?"))
	{
		WaitForSingleObject(m_sub_thread, INFINITE);
		return true;
	}
	return false;
}


void ModeSegBolus::cancelSegmentation()
{
	if( !ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Leaving?") ) return;
	
	WaitForSingleObject(m_sub_thread, INFINITE);

	m_b_modified = false;
		
  m_cylinders.clear();
  if( m_vol_cyl != 0 ) delete[] m_vol_cyl;
  m_vol_cyl = 0;

	ModeCore::getInst()->ModeSwitch(MODE_VIS_NORMAL);
	formMain_RedrawMainPanel();
}


void ModeSegBolus::FinishSegmentation()
{
	WaitForSingleObject(m_sub_thread, INFINITE);

	std::vector<byte*> &flg4d = ImageCore::GetInst()->m_flg4d ;

	const EVec3i reso = ImageCore::GetInst()->GetReso();
	const int    num_frames = (int)flg4d.size();
	const int    num_voxels = reso[0] * reso[1] * reso[2];

	bool b_fore_exist = false;
	for (int fi = 0; fi < num_frames && !b_fore_exist; ++fi)
  {
		for (int i = 0; i < num_voxels && !b_fore_exist; ++i)
    {
			if (flg4d[fi][i] == 255) b_fore_exist = true;
    }
  }

	if (!b_fore_exist)
	{
		ShowMsgDlg_OK(MESSAGE_NO_FOREGROUND, "no foreground");
		return;
	}

	//シリンダー情報をファイルに記録
  ExportCylinderInfoByText("");

	ImageCore::GetInst()->mask_storeCurrentForeGround( );

  if( m_vol_cyl != 0 ) delete[] m_vol_cyl;
  m_vol_cyl = 0;
	m_b_modified = false;
	m_drag_cpid   = -1;
	m_active_cid = -1;
	m_cylinders.clear();
	
  m_b_updated_cylinder = false;

	ModeCore::getInst()->ModeSwitch(MODE_VIS_MASK);
	formMain_RedrawMainPanel();
}
////////////////////////////////////////////////////////////////////


void ModeSegBolus::keyDown(int nChar)
{
  formMain_RedrawMainPanel();
}


//////////Mouse Listener//////////
void ModeSegBolus::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if (m_b_draw_cutstroke)
  {
    EVec3f cuboid = ImageCore::GetCuboid();
    CrssecCore::CreateCurvedCrssec( cuboid, ogl->GetCamPos(), m_stroke);
  }
  m_bL = false;
  m_drag_cpid = -1;
  m_b_drag_cylinder = false;
  m_b_draw_cutstroke = false;
	ogl->BtnUp();
	formMain_RedrawMainPanel();
}


void ModeSegBolus::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
	m_bR = false;
  m_drag_cpid = -1;
  m_b_drag_cylinder = false;
	ogl->BtnUp();
	formMain_RedrawMainPanel();
}

void ModeSegBolus::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
	m_bM = false;
  m_drag_cpid = -1;
  m_b_drag_cylinder = false;
	ogl->BtnUp();
	formMain_RedrawMainPanel();
}


void ModeSegBolus::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
	m_bL = true;

  if ( isCtrKeyOn() )
  {
    m_stroke.clear();
    m_b_draw_cutstroke = true;
  }
  else if ( isShiftKeyOn() )
	{
    //drag cpid or drag seed
	  EVec3f ray_pos, ray_dir, pos;
	  ogl->GetCursorRay(p, ray_pos, ray_dir);
	  CRSSEC_ID id = PickCrssec(ray_pos, ray_dir, pos);

	  float crssec_depth = (id == CRSSEC_NON) ? FLT_MAX : (pos - ray_pos).norm();

    //pick cylindercp and regiongrow_cp
    if ( 0 <= m_active_cid && m_active_cid < m_cylinders.size() )
    {
      m_drag_cpid = m_cylinders[m_active_cid].PickCPs( ray_pos, ray_dir, m_cp_radius );
    }
	}
	else
  {
		ogl->BtnDown_Trans(p);
  }
	
}


void ModeSegBolus::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
	m_bR = true;
  if(isCtrKeyOn() && 0 <= m_active_cid && m_active_cid < m_cylinders.size() )
  {
    m_b_drag_cylinder = true;
    m_pre_pos = p;
  }else{
	  ogl->BtnDown_Rot(p);
  }
}


void ModeSegBolus::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
	m_bM = true;

  if( isShiftKeyOn() )
  {
    EVec3f ray_pos, ray_dir, pos;
    ogl->GetCursorRay(p, ray_pos, ray_dir);
    EVec2i cyl_cp_id = PickCPs( ray_pos, ray_dir, FLT_MAX );
    m_active_cid = cyl_cp_id[0];
	  UpdateFormItemsForCylinder();
  }
  else 
  {
	  ogl->BtnDown_Zoom(p);
  }
}


void ModeSegBolus::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{
  if( !isShiftKeyOn() ) return;
  // cylinder mode --> add cylinder cp 
  // reg grow mode --> add region grow seed

  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);
  CRSSEC_ID crssecid = PickCrssec(ray_pos, ray_dir, pos)  ;

    if( crssecid == CRSSEC_NON) return;

  if ( 0 <= m_active_cid && m_active_cid < m_cylinders.size() )
  {
    if( m_cylinders[ m_active_cid ].PickCPs(ray_pos, ray_dir, m_cp_radius) >= 0 ) return;
    m_cylinders[ m_active_cid ].AddControlPoint(pos);
    m_b_updated_cylinder = true;
  }
  else
  {
    m_cylinders.push_back( GeneralizedCylinder() );      
    m_cylinders.back().AddControlPoint( pos );
    m_cylinders.back().SetRadius1(8 * ImageCore::GetInst()->GetPitch().x());
    m_cylinders.back().SetRadius2(8 * ImageCore::GetInst()->GetPitch().x());
    m_active_cid = (int)m_cylinders.size() - 1;
    UpdateFormItemsForCylinder();
    m_b_updated_cylinder = true;
  }

  formMain_RedrawMainPanel();
}



void ModeSegBolus::RBtnDclk(const EVec2i &p, OglForCLI *ogl)
{
  if( !isShiftKeyOn() ) return;

  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if ( 0 <= m_active_cid && m_active_cid < m_cylinders.size() ) 
  {
    //pick cp to remove 
    int id = m_cylinders[m_active_cid].PickCPs(ray_pos, ray_dir, m_cp_radius );

    if( id >= 0)
    {
      m_cylinders[m_active_cid].RemoveControlPoint( id );
      if( m_cylinders[m_active_cid].GetControlPointNum() == 0 )
      {
        m_cylinders.erase(m_cylinders.begin() + m_active_cid );
        m_active_cid = -1;
        UpdateFormItemsForCylinder();
      }
      m_b_updated_cylinder = true;
    }
  }

}


void ModeSegBolus::MBtnDclk(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegBolus::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
	EVec3f ray_pos, ray_dir, pos;
	ogl->GetCursorRay(p, ray_pos, ray_dir);
  CRSSEC_ID pick_crssec_id = PickCrssec(ray_pos, ray_dir, pos);
	
  if (!m_bL && !m_bR && !m_bM) return;

	
  if ( m_b_draw_cutstroke )
  {
    //cut stroke 
    m_stroke.push_back(ray_pos + 0.1f * ray_dir);
  }
  else if ( m_drag_cpid >= 0 && 0 <= m_active_cid && m_active_cid < m_cylinders.size() )
	{ 
    //1 Dragging CP
		if (pick_crssec_id != CRSSEC_NON)
		{
			m_cylinders[m_active_cid].MoveControlPoint( m_drag_cpid, pos);
			m_b_updated_cylinder = true;
		}
	}
  else if ( m_b_drag_cylinder )
  { 
    //1 Dragging whole cylinder
		EVec3f pre_pos, pre_dir;
    ogl->GetCursorRay( m_pre_pos, pre_pos, pre_dir );
    EVec3f transVec = (ray_dir - pre_dir).normalized();
		if ( isnan(transVec[0]) ) return ;
    m_cylinders[m_active_cid].Translate(transVec / 2);

    m_pre_pos = p;
  }
	else
  {
		ogl->MouseMove(p);
  }
	formMain_RedrawMainPanel();
}


void ModeSegBolus::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  if(!WheelingCrssec(p, z_delta, ogl) ) 
    ogl->ZoomCamera(z_delta * 0.1f);

	formMain_RedrawMainPanel();
}


//export cylinder information
void ModeSegBolus::ExportCylinderInfoByText(std::string file_path )
{
  if( file_path.length() == 0)
  {  
    //file_pathが空なら自動でファイル名を生成する

    char exe_path[MAX_PATH + 1];
    if ( GetModuleFileName(nullptr, exe_path, MAX_PATH) == 0 )
      return;

    char drive[MAX_PATH + 1], dir[MAX_PATH + 1], fname[MAX_PATH + 1], ext[MAX_PATH + 1];
    _splitpath(exe_path, drive, dir, fname, ext);

		//現在日時を取得し形式を変換 (file名に利用)
		time_t time_val = time(nullptr);
		const tm* local_time = localtime(&time_val);
		std::stringstream s;
		s << "20";
		s << local_time->tm_year - 100; 
		s << local_time->tm_mon + 1; 
		s << local_time->tm_mday; //そのまま
		s << local_time->tm_hour << local_time->tm_min << local_time->tm_sec;

    file_path = std::string(drive) + std::string(dir) + std::string("___RecordCylinder___\\");
    file_path += std::string( s.str() ) + "_cylinderInfo.txt";
    std::cout << file_path << "\n";
  }

	if ( file_path.size() < 4 ) file_path += ".txt";
  if ( file_path.find( ".txt", file_path.size() - 4 ) == std::string::npos ) file_path += ".txt";

  std::ofstream fp(file_path);

	if ( !fp.is_open() )
	{
    std::cout <<  "error when opening "<< file_path << "\n";
		return ;
	}

  fp << "roipainter4d_localregiongrow_cylinders\n";
  fp << "threshold_max   "  << m_thresh_max       << "\n";
  fp << "threshold_min   "  << m_thresh_min       << "\n";
	fp << "num_of_cylinder "  << m_cylinders.size() << "\n";

	for (int ci = 0; ci < (int)m_cylinders.size(); ++ci)
	{
		fp << "cylinder_idx: " << ci << "\n";
		fp << "radius1:      " << m_cylinders[ci].GetRadius1()    << "\n";
		fp << "radius2:      " << m_cylinders[ci].GetRadius2()    << "\n";
    fp << "twistangle:   " << m_cylinders[ci].GetTwistAngle() << "\n";

	  fp << "num_of_cp:    " << m_cylinders[ci].GetControlPointNum() << "\n";
		for (const auto& cp : m_cylinders[ci].GetControlPoints() )
		{
			fp << cp.x() << " " << cp.y() << " " << cp.z() << "\n";
		}
	}
	fp.close();
}



static std::vector<std::string> SplitSpace(const std::string& source, const char splitchar = ' ')
{
  std::vector<std::string> strs;

  std::string s = "";
  for (int charI = 0; charI < (int)source.size(); ++charI)
  {
    if (source[charI] == splitchar)
    {
      if (s != "") strs.push_back(s);
      s = "";
    }
    else
      s += source[charI];
  }

  if (s != "") strs.push_back(s);

  return strs;
}



void ModeSegBolus::LoadCylinderInfoFromFile(std::string filePath, bool to_perform_fitting)
{
  std::ifstream ifs(filePath);

  if (ifs.fail())
  {
    ShowMsgDlg_OK("cylinder load err", "err");
    return ;
  }
  m_cylinders.clear();
  m_active_cid = -1;

  std::string str;

  //1行目[roipainter4d_localregiongrow_cylinders]読み飛ばす
  ifs >> str;
  std::cout << str << "\n";

  ifs >> str >> m_thresh_max;  std::cout << str << m_thresh_max <<"\n";
  ifs >> str >> m_thresh_min;  std::cout << str << m_thresh_min <<"\n";

  int num_cyl;
  ifs >> str >> num_cyl;

  for( int ci = 0; ci < num_cyl; ++ci)
  {
    int cylidx;
    ifs >> str >> cylidx;
    float r1, r2, tw;
    ifs >> str >> r1;
    ifs >> str >> r2;
    ifs >> str >> tw;
    std::cout << r1 << " " << r2 << " " << tw << "\n";

    int ncp;
    ifs >> str >> ncp;
    std::cout << ncp;

    std::vector<EVec3f> cps(ncp);
    for( int i = 0; i < ncp; ++i) ifs >> cps[i][0] >> cps[i][1] >> cps[i][2]; 

    m_cylinders.push_back( GeneralizedCylinder(r1,r2,tw,cps));
  }

  UpdateFormItemsForCylinder();
	formMain_RedrawMainPanel();
  m_b_updated_cylinder = true;


  //TODO to_perform_fitting 骨領域を利用してfittingをする 
}




//////////////////////////////////////////////////////////////////////////////
//////////////////////////////draw scene//////////////////////////////////////

void ModeSegBolus::drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF) 
{
	const EVec3i reso = ImageCore::GetInst()->GetReso();

  BindAllVolumes();
  DrawCrossSections(cuboid, reso, !isSpaceKeyOn(), m_crssec_shader);
  
  //cut stroke
  if ( m_b_draw_cutstroke )
  {
    t_DrawPolyLine( EVec3f(1,1,0), 3, m_stroke, false);
  }
  
  //draw volume 
	if ( formVisParam_bRendVol() )
	{
    const bool  b_manip   = formVisParam_bOnManip() || m_bL || m_bR || m_bM;

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
    GlslShaderVolume &s = formSegBolus_IsNormalVolRend() ? m_volume_shader_norm:
                                                           m_volume_shader_segm ;
    DrawVolumeSlices(cuboid, reso, camP, camF, !isSpaceKeyOn(), b_manip, s); 
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

  //draw OBB of active cylinder 
	glEnable( GL_BLEND );
  bool b_drawobb = formSegBolus_IsDrawOBB();
	if ( m_active_cid >= 0 && m_active_cid < (int)m_cylinders.size() && b_drawobb )
		m_cylinders[m_active_cid].DrawOBBs();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT7);
  float lightwhite[4] = { 1, 1, 1, 1 };
	float lightpos[4]   = { camP[0], camP[1], camP[2], 1.0f };
	glLightfv(GL_LIGHT7, GL_DIFFUSE , lightwhite);
	glLightfv(GL_LIGHT7, GL_SPECULAR, lightwhite);
	glLightfv(GL_LIGHT7, GL_AMBIENT , lightwhite);
	glLightfv(GL_LIGHT7, GL_POSITION, lightpos );

  //draw Seed
  glEnable(GL_LIGHTING);

  //draw cylinders
  for (int i = 0; i < (int)m_cylinders.size(); ++i)
	{
		const GeneralizedCylinder& cylinder = m_cylinders[i];
		const std::vector<EVec3f>& cps = cylinder.GetControlPoints();

    //draw axis
    glDisable(GL_LIGHTING);
		if ( i == m_active_cid )
			cylinder.DrawAxisCurve( EVec3f(0, 1, 0), 2 * m_cp_radius);
		else
			cylinder.DrawAxisCurve( EVec3f(1, 0, 1), 2 * m_cp_radius);

    glEnable(GL_LIGHTING);
    //draw cps
		for (int i = 0; i < (int)cps.size(); ++i)
		{
			if( i == cps.size() -1 ) 
        TMesh::DrawSphere(cps[i], m_cp_radius, COLOR_M, COLOR_MH, COLOR_W, COLOR_SHIN64);
      else 
        TMesh::DrawSphere(cps[i], m_cp_radius, COLOR_R, COLOR_RH, COLOR_W, COLOR_SHIN64);
		}
	}

  float trans = formSegBolus_GetCylinderTransparency();
	glEnable( GL_CULL_FACE );
	glCullFace(GL_BACK);
	for (auto& cylinder : m_cylinders)
	{ 
		cylinder.DrawCylinder(trans);
		cylinder.DrawRadiusDir( EVec4f(1, 1, 0, 1), EVec4f(0, 1, 1, 1) );
	}

	glDisable( GL_BLEND );
	glDisable( GL_LIGHTING);
	glDisable( GL_LIGHT7);
}



void ModeSegBolus::UpdateFormItemsForCylinder()
{
	if ( m_active_cid < 0 || (int)m_cylinders.size() <= m_active_cid )
  {
	  formSegBolus_SetItemsForCylinderPropaty( false, 0,0,0);
    return;
  } 
	float r1    = m_cylinders[m_active_cid].GetRadius1();
	float r2    = m_cylinders[m_active_cid].GetRadius2();
	float tw    = m_cylinders[m_active_cid].GetTwistAngle();
	formSegBolus_SetItemsForCylinderPropaty( true, r1, r2, tw);
}


//return pair(cyl_idx, cp_idx)
EVec2i ModeSegBolus::PickCPs(const EVec3f &ray_pos, const EVec3f &ray_dir, const float max_depth)
{
	for (int cyl_idx = 0; cyl_idx < (int)m_cylinders.size(); ++cyl_idx)
	{ 
    int cp_idx = m_cylinders[cyl_idx].PickCPs(ray_pos, ray_dir, m_cp_radius + 0.1f, max_depth);
    if( cp_idx != -1 ) return EVec2i( cyl_idx, cp_idx );
	}
	return EVec2i(-1,-1);
}


void ModeSegBolus::SetRadius1(float length)
{
	if (m_active_cid >= 0 && m_active_cid < (int)m_cylinders.size())
	{
		m_cylinders[m_active_cid].SetRadius1( length );
		formMain_RedrawMainPanel();
		m_b_updated_cylinder = true;
	}
}


void ModeSegBolus::SetRadius2(float length)
{
	if (m_active_cid >= 0 && m_active_cid < (int)m_cylinders.size())
	{
		m_cylinders[m_active_cid].SetRadius2(length);
		formMain_RedrawMainPanel();
		m_b_updated_cylinder = true;
	}
}

void  ModeSegBolus::SetTwistAngle(float theta)
{ 
	if (m_active_cid >= 0 && m_active_cid < (int)m_cylinders.size())
	{ 
		m_cylinders[m_active_cid].SetTwistAngle( theta );
		formMain_RedrawMainPanel();
		m_b_updated_cylinder = true;
	}
}


void ModeSegBolus::FillInCylinder()
{
	const EVec3f pitch = ImageCore::GetInst()->GetPitch();
	const EVec3i reso  = ImageCore::GetInst()->GetReso();
  const int num_voxels = reso[0] * reso[1] * reso[2];
  memset( m_vol_cyl, 0, sizeof(byte) * num_voxels);
  
  for( int i=0; i < m_cylinders.size(); ++i) m_cylinders[i].FillVoxelInCylinder( reso, pitch, formMain_SetProgressValue, m_vol_cyl);
	m_b_updated_cylinder   = false;
}



void ModeSegBolus::AddNewCylinder()
{
  const EVec3f cam_pos    = formMain_GetOgl()->GetCamPos();
  const EVec3f cam_center = formMain_GetOgl()->GetCamCnt();
  const EVec3f cam_up     = formMain_GetOgl()->GetCamUp();

  EVec3f cp1 = EVec3f( 0,  0,  0);
  EVec3f cp2 = EVec3f(10, 10, 10);

  //塩野崎君の実装（要修正）
	//mainformにマウスが乗る前に呼ばれるとnullpointer例外が出る
	try
	{
		EVec3f ray_dir = cam_center - cam_pos;
		ray_dir.normalize();

		EVec3f pos;
		if (PickCrssec( cam_center, ray_dir, pos) != CRSSEC_NON)
		{
			EVec3f crsSecToCamPos = cam_pos - pos;
			cp1 = pos + crsSecToCamPos /15;
			cp2 = pos + 15 * cam_up;
		}
		else
		{
			cp1 = cam_center + 15 * ray_dir;
			cp2 = cp1 + 15 * cam_up;
		}
	}
	catch (...)
	{
		cp1 = EVec3f(0, 0, 0);
		cp2 = EVec3f(10, 10, 10);
	}


  float r = 8 * ImageCore::GetInst()->GetPitch().x();
  std::vector<EVec3f> cps = {cp1, cp2};
	m_cylinders.push_back(GeneralizedCylinder( r, r, 0, cps ));
	m_active_cid = (int)m_cylinders.size() - 1;

	UpdateFormItemsForCylinder();
	formMain_RedrawMainPanel();
	m_b_updated_cylinder = true;
}




void ModeSegBolus::RunThresholdingInClyinderOneFrame( const short minv, const short maxv)
{
	m_thresh_min = minv;
	m_thresh_max = maxv;

  if( m_b_updated_cylinder ) FillInCylinder();
	m_b_updated_cylinder   = false;

  const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const int frame_idx  = formVisParam_getframeI();
  short*    img3d = ImageCore::GetInst()->m_img4d[frame_idx];
  byte*     flg3d = ImageCore::GetInst()->m_flg4d[frame_idx];

  std::cout << "RunThresholdingInClyinderOneFrame...\n";
#pragma omp parallel for
  for (int i = 0; i < num_voxels; ++i) if (flg3d[i] != 0)
  {
    flg3d[i] = (m_vol_cyl[i] && minv <= img3d[i] && img3d[i] <= maxv) ? 255 : 1;
  }
  std::cout << "RunThresholdingInClyinderOneFrame...DONE\n";

  m_b_modified = true;
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}


void ModeSegBolus::RunThresholdingInCylinderAllFrame(const short minv, const short maxv)
{
	m_thresh_min = minv;
	m_thresh_max = maxv;

  if( m_b_updated_cylinder ) FillInCylinder();
	m_b_updated_cylinder   = false;

  std::vector<short*> &img4d = ImageCore::GetInst()->m_img4d;
  std::vector<byte*>  &flg4d = ImageCore::GetInst()->m_flg4d;
  const int  num_frames = ImageCore::GetInst()->GetNumFrames();
  const int  num_voxels = ImageCore::GetInst()->GetNumVoxels();

  std::cout << "RunThresholdingInCylinderAllFrame...\n";
#pragma omp parallel for
  for (int f = 0; f < num_frames; ++f)
  {
    short *img3d = img4d[f];
    byte  *flg3d = flg4d[f];
    for (int i = 0; i < num_voxels; ++i) if (flg3d[i] != 0)
    {
      flg3d[i] = (m_vol_cyl[i] && minv <= img3d[i] && img3d[i] <= maxv) ? 255 : 1;
    }
  }
  std::cout << "RunThresholdingInCylinderAllFrame...DONE\n";

  m_b_modified = true;
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}



void ModeSegBolus::RunErosionOneFrame()
{
  const int   frame_idx = formVisParam_getframeI();
  const EVec3i   reso   = ImageCore::GetInst()->GetReso();
  std::vector<byte*> &flg4d  = ImageCore::GetInst()->m_flg4d;

  std::cout << "RunErosionOneFrame...\n";
  t_Erode3D(reso[0], reso[1], reso[2], flg4d[frame_idx]);
  std::cout << "RunErosionOneFrame...DONE\n";

	m_b_modified = true;
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}



void ModeSegBolus::RunErosionAllFrame()
{
  const EVec3i   reso   = ImageCore::GetInst()->GetReso();
  std::vector<byte*> &flg4d  = ImageCore::GetInst()->m_flg4d;
  const int     num_frame = (int)flg4d.size();

  std::cout << "RunErosionAllFrame...\n";
#pragma omp parallel for 
  for (int i = 0; i < num_frame; ++i)
    t_Erode3D(reso[0], reso[1], reso[2], flg4d[i]);
  std::cout << "RunErosionAllFrame...DONE\n";

	m_b_modified = true;
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}



void ModeSegBolus::RunDilationOneFrame() 
{
  if( m_b_updated_cylinder ) FillInCylinder();
	m_b_updated_cylinder   = false;

  const EVec3i   reso    = ImageCore::GetInst()->GetReso();
  const int   frame_idx  = formVisParam_getframeI();
  const int   num_voxels = reso[0] * reso[1] * reso[2];
  byte* flg3d  = ImageCore::GetInst()->m_flg4d[frame_idx];

  std::cout << "RunDilationOneFrame...\n";
  t_Dilate3D(reso[0], reso[1], reso[2], flg3d );
  
  //はみ出しチェック
  for(int i=0; i < num_voxels; ++i) {
    if( flg3d[i] == 255 && m_vol_cyl[i] == 0) {
      flg3d[i] = 1;
    }
  }
  std::cout << "RunDilationOneFrame...DONE\n";

	m_b_modified = true;
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}



void ModeSegBolus::RunDilationAllFrame()
{
  if( m_b_updated_cylinder ) FillInCylinder();
  m_b_updated_cylinder   = false;

  const EVec3i   reso    = ImageCore::GetInst()->GetReso();
  std::vector<byte*> &flg4d   = ImageCore::GetInst()->m_flg4d;
  const int   num_frames = (int)flg4d.size();
  const int   num_voxels = reso[0]* reso[1]* reso[2];

  std::cout << "RunDilationAllFrame...\n";

#pragma omp parallel for 
  for (int fI = 0; fI < num_frames; ++fI)
  {
    byte* flg3d = flg4d[fI];
    t_Dilate3D(reso[0], reso[1], reso[2], flg3d );

    //はみ出しチェック
    for(int i=0; i < num_voxels; ++i) {
      if( flg3d[i] == 255 && m_vol_cyl[i] == 0) {
        flg3d[i] = 1;
      }
    }
  }
  std::cout << "RunDilationAllFrame...DONE\n";

	m_b_modified = true;
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}



///////////////////////////////////////////////////////////////////////////////
//Region Growing In Cylinder///////////////////////////////////////////////////

//region growing の seed 初期化
// generalizedCylinderの軸上の画素 --> Qへ追加
// RegionGrowSeed上の画素          --> Qへ追加
// ただし，以下の画素iはseedとして採用しない
//  - flg3d[i] == 0
//  - vol_cyl[i] == 0
//  - img3d[i] < minv or maxv < img3d[i]

static void InitializeRegionGrowQueue3D
(
  const int    frame_idx,
  const short *img3d  ,
  const byte  *flg3d  ,  //0:neverchange, 1or255:target
  const byte  *vol_cyl,  //0:outside    , 255: inside of cylinder 
  const std::vector<GeneralizedCylinder> &cylinders,
  const short minv, 
  const short maxv,
  TQueue<PixIdx4D>& Q
)
{	
  ImageCore *ic = ImageCore::GetInst();

	for (int ci = 0; ci < (int)cylinders.size(); ++ci)
	{
    std::vector<EVec3f> axiscurve = cylinders[ci].GetAxisCurve();

		for (const auto& seed : axiscurve)
		{
			const int idx = ic->GetVoxelIndex(seed);

			if ( flg3d[idx] != 0 && vol_cyl[idx] != 0 && minv <= img3d[idx] && img3d[idx] <= maxv )
			{
				Q.push_back( PixIdx4D(idx, frame_idx));
			}
		}
	}

}


void ComputeRegionGrowing3D
(
    const EVec3i reso,
    const short *img3d  ,
    const byte  *vol_cyl,  //0:outside    , 255: inside of cylinder 
    const short minv    , 
    const short maxv    ,
    const TQueue<PixIdx4D>& seedvoxels,

    byte  *flg3d    //0:neverchange, 1or255:target
)
{
	const int W = reso[0], H = reso[1], D = reso[2];
  const int WH = W * H, WHD = W * H * D;

	//volFlg : 0:never change, 1:back, 255:fore
	for (int i = 0; i < WHD; ++i)
		flg3d[i] = (flg3d[i] == 0) ? 0 : 1;

  TQueue<PixIdx4D> Q = seedvoxels;
  for (int i = 0; i < Q.size(); ++i )
    flg3d[ Q[i].idx ] = 255;

	while (!Q.empty())
	{
		const int idx = Q.front().idx;
		const int  x  =  idx % W;
		const int  y  = (idx / W) % H;
		const int  z  =  idx / WH;
    const int  f  = Q.front().frameI;
		Q.pop_front();

		//grow to 6 neighbors
		int K;

		K = idx - 1; if (x > 0 && vol_cyl[K] && flg3d[K]==1 && minv<=img3d[K] && img3d[K]<=maxv) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
		K = idx - W; if (y > 0 && vol_cyl[K] && flg3d[K]==1 && minv<=img3d[K] && img3d[K]<=maxv) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
		K = idx -WH; if (z > 0 && vol_cyl[K] && flg3d[K]==1 && minv<=img3d[K] && img3d[K]<=maxv) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
		K = idx + 1; if (x <W-1&& vol_cyl[K] && flg3d[K]==1 && minv<=img3d[K] && img3d[K]<=maxv) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
		K = idx + W; if (y <H-1&& vol_cyl[K] && flg3d[K]==1 && minv<=img3d[K] && img3d[K]<=maxv) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
		K = idx +WH; if (z <D-1&& vol_cyl[K] && flg3d[K]==1 && minv<=img3d[K] && img3d[K]<=maxv) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
	}
}






void RemoveForeVoxelCloseToOtherMask
(
  const EVec3i reso    ,
  const int    distance,
  byte* flg3d
)
{
  if( distance == 0 ) return;

  const int W = reso[0], H = reso[1], D = reso[2];
  const int WH = W*H;

#pragma omp parallel for
	for (int z = 0; z < D; ++z){
	  for (int y = 0; y < H; ++y){
	    for (int x = 0; x < W; ++x){

		    const int idx = x + y * W + z * W * H;
        if( flg3d[idx] != 255 ) continue;
        int xs = t_crop(0,W-1,x-distance);
        int ys = t_crop(0,H-1,y-distance);
        int zs = t_crop(0,D-1,z-distance);
        int xe = t_crop(0,W-1,x+distance);
        int ye = t_crop(0,H-1,y+distance);
        int ze = t_crop(0,D-1,z+distance);

        bool flg = false;
        for (int dz = zs; dz <= ze && !flg; ++dz){
          for (int dy = ys; dy <= ye && !flg; ++dy){
            for (int dx = xs; dx <= xe && !flg; ++dx)
            {
              if( flg3d[dx + dy*W + dz*WH] == 0) flg = true;
            }
          }
        }

			  if (flg) flg3d[idx] = 1;
	    }
    }
  }
}



void ModeSegBolus::RunRegionGrowingInCylinderOneFrame(const short minv, const short maxv)
{
  const EVec3i reso       = ImageCore::GetInst()->GetReso();
  const int    num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const int    frame_idx  = formVisParam_getframeI();

	m_thresh_min = minv;
	m_thresh_max = maxv;
  
  if( m_b_updated_cylinder ) FillInCylinder();
	m_b_updated_cylinder   = false;
  

  const short *img3d = ImageCore::GetInst()->m_img4d[frame_idx];
  byte  *flg3d       = ImageCore::GetInst()->m_flg4d[frame_idx];
    
  for (int i = 0; i < num_voxels; ++i) if (flg3d[i] != 0)  flg3d[i] = 1;

  //gen seed voxel
  TQueue<PixIdx4D> seed_voxels;
  InitializeRegionGrowQueue3D(frame_idx, img3d, flg3d, m_vol_cyl, m_cylinders, minv, maxv, seed_voxels);
  std::cout << seed_voxels.size() << "aaaaaaaaaaaaaaaaaa\n";

  //calc region growing 3D
  ComputeRegionGrowing3D(reso, img3d, m_vol_cyl, minv, maxv, seed_voxels, flg3d);
  //remove area close to other mask
  const int lockarea_bandwidth = formSegBolus_GetLockAreaBand();
  RemoveForeVoxelCloseToOtherMask( reso, lockarea_bandwidth, flg3d);
  
  m_b_modified = true;
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}



void ModeSegBolus::RunRegionGrowingInCylinderAllFrame(const short minv, const short maxv)
{
	m_thresh_min = minv;
	m_thresh_max = maxv;
  
  if( m_b_updated_cylinder ) FillInCylinder();
	m_b_updated_cylinder   = false;
  
  const EVec3i    reso   = ImageCore::GetInst()->GetReso();
  std::vector<byte* > &flg4d  = ImageCore::GetInst()->m_flg4d;
  std::vector<short*> &img4d  = ImageCore::GetInst()->m_img4d;
  const int   num_frames = (int)flg4d.size();
  const int   num_voxels = reso[0]* reso[1]* reso[2];
  const int lockarea_bandwidth = formSegBolus_GetLockAreaBand();

#pragma omp parallel for
  for ( int fi = 0; fi < num_frames; ++fi )
  {
    const short *img3d = img4d[fi];
    byte  *flg3d = flg4d[fi];
    
    for (int i = 0; i < num_voxels; ++i) if (flg3d[i] != 0)  flg3d[i] = 1;

    //gen seed voxel
    TQueue<PixIdx4D> seed_voxels;
    InitializeRegionGrowQueue3D(fi, img3d, flg3d, m_vol_cyl, m_cylinders, minv, maxv, seed_voxels);
    //calc region growing 3D
    ComputeRegionGrowing3D(reso, img3d, m_vol_cyl, minv, maxv, seed_voxels, flg3d);
    //remove area close to other mask
	  RemoveForeVoxelCloseToOtherMask( reso, lockarea_bandwidth , flg3d);
  }
  //todo regionGrowAllFrame

  m_b_modified = true;
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}



void ModeSegBolus::RunRegionGrowingInCylinder4D(const short minv, const short maxv)
{
  if( m_b_updated_cylinder ) FillInCylinder();
	m_b_updated_cylinder = false;
	m_thresh_min = minv;
	m_thresh_max = maxv;
  
  int W,H,D,WH,WHD;
  std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();
  const std::vector<short*> &img4d  = ImageCore::GetInst()->m_img4d;
  const int num_frames   = ImageCore::GetInst()->GetNumFrames();
  std::vector<byte* > &flg4d  = ImageCore::GetInst()->m_flg4d;

  
#pragma omp parallel for
  for ( int fi = 0; fi < num_frames; ++fi )
  {
    const short *img3d = img4d[fi];
    byte  *flg3d = flg4d[fi];
    for (int i = 0; i < WHD; ++i) if (flg3d[i] != 0)  flg3d[i] = 1;

  }

  //get seed voxels
  std::vector<TQueue<PixIdx4D>> seed_voxels(num_frames);
  for ( int fi = 0; fi < num_frames; ++fi )
  {
    const short *img3d = img4d[fi];
    const byte  *flg3d = flg4d[fi];
    InitializeRegionGrowQueue3D(fi, img3d, flg3d, m_vol_cyl, m_cylinders, minv, maxv, seed_voxels[fi]);
  }

  TQueue<PixIdx4D> Q;
  for ( int fi = 0; fi < num_frames; ++fi ) 
  {
    for ( int i =0; i < seed_voxels[fi].size(); ++i)
    {
      int idx = seed_voxels[fi][i].idx;
      Q.push_back( PixIdx4D( idx, seed_voxels[fi][i].frameI) );
      flg4d[fi][idx] = 255;
    }
  }

  //compute growth
  while (!Q.empty())
	{
		const int idx = Q.front().idx;
		const int  x  =  idx % W;
		const int  y  = (idx / W) % H;
		const int  z  =  idx / WH;
    const int  f  = Q.front().frameI;
		Q.pop_front();
    short* img3d = img4d[f]; 
    byte*  flg3d = flg4d[f]; 

		//grow to 8 neighbors
		int K;
		K = idx - 1; if (x > 0 && m_vol_cyl[K] && flg3d[K]==1 && minv<=img3d[K] && img3d[K]<=maxv) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
		K = idx - W; if (y > 0 && m_vol_cyl[K] && flg3d[K]==1 && minv<=img3d[K] && img3d[K]<=maxv) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
		K = idx -WH; if (z > 0 && m_vol_cyl[K] && flg3d[K]==1 && minv<=img3d[K] && img3d[K]<=maxv) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
		K = idx + 1; if (x <W-1&& m_vol_cyl[K] && flg3d[K]==1 && minv<=img3d[K] && img3d[K]<=maxv) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
		K = idx + W; if (y <H-1&& m_vol_cyl[K] && flg3d[K]==1 && minv<=img3d[K] && img3d[K]<=maxv) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
		K = idx +WH; if (z <D-1&& m_vol_cyl[K] && flg3d[K]==1 && minv<=img3d[K] && img3d[K]<=maxv) { flg3d[K] = 255; Q.push_back(PixIdx4D(K, f)); }
    //time direction
    K=idx;
    if ( f > 0          && m_vol_cyl[K] && flg4d[f-1][K]==1 && minv<=img4d[f-1][K] && img4d[f-1][K]<=maxv) { flg4d[f-1][K] = 255; Q.push_back(PixIdx4D(K, f-1)); }
    if ( f <num_frames-1&& m_vol_cyl[K] && flg4d[f+1][K]==1 && minv<=img4d[f+1][K] && img4d[f+1][K]<=maxv) { flg4d[f+1][K] = 255; Q.push_back(PixIdx4D(K, f+1)); }
	}


  //remove foreground close to other mask
  int lockarea_bandwidth = formSegBolus_GetLockAreaBand();
  for( int f = 0; f < num_frames; ++f)
  {

    RemoveForeVoxelCloseToOtherMask( ImageCore::GetInst()->GetReso(), lockarea_bandwidth, flg4d[f]);
  }

  m_b_modified = true;
  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}

#pragma managed



/*
EMat4f ModeSegBolus::boneRegistration(string srcBoneObjFilePath, int targetMaskID)
{
  auto SetOpenCvMatrix = [](const TMesh &mesh, cv::Mat& matrix)
                          {
                            const int num_tri = mesh.m_pSize;

                            matrix = cv::Mat(num_tri, 6, CV_32F);

                            EVec3f gc;
                            for (int n = 0; n < num_tri; ++n)
                            {
                              gc = mesh.m_vVerts[mesh.m_pPolys[n].idx[0]]
                                 + mesh.m_vVerts[mesh.m_pPolys[n].idx[1]]
                                 + mesh.m_vVerts[mesh.m_pPolys[n].idx[2]];

                              gc /= 3.0;
                              matrix.at<float>(n, 0) = gc[0];
                              matrix.at<float>(n, 1) = gc[1];
                              matrix.at<float>(n, 2) = gc[2];
                              matrix.at<float>(n, 3) = mesh.m_pNorms[n][0];
                              matrix.at<float>(n, 4) = mesh.m_pNorms[n][1];
                              matrix.at<float>(n, 5) = mesh.m_pNorms[n][2];
                            }
                          };

	TMesh sourceBone;
	if ( !sourceBone.initialize(srcBoneObjFilePath.c_str()) )
		return EMat4f::Identity();

	vector<MaskData> &mask = ImageCore::GetInst()->m_mask_data;

	TMesh targetBone;
	convertMaskToSurf(targetMaskID, targetBone);

  cv::Mat sourceMat;
  cv::Mat targetMat;

  SetOpenCvMatrix(sourceBone, sourceMat);
  SetOpenCvMatrix(targetBone, targetMat);

  EMat4d initMat = EMat4d::Identity();
  cv::ppf_match_3d::Pose3DPtr M1 = new cv::ppf_match_3d::Pose3D();
  M1->updatePose(initMat.data());
  vector<cv::ppf_match_3d::Pose3DPtr> poses;
  poses.push_back(M1);
  
  float icpRejectScale = 2.5;	// (float)m_spin_ICPrejectScale.GetPos32();
  int icpNumLevels = 6;// m_spin_ICPnumLv.GetPos32();
  try
  {
    // Create an instance of ICP
    cv::ppf_match_3d::ICP icp(500, 0.005f, icpRejectScale, icpNumLevels);
    int i = icp.registerModelToScene(sourceMat, targetMat, poses);
  }
  catch (const cv::Exception)
  {
    cout << "*** surfaceの形状または位置が大きく異なるため追跡に失敗しました ***\n";
  }

  EMat4f resM;
  resM(0, 0) = (float)poses[0]->pose[0]; resM(1, 0) = (float)poses[0]->pose[4]; resM(2, 0) = (float)poses[0]->pose[8];  resM(3, 0) = (float)poses[0]->pose[12];
  resM(0, 1) = (float)poses[0]->pose[1]; resM(1, 1) = (float)poses[0]->pose[5]; resM(2, 1) = (float)poses[0]->pose[9];  resM(3, 1) = (float)poses[0]->pose[13];
  resM(0, 2) = (float)poses[0]->pose[2]; resM(1, 2) = (float)poses[0]->pose[6]; resM(2, 2) = (float)poses[0]->pose[10]; resM(3, 2) = (float)poses[0]->pose[14];
  resM(0, 3) = (float)poses[0]->pose[3]; resM(1, 3) = (float)poses[0]->pose[7]; resM(2, 3) = (float)poses[0]->pose[11]; resM(3, 3) = (float)poses[0]->pose[15];

  cout << "fin icp\n";

	////////////   only for debug    ///////////////////////
  //sourceBone.MultMat(resM);
  //sourceBone.exportObjNoTexCd( (srcBoneObjFilePath + "regist.obj").c_str() );
	////////////////////////////////////////////////////////

  cout << resM << endl << endl;

  return resM;
}
*/


/*
void ModeSegBolus::convertMaskToSurf(int selectedMskId, TMesh& targetMesh)
{
	const ImageCore* imCore = ImageCore::GetInst();
	if (selectedMskId < 0 || imCore->m_mask_data.size() <= selectedMskId) return;

	const EVec3i reso = ImageCore::GetInst()->GetReso();
	const EVec3f pitch = ImageCore::GetInst()->GetPitch();

	const int N = reso[0] * reso[1] * reso[2];

	short *v = new short[N];
	int frameN = (int)imCore->m_img4d.size();
	//targetは最終フレームで固定
	for (int i = 0; i < N; ++i) v[i] = (imCore->m_mask4d[frameN - 1][i] == selectedMskId) ? 255 : 0;

	marchingcubes::t_MarchingCubes(reso, pitch, v, 128, 0, 0, targetMesh);
	targetMesh.smoothing(2);

	delete[] v;
}
*/

