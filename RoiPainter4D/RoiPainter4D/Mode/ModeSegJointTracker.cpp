#pragma unmanaged
#include "ModeSegJointTracker.h"
#include "ModeCommonTools.h"
#include "tmarchingcubes.h"

#pragma managed
#include "FormMain.h"
#include "FormSegJointTracker.h"
#include "FormVisParam.h"
#include "CliMessageBox.h"
#pragma unmanaged

#include <iostream>
#include <fstream>
#include "ImageCore.h"
#include "ModeSegRigidICP.h"

//openCV
#include "thandle3d.h"
#include "opencv/cv.h"
#include "opencv2/surface_matching.hpp"
#include "opencv2/surface_matching/ppf_helpers.hpp"
#include "opencv2/core/utility.hpp"
#pragma comment(lib,"opencv_core310.lib")
#pragma comment(lib,"opencv_surface_matching310.lib")

using namespace RoiPainter4D;



//memo rigid transform matrix Matrix4f 
// todo 0501 
// rigid trackingの内容を再確認 OK
// ライトの位置を調整 
// shiftを押していないとドラッグできないように OK
// eval meshにもハンドル表示を  OK 

ModeSegJointTracker::ModeSegJointTracker() : 
  m_volume_shader("shader/volVtx.glsl", "shader/volFlg_Seg.glsl"),
  m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
}


ModeSegJointTracker::~ModeSegJointTracker()
{
}



bool ModeSegJointTracker::CanEndMode()
{
  return true;
}


void ModeSegJointTracker::StartMode()
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  m_bL = m_bM = m_bR = false;
  m_b_transrot = false;  
  FormSegJointTracker_Show();
  
  EVec2i minmax = ImageCore::GetInst()->GetVolumeMinMax();
  FormSegJointTracker_InitializeItems(minmax[0], minmax[1]);
  
  //isosurface
  m_isovalue = 0;
  m_isosurfaces.resize( num_frames );
  for ( int i = 0; i < (int) m_isosurf_pflg.size(); ++i)
  { 
    if ( m_isosurf_pflg[i] != 0 ) delete[] m_isosurf_pflg[i];
  }
  m_isosurf_pflg.clear();
  m_isosurf_pflg.resize(num_frames,0);
  
  //rigid tracking matrix 
  m_mats_bone1  .resize( num_frames );
  m_mats_bone2  .resize( num_frames );
  m_mats_evalobj.resize( num_frames );
  
  for ( int i=0; i < num_frames; ++i )
  {
    m_isosurfaces [i].Clear();
    m_mats_bone1  [i].setIdentity();
    m_mats_bone2  [i].setIdentity();
    m_mats_evalobj[i].setIdentity();
  }  
  m_cps_bone1.clear();
  m_cps_bone2.clear();
  m_srcbone1 .clear();
  m_srcbone2 .clear();
  m_evalobj.Clear();
  m_handle_rot.setIdentity();
  m_dragcp << -1, -1;
  

}


void ModeSegJointTracker::FinishSegmentation(){}
void ModeSegJointTracker::CancelSegmentation(){}



/////////////////////////////////////////////////////////////////////
//MouseListener//////////////////////////////////////////////////////

const char* MSG_RUNICP  = "ICPの修整（移動）操作は，一度ICPを実行した後を行なえます．";
const char* MSG_FRMNON0 = "この移動操作はframe=0以外に対して行なえます．";
const char* MSG_NOMESH  = "評価用メッシュがロードされていません" ;
const char* MSG_FMR0    = "この移動操作はframe=0のみに対して行なえます．";

void ModeSegJointTracker::LBtnDown (const EVec2i &p, OglForCLI *ogl)
{
	m_bL = true;
	const int frame_idx = formVisParam_getframeI();

  if( !IsShiftKeyOn() )
  {
    ogl->BtnDown_Trans( p );
    m_prevpoint = p;
    return;
  }

  if ( FormJTrack_bModePlaceCpBone1() || FormJTrack_bModePlaceCpBone2() )
  {
    //1. pick cp to move
    EVec3f ray_p, ray_d;
    std::tie(ray_p, ray_d) = ogl->GetCursorRay1(p);
    
    if ( frame_idx == 0 )
      m_dragcp = CPs_Pick(  ray_p, ray_d);
    
    if ( m_dragcp[0] == -1 )
      ogl->BtnDown_Trans( p );
  }
  else if( FormJTrack_bModeTransBone1() || FormJTrack_bModeRotBone1() ) 
  {
    if ( m_srcbone1.size() == 0 ) 
      ShowMsgDlg_OK( MSG_RUNICP, "MSG" ); 
    else if ( frame_idx == 0) 
      ShowMsgDlg_OK( MSG_FRMNON0, "MSG" );
    else 
      m_b_transrot = true;
  }
  else if( FormJTrack_bModeTransBone2() || FormJTrack_bModeRotBone2() )
  {
    if ( m_srcbone2.size() == 0 ) 
      ShowMsgDlg_OK( MSG_RUNICP, "MSG" ); 
    else if ( frame_idx == 0) 
      ShowMsgDlg_OK( MSG_FRMNON0, "MSG" );
    else 
      m_b_transrot = true;

  }
  else if( FormJTrack_bModeTransObj() || FormJTrack_bModeRotObj() )
  {
    //2. trans/rot (bone1/bone2/evalsurf)
    if ( frame_idx != 0) 
      ShowMsgDlg_OK( MSG_FMR0, "MSG" );
    else 
      m_b_transrot = true;
  } 
  m_prevpoint = p;
}



