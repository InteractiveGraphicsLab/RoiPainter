#pragma managed
#include "FormVisMask.h"
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSelectMskId.h"

#pragma unmanaged
#include "ImageCore.h"
#include <iostream>
#include <string>
#include <vector>
#pragma managed

using namespace RoiPainter4D;
using namespace System::Runtime::InteropServices;


void FormVisMask::updateList()
{
  m_bListUpdating = true;

  const auto &maskData       = ImageCore::GetInst()->GetMaskData();
  const int  &maskSelectedId = ImageCore::GetInst()->GetSelectMaskIdx();

  //初期化
  maskList->Rows->Clear();
  for (int i = 0; i < maskData.size(); ++i) maskList->Rows->Add();

  //このタイミングでhighlightをoffに
  maskList->CurrentCell = nullptr;

  //cell colors
  for (int i = 0; i < maskData.size(); ++i)
  {
    std::string regionName = std::to_string(i);
    regionName.append(" : " + maskData[i].name);

    maskList[0, i]->Value = gcnew String(regionName.c_str());
    maskList[0, i]->Style->BackColor = Color::FromArgb(255, 255, 255);
    maskList[1, i]->Style->BackColor = Color::FromArgb(maskData[i].color[0], maskData[i].color[1], maskData[i].color[2]);

    if (i == maskSelectedId) 
    {
      checkbox_lock->CheckState = maskData[i].lock ? CheckState::Checked : CheckState::Unchecked;
      trackbar_alpha->Value = (int)(100 * maskData[i].alpha);
    }
  }

  this->Update();
  this->Invalidate();

  if (0 <= maskSelectedId) maskList->CurrentCell = maskList[0, maskSelectedId];
  this->Update();
  this->Invalidate();

  m_bListUpdating = false;
}




System::Void FormVisMask::maskList_SelectionChanged(System::Object^  sender, System::EventArgs^  e)
{
  //FormVisMask::updateList の maskList->Rows->Clear(); のタイミングで呼ばれてしまうので、その際は何もしない
  if (m_bListUpdating) return;

  std::cout << "selection changed " << maskList->CurrentCell->RowIndex << " " << maskList->CurrentCell->ColumnIndex << "\n";
  ImageCore::GetInst()->SetSelectMaskId( maskList->CurrentCell->RowIndex );
  updateList();
}


System::Void FormVisMask::btnColorPallet_Click(System::Object^  sender, System::EventArgs^  e)
{
  if (colorDialog1->ShowDialog() != System::Windows::Forms::DialogResult::OK) return;
  ImageCore::GetInst()->SelectedMsk_SetColor( EVec3i(colorDialog1->Color.R, colorDialog1->Color.G, colorDialog1->Color.B) );
  updateList();
  formMain_RedrawMainPanel();
}



System::Void FormVisMask::checkbox_lock_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
  ImageCore::GetInst()->SelectedMsk_SetLocked( checkbox_lock->CheckState == CheckState::Checked ? true : false );
  formMain_RedrawMainPanel();
}


System::Void FormVisMask::trackbar_alpha_Scroll(System::Object^  sender, System::EventArgs^  e)
{
  ImageCore::GetInst()->SelectedMsk_SetAlpha( trackbar_alpha->Value / (float)100.0 );
  formMain_RedrawMainPanel();
}



static void updateImageCoreVisVolumes()
{
  //4D volume (cpu) --> vis volume (gpu)
  const int fi = formVisParam_getframeI();
  const int winLvMin = formVisParam_getWinLvMin();
  const int winLvMax = formVisParam_getWinLvMax();
  ImageCore::GetInst()->UpdateVisVolume(winLvMin, winLvMax, fi);

}


System::Void FormVisMask::btnDelete_Click(System::Object^  sender, System::EventArgs^  e)
{
  ImageCore::GetInst()->SelectedMsk_delete();
  updateList();

  updateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}


System::Void FormVisMask::btnMargeTo_Click(System::Object^  sender, System::EventArgs^  e)
{
  FormSelectMskId ^modal = gcnew FormSelectMskId();
  if (modal->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  int trgtId = modal->getTrgtID();
  modal->Close();

  ImageCore::GetInst()->SelectedMsk_marge(trgtId);
  updateList();

  updateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}


System::Void FormVisMask::btnThisErode_Click(System::Object^  sender, System::EventArgs^  e) {
  ImageCore::GetInst()->SelectedMsk_erodeOne(formVisParam_getframeI());

  updateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}


System::Void FormVisMask::btnThisDilate_Click(System::Object^  sender, System::EventArgs^  e) {
  ImageCore::GetInst()->SelectedMsk_dilateOne(formVisParam_getframeI());

  updateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}


System::Void FormVisMask::btnThisFill_Click(System::Object^  sender, System::EventArgs^  e) {
  ImageCore::GetInst()->SelectedMsk_fillHoleOne(formVisParam_getframeI());

  updateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}


System::Void FormVisMask::btnThisExpObj_Click(System::Object^  sender, System::EventArgs^  e) {
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "surface data (*.obj)|*.obj||";

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());

  ImageCore::GetInst()->SelectedMsk_expObjOne(fname, formVisParam_getframeI());
}


System::Void FormVisMask::btnThisImpObj_Click(System::Object^ sender, System::EventArgs^ e) {

  std::string fname;

  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Title = "select mesh (メッシュモデルを選択してください)";
  dlg->Filter = "mesh obj (*.obj;)|*.obj;";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;
  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  fname = static_cast<const char*>(mptr.ToPointer());

  const int frame_idx = formVisParam_getframeI();
  ImageCore::GetInst()->importObjOne(fname, frame_idx);
  updateImageCoreVisVolumes();
  formVisMask_updateList();
  formMain_RedrawMainPanel();
}


System::Void FormVisMask::btnAllErode_Click(System::Object^  sender, System::EventArgs^  e) {
  ImageCore::GetInst()->SelectedMsk_erodeAll();

  updateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}


System::Void FormVisMask::btnAllDilate_Click(System::Object^  sender, System::EventArgs^  e) {
  ImageCore::GetInst()->SelectedMsk_dilateAll();

  updateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}


System::Void FormVisMask::btnAllFill_Click(System::Object^  sender, System::EventArgs^  e) {
  ImageCore::GetInst()->SelectedMsk_fillHoleAll();

  updateImageCoreVisVolumes();
  formMain_RedrawMainPanel();
}


System::Void FormVisMask::btnAllExpObj_Click(System::Object^  sender, System::EventArgs^  e) {
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "surface data (*.obj)|*.obj||";

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());

  ImageCore::GetInst()->SelectedMsk_expObjAll(fname);
}


System::Void FormVisMask::btnAllImpObj_Click(System::Object^ sender, System::EventArgs^ e) {
  std::vector<std::string> fNames;

  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Title = "select mesh (メッシュモデルを選択してください)";
  dlg->Filter = "mesh obj (*.obj;)|*.obj;";
  dlg->Multiselect = true;

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel)
    return;

  for each (auto it in  dlg->FileNames)
  {
    std::string fName;
    IntPtr mptr = Marshal::StringToHGlobalAnsi(it);
    fName = static_cast<const char*>(mptr.ToPointer());
    fNames.push_back(fName);
    std::cout << fName.c_str() << "\n";
  }

  ImageCore::GetInst()->importObjAll(fNames);
  updateImageCoreVisVolumes();
  formVisMask_updateList();
  formMain_RedrawMainPanel();
}
