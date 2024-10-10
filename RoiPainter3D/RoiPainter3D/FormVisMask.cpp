#pragma managed
#include "FormVisMask.h"
#include "FormMain.h"
#include "FormMaskIDselection.h"
#include "FormIntegerSelection.h"
#include "climessagebox.h"

#pragma unmanaged
#include "ImageCore.h"

#pragma managed


using namespace System::Runtime::InteropServices;
using namespace RoiPainter3D;



void FormVisMask::updateList()
{
  m_bListUpdating = true;

  const std::vector<MaskData> &maskData = ImageCore::GetInst()->GetMaskData();
  const int &maskSelectedId = ImageCore::GetInst()->GetActiveMaskID();


  //初期化
  maskList->Rows->Clear();
  for (int i = 0; i < maskData.size(); ++i) maskList->Rows->Add();

  //このタイミングでhighlightをoffに
  maskList->CurrentCell = nullptr;

  //cell colors
  for (int i = 0; i < maskData.size(); ++i)
  {
    std::string regionName = std::to_string(i);
    regionName.append(" : " + maskData[i].m_name);

    maskList[0, i]->Value = gcnew String(regionName.c_str());
    maskList[0, i]->Style->BackColor = Color::FromArgb(255, 255, 255);
    maskList[1, i]->Style->BackColor = Color::FromArgb(maskData[i].m_color[0], maskData[i].m_color[1], maskData[i].m_color[2]);

    if (i == maskSelectedId) 
      checkbox_lock->CheckState = maskData[i].m_b_locked ? CheckState::Checked : CheckState::Unchecked;
  }


  this->Update();
  this->Invalidate();

  if (0 <= maskSelectedId && maskSelectedId < maskData.size() )
  {
    maskList->CurrentCell = maskList[0, maskSelectedId];
  }
  this->Update();
  this->Invalidate();

  m_bListUpdating = false;
}



System::Void FormVisMask::maskList_SelectionChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  //FormVisMask::updateList の maskList->Rows->Clear(); のタイミングで呼ばれてしまうので、その際は何もしない
  if (m_bListUpdating) return;

  std::cout << "selection changed " << maskList->CurrentCell->RowIndex << " " << maskList->CurrentCell->ColumnIndex << "\n";
  ImageCore::GetInst()->SetActiveMaskID( maskList->CurrentCell->RowIndex );

  //modify values
  const std::vector<MaskData> &maskData = ImageCore::GetInst()->GetMaskData();
  const int                   &selectid = ImageCore::GetInst()->GetActiveMaskID();

  checkbox_lock->CheckState = maskData[selectid].m_b_locked ? CheckState::Checked : CheckState::Unchecked;
  trackbar_alpha->Value = (int)(100 * maskData[selectid].m_alpha);

}



System::Void FormVisMask::maskList_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e)
{

}


System::Void FormVisMask::btnColorPallet_Click  (
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  System::Windows::Forms::ColorDialog ^colorDialog 
    = gcnew System::Windows::Forms::ColorDialog();  
  if (colorDialog->ShowDialog() != System::Windows::Forms::DialogResult::OK) 
    return;
  
  EVec3i c(colorDialog->Color.R, colorDialog->Color.G, colorDialog->Color.B);
  ImageCore::GetInst()->ActiveMask_SetColor( c );

  updateList();
  RedrawScene();
}



System::Void FormVisMask::checkbox_lock_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  bool locked = (checkbox_lock->CheckState == CheckState::Checked);
  ImageCore::GetInst()->ActiveMask_SetLocked( locked );
  RedrawScene();
}


System::Void FormVisMask::trackbar_alpha_Scroll (
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  ImageCore::GetInst()->ActiveMask_SetAlpha( trackbar_alpha->Value / 100.0);
  RedrawScene();
}


//DELETE/MARGE/
System::Void FormVisMask::btnDelete_Click(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  ImageCore::GetInst()->ActiveMask_Delete();
  updateList();
  RedrawScene();
}



System::Void FormVisMask::btnMargeTo_Click( 
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  const auto &maskdata = ImageCore::GetInst()->GetMaskData();

  std::set<int> trgt_ids = formMaskIdSelection_DoMultiSelection(maskdata, 0);

  if ( trgt_ids.size() <= 1) 
  {
    const char* m = 
      "specify multiple ids to marge\n"
      "マージする複数IDを指定してください";
    CLI_MessageBox_OK_Show( m, "message");
    return;
  }

  ImageCore::GetInst()->MargeMaskIDs(trgt_ids);
  
  updateList();
  RedrawScene();
}



//ERODE/DILATE/FILLHOLE
System::Void FormVisMask::btnErode_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  ImageCore::GetInst()->ActiveMask_Erode();
  updateList();
  RedrawScene();
}



System::Void FormVisMask::btnDilate_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  ImageCore::GetInst()->ActiveMask_Dilate();
  updateList();
  RedrawScene();
}



System::Void FormVisMask::btnFillHole_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  const auto &maskdata = ImageCore::GetInst()->GetMaskData();
  const int  selectid  = ImageCore::GetInst()->GetActiveMaskID();

  std::set<int> trgt_ids = formMaskIdSelection_DoMultiSelection(maskdata, selectid);

  if ( trgt_ids.size() < 1) 
  {
    const char* m = 
      "Select multiple ids to calc fillhole\n"
      "穴埋めに利用する領域を複数指定してください";
    CLI_MessageBox_OK_Show( m, "message");
    return;
  }

  ImageCore::GetInst()->FillHole( trgt_ids );
  updateList();
  RedrawScene();
}







System::Void FormVisMask::btnExpObj_Click(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "surface data (*.obj)|*.obj";

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr  = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());

  ImageCore::GetInst()->ActiveMask_ExportObj(fname);
}



System::Void FormVisMask::btnSmartFillHole_Click(
  System::Object^  sender,
  System::EventArgs^  e)
{
  const auto &maskdata = ImageCore::GetInst()->GetMaskData();
  const int  selectid  = ImageCore::GetInst()->GetActiveMaskID();

  std::set<int> trgt_ids = formMaskIdSelection_DoMultiSelection(maskdata, selectid);

  if ( trgt_ids.size() < 1) 
  {
    const char* m = 
      "Select multiple ids to calc fillhole\n"
      "穴埋めに利用する領域を複数指定してください";
    CLI_MessageBox_OK_Show( m, "message");
    return;
  }

  const char *message1 = "Set dilation radius";
  int dilation_size = 0;
  if ( !FormIntegerSelection_doModal(1,1,30,message1,dilation_size) )
  {
    const char* m = 
      "Set radius of dilation for smart holefil\n"
      "holefill 直前に行うdilation回数を指定してください";
      CLI_MessageBox_OK_Show( m, "message");
    return;
  } 

  ImageCore::GetInst()->SmartFillHole(trgt_ids, dilation_size);
  updateList();
  RedrawScene();
}




