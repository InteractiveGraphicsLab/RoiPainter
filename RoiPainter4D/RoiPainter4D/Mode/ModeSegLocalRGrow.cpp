#pragma unmanaged
#include "ModeSegLocalRGrow.h"
#include "ImageCore.h"
#include "ModeCore.h"
#include "ModeCommonTools.h"

#include "tqueue.h"
#include <fstream>
#include <time.h>
#include <iostream>

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegLocalRGrow.h"
#include "CliMessageBox.h"
#pragma unmanaged


using namespace RoiPainter4D;


ModeSegLocalRGrow::ModeSegLocalRGrow() :
    m_volume_shader("shader/volVtx.glsl", "shader/volFlg_Seg.glsl"),
    m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
  std::cout << "ModeSegThreshPnt...\n";
  m_bL = m_bR = m_bM = false;

  m_active_seed_id = -1;
  m_is_modified          = false;
  m_is_drag_activeseed   = false;
  m_is_resize_activeseed = false;

  std::cout << "ModeSegThreshPnt DONE\n";
}


ModeSegLocalRGrow::~ModeSegLocalRGrow()
{
}


void ModeSegLocalRGrow::FinishSegmentation()
{
  std::vector<byte*> &flg4d = ImageCore::GetInst()->m_flg4d;
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int num_voxels = ImageCore::GetInst()->GetNumVoxels();

  bool bForeExist = false;

  for (int fi = 0; fi < num_frames && !bForeExist; ++fi)
  {
    byte* flg3d = flg4d[fi];
    for (int i = 0; i < num_voxels && !bForeExist; ++i)
    {
      if ( flg3d[i] == 255) bForeExist = true;
    }
  }

  if (!bForeExist)
  {
    ShowMsgDlg_OK(MESSAGE_NO_FOREGROUND, "no foreground");
    return;
  }

  //このタイミングでfalseにしないとcanEndModeでダイアログが表示される
  m_is_modified = false;

  //SaveSeedInfo("");
  ImageCore::GetInst()->mask_storeCurrentForeGround();
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);

  m_seeds.clear();
  m_active_seed_id = -1;
  formMain_RedrawMainPanel();

}


