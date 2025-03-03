#pragma unmanaged
#include "ModeSegRigidICP.h"
#include "ModeCommonTools.h"
#include "ModeCore.h"
#include "ImageCore.h"
#include "CrsSecCore.h"

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegRigidICP.h"
#include "CliMessageBox.h"
#pragma unmanaged

//Common and stl
#include "tmath.h"
#include "tmesh.h"
#include "thandle3d.h"
#include "tmarchingcubes.h"
#include <chrono>
#include <fstream>

//openCV
#include "opencv/cv.h"
#include "opencv2/surface_matching.hpp"
#include "opencv2/surface_matching/ppf_helpers.hpp"
#include "opencv2/core/utility.hpp"

#pragma comment(lib,"opencv_core310.lib")
#pragma comment(lib,"opencv_surface_matching310.lib")

using namespace RoiPainter4D;
using namespace marchingcubes;

// 1. rotate defout volume  OK
// 2. save icp matrix value OK
// 3. load icp matrix value OK
// 4. rot pivot translation OK

ModeSegRigidICP::~ModeSegRigidICP()
{
}



ModeSegRigidICP::ModeSegRigidICP() 
{
  std::cout << "ModeSegRigidICP...\n";

  m_bL = m_bR = m_bM = false;
  m_b_modified = false;
  m_b_draw_stroke = false;

  std::cout << "ModeSegRigidICP done\n";
}


bool ModeSegRigidICP::CanEndMode()
{
  if (!m_b_modified) return true;
  return ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Leaving?");
}



void ModeSegRigidICP::FinishSegmentation(const bool b_storeallframes)
{
  const int num_frame = ImageCore::GetInst()->GetNumFrames();
  const int num_voxel = ImageCore::GetInst()->GetNumVoxels();

  if(b_storeallframes)
  {
    FillInMesh(0, num_frame - 1);
  }
  else
  {
    const int frame_idx = formVisParam_getframeI();

#pragma omp parallel for
    for (int fi = 0; fi < num_frame; ++fi)
    {
      if ( fi != frame_idx) 
        memset( ImageCore::GetInst()->m_flg4d[fi], 0, sizeof(byte)*num_voxel );
    }
    FillInMesh(frame_idx, frame_idx);
  }

  bool b_fore_exist = false;

  for (int fi = 0; fi < num_frame && !b_fore_exist; ++fi)
  {
    byte* flg3d = ImageCore::GetInst()->m_flg4d[fi];
    for (int i = 0; i < num_voxel && !b_fore_exist; ++i)
    {
      if (flg3d[i] == 255) b_fore_exist = true;
    }
  }

  if ( !b_fore_exist )
  {
    ShowMsgDlg_OK(MESSAGE_NO_FOREGROUND, "no foreground");
    return;
  }

  ImageCore::GetInst()->mask_storeCurrentForeGround();

  //clear rigid tranformation results 
  m_icpmats.clear();
  m_source_surface.Clear();

  //NOTE: do not clear "m_isovalue and m_isosurfaces"

  //このタイミングでisModified = false (しないとcanEndModeでダイアログが表示される)
  m_b_modified = false;

  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  formMain_RedrawMainPanel();
}



void ModeSegRigidICP::CancelSegmentation()
{
  if (!m_b_modified || ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "cancel?"))
  {
    m_icpmats.clear();
    m_source_surface.Clear();

    //NOTE: do not clear (m_isovalue and m_isosurfaces)
    //for ( auto& i : m_isosurfaces) i.Clear();
    //m_isosurfaces.clear();

    m_b_modified = false;
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    formMain_RedrawMainPanel();
  }
}




