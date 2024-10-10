#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormVisNorm.h"
#include "FormVisMask.h"
#include "FormSegRGrow.h"
#include "FormSegPixPaint.h"
#include "FormSegRigidICP.h"
#include "FormSegClosestPix.h"
#include "FormSegLocalRGrow.h"
#include "FormSegSwallowOrgans.h"
#include "FormSegSwallowOrganTimeline.h"
#include "FormSegJointTracker.h"
#include "FormSegBolus.h"
#include "FormRefStrokeTrim.h"
#include "FormSegBronchi.h"
#include "FormLoadFrameIdx.h"
#include "FormSortFiles.h"
#include "FormSegStrokeFfd.h"
#include "FormPlaceCPs.h"

#pragma unmanaged
#include "OglForCLI.h"
#include "OglImage.h"
#include "ModeCore.h"
#include "ImageCore.h"
#include "CrsSecCore.h"
#include "ViewIndiCore.h"
#include "Mode/ModeVizNormal.h"
#include "Mode/ModeVizMask.h"
#include "Mode/ModeSegRGrow.h"
#include "Mode/ModeSegPixPaint.h"
#include "Mode/ModeSegRigidICP.h"
#include "Mode/ModeSegClosestPix.h"
#include "Mode/ModeSegLocalRGrow.h"
#include "Mode/ModeSegSwallowOrgans.h"
#include "Mode/ModeSegJointTracker.h"
#include "Mode/ModeRefStrokeTrim.h"
#include "Mode/ModeSegStrokeFfd.h"
#include "Mode/ModePlaceCPs.h"
#include <string>
#include <vector>
#pragma managed


using namespace System;
using namespace System::IO;

//for managed String --> std::string 
using namespace System::Runtime::InteropServices;
using namespace RoiPainter4D;


#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#pragma comment( lib, "gdi32.lib" )
#pragma comment( lib, "User32.lib" )



[STAThreadAttribute]
int main()
{
  std::cout << "\n\n---------------------main()--------------------------\n";


  //初期化のタイミングは重要
  //非managed はここで初期化する
  //だたしこのタイミングでは、Formを生成しshowしてはだめ．
  ImageCore::GetInst();
  CrssecCore::GetInst();
  ModeCore::GetInst();
  ModeVizMask::GetInst();
  ModeVizNormal::GetInst();
  ModeSegRGrow::GetInst();
  ModeSegPixPaint::GetInst();
  ModeSegRigidICP::GetInst();
  ModeSegClosestPix::GetInst();
  ModeSegLocalRGrow::GetInst();
  ModeSegSwallowOrgans::GetInst();
  ModeRefStrokeTrim::GetInst();
  FormSegBolus::GetInst();

  std::cout << "FormMain::getInst()->ShowDialog() \n";
  FormMain::GetInst()->ShowDialog();
  return 0;
}





//oglがnewされる手前で 
//this->ResumeLayout(false) -->  resize --> redraw が呼ばれoglが必要になる場合がある。
//これをうまく回避する必要が不明のため on sizeでnull checkをしている

FormMain::FormMain(void)
{
  std::cout << "FormMain constructor\n";

  m_prevKeyID = -1;
  m_ogl = 0;
  InitializeComponent();
  m_ogl = new OglForCLI(GetDC((HWND)FormMainPanel->Handle.ToPointer()));
  InitCameraPosition();
  m_ogl->SetBgColor(0.3f, 0.3f, 0.3f, 0.5f);

  m_progressbar->Minimum = 0;
  m_progressbar->Maximum = 100;

}

FormMain::~FormMain()
{
  delete m_ogl;

  if (components)
  {
    delete components;
  }
}


OglForCLI* FormMain::GetOgl()
{
  return m_ogl;
}