bool ModeSegLocalRGrow::CanEndMode()
{
  if ( !m_is_modified ) return true;
  return (ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Leaving?"));
}


void ModeSegLocalRGrow::CancelSegmentation()
{
  if (ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Cancel?"))
  {
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    formMain_RedrawMainPanel();
  }
}



void ModeSegLocalRGrow::SaveSeedInfo(std::string fname)
{
  const int num_frames    = ImageCore::GetInst()->GetNumFrames();
  bool rad_fixallframe    = FormSegLocalRGrow_DoSetSameRadForAllFrame();
  bool pos_fixallframe    = FormSegLocalRGrow_DoSetSamePosForAllFrame();
  bool thresh_fixallframe = FormSegLocalRGrow_DoSetSameThreshForAllFrame();

  if( fname.length() == 0 )
    fname= "./segmemo" + std::to_string( time(NULL) ) + ".txt";
	

  std::ofstream fs(fname);

	fs << "num_seeds  " << m_seeds.size() << "\n";
	fs << "num_frames " << num_frames     << "\n";
  fs << "mode_fixrad    " << (rad_fixallframe   ?1:0) <<"\n"; 
  fs << "mode_fixpos    " << (pos_fixallframe   ?1:0)<<"\n"; 
  fs << "mode_fixthresh " << (thresh_fixallframe?1:0) <<"\n"; 


  for ( int i = 0; i < (int)m_seeds.size(); ++i)
  {
		fs << "#seed " << i << "\n";
		for (int f = 0; f < num_frames; ++f)
		{
      float  radius = m_seeds[i].GetRadius   (f);
			EVec2i thresh = m_seeds[i].GetThreshold(f);
			EVec3f pos    = m_seeds[i].GetPosition (f);
      bool   bedit  = m_seeds[i].GetIsEdit   (f);
			fs << radius      << " ";
			fs << thresh[0]   << " " << thresh[1] << " ";
			fs << pos[0]      << " " << pos[1]    << " " << pos[2] << " ";
      fs << (bedit?1:0) << "\n";
		}
	}
  fs.close();
}



void ModeSegLocalRGrow::LoadSeedInfo(std::string fname)
{
  const EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();
	
  std::ifstream fs(fname);

	if ( !fs.is_open() )
	{
    std::cout << "fopen failed \n";
		return;
	}

  std::string str;
  int num_frames, num_seeds, fixrad, fixpos, fixthresh;
  fs >> str >> num_seeds ;    std::cout << str << " " << num_seeds  << "\n";
  fs >> str >> num_frames;    std::cout << str << " " << num_frames << "\n";
  fs >> str >> fixrad; 
  fs >> str >> fixpos; 
  fs >> str >> fixthresh; 
  

  if ( num_frames != ImageCore::GetInst()->GetNumFrames() )
  {
    fs.close();
    return;
  }  
  
  m_seeds.clear();
  for ( int s = 0; s < num_seeds; ++s )
  {
    int idx;
    fs >> str >> idx ;
    std::vector<float > radius  (num_frames);
    std::vector<EVec3f> position(num_frames);
    std::vector<EVec2i> thresh  (num_frames);
    std::vector<bool  > bedit   (num_frames);
      
    for ( int f = 0; f < num_frames; ++f)
    {
      fs >> radius[f];
      fs >> thresh[f][0]   >> thresh[f][1];
      fs >> position[f][0] >> position[f][1] >> position[f][2];
      int flg_01;
      fs >> flg_01;
      bedit[f] = flg_01?true:false;
    }

    m_seeds.push_back( LocalSeed( num_frames, radius, position, thresh, bedit  ));
  }

  m_active_seed_id = (int)m_seeds.size() - 1;
  ForlSegLocalRGrow_SetChecboxState( fixrad?true:false, fixpos?true:false, fixthresh?true:false);
	FormSegLocalRGrow_UpdateSeedSelection();
	formMain_RedrawMainPanel();
}



// ---------------- Mouse Listener ---------------- //
// Shift + LBtnDrag   --> move/resize seed
// Shift + L DblClick --> place new seed
void ModeSegLocalRGrow::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;

  if ( !IsShiftKeyOn() )
  {
    ogl->BtnDown_Trans(p);
    return;
  }

  //shift key on
  EVec3f ray_pos, ray_dir;
  ogl->GetCursorRay(p, ray_pos, ray_dir);
  m_active_seed_id = PickSeeds(ray_pos, ray_dir);
  FormSegLocalRGrow_UpdateSeedSelection();

  if (m_active_seed_id != -1)
  {
    int frame_idx = formVisParam_getframeI();
    const EVec3f seed_pos = m_seeds[ m_active_seed_id ].GetPosition(frame_idx);
    const float  seed_rad = m_seeds[ m_active_seed_id ].GetRadius  (frame_idx); 

    if ( DistRayAndPoint(ray_pos, ray_dir, seed_pos) > 0.8 * seed_rad )
    {
      formMain_setCursorNESW();
      m_is_resize_activeseed = true;
    }
    else{
      m_is_drag_activeseed = true;
    }
  }
}


void ModeSegLocalRGrow::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
  ogl->BtnDown_Rot(p);
}


void ModeSegLocalRGrow::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}


void ModeSegLocalRGrow::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if ( m_is_resize_activeseed || m_is_drag_activeseed)
  {
    SelectSeed_RunInterpolation();
    FormSegLocalRGrow_UpdateSeedSelection();
  }

  m_is_resize_activeseed = false;
  m_is_drag_activeseed = false;
  m_bL = false;
  ogl->BtnUp();
  formMain_setCursorDefault();
  formMain_RedrawMainPanel();
}


void ModeSegLocalRGrow::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}

