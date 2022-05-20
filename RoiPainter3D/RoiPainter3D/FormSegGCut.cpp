#pragma managed
#include "FormSegGCut.h"
#include "FormMain.h"

#pragma unmanaged
#include "mode/ModeSegGCut.h"

#pragma managed

static const float SLIDER_RATE = 0.1f;

using namespace RoiPainter3D;


FormSegGCut::FormSegGCut(void)
{
  InitializeComponent();

  trackBar_lambda->SetRange(0,10000);
  trackBar_lambda->Value = 1000;

  textBox_lambda->Text = (trackBar_lambda->Value * SLIDER_RATE).ToString();

  rbtn_thick1->Checked = true;
  rbtn_thick2->Checked = false;
  rbtn_thick3->Checked = false;
}




System::Void FormSegGCut::btn_clearAllCPs_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeSegGCut::getInst()->ClearAllCPs();
  RedrawScene();
}

System::Void FormSegGCut::trackBar_lambda_Scroll(System::Object^  sender, System::EventArgs^  e) {
  textBox_lambda->Text = (trackBar_lambda->Value * SLIDER_RATE).ToString();  
}

System::Void FormSegGCut::btn_runGraphCutWS_Click(System::Object^  sender, System::EventArgs^  e) {
	ModeSegGCut::getInst()->RunGraphCutWsdLv( trackBar_lambda->Value * SLIDER_RATE );
}

System::Void FormSegGCut::btn_runGraphCutVoxel_Click(System::Object^  sender, System::EventArgs^  e) {
  float lambda = trackBar_lambda->Value * SLIDER_RATE;
	int seamW = rbtn_thick1->Checked ? 1 : 
		          rbtn_thick2->Checked ? 2 : 3;
	ModeSegGCut::getInst()->RunGraphCutVoxLv( lambda, seamW, checkBox_bandOnlyBack->Checked ? true:false );
}

System::Void FormSegGCut::rbtn_thick1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {}
System::Void FormSegGCut::rbtn_thick2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {}
System::Void FormSegGCut::rbtn_thick3_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {}

System::Void FormSegGCut::checkBox_bandOnlyBack_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {}
System::Void FormSegGCut::btn_canel_Click(System::Object^ sender, System::EventArgs^   e) {
  ModeSegGCut::getInst()->CancelSegmentation();
  RedrawScene();
}
System::Void FormSegGCut::btn_finish_Click(System::Object^ sender, System::EventArgs^  e) {
  ModeSegGCut::getInst()->FinishSegmemntation();
  RedrawScene();
}