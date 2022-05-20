#pragma unmanaged
#include "ModeSegParallelWires.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "CrsSecCore.h"
#include "kcurves.h"
#include <fstream>
#include "../ModeCore.h"

#pragma managed
#include "FormSegParallelWires.h"
#include "FormMain.h"
#include "FormVisParam.h"
#include "climessagebox.h"

#pragma unmanaged


// 0. render volume and planes OK 
// 1. camera rotation          OK
// 2. mouse wheel and key to move target plane OK
// 3. render cps and wires OK
// 4. place/move/delete control point OK  
// 5. insert cp OK
// 6. fill inside (Finish seg) OK
// 7. save / load wires  OK 
// 8. show all wires / modify the size of cps OK
// 9. Lock Pitch Modification OK
//10. show the other mask     OK
//11. test 
//12. manual         TODO ---------

//TODO 
// File Export a ROI as OBJ
// File Export a ROI as Binary Bmp
// File Export a MASK as BMP

using namespace RoiPainter3D;

#define PW_CPSIZE_MIN 1.0f
#define PW_CPSIZE_MAX 6.0f


ModeSegParallelWires::ModeSegParallelWires() : 
  m_volume_shader("shader/volVtx.glsl"   , "shader/volFlg.glsl"),
  m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_ParallelWires.glsl")
{
  std::cout << "ModeSegParallelWires constructure...\n";

  m_bL = m_bR = m_bM = false;
  m_draging_cpid << -1, -1;

  std::cout << "ModeSegParallelWires done\n";
}



ModeSegParallelWires::~ModeSegParallelWires()
{
}



// このモードでは plane をちょうど画素中心を通る位置に配置したい 
//
// D = 5なら
// plane_xyは，0.1, 0.3, 0.5, 0.7, 0.9 に配置したい
// position = 

void ModeSegParallelWires::StartMode ()
{
  std::cout << "ModeSegParallelWires...startMode----------\n";

  m_bL = m_bR = m_bM = false;
  m_draging_cpid << -1, -1;
  FormSegParallelWires_Show();

  //initialize wires
  int W, H, D;
  std::tie(W,H,D) =ImageCore::GetInst()->GetResolution3();
  
  m_wires_xy.clear();
  m_wires_yz.clear();
  m_wires_zx.clear();

  m_wires_xy.resize(D, SplineWire(SplineWire::PLANE_XY) );
  m_wires_yz.resize(W, SplineWire(SplineWire::PLANE_YZ) );
  m_wires_zx.resize(H, SplineWire(SplineWire::PLANE_ZX) );
  
  //x
  m_planexy_idx = D / 2;
  m_planeyz_idx = W / 2;
  m_planezx_idx = H / 2;
  
  float xy_pos = (m_planexy_idx + 0.5f) * (1.0f / D);
  float yz_pos = (m_planeyz_idx + 0.5f) * (1.0f / W);
  float zx_pos = (m_planezx_idx + 0.5f) * (1.0f / H);
  CrssecCore::GetInst()->SetPlaneXyPosition( xy_pos );
  CrssecCore::GetInst()->SetPlaneYzPosition( yz_pos );
  CrssecCore::GetInst()->SetPlaneZxPosition( zx_pos );
  
  CrssecCore::GetInst()->ClearCurvedCrossec();

  EVec3f pitch  = ImageCore::GetInst()->GetPitch();
  float  base_r = (pitch[0] + pitch[1] + pitch[2]) / 3.0f;
  float r = PW_CPSIZE_MIN + (PW_CPSIZE_MAX - PW_CPSIZE_MIN) * 0.5;
  SplineWire::SetCtrlPtRadius(r * base_r);    

  //Lock/Unlock pitch box
  formVisParam_LockPitchBox();
  formVisParam_SetPlanesCheck(false, false, false);

  std::cout << "ModeSegParallelWires...startMode DONE-----\n";
}


void ModeSegParallelWires::SetControlPointSize(float size_01)
{
  EVec3f pitch  = ImageCore::GetInst()->GetPitch();
  float  base_r = (pitch[0] + pitch[1] + pitch[2]) / 3.0f;
  float r = PW_CPSIZE_MIN + (PW_CPSIZE_MAX - PW_CPSIZE_MIN) * size_01;
  SplineWire::SetCtrlPtRadius(r * base_r);    
}