void ModeSegJointTracker::LBtnUp   (const EVec2i &p, OglForCLI *ogl)
{
	const int frame_idx = formVisParam_getframeI();
  if ( frame_idx != 0 && m_dragcp[0] > 0 )
  {
    ShowMsgDlg_OK("制御点指定はframe=0で行なってください", "MSG");
  }
  
	m_bL = m_b_transrot = false;
  m_dragcp << -1, -1;
  m_handle_rot.setIdentity();
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeSegJointTracker::RBtnDown (const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
  ogl->BtnDown_Rot(p);
}


void ModeSegJointTracker::RBtnUp   (const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  ogl->BtnUp();
}


void ModeSegJointTracker::MBtnDown (const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}


void ModeSegJointTracker::MBtnUp   (const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();

}


void ModeSegJointTracker::RBtnDclk (const EVec2i &p, OglForCLI *ogl){}
void ModeSegJointTracker::MBtnDclk (const EVec2i &p, OglForCLI *ogl){}

void ModeSegJointTracker::LBtnDclk (const EVec2i &p, OglForCLI *ogl)
{
  EVec3f rayp, rayd;
  ogl->GetCursorRay(p, rayp, rayd);
	const int frame_idx = formVisParam_getframeI();

	if( IsShiftKeyOn() ) // pick iso surface to activate
	{
    IsoSurf_PickToActivate(rayp, rayd);
		return;
	}

  //CP remove / CP add 
  if ( !FormJTrack_bModePlaceCpBone1() && !FormJTrack_bModePlaceCpBone2() ) 
  {
    return;
  }

  if ( frame_idx != 0 )
  {
    ShowMsgDlg_OK("制御点配置は frame=0 で行って下さい", "MSG");
    return;
  }
  
  //pick cp to remove   
  EVec2i cpid = this->CPs_Pick( rayp, rayd);
	if( cpid[0] >= 1 ) 
	{
		this->CPs_Remove( cpid );
		return;
	}
  
  //pick to add
	EVec3f pos;
  if ( IsoSurf_PickActiveSurf( rayp, rayd, pos ) ) 
  { 
    int bone_id = FormJTrack_bModePlaceCpBone1() ? 1 : 2;
    this->CPs_Add( bone_id, pos);
  }

}





static void t_RotRigidMat(const EMat3f &R, const EVec3f c, EMat4d &M)
{
  // current pos  v' = M v
  // new pos      v''= T v' =  R * (M v - c) + c (translate in global coordinate)
  // new pos      v''= T v' =  T(c) * R * T(-c) * M v  
  M(0,3) -= c[0];
  M(1,3) -= c[1];
  M(2,3) -= c[2];
  EMat4d R44;
  R44.setIdentity();
  R44(0,0) = R(0,0); R44(0,1) = R(0,1); R44(0,2) = R(0,2); 
  R44(1,0) = R(1,0); R44(1,1) = R(1,1); R44(1,2) = R(1,2); 
  R44(2,0) = R(2,0); R44(2,1) = R(2,1); R44(2,2) = R(2,2);
  M = R44*M;
  M(0,3) += c[0];
  M(1,3) += c[1];
  M(2,3) += c[2];
}


static void t_TransRigidMat(const EVec3f &v, EMat4d &M)
{
  // current pos  v' = M v
  // new pos      v'' = T v' = T M v (translate in global coordinate)
  M(0,3) += v[0];
  M(1,3) += v[1];
  M(2,3) += v[2];
}




void ModeSegJointTracker::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
	if( !m_bR && !m_bM && !m_bL) return;

  EVec3f ray_p, ray_d;
  ogl->GetCursorRay(p, ray_p, ray_d);
	const int frame_idx = formVisParam_getframeI();

	if( m_dragcp[0] >= 1 && frame_idx == 0)
	{
		EVec3f pos;
		if( IsoSurf_PickActiveSurf( ray_p, ray_d, pos) )
		{ 
      CPs_Move( m_dragcp, pos );
		}
	}
  else if ( m_b_transrot )
  {
    bool rotbone1 = FormJTrack_bModeRotBone1();
    bool rotbone2 = FormJTrack_bModeRotBone2();
    bool rotobj   = FormJTrack_bModeRotObj();
    bool trsbone1 = FormJTrack_bModeTransBone1();
    bool trsbone2 = FormJTrack_bModeTransBone2();
    bool trsobj   = FormJTrack_bModeTransObj();
    int dx = p[0] - m_prevpoint[0];
    int dy = p[1] - m_prevpoint[1];
 
    if ( rotbone1 || rotbone2 || rotobj ) 
    {
      EMat3f rot = CalcObjectRotationMatrixByMouseDragF( ogl, dx, dy );
      m_handle_rot = rot * m_handle_rot;

      if (      rotbone1 ) {
        EVec3f gc =  CalcCentroid(m_cps_bone1) ;
        t_RotRigidMat( rot, gc, m_mats_bone1[frame_idx] ) ;
      } 
      else if ( rotbone2 ) {
        EVec3f gc =  CalcCentroid(m_cps_bone2) ;
        t_RotRigidMat( rot, gc, m_mats_bone2[frame_idx] ) ;
      }
      else if ( rotobj   ) {
        EvalObj_Rotate( frame_idx, rot );
      }
    }
    else if ( trsbone1 || trsbone2 || trsobj )
    {
      EVec3f trans = CalcObjectTransVectorByMouseDragF( ogl, dx, dy);
      Trace(trans);
      if (      trsbone1 ) t_TransRigidMat( trans, m_mats_bone1[frame_idx] ) ; 
      else if ( trsbone2 ) t_TransRigidMat( trans, m_mats_bone2[frame_idx] ) ;
      else if ( trsobj   ) EvalObj_Translate( frame_idx, trans );
    }
    m_prevpoint = p; 
  }
	else 
  {
    ogl->MouseMove( p );
  }
   
  formMain_RedrawMainPanel();
}


void ModeSegJointTracker::MouseWheel(
    const EVec2i &p, 
    short z_delta, 
    OglForCLI *ogl)
{
  if(!WheelingCrssec(p, z_delta, ogl) ) 
    ogl->ZoomCamera(z_delta * 0.1f);
  formMain_RedrawMainPanel();
}
void ModeSegJointTracker::KeyDown(int nChar){}
void ModeSegJointTracker::KeyUp(int nChar){}





void ModeSegJointTracker::DrawRegistSurfaces( int frameidx )
{
  if ( frameidx >= m_mats_bone1.size() ) return;
  glPointSize( 2 );

  //bone 1
  glColor3d( 0,0,1 );
  glPushMatrix();
  glMultMatrixd( m_mats_bone1[frameidx].data() );
  glBegin( GL_POINTS );
  for( auto it : m_srcbone1 ) glVertex3fv( it.first.data() );
  glEnd();
  glPopMatrix();
  
  //bone2
  glColor3d( 1,0,1 );
  glPushMatrix();
  glMultMatrixd( m_mats_bone2[frameidx].data() );
  glBegin( GL_POINTS );
  for( auto it : m_srcbone2 ) glVertex3fv( it.first.data() );
  glEnd();
  glPopMatrix();
}


#define CP_SIZE 2.0f

void ModeSegJointTracker::DrawCpTrajectory(int frameidx, int cpidx)
{
  const int num_frames = (int) m_isosurfaces.size();
  if ( m_isosurfaces.size() <= frameidx ) return;
  if ( m_cps_bone1.size()   <  3        ) return; 
  if ( m_cps_bone2.size()   <= cpidx ) return; 
  
	glColor3d( 0.0 ,0.0, 1.0 );
    
  const EVec4d cp( m_cps_bone2[cpidx][0], 
                   m_cps_bone2[cpidx][1], 
                   m_cps_bone2[cpidx][2],1); 

	for ( int i = 0; i < (int) m_mats_bone2.size(); ++i )
	{
    EVec4d t = m_mats_bone2[i] * cp;
    TMesh::DrawSphere(EVec3f((float)t[0], (float)t[1], (float)t[2])  , CP_SIZE * 0.3f);
	}

	glLineWidth( 2 );
	glBegin( GL_LINE_STRIP );
	for ( int i = 0; i < (int)m_mats_bone2.size(); ++i )
	{
    EVec4d t = m_mats_bone2[i] * cp;
    glVertex3d( t[0], t[1], t[2]);
	}
	glEnd();
}