//初期化のタイミングは重要
//managedクラスはここで初期化する
void FormMain::InitializeSingletons()
{
  FormVisParam::GetInst()->InitAllItemsForNewImg();
  FormVisParam::GetInst()->Show();
  FormVisParam::GetInst()->Location = Point(this->Location.X + this->Width, this->Location.Y);

  //一度Showし Hideする(そうしないと移動が効かない)
  FormVisNorm::GetInst()->Show();
  FormVisMask::GetInst()->Show();
  FormSegRGrow::GetInst()->Show();
  FormSegPixPaint::GetInst()->Show();
  FormSegRigidICP::GetInst()->Show();
  FormSegClosestPix::GetInst()->Show();
  FormSegLocalRGrow::GetInst()->Show();
  FormSegBolus::GetInst()->Show();
  FormRefStrokeTrim::GetInst()->Show();
  FormSegSwallowOrgans::GetInst()->Show();
  FormSegSwallowOrganTimeline::GetInst()->Show();
  FormSegJointTracker::GetInst()->Show();
  FormSegBronchi::GetInst()->Show();
  FormSegStrokeFfd::GetInst()->Show();
  FormPlaceCPs::GetInst()->Show();

  ReplaceOtherForms();

  FormVisNorm::GetInst()->Hide();
  FormVisMask::GetInst()->Hide();
  FormSegRGrow::GetInst()->Hide();
  FormSegPixPaint::GetInst()->Hide();
  FormSegRigidICP::GetInst()->Hide();
  FormSegClosestPix::GetInst()->Hide();
  FormSegLocalRGrow::GetInst()->Hide();
  FormSegBolus::GetInst()->Hide();
  FormRefStrokeTrim::GetInst()->Hide();
  FormSegSwallowOrgans::GetInst()->Hide();
  FormSegSwallowOrganTimeline::GetInst()->Hide();
  FormSegJointTracker::GetInst()->Hide();
  FormSegBronchi::GetInst()->Hide();
  FormSegStrokeFfd::GetInst()->Hide();
  FormPlaceCPs::GetInst()->Hide();



  FormVisParam::GetInst()->Owner = this;
  FormVisNorm::GetInst()->Owner = this;
  FormVisMask::GetInst()->Owner = this;
  FormSegRGrow::GetInst()->Owner = this;
  FormSegPixPaint::GetInst()->Owner = this;
  FormSegRigidICP::GetInst()->Owner = this;
  FormSegClosestPix::GetInst()->Owner = this;
  FormSegLocalRGrow::GetInst()->Owner = this;
  FormSegBolus::GetInst()->Owner = this;
  FormRefStrokeTrim::GetInst()->Owner = this;
  FormSegSwallowOrgans::GetInst()->Owner = this;
  FormSegStrokeFfd::GetInst()->Owner = this;
  FormSegSwallowOrganTimeline::GetInst()->Owner = this;
  FormSegJointTracker::GetInst()->Owner = this;
  FormSegBronchi::GetInst()->Owner = this;
}


static void t_DrawFrame(const EVec3f &c)
{
  glDisable(GL_LIGHTING);
  glLineWidth(2);
  glColor3d(1, 1, 0);
  glBegin(GL_LINES);
  glVertex3d(0, 0, 0); glVertex3d(c[0], 0, 0);
  glVertex3d(c[0], 0, 0); glVertex3d(c[0], c[1], 0);
  glVertex3d(c[0], c[1], 0); glVertex3d(0, c[1], 0);
  glVertex3d(0, c[1], 0); glVertex3d(0, 0, 0);
  glVertex3d(0, 0, c[2]); glVertex3d(c[0], 0, c[2]);
  glVertex3d(c[0], 0, c[2]); glVertex3d(c[0], c[1], c[2]);
  glVertex3d(c[0], c[1], c[2]); glVertex3d(0, c[1], c[2]);
  glVertex3d(0, c[1], c[2]); glVertex3d(0, 0, c[2]);
  glVertex3d(0, 0, 0); glVertex3d(0, 0, c[2]);
  glVertex3d(c[0], 0, 0); glVertex3d(c[0], 0, c[2]);
  glVertex3d(c[0], c[1], 0); glVertex3d(c[0], c[1], c[2]);
  glVertex3d(0, c[1], 0); glVertex3d(0, c[1], c[2]);
  glEnd();


  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
  glColor3d(1, 0, 0); glVertex3d(0, 0, 0); glVertex3d(10, 0, 0);
  glColor3d(0, 1, 0); glVertex3d(0, 0, 0); glVertex3d(0, 10, 0);
  glColor3d(0, 0, 1); glVertex3d(0, 0, 0); glVertex3d(0, 0, 10);
  glEnd();

}


