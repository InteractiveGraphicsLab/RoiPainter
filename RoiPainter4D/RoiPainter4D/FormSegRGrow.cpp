#pragma managed
#include "FormSegRGrow.h"
#include "FormVisParam.h"

#pragma unmanaged
#include "./Mode/ModeSegRGrow.h"
#pragma managed


using namespace RoiPainter4D;


System::Void FormSegRGrow::InitAllItems(int volmin, int volmax)
{
  m_item_updating = true;
  minThresholdBar->SetRange(volmin, volmax);
  maxThresholdBar->SetRange(volmin, volmax);

  minThresholdBar->Value = volmin;
  maxThresholdBar->Value = volmax;

  minThreshTextBox->Text = minThresholdBar->Value.ToString();
  maxThreshTextBox->Text = maxThresholdBar->Value.ToString();
  m_item_updating = false;
  
  this->Focus();
  minThreshTextBox->Focus();
}




System::Void FormSegRGrow::btn_runThresholding_Click(System::Object^  sender, System::EventArgs^  e)
{
  short minV = minThresholdBar->Value;
  short maxV = maxThresholdBar->Value;
  ModeSegRGrow::GetInst()->RunThresholding(minV, maxV);
}

System::Void FormSegRGrow::btn_runRegionGrow_Click(System::Object^  sender, System::EventArgs^  e)
{
  short minV = minThresholdBar->Value;
  short maxV = maxThresholdBar->Value;
  ModeSegRGrow::GetInst()->RunRegionGrow8(minV, maxV);
}

System::Void FormSegRGrow::btn_runThresholding_1frame_Click(System::Object^  sender, System::EventArgs^  e)
{
  int   frameI = FormVisParam::GetInst()->GetframeI();
  short minV = minThresholdBar->Value;
  short maxV = maxThresholdBar->Value;
  ModeSegRGrow::GetInst()->RunThresholding_OneFrame(minV, maxV, frameI);
}

System::Void FormSegRGrow::btn_runRegionGrow_1frame_Click(System::Object^  sender, System::EventArgs^  e)
{
  int   frameI = FormVisParam::GetInst()->GetframeI();
  short minV = minThresholdBar->Value;
  short maxV = maxThresholdBar->Value;
  ModeSegRGrow::GetInst()->RunRegionGrow6_OneFrame(minV, maxV, frameI);
}


System::Void FormSegRGrow::btn_erode_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegRGrow::GetInst()->RunErosion3D_EachFrame();
}

System::Void FormSegRGrow::btn_dilate_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegRGrow::GetInst()->RunDilation3D_EachFrame();
}

System::Void FormSegRGrow::btn_fillHole_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegRGrow::GetInst()->RunFillHole3D_EachFrame();
}

System::Void FormSegRGrow::btn_ok_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegRGrow::GetInst()->FinishSegmentation();
}

System::Void FormSegRGrow::btn_cancel_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegRGrow::GetInst()->cancelSegmentation();
}

// minV, maxVの値は可能であれば手打ちと、track barを併用してほしい
System::Void FormSegRGrow::minThreshold_Scroll(System::Object^  sender, System::EventArgs^  e) {
  if(m_item_updating) return;
  m_item_updating = true;

  minThreshTextBox->Text = minThresholdBar->Value.ToString();
  minThreshTextBox->Refresh();

  m_item_updating = false;
}

System::Void FormSegRGrow::maxThreshold_Scroll(System::Object^  sender, System::EventArgs^  e) {
  if(m_item_updating) return;
  m_item_updating = true;

  maxThreshTextBox->Text = maxThresholdBar->Value.ToString();
  maxThreshTextBox->Refresh();

  m_item_updating = false;
}



System::Void FormSegRGrow::minThreshTextBox_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
  if(m_item_updating) return;
  m_item_updating = true;


  std::cout << "minThreshTextBox_TextChanged\n";
  int x;
  if (Int32::TryParse(minThreshTextBox->Text, x)) 
  {
    if (x < minThresholdBar->Minimum) 
    {
      x = minThresholdBar->Minimum;
      minThreshTextBox->Text = (x).ToString();
    }
    else if (x > minThresholdBar->Maximum) 
    {
      x = minThresholdBar->Maximum;
      minThreshTextBox->Text = (x).ToString();
    }
    minThresholdBar->Value = x;
  }
  else if(minThreshTextBox->Text=="-")
  {
    x= 0;
  }
  else 
  {
    minThreshTextBox->Text = "0";
    minThresholdBar->Value = 0;
  }

  m_item_updating = false;

}



System::Void FormSegRGrow::maxThreshTextBox_TextChanged(System::Object^  sender, System::EventArgs^  e) 
{
  if(m_item_updating) return;
  m_item_updating = true;


  int x;
  if (Int32::TryParse(maxThreshTextBox->Text, x)) 
  {
    if (x < maxThresholdBar->Minimum) 
    {
      x = maxThresholdBar->Minimum;
      maxThreshTextBox->Text = (x).ToString();
    }
    else if (x > maxThresholdBar->Maximum)
    {
      x = maxThresholdBar->Maximum;
      maxThreshTextBox->Text = (x).ToString();
    }
    maxThresholdBar->Value = x;
  }
  else if(maxThreshTextBox->Text=="-")
  {
    x= 0;
  }
  else 
  {
    maxThreshTextBox->Text = "0";
    maxThresholdBar->Value = 0;
  }

  m_item_updating = false;
}