// flg4d[f][i] : 0背景（変化させてはダメ）, 1背景(変化可能)，255前景 
void ModeSegRigidICP::StartMode()
{
  //init ui flags
  m_bL = m_bR = m_bM = false;
  m_b_trans_srcsurf  = false;
  m_b_rot_srcsurf    = false;
  m_b_trans_pivot    = false;
  m_b_modified = false;
  m_b_show_source_and_iso_surface = true;

  //show dialog
  FormSegRigidICPShow();
  FormSegRigidICPInitializeParameters();

  //initialize vFlg
  ImageCore::GetInst()->InitializeFlg4dByMask( formMain_SetProgressValue );

  //recycle previous iso surface?????
  const int num_frame = ImageCore::GetInst()->GetNumFrames();

  bool prev_isosurf_available = (num_frame == m_isosurfaces.size()) ? true : false;
  for( int i= 0 ; i < m_isosurfaces.size(); ++i)
  {
    if( m_isosurfaces[i].m_num_triangles == 0 ){
      prev_isosurf_available = false;
      break;
    }
  }

  if( prev_isosurf_available && 
    ShowMsgDlgYesNo("前回のiso surfaceを再利用できます．再利用しますか？", "message")  )
  {
    FormSegRigidICP_ReusePrevIsoSurface(m_isovalue);
  }
  else 
  {
    m_isovalue = 0;
    m_isosurfaces.clear();
    m_isosurfaces.resize(num_frame);
  }

  m_source_surface.Clear();  
  m_icpmats.clear();
  m_icpmats.resize(num_frame, EMat4d::Identity());

  //4D volume (cpu) --> vis volume (gpu)
  UpdateImageCoreVisVolumes();
  std::cout << "finitsh start mode!!!!\n";
}




void ModeSegRigidICP::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;

  if ( IsCtrKeyOn() )
  {
    m_stroke.clear();
    m_b_draw_stroke = true;
  }
  else if (IsShiftKeyOn())
  {
    m_b_trans_srcsurf = FormSegRigidICPBTrans();
    m_b_trans_pivot   = FormSegRigidICPBTransPiv();
    m_b_rot_srcsurf   = FormSegRigidICPBRot();

    m_pre_mouse_point = p;

    m_icpmats.clear();
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }
}



void ModeSegRigidICP::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if (m_b_draw_stroke)
  {
    EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();
    CrssecCore::CreateCurvedCrssec(cuboid, ogl->GetCamPos(), m_stroke);
  }

  m_bL = false;
  m_b_draw_stroke = false;
  m_b_trans_srcsurf = false;
  m_b_trans_pivot   = false;
  m_b_rot_srcsurf   = false;

  ogl->BtnUp();
  formMain_RedrawMainPanel();
}



void ModeSegRigidICP::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
  ogl->BtnDown_Rot(p);
}



void ModeSegRigidICP::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}



void ModeSegRigidICP::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}


void ModeSegRigidICP::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeSegRigidICP::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegRigidICP::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegRigidICP::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


static void SetMat(const EMat3d &rotate_matrix, const EVec3d &translate_matrix, EMat4d &matrix)
{
  matrix(0, 0) = rotate_matrix(0, 0);  matrix(0, 1) = rotate_matrix(0, 1); matrix(0, 2) = rotate_matrix(0, 2); matrix(0, 3) = translate_matrix[0];
  matrix(1, 0) = rotate_matrix(1, 0);  matrix(1, 1) = rotate_matrix(1, 1); matrix(1, 2) = rotate_matrix(1, 2); matrix(1, 3) = translate_matrix[1];
  matrix(2, 0) = rotate_matrix(2, 0);  matrix(2, 1) = rotate_matrix(2, 1); matrix(2, 2) = rotate_matrix(2, 2); matrix(2, 3) = translate_matrix[2];
  matrix(3, 0) = 0;
  matrix(3, 1) = 0;
  matrix(3, 2) = 0;
  matrix(3, 3) = 1;
}


