#pragma managed
#include "FormSegBolus.h"
#include "FormMain.h"

#pragma unmanaged
#include "./Mode/ModeSegBolus.h"
#include <string>
#include <vector>
#pragma managed

#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>


using namespace System;
using namespace msclr::interop;

namespace RoiPainter4D
{ 

static std::string openFileDialog(const char* filterExpanded, const char* dlgTitle)
{
	String^ expanded = gcnew String(filterExpanded);
	OpenFileDialog^ dlg = gcnew OpenFileDialog();
	dlg->Filter = "cylinder info files(*." + expanded + ";)|*." + expanded;
	dlg->Title = gcnew String(dlgTitle);
	dlg->Multiselect = false;

	if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return "";

	IntPtr mptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dlg->FileName);
	std::string fname = static_cast<const char*>(mptr.ToPointer());

	return fname;
}

static const float TRACKBAR_RATIO = 10.0f;


FormSegBolus::FormSegBolus(void)
{
  InitializeComponent();
}

FormSegBolus::~FormSegBolus()
{
  if (components)
  {
    delete components;
  }
}


bool  FormSegBolus::IsDrawOBB() 
{ 
  return m_check_drawobbs->Checked; 
}

bool  FormSegBolus::IsNormalVolRend() 
{ 
  return m_check_normalvolrend->Checked; 
}

float FormSegBolus::GetCylinderAlpha() 
{ 
  return m_trackbar_cylinder_trans->Value / (float)m_trackbar_cylinder_trans->Maximum; 
}

int   FormSegBolus::GetLockAreaBand() 
{ 
  return m_trackbar_lockareaband->Value; 
}





void FormSegBolus::InitAllItems(int volmin, int volmax, float radmin, float radmax)
{
  std::cout << "INIT ALL ITEMS!!!";
	this->ControlBox  = false;
	this->MaximizeBox = false;

  //chekboxes
  m_check_drawobbs      ->Checked = true;
  m_check_normalvolrend ->Checked = false;
    
  //trackbars threshold
	m_trackbar_minthresh->SetRange(volmin,volmax) ;
	m_trackbar_maxthresh->SetRange(volmin,volmax) ;
	m_trackbar_minthresh->Value = volmin;
	m_trackbar_maxthresh->Value = volmax;
	m_textbox_maxthresh->Text   = m_trackbar_maxthresh->Value.ToString();
	m_textbox_minthresh->Text   = m_trackbar_minthresh->Value.ToString();

  //trackbar lock area thickness
  m_trackbar_lockareaband->SetRange(0, 10);
  m_trackbar_lockareaband->Value = 1;
  m_textbox_lockareaband->Text = gcnew String(std::to_string(m_trackbar_lockareaband->Value).c_str());

  //cylinder trackbar
	m_trackbar_cylinder_radius1   ->SetRange( int(radmin*TRACKBAR_RATIO ), (int)(radmax * TRACKBAR_RATIO ) );
	m_trackbar_cylinder_radius2   ->SetRange( int(radmin*TRACKBAR_RATIO ), (int)(radmax * TRACKBAR_RATIO ) );
	m_trackbar_cylinder_twistangle->SetRange( 0, 360 );
  m_trackbar_cylinder_trans     ->SetRange( 0, 10  );
	m_trackbar_cylinder_radius1->Value = int( radmin*TRACKBAR_RATIO);
	m_trackbar_cylinder_radius2->Value = int( radmin*TRACKBAR_RATIO);
	m_trackbar_cylinder_twistangle->Value = 0;
  m_trackbar_cylinder_trans->Value      = 3;
}


