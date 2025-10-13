#pragma unmanaged
#include "ModeSegLocalRGrow.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"
#include "LogCore.h"
#include "thandle3d.h"
#include <fstream>
#include <deque>
#include <vector>
#include <time.h>
#include <map>

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegLocalRGrow.h"
#include "climessagebox.h"
#include "OglForCLI.h"

#pragma unmanaged

using namespace RoiPainter3D;


ModeSegLocalRGrow::~ModeSegLocalRGrow()
{
}

ModeSegLocalRGrow::ModeSegLocalRGrow() :
	m_volume_shader("shader/volVtx.glsl"   , "shader/volFlg_LRG.glsl"   ),
	m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_LRG.glsl")
{
  m_bL = m_bR = m_bM = false;
  m_activeseed_idx = -1;
  m_drag_seedcp_id << -1, -1;
}



//flg useage 
// 0   : locked 
// 1   : not yet segmented (background) 
// 2   : fixed background (negative) region
// 3   : now visiting
// 255 : fixed foreground (pogitive) region

void ModeSegLocalRGrow::StartMode()
{
  //init vol_flg 0:locked, 1:others
  ImageCore::GetInst()->InitializeVolFlgByLockedMask();

  m_activeseed_idx = -1;

  //init field & control point
	m_b_drawstroke = false;
	m_stroke.clear();
  m_seeds .clear();
  LRGSeed::SetCpRadius( ImageCore::GetInst()->GetPitch()[0] * 4.0f );

  //show dialog
  formSegLocalRGrow_Show();

  float  max_radius = ImageCore::GetInst()->GetCuboid()[0];
  EVec2i minmax     = ImageCore::GetInst()->GetVolMinMax();
  formSegLocalRGrow_setSliderRange( max_radius, minmax[0], minmax[1]);
  formSegLocalRGrow_updateAllItems();

  //Lock/Unlock pitch box
  formVisParam_LockPitchBox();

  
  //only for user study
  //LogCore::GetInst()->StartLogger();
}


bool ModeSegLocalRGrow::CanLeaveMode()
{
	if ( m_seeds.size() == 0 ) return true;
  if ( !CLI_MessageBox_YESNO_Show("Current Result is not stored. Do you want to leave?", "message") )
	{
		return false;
	}
	return true;
}



void ModeSegLocalRGrow::FinishSegmentation()
{
	if ( !ImageCore::GetInst()->bForeVoxelsExistInVolFlg() ) 
	{
		CLI_MessageBox_OK_Show("No foreground region exist", "message");
		return;
	}

  //export log
  //time_t currenttime = time(NULL);
  //struct tm *t = localtime(&currenttime);
  //std::string fname = 
  //    to_string( t->tm_year+1900 )
  //  + to_string( t->tm_mon + 1   )
  //  + string((t->tm_mday < 10 ) ? "0" : "") + to_string( t->tm_mday )
  //  + string((t->tm_hour < 10 ) ? "0" : "") + to_string( t->tm_hour )
  //  + string((t->tm_min  < 10 ) ? "0" : "") + to_string( t->tm_min  )
  //  + string((t->tm_sec  < 10 ) ? "0" : "") + to_string( t->tm_sec  )
  //  + string("_log.txt");

  //LogCore::GetInst()->CloseLogger(fname);

  //ExportSeedInfo(fname + string("seedinfo.txt") );


  m_seeds.clear();
	ImageCore::GetInst()->StoreForegroundAsNewMask();
	ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
  RedrawScene();
}




void  ModeSegLocalRGrow::ExportSeedInfo(std::string fname)
{
  //export 
  std::ofstream ofs( fname.c_str() );

  if (!ofs){
    std::cout << "failed to open the file " << fname << "\n";
    return;
  }

  ofs << "Seedinfo\n";
  ofs << "num_seed " << m_seeds.size() << "\n";
  for ( auto s : m_seeds) 
  {
    ofs << "num_cps " << s.m_cps.size() << "\n";
    ofs << "flg_thresh_  " << s.m_flg_fore << " " 
                           << s.m_min_v    << " " 
                           << s.m_max_v    << " " 
                           << s.m_radius   << "\n";

    for ( auto c : s.m_cps )
      ofs << c[0] << " " << c[1] << " " << c[2] << "\n";
  }
  ofs.close();
}



void  ModeSegLocalRGrow::ImportSeedInfo(std::string fname)
{
  m_activeseed_idx = -1;
  m_seeds .clear();
  auto minmax = ImageCore::GetInst()->GetVolMinMax();

  std::ifstream ifs( fname.c_str() );
 
  if (!ifs)
  {
    std::cout << "failed to open the file " << fname << "\n";
  }
 
  std::string buf;
  int num_seeds; 
  ifs >> buf; // "Seedinfo\n"
  ifs >> buf >> num_seeds; // "Seedinfo\n"

  for ( int i=0; i < num_seeds; ++i )
  {
    int num_cps;
    ifs >> buf >> num_cps; // "Seedinfo\n"
    
    int flg, minv, maxv;
    float radius;
    ifs >> buf >> flg >> minv >> maxv >> radius; 
    minv = Crop(minmax[0], minmax[1], minv);
    maxv = Crop(minmax[0], minmax[1], maxv);
    m_seeds.push_back(LRGSeed(EVec3f(0,0,0), minv, maxv, flg, radius));
    
    m_seeds.back().m_cps.resize(num_cps);
    for ( auto& c : m_seeds.back().m_cps)
    {
      ifs >> c[0] >> c[1] >> c[2];    
    }
  }
  
  formSegLocalRGrow_updateAllItems();

}