bool ModeSegParallelWires::CanLeaveMode()
{
  bool is_wire_exist = false;

  for ( const auto wire : m_wires_xy )
  {
    if ( wire.GetNumCtrlPts() != 0 ) is_wire_exist = true;
    if ( is_wire_exist ) break;
  }
  for ( const auto wire : m_wires_yz )
  {
    if ( wire.GetNumCtrlPts() != 0 ) is_wire_exist = true;
    if ( is_wire_exist ) break;
  }
  for ( const auto wire : m_wires_zx )
  {
    if ( wire.GetNumCtrlPts() != 0 ) is_wire_exist = true;
    if ( is_wire_exist ) break;
  }  
  
  if ( !is_wire_exist ) return true;	

  if (CLI_MessageBox_YESNO_Show("Current Wires will be trushed. Do you want to leave?", "message") )
	{
		return true;
	}

	return false;
}




CRSSEC_ID ModeSegParallelWires::GetCurrentTrgtPlane()
{
  return FormParallelWires_bPlaneXY() ? CRSSEC_XY : 
         FormParallelWires_bPlaneYZ() ? CRSSEC_YZ : CRSSEC_ZX; 
}



void ModeSegParallelWires::FinishSegmentation()
{
  //Fill in the curve
  const EVec3i reso  = ImageCore::GetInst()->GetResolution();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();
  const EVec3f cube  = ImageCore::GetInst()->GetCuboid();
  

  //initialize ImageCore::m_vol_flg
  ImageCore::GetInst()->InitializeVolFlgByLockedMask();

  byte *vol_flg = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();

  for ( const auto&w : m_wires_xy ) {
    if ( w.GetNumCtrlPts() >= 3 && w.GetCurve().size() > 0 ) 
      t_AddPixsInsideLasso(CRSSEC_XY, reso, pitch, w.GetCurve(), true, vol_flg);
  }
  for ( const auto&w : m_wires_yz ) { 
    if ( w.GetNumCtrlPts() >= 3 && w.GetCurve().size() > 0 ) 
      t_AddPixsInsideLasso(CRSSEC_YZ, reso, pitch, w.GetCurve(), true, vol_flg);
  }
  for ( const auto&w : m_wires_zx ) { 
    if ( w.GetNumCtrlPts() >= 3 && w.GetCurve().size() > 0 ) 
      t_AddPixsInsideLasso(CRSSEC_ZX, reso, pitch, w.GetCurve(), true, vol_flg);
  }


  //precheck the foreground pixel
	if ( !ImageCore::GetInst()->bForeVoxelsExistInVolFlg() ) 
	{
		CLI_MessageBox_OK_Show("No foreground region exist", "message");
		return;
	}

  m_wires_xy.clear();
  m_wires_yz.clear();
  m_wires_zx.clear();
  
  ImageCore::GetInst()->StoreForegroundAsNewMask();
  ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
	RedrawScene();	
}




void ModeSegParallelWires::CancelSegmentation()
{
  if ( !CLI_MessageBox_YESNO_Show("Do you want leave this mode?", "dlg.") )
    return; 

  //wireを削除（しないと、canEndModeでダイアログが表示される)
  m_wires_xy.clear();
  m_wires_yz.clear();
  m_wires_zx.clear();

	ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
	RedrawScene();	
}






//-------------------------------------------------------------  
// Mouse Listener ---------------------------------------------
void ModeSegParallelWires::LBtnDclk  (const EVec2i &p, OglForCLI *ogl){}
void ModeSegParallelWires::RBtnDclk  (const EVec2i &p, OglForCLI *ogl){}
void ModeSegParallelWires::MBtnDclk  (const EVec2i &p, OglForCLI *ogl){}