void FormSegBolus::SetItemsForCylinderProparty(bool is_active, float radius1, float radius2, float twist_angle)
{
  try { m_trackbar_cylinder_radius1->Value = (int)( (int)(radius1 * TRACKBAR_RATIO) ); }
  catch (const System::ArgumentOutOfRangeException^){}
	try { m_trackbar_cylinder_radius2->Value = (int)( (int)(radius2 * TRACKBAR_RATIO) ); }
	catch (const System::ArgumentOutOfRangeException^) {}
	try { m_trackbar_cylinder_twistangle->Value = (int)(360 * twist_angle / M_PI); }
	catch (const System::ArgumentOutOfRangeException^) {}
    
  if ( is_active ){
    m_trackbar_cylinder_radius1->Show();
    m_trackbar_cylinder_radius2->Show();
    m_trackbar_cylinder_twistangle->Show();
  } else {
    m_trackbar_cylinder_radius1->Hide();
    m_trackbar_cylinder_radius2->Hide();
    m_trackbar_cylinder_twistangle->Hide();
  }
  m_trackbar_cylinder_trans->Show();
}


//////////////イベントハンドラ/////////////////////
System::Void FormSegBolus::m_btn_thresholding_allframe_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
	ModeSegBolus::getInst()->RunThresholdingInCylinderAllFrame( m_trackbar_minthresh->Value, m_trackbar_maxthresh->Value);
}

System::Void FormSegBolus::m_btn_thresholding_currframe_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
	ModeSegBolus::getInst()->RunThresholdingInClyinderOneFrame( m_trackbar_minthresh->Value, m_trackbar_maxthresh->Value);
}

System::Void FormSegBolus::m_btn_regiongrow_allframe_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  ModeSegBolus::getInst()->RunRegionGrowingInCylinderAllFrame( m_trackbar_minthresh->Value, m_trackbar_maxthresh->Value);
}

System::Void FormSegBolus::m_btn_regiongrow_currframe_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  ModeSegBolus::getInst()->RunRegionGrowingInCylinderOneFrame( m_trackbar_minthresh->Value, m_trackbar_maxthresh->Value);
}
  
System::Void FormSegBolus::m_btn_regiongrow4d_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  ModeSegBolus::getInst()->RunRegionGrowingInCylinder4D(m_trackbar_minthresh->Value, m_trackbar_maxthresh->Value);
}

System::Void FormSegBolus::m_btn_erode_currframe_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  ModeSegBolus::getInst()->RunErosionOneFrame();
}

System::Void FormSegBolus::m_btn_dilate_currframe_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  ModeSegBolus::getInst()->RunDilationOneFrame();
}

System::Void FormSegBolus::m_btn_erode_allframe_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  ModeSegBolus::getInst()->RunErosionAllFrame();
}

System::Void FormSegBolus::m_btn_dilate_allframe_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  ModeSegBolus::getInst()->RunDilationAllFrame();
}

System::Void FormSegBolus::m_btn_createnewcylinder_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
	ModeSegBolus::getInst()->AddNewCylinder();
}

  
System::Void FormSegBolus::m_btn_loadcylinder_fit_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = "cylinder info files(*.txt;)|*.txt;";
  dlg->Multiselect = false;

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
	ModeSegBolus::getInst()->LoadCylinderInfoFromTextFile(fname, true);
}


System::Void FormSegBolus::m_btn_loadcylinder_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = "cylinder info files(*.txt;)|*.txt;";
  dlg->Multiselect = false;

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeSegBolus::getInst()->LoadCylinderInfoFromTextFile(fname, false);
}


System::Void FormSegBolus::m_btn_savecylinder_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
	SaveFileDialog^ dlg = gcnew SaveFileDialog();
	dlg->Filter = "cylinder info files(*.txt;)|*.txt;";
	dlg->AddExtension = true;
	dlg->DefaultExt = "txt";
	dlg->CheckFileExists = false;

	if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

	IntPtr mptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
	ModeSegBolus::getInst()->SaveCylinderInfoAsTextFile(fname);
}


System::Void FormSegBolus::m_btn_cancel_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
	ModeSegBolus::getInst()->CancelSegmentation();
}

System::Void FormSegBolus::m_btn_finish_Clicked(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  m_btn_savecylinder_Clicked(sender, e);
	ModeSegBolus::getInst()->FinishSegmentation();
}

System::Void FormSegBolus::m_trackbar_cylinder_radius1_Scroll(
    System::Object^  sender, 
    System::EventArgs^  e)
{
	ModeSegBolus::getInst()->SetRadius1( m_trackbar_cylinder_radius1->Value / (float)TRACKBAR_RATIO);
}