void ModeSegLocalRGrow::CancelSegmentation()
{
  m_seeds.clear();
  formSegLocalRGrow_updateAllItems();
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
  RedrawScene();
}



int  ModeSegLocalRGrow::GetLatestAddedSeedIdx(bool tf)
{
  for ( int i = (int)m_seeds.size() - 1; i >= 0;  --i)
  {
    if ( tf && m_seeds[i].m_flg_fore ) return i;
    if ( !tf && !m_seeds[i].m_flg_fore) return i;
  }
  return -1;
}



///////////////////////////////////////////////////////////////////////
//Mouse Listener///////////////////////////////////////////////////////
// current version(現在のもの)
// Select seed    : L/R click on CP                   ok
// Move CP        : L/R drag  on CP                   ok
// Add seed 1     : L/R dblclick when no active seed  ok
// Add seed 2     : Button on Dialog  ( new! )        ok 
// Add seed cp    : L dblclick when active seed exist ok
// Remove seed(cp): L/R/M double click                ok 
//wheeling : zoom or modify radius or move slice      ok
void ModeSegLocalRGrow::AddNewSeed(bool b_foreseed, EVec3f &cp_position)
{
  //LogCore::GetInst()->Add("AddNewSeed");

  const EVec3f cuboid = ImageCore::GetInst()->GetCuboid();
  const EVec2i minmax = ImageCore::GetInst()->GetVolMinMax();  
  const short  vol_v  = ImageCore::GetInst()->GetVoxelValue(cp_position);

  //get initial threshold
  int prev_seedidx = GetLatestAddedSeedIdx( b_foreseed );
  short min_v, max_v;
  float radius;
  if ( prev_seedidx != -1 )
  {
	  min_v  = m_seeds[prev_seedidx].m_min_v ;
	  max_v  = m_seeds[prev_seedidx].m_max_v ;
	  radius = m_seeds[prev_seedidx].m_radius;
  }
  else
  {
	  min_v  = b_foreseed ? vol_v - 100 : minmax[0]  ;
	  max_v  = b_foreseed ? minmax[1]   : vol_v + 100;
	  radius = b_foreseed ? cuboid[0] * 0.05f : cuboid[0] * 0.02f;
  }

  min_v = std::max( min_v, (short)minmax[0]);
  max_v = std::min( max_v, (short)minmax[1]);

  m_seeds.push_back( LRGSeed( cp_position, min_v, max_v, b_foreseed, radius) );
	m_activeseed_idx = (int)m_seeds.size() - 1;

  //sort seed (Original code was by Hiryu Kamoshita)
  //negativeは前半に積む
  if( m_seeds.back().m_flg_fore == false)
  {
    for(int i = (int)m_seeds.size()-1; i > 0 ; --i) 
	  {
      //手前がnegativeなら終了
		  if( !m_seeds[i-1].m_flg_fore ) break;
      //そうでなければ入れ替え
      std::swap(m_seeds[i-1], m_seeds[i] );
      m_activeseed_idx = i-1;
	  }
  }
}



void ModeSegLocalRGrow::AddNewSeed(bool b_foreseed)
{
  //fix position (表示されている planeの中心に配置)
  const EVec3f cuboid = ImageCore::GetInst()->GetCuboid();

  EVec3f cp_position;
  if ( formVisParam_bPlaneXY() )
  {
    const float plane_xy = CrssecCore::GetInst()->GetPlaneXyPosition();
    cp_position << cuboid[0]/2, cuboid[1]/2, plane_xy * cuboid[2];
  }
  else if ( formVisParam_bPlaneYZ() )
  {
    const float plane_yz = CrssecCore::GetInst()->GetPlaneYzPosition();
    cp_position << plane_yz * cuboid[0], cuboid[1]/2, cuboid[2]/2;
  }
  else if ( FormVisParam_bPlaneZX() )
  {
    const float plane_zx = CrssecCore::GetInst()->GetPlaneZxPosition();
    cp_position << cuboid[0]/2, plane_zx * cuboid[1], cuboid[2]/2;
  }
  else
  {
    cp_position = cuboid * 0.5;
  }
  AddNewSeed( b_foreseed, cp_position);
  
}