void ModeSegLocalRGrow::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeSegLocalRGrow::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{
  if (!IsShiftKeyOn()) return;

  EVec3f rayP, rayD, pos;
  ogl->GetCursorRay(p, rayP, rayD);

  int pickSeedI = PickSeeds(rayP, rayD);

  //シードをダブルクリックしたらそのシードを消す
  if (pickSeedI != -1)
  {
    std::cout << "delete local seed id = " << pickSeedI << "\n";
    m_seeds.erase(m_seeds.begin() + pickSeedI);
    if (m_seeds.size() == 0 || pickSeedI == m_active_seed_id)m_active_seed_id = -1;
  }
  else if ( PickCrssec(rayP, rayD, pos) != CRSSEC_NON && m_active_seed_id < 0)
  {
    const int    frameI = formVisParam_getframeI();
    const int    frameN = (int)ImageCore::GetInst()->m_img4d.size();
    const EVec3f cube = ImageCore::GetInst()->GetCuboidF();
    const EVec3f pitch = ImageCore::GetInst()->GetPitch();
    const int init_max_thresh = -400;
    const int init_min_thresh = -1500;

    const short max_thresh = (m_seeds.size() == 0)? init_max_thresh : m_seeds.back().GetThreshold(frameI)[1];
    const short min_thresh = (m_seeds.size() == 0)? init_min_thresh : m_seeds.back().GetThreshold(frameI)[0];
    const float radius     = (m_seeds.size() == 0)? max3(pitch[0], pitch[1], pitch[2]) * 10: m_seeds.back().GetRadius(frameI);

    m_seeds.push_back( LocalSeed( frameN, frameI, pos, radius, EVec2i(min_thresh, max_thresh)) );

    m_active_seed_id = (int)m_seeds.size() - 1;
    std::cout << "create local seed\n";
  }

  FormSegLocalRGrow_UpdateSeedSelection();
  formMain_RedrawMainPanel();
}

void ModeSegLocalRGrow::RBtnDclk(const EVec2i &p, OglForCLI *ogl){}
void ModeSegLocalRGrow::MBtnDclk(const EVec2i &p, OglForCLI *ogl){}


void ModeSegLocalRGrow::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);
  const int frame_idx = formVisParam_getframeI();

  if( !m_bL && !m_bR && !m_bM )
  {
    //modify mouse cursor
    int sid = PickSeeds(ray_pos, ray_dir);
    if ( sid != -1)
    {
      const EVec3f seed_pos = m_seeds[ sid ].GetPosition(frame_idx);
      const float  seed_rad = m_seeds[ sid ].GetRadius  (frame_idx); 
      if ( DistRayAndPoint(ray_pos, ray_dir, seed_pos) > 0.8 * seed_rad )
        formMain_setCursorNESW();
      else 
        formMain_setCursorDefault();
    }
    else
    {
      formMain_setCursorDefault();
    }

    return;
  }

  if ( m_is_drag_activeseed )
  {
    if ( PickCrssec(ray_pos, ray_dir, pos) != CRSSEC_NON)
    {
      SelectSeed_SetPosition(pos);
    }
  }
  else if ( m_is_resize_activeseed )
  {
    if (PickCrssec(ray_pos, ray_dir, pos) != CRSSEC_NON)
    {
      float dist = DistRayAndPoint( ray_pos, ray_dir, m_seeds[m_active_seed_id].GetPosition(frame_idx) );
      SelectSeed_SetRadius( dist );
    }
  }
  else
  {
    ogl->MouseMove(p);
  }

  formMain_RedrawMainPanel();
}


void ModeSegLocalRGrow::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);
  const int frame_idx = formVisParam_getframeI();

  int sid = PickSeeds(ray_pos, ray_dir);
  if ( sid != -1)
  {
    float seed_rad = m_seeds[ sid ].GetRadius( frame_idx ) + z_delta * 0.01f;
    if(seed_rad  < 0.11f) seed_rad = 0.11f; 
    SelectSeed_SetRadius( seed_rad );
    FormSegLocalRGrow_UpdateSeedSelection();
  }
  else if(!WheelingCrssec(p, z_delta, ogl) ) {
    ogl->ZoomCamera(z_delta * 0.1f);
  }

  formMain_RedrawMainPanel();
}