void ModeSegParallelWires::LBtnDown  (const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;
    
  if ( IsShiftKeyOn() )
  {
    EVec3f rayp, rayd, pos;
    ogl->GetCursorRay( p, rayp, rayd);
    
    bool bxy = FormParallelWires_bPlaneXY();
    bool byz = FormParallelWires_bPlaneYZ();
    bool bzx = FormParallelWires_bPlaneZX();
    auto &wires    = bxy ? m_wires_xy    : byz ? m_wires_yz    : m_wires_zx   ; 
    auto  wireidx  = bxy ? m_planexy_idx : byz ? m_planeyz_idx : m_planezx_idx;
    auto  crssecid = bxy ? CRSSEC_XY     : byz ? CRSSEC_YZ     : CRSSEC_ZX    ;

    //1) pick control point, 2) pick plane
    int pidx = wires[ wireidx ].PickCtrlPt( rayp, rayd);
    if ( pidx != -1 )
    {
      m_draging_cpid << wireidx, pidx;
    }
    else if ( PickCrsSec( crssecid, rayp, rayd, &pos ) != CRSSEC_NON )
    {
      int idx = wires[ wireidx ].AddCtrlPt( pos );
      m_draging_cpid << wireidx, idx;
    }

    RedrawScene();
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }
  
}


void ModeSegParallelWires::RBtnDown  (const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
    
  if ( IsShiftKeyOn() )
  {
    EVec3f rayp, rayd, pos;
    ogl->GetCursorRay( p, rayp, rayd );
    
    if ( FormParallelWires_bPlaneXY() ) 
    {
      m_wires_xy[ m_planexy_idx ].PickToEraseCtrlPt( rayp, rayd );
    }
    if ( FormParallelWires_bPlaneYZ() ) 
    {
      m_wires_yz[ m_planeyz_idx ].PickToEraseCtrlPt( rayp, rayd );
    }
    if ( FormParallelWires_bPlaneZX() ) 
    {
      m_wires_zx[ m_planezx_idx ].PickToEraseCtrlPt( rayp, rayd );
    }
    RedrawScene();
  }
  else
  {
    ogl->BtnDown_Rot(p);
  }
}



void ModeSegParallelWires::MBtnDown  (const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
    
  if ( IsShiftKeyOn() )
  {
    EVec3f rayp, rayd, pos;
    ogl->GetCursorRay( p, rayp, rayd );
    
    if ( FormParallelWires_bPlaneXY() ) 
    {
      m_wires_xy[ m_planexy_idx ].PickToEraseCtrlPt( rayp, rayd );
    }
    if ( FormParallelWires_bPlaneYZ() ) 
    {
      m_wires_yz[ m_planeyz_idx ].PickToEraseCtrlPt( rayp, rayd );
    }
    if ( FormParallelWires_bPlaneZX() ) 
    {
      m_wires_zx[ m_planezx_idx ].PickToEraseCtrlPt( rayp, rayd );
    }

    RedrawScene();

  }
  else
  {
    ogl->BtnDown_Zoom(p);
  }
}



void ModeSegParallelWires::LBtnUp    (const EVec2i &p, OglForCLI *ogl)
{
  m_bL = false;
  m_draging_cpid << -1, -1;
  ogl->BtnUp();
}


void ModeSegParallelWires::RBtnUp    (const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  ogl->BtnUp();
}

void ModeSegParallelWires::MBtnUp    (const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
}


void ModeSegParallelWires::MouseMove (const EVec2i &p, OglForCLI *ogl)
{
  if ( !m_bL && !m_bR && !m_bM)  return;

  if ( m_draging_cpid[0] != -1 )
  {    
    auto crssec_id = FormParallelWires_bPlaneXY() ? CRSSEC_XY : 
                     FormParallelWires_bPlaneYZ() ? CRSSEC_YZ : CRSSEC_ZX;

    auto &wires    = FormParallelWires_bPlaneXY() ? m_wires_xy : 
                     FormParallelWires_bPlaneYZ() ? m_wires_yz : m_wires_zx;

	  EVec3f rayp, rayd, pos;
	  ogl->GetCursorRay( p, rayp, rayd);

    if ( PickCrsSec( crssec_id, rayp, rayd, &pos ) != CRSSEC_NON )
    {
      wires[ m_draging_cpid[0] ].MoveCtrlPt( m_draging_cpid[1], pos );
    }
  }
  else
  {
		ogl->MouseMove(p);
  }

  RedrawScene(false);

}