void ModeSegJointTracker::DrawScene(
    const EVec3f &cuboid, 
    const EVec3f &camP, 
    const EVec3f &camF)
{
  GLfloat ambi0[3] = {0.5f,0.5f,0.5f};
  GLfloat ambi1[3] = {0,0,0};
  GLfloat ambi2[3] = {0,0,0};
  GLfloat diff[3] = {0.8f,0.8f,0.8f};
  GLfloat spec[3] = {0.3f,0.3f,0.3f};

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT , ambi0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE , diff);
  glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT , ambi1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE , diff);
  glLightfv(GL_LIGHT1, GL_SPECULAR, spec);
  
  glEnable(GL_LIGHT2);
  glLightfv(GL_LIGHT2, GL_AMBIENT , ambi2);
  glLightfv(GL_LIGHT2, GL_DIFFUSE , diff);
  glLightfv(GL_LIGHT2, GL_SPECULAR, spec);

  const EVec3i reso      = ImageCore::GetInst()->GetReso();
  const int    frame_idx = formVisParam_getframeI();

  BindAllVolumes();
  DrawCrossSections( cuboid, reso, !IsSpaceKeyOn(), m_crssec_shader );
   
  //draw iso surfaces
  if ( FormJTrack_bVisIsoSurface() ) IsoSurf_Draw( frame_idx );
  if ( FormJTrack_bVisCps()        ) CPs_Draw( frame_idx);
  if ( FormJTrack_bVisFitSurface() ) DrawRegistSurfaces( frame_idx );
  if ( FormJTrack_bVisEvalSurf()   ) EvalObj_DrawSurf( frame_idx );
  if ( FormJTrack_bVisEvalDiff()   ) EvalObj_DrawDiff( frame_idx );
  if ( FormJTrack_bVisCpTrajecory())
    DrawCpTrajectory(frame_idx, FormJTrack_GetVisTrajCpid());

  const float HNDL_R = (float)(cuboid.norm() / 10.0);
  const float HNDL_W = HNDL_R * 0.01f; 

  if ( FormJTrack_bModeRotBone1() )
    DrawHandleOrthoCircles( CalcCentroid( m_cps_bone1 ), HNDL_R);
  if ( FormJTrack_bModeRotBone2() )
    DrawHandleOrthoCircles( CalcCentroid( m_cps_bone2 ), HNDL_R);    
  if ( FormJTrack_bModeTransBone1() ) 
    DrawHandleOrthoArrows(  CalcCentroid( m_cps_bone1 ), HNDL_R, HNDL_W, COLOR_R, COLOR_G, COLOR_B);
  if ( FormJTrack_bModeTransBone2() ) 
    DrawHandleOrthoArrows(  CalcCentroid( m_cps_bone2 ), HNDL_R, HNDL_W, COLOR_R, COLOR_G, COLOR_B);
  if ( FormJTrack_bModeRotObj()   ) {
    EVec3f gc( (float)m_mats_evalobj[0](0,3), 
               (float)m_mats_evalobj[0](1,3), 
               (float)m_mats_evalobj[0](2,3) );
    DrawHandleOrthoCircles( gc, HNDL_R);    
  }
  if ( FormJTrack_bModeTransObj() ) {
    EVec3f gc( (float)m_mats_evalobj[0](0,3), 
               (float)m_mats_evalobj[0](1,3), 
               (float)m_mats_evalobj[0](2,3) );
    DrawHandleOrthoArrows( gc, HNDL_R, HNDL_W, COLOR_R, COLOR_G, COLOR_B);
  }

  if (formVisParam_bRendVol())
  {
    const bool b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    DrawVolumeSlices( cuboid, reso, camP, camF, 
                      !IsSpaceKeyOn(), b_onmanip, m_volume_shader);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }

}



using namespace marchingcubes;

void ModeSegJointTracker::IsoSurf_Generate(short isovalue )
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const EVec3i reso    = ImageCore::GetInst()->GetReso();
  const EVec3f pitch   = ImageCore::GetInst()->GetPitch();

  //1. clear
  for ( auto it : m_isosurf_pflg) if ( it != 0 ) delete[] it;
  m_isosurf_pflg.clear();
  m_isosurf_pflg.resize(num_frames, 0);
  m_isosurfaces .resize(num_frames );
    
  //2. generate
  m_isovalue = isovalue;
  for( int i = 0; i < num_frames; ++i)
  {
    const short* vol = ImageCore::GetInst()->m_img4d[i];
    TMesh &mesh = m_isosurfaces[i]; 
		MarchingCubes( reso, pitch, vol, isovalue, 0, 0, mesh);
    
    m_isosurf_pflg[i] = new byte[mesh.m_pSize];
    memset( m_isosurf_pflg[i],0, sizeof(byte)* mesh.m_pSize);
    
    std::cout << "compute iso surface " << i << "/" << num_frames << "\n"; 
  }
}



static float spec [4] = {0.9f,0.9f,0.9f,0.2f};
static float shin [1] = {54.0f};
static float diff1[4] = {0.2f,0.8f,0.2f,0.2f};
static float ambi1[4] = {0.2f,0.8f,0.2f,0.2f};  
static float diff2[4] = {0.4f,0.4f,0.4f,0.5f};
static float ambi2[4] = {0.2f,0.2f,0.2f,0.5f};

void ModeSegJointTracker::IsoSurf_Draw( int frame_idx )
{
  if( frame_idx < 0 || m_isosurfaces.size() <= frame_idx) return;

  const TMesh &mesh = m_isosurfaces[ frame_idx ];
  const int    num_poly = mesh.m_pSize;
  const TPoly  *polys   = mesh.m_pPolys ;
  const EVec3f *verts   = mesh.m_vVerts;
  const EVec3f *norms   = mesh.m_vNorms;
  const byte   *pflg    = m_isosurf_pflg[ frame_idx ];

  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0   );
  glEnable( GL_LIGHT1   );
  glEnable( GL_LIGHT2   );
  glMaterialfv( GL_FRONT, GL_SHININESS, shin  );
  glMaterialfv( GL_FRONT, GL_SPECULAR , spec  );
  glMaterialfv( GL_FRONT, GL_DIFFUSE  , diff1 );
  glMaterialfv( GL_FRONT, GL_AMBIENT  , ambi1 );

  //render flg==0
	glEnable( GL_BLEND    );
	glDepthMask( false    );
	glBegin( GL_TRIANGLES );
	for( int i = 0; i < num_poly; ++i) if( pflg[i] == 0 )
	{
		const int *p = polys[i].idx;
		glNormal3fv( norms[p[0]].data() ); glVertex3fv( verts[p[0]].data() );
		glNormal3fv( norms[p[1]].data() ); glVertex3fv( verts[p[1]].data() );
		glNormal3fv( norms[p[2]].data() ); glVertex3fv( verts[p[2]].data() );
	}
	glEnd();
	glDisable  ( GL_BLEND );
	glDepthMask( true     );


  glMaterialfv( GL_FRONT, GL_DIFFUSE  , diff2 );
  glMaterialfv( GL_FRONT, GL_AMBIENT  , ambi2 );
  glBegin( GL_TRIANGLES );
	for( int i = 0; i < num_poly; ++i) if( pflg[i] )
  {
    const int *p = polys[i].idx;
    glNormal3fv( norms[p[0]].data()); glVertex3fv( verts[p[0]].data());
    glNormal3fv( norms[p[1]].data()); glVertex3fv( verts[p[1]].data());
    glNormal3fv( norms[p[2]].data()); glVertex3fv( verts[p[2]].data());
  }
  glEnd();
  glDisable( GL_LIGHTING );

  if( IsSpaceKeyOn() )
  {
    mesh.DrawEdges(1,0,1,0);
  }
}