//各シードに対してレイとの衝突を検出
int ModeSegLocalRGrow::PickSeeds(const EVec3f &ray_pos, const EVec3f &ray_dir)
{
  const int frame_idx = formVisParam_getframeI();

  float mindist = FLT_MAX;
  int   min_id = -1;

  for (int i = 0; i < (int)m_seeds.size(); ++i)
  {
    float distance = DistRayAndPoint(ray_pos, ray_dir, m_seeds[i].GetPosition(frame_idx) );
    if ( distance < m_seeds[i].GetRadius(frame_idx) && distance < mindist)
    {
      min_id  = i;
      mindist = distance;
    }
  }
  return min_id;
}


void ModeSegLocalRGrow::KeyDown(int nChar) {}
void ModeSegLocalRGrow::KeyUp(int nChar) {}






void ModeSegLocalRGrow::StartMode()
{
  //initialize field 
  m_bL = m_bR = m_bM = false;
  m_is_modified = false;
  m_active_seed_id = -1;
  m_is_drag_activeseed   = false;
  m_is_resize_activeseed = false;

  m_seeds.clear();
  m_cp_radius = 2.0f * ImageCore::GetInst()->GetPitch().x();

  m_unitsphere.InitializeAsSphere( 1.0, 20, 20);

  //initialize vFlg 0, 1
  ImageCore::GetInst()->InitializeFlg4dByMask( formMain_SetProgressValue );

  //show dialog
  EVec2i minmax = ImageCore::GetInst()->GetVolumeMinMax();
  EVec3f cube   = ImageCore::GetInst()->GetCuboidF();
  float max_radius = max3(cube[0], cube[1], cube[2]);

  FormSegLocalRGrow_Show();
  FormSegLocalRGrow_InitAllItems(max_radius, minmax[0], minmax[1]);

  //4D volume (cpu) --> vis volume (gpu)
  UpdateImageCoreVisVolumes();

  FormSegLocalRGrow_UpdateSeedSelection();
}



void ModeSegLocalRGrow::DrawScene(
    const EVec3f &cuboid, 
    const EVec3f &cam_pos, 
    const EVec3f &cam_cnt)
{
  const EVec3i reso = ImageCore::GetInst()->GetReso();

  //bind volumes ---------------------------------------
  BindAllVolumes();
  DrawCrossSections(cuboid, reso, !IsSpaceKeyOn(), m_crssec_shader);

  if ( formVisParam_bRendVol() )
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    const bool b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolumeSlices( cuboid, reso, cam_pos, cam_cnt, 
                     !IsSpaceKeyOn(), b_onmanip, m_volume_shader);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }

  const int num_seeds  = (int) m_seeds.size();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int frame_idx  = formVisParam_getframeI();

  //draw control points
  glEnable(GL_LIGHTING);
  glEnable(GL_BLEND);
  if(!IsSKeyOn())
  {
    for (int s = 0; s < num_seeds; ++s)
    {
      const EVec3f p = m_seeds[s].GetPosition( frame_idx );
      const float  r = m_cp_radius;
      if ( s == m_active_seed_id ) 
        TMesh::DrawSphere(p, r, COLOR_R, COLOR_RH, COLOR_W, COLOR_SHIN64);
      else                            
        TMesh::DrawSphere(p, r, COLOR_G, COLOR_G , COLOR_W, COLOR_SHIN64);
    }
  }

  //draw spheres
	glDisable(GL_LIGHTING);

	for (int i = 0; i < num_seeds; ++i)
	{ 
    const EVec3f p = m_seeds[i].GetPosition( frame_idx );
    const float r  = m_seeds[i].GetRadius(frame_idx);

		if (m_active_seed_id == i) 
      glColor4f(1.0f, 1.0f, 0.0f, 0.3f);
		else 
      glColor4f(0.4f, 0.4f, 1.0f, 0.3f);
    
    glTranslated(p[0], p[1], p[2]);
		m_unitsphere.Scale( r );
    m_unitsphere.Draw();
		m_unitsphere.Scale( 1.0f/r );
    glTranslated(-p[0],-p[1],-p[2]);
	}

  glDisable(GL_BLEND);
}




