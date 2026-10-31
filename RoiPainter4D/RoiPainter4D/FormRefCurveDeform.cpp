#pragma managed
#include "FormRefCurveDeform.h"
#include "FormMain.h"
#include "CliMessageBox.h"

#pragma unmanaged
#include "ImageCore.h"
#include "ModeCore.h"
#include "mode/ModeRefCurveDeform.h"
#include "../COMMON/tvolumelader.h"
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


System::Void FormRefCurveDeform::m_btn_flipnormal_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->FlipSelectedStrokeNormalSide();
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

System::Void FormRefCurveDeform::m_btn_export_config_Click(System::Object^ sender, System::EventArgs^ e)
{
  std::string fpath;
  std::string input_dir;
  std::string output_dir;

  // 1. 保存先 YAML ファイルの選択
  SaveFileDialog^ dlgSave = gcnew SaveFileDialog();
  dlgSave->Title = "Save YAML File";
  dlgSave->Filter = "YAML File (*.yaml;*.yml)|*.yaml;*.yml|All Files (*.*)|*.*";
  if (dlgSave->ShowDialog() != System::Windows::Forms::DialogResult::OK) return;
  IntPtr mptrSave = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dlgSave->FileName);
  fpath = static_cast<const char*>(mptrSave.ToPointer());

  // 2. Input ディレクトリの選択
  FolderBrowserDialog^ dlgInput = gcnew FolderBrowserDialog();
  dlgInput->Description = "Select Input Directory";
  if (dlgInput->ShowDialog() != System::Windows::Forms::DialogResult::OK) return;
  IntPtr mptrInput = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dlgInput->SelectedPath);
  input_dir = static_cast<const char*>(mptrInput.ToPointer());

  // 3. Output ディレクトリの選択
  FolderBrowserDialog^ dlgOutput = gcnew FolderBrowserDialog();
  dlgOutput->Description = "Select Output Directory";
  if (dlgOutput->ShowDialog() != System::Windows::Forms::DialogResult::OK) return;
  IntPtr mptrOutput = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dlgOutput->SelectedPath);
  output_dir = static_cast<const char*>(mptrOutput.ToPointer());

  // 処理呼び出し
  ModeRefCurveDeform::GetInst()->SaveYaml(fpath, input_dir, output_dir);
}

static void n_marshalString(String^ s, std::string& os) {
  const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
  os = chars;
  Marshal::FreeHGlobal(IntPtr((void*)chars));
}

static bool OpenDlgMulti(
  const char* filter,
  std::vector<std::string>& out_f_names,
  std::vector<std::string>& out_f_dates)
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




System::Void FormRefCurveDeform::m_btn_load_mha_Click(System::Object^ sender, System::EventArgs^ e)
{
  const char* filter = "mha files (*.mha)|*.mha";
  std::vector<std::string> fnames, fdates;
  if (!OpenDlgMulti(filter, fnames, fdates)) return;

  int W, H, D, WH, WHD;
  std::tie(W, H, D, WH, WHD) = ImageCore::GetInst()->GetResolution5();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

  if (num_frames != static_cast<int>(fnames.size()))
  {
    ShowMsgDlg_OK("strange frame count", "caution");
    return;
  }

  // 1. MHA ファイルをメモリ上（一時バッファ）に読み込み
  std::vector<std::unique_ptr<byte[]>> masks;
  for (const auto& f : fnames)
  {
    EVec3f pitch;
    EVec3i reso;
    std::unique_ptr<byte[]> data_b;
    std::unique_ptr<short[]> data_s;

    if (!ReadMha3d(f, reso, pitch, data_b, data_s) || data_b == nullptr)
    {
      std::cout << "Fail to read mha files\n";
      return;
    }
    if (reso[0] != W || reso[1] != H || reso[2] != D)
    {
      ShowMsgDlg_OK("strange resolution", "caution");
      return;
    }
    masks.push_back(std::move(data_b));
  }
  ModeRefCurveDeform::GetInst()->LoadMha(masks);
}


