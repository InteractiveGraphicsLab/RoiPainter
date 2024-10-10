#pragma unmanaged
#include "ModeSegGCut.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"
#include "tmesh.h"

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegGCut.h"
#include "climessagebox.h"

#pragma unmanaged
#include <time.h>
#include <process.h>
#include "../3rdparty/riken/TMaxFlow_BK4.h"
#include "../3rdParty/riken/TWatershedEx.h"


#pragma warning(disable : 4996)




#ifndef FOREBACK_MAX 
#define FOREBACK_MAX 1000000000.0
#endif 

using namespace RoiPainter3D;



static bool t_loadWsdLabel ( 
    const std::string fname,  
    const int    num_voxels, 
    int*         map_vox2wsd 
);

static void t_constructWsdNodesFromLabel
(
	const int W, const int H, const int D,
	const int   *vol_label,
	const short *vol      ,

  int        &num_wsdnodes,
	GCWsdNode* &wsdNodes  ,// この関数内でallocateされる
	std::set<int> * &wsdNodeNei // この関数内でallocateされる
);



ModeSegGCut::~ModeSegGCut()
{
  delete[] m_vol_wsdid;
  delete[] m_wsdnodes ;
  delete[] m_wsdnode_neibor;
}


ModeSegGCut::ModeSegGCut() :
	m_volume_shader("shader/volVtx.glsl"   , "shader/volFlg_Seg.glsl"   ),
	m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
  std::cout << "ModeSegGCutModeSegGCut constructor! \n";
  m_b_wsdnode_initialized = false;
  m_b_wsdnode_computing   = false;
  m_vol_wsdid = 0;
  m_num_wsdnodes   = 0;
  m_wsdnodes       = 0;
  m_wsdnode_neibor = 0;
  std::cout << "ModeSegGCutModeSegGCut constructor! DONE \n";
}


// flg 0: back
//     1: trgt & fore
//   255: trgt & back
bool ModeSegGCut::CanLeaveMode()
{
	if ( m_cps_fore.size() == 0 && m_cps_back.size() == 0 ) return true;
  
  if( CLI_MessageBox_YESNO_Show("Current Result is not stored. \nDo you want to leave?", "caution") )
	{
		return true;
	}

	return false;
}


void ModeSegGCut::StartMode()
{
  //initialize flg volume --------------
  ImageCore::GetInst()->InitializeVolFlgByLockedMask();

  //initialize control points ----------
	m_cps_fore.clear();
	m_cps_back.clear();
	m_cp_radius = (float) ImageCore::GetInst()->GetPitch()[0] * 3;
	m_cp_mesh.InitializeIcosaHedron( m_cp_radius );

  formSegGCut_Show();

	//compute watershad ------------------
	if( !m_b_wsdnode_initialized )
  {
    int W,H,D,WH,WHD;
    std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();

    if( m_vol_wsdid != 0 ) delete[] m_vol_wsdid;
    m_vol_wsdid = new int[WHD];

    //backup fileの読み込みを試す
		std::string backUpFilePath = ImageCore::GetInst()->GetFilePath() + ".RpWsdPre";
	  if( t_loadWsdLabel( backUpFilePath, WHD, m_vol_wsdid) )
	  {
		  const short *vol = ImageCore::GetInst()->m_vol_orig;
		  t_constructWsdNodesFromLabel( W,H,D, m_vol_wsdid, vol, m_num_wsdnodes, m_wsdnodes, m_wsdnode_neibor);
		  m_b_wsdnode_initialized = true;
		  return;
	  }

	  //threadを起動しwatershedを計算する
    if( m_b_wsdnode_computing )
    {
      CLI_MessageBox_OK_Show("現在 GraphCut用の前計算（watershed）中です．\n計算が終わるとrunGraphCutボタンが\n有効になります", "message");
    }
    else
    {
      m_b_wsdnode_computing = true;
      _beginthread( ModeSegGCut::InitializeWsdNodes_thread, 0, (LPVOID)this); 
    }
  }

  //Lock/Unlock pitch box
  formVisParam_LockPitchBox();
}




void ModeSegGCut::FinishSegmemntation ()
{
	if ( !ImageCore::GetInst()->bForeVoxelsExistInVolFlg () ) 
	{
		CLI_MessageBox_OK_Show("No foreground pixel exist\n", "caution");
		return;
	}

	//CPを削除（しないと、canEndModeでダイアログが表示される)
	m_cps_fore.clear();
	m_cps_back.clear();

	ImageCore::GetInst()->StoreForegroundAsNewMask();
	ModeCore ::GetInst()->ModeSwitch( MODE_VIS_MASK );
	RedrawScene();	
}



void ModeSegGCut::CancelSegmentation ()
{
  //CPを削除（しないと、canEndModeでダイアログが表示される)
	m_cps_fore.clear();
	m_cps_back.clear();
	ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
	RedrawScene();	
}