static void InitializeLights()
{
  GLfloat pos0 [4] = {0,0, 3000,1};
  GLfloat pos1 [4] = {0,0,-3000,1};
  GLfloat pos2 [4] = {3000,-3000, 3000,1};
  GLfloat dir0 [3] = {0,0,-1};
  GLfloat dir1 [3] = {0,0, 1};
  GLfloat dir2 [3] = {-1,1,-1};


  GLfloat ambi0[3] = {0.5f,0.5f,0.5f};
  GLfloat ambi1[3] = {0,0,0};
  GLfloat ambi2[3] = {0,0,0};
  GLfloat diff0[3] = {0.5f,0.5f,0.5f};
  GLfloat diff1[3] = {0.5f,0.5f,0.5f};
  GLfloat diff2[3] = {0.5f,0.5f,0.5f};
  GLfloat spec0[3] = {0.3f,0.3f,0.3f};
  GLfloat spec1[3] = {0.3f,0.3f,0.3f};
  GLfloat spec2[3] = {0.3f,0.3f,0.3f};

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, pos0);
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir0 );
  glLightf( GL_LIGHT0, GL_SPOT_CUTOFF,  180.0 );
  glLightf( GL_LIGHT0, GL_SPOT_EXPONENT, 0 );
  glLightfv(GL_LIGHT0, GL_AMBIENT , ambi0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE , diff0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);

  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_POSITION, pos1);
  glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir1 );
  glLightf( GL_LIGHT1, GL_SPOT_CUTOFF,  180.0 );
  glLightf( GL_LIGHT1, GL_SPOT_EXPONENT, 0 );
  glLightfv(GL_LIGHT1, GL_AMBIENT , ambi1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE , diff1);
  glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);
  
  glEnable(GL_LIGHT2);
  glLightfv(GL_LIGHT2, GL_POSITION, pos2);
  glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, dir2 );
  glLightf( GL_LIGHT2, GL_SPOT_CUTOFF,  180.0 );
  glLightf( GL_LIGHT2, GL_SPOT_EXPONENT, 0 );
  glLightfv(GL_LIGHT2, GL_AMBIENT , ambi2);
  glLightfv(GL_LIGHT2, GL_DIFFUSE , diff2);
  glLightfv(GL_LIGHT2, GL_SPECULAR, spec2);
}


void FormMain::RedrawMainPanel()
{
  static bool isFirst = true;
  if (isFirst)
  {
    isFirst = false;
    //このタイミングで 他のformを生成し, Show()も読んでOK
    InitializeSingletons();
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
  }

  EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();
  float  nearDist = (cuboid[0] + cuboid[1] + cuboid[2]) / 3.0f * 0.01f;
  float  farDist = (cuboid[0] + cuboid[1] + cuboid[2]) / 3.0f * 8;
  int    pw = FormMainPanel->Width ;
  int    ph = FormMainPanel->Height;
  EVec3f cam_pos = m_ogl->GetCamPos();
  EVec3f cam_cnt = m_ogl->GetCamCnt();
  EVec3f cam_up  = m_ogl->GetCamUp();

  m_ogl->OnDrawBegin(pw, ph, 45.0, nearDist, farDist);

  InitializeLights();
  if (FormVisParam::GetInst()->bRendFrame()) t_DrawFrame(cuboid);
  ModeCore::GetInst()->DrawScene(cuboid, cam_pos, cam_cnt);
  if (FormVisParam::GetInst()->bRendIndi())
  {
    ViewIndiCore::getInst()->DrawIndicator(pw, ph, cam_pos, cam_cnt, cam_up);
  }
  m_ogl->OnDrawEnd();
}


void FormMain::ActivateMainForm()
{
  this->Activate();
}