void ModeSegJointTracker::IsoSurf_PickToActivate(
    const EVec3f rayp, 
    const EVec3f rayd )
{
  for ( int i = 0; i < (int)m_isosurfaces.size(); ++ i)
  {
    const TMesh &mesh  = m_isosurfaces[i];
    byte        *pflgs = m_isosurf_pflg[i];

    memset( pflgs, 0, sizeof( byte ) * mesh.m_pSize );

    //extract connected component
    EVec3f pos;
    int pid;
    if( mesh.PickByRay( rayp, rayd, pos, pid ) )
    {
      std::vector<int> polyids, vertids;
      int vid = mesh.m_pPolys[ pid ].idx[0];
      mesh.ExtractLinkedVertsPolys(vid, vertids, polyids);

      for( auto it : polyids ) pflgs[ it ] = 255;
    }
  }
}




bool ModeSegJointTracker::IsoSurf_PickActiveSurf(
    const EVec3f rayp, 
    const EVec3f rayd, 
    EVec3f &pos)
{
  if ( m_isosurfaces.size() == 0) return false;


  const TMesh &mesh = m_isosurfaces [ 0 ];
  const byte* flg   = m_isosurf_pflg[ 0 ];
  const auto  num_polys = mesh.m_pSize;
  const auto *polys     = mesh.m_pPolys;
  const auto *verts     = mesh.m_vVerts;

  float depth = FLT_MAX;
  EVec3f tmppos;

  for (int pi = 0; pi < num_polys; ++pi ) if ( flg[pi] )
  {
    const int *p = polys[pi].idx;
    const EVec3f& x0 = verts[ p[0] ];
    const EVec3f& x1 = verts[ p[1] ];
    const EVec3f& x2 = verts[ p[2] ];
    if ( IntersectRayToTriangle( rayp, rayd, x0,x1,x2, tmppos))
    {
      float d = (tmppos - rayp).norm();
      if (d < depth)
      {
        depth = d;
        pos = tmppos;
      }
    }
  }
  return depth != FLT_MAX;
}




//////////////////////////////////////////////////////////////////
//Control points//////////////////////////////////////////////////

static bool t_CalcLocalCoordinate(
	  const std::vector<EVec3f> &basepoints, //3個の制御点
	  const std::vector<EVec3f> &trgtpoints, //CPs for moving object
		EVec3f &cd_orig,
		EVec3f &cd_xdir,
		EVec3f &cd_ydir,
		EVec3f &cd_zdir)
{
  if( basepoints.size() < 3 ) return false;
  
  //原点は (p0+p2)/2
	cd_orig = 0.5 * (basepoints[0] + basepoints[2]);
  
  //x方向は p1 - orig
	cd_xdir = (basepoints[1] - cd_orig).normalized();
  
  //y方向は p0 - orig
	cd_ydir = (basepoints[0] - cd_orig).normalized();
	cd_ydir -= (cd_ydir.dot(cd_xdir) ) * cd_xdir;
	cd_ydir.normalize();
  
  //z方向は 外積
	cd_zdir = cd_xdir.cross( cd_ydir );

	if(trgtpoints.size() != 0 )
	{
    EVec3f gc = CalcCentroid( trgtpoints );
		if( (gc - cd_orig).dot( cd_zdir ) < 0 ) cd_zdir *= -1;
	}

	return true;
}






void ModeSegJointTracker::CPs_Draw( int frameidx )
{
  if ( frameidx < 0 || m_mats_bone1.size() <= frameidx ) return; 
  
  static TMesh sphere;
  if ( sphere.m_vSize == 0 )
  {
    sphere.InitializeAsSphere(CP_SIZE, 10,10);
  }  

  //control points on bone 1  
  for( int i=0; i < (int) m_cps_bone1.size(); ++i)
  {
    if(      i%3 == 0 ) glColor3d( 0.5,0,0 );
    else if( i%3 == 1 ) glColor3d( 0,0.5,0 );
    else if( i%3 == 2 ) glColor3d( 0,0,0.5 );
    glPushMatrix();
    glMultMatrixd( m_mats_bone1[frameidx].data() );
    glTranslatef( m_cps_bone1[i][0], m_cps_bone1[i][1], m_cps_bone1[i][2]);
    sphere.Draw();
    glPopMatrix();
  }

  //control points on bone 2
  for( int i=0; i < (int) m_cps_bone2.size(); ++i)
  {
    if(      i%9 == 0 ) glColor3d( 1.0 ,0.25,0.25 );
    else if( i%9 == 1 ) glColor3d( 0.25,1.0 ,0.25 );
    else if( i%9 == 2 ) glColor3d( 0.25,0.25,1.0  );
    else if( i%9 == 3 ) glColor3d( 1.0 ,0.75,0.25 );
    else if( i%9 == 4 ) glColor3d( 0.25,1.0 ,0.75 );
    else if( i%9 == 5 ) glColor3d( 0.75,0.25,1.0  );
    else if( i%9 == 6 ) glColor3d( 1.0 ,0.6 ,0.6  );
    else if( i%9 == 7 ) glColor3d( 0.6 ,1.0 ,0.6  );
    else if( i%9 == 8 ) glColor3d( 0.6 ,0.6 ,1.0  );
    glPushMatrix();
    glMultMatrixd( m_mats_bone2[frameidx].data() );
    glTranslatef( m_cps_bone2[i][0],m_cps_bone2[i][1],m_cps_bone2[i][2]);
    sphere.Draw();
    glPopMatrix();
  }

  //coordinate frame
	if( m_cps_bone1.size() >= 3 )
	{
    EMat4d m1 = m_mats_bone1[frameidx];
    EMat4d m2 = m_mats_bone2[frameidx];
    std::vector<EVec3f> cps1, cps2;
    for ( const auto& p1 : m_cps_bone1) cps1.push_back( Mult( m1, p1) );    
    for ( const auto& p2 : m_cps_bone2) cps2.push_back( Mult( m2, p2) );    

    //gen coordinate system 
    EVec3f orig, axisx, axisy, axisz;
		t_CalcLocalCoordinate( cps1, cps2, orig, axisx, axisy, axisz);
    axisx = orig + 10.f * axisx;
    axisy = orig + 10.f * axisy;
    axisz = orig + 10.f * axisz;
		glLineWidth( 4 );
		glBegin( GL_LINES );
		glColor3d( 1,0,0); glVertex3fv( orig.data()); glVertex3fv( axisx.data()); 
		glColor3d( 0,1,0); glVertex3fv( orig.data()); glVertex3fv( axisy.data()); 
		glColor3d( 0,0,1); glVertex3fv( orig.data()); glVertex3fv( axisz.data()); 
		glEnd();
	}
}


EVec2i ModeSegJointTracker::CPs_Pick( EVec3f ray_p, EVec3f ray_d)
{ 
  for ( int i = 0; i < (int) m_cps_bone1.size(); ++i )
  {
    if ( DistRayAndPoint( ray_p, ray_d, m_cps_bone1[i]) < CP_SIZE) 
      return EVec2i( 1, i);
  }

  for ( int i = 0; i < (int) m_cps_bone2.size(); ++i )
  {
    if ( DistRayAndPoint( ray_p, ray_d, m_cps_bone2[i]) < CP_SIZE) 
      return EVec2i( 2, i);
  }
  return EVec2i(-1,-1);
}