bool ModeSegLocalRGrow::PickToRemoveSeedCP(
    const EVec3f &ray_pos, 
    const EVec3f &ray_dir)
{
	EVec2i seedid_cpid = PickSeedCP(ray_pos, ray_dir);
  int seedId = seedid_cpid[0];
  int cpId   = seedid_cpid[1];
	  
  if(0 <= seedId && seedId < (int)m_seeds.size() ) 
  {
		m_seeds[ seedId ].m_cps.erase( m_seeds[seedId].m_cps.begin() + cpId );
    //LogCore::GetInst()->Add("RemoveCP");
    
    if( m_seeds[ seedId ].m_cps.size() == 0 )
    {
		  m_seeds.erase( m_seeds.begin() + seedId );

      if ( m_activeseed_idx == seedId )
        m_activeseed_idx = -1;
      else if ( m_activeseed_idx > seedId )
        m_activeseed_idx--;
    }
    formSegLocalRGrow_updateAllItems();
    return true;
  }
  return false;
}



bool ModeSegLocalRGrow::PickCrssecToaddCP(
    const EVec3f &ray_pos, 
    const EVec3f &ray_dir, 
    const bool b_fore)
{
  EVec3f pos;

  if ( PickCrssec(ray_pos, ray_dir, &pos) != CRSSEC_NON)
  {
    if ( m_activeseed_idx  == -1 ) 
    {
      AddNewSeed(b_fore, pos);
    }
    else
    {
      m_seeds[ m_activeseed_idx ].m_cps.push_back(pos);
      //LogCore::GetInst()->Add("AddCP");
    }
    formSegLocalRGrow_updateAllItems();
    return true;
  }  
  return false;
}






void ModeSegLocalRGrow::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{
  //LogCore::GetInst()->Add("LBtnDclk");

  //pickしたシードを削除 or 新しいシードを追加
  EVec3f ray_pos, ray_dir;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if ( !PickToRemoveSeedCP(ray_pos, ray_dir) ) 
  {
    PickCrssecToaddCP(ray_pos, ray_dir, true );
  }
  RedrawScene();
}



void ModeSegLocalRGrow::RBtnDclk(const EVec2i &p, OglForCLI *ogl) 
{
  //LogCore::GetInst()->Add("RBtnDclk");

  //pickしたシードを削除 or 新しいシードを追加
  EVec3f ray_pos, ray_dir;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if ( !PickToRemoveSeedCP(ray_pos, ray_dir) ) 
  {
    PickCrssecToaddCP(ray_pos, ray_dir, false );
  }
  RedrawScene();
}


void ModeSegLocalRGrow::MBtnDclk(const EVec2i &p, OglForCLI *ogl) 
{
  //LogCore::GetInst()->Add("MBtnDclk");

  //pick to remove seed
  EVec3f ray_pos, ray_dir;
	ogl->GetCursorRay(p, ray_pos, ray_dir);
  PickToRemoveSeedCP(ray_pos, ray_dir);
  RedrawScene();
}



void ModeSegLocalRGrow::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  //LogCore::GetInst()->Add("LBtnDown");

  m_bL = true;
  
  if ( IsCtrKeyOn() )
  {
    m_b_drawstroke = true;
    m_stroke.clear();
    return;
  }
  
  //pick to activate and drag 
  EVec3f ray_pos, ray_dir;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  m_drag_seedcp_id = PickSeedCP(ray_pos, ray_dir);
  if ( m_drag_seedcp_id[0] != -1)
  {
    m_activeseed_idx = m_drag_seedcp_id[0];
    formSegLocalRGrow_updateAllItems();
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }

  RedrawScene();
}



void ModeSegLocalRGrow::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  //if ( m_drag_seedcp_id[0] != -1)
  //  LogCore::GetInst()->Add("MoveCP");

  if (m_b_drawstroke)
	{
    EVec3f cuboid  = ImageCore::GetInst()->GetCuboid();
    EVec3f cam_pos = ogl->GetCamPos();
		CrssecCore::GetInst()->GenerateCurvedCrssec( cuboid, cam_pos, m_stroke );
	}

	m_b_drawstroke = false;
  m_bL = false;
	m_drag_seedcp_id << -1, -1;
	ogl->BtnUp();
  RedrawScene();
}



void ModeSegLocalRGrow::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{	
  //LogCore::GetInst()->Add("RBtnDown");
  m_bR = true;
  
  //pick to activate and drag 
  EVec3f ray_pos, ray_dir;
  ogl->GetCursorRay(p, ray_pos, ray_dir);
  m_drag_seedcp_id = PickSeedCP(ray_pos, ray_dir);

  if ( m_drag_seedcp_id[0] != -1){
    m_activeseed_idx = m_drag_seedcp_id[0];
    formSegLocalRGrow_updateAllItems();
  }
  else
  {
    ogl->BtnDown_Rot(p);
  }

  RedrawScene();
}



void ModeSegLocalRGrow::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnUp();
  m_bR = false;
  m_drag_seedcp_id << -1, -1;
  RedrawScene();
}



void ModeSegLocalRGrow::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  //LogCore::GetInst()->Add("MBtnDown");

  m_bM = true;

  //pick to remove seed
  EVec3f ray_pos, ray_dir;
	ogl->GetCursorRay(p, ray_pos, ray_dir);
  if ( PickToRemoveSeedCP(ray_pos, ray_dir) ) 
  {
    formSegLocalRGrow_updateAllItems();
    RedrawScene();
  }
  else 
  {
    ogl->BtnDown_Zoom(p);
  }
}