void ModeSegLocalRGrow::SelectSeed_SetRadius(float new_radius)
{
  if ( m_active_seed_id < 0 || (int)m_seeds.size() <= m_active_seed_id ) return;

  LocalSeed &seed = m_seeds[ m_active_seed_id ];  
  
  if ( FormSegLocalRGrow_DoSetSameRadForAllFrame() ) 
  {
    const int num_frames = ImageCore::GetInst()->GetNumFrames();
    for ( int i = 0; i < num_frames; ++i) seed.SetRadius( i, new_radius);
  }
  else
  {
    seed.SetRadius( formVisParam_getframeI(), new_radius );
    seed.SetIsEdit( formVisParam_getframeI(), true       );
  }
}



void ModeSegLocalRGrow::SelectSeed_SetPosition( EVec3f new_pos )
{
  if ( m_active_seed_id < 0 || (int)m_seeds.size() <= m_active_seed_id ) return;

  LocalSeed &seed = m_seeds[ m_active_seed_id ];  
  
  if ( FormSegLocalRGrow_DoSetSamePosForAllFrame() ) 
  {
    const int num_frames = ImageCore::GetInst()->GetNumFrames();
    for ( int i = 0; i < num_frames; ++i) seed.SetPosition( i, new_pos );
  }
  else
  {
    seed.SetPosition( formVisParam_getframeI(), new_pos );
    seed.SetIsEdit  ( formVisParam_getframeI(), true    );
  }
}



//閾値変更
void ModeSegLocalRGrow::SelectSeed_SetThreshMax( int maxv )
{
  if ( m_active_seed_id < 0 || (int)m_seeds.size() <= m_active_seed_id ) return;

  LocalSeed &seed = m_seeds[ m_active_seed_id ];  
  
  if ( FormSegLocalRGrow_DoSetSameThreshForAllFrame() ) 
  {
    const int num_frames = ImageCore::GetInst()->GetNumFrames();
    for ( int i = 0; i < num_frames; ++i) seed.SetThreshMax( i, maxv );
  }
  else
  {
    seed.SetThreshMax( formVisParam_getframeI(), maxv );
    seed.SetIsEdit   ( formVisParam_getframeI(), true );
  }
}



void ModeSegLocalRGrow::SelectSeed_SetThreshMin( int minv )
{
  if ( m_active_seed_id < 0 || (int)m_seeds.size() <= m_active_seed_id ) return;

  LocalSeed &seed = m_seeds[ m_active_seed_id ];  
  
  if ( FormSegLocalRGrow_DoSetSameThreshForAllFrame() ) 
  {
    const int num_frames = ImageCore::GetInst()->GetNumFrames();
    for ( int i = 0; i < num_frames; ++i) seed.SetThreshMin( i, minv );
  }
  else
  {
    seed.SetThreshMin( formVisParam_getframeI(), minv );
    seed.SetIsEdit   ( formVisParam_getframeI(), true );
  }
}



void ModeSegLocalRGrow::SelectSeed_RunInterpolation()
{
  RunSeedInterpolation( m_active_seed_id );
}



//#example 
//  pre = 4   0.0
//  new = 10  1.0 
//  4,5,6,7,8,9,10 (10-4 = 6等分) 
//   
//  1/(10-4) = 1/6.0
//  4:0, 5:1/6, 6:2/6, 7:3/6, 8:4/6, 9:5/6, 10:6/6
//


