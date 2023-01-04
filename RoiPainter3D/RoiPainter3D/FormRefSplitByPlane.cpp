#pragma managed
#include "FormRefSplitByPlane.h"

#pragma unmanaged
#include "mode/ModeRefSplitByPlane.h"
#pragma managed


using namespace RoiPainter3D;
using namespace System::Runtime::InteropServices;


System::Void FormRefSplitByPlane::btn_cancel_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefSplitByPlane::GetInst()->cancelSegmentation();
}

System::Void FormRefSplitByPlane::btn_finish_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefSplitByPlane::GetInst()->finishSegmentation();
}

System::Void FormRefSplitByPlane::btn_gen_plane_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefSplitByPlane::GetInst()->GeneratePlaneFromPoints();
}

System::Void FormRefSplitByPlane::btn_split_by_plane_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefSplitByPlane::GetInst()->SplitVolumeByPlane();
}

System::Void FormRefSplitByPlane::btn_perform_analysis_Click(System::Object^ sender, System::EventArgs^ e)
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "image file name (*)|*";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());

  ModeRefSplitByPlane::GetInst()->AnalysisWithCurrentPlane(fname);
}

System::Void FormRefSplitByPlane::btn_flip_plane_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeRefSplitByPlane::GetInst()->FlipPlaneNormal();
}