void ModeSegLocalRGrow::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnUp();
	m_bM = false;
	m_drag_seedcp_id << -1, -1;
  RedrawScene();
}



EVec2i ModeSegLocalRGrow::PickSeedCP(const EVec3f &ray_pos, const EVec3f &ray_dir)
{
  //cross sectionの後ろは触らない
  float max_depth = FLT_MAX;
  EVec3f pos;
  if ( PickCrssec(ray_pos, ray_dir, &pos) != CRSSEC_NON)
  {
    max_depth = (pos - ray_pos).norm();
  }


  float CP_RADIUS = LRGSeed::GetCpRadius();
  for(int i = 0; i < (int) m_seeds.size(); ++i)
	{
		for(int j = 0; j < (int) m_seeds[i].m_cps.size(); ++j)
		{
      const EVec3f p = m_seeds[i].m_cps[j];

			if ( DistRayAndPoint(ray_pos, ray_dir, p ) < CP_RADIUS && 
           (p-ray_pos).norm() < max_depth ) 
      {
				return EVec2i(i,j);
      }
		}
	}	
	return EVec2i( -1, -1);
}





bool ModeSegLocalRGrow::PickActiveSeed(const EVec3f &ray_pos, const EVec3f &ray_dir, EVec3f &pos)
{
	if ( m_activeseed_idx < 0 || m_seeds.size() <= m_activeseed_idx)
    return false;

  //cross sectionの後ろは触らない
  float max_depth = FLT_MAX;
  EVec3f crssec_pos;
  if ( PickCrssec(ray_pos, ray_dir, &crssec_pos) != CRSSEC_NON)
  {
    max_depth = (crssec_pos - ray_pos).norm();
  }


  LRGSeed &s = m_seeds[m_activeseed_idx];

  for ( const auto &cp : s.m_cps)
  {
    if ( DistRayAndPoint(ray_pos, ray_dir, cp) < s.m_radius 
         && (ray_pos - cp).norm() - s.m_radius < max_depth)
    {
      pos = cp;
      return true; 
    }
  }
  
  for ( int i=1; i < (int)s.m_cps.size(); ++i)
  {
    const EVec3f cp1 = s.m_cps[i-1];
    const EVec3f cp2 = s.m_cps[ i ];
    //float t1, t2; 
    //if (t_distRayToLineSegment(ray_pos, ray_dir, cp1, cp2, t1, t2) < s.m_radius) {
    //pos = (t2 < 0) ? cp1 :
    //  (t2 > 1.0f) ? cp2 :
    //  ray_pos + t1 * ray_dir;

    auto res = DistRayAndLineSegm(ray_pos, ray_dir, cp1, cp2);
    float  dist = std::get<0>(res);
    EVec3f pos  = std::get<1>(res);

    if (dist < s.m_radius)
    {
      if ( (ray_pos - pos).norm() - s.m_radius < max_depth )
        return true; 
    }
  } 
  return false;
}






void ModeSegLocalRGrow::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
	if ( !m_bL && !m_bR && !m_bM ) return;

	EVec3f ray_pos, ray_dir, pos;
	ogl->GetCursorRay( p, ray_pos, ray_dir);

	if ( m_b_drawstroke )
	{
		m_stroke.push_back( ray_pos + 0.1f * ray_dir );
	}
	else if ( m_drag_seedcp_id[0] != -1 )
	{
    if ( PickCrssec( ray_pos, ray_dir, &pos) != CRSSEC_NON )
    {
      m_seeds[ m_drag_seedcp_id[0] ].m_cps[ m_drag_seedcp_id[1] ] = pos;
    }
	}
  else 
	{
		ogl->MouseMove( p );
	}

  RedrawScene(false);
}



void ModeSegLocalRGrow::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  //active seed があり，それをピックしていればその半径を変更する 
  //crs sectionを触っていれば移動
  //そうでなければredraw
  EVec3f ray_pos, ray_dir, seedpos, crssecpos;
  ogl->GetCursorRay( p, ray_pos, ray_dir);
  
  bool pickseed   = PickActiveSeed(ray_pos, ray_dir , seedpos    );
  bool pickcrssec = PickCrssec    (ray_pos, ray_dir, &crssecpos );
  if ( pickseed && pickcrssec)
  { 
    //手前を採用
    EVec3f e = ogl->GetCamPos();
    if ( (seedpos - e).norm() > (crssecpos-e).norm() )
      pickseed = false;

  }
  
  
  if ( pickseed ) 
  {
    //LogCore::GetInst()->Add("WHEELSEED"); 

    float d = IsAltKeyOn() ? (float)(z_delta * 0.1) : (float)(z_delta * 0.001);
    m_seeds[ m_activeseed_idx ].ModifyRadius( d );
    formSegLocalRGrow_updateSliders();
  } 
  else if( pickcrssec )
  {
    PickToMoveCrossSecByWheeling(p, ogl, (short) (3 * z_delta) ) ;
    //LogCore::GetInst()->Add("WHEELCRSSEC"); 
  }
  else
  {
    //LogCore::GetInst()->Add("WHEELZOOM"); 
    ogl->ZoomCamera(z_delta * 0.1f);
  }
  RedrawScene();
}



