#pragma unmanaged
#include "ModeSegSwallowOrgans.h"
#include "ModeCommonTools.h"
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#pragma managed
#include "CliMessageBox.h"
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegSwallowOrgans.h"
#include "FormSegSwallowOrganTimeline.h"
#pragma unmanaged

#include <fstream>
#include <iomanip> //setw�p(obj�o�͎��̃t�@�C�����j

// 2020�N11����ƃ���
//1 �K�v�ȕ����Ɉȉ��̒ǉ�  OK
//    m_manip_log.clear();
//    m_undo_piv = -1;
//2 �ό`�𔺂������� m_manip_log��push OK 
//3 �ό`�O�Ɍ��`����L�^ OK 
//4 �E�N���b�N���Ɍ��ɖ߂� OK 
//5 scaling�̃n���h������������  OK
//6 �����炭�𑜓x�����肸�ɕςȕό`���N���Ă�E�����N���Ă邩�m�F
//7 ���b�V���̓ǂݍ��݂ƕ\��


// User interface --> 4 modes
//
// Selection 
// + Shift + R drag  --> ���_�̋�`�I��
// + Shift + R click --> ���_�̃N���b�N�I��
// + s key + L drag  --> ���_�̋�`�I��
// + s key + L click --> ���_�̃N���b�N�I��
//
//
// Modification 
// + Shift + L - drag : �܂Ƃ߂ĉ�]�E���s�ړ��E�g�� 
//   (mode�̓_�C�A���O���؂�ւ��\)
//
// Others
// + mesh��cage�̓_�C�A���O��胍�[�h�\
// + ���̗̈��\��mesh���_�C�A���O��胍�[�h�\



//DONE Visualization 
//����_�̃V�F�[�f�B���O   
//�n���h���̃V�F�[�f�B���O 
//���n���h�������Ȃ� -->�n���h���̌`��ω�    
//����_�̑傫�����ς�  --> dialog��cp size��ǉ�
//�O���t��y���������X�P�[�����O 

//TODO ?
//�O���t��ɕ����̒��_������  
//�����_view�̕\��
//�������_�̓����ړ��i����/ gaussian mode�ړ��ʂ������ɉ����Č����j
//
//�ۑ� : �������Ă��邤���ɂ��������ɂȂ� --> �������H


//����_�����W���Ă���Ƃ���ł͕ό`�Ƀm�C�Y?���̂��Ċ��炩�łȂ��Ȃ�
//--> �𑜓x�s���̉\������i�A���j
//���łɕ��������̈��mask volume�̕\�� �i�\ �� �}�X�N���ɐ��䂵�Ȃ��Ă悯��ΊȒP�j�D��x��   
//�s�v ? �O���t�̈�A�̓_���X�g���[�N�Ŏw��\��


using namespace RoiPainter4D;


//////////////////////////////////////////////////////////////////////////
/////////ModeSegSwallowOrgans////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ModeSegSwallowOrgans::ModeSegSwallowOrgans() : 
  //m_volume_shader("shader/volVtx.glsl", "shader/volFlg.glsl"),   // normal volume vis
  m_volume_shader("shader/volVtx.glsl", "shader/volFlg_Msk.glsl"), // mask volume vis
  m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
{
  std::cout << "ModeSegSwallowOrgans...\n";

  m_bL = m_bR = m_bM = false;
  m_meshseq.Clear();
  m_b_draw_cutstroke = false;

  std::cout << "ModeSegSwallowOrgans DONE\n";
}



ModeSegSwallowOrgans::~ModeSegSwallowOrgans()
{
  m_meshseq.Clear();
}



