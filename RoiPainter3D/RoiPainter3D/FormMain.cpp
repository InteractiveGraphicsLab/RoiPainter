#pragma managed
#include "FormMain.h"
#include "FormSubView.h"
#include "FormVisParam.h"
#include "FormVisNorm.h"
#include "FormVisMask.h"
#include "FormSegRGrow.h"
#include "FormSegGCut.h"
#include "FormSegThreshfieldPaint.h"
#include "FormSegVoxelPaint.h"
#include "FormSegLocalRGrow.h"
#include "FormSegParallelWires.h"
#include "FormRefStrokeTrim.h"
#include "formMaskIdSelection.h"
#include "FormIntegerSelection.h"
#include "climessagebox.h"

#pragma unmanaged
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"
#include "ViewAngleCore.h"
#include <iostream>

#pragma managed

using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace RoiPainter3D;

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib"    )
#pragma comment( lib, "gdi32.lib"    )
#pragma comment( lib, "User32.lib"   )


FormMain::FormMain(void)
{
  std::cout << "FormMain constructor\n";

  m_prevKeyID = -1;
  m_ogl = 0;
  
  InitializeComponent();
  
  m_ogl = new OglForCLI(GetDC((HWND)FormMainPanel->Handle.ToPointer()));
  initCameraPosition();
  m_ogl->SetBgColor(0.3f, 0.3f, 0.3f, 0.5f);

  std::cout << "FormMain constructor .. DONE\n";
}


FormMain::~FormMain()
{
  delete m_ogl;

  if (components)
  {
    delete components;
  }
}


void FormMain::setBkColor(float r, float g, float b) 
{ 
  m_ogl->SetBgColor(r, g, b, 0); 
}



System::Void FormMain::FormMainPanel_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
  if (e->Button == System::Windows::Forms::MouseButtons::Left  ) ModeCore::GetInst()->LBtnDown(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Middle) ModeCore::GetInst()->MBtnDown(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Right ) ModeCore::GetInst()->RBtnDown(EVec2i(e->X, e->Y), m_ogl);
}

System::Void FormMain::FormMainPanel_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
  ModeCore::GetInst()->MouseMove(EVec2i(e->X, e->Y), m_ogl);
}

System::Void FormMain::FormMainPanel_MouseUp  (System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
  if (e->Button == System::Windows::Forms::MouseButtons::Left  ) ModeCore::GetInst()->LBtnUp(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Middle) ModeCore::GetInst()->MBtnUp(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Right ) ModeCore::GetInst()->RBtnUp(EVec2i(e->X, e->Y), m_ogl);
}

System::Void FormMain::FormMainPanel_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
  if (e->Button == System::Windows::Forms::MouseButtons::Left)
  {
    if (!pickViewAngleIndicator(e->X, e->Y) ) ModeCore::GetInst()->LBtnDblClk(EVec2i(e->X, e->Y), m_ogl);
  }
  if (e->Button == System::Windows::Forms::MouseButtons::Middle) ModeCore::GetInst()->MBtnDblClk(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Right) ModeCore::GetInst()->RBtnDblClk(EVec2i(e->X, e->Y), m_ogl);
}





System::Void FormMain::FormMainPanel_Resize   (System::Object^  sender, System::EventArgs^  e)
{
  if (m_ogl == 0) return;
  redrawMainPanel();
}


System::Void FormMain::FormMainPanel_Paint    (System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
{
  if (m_ogl == 0) return;
  redrawMainPanel();
}


System::Void FormMain::FormMain_Resize(System::Object^  sender, System::EventArgs^  e)
{
  if (m_ogl == 0) return;
  replaceOtherForms();
}

System::Void FormMain::FormMain_Move  (System::Object^  sender, System::EventArgs^  e)
{
  if (m_ogl == 0) return;
  replaceOtherForms();
}



System::Void FormMain::FormMain_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
  int x = e->X - this->FormMainPanel->Location.X;
  int y = e->Y - this->FormMainPanel->Location.Y;

  ModeCore::GetInst()->MouseWheel(EVec2i(x, y), e->Delta, m_ogl);
}