void FormMain::InitCameraPosition()
{
  EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();
  EVec3f camC = cuboid * 0.5;
  EVec3f camY(0, -1, 0);
  EVec3f camP = camC - cuboid[2] * 1.5f * EVec3f(0, 0, 1);
  m_ogl->SetCam(camP, camC, camY);
}


void FormMain::SetBackColor(float r, float g, float b, float a) 
{ 
  m_ogl->SetBgColor(r, g, b, a);
}


void FormMain::SetCursorNESW() 
{ 
  this->Cursor = Cursors::SizeNESW; 
}


void FormMain::SetCursorDefault() 
{ 
  this->Cursor = Cursors::Default; 
}


void FormMain::SetProgressValue(float value0to1)
{
  value0to1 = std::max(0.f, std::min(1.f, value0to1));
  int value = (int)(value0to1 * m_progressbar->Maximum);
  try {
    m_progressbar->Value = value;
  }
  catch (const System::ArgumentOutOfRangeException^) {
    if (value < m_progressbar->Minimum) m_progressbar->Value = m_progressbar->Minimum;
    else m_progressbar->Value = m_progressbar->Maximum;
  }
}


bool FormMain::PickViewIndicator(int px, int py)
{
  if (!FormVisParam::GetInst()->bRendIndi()) return false;

  int curViewW = FormMainPanel->Width;
  int curViewH = FormMainPanel->Height;
  EVec3f camP = m_ogl->GetCamPos();
  EVec3f camC = m_ogl->GetCamCnt();
  EVec3f camY = m_ogl->GetCamUp();

  int flg = ViewIndiCore::getInst()->PickIndicator(m_ogl, curViewW, curViewH,
    camP, camC, camY, EVec2i(px, py));

  bool tf = false;
  if (flg == 1) { camP << 0, -1, 0;  camY << 0, 0, 1; tf = true; }
  else if (flg == 2) { camP << 1, 0, 0;  camY << 0, 0, 1; tf = true; }
  else if (flg == 3) { camP << 0, 1, 0;  camY << 0, 0, 1; tf = true; }
  else if (flg == 4) { camP << -1, 0, 0;  camY << 0, 0, 1; tf = true; }
  else if (flg == 5) { camP << 0, 0, 1;  camY << 0, -1, 0; tf = true; }
  else if (flg == 6) { camP << 0, 0, -1;  camY << 0, -1, 0; tf = true; }

  if (!tf) return false;

  EVec3f cuboid = ImageCore::GetInst()->GetCuboidF();
  double  D = cuboid.norm() * 1.0;
  camC = 0.5 * cuboid;
  camP = camC + ((float)D) * camP;
  m_ogl->SetCam(camP, camC, camY);
  formMain_RedrawMainPanel();
  return true;
}


void FormMain::ReplaceOtherForms()
{
  const int thisX = this->Location.X;
  const int thisY = this->Location.Y;
  const int thisW = this->Width;
  const int dlgH = FormVisParam::GetInst()->Height;
  FormVisParam::GetInst()->Location = Point(thisX + thisW, thisY);
  FormVisNorm ::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormVisMask ::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormPlaceCPs::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);

  FormSegRGrow::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegPixPaint  ::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegRigidICP  ::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegClosestPix::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegLocalRGrow   ::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormRefStrokeTrim   ::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegBolus        ::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegSwallowOrgans::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegJointTracker ::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegBronchi      ::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegStrokeFfd    ::GetInst()->Location = Point(thisX + thisW, thisY + dlgH);
}



#pragma unmanaged
#define LINE_SIZE 1024
void t_info(const char* fmt, ...)
{
  char line[LINE_SIZE];
  va_list ap;

  va_start(ap, fmt);
  vsprintf_s(line, LINE_SIZE, fmt, ap);
  va_end(ap);

  std::cout << line;
}
#pragma managed



System::Void FormMain::switch_visNorm_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
  RedrawMainPanel();
}

System::Void FormMain::switch_visMask_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  RedrawMainPanel();
}