void ModeSegSwallowOrgans::startMode()
{
  m_bL = m_bR = m_bM = false;
  m_draghandle_id = OHDL_NON;
  m_b_draw_selectionrect = false;
  m_manip_log.clear();
  m_undo_piv = -1;
  m_meshseq.Clear();

  auto c = ImageCore::GetInst()->GetCuboidF();
  int cprate = FormSegSwqllowOrgans_GetCpSize();
  m_meshseq.SetHandleLength      ( c[0] * 0.03f   * cprate);
  m_meshseq.SetControlPointRadius( c[0] * 0.0015f * cprate);

  //initialize vFlg
  ImageCore::GetInst()->InitializeFlg4dByMask( formMain_SetProgressValue );
  
  //show dialog
  FormSegSwallowOrgans_Show();
  FormSegSwallowOrgans_InitAllItems();

  //NOTE : do not use vol_flg
  //4D volume (cpu) --> vis volume (gpu)
  UpdateImageCoreVisVolumes();

  //initialize cage point growp
  for ( int i=0; i < NUM_POINT_GROUPS; ++i )
  {
    m_groups[i].m_name = "na";
    m_groups[i].m_ids.clear();
  }

  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  FormSegSwallowTimeline_StartMode( num_frames );
  FormSegSwallowTimeline_Show();
  FormSegSwallowOrgans_UpdateList();
}



bool ModeSegSwallowOrgans::canEndMode()
{
  if( !m_meshseq.IsInitialized() ) 
    return true;
  else return 
    ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Leaving?"); 
}



void ModeSegSwallowOrgans::FinishSegmentation()
{
  const int num_frame = ImageCore::GetInst()->GetNumFrames();
  const int num_voxel = ImageCore::GetInst()->GetNumVoxels();

  FillInMesh();

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

  m_meshseq.Clear();
  
  //NOTE: do not clear (m_isovalue and m_isosurfaces)
  ModeCore::getInst()->ModeSwitch(MODE_VIS_MASK);
  formMain_RedrawMainPanel();
}



void ModeSegSwallowOrgans::CancelSegmentation()
{
  if( !m_meshseq.IsInitialized() || 
    ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "cancel?") )
  {
    ModeCore::getInst()->ModeSwitch(MODE_VIS_NORMAL);
    formMain_RedrawMainPanel();
  }
}



void ModeSegSwallowOrgans::FillInMesh()
{
  if ( !m_meshseq.IsInitialized() )
  {
    return;
  }

  const EVec3i   resolution = ImageCore::GetInst()->GetReso();
  const EVec3f   pitch      = ImageCore::GetInst()->GetPitch();
  std::vector<byte*> &flg4d = ImageCore::GetInst()->m_flg4d;

  const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  byte *flgInOut = new byte[num_voxels];

  for (int f = 0; f < num_frames; ++f)
  {
    TMesh &m = m_meshseq.GetMesh(f);


    std::cout << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    GenBinaryVolumeFromMesh_Y(resolution, pitch, m, flgInOut);
    std::cout << "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";

    byte* flg3d = flg4d[f];
#pragma omp parallel for
    for (int i = 0; i < num_voxels; ++i)
    {
      flg3d[i] = (flg3d[i]    == 0) ? 0 :
                 (flgInOut[i] == 1) ? 255 : 1;
    }

    formMain_SetProgressValue( f / (float)num_frames );
    std::cout << "fillInMesh " << f << "/" << num_frames << "\n";
  }
  delete[] flgInOut;

  formMain_SetProgressValue(0);

  UpdateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}







void ModeSegSwallowOrgans::LoadNewMeshAndCage
(
    std::string mesh_path, 
    std::string cage_path
)
{
  m_fpath_mesh = mesh_path;
  m_fpath_cage = cage_path;

  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const EVec3f cuboid  = ImageCore::GetInst()->GetCuboidF();
  m_meshseq.Initialize( num_frames, cuboid, mesh_path, cage_path);

  //initialize group
  for ( int i=0; i < NUM_POINT_GROUPS; ++i )
  {
    m_groups[i].m_name = "na";
    m_groups[i].m_ids.clear();
  }

  m_groups[0].m_name = "all";
  for( int i=0; i < m_meshseq.GetNumCageVertex(); ++i) 
    m_groups[0].m_ids.insert(i);

  m_manip_log.clear();
  m_undo_piv = -1;

  FormSegSwallowOrgans_UpdateList();
  formMain_RedrawMainPanel();
}  