bool FormMain::pickViewAngleIndicator(int x, int y)
{
  if (!FormVisParam::getInst()->bRendIndi()) return false;

  int curViewW = FormMainPanel->Width;
  int curViewH = FormMainPanel->Height;
  EVec3f  camP = m_ogl->GetCamPos();
  EVec3f  camC = m_ogl->GetCamCnt();
  EVec3f  camY = m_ogl->GetCamUp();

  int flg = ViewAngleCore::GetInst()->PickIndicator(*m_ogl, curViewW, curViewH, camP, camC, camY, EVec2i(x,y));

  bool tf = false;
  if      (flg == 1) { camP << 0,-1, 0;  camY << 0, 0, 1; tf = true; }
  else if (flg == 2) { camP << 1, 0, 0;  camY << 0, 0, 1; tf = true; }
  else if (flg == 3) { camP << 0, 1, 0;  camY << 0, 0, 1; tf = true; }
  else if (flg == 4) { camP <<-1, 0, 0;  camY << 0, 0, 1; tf = true; }
  else if (flg == 5) { camP << 0, 0, 1;  camY << 0,-1, 0; tf = true; }
  else if (flg == 6) { camP << 0, 0,-1;  camY << 0,-1, 0; tf = true; }

  if (!tf) return false;

  EVec3f cuboid = ImageCore::GetInst()->GetCuboid();
  double  D = cuboid.norm() * 1.0;
  camC = 0.5 * cuboid;
  camP = camC + ((float)D) * camP;
  m_ogl->SetCam(camP, camC, camY);
  redrawMainPanel();
  return true;
}








void FormMain::replaceOtherForms()
{
  const int thisX = this->Location.X;
  const int thisY = this->Location.Y;
  const int thisW = this->Width;
  const int dlgH = FormVisParam::getInst()->Height;

  FormVisParam::getInst()->Location = Point(thisX + thisW, thisY);
  FormVisNorm ::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormVisMask ::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegRGrow::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegGCut ::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegLocalRGrow   ::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegVoxelPaint   ::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormRefStrokeTrim   ::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegParallelWires::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegThreshfieldPaint::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
}



//初期化のタイミングは重要
//managedクラスはここで初期化する
void FormMain::initializeOtherForms()
{
  std::cout << "--------initialize form(dialogs)...\n";
  FormVisParam::getInst()->initAllItemsForNewImg();
  FormVisParam::getInst()->Show();
  FormVisParam::getInst()->Location = Point(this->Location.X + this->Width, this->Location.Y);


  //一度Showし Hideする(そうしないと移動が効かない)
  FormVisNorm ::getInst()->Show();
  FormVisMask ::getInst()->Show();
  FormSegRGrow::getInst()->Show();
  FormSegGCut ::getInst()->Show();
  FormSegLocalRGrow ::getInst()->Show();
  FormSegVoxelPaint ::getInst()->Show();
  FormRefStrokeTrim ::getInst()->Show();
  FormSegParallelWires::GetInst()->Show();
  FormSegThreshfieldPaint::getInst()->Show();

  replaceOtherForms();
  
  FormVisNorm ::getInst()->Hide();
  FormVisMask ::getInst()->Hide();
  FormSegRGrow::getInst()->Hide();
  FormSegGCut ::getInst()->Hide();
  FormSegLocalRGrow ::getInst()->Hide();
  FormSegVoxelPaint ::getInst()->Hide();
  FormRefStrokeTrim ::getInst()->Hide();
  FormSegParallelWires::GetInst()->Hide();
  FormSegThreshfieldPaint::getInst()->Hide();

  std::cout << "--------initialize form(dialogs)...DONE\n";
}



















