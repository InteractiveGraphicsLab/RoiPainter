#pragma managed
#include "FormSegVoxelPaint.h"
#include "FormMain.h"

#pragma unmanaged
#include "mode/ModeSegVoxelPaint.h"

#pragma managed


using namespace RoiPainter3D;

System::Void FormSegVoxelPaint::rbtn_paint_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
  RedrawScene();
}
System::Void FormSegVoxelPaint::rbtn_lasso_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
  RedrawScene();
}
System::Void FormSegVoxelPaint::btn_Cancel_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeSegVoxelPaint::GetInst()->CancelSegmentation();
  RedrawScene();
}
System::Void FormSegVoxelPaint::btn_finish_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeSegVoxelPaint::GetInst()->FinishSegmentation();
  RedrawScene();
}