void ModeSegJointTracker::CPs_Add(
    const int boneid, 
    const EVec3f pos)
{
  if ( boneid == 1 )
  {
    if ( m_cps_bone1.size() < 3) 
    {
      m_cps_bone1.push_back(pos);
    }
    else
    {
      ShowMsgDlg_OK(
        "bone1用CPは既に3個置かれています．\nbone2モードへ移行します．", "MSG");
      FormJTrack_SetModePlaceCpBone2();
      return;
    }
  }
  else if ( boneid == 2 )
  {
    m_cps_bone2.push_back(pos);
  }
}


void ModeSegJointTracker::CPs_Move  (const EVec2i cpid, const EVec3f pos)
{
  if ( cpid[1] < 0 ) return;
  if ( cpid[0] == 1 && cpid[1] < m_cps_bone1.size() ) m_cps_bone1[cpid[1]] = pos;
	if ( cpid[0] == 2 && cpid[1] < m_cps_bone2.size() ) m_cps_bone2[cpid[1]] = pos;
}


void ModeSegJointTracker::CPs_Remove(const EVec2i cpid )
{
    if ( cpid[1] < 0 ) return;
  if ( cpid[0] == 1 && cpid[1] < m_cps_bone1.size() ) 
    m_cps_bone1.erase(m_cps_bone1.begin() + cpid[1] );
	if ( cpid[0] == 2 && cpid[1] < m_cps_bone2.size() )
    m_cps_bone2.erase(m_cps_bone2.begin() + cpid[1] );
}



static void t_ExtractLinkedVsNs
(
    const TMesh  &mesh,
    const EVec3f &pivpoint,
    const int    &ringsize,
    
    std::vector<int> &vector_vertids,
    std::vector<std::pair<EVec3f,EVec3f>> &vertnorms
)
{
  int vi = mesh.GetNearestVertexIdx( pivpoint );

  //std::vector<int> vertids, polyids;
  //mesh.ExtractLinkedVertsPolys(vi, vertids, polyids);
  std::set<int> vertids;
  mesh.ExtractRingNeighborVerts(vi, ringsize, vertids);
  
  vector_vertids = std::vector<int> (vertids.begin(), vertids.end());  

  vertnorms.reserve( vertids.size() );
  for ( auto it : vertids ) 
  { 
    vertnorms.push_back( std::make_pair( mesh.m_vVerts[it], mesh.m_vNorms[it]) );
  }
}



static void t_SetMat(
    const std::vector<std::pair<EVec3f,EVec3f>> &vertnorms, 
    cv::Mat &mat
)
{
  //setup cv::matrix 
  mat = cv::Mat( (int) vertnorms.size(), 6, CV_32F);
  for (int n = 0; n < (int)vertnorms.size(); ++n)
  {
    mat.at<float>(n, 0) = vertnorms[n].first[0];
    mat.at<float>(n, 1) = vertnorms[n].first[1];
    mat.at<float>(n, 2) = vertnorms[n].first[2];
    mat.at<float>(n, 3) = vertnorms[n].second[0];
    mat.at<float>(n, 4) = vertnorms[n].second[1];
    mat.at<float>(n, 5) = vertnorms[n].second[2];
  }
}

static void t_SetMat( const TMesh &mesh, cv::Mat &mat )
{
  mat = cv::Mat( mesh.m_vSize, 6, CV_32F);
  for (int n = 0; n < (int)mesh.m_vSize; ++n)
  {
    mat.at<float>(n, 0) = mesh.m_vVerts[n][0];
    mat.at<float>(n, 1) = mesh.m_vVerts[n][1];
    mat.at<float>(n, 2) = mesh.m_vVerts[n][2];
    mat.at<float>(n, 3) = mesh.m_vNorms[n][0];
    mat.at<float>(n, 4) = mesh.m_vNorms[n][1];
    mat.at<float>(n, 5) = mesh.m_vNorms[n][2];
  }
}



//this function returns rigid transformation matrix 
//from srcPC to trgtPC
static EMat4d t_ICP(
    const float		 icp_reject,// icp parameter
    const int			 icp_levels,// icp parameter
    EMat4d				 init_mat  ,// initial 4x4 matrix
    const cv::Mat &src_mat   ,// src point cloud
    const cv::Mat &trgt_mat   // trgt point cloud
)
{
  //for ( int i=0; i < src_mat.rows; ++i )  
  //  for ( int j=0; j < src_mat.cols; ++j )  
  //    if (isnan(src_mat.at<float>(i,j)) ) { 
  //      std::cout << "nananananananananan" << j;
  //    }

  //for ( int i=0; i < trgt_mat.rows; ++i )  
  //  for ( int j=0; j < trgt_mat.cols; ++j )  
  //    if (isnan(trgt_mat.at<float>(i,j)) ) std::cout << "nananananananananan";


  std::cout << "\n -t_ICP-- \n";
  std::cout << "rejScale: "<< icp_reject << " numLv: " << icp_levels << "\n";
  std::cout << "srcmat  : row:" << src_mat.rows  <<  " col:" <<  src_mat.cols  << "\n";
  std::cout << "trgtmat : row:" << trgt_mat.rows <<  " col:" <<  trgt_mat.cols << "\n";
  Trace( init_mat );

  init_mat.transposeInPlace();

  cv::ppf_match_3d::Pose3DPtr M1 = new cv::ppf_match_3d::Pose3D();
  M1->updatePose( init_mat.data() );
  std::vector<cv::ppf_match_3d::Pose3DPtr> poses;
  poses.push_back(M1);
  
  
  
  try
  {
    // Create an instance of ICP
    cv::ppf_match_3d::ICP icp( 300, 0.0001f, icp_reject, icp_levels);
    int i = icp.registerModelToScene(src_mat, trgt_mat, poses);
  }
  catch (...)
  {
    std::cout << "*** surfaceの形状または位置が大きく異なるため追跡に失敗しました ***\n";
  }

  EMat4d m;
  m(0, 0) = poses[0]->pose[0]; m(1, 0) = poses[0]->pose[4]; m(2, 0) = poses[0]->pose[8];  m(3, 0) = poses[0]->pose[12];
  m(0, 1) = poses[0]->pose[1]; m(1, 1) = poses[0]->pose[5]; m(2, 1) = poses[0]->pose[9];  m(3, 1) = poses[0]->pose[13];
  m(0, 2) = poses[0]->pose[2]; m(1, 2) = poses[0]->pose[6]; m(2, 2) = poses[0]->pose[10]; m(3, 2) = poses[0]->pose[14];
  m(0, 3) = poses[0]->pose[3]; m(1, 3) = poses[0]->pose[7]; m(2, 3) = poses[0]->pose[11]; m(3, 3) = poses[0]->pose[15];

  std::cout << "result matrix\n";
  Trace( m );
  std::cout << "fin icp\n";

  return m;
}



