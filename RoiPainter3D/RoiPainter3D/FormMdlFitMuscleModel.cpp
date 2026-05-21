#include "FormMdlFitMuscleModel.h"
#include "FormVisParam.h"
#include "FormMain.h"

#pragma unmanaged
#include "mode/ModeMdlFitMuscleModel.h"
#pragma managed

using namespace RoiPainter3D;
using namespace System::IO;
using namespace System::Runtime::InteropServices;



System::Void FormMdlFitMuscleModel::m_btn_import_obj_Click(System::Object^ sender, System::EventArgs^ e)
{
  FolderBrowserDialog^ fbd = gcnew FolderBrowserDialog();
  fbd->Description = "OBJファイルが含まれるフォルダを選択してください ";

  if (fbd->ShowDialog() == System::Windows::Forms::DialogResult::OK)
  {
    m_treeView_models->Nodes->Clear();

    CreateTreeView(fbd->SelectedPath, nullptr);
  }
  UpdateMasterCheckboxState();
}


System::Void FormMdlFitMuscleModel::m_btn_reset_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeMdlFitMuscleModel::GetInst()->ModelReset();
  m_treeView_models->Nodes->Clear();

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


void FormMdlFitMuscleModel::CreateTreeView(String^ dirPath, TreeNode^ parentNode)
{
  array<String^>^ dirs = Directory::GetDirectories(dirPath);
  for each(String ^ d in dirs)
  {
    DirectoryInfo^ di = gcnew DirectoryInfo(d);
    TreeNode^ node = gcnew TreeNode(di->Name);

    node->Checked = true;

    if (parentNode == nullptr)
      m_treeView_models->Nodes->Add(node);
    else
      parentNode->Nodes->Add(node);
    CreateTreeView(d, node);
  }

  array<String^>^ files = Directory::GetFiles(dirPath, "*.obj");
  for each(String ^ f in files)
  {
    FileInfo^ fi = gcnew FileInfo(f);
    TreeNode^ node = gcnew TreeNode(fi->Name);

    IntPtr mptr = Marshal::StringToHGlobalAnsi(f);
    try
    {
      std::string cppStr = static_cast<const char*>(mptr.ToPointer());
      TMesh* meshPtr = ModeMdlFitMuscleModel::GetInst()->ImportObjFile(cppStr);
      if (meshPtr != nullptr)
      {
        node->Tag = (IntPtr)meshPtr;
        node->Checked = true;

        if (parentNode == nullptr) m_treeView_models->Nodes->Add(node);
        else parentNode->Nodes->Add(node);
      }
    }
    finally
    {
      Marshal::FreeHGlobal(mptr);
    }
  }


}

static void UpdateModelVisibility(TreeNode^ node)
{
  if (node->Tag != nullptr)
  {
    TMesh* mesh = (TMesh*)((IntPtr)node->Tag).ToPointer();
    ModeMdlFitMuscleModel::GetInst()->SetModelVisibility(mesh, node->Checked);
  }
}

static void CheckAllChildNodes(TreeNode^ treeNode, bool nodeChecked)
{
  for each(TreeNode ^ node in treeNode->Nodes)
  {
    node->Checked = nodeChecked;
    if (node->Nodes->Count > 0)
    {
      CheckAllChildNodes(node, nodeChecked);
    }
    UpdateModelVisibility(node);
  }
}

System::Void FormMdlFitMuscleModel::m_treeView_models_AfterCheck(System::Object^ sender, System::Windows::Forms::TreeViewEventArgs^ e)
{
  if (e->Action != TreeViewAction::Unknown)
  {
    CheckAllChildNodes(e->Node, e->Node->Checked);
  }
  UpdateModelVisibility(e->Node);
  UpdateMasterCheckboxState();
}


void FormMdlFitMuscleModel::SetAllNodesCheckedState(TreeNodeCollection^ nodes, bool isChecked)
{
  for each (TreeNode ^ node in nodes)
  {
    node->Checked = isChecked;
    if (node->Nodes->Count > 0)
    {
      SetAllNodesCheckedState(node->Nodes, isChecked);
    }
    UpdateModelVisibility(node);
  }
}


System::Void FormMdlFitMuscleModel::m_checkbox_select_all_Click(System::Object^ seder, System::EventArgs^ e)
{
  if (m_checkbox_select_all->CheckState == CheckState::Indeterminate)
  {
    m_checkbox_select_all->CheckState = CheckState::Unchecked;
  }

  bool isChecked = (m_checkbox_select_all->CheckState == CheckState::Checked);
  SetAllNodesCheckedState(m_treeView_models->Nodes, isChecked);
}


void FormMdlFitMuscleModel::CountCheckedNodes(TreeNodeCollection^ nodes, int% totalCount, int% checkedCount)
{
  for each (TreeNode ^ node in nodes)
  {
    if (node->Tag != nullptr)
    {
      totalCount++;
      if (node->Checked)
        checkedCount++;
    }
    if (node->Nodes->Count > 0)
      CountCheckedNodes(node->Nodes, totalCount, checkedCount);
  }
}

void FormMdlFitMuscleModel::UpdateMasterCheckboxState()
{
  int total = 0;
  int checked = 0;

  CountCheckedNodes(m_treeView_models->Nodes, total, checked);

  if (total == 0)
    m_checkbox_select_all->CheckState = CheckState::Unchecked;
  else if (checked == total)
    m_checkbox_select_all->CheckState = CheckState::Checked;
  else if (checked == 0)
    m_checkbox_select_all->CheckState = CheckState::Unchecked;
  else
    m_checkbox_select_all->CheckState = CheckState::Indeterminate;
}

System::Void FormMdlFitMuscleModel::m_btn_pca_Click(System::Object^ sender, System::EventArgs^ e)
{
  ModeMdlFitMuscleModel::GetInst()->TranslateModel();
  ModeMdlFitMuscleModel::GetInst()->SetPCAAxis(true);
}