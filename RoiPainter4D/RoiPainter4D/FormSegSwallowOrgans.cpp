#pragma managed
#include "FormSegSwallowOrgans.h"
#include "FormMain.h"
#include "CliMessageBox.h"

#pragma unmanaged
#include "ImageCore.h"
#include "mode/ModeSegSwallowOrgans.h"
#include <vector>
#include <string>
#pragma managed

using namespace System::Runtime::InteropServices;
using namespace RoiPainter4D;


void FormSegSwallowOrgans::InitAllItems()
{
  m_radiobtn_scalecage    ->Checked = false;
  m_radiobtn_rotatecage   ->Checked = false;
  m_radiobtn_translatecage->Checked = true ;

  m_checkbox_vissurf_trans->Checked = true ;
  m_checkbox_vissurf_solid->Checked = false;
  m_checkbox_vismeshes->Checked = true;

  m_checkbox_visbound->Checked = true;
  UpdateList();
  SetNumSelectVtx(0);

  m_updown_cpsize->Value = 10;
}


void FormSegSwallowOrgans::SetNumSelectVtx(int num_select_vtx)
{
  std::string str = std::string("#PTs: ") + std::to_string( num_select_vtx);
  m_textbox_selection->Text = gcnew System::String(str.c_str()) ;
  
  m_textbox_selection->BackColor =
    ( num_select_vtx == 0 ) ? System::Drawing::Color::Red   :
    ( num_select_vtx == 1 ) ? System::Drawing::Color::Green : System::Drawing::Color::Blue;

  m_textbox_selection->ForeColor =
    ( num_select_vtx == 0 ) ? System::Drawing::Color::Black :
    ( num_select_vtx == 1 ) ? System::Drawing::Color::White : System::Drawing::Color::White; 
}

void FormSegSwallowOrgans::FitCrssecToSelectedCageVtx()
{
  ModeSegSwallowOrgans::GetInst()->FitCrssecToSelectedCageVtx();
  formMain_RedrawMainPanel();
}




//radio button -- do nothing
System::Void FormSegSwallowOrgans::m_radiobtn_scalecage_CheckedChanged    (System::Object^  sender, System::EventArgs^  e){}
System::Void FormSegSwallowOrgans::m_radiobtn_rotatecage_CheckedChanged   (System::Object^  sender, System::EventArgs^  e){}
System::Void FormSegSwallowOrgans::m_radiobtn_translatecage_CheckedChanged(System::Object^  sender, System::EventArgs^  e){}

System::Void FormSegSwallowOrgans::m_btn_clearselection_Click(System::Object^  sender, System::EventArgs^  e)
{
  SetNumSelectVtx(0);
  ModeSegSwallowOrgans::GetInst()->SelectedCageVtx_Clear();
}



System::Void FormSegSwallowOrgans::m_btn_addmesh_Click     (System::Object^  sender, System::EventArgs^  e) 
{
  std::string meshname = "";
  std::string cagename = "";

	OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Title  = "select mesh (メッシュモデルを選択してください)";
	dlg->Filter = "mesh file(*.obj;)|*.obj;";
	dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

	IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
	meshname = static_cast<const char*>(mptr.ToPointer());


	dlg->Filter = "cage file (*.obj;)|*.obj;";
  dlg->Title  = "cage mesh (ケージモデルを選択してください)";
	dlg->Multiselect = false;
  if (dlg->ShowDialog() != System::Windows::Forms::DialogResult::Cancel){
	  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
	  cagename = static_cast<const char*>(mptr.ToPointer());
    ModeSegSwallowOrgans::GetInst()->LoadNewMeshAndCage(meshname, cagename);  
  }
  
}




System::Void FormSegSwallowOrgans::m_btn_copytoallframes_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegSwallowOrgans::GetInst()->CopyCurrentMeshToOtherFrames();  
}




System::Void FormSegSwallowOrgans::m_btn_savecagetxt_Click(System::Object^  sender, System::EventArgs^  e) 
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
	dlg->Filter = "cage sequence info (*.txt;)|*.txt;";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

	IntPtr mptr  = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeSegSwallowOrgans::GetInst()->SaveCageMeshSequenceTxt(fname, true);  
}


System::Void FormSegSwallowOrgans::m_btn_loadcagetxt_Click(System::Object^  sender, System::EventArgs^  e) 
{
  if( !ShowMsgDlgYesNo("Cage変形を読み込むと現在の編集内容が失われます。良いですか?", "注意") )
    return;

	OpenFileDialog^ dlg = gcnew OpenFileDialog();
	dlg->Filter = "cage sequence info (*.txt;)|*.txt;";
	dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

	IntPtr mptr  = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeSegSwallowOrgans::GetInst()->LoadCageSequence(fname);  

}

