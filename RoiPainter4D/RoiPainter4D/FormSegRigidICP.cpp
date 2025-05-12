#pragma managed
#include "FormSegRigidICP.h"
#include "FormVisParam.h"
#include "FormMain.h"
#include "FormLoadFrameIdx.h"
#include "CliMessageBox.h"

#pragma unmanaged
#include "Mode\ModeSegRigidICP.h"
#include "ImageCore.h"
#pragma managed

using namespace RoiPainter4D;
using namespace System::Runtime::InteropServices;

void FormSegRigidICP::InitializeParameters()
{
  trackbar_setisoValue_->SetRange(FormVisParam::GetInst()->GetWinLvMin(), FormVisParam::GetInst()->GetWinLvMax());
  trackbar_setisoValue_->Value = trackbar_setisoValue_->Minimum;
  textbox_setisovalue_->Text   = FormVisParam::GetInst()->GetWinLvMin().ToString();
  
  b_loaded_source_surface_     = false;
  b_generated_all_isosurfaces_ = false;

  button_runicp_range_->Enabled       = false;
  button_runicp_current_->Enabled     = false;
  button_runicp_allPara_->Enabled     = false;
  button_finish_->Enabled             = false;
  button_finish_currentframe->Enabled = false;
  radiobutton_rotate_    ->Enabled    = false;
  radiobutton_translate_ ->Enabled    = false;
  radiobutton_transpivot_->Enabled    = false;

}



void FormSegRigidICP::ReusePrevIsoSurface( const int isovalue )
{
  trackbar_setisoValue_->Value = isovalue;
  textbox_setisovalue_->Text = trackbar_setisoValue_->Value.ToString();
  b_generated_all_isosurfaces_ = true;
}



System::Void FormSegRigidICP::BtnGenIsoSurfClick(System::Object^  sender, System::EventArgs^  e)
{
	b_generated_all_isosurfaces_ = true;

	if (b_loaded_source_surface_)
  {
    button_runicp_range_->Enabled = true;
    button_runicp_current_->Enabled = true;
    button_runicp_allPara_->Enabled = true;
  }

	ModeSegRigidICP::GetInst()->GenIsoSurface(trackbar_setisoValue_->Value, true);
	formMain_RedrawMainPanel();
}




System::Void FormSegRigidICP::BtnLoadSurfaceClick(System::Object^  sender, System::EventArgs^  e)
{
	OpenFileDialog^ dlg = gcnew OpenFileDialog();
	dlg->Filter = "obj files(*.obj;)|*.obj;";
	dlg->Multiselect = false;

	if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

	if (b_generated_all_isosurfaces_)
  {
    button_runicp_range_->Enabled = true;
    button_runicp_current_->Enabled = true;
    button_runicp_allPara_->Enabled = true;
  }

	b_loaded_source_surface_ = true;
	button_finish_->Enabled = true;
  button_finish_currentframe->Enabled = true;
	radiobutton_rotate_    ->Enabled = true;
	radiobutton_translate_ ->Enabled = true;
	radiobutton_transpivot_->Enabled = true;

	IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
	std::string fname = static_cast<const char*>(mptr.ToPointer());
	ModeSegRigidICP::GetInst()->LoadSrcSurface(fname);

	formMain_RedrawMainPanel();
}



System::Void FormSegRigidICP::BtnRunICPClick(System::Object^  sender, System::EventArgs^  e)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  int startI = formVisParam_getframeI();
  int endI   = num_frames - 1;

  FormLoadFrameIdx ^modal = gcnew FormLoadFrameIdx(num_frames, startI, endI);
  if (modal->ShowDialog() != System::Windows::Forms::DialogResult::OK) return;
  modal->setIndex(startI, endI);
  modal->Close();

  int   numLv    = 6;		// m_spin_ICPnumLv.GetPos32();
  float rejScale = 2.5;	// (float)m_spin_ICPrejectScale.GetPos32();
  ModeSegRigidICP::GetInst()->PerformTracking(rejScale, numLv, startI, endI, false);
  formMain_RedrawMainPanel();
}


