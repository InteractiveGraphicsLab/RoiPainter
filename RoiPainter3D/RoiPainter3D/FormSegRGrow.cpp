#pragma managed
#include "FormSegRGrow.h"
#include "FormMain.h"

#pragma unmanaged
#include "mode/ModeSegRGrow.h"
#include "ImageCore.h"

#pragma managed



using namespace RoiPainter3D;

void FormSegRGrow::InitAllItems(short volMin, short volMax)
{
  trackbar_min->SetRange( volMin, volMax);
  trackbar_max->SetRange( volMin, volMax);
  trackbar_min->Value = volMin;
  trackbar_max->Value = volMax;
  textBox_min->Text = trackbar_min->Value.ToString();
  textBox_max->Text = trackbar_max->Value.ToString();

  trackBar_maxiteration->SetRange( 1, ImageCore::GetInst()->GetNumVoxels() / 30 );
  trackBar_maxiteration->Value = 10;
  textBox_maxiteration->Text = trackBar_maxiteration->Value.ToString();
}


System::Void FormSegRGrow::textBox_max_TextChanged(System::Object^ sender, System::EventArgs^ e) 
{
  int x;
  if (Int32::TryParse(textBox_max->Text, x)) 
  {
    t_crop( trackbar_max->Minimum, trackbar_max->Maximum, x);
    textBox_max->Text = (x).ToString();
    trackbar_max->Value = x;
  }
  else 
  {
    textBox_max->Text = "0";
    trackbar_max->Value = 0;
  }

}

System::Void FormSegRGrow::textBox_min_TextChanged(System::Object^ sender, System::EventArgs^ e) 
{
  int x;
  if (Int32::TryParse(textBox_min->Text, x)) 
  {
    t_crop( trackbar_min->Minimum, trackbar_min->Maximum, x);
    textBox_min->Text = (x).ToString();
    trackbar_min->Value = x;
  }
  else 
  {
    textBox_min->Text = "0";
    trackbar_min->Value = 0;
  }
}



System::Void FormSegRGrow::trackbar_min_Scroll(System::Object^  sender, System::EventArgs^     e) 
{
  textBox_min->Text = trackbar_min->Value.ToString();
  trackbar_min->Refresh();
}

System::Void FormSegRGrow::trackBar_max_Scroll(System::Object^  sender, System::EventArgs^     e) 
{
  textBox_max->Text = trackbar_max->Value.ToString();
  trackbar_max->Refresh();
}

System::Void FormSegRGrow::btn_thresholding_Click(System::Object^  sender, System::EventArgs^  e) 
{ 
  ModeSegRGrow::GetInst()->RunThresholding( (short)trackbar_min->Value, (short)trackbar_max->Value);
  RedrawScene();
}

System::Void FormSegRGrow::btn_rgrow6_Click(System::Object^  sender, System::EventArgs^    e) 
{
  ModeSegRGrow::GetInst()->RunRegionGrow6((short)trackbar_min->Value, (short)trackbar_max->Value);
  RedrawScene();
}
System::Void FormSegRGrow::btn_rgrow26_Click(System::Object^  sender, System::EventArgs^   e)
{
  ModeSegRGrow::GetInst()->RunRegionGrow26((short)trackbar_min->Value, (short)trackbar_max->Value);
  RedrawScene();
}
System::Void FormSegRGrow::btn_erode_Click(System::Object^  sender, System::EventArgs^     e) {
  ModeSegRGrow::GetInst()->RunErosion();
  RedrawScene();
}
System::Void FormSegRGrow::btn_dilate_Click(System::Object^  sender, System::EventArgs^    e) {
  ModeSegRGrow::GetInst()->RunDilation();
  RedrawScene();
}
System::Void FormSegRGrow::btn_fillhole_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeSegRGrow::GetInst()->RunFillHole();
  RedrawScene();
} 
System::Void FormSegRGrow::btn_finish_Click(System::Object^  sender, System::EventArgs^    e) {
  ModeSegRGrow::GetInst()->FinishSegmentation();
  RedrawScene();
}

System::Void FormSegRGrow::trackBar_maxiteration_Scroll(System::Object^  sender, System::EventArgs^  e) 
{
  textBox_maxiteration->Text = trackBar_maxiteration->Value.ToString();
}