void ModeSegParallelWires::MouseWheel(
    const EVec2i &p, 
    short zDelta, 
    OglForCLI *ogl)
{
	EVec3f ray_pos, ray_dir, pos;
	ogl->GetCursorRay( p, ray_pos, ray_dir);
  
  const bool b_xy = formVisParam_bPlaneXY() || FormParallelWires_bPlaneXY();
  const bool b_yz = formVisParam_bPlaneYZ() || FormParallelWires_bPlaneYZ();
  const bool b_zx = FormVisParam_bPlaneZX() || FormParallelWires_bPlaneZX();
  CRSSEC_ID trgt_id = PickCrsSec(ray_pos, ray_dir, &pos, b_xy, b_yz, b_zx);

  int W,H,D;
  std::tie(W,H,D) = ImageCore::GetInst()->GetResolution3();
  
  if ( trgt_id == CRSSEC_XY )
  {
    m_planexy_idx += (zDelta>0)?1:-1;
    m_planexy_idx = t_crop<int>(0, D-1,m_planexy_idx);

    float xy_pos = (m_planexy_idx + 0.5f) * (1.0f / D);
    CrssecCore::GetInst()->SetPlaneXyPosition( xy_pos );
  } 
  else if (trgt_id == CRSSEC_YZ) 
  { 
    m_planeyz_idx += (zDelta>0)?1:-1;
    m_planeyz_idx = t_crop<int>(0, W-1,m_planeyz_idx);

    float yz_pos = (m_planeyz_idx + 0.5f) * (1.0f / W);
    CrssecCore::GetInst()->SetPlaneYzPosition( yz_pos );
  } 
  else if (trgt_id == CRSSEC_ZX)
  {
    m_planezx_idx += (zDelta>0)?1:-1;
    m_planezx_idx = t_crop<int>(0, H-1,m_planezx_idx);

    float zx_pos = (m_planezx_idx + 0.5f) * (1.0f / H);
    CrssecCore::GetInst()->SetPlaneZxPosition( zx_pos );
  } 

  RedrawScene();
}


// Mouse Listener ---------------------------------------------
//-------------------------------------------------------------  





void ModeSegParallelWires::KeyDown(int nChar)
{
  if ( nChar == 38 /* up */ || nChar == 40 /* down */) 
  {
    int step = (nChar == 38) ? + 1: -1;
 
    int W,H,D;
    std::tie(W,H,D) = ImageCore::GetInst()->GetResolution3();
  
    if ( FormParallelWires_bPlaneXY() )
    {
      m_planexy_idx += step;
      m_planexy_idx = t_crop<int>(0, D-1,m_planexy_idx);

      float xy_pos = (m_planexy_idx + 0.5f) * (1.0f / D);
      CrssecCore::GetInst()->SetPlaneXyPosition( xy_pos );
    } 
    else if ( FormParallelWires_bPlaneYZ() ) 
    { 
      m_planeyz_idx += step;
      m_planeyz_idx = t_crop<int>(0, W-1,m_planeyz_idx);

      float yz_pos = (m_planeyz_idx + 0.5f) * (1.0f / W);
      CrssecCore::GetInst()->SetPlaneYzPosition( yz_pos );
    } 
    else if ( FormParallelWires_bPlaneZX() )
    {
      m_planezx_idx += step;
      m_planezx_idx = t_crop<int>(0, H-1,m_planezx_idx);

      float zx_pos = (m_planezx_idx + 0.5f) * (1.0f / H);
      CrssecCore::GetInst()->SetPlaneZxPosition( zx_pos );
    } 
    RedrawScene();
  }

}

void ModeSegParallelWires::KeyUp  (int nChar){}




