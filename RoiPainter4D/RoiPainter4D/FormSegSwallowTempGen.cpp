#pragma managed
#include "FormSegSwallowTempGen.h"

#pragma unmanaged
#include "mode/ModeSegSwallowTempGen.h"
#include "mode/ModeSegSwallowOrgans.h"
#include <string>
#pragma managed

using namespace System::Runtime::InteropServices;
using namespace RoiPainter4D;


static bool GetMeshAndCageFileNames(
    std::string& meshname, 
    std::string& cagename, 
    std::string& defname)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Title = "select mesh (メッシュモデルを選択してください)";
  dlg->Filter = "mesh file(*.obj;)|*.obj;";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return false;
  IntPtr mptr1 = Marshal::StringToHGlobalAnsi(dlg->FileName);
  meshname = static_cast<const char*>(mptr1.ToPointer());

  dlg->Filter = "cage file (*.obj;)|*.obj;";
  dlg->Title = "cage mesh (ケージモデルを選択してください)";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return false;
  IntPtr mptr2 = Marshal::StringToHGlobalAnsi(dlg->FileName);
  cagename = static_cast<const char*>(mptr2.ToPointer());

  dlg->Filter = "deformation file (*.txt;)|*.txt;";
  dlg->Title = "変形データ (変形データを選択してください)";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return false;
  IntPtr mptr3 = Marshal::StringToHGlobalAnsi(dlg->FileName);
  defname = static_cast<const char*>(mptr3.ToPointer());
  return true;
}



System::Void FormSegSwallowTempGen::m_btn_addzetumesh_Click(
  System::Object^ sender,
  System::EventArgs^ e)
{
  std::string cagename, meshname, defname;
  if (GetMeshAndCageFileNames(meshname, cagename, defname))
    ModeSegSwallowTempGen::getInst()->LoadCagedMesh(meshname, cagename, defname, SOT_ZETSU);
}

System::Void FormSegSwallowTempGen::m_btn_addnankmesh_Click(
  System::Object^ sender,
  System::EventArgs^ e)
{
  std::string cagename, meshname, defname;
  if (GetMeshAndCageFileNames(meshname, cagename, defname))
    ModeSegSwallowTempGen::getInst()->LoadCagedMesh(meshname, cagename, defname, SOT_NANK);
}

System::Void FormSegSwallowTempGen::m_btn_addkoutmesh_Click(
  System::Object^ sender,
  System::EventArgs^ e)
{
  std::string cagename, meshname, defname;
  if (GetMeshAndCageFileNames(meshname, cagename, defname))
    ModeSegSwallowTempGen::getInst()->LoadCagedMesh(meshname, cagename, defname, SOT_KOTO);
}

System::Void FormSegSwallowTempGen::m_btn_addintomesh_Click(
  System::Object^ sender,
  System::EventArgs^ e)
{
  std::string cagename, meshname, defname;
  if (GetMeshAndCageFileNames(meshname, cagename, defname))
    ModeSegSwallowTempGen::getInst()->LoadCagedMesh(meshname, cagename, defname, SOT_INTO);
}



System::Void RoiPainter4D::FormSegSwallowTempGen::m_btn_update_timing_Click(
    System::Object^ sender,
    System::EventArgs^ e)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Title = "timing info(テンプレートのタイミング情報)";
  dlg->Filter = "タイミングデータファイル(*.txt;)|*.txt;";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr1 = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr1.ToPointer());
    
  int f1 = (int)m_numeric1->Value;
  int f2 = (int)m_numeric2->Value;
  int f3 = (int)m_numeric3->Value;
  int f4 = (int)m_numeric4->Value;
  ModeSegSwallowTempGen::getInst()->UpdateCageByTiming(fname, f1, f2, f3, f4);
}


System::Void RoiPainter4D::FormSegSwallowTempGen::m_btn_export_motion_Click(
    System::Object^ sender,
    System::EventArgs^ e)
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "cage変形データ(*.txt)|*.txt";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeSegSwallowTempGen::getInst()->ExportCageMotionData(fname);
}


System::Void RoiPainter4D::FormSegSwallowTempGen::m_choosezetu_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
}

System::Void RoiPainter4D::FormSegSwallowTempGen::m_choosenank_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
}

System::Void RoiPainter4D::FormSegSwallowTempGen::m_choosekout_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
}

System::Void RoiPainter4D::FormSegSwallowTempGen::m_chooseinto_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
}





void RoiPainter4D::FormSegSwallowTempGen::initialize_spin_control(
  int num_frames, int f1, int f2, int f3, int f4)
{
  m_numeric1->Maximum = num_frames;
  m_numeric2->Maximum = num_frames;
  m_numeric3->Maximum = num_frames;
  m_numeric4->Maximum = num_frames;

  m_numeric1->Value = (f1 <= num_frames) ? f1 : num_frames;
  m_numeric2->Value = (f2 <= num_frames) ? f2 : num_frames;
  m_numeric3->Value = (f3 <= num_frames) ? f3 : num_frames;
  m_numeric4->Value = (f4 <= num_frames) ? f4 : num_frames;
}
