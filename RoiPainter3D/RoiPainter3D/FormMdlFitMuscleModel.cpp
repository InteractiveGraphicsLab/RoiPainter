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