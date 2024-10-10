#pragma managed
#include "FormSegStrokeFfd.h"
#include "FormMain.h"
//#include "CliMessageBox.h"

#pragma unmanaged
#include "ImageCore.h"
#include "mode/ModeSegStrokeFfd.h"
#pragma managed

using namespace System::Runtime::InteropServices;
using namespace RoiPainter4D;


void FormSegStrokeFfd::InitAllItems()
{
  m_textbox_alpha->Text = "1.0";
  m_textbox_beta->Text = "1.0";
  m_textbox_gamma->Text = "5.0";
  m_radiobtn_stroke->Checked = true;
  m_radiobtn_movement->Checked = true;
  m_numbox_cpsize->Value = 10;
  m_checkbox_showonlyselectedstroke->Checked = false;
}


System::Void FormSegStrokeFfd::m_btn_loadmeshandcage_Click(System::Object^ sender, System::EventArgs^ e)
{
  std::string fpath_mesh;
  std::string fpath_cage;

  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Title = "select mesh (メッシュモデルを選択してください)";
  dlg->Filter = "mesh obj (*.obj;)|*.obj;";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  fpath_mesh = static_cast<const char*>(mptr.ToPointer());

  dlg->Title = "select cage (ケージモデルを選択してください)";
  dlg->Filter = "cage obj (*.obj;)|*.obj;";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() != System::Windows::Forms::DialogResult::Cancel) {
    IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
    fpath_cage = static_cast<const char*>(mptr.ToPointer());
    ModeSegStrokeFfd::GetInst()->LoadMeshAndCage(fpath_mesh, fpath_cage);
  }
}


System::Void FormSegStrokeFfd::m_btn_savemeshandcage_Click(System::Object^ sender, System::EventArgs^ e)
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Title = "select mesh (メッシュモデルを選択してください)";
  dlg->Filter = "mesh obj (*.obj;)|*.obj;";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fpath_mesh = static_cast<const char*>(mptr.ToPointer());

  dlg->Title = "select cage (ケージモデルを選択してください)";
  dlg->Filter = "cage obj (*.obj;)|*.obj;";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fpath_cage = static_cast<const char*>(mptr.ToPointer());

  ModeSegStrokeFfd::GetInst()->SaveMeshAndCage(fpath_mesh, fpath_cage);
}


System::Void FormSegStrokeFfd::m_btn_loadstate_Click(System::Object^ sender, System::EventArgs^ e)
{
  std::string fpath;

  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Title = "select state (stateファイルを選択してください)";
  dlg->Filter = "state (*.txt;)|*.txt;";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  fpath = static_cast<const char*>(mptr.ToPointer());

  std::set<int> set_frame_idx;
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  for (int i = 0; i < num_frames; ++i)
  {
    set_frame_idx.insert(i);
  }
  ModeSegStrokeFfd::GetInst()->LoadState(fpath, set_frame_idx);
}


System::Void FormSegStrokeFfd::m_btn_savestate_Click(System::Object^ sender, System::EventArgs^ e)
{
  std::string fpath;

  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Title = "save state";
  dlg->Filter = "state (*.txt;)|*.txt;";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  fpath = static_cast<const char*>(mptr.ToPointer());

  std::set<int> set_frame_idx;
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  for (int i = 0; i < num_frames; ++i)
  {
    set_frame_idx.insert(i);
  }
  ModeSegStrokeFfd::GetInst()->SaveState(fpath, set_frame_idx);
}


System::Void FormSegStrokeFfd::m_btn_deform_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeSegStrokeFfd::GetInst()->Deform();
}


System::Void FormSegStrokeFfd::m_btn_undo_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeSegStrokeFfd::GetInst()->Undo();
}


System::Void FormSegStrokeFfd::m_btn_redo_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeSegStrokeFfd::GetInst()->Redo();
}


System::Void FormSegStrokeFfd::m_btn_copyfromprevframe_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeSegStrokeFfd::GetInst()->CopyFromPrevFrame();
}