void ModeSegGCut::ClearAllCPs()
{
  m_cps_fore.clear();
	m_cps_back.clear();
	RedrawScene();	
}





////////////////////////////////////////////////////////////////////
//MOUSE LISTENER 

void ModeSegGCut::LBtnDown(const EVec2i &p, OglForCLI *ogl) 
{
  m_bL = true;
	m_stroke.clear();

  if ( IsShiftKeyOn())
  {
    m_b_paint_cps = true;
  }
	else if ( IsCtrKeyOn() )
  {
    m_b_draw_cutsrtoke = true;
  }
	else 
  {
    ogl->BtnDown_Trans(p);
  }
}



void ModeSegGCut::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{  
  if (m_b_draw_cutsrtoke) 
  {
    const EVec3f cuboid = ImageCore::GetInst()->GetCuboid();
    CrssecCore::GetInst()->GenerateCurvedCrssec( cuboid, ogl->GetCamPos(), m_stroke );
  }
  m_bL = false;
	m_b_paint_cps = false;
  m_b_draw_cutsrtoke = false;
	ogl->BtnUp();
  RedrawScene();
}


void ModeSegGCut::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  if( IsShiftKeyOn() ) 
  {
    m_b_paint_cps = true;
  }
  else
  {
    ogl->BtnDown_Rot(p);
  }
	m_bR = true;
}


void ModeSegGCut::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  m_b_paint_cps = false;
  ogl->BtnUp();
  RedrawScene();
}

void ModeSegGCut::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  if( IsShiftKeyOn() )
  {
    m_b_paint_cps = true;
  }
	else
  {
    ogl->BtnDown_Zoom(p);
  }
	m_bM = true;
}


void ModeSegGCut::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = m_b_paint_cps = false;
	ogl->BtnUp();
  RedrawScene();
}


void ModeSegGCut::LBtnDclk(const EVec2i &p, OglForCLI *ogl){}
void ModeSegGCut::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegGCut::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


void ModeSegGCut::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if ( !m_bL && !m_bR && !m_bM)  return;

	EVec3f ray_pos, ray_dir;
	ogl->GetCursorRay( p, ray_pos, ray_dir);

	if (m_b_draw_cutsrtoke)
	{
		m_stroke.push_back( ray_pos + 0.1f * ray_dir );
	}
	else if (m_b_paint_cps && m_bM)
	{
    //erase control points
		for (int i = 0; i < (int)m_cps_fore.size(); ++i)
		{
			if (DistRayAndPoint(ray_pos, ray_dir, m_cps_fore[i].m_pos) < m_cp_radius)
      {
        m_cps_fore.erase( m_cps_fore.begin() + i );
      }
		}
		for (int i = 0; i < (int)m_cps_back.size(); ++i)
		{
			if ( DistRayAndPoint(ray_pos, ray_dir, m_cps_back[i].m_pos) < m_cp_radius)
      {
        m_cps_back.erase( m_cps_back.begin() + i );
      }
		}
	}
	else if( m_b_paint_cps && (m_bL || m_bR) )
	{
    //paint control points
    EVec3f pos;
		if ( PickCrssec( ray_pos, ray_dir, &pos) != CRSSEC_NON )
		{
			std::vector<GCutCp> &cps =  m_bL ? m_cps_fore : m_cps_back;

			if( cps.empty() || ( pos - cps.back().m_pos).norm() > m_cp_radius * 3 )
			{
				EVec4i vi = ImageCore::GetInst()->GetVoxelIndex4i(pos);
				cps.push_back( GCutCp( pos, vi) );	
			}
		}
	}
	else
	{
		ogl->MouseMove(p);
	}

  RedrawScene(false);
}


void ModeSegGCut::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  if( !PickToMoveCrossSecByWheeling(p, ogl, z_delta ) )
  { 
		ogl->ZoomCamera(z_delta * 0.1f);
  }
  RedrawScene();
}



void ModeSegGCut::KeyDown(int nChar) 
{
  RedrawScene();
}
void ModeSegGCut::KeyUp(int nChar) 
{
  RedrawScene();
}