void FormMain::redrawMainPanel()
{
  static bool isFirst = true;
  if (isFirst)
  {
    //このタイミングで 他のformを生成し, Show()も読んでOK
    isFirst = false;
    initializeOtherForms();
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
  }

  const EVec3f cuboid   = ImageCore::GetInst()->GetCuboid();
  const float  nearDist = (cuboid[0] + cuboid[1] + cuboid[2]) / 3.0f * 0.01f;
  const float  farDist  = (cuboid[0] + cuboid[1] + cuboid[2]) / 3.0f * 8;
  const int    viewW    = FormMainPanel->Width;
  const int    viewH    = FormMainPanel->Height;

  m_ogl->OnDrawBegin(viewW, viewH, 45.0, nearDist, farDist);
  //initialize lights	
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);
  float p0[4] = {  1000,  1000, -1000, 1 };
  float p1[4] = { -1000,  1000, -1000, 1 };
  float p2[4] = {  1000, -1000, -1000, 1 };
  glLightfv(GL_LIGHT0, GL_POSITION, p0);
  glLightfv(GL_LIGHT1, GL_POSITION, p1);
  glLightfv(GL_LIGHT2, GL_POSITION, p2);

  if (FormVisParam::getInst()->bRendFrame()) t_DrawCuboidFrame(cuboid);

  ModeCore::GetInst()->DrawScene(cuboid, m_ogl->GetCamPos(), m_ogl->GetCamCnt());

  if (FormVisParam::getInst()->bRendIndi())
  {
    ViewAngleCore::GetInst()->DrawIndicator(viewW, viewH, m_ogl->GetCamPos(), m_ogl->GetCamCnt(), m_ogl->GetCamUp());
  }
  m_ogl->OnDrawEnd();
}



void FormMain::initCameraPosition()
{
  EVec3f& cuboid = ImageCore::GetInst()->GetCuboid();
  EVec3f camC = cuboid * 0.5;
  EVec3f camY(0, -1, 0);
  EVec3f camP = camC - cuboid[2] * 1.5f * EVec3f(0, 0, 1);
  m_ogl->SetCam(camP, camC, camY);
}






static void n_marshalString(String ^ s, std::string& os) {
  const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
  os = chars;
  Marshal::FreeHGlobal(IntPtr((void*)chars));
}


class LessString_ForFnameTime {
public:
  bool operator()
    (
    const std::pair<std::string, std::string>& rsLeft,
    const std::pair<std::string, std::string>& rsRight
    ) const 
  {
    if(rsLeft.first.length() == rsRight.first.length()) return rsLeft.first < rsRight.first;
    else                                                return rsLeft.first.length() < rsRight.first.length();
  }
};






static bool t_showOpenFileDlg_multi
(
  const char* filter,
  std::vector<std::string> &fNames
)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = gcnew System::String(filter);
  dlg->Multiselect = true;

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return false;

  std::vector<std::pair<std::string, std::string>> vec_fname_date;

  for each (auto it in  dlg->FileNames)
  {
    std::string fName, fTime;
    n_marshalString(it, fName);
    n_marshalString(File::GetCreationTime(it).ToString(), fTime);
    vec_fname_date.push_back(std::make_pair(fName, fTime));
  }

  //sort fname_ftime 
  //currentry the system only uses file name
  sort(vec_fname_date.begin(), vec_fname_date.end(), LessString_ForFnameTime() );
  
  fNames.clear(); 
  for( const auto &it : vec_fname_date) fNames.push_back(it.first);

  return true;
}


static bool t_showOpenFileDlg_single
(
    const char* filter,
    std::string &fname
)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = gcnew System::String(filter);
  dlg->Multiselect = false;

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return false;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  fname = static_cast<const char*>(mptr.ToPointer());

  return true;
}


static bool t_showSaveFileDlg
(
    const char *filter, 
    std::string &fname
)
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = gcnew System::String(filter);

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return false;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  fname = static_cast<const char*>(mptr.ToPointer());
  return true;
}