System::Void FormSegStrokeFfd::m_btn_sharestroke_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeSegStrokeFfd::GetInst()->ShareSelectedStroke();
}


System::Void FormSegStrokeFfd::m_btn_copycagetoallframes_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeSegStrokeFfd::GetInst()->CopyCageToAllFrames();
}


std::string FormSegStrokeFfd::GetAlpha()
{
  IntPtr mptr = Marshal::StringToHGlobalAnsi(m_textbox_alpha->Text);
  return static_cast<const char*>(mptr.ToPointer());
}


std::string FormSegStrokeFfd::GetBeta()
{
  IntPtr mptr = Marshal::StringToHGlobalAnsi(m_textbox_beta->Text);
  return static_cast<const char*>(mptr.ToPointer());
}


std::string FormSegStrokeFfd::GetGamma()
{
  IntPtr mptr = Marshal::StringToHGlobalAnsi(m_textbox_gamma->Text);
  return static_cast<const char*>(mptr.ToPointer());
}


int FormSegStrokeFfd::GetDeformMode()
{
  if (m_radiobtn_stroke->Checked) return 0;
  if (m_radiobtn_cage->Checked) return 1;
  return -1;
}


int FormSegStrokeFfd::GetTransformMode()
{
  if (m_radiobtn_movement->Checked) return 0;
  if (m_radiobtn_rotation->Checked) return 1;
  if (m_radiobtn_scaling->Checked) return 2;
  return -1;
}


int FormSegStrokeFfd::GetCPSize()
{
  return static_cast<int>(m_numbox_cpsize->Value);
}


bool FormSegStrokeFfd::GetShowOnlySelectedStroke()
{
  return static_cast<bool>(m_checkbox_showonlyselectedstroke->Checked);
}


void FormSegStrokeFfd::SetDeformMode(const int& _mode)
{
  if (_mode == 0)
  {
    m_radiobtn_stroke->Checked = true;
  }
  else if (_mode == 1)
  {
    m_radiobtn_cage->Checked = true;
  }
}


void FormSegStrokeFfd::SetTransformMode(const int& _mode)
{
  if (_mode == 0)
  {
    m_radiobtn_movement->Checked = true;
  }
  else if (_mode == 1)
  {
    m_radiobtn_rotation->Checked = true;
  }
  else if (_mode == 2)
  {
    m_radiobtn_scaling->Checked = true;
  }
}


void FormSegStrokeFfd::FrameChanged()
{
  ModeSegStrokeFfd::GetInst()->ClearSelectedStrokes();
}


System::Void FormSegStrokeFfd::m_radiobtn_stroke_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
  if (m_radiobtn_stroke->Checked)
  {
    ModeSegStrokeFfd::GetInst()->SetDeformMode();
  }
}


System::Void FormSegStrokeFfd::m_radiobtn_cage_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
  if (m_radiobtn_cage->Checked)
  {
    ModeSegStrokeFfd::GetInst()->SetDeformMode();
  }
}


System::Void FormSegStrokeFfd::m_radiobtn_movement_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
  if (m_radiobtn_movement->Checked)
  {
    ModeSegStrokeFfd::GetInst()->SetTransformMode();
  }
}


System::Void FormSegStrokeFfd::m_radiobtn_rotation_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
  if (m_radiobtn_rotation->Checked)
  {
    ModeSegStrokeFfd::GetInst()->SetTransformMode();
  }
}


System::Void FormSegStrokeFfd::m_radiobtn_scaling_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
  if (m_radiobtn_scaling->Checked)
  {
    ModeSegStrokeFfd::GetInst()->SetTransformMode();
  }
}


System::Void FormSegStrokeFfd::m_numbox_cpsize_ValueChanged(System::Object^ sender, System::EventArgs^ e)
{
  ModeSegStrokeFfd::GetInst()->SetCPSize();
}


System::Void FormSegStrokeFfd::m_checkbox_showonlyselectedstroke_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
  ModeSegStrokeFfd::GetInst()->SetShowOnlySelectedStroke();
}