//シード補間(位置 / 半径 / 閾値)
void ModeSegLocalRGrow::RunSeedInterpolation( int trgt_seed_id)
{
  if (trgt_seed_id < 0 || trgt_seed_id >= m_seeds.size())return;
  
  const int num_frames    = ImageCore::GetInst()->GetNumFrames();
  bool rad_fixallframe    = FormSegLocalRGrow_DoSetSameRadForAllFrame();
  bool pos_fixallframe    = FormSegLocalRGrow_DoSetSamePosForAllFrame();
  bool thresh_fixallframe = FormSegLocalRGrow_DoSetSameThreshForAllFrame();
  LocalSeed &seed = m_seeds[trgt_seed_id];


  if( !rad_fixallframe ) 
  {
    int preid = -1;

    for ( int fi = 0; fi < num_frames; ++fi )
    {
      if ( !seed.GetIsEdit(fi) ) continue;

      float r1    = seed.GetRadius( preid );
      float r2    = seed.GetRadius( fi    );
      float delta = (r2-r1) / (float)(fi - preid); 

      if ( preid == -1 ) 
        for ( int i = preid + 1; i < fi; ++i) seed.SetRadius( i, r2 );
      else
        for ( int i = preid + 1; i < fi; ++i) seed.SetRadius( i, delta * (i - preid) + r1);

      preid = fi;
    }
       
    if ( preid != -1 )
      for ( int i = preid + 1; i < num_frames; ++i) seed.SetRadius( i, seed.GetRadius(preid) );
  }


  if( !pos_fixallframe ) 
  {
    int preid = -1;

    for ( int fi = 0; fi < num_frames; ++fi )
    {
      if ( !seed.GetIsEdit(fi) ) continue;
      EVec3f p1    = seed.GetPosition( preid );
      EVec3f p2    = seed.GetPosition( fi );
      EVec3f delta = (p2-p1)/(float)(fi - preid); 

      if ( preid == -1 )
        for ( int i = preid + 1; i < fi; ++i) seed.SetPosition( i, p2 );
      else 
        for ( int i = preid + 1; i < fi; ++i) seed.SetPosition( i, delta * (float)(i - preid) + p1);

      preid = fi;
    }

    if ( preid != -1 )
      for ( int i = preid + 1; i < num_frames; ++i) seed.SetPosition( i, seed.GetPosition(preid) );
  }

  if( !thresh_fixallframe ) 
  {
    int preid = -1;
    for ( int fi = 0; fi < num_frames; ++fi )
    {
      if ( !seed.GetIsEdit(fi) ) continue;
      EVec2i t1 = seed.GetThreshold( preid );
      EVec2i t2 = seed.GetThreshold( fi );
      float  d0 = (t2[0]-t1[0])/(float)(fi - preid); 
      float  d1 = (t2[1]-t1[1])/(float)(fi - preid); 

      if ( preid == -1 )
        for ( int i = preid + 1; i < fi; ++i) seed.SetThresh( i, t2 );
      else 
        for ( int i = preid + 1; i < fi; ++i) seed.SetThresh( i, EVec2i((int)( d0*(i - preid)+t1[0] ), (int)( d1*(i - preid)+t1[1] )) );

      preid = fi;
    }

    if ( preid != -1 )
      for ( int i = preid + 1; i < num_frames; ++i) seed.SetThresh( i, seed.GetThreshold(preid) );
  }
}




template<class T, class M>
static bool t_inRange(const M &num, const T &min, const T &max) {
  if (min > num) return false;
  if (max < num) return false;
  return true;
}


