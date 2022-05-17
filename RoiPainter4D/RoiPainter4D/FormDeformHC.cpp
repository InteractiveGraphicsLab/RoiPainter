#include "FormDeformHC.h"
#include "FormVisParam.h"
#include "FormMain.h"

#include "Mode/ModeDeformHC.h"

#include "CliMessageBox.h"
#include "ImageCore.h"

using namespace RoiPainter4D;
using namespace System::Runtime::InteropServices;

System::Void FormDeformHC::InitializeItems() {
}


System::Void FormDeformHC::btn_FinishAndStore_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeDeformHC::GetInstance()->FinishSegmentation();
}


System::Void FormDeformHC::btn_Cancel_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeDeformHC::GetInstance()->CancelSegmentation();
}


System::Void FormDeformHC::btn_LoadMesh_Click(System::Object^  sender, System::EventArgs^  e) 
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = "obj files(*.obj;)|*.obj;";
  dlg->Multiselect = false;

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;


  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeDeformHC::GetInstance()->LoadMesh(fname);

  checkBox_DeformMode->Checked = false;

  formMain_RedrawMainPanel();
  CLI_MessageBox_OK_Show("Mesh を覆うように Cage頂点 を配置してください", "caution");
}

System::Void RoiPainter4D::FormDeformHC::btn_SaveMesh_Click(System::Object ^ sender, System::EventArgs ^ e)
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "save data (*.obj)|*.obj||";

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeDeformHC::GetInstance()->SaveMesh(fname);
  CLI_MessageBox_OK_Show("finish!", "caution");
}




System::Void FormDeformHC::btn_LoadCage_Click(System::Object^  sender, System::EventArgs^  e) {
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = "obj files(*.obj;)|*.obj;";
  dlg->Multiselect = false;

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;


  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeDeformHC::GetInstance()->LoadCage(fname);

  checkBox_DeformMode->Checked = false;

  formMain_RedrawMainPanel();
  CLI_MessageBox_OK_Show("finish!", "caution");
}


System::Void FormDeformHC::btn_SaveCage_Click(System::Object^ sender, System::EventArgs^  e) {
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "cage data (*.obj)|*.obj||";

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeDeformHC::GetInstance()->SaveCage(fname);
  CLI_MessageBox_OK_Show("finish!", "caution");
}





System::Void FormDeformHC::checkBox_DeformMode_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
  if (ModeDeformHC::GetInstance()->ChangeDeformMode(checkBox_DeformMode->Checked) == false)
  {
    checkBox_DeformMode->Checked = false;
  }
}


System::Void FormDeformHC::btn_FillinMesh_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeDeformHC::GetInstance()->FillInMesh();
  formMain_RedrawMainPanel();
  CLI_MessageBox_OK_Show("finish!", "caution");
}

System::Void FormDeformHC::textBox_scaleVal_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
{
  //0～9と、バックスペース以外の時は、イベントをキャンセルする
  if ((e->KeyChar < '0' || '9' < e->KeyChar) && e->KeyChar != '\b' && e->KeyChar != '.')
  {
    e->Handled = true;
  }
}


System::Void FormDeformHC::btn_scale_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeDeformHC::GetInstance()->Scale((float)Convert::ToSingle(textBox_scaleVal->Text));
}
