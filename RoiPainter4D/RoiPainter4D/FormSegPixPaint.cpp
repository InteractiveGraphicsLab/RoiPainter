#pragma managed
#include "FormSegPixPaint.h"
#pragma unmanaged
#include "./Mode/ModeSegPixPaint.h"
#pragma managed

using namespace RoiPainter4D;


System::Void FormSegPixPaint::okButton_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegPixPaint::GetInst()->FinishSegmentation();
}

System::Void FormSegPixPaint::cancelButton_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegPixPaint::GetInst()->CancelSegmentation();
}