void ModeSegLocalRGrow::KeyDown(int nChar) 
{ 
  //LogCore::GetInst()->Add("KeyDown");

  if ( nChar == 'n' || nChar == 'N' )
  {
    m_activeseed_idx = -1;
    formSegLocalRGrow_updateAllItems();
  }

  RedrawScene(); 
}


void ModeSegLocalRGrow::KeyUp  (int nChar) 
{
  RedrawScene(); 
}



void ModeSegLocalRGrow::DrawScene(const EVec3f &cuboid, const EVec3f &cam_pos, const EVec3f &cam_center)
{
  
	//bind volumes ---------------------------------------
  BindAllVolumes();

	//render cross sections ----------------------------------
  const EVec3i reso = ImageCore::GetInst()->GetResolution();
  DrawCrossSections(cuboid, reso, m_crssec_shader);

	//volume rendering ---------------------------------------
  const bool   b_draw_vol = formVisParam_bRendVol();
	if ( b_draw_vol && !IsSpaceKeyOn() )
	{
    const bool  b_pse   = formVisParam_bDoPsued();
    const bool  b_roi   = formVisParam_GetOtherROI();
    const float alpha   = formVisParam_getAlpha();
    const bool  b_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int   n_slice = (int)((b_manip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());


		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		m_volume_shader.Bind(0, 1, 2, 3, 4, 5, 6, alpha * 0.1f, reso, cam_pos, b_pse, b_roi);
		t_DrawCuboidSlices(n_slice, cam_pos, cam_center, cuboid);
		m_volume_shader.Unbind();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

  //draw seeds
  if( !IsSpaceKeyOn() && !IsAltKeyOn()) 
  {
    for( const auto& s : m_seeds) s.Draw( );
	  if(0 <= m_activeseed_idx && m_activeseed_idx < m_seeds.size())
    {
      m_seeds[m_activeseed_idx].DrawAsActive( );
    }
  }

  if ( IsAltKeyOn() ) // for figure of the paper 
  {
    //小さい順に書く
    std::multimap<float, int> idx_map;
    for ( int i=0; i < m_seeds.size(); ++i) 
      idx_map.insert(std::make_pair( m_seeds[i].m_radius,i ));

    for( const auto& s : idx_map) m_seeds[s.second].DrawAsActive( );
  }

	
	if (m_b_drawstroke)
  {
    DrawPolyLine(EVec3f(1,1,0), 3, m_stroke);
  }
}



















static void s_LocalRegionGrow
(
	const int W,
	const int H,
	const int D,
	const LRGSeed &seed ,
  const short   *vol  ,
	const EVec3f  &pitch,

	byte* vol_f
);


// Local Region Grow
// flg useage 
// 0   : lock 
// 1   : yet
// 2   : negative region
// 3   : now visiting
// 255 : pogitive region
void ModeSegLocalRGrow::RunLocalRegionGrow()
{

  //LogCore::GetInst()->Add("RunLocalRegionGrow");


  int W,H,D,WH,WHD;
  std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();
  const short *vol   = ImageCore::GetInst()->m_vol_orig     ;
	const EVec3f pitch = ImageCore::GetInst()->GetPitch()     ;

  byte *flg3d = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();

	for (int i = 0; i < WHD; ++i ) flg3d[i] = (flg3d[i] == 0) ? 0 : 1;

	byte *flg = new byte[ WHD ];

  //seed毎にRegion growingを計算
	for( int j = 0; j < (int) m_seeds.size(); ++j)
	{
    //flg初期化(0:locked, 1:yet fixed, 2:negative, 255:positive)
    //前景seed : locked(0)とnegative(2)以外をyet(1)に
    //背景seed : 領域計算は全体に対して行う
		if ( m_seeds[j].m_flg_fore)
    {
      for ( int i = 0; i < WHD; ++i) 
        flg[i] = (flg3d[i] == 255) ? 1 : flg3d[i];
    }
		else
    {
      for ( int i = 0; i < WHD; ++i) 
        flg[i] = 1;
    }
	
    //region grow
		s_LocalRegionGrow( W,H,D, m_seeds[j], vol, pitch, flg);
		
		for (int i = 0; i < WHD; ++i ) 
      if(flg[i] != 1) 
        flg3d[i] = flg[i];
	}

	delete[] flg;

  ImageCore::GetInst()->m_vol_flag.SetUpdated();
	std::cout << "Local Region Grow done\n";
}





// grow distans transform volume (vol_dist)
// return minimum value of grown voxel
static float s_DistTransGrowFronteer
(
    const int W, const int H, const int D,
    const EVec3f &pitch, 

    std::deque<EVec4i> &Q1, //input : current fronteer 
    std::deque<EVec4i> &Q2, //output: new fronteer

    float *vol_dist,
    byte  *vol_flg  
)
{
	const int WH = W*H, WHD = W*H*D;
	float pX   = pitch[0];
  float pY   = pitch[1];
  float pZ   = pitch[2];
	float pXY  = sqrt(pX*pX + pY*pY        );
	float pYZ  = sqrt(        pY*pY + pZ*pZ);
	float pXZ  = sqrt(pX*pX         + pZ*pZ);
	float pXYZ = sqrt(pX*pX + pY*pY + pZ*pZ);
	
	float min_dist = FLT_MAX; 

	while(!Q1.empty())
	{
		const int x = Q1.front()[0];
		const int y = Q1.front()[1];
		const int z = Q1.front()[2];
		const int I = Q1.front()[3];
		Q1.pop_front();

		int K ;
	  
    //1. vol[I]の26近傍を見て，距離が最小となるものを入れる
    //2. one ring分growさせる
		//calc Dist for voxel (x,y,z) 
    //unisotropic volumeで、seedが曲線状なのでこの実装のほうが多少正確
		K = I-1-W-WH; if(x>0  &&y>0  &&z>0	&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I  -W-WH; if(       y>0  &&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pYZ  );
		K = I+1-W-WH; if(x<W-1&&y>0  &&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I-1  -WH; if(x>0         &&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXZ  );
		K = I    -WH; if(              z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pZ   );
		K = I+1  -WH; if(x<W-1       &&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXZ  );
		K = I-1+W-WH; if(x>0  &&y<H-1&&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I  +W-WH; if(       y<H-1&&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pYZ  );
		K = I+1+W-WH; if(x<W-1&&y<H-1&&z>0  && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		
		K = I-1-W   ; if(x>0  &&y>0         && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXY );
		K = I  -W   ; if(       y>0         && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pY  );
		K = I+1-W   ; if(x<W-1&&y>0         && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXY );
		K = I-1     ; if(x>0                && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pX  );
		K = I+1     ; if(x<W-1              && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pX  );
		K = I-1+W   ; if(x>0  &&y<H-1       && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXY );
		K = I  +W   ; if(       y<H-1       && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pY  );
		K = I+1+W   ; if(x<W-1&&y<H-1       && vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXY );

		K = I-1-W+WH; if(x>0  &&y>0  &&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I  -W+WH; if(       y>0  &&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pYZ  );
		K = I+1-W+WH; if(x<W-1&&y>0  &&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I-1  +WH; if(x>0         &&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXZ  );
		K = I    +WH; if(              z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pZ   );
		K = I+1  +WH; if(x<W-1       &&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXZ  );
		K = I-1+W+WH; if(x>0  &&y<H-1&&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I  +W+WH; if(       y<H-1&&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pYZ  );
		K = I+1+W+WH; if(x<W-1&&y<H-1&&z<D-1&& vol_flg[K]) vol_dist[I] = std::min( vol_dist[I], vol_dist[K] + pXYZ );

		//grow fronteer to deque
		K = I-1-W-WH; if(x>0  &&y>0  &&z>0	&&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y-1, z-1, K)); vol_flg[K] = 1;}	
		K = I  -W-WH; if(       y>0  &&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y-1, z-1, K)); vol_flg[K] = 1;}	
		K = I+1-W-WH; if(x<W-1&&y>0  &&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y-1, z-1, K)); vol_flg[K] = 1;}	
		K = I-1  -WH; if(x>0         &&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y  , z-1, K)); vol_flg[K] = 1;}	
		K = I    -WH; if(              z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y  , z-1, K)); vol_flg[K] = 1;}
		K = I+1  -WH; if(x<W-1       &&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y  , z-1, K)); vol_flg[K] = 1;}
		K = I-1+W-WH; if(x>0  &&y<H-1&&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y+1, z-1, K)); vol_flg[K] = 1;}
		K = I  +W-WH; if(       y<H-1&&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y+1, z-1, K)); vol_flg[K] = 1;}
		K = I+1+W-WH; if(x<W-1&&y<H-1&&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y+1, z-1, K)); vol_flg[K] = 1;}
		
		K = I-1-W   ; if(x>0  &&y>0         &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y-1, z  , K)); vol_flg[K] = 1;}
		K = I  -W   ; if(       y>0         &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y-1, z  , K)); vol_flg[K] = 1;}
		K = I+1-W   ; if(x<W-1&&y>0         &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y-1, z  , K)); vol_flg[K] = 1;}
		K = I-1     ; if(x>0                &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y  , z  , K)); vol_flg[K] = 1;}
		K = I+1     ; if(x<W-1              &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y  , z  , K)); vol_flg[K] = 1;}
		K = I-1+W   ; if(x>0  &&y<H-1       &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y+1, z  , K)); vol_flg[K] = 1;}
		K = I  +W   ; if(       y<H-1       &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y+1, z  , K)); vol_flg[K] = 1;}
		K = I+1+W   ; if(x<W-1&&y<H-1       &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y+1, z  , K)); vol_flg[K] = 1;}

		K = I-1-W+WH; if(x>0  &&y>0  &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y-1, z+1, K)); vol_flg[K] = 1;}
		K = I  -W+WH; if(       y>0  &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y-1, z+1, K)); vol_flg[K] = 1;}
		K = I+1-W+WH; if(x<W-1&&y>0  &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y-1, z+1, K)); vol_flg[K] = 1;}
		K = I-1  +WH; if(x>0         &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y  , z+1, K)); vol_flg[K] = 1;}
		K = I    +WH; if(              z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y  , z+1, K)); vol_flg[K] = 1;}
		K = I+1  +WH; if(x<W-1       &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y  , z+1, K)); vol_flg[K] = 1;}
		K = I-1+W+WH; if(x>0  &&y<H-1&&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y+1, z+1, K)); vol_flg[K] = 1;}
		K = I  +W+WH; if(       y<H-1&&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y+1, z+1, K)); vol_flg[K] = 1;}
		K = I+1+W+WH; if(x<W-1&&y<H-1&&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y+1, z+1, K)); vol_flg[K] = 1;}

		min_dist = std::min( min_dist, vol_dist[I] );
	}
	return min_dist;
}