void ModeSegGCut::DrawScene (
  const EVec3f &cuboid, 
  const EVec3f &cam_pos,
  const EVec3f &cam_center )
{

	//bind volumes ---------------------------------------
	BindAllVolumes();

	//render cross sections ----------------------------------
  const EVec3i reso = ImageCore::GetInst()->GetResolution();
	DrawCrossSections(cuboid, reso, m_crssec_shader);

	//volume rendering ---------------------------------------
  const bool   b_draw_vol = formVisParam_bRendVol();

	if ( b_draw_vol && !IsSpaceKeyOn())
	{
    const bool  b_psu     = formVisParam_bDoPsued();
    const bool  b_roi     = formVisParam_GetOtherROI();
    const float alpha     = formVisParam_getAlpha();
    const bool  b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int   num_slice = (int)((b_onmanip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		m_volume_shader.Bind(0, 1, 2, 3, 4, 5, 6, alpha * 0.1f, reso, cam_pos, b_psu, b_roi);
		t_DrawCuboidSlices(num_slice, cam_pos, cam_center, cuboid);
		m_volume_shader.Unbind();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

  //draw control points
	glDisable(GL_LIGHTING);
  glColor3d(1,0,0);
	for (const auto& it: m_cps_fore )
	{
		glTranslated( it.m_pos[0], it.m_pos[1], it.m_pos[2]);
		m_cp_mesh.Draw();
		glTranslated(-it.m_pos[0],-it.m_pos[1],-it.m_pos[2]);
	}

  glColor3d(0,0,1);
	for (const auto& it: m_cps_back )
	{
		glTranslated( it.m_pos[0], it.m_pos[1], it.m_pos[2]);
		m_cp_mesh.Draw();
		glTranslated(-it.m_pos[0],-it.m_pos[1],-it.m_pos[2]);
	}
	glDisable(GL_LIGHTING);


  //draw cut stroke 
  if (m_b_draw_cutsrtoke) DrawPolyLine(EVec3f(1,1,0), 3, m_stroke);

}





//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//Watershed segmentation//////////////////////////////////////////////////////////////

//try to load backup wsd segmentation file (一度計算したらbackupをしておいて再利用する)
// fname = volume_file_name.RpWsdPre

static bool t_loadWsdLabel 
(
		const std::string fname,
		const int num_voxels,
		int* map_vox2wsd
)
{
	if( fname.length() <= 9 ) return false;

	FILE *fp = fopen( fname.c_str(), "rb" );
	if( fp == 0 ) return false;

	//check volume size	
	int num_voxel_fromfile; 
	fread( &num_voxel_fromfile, sizeof(int),1, fp );
	if( num_voxel_fromfile != num_voxels ) 
	{ 
		fclose( fp); 
		return false;
	}

	//check user's intension
	std::cout << "fname:" << fname.c_str() << "\n\n";
	if(!CLI_MessageBox_YESNO_Show( "watershedの前計算ファイルが利用できます。利用しますか？", "Use precompute file?") )
	{
		fclose( fp); 
		return false;
	}

	//read label 
	fread( map_vox2wsd, sizeof(int), num_voxels, fp );
	fclose( fp );

	return true;
}


static bool t_saveWsdLabel
( 
    const std::string fname,
    const int    num_voxels,
    const int   *map_vox2wsd 
)
{
	FILE *fp = fopen( fname.c_str(), "wb" );
	if( fp == 0 ) return false;

	fwrite( &num_voxels, sizeof(int), 1, fp );
  fwrite( map_vox2wsd, sizeof(int), num_voxels, fp );
	fclose( fp );
	return true;
}


void t_constructWsdNodesFromLabel
(
	const int W,
	const int H, 
	const int D,
	const int   *vol_label,
	const short *vol      ,
  
  int         &num_wsdnodes, //この関数内で計算される
	GCWsdNode*  &wsdNodes    , //この関数内でallocateされる
	std::set <int>*  &wsdNodeNei    // この関数内でallocateされる
)
{
	time_t t0 = clock();
	std::cout << "t_constructWsdNodesFromLabel start......";

	const int WH = W*H;
	const int WHD =W*H*D;

	int max_label = 0;
	for( int i= 0; i < WHD; ++i ) max_label = std::max( max_label, vol_label[i] );

  if( wsdNodes   != 0 ) delete[] wsdNodes;
  if( wsdNodeNei != 0 ) delete[] wsdNodeNei;
  num_wsdnodes = max_label + 1;
  wsdNodes   = new GCWsdNode[ num_wsdnodes ];
	wsdNodeNei = new std::set<int> [ num_wsdnodes ];

	for( int z = 0; z < D ; ++z)
  {
	  for( int y = 0; y < H ; ++y)
    {
	    for( int x = 0; x < W ; ++x)
	    {
		    const int I = x + y*W + z*WH;
		    wsdNodes[ vol_label[I] ].addVoxel( I, (float)vol[I] );

		    if( x < W-1 && vol_label[I] != vol_label[ I+1 ]) 
		    {
          int labelsmall = std::min(vol_label[I], vol_label[I+1] );
          int labellarge = std::max(vol_label[I], vol_label[I+1] );
          wsdNodeNei[ labelsmall ].insert( labellarge );
		    }
		    if( y < H-1 && vol_label[I] != vol_label[ I+W ] )
		    {
          int labelsmall = std::min(vol_label[I], vol_label[I+W] );
          int labellarge = std::max(vol_label[I], vol_label[I+W] );
          wsdNodeNei[ labelsmall ].insert( labellarge );		
        }
		    if( z < D-1 && vol_label[I] != vol_label[ I+WH ] )
		    {
          int labelsmall = std::min(vol_label[I], vol_label[I+WH] );
          int labellarge = std::max(vol_label[I], vol_label[I+WH] );
          wsdNodeNei[ labelsmall ].insert( labellarge );
        }
	    }
    }
  }

	time_t t1 = clock();
	std::cout << "t_constructWsdNodesFromLabel : " << W << " " << H << " " << D 
            << " time " << (t1-t0)/(double)CLOCKS_PER_SEC << "\n";

}




//threadとして起動される
void ModeSegGCut::InitializeWsdNodes_thread(void *pParam)
{
	std::cout << "initWsdNodes_thread start!!\n";
  time_t t0 = clock();

	ModeSegGCut *p_modeseggcut  = (ModeSegGCut*)pParam;
  
  int W,H,D,WH,WHD;
  std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();
	const short  *vol      = ImageCore::GetInst()->m_vol_orig   ;
	const float  *vol_gm   = ImageCore::GetInst()->m_vol_origgm ;
	
	//calc WsdLabel 0:境界画素, >0:ラベル値
	int *vox_wsdid = p_modeseggcut->m_vol_wsdid;

  time_t t1 = clock();
	t_wsd_CalcLabelFromGMag( W, H, D, vol_gm, 1000, vox_wsdid); 
  time_t t2 = clock();
	t_wsd_RemoveOneVoxWsdLabel( WHD, vox_wsdid ); 
  time_t t3 = clock();
	t_wsd_CollapseWsdPixels3D( W, H, D, vol, vox_wsdid ); 
  time_t t4 = clock();

	for( int i= 0; i < WHD; ++i ) vox_wsdid[i] -= 1; 

	//calc Node & neighbors
	t_constructWsdNodesFromLabel( W, H, D, vox_wsdid, vol, p_modeseggcut->m_num_wsdnodes, p_modeseggcut->m_wsdnodes, p_modeseggcut->m_wsdnode_neibor );

	//save backup file
	std::string backup_fpath = ImageCore::GetInst()->GetFilePath() + ".RpWsdPre";
	t_saveWsdLabel( backup_fpath, WHD, vox_wsdid);

	p_modeseggcut->m_b_wsdnode_initialized = true;
	p_modeseggcut->m_b_wsdnode_computing   = false;
	
  time_t t5 = clock();
	std::cout << "TIME: " << (t1-t0) / (double)CLOCKS_PER_SEC << " " 
                        << (t2-t1) / (double)CLOCKS_PER_SEC << " " 
                        << (t3-t2) / (double)CLOCKS_PER_SEC << " " 
                        << (t4-t3) / (double)CLOCKS_PER_SEC << " "
                        << (t5-t4) / (double)CLOCKS_PER_SEC << "\n" ;
	std::cout << "initWsdNodes_thread DONE!!\n";
}

//original 
//TIME: 0.001 228.363 1.58 4.394 16.986
//TIME: 0.001 227.302 1.577 4.305 17.171

inline static EVec4i t_voxIdx(const int &W, const int &WH, const int &vI)
{
	EVec4i v;
	v[3] = vI;
	v[2] =  vI/WH;
	v[1] = (vI - v[2]*WH) / W;
	v[0] = (vI - v[2]*WH - v[1]*W);
	return v;
}



//
//slipt one wsd region that contains multiple fore/back seeds
//
void t_wsd_DivideOneLabel( 
	const int W, 
	const int H,
	const int D, 
	const int maxLabel,
	const int fI, 
	const int bI,
  int* vLabel)
{
	std::cout << "divide conflict wsd area " << fI << " " << bI 
            << "(" << vLabel[ fI ] << " " << vLabel[ bI ] << ")\n";

	if( vLabel[ fI ] != vLabel[ bI ] ) return;

	const int WH      = W*H;
	const int TRGT    = vLabel[ fI ];
	const int F_LABEL = maxLabel + 2;
	const int B_LABEL = maxLabel + 1;

	TQueue<EVec4i> fQ, bQ; 
	fQ.push_back( t_voxIdx(W,WH,fI) ); 
	bQ.push_back( t_voxIdx(W,WH,bI) ); 

	//growth
	while( !fQ.empty() || !bQ.empty() )
	{
		if( !bQ.empty() )
		{
			const int x = bQ.front()[0]; 
			const int y = bQ.front()[1]; 
			const int z = bQ.front()[2]; 
			const int I = bQ.front()[3]; 
			bQ.pop_front( );
			if( x>0  && vLabel[I-1 ]==TRGT ){ bQ.push_back( EVec4i(x-1,y  ,z  ,I- 1) ); vLabel[I-1 ] = B_LABEL; }
			if( x<W-1&& vLabel[I+1 ]==TRGT ){ bQ.push_back( EVec4i(x+1,y  ,z  ,I+ 1) ); vLabel[I+1 ] = B_LABEL; }
			if( y>0  && vLabel[I-W ]==TRGT ){ bQ.push_back( EVec4i(x  ,y-1,z  ,I- W) ); vLabel[I-W ] = B_LABEL; }
			if( y<H-1&& vLabel[I+W ]==TRGT ){ bQ.push_back( EVec4i(x  ,y+1,z  ,I+ W) ); vLabel[I+W ] = B_LABEL; }
			if( z>0  && vLabel[I-WH]==TRGT ){ bQ.push_back( EVec4i(x  ,y  ,z-1,I-WH) ); vLabel[I-WH] = B_LABEL; }
			if( z<D-1&& vLabel[I+WH]==TRGT ){ bQ.push_back( EVec4i(x  ,y  ,z+1,I+WH) ); vLabel[I+WH] = B_LABEL; }
		}
		if( !fQ.empty() )
		{
			const int x = fQ.front()[0]; 
			const int y = fQ.front()[1]; 
			const int z = fQ.front()[2]; 
			const int I = fQ.front()[3]; 
			fQ.pop_front( );
			if( x>0  && vLabel[I-1 ]==TRGT ){ fQ.push_back( EVec4i(x-1,y,z,I- 1) ); vLabel[I-1 ] = F_LABEL; }
			if( x<W-1&& vLabel[I+1 ]==TRGT ){ fQ.push_back( EVec4i(x+1,y,z,I+ 1) ); vLabel[I+1 ] = F_LABEL; }
			if( y>0  && vLabel[I-W ]==TRGT ){ fQ.push_back( EVec4i(x,y-1,z,I- W) ); vLabel[I-W ] = F_LABEL; }
			if( y<H-1&& vLabel[I+W ]==TRGT ){ fQ.push_back( EVec4i(x,y+1,z,I+ W) ); vLabel[I+W ] = F_LABEL; }
			if( z>0  && vLabel[I-WH]==TRGT ){ fQ.push_back( EVec4i(x,y,z-1,I-WH) ); vLabel[I-WH] = F_LABEL; }
			if( z<D-1&& vLabel[I+WH]==TRGT ){ fQ.push_back( EVec4i(x,y,z+1,I+WH) ); vLabel[I+WH] = F_LABEL; }
		}
	}
	//fore label の値を元に戻す
  for ( int i=0, s = W*H*D; i < s; ++i)
  {
    if( vLabel[i] == F_LABEL ) vLabel[i] = TRGT;
  }

	std::cout << "done!!\n";

}



//divide the wsd labeled region 
//when the bothe pos and neg CPs exist in the region
bool t_wsd_CheckAndSolveConflictCP( 
  const int W, const int H, const int D,
	const std::vector<GCutCp> &cps_fore ,
	const std::vector<GCutCp> &cps_back ,

	int* vol_label)
{
	std::set<int> fore_ids, back_ids;
	for(const auto& cp : cps_fore ) fore_ids.insert( cp.m_vidx[3] );
	for(const auto& cp : cps_back ) back_ids.insert( cp.m_vidx[3] );

  const int num_voxel = W*H*D;

	int max_label = 0;
  for( int i=0; i < num_voxel; ++i) 
  {
    max_label = std::max( max_label, vol_label[i]);
  }

	bool modified = false;

	for ( const auto &f : fore_ids)
	{
		for ( const auto &b : back_ids)
		{
			if (vol_label[f] == vol_label[b])
			{
				t_wsd_DivideOneLabel( W,H,D, max_label, f, b, vol_label);

				modified = true;
				max_label++;
			}
		}
	}

	return modified;

}










////////////////////////////////////////////////////////////////////////////////////////
//Graph cut segmentation////////////////////////////////////////////////////////////////

static inline float t_DistSq(const float &f1, const float &f2)
{
	return (f1-f2)*(f1-f2);
}

//use data energy introduced by lazy snapping paper
//see Lazy snapping papaer for detail
inline static void t_CalcE1(
	const GCWsdNode   *nodes,
	const std::vector<int> &forenode_ids,
	const std::vector<int> &backnode_ids,
	const int         &pivnode_id,
	EVec2f &e
)
{
	float df = FLT_MAX;
	float db = FLT_MAX;
	float piv_v = nodes[ pivnode_id ].m_mean_val;
	for( const auto& it: forenode_ids)  df = std::min(df, t_DistSq( nodes[it].m_mean_val, piv_v) );
	for( const auto& it: backnode_ids)  db = std::min(db, t_DistSq( nodes[it].m_mean_val, piv_v) );

	df = sqrt( df );
	db = sqrt( db );

	if( df == 0 && db == 0 ) df = db =  0.5f;

	e << db / ( df + db ), df / ( df + db ); 
}

//use smoothness energy introduced in lazy snapping
inline static float t_CalcE2( const float &v1, const float &v2, const float &lambda)
{	    
	return  lambda / ( 1.0f + (v1-v2)*(v1-v2) ); 
}

//重複を削除 (sort --> unique --> erase)
static void t_VectorUnique(std::vector<int> &vectInt)
{
	sort( vectInt.begin(), vectInt.end() ); 
	auto fvIt = unique( vectInt.begin(), vectInt.end() ); 
	vectInt.erase( fvIt, vectInt.end() ); 
}


//
//only extract region connect to foreground seeds 
//vFlg = 0:not target, 1:back, 255:fore
//
void t_RemoveIsolatedForeRegion( 
	const int &W, 
	const int &H, 
	const int &D, 
	const std::vector<GCutCp> &cps_fore,

	byte* vol_flg)
{	
	const int WH = W*H, WHD = W*H*D;

	//新たな foregroundを254として領域拡張する
	TQueue<EVec4i> Q;
	for (const auto &cp : cps_fore)
	{
		Q.push_back( cp.m_vidx );
		vol_flg[ cp.m_vidx[3] ] = 254;
	}

	while( !Q.empty() )
	{
		const int x = Q.front()[0];
		const int y = Q.front()[1];
		const int z = Q.front()[2];
		const int I = Q.front()[3];
		Q.pop_front();

		if( x != 0   && vol_flg[ I - 1  ] == 255 ) { vol_flg[ I - 1 ] = 254; Q.push_back( EVec4i(x-1,y  ,z  ,I-1 ) );}
		if( y != 0   && vol_flg[ I - W  ] == 255 ) { vol_flg[ I - W ] = 254; Q.push_back( EVec4i(x  ,y-1,z  ,I-W ) );}
		if( z != 0   && vol_flg[ I - WH ] == 255 ) { vol_flg[ I - WH] = 254; Q.push_back( EVec4i(x  ,y  ,z-1,I-WH) );}
		if( x != W-1 && vol_flg[ I + 1  ] == 255 ) { vol_flg[ I + 1 ] = 254; Q.push_back( EVec4i(x+1,y  ,z  ,I+1 ) );}
		if( y != H-1 && vol_flg[ I + W  ] == 255 ) { vol_flg[ I + W ] = 254; Q.push_back( EVec4i(x  ,y+1,z  ,I+W ) );}
		if( z != D-1 && vol_flg[ I + WH ] == 255 ) { vol_flg[ I + WH] = 254; Q.push_back( EVec4i(x  ,y  ,z+1,I+WH) );}
	}

#pragma omp parallel for
	for( int idx = 0; idx < WHD; ++idx) 
	{
		if(      vol_flg[ idx ] == 254 ) vol_flg[ idx ] = 255;
		else if( vol_flg[ idx ] == 255 ) vol_flg[ idx ] = 1  ;	
	}
		
}




void ModeSegGCut::RunGraphCutWsdLv(float lambda)
{
	if (m_b_wsdnode_computing || !m_b_wsdnode_initialized)
	{
		CLI_MessageBox_OK_Show("現在前計算中です、少々お待ちください", "Message from Graph cut tool");
		return;
	}

	time_t t1 = clock();
	std::cout << "graphCut 1....\n";


  int W,H,D,WH,WHD;
  std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();
  const short  *vol      = ImageCore::GetInst()->m_vol_orig;
  byte *vol_flg          = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();


	// ひとつのノードに二つ以上の前景・背景制御点が配置されていたら、分割する
	if ( t_wsd_CheckAndSolveConflictCP(W,H,D, m_cps_fore, m_cps_back, m_vol_wsdid) )
	{
		t_constructWsdNodesFromLabel( W,H,D, m_vol_wsdid, vol, m_num_wsdnodes, m_wsdnodes, m_wsdnode_neibor );
	}

	// disable wsd nodes that exist in locked mask region
	for( int i=0; i < m_num_wsdnodes; ++i)
	{
    m_wsdnodes[i].m_b_enable = false;
  }
  for( int i=0; i < WHD; ++i)
  {
    if( vol_flg[i] != 0) m_wsdnodes[ m_vol_wsdid[i] ].m_b_enable = true;
  }

	// cps --> node ids
	std::vector< int > fore_wsdnodeids, back_wsdnodeids;
	for (const auto& c : m_cps_fore) fore_wsdnodeids.push_back( m_vol_wsdid[ c.m_vidx[3] ] );
	for (const auto& c : m_cps_back) back_wsdnodeids.push_back( m_vol_wsdid[ c.m_vidx[3] ] );
	t_VectorUnique( fore_wsdnodeids );
	t_VectorUnique( back_wsdnodeids );


	//generate flow network
	int estimate_numedge = 0 ; 
  for ( int i=0; i < m_num_wsdnodes; ++i)
    estimate_numedge += (int) m_wsdnode_neibor[i].size();

	const int num_edge  = 2 * (m_num_wsdnodes + estimate_numedge)  ; //2 * tLink + nLinks
	const int soucenode_id = m_num_wsdnodes + 0 ;
	const int sinknode_id  = m_num_wsdnodes + 1 ;

	TFlowNetwork_BK4 network( m_num_wsdnodes + 2, num_edge, soucenode_id, sinknode_id ); 


	time_t t2 = clock();
  std::cout << "graphCut " << m_num_wsdnodes << " " << num_edge 
            << " " << fore_wsdnodeids.size() << " " << back_wsdnodeids.size() << "\n";

	//t-link
	std::vector<EVec2f> tLinkE( m_num_wsdnodes );

#pragma omp parallel for
	for( int i = 0; i < m_num_wsdnodes; ++i)
	{
		tLinkE[i] << 0, FOREBACK_MAX; 
		if( m_wsdnodes[i].m_b_enable ) t_CalcE1( m_wsdnodes, fore_wsdnodeids, back_wsdnodeids, i, tLinkE[i] ); 
	}

	for( const auto &id : fore_wsdnodeids ) if( m_wsdnodes[ id ].m_b_enable ) tLinkE[id] << FOREBACK_MAX, 0 ;  
	for( const auto &id : back_wsdnodeids ) if( m_wsdnodes[ id ].m_b_enable ) tLinkE[id] << 0, FOREBACK_MAX ;  

	for( int i = 0; i < m_num_wsdnodes; ++i ) 
  {
    network.add_tLink( soucenode_id, sinknode_id, i, tLinkE[i][0], tLinkE[i][1]);
  }


	//n-link b_enableなノード間のみ
	for (int i = 0; i < m_num_wsdnodes; ++i) if( m_wsdnodes[i].m_b_enable ) 
	{
		for( const auto &neiI : m_wsdnode_neibor[i] )
		{
			if( i < neiI && m_wsdnodes[ neiI ].m_b_enable )
			{
				network.add_nLink( i, neiI, t_CalcE2( m_wsdnodes[i].m_mean_val, m_wsdnodes[neiI].m_mean_val, lambda) );
			}
		}
	}

	time_t t3 = clock();
	std::cout << "graphCut 2....\n";

	//Graph Cut
	byte *minCut = new byte[ m_num_wsdnodes + 2 ];
	network.calcMaxFlow( soucenode_id, sinknode_id, minCut );


#pragma omp parallel for
	for(int i = 0; i < WHD; ++i ) if( vol_flg[i] != 0 ) 
  {
    vol_flg[i] = (minCut[ m_vol_wsdid[i] ]) ? 255 : 1;
  }

	t_RemoveIsolatedForeRegion( W,H,D, m_cps_fore, vol_flg );

	ImageCore::GetInst()->m_vol_flag.SetUpdated();

	time_t t4 = clock();
	std::cout << "graphCut 3.... "
			<< (t2-t1)/(double)CLOCKS_PER_SEC << " "
			<< (t3-t2)/(double)CLOCKS_PER_SEC << " "
			<< (t4-t3)/(double)CLOCKS_PER_SEC << "\n";

	delete[] minCut ;

  RedrawScene();
}




//float lambda,        係数ラムダ
//int bandWidth,　　   計算領域のwidth
//bool genBundOnlyBack 計算領域を背景ボクセルのみから生成する
void ModeSegGCut::RunGraphCutVoxLv(float lambda, int band_width, bool b_genband_onlybackground)
{
	if (m_b_wsdnode_computing || !m_b_wsdnode_initialized)
	{
		CLI_MessageBox_OK_Show("現在前計算中です、少々お待ちください", "Message from Graph cut tool");
		return;
	}
    
  int W,H,D,WH,WHD;
  std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();
  const int    SEAM_W  = band_width;
	const short  *vol    = ImageCore::GetInst()->m_vol_orig;
	byte  *vol_flg       = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();


	// 前景-背景 境界帯状領域の生成

	// v_mapNodeId --> -1:fore, -2:back, 0以上: nodeIdx
	int  *vol_nodeid = new int[ WHD ];  
	
	for( int i=0; i < WHD; ++i) vol_nodeid[i] = (vol_flg[i] == 255) ? -1 : -2;
	
	for( int z = 1; z < D -1; ++z)
  {
	  for( int y = 1; y < H -1; ++y)
    {
	    for( int x = 1; x < W -1; ++x)
	    {
		    const int I = x + y*W + z*WH;
		    if( vol_flg[ I ] != 255 ) continue;

		    if( vol_flg[I-1] <= 1 || vol_flg[I-W] <= 1 || vol_flg[I-WH] <= 1 || 
			      vol_flg[I+1] <= 1 || vol_flg[I+W] <= 1 || vol_flg[I+WH] <= 1 )
		    {
			    for( int zz = -SEAM_W; zz <= SEAM_W; ++zz) if( 0 <= z+zz && z+zz < D )
			    for( int yy = -SEAM_W; yy <= SEAM_W; ++yy) if( 0 <= y+yy && y+yy < H )
			    for( int xx = -SEAM_W; xx <= SEAM_W; ++xx) if( 0 <= x+xx && x+xx < W )
			    {
				    const int pivI = I + xx + yy*W + zz * WH;
				    if (b_genband_onlybackground)
				    {
					    if( vol_flg[ pivI ] != 255 ) vol_nodeid[ pivI ] = 0;
				    }
				    else
				    {
					    vol_nodeid[ pivI ] = 0;
				    }
			    }
		    }
      }
    }
	}

	//gen flow network
	std::vector<GCVoxNode> vox_nodes;
	vox_nodes.reserve(WHD);
	for ( int i = 0; i < WHD; ++i) if( vol_nodeid[i] == 0 )
	{
		vol_nodeid[i] = (int) vox_nodes.size();
		vox_nodes.push_back( GCVoxNode(i,vol[i]) );
	}

	const int num_node  = (int) vox_nodes.size();
	const int num_edge  = 2 * num_node + 6 * num_node ;
	const int sourcenode_id = num_node + 0;
	const int sinknode_id   = num_node + 1;
	TFlowNetwork_BK4 network( num_node + 2, num_edge, sourcenode_id, sinknode_id );

	for( int ni = 0; ni < num_node; ++ni)
	{	
		const EVec4i vI = t_voxIdx(W, WH, vox_nodes[ni].m_idx);
		const int     I = vI[3];

		int state = 0; // -2 : 背景隣接   -1 :前景隣接   >=0:その他
		if( vI[0] != 0  && vol_nodeid[ I-1 ] < 0 ) state = vol_nodeid[ I-1  ]; // = -1 or -2
		if( vI[1] != 0  && vol_nodeid[ I-W ] < 0 ) state = vol_nodeid[ I-W  ]; 
		if( vI[2] != 0  && vol_nodeid[ I-WH] < 0 ) state = vol_nodeid[ I-WH ]; 
		if( vI[0] !=W-1 && vol_nodeid[ I+1 ] < 0 ) state = vol_nodeid[ I+1  ]; 
		if( vI[1] !=H-1 && vol_nodeid[ I+W ] < 0 ) state = vol_nodeid[ I+W  ]; 
		if( vI[2] !=D-1 && vol_nodeid[ I+WH] < 0 ) state = vol_nodeid[ I+WH ]; 

		//tlink
		double e1_f = (state == -1) ? FOREBACK_MAX : (state ==-2 ) ? 0            : 1;
		double e1_b = (state == -1) ?     0        : (state ==-2 ) ? FOREBACK_MAX : 1;
		network.add_tLink( sourcenode_id, sinknode_id,  ni, e1_f, e1_b );

		//nlink
		if( vI[0] != W-1 && vol_nodeid[I+1 ] >= 0 ) { network.add_nLink( ni, vol_nodeid[I+1 ], t_CalcE2( vox_nodes[ni].m_val, vox_nodes[vol_nodeid[I+1 ]].m_val, lambda) ); }
		if( vI[1] != H-1 && vol_nodeid[I+W ] >= 0 ) { network.add_nLink( ni, vol_nodeid[I+W ], t_CalcE2( vox_nodes[ni].m_val, vox_nodes[vol_nodeid[I+W ]].m_val, lambda) ); }
		if( vI[2] != D-1 && vol_nodeid[I+WH] >= 0 ) { network.add_nLink( ni, vol_nodeid[I+WH], t_CalcE2( vox_nodes[ni].m_val, vox_nodes[vol_nodeid[I+WH]].m_val, lambda) ); }
	}

	byte *mincut = new byte[ num_node + 2 ];
	network.calcMaxFlow( sourcenode_id, sinknode_id, mincut );

#pragma omp parallel for
	for( int i = 0; i < WHD; ++i) if( vol_flg[i] != 0 )
	{
		vol_flg[i] = ( vol_nodeid[i] == -1    ) ?  255 :
			           ( vol_nodeid[i] == -2    ) ?  1   : 
				         ( mincut[ vol_nodeid[i] ]) ?  255 : 1;
	}

	delete[] mincut;
	delete[] vol_nodeid;
	ImageCore::GetInst()->m_vol_flag.SetUpdated();
  RedrawScene();
}


void ModeSegGCut::NewVolLoaded()
{
  //watershed 計算途中に新しい volume読み込みが行なわれる場合がある
  //threadにメッセージを投げるべき
  //現在は，messege boxによりユーザに警告を送るだけ
  if ( m_b_wsdnode_computing )
  {
    CLI_MessageBox_OK_Show( "graph cut modeが watershedを\n計算している最中に新しいvolumeが読み込まれました．\n このソフトウエアを再起動することをお勧め得します．", "caution");
  }
  m_b_wsdnode_initialized = false; 
}



#pragma managed




