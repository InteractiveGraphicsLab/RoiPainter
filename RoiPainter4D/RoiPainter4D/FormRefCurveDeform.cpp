#pragma managed
#include "FormRefCurveDeform.h"
#include "FormMain.h"
#include "FormSelectMskId.h"
#include "CliMessageBox.h"

#pragma unmanaged
#include "ImageCore.h"
#include "ModeCore.h"
#include "mode/ModeRefCurveDeform.h"
#pragma managed

using namespace System::Runtime::InteropServices;
using namespace RoiPainter4D;


void FormRefCurveDeform::InitAllItems()
{
  m_numbox_cpsize->Value = 10;
  m_checkbox_showonlyselectedstroke->Checked = false;
  m_trackbar_mcscale->Value = 2;
  m_checkbox_vissurf_trans->Checked = true;
  m_checkbox_vissurf_solid->Checked = false;

  FormSelectMskId^ modal = gcnew FormSelectMskId();
  if (modal->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  int trgtId = modal->getTrgtID();
  if (trgtId == 0)
  {
    ShowMsgDlg_OK("0th region (background) cannot be deformed", "caution");
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }
  modal->Close();

  ImageCore::GetInst()->SetSelectMaskId(trgtId);
}


System::Void FormRefCurveDeform::m_btn_convert_mask_mesh_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->ConvertMaskToMesh();
}


System::Void FormRefCurveDeform::m_btn_reload_mesh_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->ReloadMesh();
}


System::Void FormRefCurveDeform::m_btn_deform_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->Deform();
}


System::Void FormRefCurveDeform::m_btn_undo_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->Undo();
}


System::Void FormRefCurveDeform::m_btn_redo_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->Redo();
}


System::Void FormRefCurveDeform::m_copy_from_prev_frame_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->CopyFromPrevFrame();
}


System::Void FormRefCurveDeform::m_btn_convert_mesh_mask_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->ConvertMeshToMask();
}


System::Void FormRefCurveDeform::m_btn_copy_stroke_all_frame_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->CopyStrokesToAllFrame();
}


System::Void FormRefCurveDeform::m_checkbox_showonlyselectedstroke_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->SetShowOnlySelectedStroke();
}

System::Void FormRefCurveDeform::m_btn_sharestroke_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->ShareSelectedStroke();
}


System::Void FormRefCurveDeform::m_btn_loadstate_Click(System::Object^ sender, System::EventArgs^ e)
{
  LoadState();
}


System::Void FormRefCurveDeform::m_btn_savestate_Click(System::Object^ sender, System::EventArgs^ e)
{
  SaveState();
}

System::Void FormRefCurveDeform::m_btn_flip_normals_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->FlipSelectedStrokeNormalSide();
}

System::Void FormRefCurveDeform::m_checkbox_visbound_CheckedChanged(
  System::Object^ sender,
  System::EventArgs^ e)
{
  formMain_RedrawMainPanel();
}

System::Void FormRefCurveDeform::m_checkbox_vissurf_CheckedChanged(
  System::Object^ sender,
  System::EventArgs^ e)
{
  if (m_checkbox_vissurf_trans->Checked)
    m_checkbox_vissurf_solid->Checked = false;
  formMain_RedrawMainPanel();
}

System::Void FormRefCurveDeform::m_checkbox_vissurf_solid_CheckedChanged(
  System::Object^ sender,
  System::EventArgs^ e)
{
  if (m_checkbox_vissurf_solid->Checked)
    m_checkbox_vissurf_trans->Checked = false;
  formMain_RedrawMainPanel();
}

System::Void FormRefCurveDeform::m_checkbox_visnormals_CheckedChanged(
  System::Object^ sender,
  System::EventArgs^ e)
{
  formMain_RedrawMainPanel();
}

void FormRefCurveDeform::LoadState()
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
  ModeRefCurveDeform::GetInst()->LoadState(fpath, set_frame_idx);
}


void FormRefCurveDeform::SaveState()
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
  ModeRefCurveDeform::GetInst()->SaveState(fpath, set_frame_idx);
}


int FormRefCurveDeform::GetCPSize()
{
  return static_cast<int>(m_numbox_cpsize->Value);
}


bool FormRefCurveDeform::GetShowOnlySelectedStroke()
{
  return static_cast<bool>(m_checkbox_showonlyselectedstroke->Checked);
}


int FormRefCurveDeform::GetMCScale()
{
  return static_cast<int>(m_trackbar_mcscale->Value);
}


System::Void FormRefCurveDeform::m_numbox_cpsize_ValueChanged(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefCurveDeform::GetInst()->SetCPSize();
}