static const char *MESSAGE_FOR_CANCEL =  
  "The system is going to copy current shape to the other all frames, OK?\n"
  "����frame�̌`���S�t���[���ɃR�s�[���Ă悢�ł����H\n"
  "���̏������s���Ƃ����܂ł�undo���ł��Ȃ��Ȃ�܂�";

void ModeSegSwallowOrgans::CopyCurrentMeshToOtherFrames()
{
  if( !ShowMsgDlgYesNo( MESSAGE_FOR_CANCEL, "Message") ) 
    return;

  const int frame_idx = formVisParam_getframeI();
  m_meshseq.CopyOneFrameToTheOtherAllFrames(frame_idx);
  m_manip_log.clear();
  m_undo_piv = -1;
}





void ModeSegSwallowOrgans::SaveCageMeshSequenceObj(std::string fname, bool b_cage)
{
  m_meshseq.ExportCageMeshSequenceAsObj(fname, b_cage);
}



void ModeSegSwallowOrgans::SaveCageMeshSequenceTxt(std::string fname, bool b_cage)
{
  m_meshseq.ExportCageMeshSequenceAsTxt(fname, b_cage);
}



void ModeSegSwallowOrgans::LoadCageSequence(std::string fname)
{
  m_meshseq.ImportCageSequenceFromTxt(fname);
  
  m_manip_log.clear();
  m_undo_piv = -1;
  formMain_RedrawMainPanel();
} 



//  DeformImportedCage
//1. �V���� obj (���̊K�w�̃P�[�W)��ǂݍ��݁A
//2. �ό`�p��hamonic coordinate�쐬
//3. ���݂̕ό`���ʂ�K�p���Aobj�̕ό`���o�͂���
void ModeSegSwallowOrgans::DeformImportedCage(
    std::string fname_obj,
    std::string fname_output)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const EVec3f cuboid  = ImageCore::GetInst()->GetCuboidF();

  CagedMeshSequence meshseq;
  meshseq.Initialize(num_frames, cuboid, fname_obj, m_fpath_cage);

  //3-1. ���݂̕ό`���ʂ�K�p
  const int num_cage_vtx = meshseq.GetNumCageVertex();
  for (int f = 0; f < num_frames; ++f)
  {
    for (int i = 0; i < num_cage_vtx; ++i)
    {
      EVec3f p = m_meshseq.GetCageVertex(f,i);
      meshseq.SetCageVertex(f,i,p,false);
    }
    meshseq.UpdateMeshShape(f);
  }
  
  //3-2. cage���_���o��
  meshseq.ExportCageMeshSequenceAsTxt(fname_output, false);
 
}




///////////////////////////////////////////////////////////////////////////////
//Mouse Listener///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ModeSegSwallowOrgans::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;
  
  const int frame_idx = formVisParam_getframeI();
  EVec3f ray_p, ray_d;
  std::tie(ray_p, ray_d) = ogl->GetCursorRay1( p );

  if ( isCtrKeyOn() )
  {
    m_stroke.clear();
    m_b_draw_cutstroke = true;
  }
  else if ( isShiftKeyOn() && m_meshseq.IsInitialized() )
  {
    if(m_meshseq.GetNumSelectedVtx() > 0)
    {
      //deformation
      const int trs = FormSegSwallowOrgans_IsModeTranslate() ? 0 : 
                      FormSegSwallowOrgans_IsModeRotate   () ? 1: 2;

      m_draghandle_id = m_meshseq.PickCageHandle(frame_idx, ray_p, ray_d, trs );

      if (m_draghandle_id > 0) 
      {
        m_cagevtx_prev = m_meshseq.GetCageVertices(frame_idx);
      }
    }

    if(m_draghandle_id == OHDL_NON)
    {
      if(isSKeyOn()) m_meshseq.ClearSelectedVtx();

      //pick to add
      m_meshseq.SelectCageVtxByPick( frame_idx, ray_p, ray_d);
      FormSegSwallowOrgans_SetNumSelectVtx( m_meshseq.GetNumSelectedVtx() );
      FormSegSwallowTimeline_RedrawPanel();
      
      //start drawing rect
      m_b_draw_selectionrect = true;
      
      for ( int i = 0; i < 4; ++i)
      {
        m_selectrect[i] = EVec3f(0,0,0);
      }

      formMain_RedrawMainPanel();
    }
  } 
  else
  {
    ogl->BtnDown_Trans(p);
  }

  m_prevpt = m_initpt = p;
}