System::Void FormSegRigidICP::BtnRunICPAllParaClick(System::Object^  sender, System::EventArgs^  e)
{
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  int startI = formVisParam_getframeI();
  int endI   = num_frames - 1;

  int   numLv    = 6;		// m_spin_ICPnumLv.GetPos32();
  float rejScale = 2.5;	// (float)m_spin_ICPrejectScale.GetPos32();
  ModeSegRigidICP::GetInst()->PerformTracking(rejScale, numLv, startI, endI, true);

  formMain_RedrawMainPanel();
}



System::Void FormSegRigidICP::BtnRunICPCurrentClick(System::Object^  sender, System::EventArgs^  e) 
{
  const int cur_frames = formVisParam_getframeI();
  
  int   numLv    = 6;		// m_spin_ICPnumLv.GetPos32();
  float rejScale = 2.5;	// (float)m_spin_ICPrejectScale.GetPos32();
  ModeSegRigidICP::GetInst()->PerformTracking(rejScale, numLv, cur_frames, cur_frames, false);

  formMain_RedrawMainPanel();
}




System::Void FormSegRigidICP::BtnFinishandStoreCurrentFrameClick(
    System::Object^  sender, 
    System::EventArgs^  e
)
{
  button_finish_->Enabled = false;
  button_finish_currentframe->Enabled = false;
  ModeSegRigidICP::GetInst()->FinishSegmentation(false);
}



static void ExportICPMatrixSequence()
{
	SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Title  = "Save Transform matrix ";
	dlg->Filter = "TransformMatrix file (*.txt;)|*.txt;";
	dlg->AddExtension = true;
	dlg->DefaultExt = "txt";
	dlg->CheckFileExists = false;
	
  if (  dlg->ShowDialog() != System::Windows::Forms::DialogResult::Cancel)
  {
	  IntPtr mptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dlg->FileName);
    std::string fname = static_cast<const char*>(mptr.ToPointer());
	  ModeSegRigidICP::GetInst()->ExportMatrixSequence(fname);
  }
}


System::Void FormSegRigidICP::button_export_mats__Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  ExportICPMatrixSequence();
}



System::Void FormSegRigidICP::button_import_mats__Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Title       = "Open Transform matrix ";
	dlg->Filter      = "TransformMatrix file (*.txt;)|*.txt;";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeSegRigidICP::GetInst()->ImportMatrixSequence(fname);
}


System::Void FormSegRigidICP::BtnFinishandStoreAllFrameClick(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  button_finish_->Enabled = false;
  button_finish_currentframe->Enabled = false;

  if( ShowMsgDlgYesNo( "Export rigid trans matrix as backup?", "export?") ) 
  {
    ExportICPMatrixSequence();
  }
  ModeSegRigidICP::GetInst()->FinishSegmentation(true);
}


System::Void FormSegRigidICP::BtnCancelClick(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegRigidICP::GetInst()->CancelSegmentation();
}


System::Void FormSegRigidICP::TextBoxSetIsoValueTextChanged(System::Object^  sender, System::EventArgs^  e)
{
  int x;
  if (Int32::TryParse(textbox_setisovalue_->Text, x))
  {
    if (x < trackbar_setisoValue_->Minimum)
    {
      x = trackbar_setisoValue_->Minimum;
      textbox_setisovalue_->Text = (x).ToString();
    }
    else if (x > trackbar_setisoValue_->Maximum)
    {
      x = trackbar_setisoValue_->Maximum;
      textbox_setisovalue_->Text = (x).ToString();
    }
    trackbar_setisoValue_->Value = x;
  }
  else
  {
    textbox_setisovalue_->Text = trackbar_setisoValue_->Value.ToString();
  }
}


System::Void FormSegRigidICP::TrackBarSetIsoValueScroll(System::Object^  sender, System::EventArgs^  e)
{
  textbox_setisovalue_->Text = trackbar_setisoValue_->Value.ToString();
}


System::Void FormSegRigidICP::btnGenTest_Click(System::Object^  sender, System::EventArgs^  e)
{
  b_generated_all_isosurfaces_ = false;
  button_runicp_range_->Enabled = false;
  button_runicp_current_->Enabled = false;
  button_runicp_allPara_->Enabled = false;

  ModeSegRigidICP::GetInst()->GenIsoSurface(trackbar_setisoValue_->Value, false, formVisParam_getframeI());
  formMain_RedrawMainPanel();
}

