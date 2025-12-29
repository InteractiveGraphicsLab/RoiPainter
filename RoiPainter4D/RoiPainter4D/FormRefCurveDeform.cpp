#pragma managed
#include "FormRefCurveDeform.h"
#include "FormMain.h"
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
  ModeRefCurveDeform::GetInst()->Deform();
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



