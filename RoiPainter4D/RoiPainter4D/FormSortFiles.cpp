#pragma managed
#include "FormSortFiles.h"

using namespace RoiPainter4D;
using namespace Runtime::InteropServices;

static std::string n_getFileName(const std::string &filePath) {
  int path_i = (int)filePath.find_last_of("\\") + 1;
  int ext_i  = (int)filePath.find_last_of(".");

  return filePath.substr(path_i, ext_i - path_i);
}

// String^ -> string
static void n_marshalString(String ^ s, std::string& os) {
  const char* chars =
    (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
  os = chars;
  Marshal::FreeHGlobal(IntPtr((void*)chars));
}

static void n_swapInt(int &a, int &b) {
  int temp = a;
  a = b;
  b = temp;
}


// 文字列リストの文字列が昇順/降順になるようにIndexリストを並び替える
static void n_sortIdxsByChara(
    const std::vector<std::string> &strList,
    const bool &isUp, 
  std::vector<int> &idxList) {
  // bubble sort
  for (int i = (int)strList.size() - 1; i > 0; i--) 
  {
    for (int j = 0; j < i; j++) 
    {
      std::string str1 = strList[idxList[j]], str2 = strList[idxList[j + 1]];

      bool bUp = false, bDown = false;
      for (int ch = 0, chN = (int) Math::Max(str1.length(), str2.length()); ch < chN; ++ch)
      {
        if (ch >= str1.length()) { bUp   = true; break; } // str1の方が短い　昇順ではstr1->str2
        if (ch >= str2.length()) { bDown = true; break; } // str2の方が短い　昇順ではstr2->str1
        if (str1[ch] < str2[ch]) { bUp   = true; break; } // str1の方が小    昇順ではstr1->str2
        if (str1[ch] > str2[ch]) { bDown = true; break; } // str2の方が小  　昇順ではstr2->str1
      }
      if (isUp) { if (bDown) n_swapInt(idxList[j], idxList[j + 1]); } // 昇順
      else { if (bUp)   n_swapInt(idxList[j], idxList[j + 1]); } // 降順
    }
  }
}



void FormSortFiles::initList(
    const std::vector<std::string> &fNames,
    const std::vector<std::string> &fDateTimes) 
{

  //初期化
  fnameList->Rows->Clear();
  for (int i = 0; i < fNames.size(); ++i) fnameList->Rows->Add();

  //このタイミングでhighlightをoffに
  fnameList->CurrentCell = nullptr;

  for (int i = 0; i < fNames.size(); ++i)
  {
    fnameList[0, i]->Value = i;
    fnameList[1, i]->Value = gcnew String(fNames[i].c_str());
    fnameList[2, i]->Value = gcnew String(fDateTimes[i].c_str());
  }

  //fnameList->Columns[1]->DefaultCellStyle->Alignment =  DataGridViewContentAlignment::MiddleRight;
  this->Update();
  this->Invalidate();
}



void FormSortFiles::updateList(const std::vector<int> &idxs) 
{
  std::vector<std::string> fnames(idxs.size());
  std::vector<std::string> fDates(idxs.size());
  int N = (int)idxs.size();
  for (int i = 0; i < N; ++i)
  {
    n_marshalString(fnameList[1, idxs[i]]->Value->ToString(), fnames[i]);
    n_marshalString(fnameList[2, idxs[i]]->Value->ToString(), fDates[i]);
  }

  for (int i = 0; i < N; ++i)
  {
    fnameList[1, i]->Value = gcnew String(fnames[i].c_str());
    fnameList[2, i]->Value = gcnew String(fDates[i].c_str());
  }
  this->Update();
  this->Invalidate();
}



void FormSortFiles::copyNameList(std::vector<std::string> &fNames) {
  for (int i = 0; i < fNames.size(); ++i)
  {
    n_marshalString(fnameList[1, i]->Value->ToString(), fNames[i]);
  }

}



System::Void FormSortFiles::btnSort_Click(System::Object^  sender, System::EventArgs^  e) {
  std::vector<std::string> strList(fnameList->RowCount);
  std::vector<int> idxs(fnameList->RowCount);

  for (int i = 0; i < strList.size(); ++i)
  {
    idxs[i] = i;
    n_marshalString(fnameList[(radioBtnName->Checked) ? 1 : 2, i]->Value->ToString(), strList[i]);
  }

  n_sortIdxsByChara(strList, radioBtnUp->Checked, idxs);

  updateList(idxs);
}

System::Void FormSortFiles::btnImport_Click(System::Object^  sender, System::EventArgs^  e) {
}

System::Void FormSortFiles::btnCancel_Click(System::Object^  sender, System::EventArgs^  e) {
}
