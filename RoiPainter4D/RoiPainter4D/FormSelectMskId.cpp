#pragma managed
#include "FormSelectMskId.h"

#pragma unmanaged
#include <iostream>
#include <string>
#include <vector>
#include "ImageCore.h" 
#pragma managed

using namespace RoiPainter4D;


System::Void FormSelectMskId::initList() {
  m_bListInit = true;

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

    //if (i == maskSelectedId) checkbox_lock->CheckState = maskData[i].lock ? CheckState::Checked : CheckState::Unchecked;
  }


  this->Update();
  this->Invalidate();

  if (0 <= maskSelectedId) {
    m_activeId = maskSelectedId;
    maskList->CurrentCell = maskList[0, maskSelectedId];
  }

  this->Update();
  this->Invalidate();

  m_bListInit = false;


}



System::Void FormSelectMskId::maskList_SelectionChanged(System::Object^  sender, System::EventArgs^  e) {
  //FormVisMask::updateList の maskList->Rows->Clear(); のタイミングで呼ばれてしまうので、その際は何もしない
  if (m_bListInit) return;

  std::cout << "selection changed " << maskList->CurrentCell->RowIndex << " " << maskList->CurrentCell->ColumnIndex << "\n";
  m_activeId = maskList->CurrentCell->RowIndex;


}
