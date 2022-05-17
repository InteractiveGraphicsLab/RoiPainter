#pragma managed
#include "FormSegJointTracker.h"
#include "FormMain.h"
#include "FormLoadFrameIdx.h"

#pragma unmanaged
#include "ImageCore.h"
#include "Mode/ModeSegJointTracker.h"
#pragma managed

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace RoiPainter4D;


void FormSegJointTracker::InitializeItems(int volminv, int volmaxv)
{
  m_mode_placecp_bone1->Checked = true ;
  m_mode_placecp_bone2->Checked = false;
  m_mode_rotobj->Checked = false;
  m_mode_rotbone1->Checked = false;
  m_mode_rotbone2->Checked = false;
  m_mode_transobj->Checked = false;
  m_mode_transbone1->Checked = false;
  m_mode_transbone2->Checked = false;

  m_updown_icplv->Minimum = 1 ;
  m_updown_icplv->Maximum = 10;
  m_updown_icplv->Value   =  8;

  m_bvis_cps->Checked = true;
  m_bvis_cptrajectory->Checked = true;
  m_bvis_isosurface->Checked = true;
  m_bvis_fitsurface->Checked = true;
  m_bvis_evalsurface->Checked = true;
  m_bvis_evaldiff->Checked = true;
  
  m_updown_trajectorycpid->Minimum = 0   ;
  m_updown_trajectorycpid->Maximum = 100 ;
  m_updown_trajectorycpid->Value   = 20  ;

  m_updown_rejectscale->Increment = Decimal( 0.1 );
  m_updown_rejectscale->DecimalPlaces = 1;
  m_updown_rejectscale->Minimum = 1      ;
  m_updown_rejectscale->Maximum = 100    ;
  m_updown_rejectscale->Value   = 20     ;

  m_updown_isovalue->Minimum = volminv;
  m_updown_isovalue->Maximum = volmaxv;
  m_updown_isovalue->Value   = 0      ;
  
}


///////////////////////////////////////////
///////////////switch mode/////////////////
System::Void FormSegJointTracker::m_mode_placecp_bone1_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
  formMain_RedrawMainPanel();
}
System::Void FormSegJointTracker::m_mode_placecp_bone2_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
  formMain_RedrawMainPanel();
}
System::Void FormSegJointTracker::m_mode_transbone1_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
  formMain_RedrawMainPanel();
}
System::Void FormSegJointTracker::m_mode_rotbone1_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
  formMain_RedrawMainPanel();
}
System::Void FormSegJointTracker::m_mode_transbone2_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
  formMain_RedrawMainPanel();
}
System::Void FormSegJointTracker::m_mode_rotbone2_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
  formMain_RedrawMainPanel();
}
System::Void FormSegJointTracker::m_mode_transobj_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
  formMain_RedrawMainPanel();
}
System::Void FormSegJointTracker::m_mode_rotobj_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
  formMain_RedrawMainPanel();
}


///////////////////////////////////////////
//numerical up down control ///////////////

System::Void FormSegJointTracker::m_updown_isovalue_ValueChanged(System::Object^  sender, System::EventArgs^  e){}
System::Void FormSegJointTracker::m_updown_rejectscale_ValueChanged(System::Object^  sender, System::EventArgs^  e){}
System::Void FormSegJointTracker::m_updown_icplv_ValueChanged(System::Object^  sender, System::EventArgs^  e){}


///////////////////////////////////////////
///////////////Buttons  ///////////////////

System::Void FormSegJointTracker::m_button_genisosurf_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  short isovalue = (short) m_updown_isovalue->Value;
  ModeSegJointTracker::GetInst()->IsoSurf_Generate(isovalue);

  formMain_RedrawMainPanel();
}



System::Void FormSegJointTracker::m_button_runicp_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  int   icplevel  = (int)m_updown_icplv->Value;
  float icpreject = (float)m_updown_rejectscale->Value;
  ModeSegJointTracker::GetInst()->RunICP_all(icplevel, icpreject);  
  formMain_RedrawMainPanel();
}


System::Void FormSegJointTracker::m_button_runicp_curronly_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  int   icplevel  = (int)m_updown_icplv->Value;
  float icpreject = (float)m_updown_rejectscale->Value;
  ModeSegJointTracker::GetInst()->RunICP_currenrt(icplevel, icpreject);  
  formMain_RedrawMainPanel();
}




System::Void FormSegJointTracker::m_button_exportcps_Click(System::Object^  sender, System::EventArgs^  e)
{
	SaveFileDialog^ dlg = gcnew SaveFileDialog();
	dlg->Filter = "cp info file (*.txt;)|*.txt;";
	dlg->AddExtension = true;
	dlg->DefaultExt = "txt";
	dlg->CheckFileExists = false;
	if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
	IntPtr mptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dlg->FileName);
	std::string fname = static_cast<const char*>(mptr.ToPointer());


  ModeSegJointTracker::GetInst()->ExportCPsInfo(fname);
  formMain_RedrawMainPanel();
}



System::Void FormSegJointTracker::m_button_importcps_Click(System::Object^  sender, System::EventArgs^  e)
{
	OpenFileDialog^ dlg = gcnew OpenFileDialog();
	dlg->Filter = "cp data file (*.txt)|*.txt";
	dlg->Multiselect = false;
	if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
	IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
	std::string fname = static_cast<const char*>(mptr.ToPointer());

  
  ModeSegJointTracker::GetInst()->ImportCPsInfo(fname);
  formMain_RedrawMainPanel();
}



System::Void FormSegJointTracker::m_button_loadobj_Click(System::Object^  sender, System::EventArgs^  e)
{
	OpenFileDialog^ dlg = gcnew OpenFileDialog();
	dlg->Filter = "obj mesh file (*.obj)|*.obj";
	dlg->Multiselect = false;
	if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
	IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
	std::string fname = static_cast<const char*>(mptr.ToPointer());

  
  ModeSegJointTracker::GetInst()->EvalObj_Load( fname );
  formMain_RedrawMainPanel();
}


System::Void FormSegJointTracker::m_button_fitobjcalcerror_Click(System::Object^  sender, System::EventArgs^  e)
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "fitting difference info (*.txt)|*.txt";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());

  
  int   icplevel  = (int)m_updown_icplv->Value;
  float icpreject = (float)m_updown_rejectscale->Value;
  ModeSegJointTracker::GetInst()->EvalObj_RunAnalysis( fname, icplevel, icpreject );
  formMain_RedrawMainPanel();
}



System::Void FormSegJointTracker::m_bvis_isosurface_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
  formMain_RedrawMainPanel();
}

System::Void FormSegJointTracker::m_bvis_fitsurface_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
  formMain_RedrawMainPanel();
}
System::Void FormSegJointTracker::m_bvis_cps_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
  formMain_RedrawMainPanel();
}
System::Void FormSegJointTracker::m_bvis_cptrajectory_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
  formMain_RedrawMainPanel();
}
System::Void FormSegJointTracker::m_bvis_evalsurface_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
  formMain_RedrawMainPanel();
}
System::Void FormSegJointTracker::m_bvis_evaldiff_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
  formMain_RedrawMainPanel();
}
System::Void FormSegJointTracker::m_updown_trajectorycpid_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
  formMain_RedrawMainPanel();
}