void t_RunLocalRegionGrow3D
(
  const EVec3f &seed_pos   ,
  const float  &seed_rad   ,
  const EVec2i &seed_minmax,
  
  const int W, const int H, const int D,
  const EVec3f &pitch,
  const short *img3d,
  byte* flg3d
)
{
  //get resolution (この関数でImageCore二アクセスするのは少し気持ち悪いけど許容)
  int WH = W*H, WHD = W*H*D;
  const short minv = seed_minmax[0];
  const short maxv = seed_minmax[1];
  const float radi2 = seed_rad*seed_rad;

#pragma omp parallel for 
  for ( int i=0; i < WHD; ++i) flg3d[i] = (flg3d[i]==0) ? 0 : 1; 

  //CP --> pixel id
  TQueue<EVec4i> Q;
  {
    const int xi = Crop( 0, W - 1, (int)(seed_pos[0] / pitch[0]));
    const int yi = Crop( 0, H - 1, (int)(seed_pos[1] / pitch[1]));
    const int zi = Crop( 0, D - 1, (int)(seed_pos[2] / pitch[2]));
    const int idx = xi + yi * W + zi * WH;
    if ( flg3d[idx] != 0 && t_inRange(img3d[idx], minv, maxv) )
    {
      Q.push_back( EVec4i(xi,yi,zi,idx) );
      flg3d[idx] = 255;
    }
  }

  //grow roi
  while (!Q.empty())
  {
    const int x = Q.front()[0];
    const int y = Q.front()[1];
    const int z = Q.front()[2];
    const int I = Q.front()[3];
    const EVec3f piv_pos( (x+0.5f)*pitch[0], (y+0.5f)*pitch[1], (z+0.5f)*pitch[2]);
    Q.pop_front();

    //範囲外にはみ出ていたら成長しない（1画素分はみ出てしまうが，計算量的にこの実装を採用）
    if ( DistSq( piv_pos, seed_pos) > radi2) continue;
    
    //grow to 6 neighbors
    int    K;
    K = I - 1 ; if (x > 0 && flg3d[K] == 1 && t_inRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(EVec4i(x-1,y,z,K)); }
    K = I - W ; if (y > 0 && flg3d[K] == 1 && t_inRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(EVec4i(x,y-1,z,K)); }
    K = I - WH; if (z > 0 && flg3d[K] == 1 && t_inRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(EVec4i(x,y,z-1,K)); }
    K = I + 1 ; if (x <W-1&& flg3d[K] == 1 && t_inRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(EVec4i(x+1,y,z,K)); }
    K = I + W ; if (y <H-1&& flg3d[K] == 1 && t_inRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(EVec4i(x,y+1,z,K)); }
    K = I + WH; if (z <D-1&& flg3d[K] == 1 && t_inRange(img3d[K], minv, maxv)) { flg3d[K] = 255; Q.push_back(EVec4i(x,y,z+1,K)); }
  }
}


void ModeSegLocalRGrow::RunLocalRegionGrow_OnlyTrgtFrame(int trgt_frame_idx)
{
  int W,H,D,WH,WHD;
  std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();
  const EVec3f pitch     = ImageCore::GetInst()->GetPitch();
  std::vector <short*> img4d  = ImageCore::GetInst()->m_img4d;
  std::vector <byte *> flg4d  = ImageCore::GetInst()->m_flg4d;
  const int num_voxels   = ImageCore::GetInst()->GetNumVoxels();
  const int num_frames   = ImageCore::GetInst()->GetNumFrames();


  const short* img3d = img4d[ trgt_frame_idx ];
  byte*        flg3d = flg4d[ trgt_frame_idx ];
  for (int vi = 0; vi < num_voxels; ++vi)
  {
    flg3d[vi] = (flg3d[vi] == 0) ? 0 : 1;
  }
  byte* init_inout = new byte[WHD];
  byte* tmp_flg3d  = new byte[WHD];
  memcpy(init_inout, flg3d, sizeof(byte)*WHD);

  //region growing for each frame
  for( int si = 0; si < m_seeds.size(); ++si)
  {
    memcpy(tmp_flg3d, init_inout, sizeof(byte)*WHD);

    t_RunLocalRegionGrow3D(
      m_seeds[si].GetPosition(trgt_frame_idx), 
      m_seeds[si].GetRadius(trgt_frame_idx), 
      m_seeds[si].GetThreshold(trgt_frame_idx), 
      W,H,D,pitch, img3d, tmp_flg3d);

    for (int vi = 0; vi < WHD; ++vi) {
      if (tmp_flg3d[vi] == 255)
        flg3d[vi] = 255;
    }
  }
  delete[] tmp_flg3d ;
  delete[] init_inout;
}


void ModeSegLocalRGrow::RunLocalRegionGrow_Allframe()
{
  const int num_frames   = ImageCore::GetInst()->GetNumFrames();

#pragma omp parallel for
  for (int fi = 0; fi < num_frames; ++fi)
  {
    RunLocalRegionGrow_OnlyTrgtFrame(fi);
  }

  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}



void ModeSegLocalRGrow::RunLocalRegionGrow_OnlyCurrentFrame()
{
  const int frame_idx = formVisParam_getframeI();
  RunLocalRegionGrow_OnlyTrgtFrame(frame_idx);

  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}


#pragma managed
