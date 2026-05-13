#include "FormMdlFitMuscleModel.h"
#include "FormVisParam.h"
#include "FormMain.h"

#pragma unmanaged
#include "mode/ModeMdlFitMuscleModel.h"
#pragma managed

using namespace RoiPainter3D;
using namespace System::Runtime::InteropServices;


System::Void FormMdlFitMuscleModel::m_btn_import_obj_Click(System::Object^ sender, System::EventArgs^ e)
{
  FolderBrowserDialog^ fbd = gcnew FolderBrowserDialog();
  fbd->Description = "OBJファイルが含まれるフォルダを選択してください ";

  if (fbd->ShowDialog() == System::Windows::Forms::DialogResult::OK)
  {
    IntPtr mptr = Marshal::StringToHGlobalAnsi(fbd->SelectedPath);
    try {
      std::string folderPath = static_cast<const char*>(mptr.ToPointer());
      ModeMdlFitMuscleModel::GetInst()->ImportAllObjInFolder(folderPath);
    }
    finally {
      Marshal::FreeHGlobal(mptr);
    }
  }
}


System::Void FormMdlFitMuscleModel::m_btn_reset_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeMdlFitMuscleModel::GetInst()->ModelReset();
}


System::Void FormMdlFitMuscleModel::m_trackBar_isovalue_Scroll(System::Object^ sender, System::EventArgs^ e)
{
  m_textBox_isovalue->Text = m_trackBar_isovalue->Value.ToString();
}


System::Void FormMdlFitMuscleModel::m_btn_gen_iso_Click(System::Object^ sender, System::EventArgs^ e)
{
  int value = m_trackBar_isovalue->Value;
  bool do_halfen = m_checkBox_half_iso->Checked;
  ModeMdlFitMuscleModel::GetInst()->GenIsoSurface(value, do_halfen);
}


System::Void FormMdlFitMuscleModel::m_textBox_isovalue_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
  int x;
  if (Int32::TryParse(m_textBox_isovalue->Text, x))
  {
    if (x < m_trackBar_isovalue->Minimum)
    {
      x = m_trackBar_isovalue->Minimum;
      m_textBox_isovalue->Text = (x).ToString();
    }
    else if (x > m_trackBar_isovalue->Maximum)
    {
      x = m_trackBar_isovalue->Maximum;
      m_textBox_isovalue->Text = (x).ToString();
    }
    m_trackBar_isovalue->Value = x;
  }
  else
  {
    m_textBox_isovalue->Text = m_trackBar_isovalue->Value.ToString();
  }
}



System::Void FormMdlFitMuscleModel::m_btn_export_lmks_Click(System::Object^ sender, System::EventArgs^ e)
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "LMK info (*.txt)|*.txt";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeMdlFitMuscleModel::GetInst()->ExportLandmarks(fname);
}


System::Void FormMdlFitMuscleModel::m_btn_import_lmks_Click(System::Object^ sender, System::EventArgs^ e)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = "LMK info (*.txt)|*.txt";

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeMdlFitMuscleModel::GetInst()->ImportLandmarks(fname);
}


System::Void FormMdlFitMuscleModel::m_btn_finish_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeMdlFitMuscleModel::GetInst()->FinishSegmentation();
}