void ModeSegSwallowOrgans::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if (m_b_draw_cutstroke)
  {
    EVec3f cuboid = ImageCore::GetCuboid();
    CrssecCore::CreateCurvedCrssec( cuboid, ogl->GetCamPos(), m_stroke);
  }
  
  if (m_draghandle_id != OHDL_NON)
  {
    const int frameidx = formVisParam_getframeI();
    auto verts = m_meshseq.GetCageVertices(frameidx);

    if (m_undo_piv < (int)m_manip_log.size() - 1)
    {
      //m_manip_log[m_undo_piv + 1...]���폜
      m_manip_log.erase( m_manip_log.begin() + m_undo_piv+1, m_manip_log.end());
    }

    m_manip_log.push_back(CageManipLog(frameidx, m_cagevtx_prev, verts));
    m_undo_piv = (int)m_manip_log.size() - 1;
  }

  if ( m_b_draw_selectionrect && m_meshseq.IsInitialized() )
  {
    m_meshseq.SelectCageVtxByRect( formVisParam_getframeI(), m_initpt, p, ogl);
    FormSegSwallowOrgans_SetNumSelectVtx( m_meshseq.GetNumSelectedVtx() );
    FormSegSwallowTimeline_RedrawPanel();

    if( FormSegSwallowOrgans_bFitCrssec() ) FitCrssecToSelectedCageVtx();
  }

  m_draghandle_id = OHDL_NON;
  m_b_draw_cutstroke = false;
  m_b_draw_selectionrect = false;

  m_bL = false;
  ogl->BtnUp();
  formMain_setCursorDefault();
  formMain_RedrawMainPanel();
}



void ModeSegSwallowOrgans::RBtnDown  (const EVec2i &p, OglForCLI *ogl)
{
  if ( m_draghandle_id != OHDL_NON )
  {
    //Translate�̃h���b�O���̉E�N���b�N�L�����Z��
    const int frame_idx = formVisParam_getframeI();
    m_meshseq.SetCageVertices(frame_idx, m_cagevtx_prev);

    m_draghandle_id = OHDL_NON;
    FormSegSwallowTimeline_RedrawPanel();
  }
  m_bR = true;
  ogl->BtnDown_Rot(p);
}



void ModeSegSwallowOrgans::RBtnUp    (const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}



void ModeSegSwallowOrgans::MBtnDown  (const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}



void ModeSegSwallowOrgans::MBtnUp    (const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}