System::Void FormMain::switch_segPaint_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeCore::GetInst()->ModeSwitch(MODE_SEG_PIXPAINT);
  RedrawMainPanel();
}

System::Void FormMain::switch_visRegionGrow_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeCore::GetInst()->ModeSwitch(MODE_SEG_REGGROW);
  RedrawMainPanel();
}

System::Void FormMain::switch_segRigidICP_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeCore::GetInst()->ModeSwitch(MODE_SEG_RIGIDICP);
  RedrawMainPanel();
}

System::Void FormMain::switch_segClosestPix_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeCore::GetInst()->ModeSwitch(MODE_SEG_CLOSESTPIX);
  RedrawMainPanel();
}


System::Void FormMain::switch_segLocalRrow_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeCore::GetInst()->ModeSwitch(MODE_SEG_LCLRGROW);
  RedrawMainPanel();
}

System::Void FormMain::switch_segSwallowOrgans_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeCore::GetInst()->ModeSwitch(MODE_SEG_SWALLOW );
  RedrawMainPanel();
}

System::Void FormMain::switch_refStrokeTrim_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeCore::GetInst()->ModeSwitch(MODE_REF_STRKTRIM);
  RedrawMainPanel();
}

System::Void FormMain::switch_SegBolus_Click(System::Object^  sender, System::EventArgs^  e)
{
	ModeCore::GetInst()->ModeSwitch(MODE_SEG_BOLUS);
	RedrawMainPanel();
}

System::Void FormMain::switch_deformHC_Click(System::Object^  sender, System::EventArgs^  e)
{
  RedrawMainPanel();
}

System::Void FormMain::switch_segJointTracker_Click(System::Object^  sender, System::EventArgs^  e) 
{
	ModeCore::GetInst()->ModeSwitch(MODE_SEG_JTRACKER);
	RedrawMainPanel();    
}

System::Void FormMain::switch_segBronchi_Click(System::Object^ sender, System::EventArgs^ e)
{
	ModeCore::GetInst()->ModeSwitch(MODE_SEG_BRONCHI);
	RedrawMainPanel();    
}

System::Void FormMain::switch_segSwallowTempGen_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeCore::GetInst()->ModeSwitch(MODE_SEG_SWLTMPGEN);
  RedrawMainPanel();
}

System::Void FormMain::switch_segStrokeFFD_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeCore::GetInst()->ModeSwitch(MODE_SEG_STROKEFFD);
  RedrawMainPanel();
}

System::Void FormMain::switch_placeCPs_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeCore::GetInst()->ModeSwitch(MODE_PLC_CPS);
  RedrawMainPanel();
}



static void n_marshalString(String ^ s, std::string& os) {
  const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
  os = chars;
  Marshal::FreeHGlobal(IntPtr((void*)chars));
}


static bool OpenDlgMulti(
    const char* filter,
    std::vector<std::string> &out_f_names,
    std::vector<std::string> &out_f_dates)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = gcnew System::String(filter);
  dlg->Multiselect = true;

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) 
    return false;

  for each (auto it in  dlg->FileNames)
  {
    std::string fName, fTime;
    n_marshalString(it, fName);
    n_marshalString(File::GetCreationTime(it).ToString(), fTime);
    out_f_names.push_back(fName);
    out_f_dates.push_back(fTime);
    std::cout << fName.c_str() << " " << fTime.c_str() << "\n";
  }
  
  return true;
}



static bool SaveDlgSingle(
  const char* filter,
  std::string &out_f_name)
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = gcnew System::String(filter);
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) 
    return false;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  out_f_name = static_cast<const char*>(mptr.ToPointer());
  return true;
}







System::Void  FormMain::file_dcm3d_Click(
  System::Object^  sender,
  System::EventArgs^  e)
{
  const char* filter = "3d dcm files(*.dcm;*)|*.dcm;*";
  std::vector<std::string> fNames, fDates;
  if (!OpenDlgMulti(filter, fNames, fDates) ) return;

  if (fNames.size() == 1 || formSortFiles_showModalDialog(fNames, fDates)) 
  {
    std::vector<std::vector<std::string>> tmp;
    ImageCore::GetInst()->load4DCT(fNames, tmp, FT_DCM3D);
    FormVisParam::GetInst()->InitAllItemsForNewImg();
    InitCameraPosition();
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    RedrawMainPanel();
  }
}