System::Void FormSegSwallowOrgans::m_btn_savemeshtxt_Click(System::Object^  sender, System::EventArgs^  e) 
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
	dlg->Filter = "mesh sequence info (*.txt;)|*.txt;";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

	IntPtr mptr  = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeSegSwallowOrgans::GetInst()->SaveCageMeshSequenceTxt(fname, false);  
}

System::Void FormSegSwallowOrgans::m_btn_savemeshobj_Click(System::Object^  sender, System::EventArgs^  e) 
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
	dlg->Filter = "mesh obj (*.obj;)|*.obj;";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

	IntPtr mptr  = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeSegSwallowOrgans::GetInst()->SaveCageMeshSequenceObj(fname, false);  
}



System::Void FormSegSwallowOrgans::m_btn_loaddeform_nextcage_Click(System::Object^ sender, System::EventArgs^ e)
{
  //get file names 
  OpenFileDialog^ dlg_cage = gcnew OpenFileDialog();
  dlg_cage->Filter = "Cage File (*.obj)|*.obj";
  dlg_cage->Title = "変形するケージまたはメッシュ(obj)を選択してください";

  dlg_cage->Multiselect = false;
  if (dlg_cage->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr_cage = Marshal::StringToHGlobalAnsi(dlg_cage->FileName);
  std::string fname_cage = static_cast<const char*>(mptr_cage.ToPointer());

  SaveFileDialog^ dlg_txt = gcnew SaveFileDialog();
  dlg_txt->Filter = "cage 変形情報 (*.txt)|*.txt";
  dlg_txt->Title  = "変形情報を出力するファイル名(txt)を指定してください";


  if (dlg_txt->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr_txt = Marshal::StringToHGlobalAnsi(dlg_txt->FileName);
  std::string fname_txt = static_cast<const char*>(mptr_txt.ToPointer());

  ModeSegSwallowOrgans::GetInst()->DeformImportedCage(fname_cage, fname_txt);
}


System::Void FormSegSwallowOrgans::m_btn_finish_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeSegSwallowOrgans::GetInst()->FinishSegmentation();
}


System::Void FormSegSwallowOrgans::m_btn_cancel_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeSegSwallowOrgans::GetInst()->CancelSegmentation();
}









// group list 関連 ↓

void FormSegSwallowOrgans::UpdateList()
{
  m_bListUpdating = true;


  //初期化
  m_grouplist->Rows->Clear();
  for (int i = 0; i < NUM_POINT_GROUPS; ++i) 
  {
    const auto &g = ModeSegSwallowOrgans::GetInst()->GetPointGroup(i);
    m_grouplist->Rows->Add();
  }

  m_grouplist->CurrentCell = nullptr;

  //cell colors
  for (int i = 0; i < NUM_POINT_GROUPS; ++i) 
  {
    const auto &g = ModeSegSwallowOrgans::GetInst()->GetPointGroup(i);

    char key = (char)(i);
    int  num = (int)g.m_ids.size();
    std::string name = g.m_name;

    std::string text = std::to_string(key) + std::string("-key, ")
                     + std::to_string(num) + std::string(", ") + name;

    m_grouplist[0, i]->Value = gcnew String(text.c_str());
  }

  this->Update();
  this->Invalidate();

  m_bListUpdating = false;
}


System::Void FormSegSwallowOrgans::m_btn_registgroup_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  ModeSegSwallowOrgans::GetInst()->RegistPointGroup();
  UpdateList();
}


System::Void FormSegSwallowOrgans::m_btn_loadgroup_Click(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = "cage point group data (*.txt)|*.txt";
  dlg->Multiselect = false;

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());

  ModeSegSwallowOrgans::GetInst()->LoadPointGroup(fname);
  UpdateList();

}


System::Void FormSegSwallowOrgans::m_btn_savegroup_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{

  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "cage point group data (*.txt)|*.txt";

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());


  ModeSegSwallowOrgans::GetInst()->SavePointGroup(fname);
  UpdateList();
}



System::Void FormSegSwallowOrgans::m_grouplist_CellContentClick(
    System::Object^  sender, 
    System::Windows::Forms::DataGridViewCellEventArgs^  e)
{

}