//compute ICP 
void ModeSegJointTracker::RunICP_all(
    int icplevel, 
    float icpreject)
{
	const int num_frames = ImageCore::GetInst()->GetNumFrames();

  for ( const auto &mesh : m_isosurfaces ) if( mesh.m_vSize == 0 ) {
    ShowMsgDlg_OK( "Iso surfaceが未作成です", "MSG");
    return;
  }
	if( m_cps_bone1.size() < 3 ){ 
    ShowMsgDlg_OK( "frame0のbone1に3個のpointsを置いてください", "MSG");
    return;
  }
	if( m_cps_bone2.size() < 3 ){ 
    ShowMsgDlg_OK( "frame0のbone2に3個以上のpointsを置いてください", "MSG");
    return;
  }

  //extract source matrix (150-ringと100ringを利用)
	cv::Mat mat_src1, mat_src2;
  t_ExtractLinkedVsNs( m_isosurfaces[0], m_cps_bone1[0], 150, m_srcbone1_ids, m_srcbone1);
  t_ExtractLinkedVsNs( m_isosurfaces[0], m_cps_bone2[0], 100, m_srcbone2_ids, m_srcbone2);
  t_SetMat( m_srcbone1, mat_src1);
  t_SetMat( m_srcbone2, mat_src2);

  m_mats_bone1[0].setIdentity();
  m_mats_bone2[0].setIdentity();
	for( int t = 1; t < num_frames; ++t)
  {
		const TMesh &mesh = m_isosurfaces[t];
		cv::Mat trgt = cv::Mat( (int)mesh.m_vSize, 6, CV_32F);
		t_SetMat( mesh, trgt );

	  std::cout << "start registration " << t << "\n";
    m_mats_bone1[t] = 
      t_ICP( icpreject, icplevel, m_mats_bone1[t-1], mat_src1, trgt );
    m_mats_bone2[t] = 
      t_ICP( icpreject, icplevel, m_mats_bone2[t-1], mat_src2, trgt );
	  std::cout << "Done!";
  }
}



//compute ICP 
void ModeSegJointTracker::RunICP_currenrt(
    int icplevel, 
    float icpreject)
{
	const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int frame_idx = formVisParam_getframeI();

  for ( const auto &mesh : m_isosurfaces ) if( mesh.m_vSize == 0 ) {
    ShowMsgDlg_OK( "Iso surfaceが未作成です", "MSG");
    return;
  }
	if( m_cps_bone1.size() < 3 ){ 
    ShowMsgDlg_OK( "frame0のbone1に3個のpointsを置いてください", "MSG");
    return;
  }
	if( m_cps_bone2.size() < 3 ){ 
    ShowMsgDlg_OK( "frame0のbone2に3個以上のpointsを置いてください", "MSG");
    return;
  }
  if ( m_srcbone1.size() == 0 || m_srcbone2.size() == 0 ){
    ShowMsgDlg_OK( "ICP One Frameは\n\
                    一度ICP ALLしてから利用できます", "MSG");
    return;
  }
  if ( frame_idx == 0 ) {
    ShowMsgDlg_OK( "frame 0には適用できません", "MSG" );
    return;
  }

  //extract source matrix (150-ringと100ringを利用)
	cv::Mat mat_src1, mat_src2;
  t_SetMat( m_srcbone1, mat_src1);
  t_SetMat( m_srcbone2, mat_src2);
  
	const TMesh &mesh = m_isosurfaces[frame_idx];
	cv::Mat trgt = cv::Mat( (int)mesh.m_vSize, 6, CV_32F);
	t_SetMat( mesh, trgt );

	std::cout << "start registration " << frame_idx << "\n";
  m_mats_bone1[frame_idx] = 
    t_ICP( icpreject, icplevel, m_mats_bone1[frame_idx], mat_src1, trgt );
  m_mats_bone2[frame_idx] = 
    t_ICP( icpreject, icplevel, m_mats_bone2[frame_idx], mat_src2, trgt );
	std::cout << "Done!";
}








////////////////////////////////////////////////////////////////////
//EVALUATION ///////////////////////////////////////////////////////
void ModeSegJointTracker::EvalObj_Load(
    std::string fname)
{
  m_evalobj.Initialize( fname.c_str() );
  EVec3f gc = m_evalobj.GetGravityCenter();
  m_evalobj.Translate(-gc);
  for ( auto &it : m_mats_evalobj )
  {
    it.setIdentity();
    it(0,3) = gc[0];
    it(1,3) = gc[1];
    it(2,3) = gc[2];
  }
}


void ModeSegJointTracker::EvalObj_Translate(int frameidx, EVec3f trans)
{
  m_mats_evalobj[frameidx](0,3) += trans[0];
  m_mats_evalobj[frameidx](1,3) += trans[1];
  m_mats_evalobj[frameidx](2,3) += trans[2];
}


void ModeSegJointTracker::EvalObj_Rotate (int frameidx, EMat3f rot  )
{
  //平行移動を無視するので，回転成分だけを計算する
  EMat4d &M = m_mats_evalobj[frameidx];
  EMat3f R;
  for ( int i=0; i < 3; ++i ) 
    for ( int j = 0; j < 3; ++j )
      R(i,j) = (float)M(i,j); 

  R = rot * R;

  for ( int i=0; i < 3; ++i ) 
    for ( int j = 0; j < 3; ++j )
      M(i,j) = (double)R(i,j); 
}


void ModeSegJointTracker::EvalObj_DrawDiff (int frameidx )
{
	if ( frameidx < 0 || m_mats_evalobj.size() <= frameidx   ) return;
  if ( m_evalinfo.size() == 0 || m_evalinfo[0].size() == 0 ) return;

	const EMat4d &M    = m_mats_evalobj[frameidx];
	const auto   &info = m_evalinfo    [frameidx];
	const TMesh  &mesh = m_isosurfaces[0];

  glDisable ( GL_LIGHTING );
	glPointSize(4);
	glBegin( GL_POINTS );
	for( const auto& it : info )
	{
		glColor3d  ( 0.5 * it.m_dist, 0, 0 );
		glVertex3fv( it.m_pSrc.data() );
	}
	glEnd();

	if ( IsShiftKeyOn() )
	{
		glColor3d( 0, 1, 1 );
		glLineWidth(1);
		glBegin( GL_LINES);
	  for( const auto& it : info )
		{
			glVertex3fv( it.m_pSrc.data() );
			glVertex3fv( it.m_pTgt.data() );
		}
		glEnd();
	}
}



void ModeSegJointTracker::EvalObj_DrawSurf (int frameidx )
{
  if( m_evalobj.m_vSize ==0 ) return;
  static float spec [4] = {0.9f,0.9f,0.9f,0.2f};
  static float shin [1] = {54.0f};
  static float diff[4]  = {0.2f,0.0f,0.8f,0.2f};
  static float ambi[4]  = {0.2f,0.0f,0.8f,0.2f};  

  glEnable( GL_LIGHTING );
  glPushMatrix( );
  glMultMatrixd( m_mats_evalobj[ frameidx ].data() );
  m_evalobj.Draw(diff, ambi, spec, shin);
  glPopMatrix( );
  glDisable( GL_LIGHTING );
}





static void t_GetNearestVid(
    const  EVec3f &p,
    const  EVec3f &n,
    const  EVec3f *verts,
    const  EVec3f *norms,
    const  std::vector<int> &vids,
    float  &min_distsq,
    int    &min_idx)
{
	for ( const auto &vi : vids )
	{
    if ( n.dot(norms[vi]) <= 0 ) continue;

		float d = DistSq( p, verts[ vi ] );
		if (d < min_distsq)
		{
			min_distsq = d;
			min_idx = vi;
		}
	}
}