void ModeSegParallelWires::DrawScene(
    const EVec3f &cuboid, 
    const EVec3f &camP, 
    const EVec3f &camF)
{
  //bind volumes ---------------------------------------
  ImageCore::GetInst()->UpdateOGLMaskColorImg();
  BindAllVolumes();

  const EVec3i reso = ImageCore::GetInst()->GetResolution();

  //if (m_bDrawStr) t_DrawPolyLine(EVec3f(1,1,0), 3, m_stroke);

  //draw wires (ctrlpt and curve)
  EVec3f offset_xy( 0, 0, 0.05f * cuboid[2] / reso[2]);
  EVec3f offset_yz( 0.05f * cuboid[2] / reso[0], 0, 0);
  EVec3f offset_zx( 0, 0.05f * cuboid[2] / reso[1], 0);

  EVec3f ray_dir = camP - camF;
  if ( ray_dir[0] < 0 ) offset_yz[0] *= -1;
  if ( ray_dir[1] < 0 ) offset_zx[1] *= -1;
  if ( ray_dir[2] < 0 ) offset_xy[2] *= -1;
  
  static EVec3f cyan(0,   1, 1   );
  static EVec3f red (1,0.2f, 0.2f);
  static EVec3f ofstzero(0,0,0);
  if ( FormParallelWires_bDrawAllWires() && !IsSpaceKeyOn() )
  {
    for ( const auto& w : m_wires_xy) w.DrawWire( ofstzero, cyan, 2 );  
    for ( const auto& w : m_wires_yz) w.DrawWire( ofstzero, cyan, 2 );  
    for ( const auto& w : m_wires_zx) w.DrawWire( ofstzero, cyan, 2 );  
  }
  
  //draw current target wire
  glCullFace( GL_BACK );
  if ( FormParallelWires_bPlaneXY() && !IsSpaceKeyOn() ) {
    m_wires_xy[m_planexy_idx].DrawWire  ( offset_xy, red, 5 );  
    m_wires_xy[m_planexy_idx].DrawCtrlPt( );  
  }  
  if ( FormParallelWires_bPlaneYZ() && !IsSpaceKeyOn() ){
    m_wires_yz[m_planeyz_idx].DrawWire  ( offset_yz, red, 5 );  
    m_wires_yz[m_planeyz_idx].DrawCtrlPt( );  
  }  
  if ( FormParallelWires_bPlaneZX() && !IsSpaceKeyOn() ){
    m_wires_zx[m_planezx_idx].DrawWire  ( offset_zx, red, 5 );  
    m_wires_zx[m_planezx_idx].DrawCtrlPt( );  
  }  
  

  DrawCrossSections(cuboid, reso, m_crssec_shader);


  if (formVisParam_bRendVol() && !IsShiftKeyOn() )
  {
    const bool  b_pse   = formVisParam_bDoPsued();
    const bool  b_roi   = formVisParam_GetOtherROI();
    const float alpha   = formVisParam_getAlpha();
    const bool  b_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int   n_slice = (int)((b_manip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());

    glDisable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    m_volume_shader.Bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, b_pse, b_roi );
    t_DrawCuboidSlices( n_slice, camP, camF, cuboid);
    m_volume_shader.Unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }
}



//------------------------------------------------------------------------------

void ModeSegParallelWires::ImportWireInfo(std::string fname)
{
  std::ifstream ifs( fname.c_str() );
 
  if (!ifs)
  {
    std::cout << "failed to open the file " << fname << "\n";
  }
 
  EVec3i reso  = ImageCore::GetInst()->GetResolution();
  EVec3f pitch = ImageCore::GetInst()->GetPitch();

  std::string buf;
  EVec3i r;
  EVec3f p;
  ifs >> buf;
  ifs >> r[0] >> r[1] >> r[2];
  ifs >> p[0] >> p[1] >> p[2];

  if ( r[0] != reso[0] || r[1] != reso[1] || r[2] != reso[2])  
  {
    std::cout << "image size does not coincide... error ...";
    return ;
  }

  ImageCore::GetInst()->SetPitch( p );

  for ( auto& wire : m_wires_xy ) wire.importCtrlPtInfo( ifs );
  for ( auto& wire : m_wires_yz ) wire.importCtrlPtInfo( ifs );
  for ( auto& wire : m_wires_zx ) wire.importCtrlPtInfo( ifs );

  ifs >> buf; 
  std::cout << buf << "\n";
  ifs.close();
  
  formVisParam_initAllItemsForNewImg();
}