System::Void FormMain::file_traw3d_Click(
  System::Object^ sender,
  System::EventArgs^ e)
{
  const char* filter = "traw3d files(*.traw3D_ss)|*.traw3D_ss";
  std::vector<std::string> fNames, fDates;
  if (!OpenDlgMulti(filter, fNames, fDates)) return;

  if (fNames.size() == 1 || formSortFiles_showModalDialog(fNames, fDates)) 
  {
    std::vector<std::vector<std::string>> tmp;
    ImageCore::GetInst()->load4DCT(fNames, tmp, FT_TRAW3D);
    FormVisParam::GetInst()->InitAllItemsForNewImg();
    InitCameraPosition();
    RedrawMainPanel();
  }
}


System::Void FormMain::file_mha_Click(
    System::Object^ sender, 
    System::EventArgs^ e) 
{
  const char* filter = "mha files(*.mha)|*.mha";
  std::vector<std::string> fNames, fDates;
  if (!OpenDlgMulti(filter, fNames, fDates)) return;

  if (fNames.size() == 1 || formSortFiles_showModalDialog(fNames, fDates))
  {
    std::vector<std::vector<std::string>> tmp;
    ImageCore::GetInst()->load4DCT(fNames, tmp, FT_MHA);
    FormVisParam::GetInst()->InitAllItemsForNewImg();
    InitCameraPosition();
    RedrawMainPanel();
  }
}



System::Void FormMain::file_raw8bit_Click(
  System::Object^ sender,
  System::EventArgs^ e)
{
  const char* filter = "3d 8bit raw (*.raw)|*.raw";
  std::vector<std::string> fNames, fDates;
  if (!OpenDlgMulti(filter, fNames, fDates)) return;

  if (fNames.size() == 1 || formSortFiles_showModalDialog(fNames, fDates)) 
  {
    std::vector<std::vector<std::string>> tmp;
    ImageCore::GetInst()->load4DCT(fNames, tmp, FT_RAW8BIT);
    FormVisParam::GetInst()->InitAllItemsForNewImg();
    InitCameraPosition();
    RedrawMainPanel();
  }
}




/*

topDirPath
+Dir1
-file1
-file2
-file3
-...
+Dir2
-file1
-file2
-file3
-...
+...
上記のフォルダ構造を走査しすべてのfile nameを抽出しロードする
*/

System::Void FormMain::file_dcm2D_Click
(
  System::Object^ sender,
  System::EventArgs^  e
)
{
  FolderBrowserDialog^ dlg = gcnew FolderBrowserDialog();
  dlg->Description = "『dicom slice画像の入ったフォルダ』が複数入ったフォルダを選択してください";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;


  //1. extract file paths
  std::vector<std::vector<std::string>> fnamesInDirs;

  ::cli::array<String^> ^childDirs = Directory::GetDirectories(dlg->SelectedPath);
  if (childDirs->Length > 1) {
    std::vector<std::string> dirNames(childDirs->Length), dirDates(childDirs->Length);
    for (int i = 0; i < childDirs->Length; ++i)
    {
      n_marshalString(childDirs[i], dirNames[i]);
      n_marshalString(File::GetCreationTime(childDirs[i]).ToString(), dirDates[i]);
    }

    if (!formSortFiles_showModalDialog(dirNames, dirDates)) return;
  }


  for (int i = 0; i < childDirs->Length; i++)
  {
    ::cli::array<String^>^ file = Directory::GetFiles(childDirs[i]);
    fnamesInDirs.push_back(std::vector<std::string>());
    for (int i = 0; i < file->Length; i++)
    {
      IntPtr mptr = Marshal::StringToHGlobalAnsi(file[i]);
      std::string fname = static_cast<const char*>(mptr.ToPointer());
      int posI = (int)fname.find_last_of(".");
      //int size = fname.length();
      if (posI == -1) {
        fnamesInDirs.back().push_back(fname);
        std::cout << fname.c_str() << " " << posI << "\n";
      }
      else if (fname.substr(posI) == ".dcm" || fname.substr(posI) == ".DCM") {
        fnamesInDirs.back().push_back(fname);
        std::cout << fname.substr(posI).c_str() << "\n";
      }
    }
  }



  //2. start and end index
  const int frameN = (int)fnamesInDirs.size();
  int startI, endI;
  FormLoadFrameIdx ^modal = gcnew FormLoadFrameIdx(frameN);
  if (modal->ShowDialog() != System::Windows::Forms::DialogResult::OK) return;

  modal->setIndex(startI, endI);
  if (startI > endI) return;

  modal->Close();

  std::vector<std::vector<std::string>> fnamesInDirs_trimed;
  for (int i = startI; i <= endI; ++i)
  {
    fnamesInDirs_trimed.push_back(fnamesInDirs[i]);
  }

  //3. load file
  ImageCore::GetInst()->load4DCT(std::vector<std::string>(), fnamesInDirs_trimed, FT_DCM2D);
  FormVisParam::GetInst()->InitAllItemsForNewImg();
  InitCameraPosition();
  RedrawMainPanel();
}


