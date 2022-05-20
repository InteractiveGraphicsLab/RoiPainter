#pragma managed
#include "FormRefStrokeTrim.h"
#include "FormMain.h"

#pragma unmanaged
#include "ModeCore.h"
#include "mode/ModeRefStrokeTrim.h"

#pragma managed



using namespace RoiPainter3D;
System::Void FormRefStrokeTrim::btn_cancel_Click(System::Object^  sender, System::EventArgs^  e){
  ModeRefStrokeTrim::GetInst()->cancelSegmentation();
  RedrawScene();
}


System::Void FormRefStrokeTrim::btn_finish_Click(System::Object^  sender, System::EventArgs^  e){
  ModeRefStrokeTrim::GetInst()->finishSegmentation();
  RedrawScene();
}