void ModeSegRigidICP::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if ( m_b_draw_stroke )
  {
    m_stroke.push_back(ray_pos + 0.1f * ray_dir);
  }
  else if (m_b_rot_srcsurf )
  {
    int dx = p.x() - m_pre_mouse_point.x();
    int dy = p.y() - m_pre_mouse_point.y();
    EMat3d dR = CalcObjectRotationMatrixByMouseDrag(ogl, dx,dy,0.007);

    //pivot (= icpmat[i] * pivot_vec ) を中心に回転する
    // pos  = R x + t
    // pos' = dR (R x + t - piv) + piv = dR R x + dR t - dR piv + piv
    const int frameI = formVisParam_getframeI();
    EVec3f piv = Mult( m_icpmats[frameI], m_handle_pivot);
    
    EMat3d R;
    R(0, 0) = m_icpmats[frameI](0, 0);  R(0, 1) = m_icpmats[frameI](0, 1);  R(0, 2) = m_icpmats[frameI](0, 2);
    R(1, 0) = m_icpmats[frameI](1, 0);  R(1, 1) = m_icpmats[frameI](1, 1);  R(1, 2) = m_icpmats[frameI](1, 2);
    R(2, 0) = m_icpmats[frameI](2, 0);  R(2, 1) = m_icpmats[frameI](2, 1);  R(2, 2) = m_icpmats[frameI](2, 2);
    
    EVec3d tmp(m_icpmats[frameI](0, 3), m_icpmats[frameI](1, 3), m_icpmats[frameI](2, 3) );
    EVec3d t = piv.cast<double>() + dR * ( tmp - piv.cast<double>());
    SetMat(dR * R, t, m_icpmats[frameI]);
    
    m_b_modified = true;
  }
  else if (m_b_trans_srcsurf)
  {
    int dx = p.x() - m_pre_mouse_point.x();
    int dy = p.y() - m_pre_mouse_point.y();
    EVec3d t = CalcObjectTransVectorByMouseDrag(ogl, dx, dy, 0.1); 
    const int frameI = formVisParam_getframeI();
    m_icpmats[frameI](0, 3) += t[0];
    m_icpmats[frameI](1, 3) += t[1];
    m_icpmats[frameI](2, 3) += t[2];
    m_b_modified = true;
  }
  else if (m_b_trans_pivot)
  {
    int dx = p.x() - m_pre_mouse_point.x();
    int dy = p.y() - m_pre_mouse_point.y();
    EVec3d t = CalcObjectTransVectorByMouseDrag(ogl, dx, dy, 0.1); 
    EMat4d Minv = m_icpmats[formVisParam_getframeI()].inverse();
    // piv  = R * handle_piv + t (Rとtは ICPMatの回転・移動成分)   
    // piv' = R * handle_piv + t + dt 
    //      = R * (handle_piv + Rinv dt ) * t -->Rinv * dt だけ handle_piv を移動 
    m_handle_pivot += MultOnlyRot( Minv, t.cast<float>());
    m_b_modified = true;
  }
  else
  {
    ogl->MouseMove(p);
  }

  m_pre_mouse_point = p;
  formMain_RedrawMainPanel();
}


void ModeSegRigidICP::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  if(!WheelingCrssec(p, z_delta, ogl) ) 
    ogl->ZoomCamera(z_delta * 0.1f);

  formMain_RedrawMainPanel();
}


void ModeSegRigidICP::KeyDown(int nChar)
{
  if (nChar == VK_TAB)
    m_b_show_source_and_iso_surface = !m_b_show_source_and_iso_surface;

  formMain_RedrawMainPanel();
}
void ModeSegRigidICP::KeyUp(int nChar) {}




void ModeSegRigidICP::DrawScene(const EVec3f &cuboid, const EVec3f &cam_pos, const EVec3f &cam_cnt)
{
  const int    frame_idx = formVisParam_getframeI();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();

  //bind volumes
  BindAllVolumes();
  DrawCrossSectionsVisFore(!IsSpaceKeyOn());

  if (m_b_draw_stroke)
  {
    DrawPolyLine( EVec3f(1,1,0), 3, m_stroke, false);
  }

  //draw handle
  if ( IsShiftKeyOn() )
  {
    EVec3f pivot = Mult( m_icpmats[frame_idx], m_handle_pivot );

    TMesh::DrawSphere( pivot, pitch[0] * 3);
    if (FormSegRigidICPBTrans()) 
    {
      DrawHandleOrthoArrows(
          pivot, cuboid.norm() * 0.2, cuboid.norm() * 0.01,
          COLOR_R, COLOR_G, COLOR_B );
    }
    if (FormSegRigidICPBTransPiv()) 
    {
      DrawHandleOrthoArrows(
        pivot, cuboid.norm() * 0.2, cuboid.norm() * 0.005, 
        COLOR_R, COLOR_R, COLOR_R );
    }
    if (FormSegRigidICPBRot()  )
    {
      DrawHandleOrthoCircles(pivot, cuboid.norm() * 0.15);
    }
  }

  //draw source and iso surfaces
  static float diff1[4] = {0.2f,0.8f,0.2f,0.3f};
  static float ambi1[4] = {0.2f,0.8f,0.2f,0.3f};
  static float diff2[4] = {0.4f,0.1f,0.8f,1.0f};
  static float ambi2[4] = {0.4f,0.1f,0.8f,1.0f};
  static float spec[4]  = {0.9f,0.9f,0.9f,0.3f};
  static float shin[1]  = {54.0f};

  glEnable(GL_LIGHTING);

  glPushMatrix();
  glMultMatrixd(m_icpmats[frame_idx].data());
  if (m_b_show_source_and_iso_surface)
    m_source_surface.Draw(diff2, ambi2, spec, shin);
  glPopMatrix();

  glDepthMask( false );
  glEnable( GL_BLEND );
  if (m_b_show_source_and_iso_surface) 
    m_isosurfaces[frame_idx].Draw(diff1, ambi1, spec, shin);
  glDisable( GL_BLEND );
  glDisable(GL_LIGHTING);
  glDepthMask( true );


  if (formVisParam_bRendVol())
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    const bool b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolumeVisFore(b_onmanip, !IsSpaceKeyOn(), cam_pos, cam_cnt);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }

}





