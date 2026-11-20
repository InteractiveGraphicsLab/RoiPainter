#pragma managed
#include "FormRefCurveDeform.h"
#include "FormMain.h"
#include "CliMessageBox.h"
#include <msclr/marshal_cppstd.h>
#undef min
#undef max

#pragma unmanaged
#include "ImageCore.h"
#include "ModeCore.h"
#include "mode/ModeRefCurveDeform.h"
#pragma managed


using namespace System::Runtime::InteropServices;
using namespace RoiPainter4D;
using namespace System::IO;


void FormRefCurveDeform::InitAllItems()
{
  m_numbox_cpsize->Value = 10;
  m_trackbar_mcstride->Value = 2;
  m_cb_only_select_curve->Checked = false;
}



System::Void FormRefCurveDeform::m_btn_genmesh_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->ConvertMaskToMesh();
}

System::Void FormRefCurveDeform::m_btn_reload_mesh_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->ReloadOrigMeshCurrentFrame();
}

System::Void FormRefCurveDeform::m_cancel_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->CancelSegmentation();
}

System::Void FormRefCurveDeform::m_finish_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->FinishSegmentation();
}

System::Void FormRefCurveDeform::m_btn_deform_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->DeformAllFrame();
}
System::Void FormRefCurveDeform::m_btn_deform_all_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->DeformAllFrame();
}

System::Void FormRefCurveDeform::m_btn_undo_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->Undo_LoadSnapShot();
}

System::Void FormRefCurveDeform::m_btn_copy_from_preframe_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->CopyFromPrevFrame();
}

System::Void FormRefCurveDeform::m_btn_copy_to_allframe_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->CopyStrokesToAllFrame();
}

System::Void FormRefCurveDeform::m_cb_onlyselectedcurve_CheckedChanged(
  System::Object^ sender, System::EventArgs^ e)
{
  FormMain::GetInst()->RedrawMainPanel();
}

System::Void FormRefCurveDeform::m_numbox_cpsize_ValueChanged(
  System::Object^ sender, System::EventArgs^ e)
{
  FormMain::GetInst()->RedrawMainPanel();

}

System::Void FormRefCurveDeform::m_btn_sharestroke_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->MakeSelectedStroke_Shared();
}


System::Void FormRefCurveDeform::m_btn_flip_curve_normals_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->FlipSelectedCurveNormals();
}

System::Void FormRefCurveDeform::m_btn_toggle_curve_normals_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->ToggleSelectedCurveNormals();
}



System::Void FormRefCurveDeform::m_btn_loadstate_Click(System::Object^ sender, System::EventArgs^ e)
{
  std::string fpath;

  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Title = "select state (stateファイルを選択してください)";
  dlg->Filter = "state (*.statelog;)|*.statelog;";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dlg->FileName);
  fpath = static_cast<const char*>(mptr.ToPointer());

  ModeRefCurveDeform::GetInst()->LoadState(fpath);
}


System::Void FormRefCurveDeform::m_btn_savestate_Click(System::Object^ sender, System::EventArgs^ e)
{
  std::string fpath;

  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Title = "save state";
  dlg->Filter = "state (*.statelog;)|*.statelog;";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dlg->FileName);
  fpath = static_cast<const char*>(mptr.ToPointer());

  ModeRefCurveDeform::GetInst()->SaveState(fpath);
}

static std::string ToUtf8(String^ value)
{
  const auto bytes = System::Text::Encoding::UTF8->GetBytes(value);
  std::string result(bytes->Length, '\0');
  for (int i = 0; i < bytes->Length; ++i) result[i] = static_cast<char>(bytes[i]);
  return result;
}


System::Void FormRefCurveDeform::m_btn_export_yaml_config_Click(System::Object^ sender, System::EventArgs^ e)
{
  SaveFileDialog dialog;
  dialog.Title = "Export deformation configuration";
  dialog.Filter = "YAML configuration (*.yaml;*.yml)|*.yaml;*.yml";
  dialog.DefaultExt = "yaml";
  dialog.AddExtension = true;
  if (dialog.ShowDialog() != System::Windows::Forms::DialogResult::OK) return;

  FolderBrowserDialog input;
  input.Description = "Select the input image sequence folder";
  if (input.ShowDialog() != System::Windows::Forms::DialogResult::OK) return;
  FolderBrowserDialog output;
  output.Description = "Select the output folder for deformation results";
  if (output.ShowDialog() != System::Windows::Forms::DialogResult::OK) return;

  std::string error;
  if (!ModeRefCurveDeform::GetInst()->ExportDeformationConfig(
      msclr::interop::marshal_as<std::wstring>(dialog.FileName),
      ToUtf8(input.SelectedPath), ToUtf8(output.SelectedPath), error))
    ShowMsgDlg_OK(error.c_str(), "Export YAML configuration");
}


System::Void FormRefCurveDeform::m_btn_import_mha_meshes_Click(System::Object^ sender, System::EventArgs^ e)
{
  OpenFileDialog dialog;
  dialog.Title = "Import MHA masks as meshes (numeric file-name order = frame order)";
  dialog.Filter = "MHA masks (*.mha)|*.mha";
  dialog.Multiselect = true;
  if (dialog.ShowDialog() != System::Windows::Forms::DialogResult::OK) return;

  std::vector<std::wstring> file_paths;
  for each (String^ path in dialog.FileNames)
    file_paths.push_back(msclr::interop::marshal_as<std::wstring>(path));
  std::string error;
  if (!ModeRefCurveDeform::GetInst()->ImportMeshSequenceFromMha(file_paths, error))
    ShowMsgDlg_OK(error.c_str(), "Import MHA meshes");
}