//Written by Shogo Tsuruoka 11/15
//refactored by Takashi Ijiri 11/29
static void s_calcDistTrans
(
	const std::deque<EVec4i> &seedPixels,
	const float max_dist, 
	float *vol_dist //allocated
)	
{
	std::cout << "start Distance Transform...\n";
	const EVec3f cuboid = ImageCore::GetInst()->GetCuboid();
	const EVec3f pitch  = ImageCore::GetInst()->GetPitch ();

  int W,H,D,WH,WHD;
  std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();
	
  //vol_flg -- 0:yet, 1:visited. 
	byte *vol_flg = new byte[WHD];

	for(int i = 0; i < WHD; ++i)
	{
		vol_dist[i] = FLT_MAX;
		vol_flg [i] = 0;
	}
	
  //sphere !
  if ( seedPixels.size() == 1 )
  {
    EVec3f s( (seedPixels[0][0]+0.5f)*pitch[0],
              (seedPixels[0][1]+0.5f)*pitch[1],
              (seedPixels[0][2]+0.5f)*pitch[2] );
    for ( int z = 0; z < D; ++z )
    {
      float zp = (z+0.5f) * pitch[2];
      if ( std::fabs( s[2]-zp) > max_dist ) continue;

      for ( int y = 0; y < H; ++y )
      {
        float yp = (y+0.5f) * pitch[1];
        if ( std::fabs( s[1]-yp) > max_dist ) continue;

        for ( int x = 0; x < W; ++x )
        {
          float xp = (x+0.5f) * pitch[0];
          if ( std::fabs( s[0]-xp) > max_dist ) continue;
          vol_dist[x+y*W+z*WH] = sqrt( (xp-s[0])*(xp-s[0]) + 
                                        (yp-s[1])*(yp-s[1]) + 
                                        (zp-s[2])*(zp-s[2]) );         
        }
      }
    }
    
    delete[] vol_flg;
    return;
  }


  std::deque<EVec4i> Q1, Q2;

	for (const auto &p : seedPixels)
	{
		Q1.push_back(p);
		vol_dist[ p[3] ] = 0.0;
		vol_flg [ p[3] ] = 1  ;
	}


	if( max_dist * 2 > cuboid[0] ) 
	{
    //R大きい場合は，distfieldの計算は無駄なので全て1にしてしまう
		for(int i = 0; i < WHD; ++i) vol_dist[i] = 1;
	}
	else
	{
		while( !Q1.empty() || !Q2.empty() )
		{
			float min_dist;
			min_dist = s_DistTransGrowFronteer(W,H,D,pitch, Q1,Q2, vol_dist, vol_flg);
			if( min_dist > max_dist) break;
			min_dist = s_DistTransGrowFronteer(W,H,D,pitch, Q2,Q1, vol_dist, vol_flg);			
			if( min_dist > max_dist) break;
		}
	}

	delete[] vol_flg;
  std::cout << "...Distance Transform Done\n";
}