void ModeSegRigidICP::GenIsoSurface_OneFrm(const int isovalue, const int frame_idx)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  m_isosurfaces.clear();
  m_isosurfaces.resize(num_frames);
  m_isovalue = isovalue;

  const EVec3i reso       = ImageCore::GetInst()->GetReso();
  const EVec3f pitch      = ImageCore::GetInst()->GetPitch();
  const short* volume     = ImageCore::GetInst()->m_img4d[frame_idx];
  MarchingCubesPolygonSoup( reso, pitch, volume, isovalue, 0, 0, m_isosurfaces[frame_idx]);

  std::cout <<"isoSurfGen1Frame " << frame_idx << " / "  <<  num_frames << "\n";
}



void ModeSegRigidICP::GenIsoSurface_AllFrn(const int isovalue)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  m_isosurfaces.clear();
  m_isosurfaces.resize(num_frames);
  m_isovalue = isovalue;

  const EVec3i reso  = ImageCore::GetInst()->GetReso();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();

  for (int i = 0; i < num_frames; ++i)
  {
    short* volume = ImageCore::GetInst()->m_img4d[i];
    MarchingCubesPolygonSoup( reso, pitch, volume, isovalue, 0, 0, m_isosurfaces[i] );
    std::cout << "isoSurfGen " << i << "/" << num_frames << "\n";
  }

}





//this function returns rigid transformation matrix 
//from srcPC to trgtPC
static EMat4d t_calcRigidTransformation(
    const float		 icp_rejectionScale,  // icp parameter
    const int			 icp_numLevels,       // icp parameter
    EMat4d				 init_Matrix,         // initial 4x4 matrix
    const cv::Mat &source_points,       // src point cloud
    const cv::Mat &target_points        // trgt point cloud
)
{
  std::cout << "\n -t_calcRigidTransformation-- \n";
  std::cout <<"rejScale: "<< icp_rejectionScale << " numLv: " << icp_numLevels << "\n";

  init_Matrix.transposeInPlace();

  cv::ppf_match_3d::Pose3DPtr M1 = new cv::ppf_match_3d::Pose3D();
  M1->updatePose(init_Matrix.data());
  std::vector<cv::ppf_match_3d::Pose3DPtr> poses;
  poses.push_back(M1);

  //cout << "initM \n";
  //Trace(initM);
  //for (int i = 0; i < 16; ++i) {
  //	if (i % 4 == 0) cout << "\n";
  //	cout << "poses[0]->pose[" << i << "] = " <<  poses[0]->pose[i] << "\n";
  //} 
  std::cout << "srcPC  : row:" << source_points.rows <<  " col:" <<  source_points.cols << "\n";
  std::cout << "trgtPC : row:" << target_points.rows <<  " col:" <<  target_points.cols << "\n";

  try
  {
    // Create an instance of ICP
    cv::ppf_match_3d::ICP icp(100, 0.05f, icp_rejectionScale, icp_numLevels);
    int i = icp.registerModelToScene(source_points, target_points, poses);
  }
  catch (...)
  {
    std::cout << "*** surfaceの形状または位置が大きく異なるため追跡に失敗しました ***\n";
  }

  EMat4d resM;
  resM(0, 0) = poses[0]->pose[0]; resM(1, 0) = poses[0]->pose[4]; resM(2, 0) = poses[0]->pose[8];  resM(3, 0) = poses[0]->pose[12];
  resM(0, 1) = poses[0]->pose[1]; resM(1, 1) = poses[0]->pose[5]; resM(2, 1) = poses[0]->pose[9];  resM(3, 1) = poses[0]->pose[13];
  resM(0, 2) = poses[0]->pose[2]; resM(1, 2) = poses[0]->pose[6]; resM(2, 2) = poses[0]->pose[10]; resM(3, 2) = poses[0]->pose[14];
  resM(0, 3) = poses[0]->pose[3]; resM(1, 3) = poses[0]->pose[7]; resM(2, 3) = poses[0]->pose[11]; resM(3, 3) = poses[0]->pose[15];

  std::cout << "fin icp\n";
  return resM;
}