//SAVE Mask 
System::Void FormMain::file_saveMask_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  const char* filter = "4d mask file (*.msk4)|*.msk4";
  std::string fname;
  if( !SaveDlgSingle(filter, fname) ) return;

  ImageCore::GetInst()->SaveMask_Msk4(fname);
}


System::Void FormMain::file_saveMasktrawub_Click(
    System::Object^ sender,
    System::EventArgs^ e)
{
  const char* filter = "mask file (*.traw3d_ub)|*";
  std::string fname;
  if (!SaveDlgSingle(filter, fname)) return;

  ImageCore::GetInst()->SaveMask_Trawub(fname);
}


System::Void FormMain::file_saveMaskMha_Click(
    System::Object^ sender, 
    System::EventArgs^ e) 
{
  const char* filter = "mask files (*.mha) |*";
  std::string fname;
  if (!SaveDlgSingle(filter, fname)) return;

  ImageCore::GetInst()->SaveMask_Mha(fname);
}




//Load Mask 
System::Void FormMain::file_loadMask_Click(System::Object^  sender, System::EventArgs^  e)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = "4D mask file (*.msk4)|*.msk4";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());

  const int frameI = FormVisParam::GetInst()->GetframeI();

  ImageCore::GetInst()->LoadMask_Msk4(fname, frameI);
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  RedrawMainPanel();
}


System::Void FormMain::file_loadMaskMha_Click(System::Object^ sender, System::EventArgs^ e)
{
  const char* filter = "mha files (*.mha)|*.mha";
  std::vector<std::string> fnames, fdates;
  if (!OpenDlgMulti(filter, fnames, fdates)) return;

  const int frameI = FormVisParam::GetInst()->GetframeI();
  ImageCore::GetInst()->LoadMask_Mha(fnames, frameI);
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  RedrawMainPanel();
}


System::Void FormMain::file_loadMasktrawub_Click(
  System::Object^ sender,
  System::EventArgs^ e)
{
  const char* filter = "mask files (*.traw3d_ub)|*.traw3d_ub";
  std::vector<std::string> fnames, fdates;
  if (!OpenDlgMulti(filter, fnames, fdates)) return;

  const int frameI = FormVisParam::GetInst()->GetframeI();
  ImageCore::GetInst()->LoadMask_Trawub(fnames, frameI);
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
  RedrawMainPanel();

}


System::Void FormMain::file_export4dcttraw_Click(
  System::Object^ sender, 
  System::EventArgs^ e)
{
  const char* filter = "traw3d_ss |*";
  std::string fname;
  if (!SaveDlgSingle(filter, fname)) return;

  ImageCore::GetInst()->SaveImg4D_traw3dss(fname);
}