//近傍探索 
//src_vsnsに最も近い 
static void t_GetNearestVtxIdx
(
	const EVec3f   cube   ,
  const std::vector<std::pair<EVec3f,EVec3f>> &src_vsns,
  const TMesh &trgtmesh,

	int *vid_src2tgt // size:srcN
)
{
	const int DIV_N = 30;
  std::vector<int> idsets[DIV_N][DIV_N][DIV_N];

	//登録
	for( int i = 0; i < trgtmesh.m_vSize; ++i)
	{
		//C[0] = 50.0, DIV_N = 4 のとき, [0,12.5] -> 0, [12.5,25.0]-->1
    const EVec3f &v = trgtmesh.m_vVerts[i]; 
		int xi = (int) std::min( v[0] / cube[0] * DIV_N, DIV_N-1.0f);  
		int yi = (int) std::min( v[1] / cube[1] * DIV_N, DIV_N-1.0f);  
		int zi = (int) std::min( v[2] / cube[2] * DIV_N, DIV_N-1.0f);  
		idsets[zi][yi][xi].push_back(i);
	}
  
  const EVec3f *tgt_vs = trgtmesh.m_vVerts;
  const EVec3f *tgt_ns = trgtmesh.m_vNorms;
	//検索
	for (int i = 0; i < (int)src_vsns.size(); ++i)
	{
		const EVec3f &p = src_vsns[i].first ;
		const EVec3f &n = src_vsns[i].second;
		int x = (int)std::min( p[0] / cube[0] * DIV_N, DIV_N-1.0f);
		int y = (int)std::min( p[1] / cube[1] * DIV_N, DIV_N-1.0f);
		int z = (int)std::min( p[2] / cube[2] * DIV_N, DIV_N-1.0f);

		//本来は26近傍にすべきだが影響は非常に小さいので近似解を得る目的で6近傍のみ
		float  min_d = FLT_MAX;
		int    min_i = -1;
		               t_GetNearestVid( p,n, tgt_vs, tgt_ns, idsets[z][y][x]  , min_d, min_i);
		if (x>0      ) t_GetNearestVid( p,n, tgt_vs, tgt_ns, idsets[z][y][x-1], min_d, min_i);
		if (y>0      ) t_GetNearestVid( p,n, tgt_vs, tgt_ns, idsets[z][y-1][x], min_d, min_i);
		if (z>0      ) t_GetNearestVid( p,n, tgt_vs, tgt_ns, idsets[z-1][y][x], min_d, min_i);
		if (x<DIV_N-1) t_GetNearestVid( p,n, tgt_vs, tgt_ns, idsets[z][y][x+1], min_d, min_i);
		if (y<DIV_N-1) t_GetNearestVid( p,n, tgt_vs, tgt_ns, idsets[z][y+1][x], min_d, min_i);
		if (z<DIV_N-1) t_GetNearestVid( p,n, tgt_vs, tgt_ns, idsets[z+1][y][x], min_d, min_i);
		
		if ( min_i == -1)
		{
      std::cout << "searchAll- ";
      VertsGetNearestPoint( trgtmesh.m_vSize, trgtmesh.m_vVerts, p, min_i, min_d);
		}
		vid_src2tgt[i] = min_i;
	}
}




//近傍探索   mat * src_vs[i] に最も近い頂点 tgt_vs[idx[i]] を求める
static void t_ExtractVsNs
(
  const TMesh  &mesh, 
	const EMat4d  mat ,
  
  std::vector<std::pair<EVec3f,EVec3f>> &verts_norms
)
{
  verts_norms.resize(mesh.m_vSize);
  for ( int i = 0; i < mesh.m_vSize; ++i )
  {
    verts_norms[i].first  = Mult( mat, mesh.m_vVerts[i]);
    verts_norms[i].second = MultOnlyRot( mat, mesh.m_vNorms[i]);
  }
}





void ModeSegJointTracker::EvalObj_RunAnalysis(
    std::string fname, 
    int icplv, 
    float icpreject 
    )
{
	const int num_frames = (int) m_isosurfaces.size();

	if( m_evalobj.m_vSize == 0 ) {
		ShowMsgDlg_OK( "評価用surfaceを読み込んでください", "MSG");
    return;
  } 
  for( int i=0; i < num_frames; ++i ) if( m_isosurfaces[i].m_vSize == 0 ) {
    ShowMsgDlg_OK( "Iso surfaceが未作成です", "MSG" );
    return;
  }
		
	ShowMsgDlg_OK("評価用surfaceのICP計算と差分計算を行ないます．", "MSG");

	// registration m_f_EvalSurfTrans[0] は初期化すみ
	for( int t = 0; t < num_frames; ++t)
  {
		cv::Mat src, trgt;
    t_SetMat( m_evalobj       , src );
    t_SetMat( m_isosurfaces[t], trgt);
    EMat4d initmat =  (t == 0 ) ? m_mats_evalobj[0] : m_mats_evalobj[t-1];
    m_mats_evalobj[t] = t_ICP( icpreject, icplv, initmat, src, trgt);    
	}


  //compute diff ///////////////////////////////////////////////////
  int W,H,D,WH,WHD;
  std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();
	EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();

	m_evalinfo.clear();
	m_evalinfo.resize(num_frames);

	for (int fi = 0; fi < num_frames; ++fi)
	{
		std::cout << "compute frame" <<  fi << "\n";
    std::vector<std::pair<EVec3f,EVec3f>> src_vsns ;
    t_ExtractVsNs( m_evalobj, m_mats_evalobj[fi], src_vsns);
    
    //precompute nearest vid 		
		int *nearest_vids = new int[ m_evalobj.m_vSize ];
		t_GetNearestVtxIdx( cuboid, src_vsns, m_isosurfaces[fi], nearest_vids );

    //compute nearest points on surf verts
		m_evalinfo[fi].resize(m_evalobj.m_vSize);
		for (int i=0; i < (int)src_vsns.size(); ++i)
		{
			const EVec3f &p = src_vsns[i].first ;

			EVec3f pos;
			double dist;// = DBL_MAX;
			m_isosurfaces[fi].GetDistToPoint( p, nearest_vids[i], pos, dist);
      m_evalinfo[fi][i] = TVtxDistInfo( i, Dist(p, pos), p, pos );

			if( i%50000 == 0 ) 
        std::cout << i << "/" << m_evalobj.m_vSize << "\n";
		}

		delete[] nearest_vids;
		std::cout << "compute " << fi << " frame done!!\n";
	}

  std::ofstream fs(fname);
  fs << "dist data  vtx_num: " << m_evalobj.m_vSize 
     << " frame_Num: " << num_frames << "\n";

	for( int fi = 0; fi < num_frames; ++fi) 
  {
    double mean_dist = 0;
    for ( auto it : m_evalinfo[fi] ) 
      mean_dist += it.m_dist;
    mean_dist /= m_evalinfo[fi].size();
    fs << fi << " " << mean_dist << "\n";
  }
  
  fs.close();
}