static void SetOpenCvMatrix(
    const TTriangleSoup &triangle_soup,
    cv::Mat &matrix)
{
  const int num_tri = triangle_soup.m_num_triangles;

  matrix = cv::Mat( num_tri, 6, CV_32F);

  EVec3f gc; 
  for (int n = 0; n < num_tri; ++n)
  {
    gc = triangle_soup.m_triangles[n].m_verts[0] 
       + triangle_soup.m_triangles[n].m_verts[1] 
       + triangle_soup.m_triangles[n].m_verts[2];
    gc /= 3.0;
    matrix.at<float>(n, 0) = gc[0];
    matrix.at<float>(n, 1) = gc[1];
    matrix.at<float>(n, 2) = gc[2];
    matrix.at<float>(n, 3) = triangle_soup.m_normals[n][0];
    matrix.at<float>(n, 4) = triangle_soup.m_normals[n][1];
    matrix.at<float>(n, 5) = triangle_soup.m_normals[n][2];
  }
}




// rigid registrtation for bones in first frame to other frames
void ModeSegRigidICP::PerformTracking(
    const float icp_reject_scale,
    const int		icp_number_level,
    const int		start_index,
    const int		end_index,
    const bool  b_parallelTracking
)
{
  if ( m_source_surface.m_num_triangles == 0)
  {
    ShowMsgDlg_OK("位置合わせを行うメッシュデータをロードしてください。", "caution");
    return;
  }
  if (m_isosurfaces[0].m_num_triangles == 0)
  {
    ShowMsgDlg_OK("isoSurface を生成してください。", "caution");
    return;
  }

  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  std::vector<int> trgtFrameList;

  if ( start_index < end_index )
    for (int i = start_index; i <= end_index; ++i) trgtFrameList.push_back(i);
  else
    for (int i = start_index; i >= end_index; --i) trgtFrameList.push_back(i);


  // matrix (3*num_triangles_ x 6) CV_32F;
  cv::Mat srcM; 
  SetOpenCvMatrix( m_source_surface, srcM);


  auto start = std::chrono::system_clock::now();

  if(b_parallelTracking)
  {
    const int trgtFrameListSize = (int) trgtFrameList.size();

#pragma omp parallel for
    for (int i = 0; i < trgtFrameListSize; ++i)
    {
      const int f = trgtFrameList[i];
      if (m_isosurfaces[f].m_num_triangles == 0) continue;
      cv::Mat trgtM;
      SetOpenCvMatrix( m_isosurfaces[f], trgtM);
      int prev_fidx = f;
      m_icpmats[f] = t_calcRigidTransformation( icp_reject_scale, icp_number_level, m_icpmats[prev_fidx],srcM,trgtM );
    }
  }
  else
  {
    int count = 0;
    for (auto f : trgtFrameList)
    {
      if (m_isosurfaces[f].m_num_triangles == 0) continue;
      std::cout << "\nstart registration  " << f << "\n";

      cv::Mat trgtM;
      SetOpenCvMatrix( m_isosurfaces[f], trgtM);

      int prev_fidx = (f == trgtFrameList.front()) ? f     : 
                      ( start_index < end_index  ) ? f - 1 : f + 1 ;
      prev_fidx = Crop(0, num_frames-1, prev_fidx);
                                                         
      m_icpmats[f] = t_calcRigidTransformation( icp_reject_scale, icp_number_level, m_icpmats[prev_fidx],srcM,trgtM );

      std::cout << "frame " << prev_fidx << " mat \n";
      Trace(m_icpmats[prev_fidx ]);
      std::cout << "frame " << f         << " mat \n";
      Trace(m_icpmats[f]);
      std::cout << "done " << f << "\n\n";
      formMain_SetProgressValue( (float) ++count / trgtFrameList.size() );
    }
  }


  auto end = std::chrono::system_clock::now();
  double timeSec = (double)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
  std::cout << timeSec << "second \n";
    
  FillInMesh(start_index, end_index);  

  formMain_SetProgressValue(0);
}