System::Void FormMain::file_export4dct_mha_Click(
  System::Object^ sender,
  System::EventArgs^ e)
{
  const char* filter = "mha |*";
  std::string fname;
  if (!SaveDlgSingle(filter, fname)) return;

  ImageCore::GetInst()->SaveImg4D_mha(fname);
}

















System::Void FormMain::FormMain_Resize(System::Object^  sender, System::EventArgs^  e)
{
  ReplaceOtherForms();
}

System::Void FormMain::FormMain_Move(System::Object^  sender, System::EventArgs^  e)
{
  ReplaceOtherForms();
}

System::Void FormMain::FormMain_KeyDown (
    System::Object^  sender, 
    System::Windows::Forms::KeyEventArgs^  e)
{
  if (m_prevKeyID == (int)e->KeyCode) return;
  if (      (int)e->KeyCode == 37) FormVisParam::GetInst()->ChangeFrameI(-1);
  else if ( (int)e->KeyCode == 39) FormVisParam::GetInst()->ChangeFrameI(+1);

  ModeCore::GetInst()->KeyDown((int)e->KeyCode);
  m_prevKeyID = (int)e->KeyCode;
  RedrawMainPanel();
}

System::Void FormMain::FormMain_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
{
  ModeCore::GetInst()->KeyUp((int)e->KeyCode);
  m_prevKeyID = -1;
  RedrawMainPanel();
}

System::Void FormMain::FormMain_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
{

}

//Event Handler
System::Void FormMain::FormMainPanel_Paint(
    System::Object^ sender, 
    System::Windows::Forms::PaintEventArgs^ e)
{
  RedrawMainPanel();
}

System::Void FormMain::FormMainPanel_Resize(
    System::Object^ sender, 
    System::EventArgs^ e)
{
  if (m_ogl != 0) RedrawMainPanel();
}


System::Void FormMain::FormMainPanel_MouseUp(
    System::Object^ sender, 
    System::Windows::Forms::MouseEventArgs^ e)
{
  if (e->Button == System::Windows::Forms::MouseButtons::Left) 
    ModeCore::GetInst()->LBtnUp(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Middle) 
    ModeCore::GetInst()->MBtnUp(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Right) 
    ModeCore::GetInst()->RBtnUp(EVec2i(e->X, e->Y), m_ogl);
}

System::Void FormMain::FormMainPanel_MouseMove(
    System::Object^ sender, 
    System::Windows::Forms::MouseEventArgs^ e)
{
  ModeCore::GetInst()->MouseMove(EVec2i(e->X, e->Y), m_ogl);
}

System::Void FormMain::FormMainPanel_MouseDown(
    System::Object^ sender, 
    System::Windows::Forms::MouseEventArgs^ e)
{
  if (e->Button == System::Windows::Forms::MouseButtons::Left) 
    ModeCore::GetInst()->LBtnDown(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Middle) 
    ModeCore::GetInst()->MBtnDown(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Right) 
    ModeCore::GetInst()->RBtnDown(EVec2i(e->X, e->Y), m_ogl);
}

System::Void FormMain::FormMainPanel_DblClick(
    System::Object^ sender, 
    System::Windows::Forms::MouseEventArgs^ e)
{
  if (e->Button == System::Windows::Forms::MouseButtons::Left)
  {
    if (!PickViewIndicator(e->X, e->Y)) ModeCore::GetInst()->LBtnDblClk(EVec2i(e->X, e->Y), m_ogl);
  }
  if (e->Button == System::Windows::Forms::MouseButtons::Middle) ModeCore::GetInst()->MBtnDblClk(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Right) ModeCore::GetInst()->RBtnDblClk(EVec2i(e->X, e->Y), m_ogl);
}

System::Void FormMain::FormMain_MouseWheel(
    System::Object^ sender, 
    System::Windows::Forms::MouseEventArgs^ e)
{
  int x = e->X - this->FormMainPanel->Location.X;
  int y = e->Y - this->FormMainPanel->Location.Y;
  ModeCore::GetInst()->MouseWheel(EVec2i(x, y), e->Delta, m_ogl);
}