//vol_f
//入力時 : 0:locked, 1:yet fixed
//出力時 : 0:locked, 1:yet(ないはず), 2:negative, 255:positive

static void s_LocalRegionGrow
(
	const int W,
	const int H,
	const int D,
	const LRGSeed &seed ,
  const short   *vol  ,
	const EVec3f  &pitch,

	byte* vol_f
)
{
	if( seed.m_cps.size() == 0 )
  {
    std::cout << "never comes here, s_LocalRegionGrow\n";
    return;
  }
  
  const int WH = W*H;

	//1 seed --> seed pixel (最初と最後の二点 + ポリラインをsampling
	std::deque<EVec4i> Q;

	Q.push_back( ImageCore::GetInst()->GetVoxelIndex4i( seed.m_cps.front() ) );
  if ( Q.size() >= 2)
	  Q.push_back( ImageCore::GetInst()->GetVoxelIndex4i( seed.m_cps.back () ) );

  float len = pitch.norm() * 2;
	for (int i = 1; i < seed.m_cps.size(); ++i)
	{
		const EVec3f &p1    = seed.m_cps[i - 1];
		const EVec3f &p2    = seed.m_cps[  i  ];
		const EVec3f dir    = (p2 - p1).normalized();
		const float  length = (p2 - p1).norm();

		for (float step_len = 0; step_len < length; step_len += pitch[0] * 0.5f) 
    {
      EVec4i voxidx = ImageCore::GetInst()->GetVoxelIndex4i(p1 + step_len * dir);
    
      if ( seed.m_min_v <= vol[voxidx[3]] && vol[voxidx[3]] <= seed.m_max_v)
        Q.push_back( voxidx );
    }
	}

	//2 distance transform
	const byte    VALUE    = seed.m_flg_fore ? 255 : 2;
	const float   max_dist = seed.m_radius ;
	const EVec3f &seedP    = seed.m_cps[0] ;
	const float   minv     = seed.m_min_v  ;
	const float   maxv     = seed.m_max_v  ;
  
	float *dist = new float[ W*H*D ];
  s_calcDistTrans( Q, max_dist, dist);

	//3  region growing for pix dist[i] < max_dist
	for (const auto &p : Q)
	{
		if (vol_f[p[3]] == 1) vol_f[p[3]] = VALUE;
	}

	while ( !Q.empty() )
	{
		const int x = Q.front()[0];
		const int y = Q.front()[1];
		const int z = Q.front()[2];
		const int I = Q.front()[3];
		Q.pop_front();
		
		if (vol_f[I] == 0 ) continue;
		
		int K;
    K = I - 1 ; if (x > 0   && vol_f[K] == 1 && dist[K] < max_dist && minv <= vol[K] && vol[K] <= maxv){ vol_f[K] = VALUE; Q.push_back(EVec4i(x - 1, y, z, K)); }
		K = I - W ; if (y > 0   && vol_f[K] == 1 && dist[K] < max_dist && minv <= vol[K] && vol[K] <= maxv){ vol_f[K] = VALUE; Q.push_back(EVec4i(x, y - 1, z, K)); }
		K = I - WH; if (z > 0   && vol_f[K] == 1 && dist[K] < max_dist && minv <= vol[K] && vol[K] <= maxv){ vol_f[K] = VALUE; Q.push_back(EVec4i(x, y, z - 1, K)); }
		K = I + 1 ; if (x < W-1 && vol_f[K] == 1 && dist[K] < max_dist && minv <= vol[K] && vol[K] <= maxv){ vol_f[K] = VALUE; Q.push_back(EVec4i(x + 1, y, z, K)); }
		K = I + W ; if (y < H-1 && vol_f[K] == 1 && dist[K] < max_dist && minv <= vol[K] && vol[K] <= maxv){ vol_f[K] = VALUE; Q.push_back(EVec4i(x, y + 1, z, K)); }
		K = I + WH; if (z < D-1 && vol_f[K] == 1 && dist[K] < max_dist && minv <= vol[K] && vol[K] <= maxv){ vol_f[K] = VALUE; Q.push_back(EVec4i(x, y, z + 1, K)); }
	}

	delete[] dist;
}


