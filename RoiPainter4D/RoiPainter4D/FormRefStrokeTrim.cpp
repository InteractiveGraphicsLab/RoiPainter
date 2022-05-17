#pragma managed
#include "FormRefStrokeTrim.h"

#pragma unmanaged
#include "./Mode/ModeRefStrokeTrim.h"
#pragma managed

using namespace RoiPainter4D;

System::Void FormRefStrokeTrim::btn_ok_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeRefStrokeTrim::GetInst()->finishTrim();
}

System::Void FormRefStrokeTrim::btn_cancel_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeRefStrokeTrim::GetInst()->cancelBtnClick();
}