void ModeSegRigidICP::LoadSrcSurface(const std::string file_name)
{
  //obj or stl
  m_source_surface.Clear();
  m_source_surface.ImportObj( file_name.c_str() );

  m_handle_pivot = m_source_surface.GetGravityCenter();

  m_icpmats.resize( ImageCore::GetInst()->GetNumFrames() );
  for (auto &m : m_icpmats)
    m = EMat4d::Identity();
  
}



void ModeSegRigidICP::FillInMesh(
    int start_idx,
    int	end_idx
)
{
  if (m_source_surface.m_num_triangles == 0)
  {
    ShowMsgDlg_OK("ICPが計算されていません\n ICP is not computed yet ", "no mesh");
    return;
  }

  const int     num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const int     num_frames = ImageCore::GetInst()->GetNumFrames();
  const EVec3i  resolution = ImageCore::GetInst()->GetReso();
  const EVec3f  pitch      = ImageCore::GetInst()->GetPitch();
  std::vector<byte*> &flg4d     = ImageCore::GetInst()->m_flg4d;

  if( start_idx > end_idx ) std::swap( start_idx, end_idx );

  byte *flgInOut = new byte[num_voxels];
  
  int count = 0;
  for ( int f = start_idx; f <= end_idx; ++f )
  {
    TTriangleSoup mesh = m_source_surface;
    EMat4f M = m_icpmats[f].cast<float>();
    mesh.MultMat( M );

    GenBinaryVolumeFromMeshY(resolution, pitch, mesh, flgInOut);

    byte* flg3d = flg4d[f];
#pragma omp parallel for
    for (int i = 0; i < num_voxels; ++i)
    {
      flg3d[i] = (flg3d[i]    == 0) ? 0 :
                 (flgInOut[i] == 1) ? 255 : 1;
    }
    
    //progressbar 
    float v = (float) ++count / (end_idx - start_idx + 1.0f);
    formMain_SetProgressValue( v );
    std::cout << "fillInMesh " << f << "\n";
  }
  delete[] flgInOut;

  formMain_SetProgressValue(0);

  m_b_modified = true;

  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}








void ModeSegRigidICP::ExportMatrixSequence(std::string fname)
{
  
  std::string ext = ".txt";
  if (  fname.size() < ext.size() || 
        fname.find(ext, fname.size() - ext.size()) == std::string::npos )
    fname += ext;

  std::ofstream ofs(fname);

  if(!ofs.is_open())
  {
    std::cout << fname << " open err\n" ;
    return;
  }

  ofs << "iso value " << m_isovalue << "\n";
  
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  for(int f = 0; f < num_frames; ++f)
  {
    ofs << "frame " << f << " map\n";
    for( int i = 0; i < 4; i++) 
      ofs << m_icpmats[f](i, 0) << " " 
          << m_icpmats[f](i, 1) << " " 
          << m_icpmats[f](i, 2) << " " 
          << m_icpmats[f](i, 3) << "\n";
    ofs << "\n";
  }
    
	ofs.close();
  std::cout << "export matrix ... done \n";
}



void ModeSegRigidICP::ImportMatrixSequence(std::string fname)
{
  const int &num_frames = ImageCore::GetInst()->GetNumFrames();
  m_icpmats.resize(num_frames);

  std::ifstream ifs(fname);
  
  if ( ifs.fail() )
  {
    ShowMsgDlg_OK("file load err", "err");
    return ;
  }
  
  std::string str  ;
  int tmpint;

  //read 1st line [iso value value]
  ifs >> str >> str >> tmpint;

  for(int f = 0; f < num_frames ; ++f)
  {
    //read line [frame fidx map ]
    ifs >> str >> tmpint >> str; 
    std::cout << "frame " << f << " " << tmpint << "\n";
    if(ifs.eof()) break;

    for( int i = 0; i < 4; ++i) 
      ifs >> m_icpmats[f](i, 0) 
          >> m_icpmats[f](i, 1)
          >> m_icpmats[f](i, 2) 
          >> m_icpmats[f](i, 3); 
    if(ifs.eof()) break;
  }
    
	ifs.close();
  std::cout << "ImportMatrixSequence finish\n";
}