void ModeSegSwallowOrgans::MouseMove (const EVec2i &p, OglForCLI *ogl)
{
  const int frame_idx = formVisParam_getframeI();
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);
  if ( !m_bL && !m_bR && !m_bM )
  {
    formMain_setCursorDefault();
    if ( isShiftKeyOn() && m_meshseq.IsInitialized() && m_meshseq.GetNumSelectedVtx() > 0)
    {
      const int trs = FormSegSwallowOrgans_IsModeTranslate() ? 0 : 
                      FormSegSwallowOrgans_IsModeRotate()    ? 1 : 2;
      auto tmp_draghandle_id = m_meshseq.PickCageHandle(frame_idx, ray_pos, ray_dir, trs );
      if(tmp_draghandle_id != OHDL_NON) formMain_setCursorNESW();
    }
    return;
  }
    
  if ( m_b_draw_cutstroke )
  {
    //cut stroke 
    m_stroke.push_back(ray_pos + 0.1f * ray_dir);
  }
  else if ( m_draghandle_id != OHDL_NON )
  {
    if ( FormSegSwallowOrgans_IsModeTranslate() )
    {
      m_meshseq.TranslateSelectedVerts( frame_idx, m_prevpt, p, m_draghandle_id, ogl);
    }
    else if ( FormSegSwallowOrgans_IsModeRotate() )
    {
      m_meshseq.RotateSelectedVerts( frame_idx, m_prevpt, p, m_draghandle_id, ogl);
    }
    else if ( FormSegSwallowOrgans_IsModeScale() )
    {
      m_meshseq.ScaleSelectedVerts( frame_idx, m_prevpt, p, ogl);
    }
    
    if( FormSegSwallowOrgans_bFitCrssec() ) FitCrssecToSelectedCageVtx();
    FormSegSwallowTimeline_RedrawPanel();

  }
  else if ( m_b_draw_selectionrect )
  {
    //draw selecting rect
    EVec2i rectpt[4] = { 
      EVec2i(std::min(p[0],m_initpt[0]), std::min(p[1],m_initpt[1]) ),
      EVec2i(std::min(p[0],m_initpt[0]), std::max(p[1],m_initpt[1]) ),
      EVec2i(std::max(p[0],m_initpt[0]), std::max(p[1],m_initpt[1]) ),
      EVec2i(std::max(p[0],m_initpt[0]), std::min(p[1],m_initpt[1]) ) };

    for ( int i = 0; i < 4; ++i)
    {
      auto ray = ogl->GetCursorRay1( rectpt[i] );
      m_selectrect[i] = std::get<0>(ray) + 0.1f * std::get<1>(ray);
    }
  }
  else 
  {
    ogl->MouseMove(p);
  }

  m_prevpt = p;
  formMain_RedrawMainPanel();
}



void ModeSegSwallowOrgans::MouseWheel(
    const EVec2i &p, 
    short z_delta, 
    OglForCLI *ogl)
{
  if(!WheelingCrssec(p, z_delta, ogl) ) 
    ogl->ZoomCamera(z_delta * 0.1f);
  formMain_RedrawMainPanel();
}



void ModeSegSwallowOrgans::LBtnDclk  (const EVec2i &p, OglForCLI *ogl){}
void ModeSegSwallowOrgans::RBtnDclk  (const EVec2i &p, OglForCLI *ogl){}
void ModeSegSwallowOrgans::MBtnDclk  (const EVec2i &p, OglForCLI *ogl){}



void ModeSegSwallowOrgans::keyDown( int nChar )
{
  if ( nChar == 'C' )
  {
    SelectedCageVtx_Clear();
    FormSegSwallowOrgans_SetNumSelectVtx(0);
    FormSegSwallowTimeline_RedrawPanel();
  }
  else if ( nChar == 'R'){ this->RegistPointGroup(); }
  else if ( nChar == '0'){ this->CheckoutPointGroup(0); }
  else if ( nChar == '1'){ this->CheckoutPointGroup(1); }
  else if ( nChar == '2'){ this->CheckoutPointGroup(2); }
  else if ( nChar == '3'){ this->CheckoutPointGroup(3); }
  else if ( nChar == '4'){ this->CheckoutPointGroup(4); }
  else if ( nChar == '5'){ this->CheckoutPointGroup(5); }
  else if ( nChar == '6'){ this->CheckoutPointGroup(6); }
  else if ( nChar == '7'){ this->CheckoutPointGroup(7); }
  else if ( nChar == '8'){ this->CheckoutPointGroup(8); }
  else if ( nChar == '9'){ this->CheckoutPointGroup(9); }
  else if ( nChar == 'Z' && isCtrKeyOn()) {
    this->UndoCpManipulation();
  }
  else if (nChar == 'Y' && isCtrKeyOn()) {
    this->RedoCpManipulation();
  }
  //std::cout << nChar << "\n";
}


void ModeSegSwallowOrgans::keyUp  ( int nChar )
{

}



void ModeSegSwallowOrgans::SelectedCageVtx_Clear()
{
  m_meshseq.ClearSelectedVtx();
}