// bmp/tif slices
System::Void FormMain::open2DSlicesToolStripMenuItem_Click      (System::Object^  sender, System::EventArgs^  e) 
{
  std::vector<std::string> fNames;
  if( !t_showOpenFileDlg_multi("2D slice files(*.bmp;*.tif)|*.bmp;*.tif", fNames) ) return;

  if( fNames.size() == 0 || fNames.front().length() < 3) return;

  std::string fext = fNames.front().substr(fNames.front().length()-3, 3);
  std::cout << fext.c_str() << "!!!!!!!!!!!!!!!!\n";

  //load volume / update visParam / init camera / redraw 
  ImageCore   ::GetInst()->LoadVolume(fNames,fext);
  FormVisParam::getInst()->initAllItemsForNewImg();
  initCameraPosition();
  redrawMainPanel();
}


//2d dcm slices 
System::Void FormMain::open2DSlicesdcmToolStripMenuItem_Click   (System::Object^  sender, System::EventArgs^  e) 
{
  std::vector<std::string> fNames;
  if( !t_showOpenFileDlg_multi("2d dcm slices(*.dcm;*.DCM;*)|*.dcm;*.DCM;*", fNames) ) return;

  if( fNames.size() == 0 || fNames.front().length() < 3) return;

  //load volume / update visParam / init camera / redraw 
  ImageCore   ::GetInst()->LoadVolume(fNames, std::string("dcm"));
  FormVisParam::getInst()->initAllItemsForNewImg();
  initCameraPosition();
  redrawMainPanel();
}


System::Void FormMain::open3DVolumetraw3DToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
  std::string fname;
  if( !t_showOpenFileDlg_single("3d volume (*.traw3D_ss)|*.traw3D_ss", fname) ) return;
  if( fname.length() < 9) return;

  std::string fext = fname.substr(fname.length()-9, 9);
  std::cout << fext.c_str() << "!!!!!!!!!!!!!!!!\n";

  //load volume / update visParam / init camera / redraw 
  ImageCore   ::GetInst()->LoadVolume(fname,fext);
  FormVisParam::getInst()->initAllItemsForNewImg();
  initCameraPosition();
  redrawMainPanel();
}


System::Void FormMain::open3DColumedcmToolStripMenuItem_Click   (System::Object^  sender, System::EventArgs^  e)
{
  std::string fname;
  if( !t_showOpenFileDlg_single("3d volume (*.dcm;*.DCM;*)|*.dcm;*.DCM;*", fname) ) return;
  if( fname.length() < 3) return;

  //load volume / update visParam / init camera / redraw 
  ImageCore   ::GetInst()->LoadVolume(fname, std::string("dcm"));
  FormVisParam::getInst()->initAllItemsForNewImg();
  initCameraPosition();
  redrawMainPanel();
}


System::Void FormMain::open3DVolumefavToolStripMenuItem_Click   (System::Object^  sender, System::EventArgs^  e)
{
  std::string fname;
  if( !t_showOpenFileDlg_single("3d volume (*.traw3D_ss)|*.traw3D_ss", fname) ) return;
  if( fname.length() < 3) return;

  std::string fext = fname.substr(fname.length()-3, 3);
  std::cout << fext.c_str() << "!!!!!!!!!!!!!!!!\n";

  //load volume / update visParam / init camera / redraw 
  ImageCore   ::GetInst()->LoadVolume(fname,fext);
  FormVisParam::getInst()->initAllItemsForNewImg();
  initCameraPosition();
  redrawMainPanel();
}


System::Void FormMain::open3DVolumepvmSphToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
{
  std::string fname;
  if( !t_showOpenFileDlg_single("3d volume (*.pvm; sph)|*.pvm; *.sph", fname) ) return;
  if( fname.length() < 3) return;

  std::string fext = fname.substr(fname.length()-3, 3);
  std::cout << fext.c_str() << "!!!!!!!!!!!!!!!!\n";

  //load volume / update visParam / init camera / redraw 
  ImageCore   ::GetInst()->LoadVolume(fname,fext);
  FormVisParam::getInst()->initAllItemsForNewImg();
  initCameraPosition();
  redrawMainPanel();
}



