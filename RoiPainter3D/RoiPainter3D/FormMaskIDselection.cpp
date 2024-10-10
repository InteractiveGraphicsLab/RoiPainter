#include "FormMaskIDselection.h"


using namespace RoiPainter3D;


System::Void FormMaskIDselection::initList(
  const std::vector<MaskData> &mask_set,
  const int default_mask_id,
  const int b_multiselect) 
{
  m_bListInit = true;

 
  //初期化
  maskIdList->Rows->Clear();
  for (int i = 0; i < mask_set.size(); ++i) maskIdList->Rows->Add();

  //このタイミングでhighlightをoffに
  maskIdList->CurrentCell = nullptr;

  //cell colors
  for (int i = 0; i < mask_set.size(); ++i)
  {
    std::string regionName = std::to_string(i);
    regionName.append(" : " + mask_set[i].m_name);

    maskIdList[0, i]->Value = gcnew String(regionName.c_str());
    maskIdList[0, i]->Style->BackColor = Color::FromArgb(255, 255, 255);
    maskIdList[1, i]->Style->BackColor = Color::FromArgb(mask_set[i].m_color[0], mask_set[i].m_color[1], mask_set[i].m_color[2]);
  }

  maskIdList->MultiSelect = b_multiselect;


  this->Update();
  this->Invalidate();

  m_active_ids->Clear();
  if ( 0 <= default_mask_id && default_mask_id < mask_set.size()) 
  {
    m_active_ids->Add(default_mask_id);
    maskIdList->CurrentCell = maskIdList[0, default_mask_id];
  }

  this->Update();
  this->Invalidate();

  m_bListInit = false;
}



System::Void FormMaskIDselection::maskIdList_SelectionChanged(
  System::Object^  sender, 
  System::EventArgs^  e)
{
  //FormVisMask::updateList の maskList->Rows->Clear(); 
  //のタイミングで呼ばれてしまうので、その際は何もしない
  if (m_bListInit) return;

  if( m_bMultiSelect )
  {
    m_active_ids->Clear();

    for( int i = 0; i < maskIdList->SelectedCells->Count; ++i )
    {
      std::cout << i << " selection changed  " 
                << maskIdList->SelectedCells[i]->RowIndex << "  " 
                << maskIdList->SelectedCells[i]->ColumnIndex << "\n";

      m_active_ids->Add(maskIdList->SelectedCells[i]->RowIndex);
    }
  }
  else
  {
    //single selection
    std::cout << "selection changed " 
              << maskIdList->CurrentCell->RowIndex << " " 
              << maskIdList->CurrentCell->ColumnIndex << "\n";

    m_active_ids->Clear();
    m_active_ids->Add(maskIdList->CurrentCell->RowIndex);
  }

}

