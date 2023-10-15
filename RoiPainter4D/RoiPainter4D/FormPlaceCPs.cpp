#include "FormPlaceCPs.h"
#include "FormVisParam.h"

#pragma unmanaged
#include "Mode/ModePlaceCPs.h"
#pragma managed

using namespace RoiPainter4D;



System::Void FormPlaceCPs::trackbar_setisoValue__Scroll(System::Object^ sender, System::EventArgs^ e)
{
  textbox_setisovalue_->Text = trackbar_setisoValue_->Value.ToString();
}


System::Void FormPlaceCPs::button_generateisosurfaces__Click(System::Object^ sender, System::EventArgs^ e)
{
  int value = trackbar_setisoValue_->Value;
  ModePlaceCPs::GetInst()->IsosurfaceGenerateAllFrame(value);
}


System::Void FormPlaceCPs::button_generate_test__Click(System::Object^ sender, System::EventArgs^ e)
{
  int value = trackbar_setisoValue_->Value;
  int fidx  = FormVisParam::GetInst()->GetframeI();
  ModePlaceCPs::GetInst()->IsosurfaceGenerateOneFrame(value, fidx);
}


using namespace System::Runtime::InteropServices;


System::Void FormPlaceCPs::button_export_cpinfo__Click(System::Object^ sender, System::EventArgs^ e)
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "CP info (*.txt)|*.txt";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModePlaceCPs::GetInst()->ExportControlPoints(fname);
}


System::Void FormPlaceCPs::button_inport_cpinfo__Click(System::Object^ sender, System::EventArgs^ e)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = "CP info (*.txt)|*.txt";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModePlaceCPs::GetInst()->ImportControlPoints(fname);
}



System::Void FormPlaceCPs::button_finish_placement__Click(System::Object^ sender, System::EventArgs^ e)
{
  ModePlaceCPs::GetInst()->FinishSegmentation();
}






void FormPlaceCPs::InitParams()
{
  trackbar_setisoValue_->SetRange(FormVisParam::GetInst()->GetWinLvMin(), 
                                  FormVisParam::GetInst()->GetWinLvMax());
  trackbar_setisoValue_->Value = trackbar_setisoValue_->Minimum;
  textbox_setisovalue_->Text = FormVisParam::GetInst()->GetWinLvMin().ToString();
}



System::Void FormPlaceCPs::textbox_setisovalue__TextChanged(System::Object^ sender, System::EventArgs^ e)
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


System::Void FormPlaceCPs::groupBox1_Enter(System::Object^ sender, System::EventArgs^ e)
{
}

System::Void FormPlaceCPs::checkbox_modify_scale__CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
}


System::Void FormPlaceCPs::button_fit_template__Click(System::Object^ sender, System::EventArgs^ e)
{
  bool tf = this->checkbox_modify_scale_->Checked;
  ModePlaceCPs::GetInst()->FitTemplateUsingCPs(tf);
}

System::Void FormPlaceCPs::button_load_mesh__Click(System::Object^ sender, System::EventArgs^ e)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = "plygon mesh (*.obj)|*.obj";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());

  ModePlaceCPs::GetInst()->LoadTemplateMesh(fname);

}