void ModeSegParallelWires::ExportWireInfo(std::string fname)
{
  std::ofstream ofs( fname.c_str() );
 
  if (!ofs)
  {
    std::cout << "failed to open the file " << fname << "\n";
  }
 
  EVec3i reso  = ImageCore::GetInst()->GetResolution();
  EVec3f pitch = ImageCore::GetInst()->GetPitch();
  
  ofs << "wire_info\n";
  ofs << reso[0]  << " " << reso[1]  << " " << reso[2]  << "\n";
  ofs << pitch[0] << " " << pitch[1] << " " << pitch[2] << "\n";
  
  //xy 
  for ( const auto& wire : m_wires_xy ) wire.exportCtrlPtInfo( ofs );
  for ( const auto& wire : m_wires_yz ) wire.exportCtrlPtInfo( ofs );
  for ( const auto& wire : m_wires_zx ) wire.exportCtrlPtInfo( ofs );
  
  ofs << "wire_info_fin\n";

  ofs.close();
}




//---------------------------------------------------------------------------//
//--------SplineWire---------------------------------------------------------//
float SplineWire::m_cp_radius = 1.0f;


SplineWire::SplineWire( PLANE_ID plane_id ) : 
  m_plane_id( plane_id )
{
  m_cps.clear();
  m_curve.clear();
}


//retern idx where the point was inserted
int SplineWire::AddCtrlPt (const EVec3f &p)
{
  //find insert position 
  int   found_idx = -1;
  float found_dist = FLT_MAX;
  for ( int i = 1; i < (int)m_cps.size(); ++i )
  { 
    //中心 : 0.5 * ( m_cp[i-1] + m_cp[i] ) 
    //半径 : 0.5 * | m_cp[i-1] - m_cp[i] |
    //の円に入っていれば i-1 ～ i の間に挿入
    //複数の円に入るのであれば最も近い線分に挿入
    
    EVec3f c = 0.5f * (m_cps[i-1] + m_cps[i]);
    float  r = 0.5f * (m_cps[i-1] - m_cps[i]).norm(); 
    float dist = (p - c).norm();
    if ( dist < r && dist < found_dist) 
    {
      found_dist = dist;
      found_idx = i;
    }
  }  
  
  if ( found_idx == -1 )
  {
    m_cps.push_back(p);
    UpdateCurveFromCPs();
    return (int)m_cps.size()-1;
  }
  else
  {
    m_cps.insert( m_cps.begin() + found_idx, p);
    UpdateCurveFromCPs();
    return found_idx;
  }

}



void SplineWire::MoveCtrlPt(const int idx, const EVec3f &p)
{
  if ( idx < 0 || m_cps.size() <= idx ) return;
  m_cps[ idx ] = p;
  UpdateCurveFromCPs();
}



int SplineWire::PickCtrlPt( const EVec3f &ray_pos, const EVec3f &ray_dir)
{
  for ( int i = 0; i < (int)m_cps.size(); ++i)
  {
    if( t_DistRayAndPoint( ray_pos, ray_dir, m_cps[i] ) < m_cp_radius ) 
      return i;
  }
  return -1;
}



void SplineWire::PickToEraseCtrlPt( const EVec3f &ray_pos, const EVec3f &ray_dir)
{
  int idx = PickCtrlPt(ray_pos, ray_dir);
  
  if ( idx != -1 ) 
  {
    m_cps.erase( m_cps.begin() + idx );  
  }

  UpdateCurveFromCPs();
}