static int VIS_COL_MAX = 5;
static float diff_vis[5][4] = {
  {0.9f,0.5f,0.5f,0.2f},
  {0.9f,0.5f,0.1f,0.2f},
  {0.5f,0.9f,0.5f,0.2f},
  {0.3f,0.9f,0.4f,0.2f},
  {0.5f,0.5f,0.9f,0.2f},
};

static float ambi_vis[5][4] = {
  {0.7f,0.5f,0.5f,0.2f},
  {0.7f,0.5f,0.1f,0.2f},
  {0.5f,0.7f,0.5f,0.2f},
  {0.3f,0.7f,0.4f,0.2f},
  {0.5f,0.5f,0.7f,0.2f},
};
static float spec[4] = { 0.9f,0.9f,0.9f,0.2f };
static float shin[1] = { 54.0f};



void ModeSegSwallowOrgans::drawScene(
    const EVec3f &cuboid, 
    const EVec3f &cam_pos, 
    const EVec3f &cam_center)
{
  const EVec3i reso = ImageCore::GetInst()->GetReso();
  const int frame_idx = formVisParam_getframeI();

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);

  //bind volumes ---------------------------------------
  BindAllVolumes();
  DrawCrossSections(cuboid, reso, !isSpaceKeyOn(), m_crssec_shader);
  
  //draw cut stroke
  if ( m_b_draw_cutstroke )
  {
    t_DrawPolyLine( EVec3f(1,1,0), 3, m_stroke, false);
  }
 
  //draw selection rect
  if ( m_b_draw_selectionrect )
  {
    t_DrawPolyLine( EVec3f(1,1,0), 3, 4, m_selectrect, true);
  }

  //draw cage (handle or normam)
  if ( isShiftKeyOn() && m_meshseq.GetNumSelectedVtx() > 0)
  {
    int trans_rot_scale = FormSegSwallowOrgans_IsModeTranslate() ? 0 :
                          FormSegSwallowOrgans_IsModeRotate()    ? 1 : 2;
    m_meshseq.DrawHandle( frame_idx, trans_rot_scale);
  }

  if ( !isSpaceKeyOn() )
  {
    m_meshseq.DrawCage( frame_idx , !FormSegSwallowOrgans_bShowOnlySelectedPts());
  }

  
  bool draw_bound      = FormSegSwallowOrgans_bVisBound();
  bool draw_surf_trans = FormSegSwallowOrgans_bVisSurfTrans();
  bool draw_surf_solid = FormSegSwallowOrgans_bVisSurfSolid();
  bool draw_vismeshes  = FormSegSwallowOrgans_bVisMeshes();

  if ( !isSpaceKeyOn() && (draw_bound || draw_surf_trans) )
  {
    //draw cage & mesh 
    glDisable( GL_CULL_FACE );
    glDepthMask( false );
    glEnable( GL_BLEND );

    const bool b_xy = formVisParam_bPlaneXY();
    const bool b_yz = formVisParam_bPlaneYZ();
    const bool b_zx = formVisParam_bPlaneZX();
    float planexy = b_xy ? CrssecCore::GetInst()->GetPlanePosXY() : -1;
    float planeyz = b_yz ? CrssecCore::GetInst()->GetPlanePosYZ() : -1;
    float planezx = b_zx ? CrssecCore::GetInst()->GetPlanePosZX() : -1;
    if ( !draw_bound)
    {
      planexy = planeyz = planezx = -1;
    }

    float opacity = draw_surf_trans ? (float) 0.4 : (float) 0;
    m_meshseq.DrawMesh( frame_idx, planeyz, planezx, planexy, opacity, cuboid);

    //glEnable( GL_DEPTH_TEST );
    glDepthMask( true );
    glDisable( GL_BLEND );
    glEnable( GL_CULL_FACE );
  }
  
  //draw mesh (solid)
  if (!isSpaceKeyOn() && draw_surf_solid)
  {
    //draw cage & mesh 
    glDisable(GL_CULL_FACE);
    float opacity = draw_surf_trans ? (float)0.4 : (float)0;
    m_meshseq.DrawMesh(frame_idx, -1, -1, -1, 1.0f, cuboid);
  }

  //draw vis meshes
  if (!isSpaceKeyOn() && draw_vismeshes)
  {
    glEnable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    for (int i = 0; i < m_vismeshes.size(); ++i)
    {
      const TMeshSequence &ms = m_vismeshes[i];
      const int color_idx = i % VIS_COL_MAX;
      if (frame_idx >= ms.m_meshes.size()) continue;
      ms.m_meshes[frame_idx].Draw(diff_vis[color_idx], ambi_vis[color_idx], spec, shin);
    }
  }



  //draw volume 
  if ( formVisParam_bRendVol() )
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    const bool b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolumeSlices( cuboid, reso, cam_pos, cam_center, 
                    !isSpaceKeyOn(), b_onmanip, m_volume_shader);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }
}





