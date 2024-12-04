#pragma managed
#include "FormFixMask.h"
#include "FormMain.h"
//#include "CliMessageBox.h"

#pragma unmanaged
#include "ImageCore.h"
#include "mode/ModeFixMask.h"
#pragma managed

using namespace System::Runtime::InteropServices;
using namespace RoiPainter4D;


void FormFixMask::InitAllItems()
{

}


System::Void FormFixMask::m_btn_convert_mask_mesh_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeFixMask::GetInst()->ConvertMaskToMesh();
}


System::Void FormFixMask::m_btn_reload_mesh_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeFixMask::GetInst()->ReloadMesh();
}


System::Void FormFixMask::m_btn_deform_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeFixMask::GetInst()->Deform();
}


System::Void FormFixMask::m_btn_undo_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeFixMask::GetInst()->Undo();
}


System::Void FormFixMask::m_btn_redo_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeFixMask::GetInst()->Redo();
}


System::Void FormFixMask::m_copy_from_prev_frame_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeFixMask::GetInst()->CopyFromPrevFrame();
}


System::Void FormFixMask::m_btn_convert_mesh_mask_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeFixMask::GetInst()->ConvertMeshToMask();
}


System::Void FormFixMask::m_btn_copy_stroke_all_frame_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeFixMask::GetInst()->CopyStrokesToAllFrame();
}


System::Void FormFixMask::m_checkbox_showonlyselectedstroke_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
  ModeFixMask::GetInst()->SetShowOnlySelectedStroke();
}

System::Void FormFixMask::m_btn_sharestroke_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeFixMask::GetInst()->ShareSelectedStroke();
}


System::Void FormFixMask::m_btn_loadstate_Click(System::Object^ sender, System::EventArgs^ e)
{
  std::string fpath;

  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Title = "select state (stateファイルを選択してください)";
  dlg->Filter = "state (*.statelog;)|*.statelog;";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dlg->FileName);
  fpath = static_cast<const char*>(mptr.ToPointer());

  std::set<int> set_frame_idx;
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  for (int i = 0; i < num_frames; ++i)
  {
    set_frame_idx.insert(i);
  }
  ModeFixMask::GetInst()->LoadState(fpath, set_frame_idx);
}


System::Void FormFixMask::m_btn_savestate_Click(System::Object^ sender, System::EventArgs^ e)
{
  std::string fpath;

  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Title = "save state";
  dlg->Filter = "state (*.statelog;)|*.statelog;";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dlg->FileName);
  fpath = static_cast<const char*>(mptr.ToPointer());

  std::set<int> set_frame_idx;
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  for (int i = 0; i < num_frames; ++i)
  {
    set_frame_idx.insert(i);
  }
  ModeFixMask::GetInst()->SaveState(fpath, set_frame_idx);
}


int FormFixMask::GetCPSize()
{
  return static_cast<int>(m_numbox_cpsize->Value);
}


bool FormFixMask::GetShowOnlySelectedStroke()
{
  return static_cast<bool>(m_checkbox_showonlyselectedstroke->Checked);
}


int FormFixMask::GetMCScale()
{
  return static_cast<int>(m_trackbar_mcscale->Value);
}


System::Void FormFixMask::m_numbox_cpsize_ValueChanged(System::Object^ sender, System::EventArgs^ e)
{
  ModeFixMask::GetInst()->SetCPSize();
}