System::Void FormMain::saveMaskmskToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
  std::string fname;
  if( !t_showSaveFileDlg("RoiPainter Mask data (*.msk)|*.msk", fname) ) return;
  ImageCore::GetInst()->SaveMask(fname.c_str());
  redrawMainPanel();
}


System::Void FormMain::loadMaskmskToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
  std::string fname;
  if( !t_showOpenFileDlg_single("RoiPainter Mask data (*.msk)|*.msk", fname) ) return;
	ImageCore::GetInst()->LoadMask( fname.c_str() );
	ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK);
  redrawMainPanel();
}




// Export functions fav
System::Void FormMain::saveMaskfavbToolStripMenuItem_Click(
    System::Object^  sender, 
    System::EventArgs^  e
    )
{
  CLI_MessageBox_OK_Show( "This function is under construction", "message" );
}



System::Void FormMain::exportROIsAsMesh_Click(
    System::Object^  sender, 
    System::EventArgs^  e
) 
{
  std::set<int> mask_ids = formMaskIdSelection_DoMultiSelection( 
                             ImageCore::GetInst()->GetMaskData(), 
                             ImageCore::GetInst()->GetActiveMaskID());
  if( mask_ids.empty() ) return;
  
  std::string filter = "export obj mesh name (*.obj)|*.obj";
  std::string fname;
  if( !t_showSaveFileDlg( filter.c_str(), fname) ) return;


  ImageCore::GetInst()->ExportMaskIDsAsOneMesh( mask_ids, fname.c_str() );
  

}

System::Void FormMain::exportROIsAsBmpImage_Click(
    System::Object^  sender, 
    System::EventArgs^  e
    ) 
{
  std::string filter = "Export bmp file name (*)|*";
  std::string fname;
  if( !t_showSaveFileDlg( filter.c_str(), fname) ) return;
  
  ImageCore::GetInst()->ExportAllMaskIdAsBmpSlice( fname.c_str() );

  
}





System::Void FormMain::exportAROIAsBmpImage_Click(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  int mask_id = formMaskIdSelection_DoSingleSelection( 
    ImageCore::GetInst()->GetMaskData(), 
    ImageCore::GetInst()->GetActiveMaskID());

  if ( mask_id == -1 ) return;
  
  std::string filter = "Export bmp file name (*)|*";
  std::string fname;
  if( !t_showSaveFileDlg( filter.c_str(), fname) ) return;
  
  ImageCore::GetInst()->ExportOneMaskIdAsBmpSlice(mask_id, fname.c_str());
}


System::Void FormMain::exportVolumeXyToolStripMenuItem_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  std::string filter = "Export bmp file name (*)|*";
  std::string fname;
  if( !t_showSaveFileDlg( filter.c_str(), fname) ) return;
  
  ImageCore::GetInst()->ExportVolumeAsBmpSlice( fname.c_str(), 0 );
}


System::Void FormMain::exportVolumeYzToolStripMenuItem_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  std::string filter = "Export bmp file name (*)|*";
  std::string fname;
  if( !t_showSaveFileDlg( filter.c_str(), fname) ) return;
  
  ImageCore::GetInst()->ExportVolumeAsBmpSlice( fname.c_str(), 1 );
}


System::Void FormMain::exportVolumeZxToolStripMenuItem_Click(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  std::string filter = "Export bmp file name (*)|*";
  std::string fname;
  if( !t_showSaveFileDlg( filter.c_str(), fname) ) return;
  
  ImageCore::GetInst()->ExportVolumeAsBmpSlice( fname.c_str(), 2 );
}