//PointGroup�̓o�^
void ModeSegSwallowOrgans::RegistPointGroup()
{
  //find null group
  int group_idx = -1;
  for( int i=0; i < NUM_POINT_GROUPS; ++i ) 
    if ( m_groups[i].m_ids.size() == 0)
    {
      group_idx = i;
      break;
    }

  if ( group_idx == -1 )
  {
    ShowMsgDlg_OK( "group is full/�O���[�v���ɋ󂫂�����܂���", "message" );
    return;
  }
  
  m_groups[group_idx].m_ids = m_meshseq.GetSelectedCageVtx();
  m_groups[group_idx].m_name = "group";

  FormSegSwallowOrgans_UpdateList();
}



//group[idx]����т���
void  ModeSegSwallowOrgans::CheckoutPointGroup(int idx)
{
  m_meshseq.ClearSelectedVtx();

  if ( 0 <= idx && idx < NUM_POINT_GROUPS && 
       m_groups[idx].m_ids.size() != 0 ) 
  {
    m_meshseq.SetCageVtxSelected(m_groups[idx].m_ids);
    FormSegSwallowOrgans_SetNumSelectVtx( m_meshseq.GetNumSelectedVtx() );
    FormSegSwallowTimeline_RedrawPanel();

    if( FormSegSwallowOrgans_bFitCrssec() ) FitCrssecToSelectedCageVtx();
  }

  formMain_RedrawMainPanel();
}



//group[idx]���폜
void  ModeSegSwallowOrgans::DeletePointGroup(int idx)
{
  m_meshseq.ClearSelectedVtx();

  if ( 0 <= idx && idx < NUM_POINT_GROUPS && 
       m_groups[idx].m_ids.size() != 0 ) 
  {
    m_groups[idx].m_name = "na";
    m_groups[idx].m_ids .clear();

    FormSegSwallowOrgans_UpdateList();
    formMain_RedrawMainPanel();
  }
 
}



void  ModeSegSwallowOrgans::LoadPointGroup(std::string fname)
{
  if ( m_meshseq.GetNumCageVertex() == 0 ) return;

  std::ifstream ifs( fname );

  if (ifs.fail())
  {
    std::cout <<  "error when opening "<< fname.c_str() << "\n";
    return ;
  }

  std::string str;
  int tmp_numgroups;
  ifs >> str >> tmp_numgroups;
  
  for (int i = 0; i < tmp_numgroups && i < NUM_POINT_GROUPS; ++i)
  {
    int idx, num_points;
    std::string name;
    ifs >> idx >> name >> num_points;
    m_groups[i].m_name = name;

    for ( int j = 0; j < num_points; ++j )
    {
      int point_idx;
      ifs >> point_idx;
      m_groups[i].m_ids.insert( point_idx );
    }
  } 
  ifs.close();

  FormSegSwallowOrgans_UpdateList();
  formMain_RedrawMainPanel();
}