//
// class LRGSeed 
//

const static float diffR[4] = {1   ,0,0,0}, diffB[4] = {0.3f,0.3f,1   ,0}, diffG[4] = {0,  1 ,0,0};
const static float ambiR[4] = {0.5f,0,0,0}, ambiB[4] = {0.3f,0.3f,0.8f,0}, ambiG[4] = {0,0.8f,0,0};
const static float spec [4] = {1   ,1,1,0};
const static float shin [1] = {56.0f};


TMesh LRGSeed::m_cp_mesh = TMesh();
float LRGSeed::m_cp_radius = 1.0;
void LRGSeed::SetCpRadius(float radius)
{
  m_cp_radius = radius;
	m_cp_mesh.InitializeIcosaHedron( m_cp_radius );
}

float LRGSeed::GetCpRadius()
{
  return m_cp_radius;
}



void LRGSeed::Draw() const
{
	//draw seed 
	glPointSize(15);
	glEnable(GL_LIGHTING);

	for( const auto& p : m_cps)
	{
		glTranslated( p[0], p[1], p[2] );
		if(m_flg_fore) m_cp_mesh.Draw( diffR, ambiR, spec, shin);
		else           m_cp_mesh.Draw( diffB, ambiB, spec, shin);
		glTranslated( -p[0], -p[1],-p[2] );
	}
	glDisable(GL_LIGHTING);

}



void LRGSeed::DrawAsActive() const
{
	float r = (m_flg_fore == 1) ? 1.0f : 0;
	float b = (m_flg_fore == 1) ? 0 : 1.0f;

	float spec[4] = { 1,1,1,0.3f };
	float diff[4] = { r,0,b,0.3f };
	//float diff[4] = { r,0,b,0.5f };
	//float ambi[4] = { 0.3f*r,0,0.3f*b,0.3f };
	float ambi[4] = { 2.0f*r,0,2.0f*b,1.0f };
	float shin[1] = { 64.0f };


	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , ambi);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shin);

  glEnable(GL_CULL_FACE );
  glCullFace(GL_BACK);

	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);

	//glDepthMask(false);
	for (int i = 1; i < (int)m_cps.size(); ++i)
		DrawCylinder(m_cps[i-1], m_cps[i], m_radius);
	//glDepthMask(true);
  
	for (int i = 0; i < (int)m_cps.size(); ++i)
	  DrawSphere(m_cps[i], m_radius);


	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
}


#pragma managed