void ModeSegJointTracker::ExportCPsInfo( std::string fname )
{
	const int num_frames = ImageCore::GetInst()->GetNumFrames();

  if( m_cps_bone1.size() < 3 || m_cps_bone2.size() < 1 )
  {
    ShowMsgDlg_OK("制御点数が足りません\n\
            肩甲骨に3個，上腕骨に1個以上の制御点を配置してください","Msg");
    return;
  }

  std::ofstream fs(fname);
  fs << "ControlPointInfo\n";

  //data size
  fs << "CP_positions_raw_data\n";
  fs << "isoValue " <<  m_isovalue << "\n";
  fs << "frameNum " <<  num_frames << "\n";

  //control point ()
  for( int f = 0; f < num_frames; ++f)
  {
    fs << "frame[" << f << "] CPonBone1 " << m_cps_bone1.size() << 
                           " CPonBone2 "  << m_cps_bone2.size() << "\n";
	  
    for( const auto cp : m_cps_bone1) WriteToFstream( fs, Mult( m_mats_bone1[f], cp));
	  for( const auto cp : m_cps_bone2) WriteToFstream( fs, Mult( m_mats_bone2[f], cp));
  }

  //cp pos with respect to local coardinate
  fs << "\n\n\n";
  fs << "CP_positions_RELATIVE_COORDINATE\n";

  for( int f = 0; f < num_frames; ++f)
  {
    fs << "frame[" << f << "] CPonBone1 " << m_cps_bone1.size() << 
                           "  CPonBone2 " << m_cps_bone2.size() << "\n";
    //conver cps by matrix     
    EMat4d m1 = m_mats_bone1[f];
    EMat4d m2 = m_mats_bone2[f];
    std::vector<EVec3f> cps1, cps2;
    for ( const auto& p1 : m_cps_bone1) cps1.push_back( Mult( m1, p1) );    
    for ( const auto& p2 : m_cps_bone2) cps2.push_back( Mult( m2, p2) );    

    //gen coordinate system 
    EVec3f orig, cdX, cdZ, cdY;
    t_CalcLocalCoordinate( cps1, cps2, orig, cdX, cdY, cdZ );
    
    for ( const auto &it : cps2)
    {
      EVec3f pt = it - orig;
      fs << pt.dot(cdX) << " " << pt.dot(cdY) << " " << pt.dot(cdZ) << "\n"; 
    }
  }

  fs << "\n\n\n";
  fs << "TransMatInfo\n";
  for( const auto &it: m_mats_bone1 ) WriteToFstream( fs, it); 
  for( const auto &it: m_mats_bone2 ) WriteToFstream( fs, it); 

  fs << "set_point_ids " << m_srcbone1_ids.size() << " " 
                         << m_srcbone2_ids.size() << "\n";

  for( const auto &i : m_srcbone1_ids) fs << i << " ";
  fs << "\n";
  for( const auto &i : m_srcbone2_ids) fs << i << " ";
  fs << "\n";

  fs.close();

  /*
  //for debug 
  for ( int i=0; i < num_frames; ++i)
  {
    EMat4d m = m_mats_bone1[i];
    EMat3d R;
    R(0,0)=m(0,0);  R(0,1)=m(0,1);  R(0,2)=m(0,2);
    R(1,0)=m(1,0);  R(1,1)=m(1,1);  R(1,2)=m(1,2); 
    R(2,0)=m(2,0);  R(2,1)=m(2,1);  R(2,2)=m(2,2);
    std::cout << "det  " << i << " "  << R.determinant();
  }

  for ( int i=0; i < num_frames; ++i)
  {
    EMat4d m = m_mats_bone2[i];
    EMat3d R;
    R(0,0)=m(0,0);  R(0,1)=m(0,1);  R(0,2)=m(0,2);
    R(1,0)=m(1,0);  R(1,1)=m(1,1);  R(1,2)=m(1,2); 
    R(2,0)=m(2,0);  R(2,1)=m(2,1);  R(2,2)=m(2,2);
    std::cout << "det  " << i << " "  << R.determinant();
  }
  */
}



void ModeSegJointTracker::ImportCPsInfo(std::string fname)
{
  std::ifstream fs = std::ifstream(fname);
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

	char buf[1024], buf1[128], buf2[128];
  
	while( fs >> buf )
    if( strcmp( buf, "isoValue") ==0) break;

  //basici information 
	int tmp_num_frames;
  fs >> m_isovalue;
  fs >> buf >> tmp_num_frames;

	if( tmp_num_frames != num_frames )
	{
		std::cout << buf << " " << tmp_num_frames;
		ShowMsgDlg_OK( "読み込み4dCTとロード中データのフレーム数が不一致", "Msg");
		return;
	}
	this->IsoSurf_Generate( m_isovalue );
	

  //control point info 
  for( int f = 0; f < num_frames; ++f)
  {		
    int num_cp1, num_cp2;
    fs >> buf >> buf1 >> num_cp1 >> buf2 >> num_cp2;
    std::cout << buf << buf1 << buf2 << " " << num_cp1 << " " << num_cp2 << "\n";
    
    if ( f == 0 ) { 
      m_cps_bone1.resize( num_cp1 );
      m_cps_bone2.resize( num_cp2 );
      for( int i = 0; i < num_cp1; ++i) ReadFromFstream( fs, m_cps_bone1[i] );
      for( int i = 0; i < num_cp2; ++i) ReadFromFstream( fs, m_cps_bone2[i] );
    } else{
      //読み捨て
      EVec3f tmp;
      for( int i = 0; i < num_cp1; ++i) ReadFromFstream( fs, tmp );
      for( int i = 0; i < num_cp2; ++i) ReadFromFstream( fs, tmp );
    }
  }

  
	//CPs in Relative coordinate 読み捨て
  fs >> buf;
  std::cout << "--------------------" << buf << "\n";
  for( int f = 0; f < num_frames; ++f)
  {		
		int num1, num2;
    fs >> buf >> buf1 >> num1 >> buf2 >> num2;
    EVec3f tmp;        
		for( int i = 0; i < num2; ++i) ReadFromFstream(fs, tmp);
  }

  // TransMatInfo 
  fs >> buf;
  std::cout << "--------------------" << buf << "\n";
	m_mats_bone1.resize(num_frames);
	m_mats_bone2.resize(num_frames);
  for( int f = 0; f < num_frames; ++f) ReadFromFstream(fs, m_mats_bone1[f]);
  for( int f = 0; f < num_frames; ++f) ReadFromFstream(fs, m_mats_bone2[f]);


	int n1, n2;
  fs >> buf >> n1 >> n2;
  std::cout << "--------------------" << buf << "\n";
	m_srcbone1_ids.resize(n1);
	m_srcbone2_ids.resize(n2);

	for( int i=0; i < n1; ++i) fs >> m_srcbone1_ids[i];
	for( int i=0; i < n2; ++i) fs >> m_srcbone2_ids[i];

  //gen srcbone1 srcbone2;
  TMesh &mesh = m_isosurfaces.front();
  m_srcbone1.clear();
  m_srcbone1.reserve(n1);
	for(auto it : m_srcbone1_ids )
  {
    if ( it >= mesh.m_vSize ) { 
      std::cout  << "strange!!";
      break;
    }
    m_srcbone1.push_back(std::make_pair( mesh.m_vVerts[it],mesh.m_vNorms[it]) );
  }
  m_srcbone2.clear();
  m_srcbone2.reserve(n2);
	for(auto it : m_srcbone2_ids )
  {
    if ( it >= mesh.m_vSize ) { 
      std::cout  << "strange!!";
      break;
    }
    m_srcbone2.push_back(std::make_pair( mesh.m_vVerts[it], mesh.m_vNorms[it]) );
  }
  
  fs.close();

  if ( ShowMsgDlgYesNo( "このデータは旧SJTrackerで作成されたものですか?", "Msg") )
  {
    for ( auto& m: m_mats_bone1) m.transposeInPlace();
    for ( auto& m: m_mats_bone2) m.transposeInPlace();
  }

}