void  ModeSegSwallowOrgans::SavePointGroup(std::string fname)
{
  std::ofstream fp( fname );

	if ( !fp.is_open() )
	{
    std::cout <<  "error when opening "<< fname.c_str() << "\n";
		return ;
	}

  fp << "roipainter4d_Swallowingtool_pointgroup " << NUM_POINT_GROUPS <<"\n" ;
  for ( int i=0; i < NUM_POINT_GROUPS; ++i )
  {
    fp << i << " " << m_groups[i].m_name << " " 
       << m_groups[i].m_ids.size() << "\n";
    
    for ( auto& it : m_groups[i].m_ids ) 
      fp << it << " ";
 
    fp << "\n\n";
  }

  fp.close();
}



std::vector<EVec3f> ModeSegSwallowOrgans::SelectedCageVtx_GetCentroidSeq()
{
  return m_meshseq.GetSelectedVtxCentroidSeq();
}

std::vector<std::vector<EVec3f>> ModeSegSwallowOrgans::SelectedCageVtx_GetSeq()
{
  return m_meshseq.GetSelectedVtxSeq();
}

std::vector<std::vector<EVec3f>> ModeSegSwallowOrgans::SelectedCageVtx_Get1RingSeq()
{
  return m_meshseq.GetSelectVtx1RingSeq();
}








void ModeSegSwallowOrgans::SelectedCageVtx_Move(
    int frame_idx, 
    int dim_xyz, 
    float pos)
{
  //�����ł�m_manip_log�ɓo�^���s���������A���݂�dlg��mouse move�ŌĂԎ����Ȃ̂�
  //���̑����undo�ł��Ȃ����Ƃɂ���
  m_meshseq.SetSelectedCageVtxPos(frame_idx, dim_xyz, pos );
}


void ModeSegSwallowOrgans::SelectedCageVtx_Smoothing(std::vector<int> trgt_fids)
{
  m_meshseq.SelectedCageVtx_Smoothing(trgt_fids);
}

void ModeSegSwallowOrgans::SelectedCageVtx_CopyLeft (std::vector<int> trgt_fids)
{
  m_meshseq.SelectedCageVtx_CopyFromLeft(trgt_fids);
}

void ModeSegSwallowOrgans::SelectedCageVtx_CopyRight(std::vector<int> trgt_fids)
{
  m_meshseq.SelectedCageVtx_CopyFromRight(trgt_fids);
}


void ModeSegSwallowOrgans::FitCrssecToSelectedCageVtx()
{
  if( !FormSegSwallowOrgans_bFitCrssec()) return;
  if ( m_meshseq.GetNumSelectedVtx() <= 0 ) return;

  const int    frame_idx = formVisParam_getframeI();
  const EVec3i reso      = ImageCore::GetInst()->GetReso();
  const EVec3f pitch     = ImageCore::GetInst()->GetPitch();
  const EVec3f gc        = m_meshseq.GetSelectedVtxCentroid( frame_idx );  
  CrssecCore::FitCrssecToPosition(reso, pitch, gc, CRSSEC_XY);
  CrssecCore::FitCrssecToPosition(reso, pitch, gc, CRSSEC_YZ);
  CrssecCore::FitCrssecToPosition(reso, pitch, gc, CRSSEC_ZX);
}



void ModeSegSwallowOrgans::UndoCpManipulation()
{
  std::cout << "UndoCpManipulation";
  //m_manip_log[m_undo_piv] �Ƃ�����������Ƃɖ߂�
  if (0 <= m_undo_piv && m_undo_piv < (int)m_manip_log.size())
  {
    m_meshseq.SetCageVertices(m_manip_log[m_undo_piv].m_frame, m_manip_log[m_undo_piv].m_point_prev);
    m_undo_piv -= 1;
  }
}



void ModeSegSwallowOrgans::RedoCpManipulation()
{
  std::cout << "RedoCpManipulation";

  //m_manip_log[m_undo_piv+1] �Ƃ���������ēx���{����

  if (0 <= m_undo_piv+1 && m_undo_piv+1 < (int)m_manip_log.size())
  {
    m_meshseq.SetCageVertices(m_manip_log[m_undo_piv].m_frame, m_manip_log[m_undo_piv+1].m_point_new);
    m_undo_piv += 1;
  }
}