//
System::Void FormMain::exportVolumeAsTraw3dssToolStripMenuItem_Click(
    System::Object^  sender, 
    System::EventArgs^  e
    ) 
{
  std::string filter = "traw3d signed short file (*.traw3D_ss)|*.traw3D_ss";
  std::string fname;
  if( !t_showSaveFileDlg( filter.c_str(), fname) ) return;
  ImageCore::GetInst()->SaveVolumeAsTraw3dss(fname.c_str());
}






System::Void FormMain::FormMain_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
{
  if (m_prevKeyID == (int)e->KeyCode) return;
  ModeCore::GetInst()->KeyDown((int)e->KeyCode);
  m_prevKeyID = (int)e->KeyCode;
  redrawMainPanel();
}

System::Void FormMain::FormMain_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
{
  ModeCore::GetInst()->KeyUp((int)e->KeyCode);
  m_prevKeyID = -1;
  redrawMainPanel();
}

// mode switch items
System::Void FormMain::visualizationStandardToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e){
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
  redrawMainPanel();
}
System::Void FormMain::visualizationMaskToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e){
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  redrawMainPanel();
}

System::Void FormMain::segmentationThresholdToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeCore::GetInst()->ModeSwitch(MODE_SEG_REGGROW);
  redrawMainPanel();
}

System::Void FormMain::segmentationGraphCutToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeCore::GetInst()->ModeSwitch(MODE_SEG_GCUT);
  redrawMainPanel();
}

System::Void FormMain::segmentationToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeCore::GetInst()->ModeSwitch(MODE_SEG_VOXPAINT);
  redrawMainPanel();
}

System::Void FormMain::refinementVoxelPaintToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeCore::GetInst()->ModeSwitch(MODE_REF_VOXPAINT);
  redrawMainPanel();
}

System::Void FormMain::refinementStrokeTrimingToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeCore::GetInst()->ModeSwitch(MODE_REF_STRKTRIM);
  redrawMainPanel();
}

System::Void FormMain::segmentationToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeCore::GetInst()->ModeSwitch(MODE_SEG_LCLRGROW);
  redrawMainPanel();
}

System::Void FormMain::segmentationParallelWiresToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e){
  ModeCore::GetInst()->ModeSwitch(MODE_SEG_PARAWIRE);
  redrawMainPanel();
}



System::Void FormMain::m_menuitem_subview_x_Click(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  m_menuitem_subview_x->Checked = !m_menuitem_subview_x->Checked;

  if ( m_menuitem_subview_x->Checked ) {
    FormSubViewX_Show();
    FormSubViewX_InitCamera();
  }
  else {
    FormSubViewX_Hide();
  }

}

System::Void FormMain::m_menuitem_subview_y_Click(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  m_menuitem_subview_y->Checked = !m_menuitem_subview_y->Checked;

  if ( m_menuitem_subview_y->Checked ) {
    FormSubViewY_Show();
    FormSubViewY_InitCamera();
  }
  else {
    FormSubViewY_Hide();
  }

}



System::Void FormMain::m_menuitem_subview_z_Click(
    System::Object^  sender, 
    System::EventArgs^  e) 
{

  m_menuitem_subview_z->Checked = !m_menuitem_subview_z->Checked;

  if ( m_menuitem_subview_z->Checked ) {
    FormSubViewZ_Show();
    FormSubViewZ_InitCamera();
  }
  else{
    FormSubViewZ_Hide();
  }

}



void RoiPainter3D::FormMain_SetBkColor(float r, float g, float b) {
  FormMain::getInst()->setBkColor(r, g, b);
}



void RoiPainter3D::RedrawScene(bool redraw_subview)
{
  FormMain::getInst()->redrawMainPanel();

  if (redraw_subview)
  {
    FormSubView::getInstX()->RedrawPanel();
    FormSubView::getInstY()->RedrawPanel();
    FormSubView::getInstZ()->RedrawPanel();
  }
}


