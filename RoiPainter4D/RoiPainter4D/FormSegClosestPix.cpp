#pragma managed
#include "FormSegClosestPix.h"
#pragma unmanaged
#include "./Mode/ModeSegClosestPix.h"
#pragma managed

using namespace RoiPainter4D;

void FormSegClosestPix::initItems(const int &frameN) {
  startFrameUpDown->Minimum = 1;
  startFrameUpDown->Maximum = frameN - 1;
  startFrameUpDown->Value = 1;

  endFrameUpDown->Minimum = 1;
  endFrameUpDown->Maximum = frameN - 1;
  endFrameUpDown->Value = frameN - 1;
}

System::Void FormSegClosestPix::btnGo_Click(System::Object^  sender, System::EventArgs^  e) {
  m_startI = Decimal::ToInt32(startFrameUpDown->Value);
  m_endI = Decimal::ToInt32(endFrameUpDown->Value);
  if (m_startI <= m_endI) {
  }
}

System::Void FormSegClosestPix::btnOk_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeSegClosestPix::GetInst()->finishSegmentation();
}

System::Void FormSegClosestPix::btnCancel_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegClosestPix::GetInst()->cancelSegmentation();
}

System::Void FormSegClosestPix::startFrameUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
  int startI = Decimal::ToInt32(startFrameUpDown->Value);
  baseFrameTextBox->Text = Convert::ToString(startI - 1);
}