System::Void FormSegSwallowOrgans::m_grouplist_KeyDown(
    System::Object^  sender, 
    System::Windows::Forms::KeyEventArgs^  e)
{
  int keyid = (int)e->KeyCode ;

  if ( keyid == 46 )
  {
    if ( m_grouplist->CurrentCell == nullptr ) return;

    if ( 0 <= m_grouplist->CurrentCell->RowIndex && 
         m_grouplist->CurrentCell->RowIndex < NUM_POINT_GROUPS )
    {
      ModeSegSwallowOrgans::GetInst()->DeletePointGroup(
        m_grouplist->CurrentCell->RowIndex);
    } 
    return;
  }

  int trgt = -1;
  if ( keyid == '0' ) trgt = 0;
  if ( keyid == '1' ) trgt = 1;
  if ( keyid == '2' ) trgt = 2;
  if ( keyid == '3' ) trgt = 3;
  if ( keyid == '4' ) trgt = 4;
  if ( keyid == '5' ) trgt = 5;
  if ( keyid == '6' ) trgt = 6;
  if ( keyid == '7' ) trgt = 7;
  if ( keyid == '8' ) trgt = 8;
  if ( keyid == '9' ) trgt = 9;
  
  if ( 0 <= trgt && trgt < NUM_POINT_GROUPS )
  {
    ModeSegSwallowOrgans::GetInst()->CheckoutPointGroup(trgt);
    m_grouplist->CurrentCell = m_grouplist[0,trgt];
  }



}


System::Void FormSegSwallowOrgans::FormSegSwallowOrgans_KeyDown(
    System::Object^  sender, 
    System::Windows::Forms::KeyEventArgs^  e) 
{
  int keyid = (int)e->KeyCode ;

  std::cout << "aaaaaaaaaaaaaaaaaaaaaaaaaaaa" << keyid << "\n";
  //never come here why??

}





System::Void FormSegSwallowOrgans::m_grouplist_MouseDoubleClick(
    System::Object^  sender, 
    System::Windows::Forms::MouseEventArgs^  e) 
{
  std::cout << "double click !!!!!!!\n";

  if ( m_grouplist->CurrentCell == nullptr ) return;
  
  if ( 0 <= m_grouplist->CurrentCell->RowIndex && 
       m_grouplist->CurrentCell->RowIndex < NUM_POINT_GROUPS )
  {
    ModeSegSwallowOrgans::GetInst()->CheckoutPointGroup(
      m_grouplist->CurrentCell->RowIndex);
  }


}




System::Void FormSegSwallowOrgans::m_checkbox_vissurf_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  if (m_checkbox_vissurf_trans->Checked)
    m_checkbox_vissurf_solid->Checked = false;
  formMain_RedrawMainPanel();
}



System::Void FormSegSwallowOrgans::m_checkbox_vissurf_solid_CheckedChanged(
    System::Object^ sender,
    System::EventArgs^ e)
{
  if ( m_checkbox_vissurf_solid->Checked )
    m_checkbox_vissurf_trans->Checked = false;
  formMain_RedrawMainPanel();
}



System::Void FormSegSwallowOrgans::m_checkbox_vismeshes_CheckedChanged(
  System::Object^ sender,
  System::EventArgs^ e)
{

  formMain_RedrawMainPanel();
}



System::Void FormSegSwallowOrgans::m_checkbox_visbound_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  formMain_RedrawMainPanel();
}



System::Void FormSegSwallowOrgans::m_checkbox_fit_crssec_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  FitCrssecToSelectedCageVtx();
}



System::Void FormSegSwallowOrgans::m_checkbox_show_only_selected_points_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  formMain_RedrawMainPanel();
}


System::Void FormSegSwallowOrgans::m_updown_cpsize_ValueChanged(
    System::Object^ sender, 
    System::EventArgs^ e)
{
  auto c = ImageCore::GetInst()->GetCuboidF();
  int cprate = (int)m_updown_cpsize->Value;
  CagedMeshSequence::SetHandleLength      ( c[0] * 0.03f   * cprate);
  CagedMeshSequence::SetControlPointRadius( c[0] * 0.0015f * cprate);
  formMain_RedrawMainPanel();
}





System::Void FormSegSwallowOrgans::m_btn_loadvisobjs_Click(
    System::Object^ sender, 
    System::EventArgs^ e)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = "3d obj files (*.obj)|*.obj";
  dlg->Multiselect = true;

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  std::vector<std::string> file_names;

  for each (auto it in dlg->FileNames)
  {
    IntPtr mptr = Marshal::StringToHGlobalAnsi(it);
    std::string fname = static_cast<const char*>(mptr.ToPointer());
    file_names.push_back(fname);
  }
  
  //sort 
  std::sort(file_names.begin(), file_names.end());
  
  for (auto it : file_names)
    std::cout << "load obj " << it.c_str() << "\n";
  
  ModeSegSwallowOrgans::GetInst()->LoadVisObjs(file_names);
  formMain_RedrawMainPanel();
}



System::Void FormSegSwallowOrgans::m_btn_clearvisobjs_Click(
    System::Object^ sender, 
    System::EventArgs^ e)
{
  ModeSegSwallowOrgans::GetInst()->ClearVisObjs();
  formMain_RedrawMainPanel();
}
