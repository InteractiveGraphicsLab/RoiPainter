#include "FormMdlPlaceLMKs.h"
#include "FormVisParam.h"
#include "FormMain.h"

#pragma unmanaged
#include "mode/ModeMdlPlaceLMKs.h"
#pragma managed

using namespace RoiPainter3D;
using namespace System::Runtime::InteropServices;

System::Void FormMdlPlaceLMKs::m_trackbar_setIsoValue__Scroll(System::Object^ sender, System::EventArgs^ e) {
  m_textbox_setIsoValue->Text = m_trackbar_setIsoValue->Value.ToString();
}


System::Void FormMdlPlaceLMKs::m_btn_genIsosurface_Click(System::Object^ sender, System::EventArgs^ e) {
  int value = m_trackbar_setIsoValue->Value;
  ModeMdlPlaceLMKs::GetInst()->GenIsoSurFace(value);
}


System::Void FormMdlPlaceLMKs::m_textbox_setIsoValue_TextChanged(System::Object^ sender, System::EventArgs^ e) {
  int x;
  if (Int32::TryParse(m_textbox_setIsoValue->Text, x)) {
    if (x < m_trackbar_setIsoValue->Minimum) {
      x = m_trackbar_setIsoValue->Minimum;
      m_textbox_setIsoValue->Text = (x).ToString();
    } else if (x > m_trackbar_setIsoValue->Maximum){
      x = m_trackbar_setIsoValue->Maximum;
      m_textbox_setIsoValue->Text = (x).ToString();
    }
    m_trackbar_setIsoValue->Value = x;
  } else {
    m_textbox_setIsoValue->Text = m_trackbar_setIsoValue->Value.ToString();
  }
}


System::Void FormMdlPlaceLMKs::m_btn_import_Click(System::Object^ sender, System::EventArgs^ e) {
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = "LMK info (*.txt)|*.txt";
  
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeMdlPlaceLMKs::GetInst()->ImportLandmark(fname);
}


System::Void FormMdlPlaceLMKs::m_btn_export_Click(System::Object^ sender, System::EventArgs^ e) {
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "LMK info (*.txt)|*.txt";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeMdlPlaceLMKs::GetInst()->ExportLandmark(fname);
}


System::Void FormMdlPlaceLMKs::m_btn_finish_Click(System::Object^ sender, System::EventArgs^ e) {
  ModeMdlPlaceLMKs::GetInst()->FinishSegmentation();
}