System::Void FormSegBolus::m_trackbar_cylinder_radius2_Scroll(
    System::Object^  sender, 
    System::EventArgs^  e)
{
	ModeSegBolus::getInst()->SetRadius2( m_trackbar_cylinder_radius2->Value / (float)TRACKBAR_RATIO);
}
	
System::Void FormSegBolus::m_trackbar_cylinder_twistangle_Scroll(
    System::Object^  sender, 
    System::EventArgs^  e)
{
	ModeSegBolus::getInst()->SetTwistAngle( (float)( M_PI * m_trackbar_cylinder_twistangle->Value / 360 ) );
}

System::Void FormSegBolus::m_trackbar_cylinder_trans_Scroll(
    System::Object^  sender,
    System::EventArgs^  e)
{
  float transparency = (float)(m_trackbar_cylinder_trans->Value) / m_trackbar_cylinder_trans->Maximum;
  formMain_RedrawMainPanel();
}
  
  
System::Void FormSegBolus::m_textbox_minthresh_TextChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  if( m_textbox_minthresh->Text->Equals("") || m_textbox_minthresh->Text->Equals("-") ) return ;

  int x;
  if ( Int32::TryParse( m_textbox_minthresh->Text, x) ) 
  {
    if ( x < m_trackbar_minthresh->Minimum ) 
      x = m_trackbar_minthresh->Minimum;
    else if (x > m_trackbar_minthresh->Maximum) 
      x = m_trackbar_minthresh->Maximum;      
  }
  else 
  {
    x = m_trackbar_minthresh->Minimum;
  }
  m_textbox_minthresh->Text = (x).ToString();
  m_trackbar_minthresh->Value = x;
}


System::Void FormSegBolus::m_textbox_maxthresh_TextChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  if( m_textbox_maxthresh->Text->Equals("") || m_textbox_maxthresh->Text->Equals("-") ) return ;

  int x;
  if ( Int32::TryParse( m_textbox_maxthresh->Text, x) ) 
  {
    if ( x < m_trackbar_maxthresh->Minimum ) 
      x = m_trackbar_maxthresh->Minimum;
    else if (x > m_trackbar_maxthresh->Maximum) 
      x = m_trackbar_maxthresh->Maximum;      
  }
  else 
  {
    x = m_trackbar_maxthresh->Maximum;
  }
  m_textbox_maxthresh->Text = (x).ToString();
  m_trackbar_maxthresh->Value = x;
}
  

System::Void FormSegBolus::m_textbox_lockareaband_TextChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  if( m_textbox_lockareaband->Text->Equals("") || m_textbox_lockareaband->Text->Equals("-") ) return ;

  int x;
  if ( Int32::TryParse( m_textbox_lockareaband->Text, x) ) 
  {
    if ( x < m_trackbar_lockareaband->Minimum ) 
      x = m_trackbar_lockareaband->Minimum;
    else if (x > m_trackbar_lockareaband->Maximum) 
      x = m_trackbar_lockareaband->Maximum;      
  }
  else 
  {
    x = m_trackbar_lockareaband->Maximum;
  }
  m_textbox_lockareaband->Text = (x).ToString();
  m_trackbar_lockareaband->Value = x;
}


System::Void FormSegBolus::m_trackbar_minthresh_Scroll(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  m_textbox_minthresh->Text = m_trackbar_minthresh->Value.ToString();
  m_textbox_minthresh->Refresh();
}
  
System::Void FormSegBolus::m_trackbar_maxthresh_Scroll(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  m_textbox_maxthresh->Text = m_trackbar_maxthresh->Value.ToString();
  m_textbox_maxthresh->Refresh();
}

System::Void FormSegBolus::m_trackbar_lockareaband_Scroll(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  m_textbox_lockareaband->Text = m_trackbar_lockareaband->Value.ToString();
  m_textbox_lockareaband->Refresh();
}
  
System::Void FormSegBolus::M_check_drawobbs_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  formMain_RedrawMainPanel();
}

System::Void FormSegBolus::M_check_normalvolrend_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  formMain_RedrawMainPanel();
}
}