void SplineWire::UpdateCurveFromCPs()
{
  m_curve.clear();
  if( m_cps.size() < 3 ) return;
  
  //convert point 3D --> 2D 
  std::vector<EVec2d> cps_2d ( m_cps.size() ) ;
  if ( m_plane_id == PLANE_ID::PLANE_XY ) 
  {
    for ( int i = 0; i < (int) m_cps.size(); ++i)
      cps_2d[i] << m_cps[i][0], m_cps[i][1];   
  }
  else if ( m_plane_id == PLANE_ID::PLANE_YZ ) 
  {
    for ( int i = 0; i < (int) m_cps.size(); ++i)
      cps_2d[i] << m_cps[i][1], m_cps[i][2];   
  }
  else 
  {
    for ( int i = 0; i < (int) m_cps.size(); ++i)
      cps_2d[i] << m_cps[i][2], m_cps[i][0];
  }

  //prepare num of sampling points
  EVec3f pitch        = ImageCore::GetInst()->GetPitch();
  float  ave_pitch    = (pitch[0] + pitch[1] + pitch[2]) / 3.0f;
  float  total_length = t_verts_Length( m_cps, true );
  int    num_samples  = (int)( total_length / ave_pitch / 10.0);
  if ( num_samples < 15 ) num_samples = 15; 

  //compute curve
  std::vector<EVec2d> new_cps, curve_2d;
  compute_kCurves( cps_2d, num_samples, new_cps, curve_2d);
    

  //convert curve 3D --> 2D 
  m_curve.resize( curve_2d.size() );
  if ( m_plane_id == PLANE_ID::PLANE_XY ) 
  {
    float z = m_cps.front()[2];
    for ( int i = 0; i < (int) curve_2d.size(); ++i)
      m_curve[i] << (float)curve_2d[i][0], (float)curve_2d[i][1], z;
  }
  else if ( m_plane_id == PLANE_ID::PLANE_YZ ) 
  {
    float x = m_cps.front()[0];
    for ( int i = 0; i < (int) curve_2d.size(); ++i)
      m_curve[i] << x, (float)curve_2d[i][0], (float)curve_2d[i][1];
  }
  else 
  {
    float y = m_cps.front()[1];
    for ( int i = 0; i < (int) curve_2d.size(); ++i)
      m_curve[i] << (float)curve_2d[i][1], y, (float)curve_2d[i][0];
  }
}



const static float diff_g[4] = { 0.5f, 1.0f, 0.5f, 0.3f };
const static float ambi_g[4] = { 0.5f, 1.0f, 0.5f, 0.3f };
const static float diff_r[4] = { 1.0f, 0.2f, 0, 0.3f };
const static float ambi_r[4] = { 1.0f, 0.2f, 0, 0.3f };
const static float spec[4] = { 1,1,1,0.3f };
const static float shin[1] = { 64.0f };


void SplineWire::DrawCtrlPt( ) const
{
  glEnable( GL_LIGHTING );

  for ( int i=0; i < (int)m_cps.size(); ++i)
  { 
    const EVec3f &p = m_cps[i];
    const float  &r = m_cp_radius;
    if ( i == 0 || i == m_cps.size() -1 ) 
      TMesh::DrawIcosaHedron( p, r, diff_g, ambi_g, spec, shin );
    else
      TMesh::DrawIcosaHedron( p, r, diff_r, ambi_r, spec, shin );
  }
  glDisable( GL_LIGHTING );

}


void SplineWire::DrawWire(
    const EVec3f &offset, 
    const EVec3f &color, 
    float width) const
{
  if ( m_curve.size() < 2 ) return ;
  
  glDisable( GL_LIGHTING );
  glTranslated( offset[0], offset[1], offset[2]);
  t_DrawPolyLine ( color, width, m_curve, true );
  glTranslated(-offset[0],-offset[1],-offset[2]);
}




void SplineWire::exportCtrlPtInfo(std::ofstream &ofs) const
{
  ofs << (int) m_cps.size() << "\n";
  for ( int i=0; i < (int) m_cps.size(); ++i)
    ofs << m_cps[i][0] << " " << m_cps[i][1] << " " << m_cps[i][2] << "\n";
}


void SplineWire::importCtrlPtInfo(std::ifstream &ifs) 
{
  m_curve.clear();
  m_cps.clear();

  int n;
  ifs >> n;
  m_cps.resize(n);

  for ( int i=0; i < (int) m_cps.size(); ++i)
    ifs >> m_cps[i][0] >> m_cps[i][1] >> m_cps[i][2];
  
  UpdateCurveFromCPs();
}