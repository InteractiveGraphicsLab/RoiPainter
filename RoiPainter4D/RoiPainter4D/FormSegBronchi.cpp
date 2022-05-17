#pragma managed
#include "FormSegBronchi.h"
#include "FormVisParam.h"
#include "FormMain.h"

#pragma unmanaged
#include "./Mode/ModeSegBronchi.h"
#pragma managed

#include <msclr/marshal_cppstd.h>

using namespace System::Runtime::InteropServices;
using namespace RoiPainter4D;




void RoiPainter4D::FormSegBronchi::InitAllItems(int vol_min, int vol_max)
{
  minThreshold_trackBar      ->SetRange(vol_min, vol_max);
  maxThresholdFormer_trackBar->SetRange(vol_min, vol_max);
  maxThresholdLatter_trackBar->SetRange(vol_min, vol_max);

  minThreshold_trackBar->Value       = vol_min;
  maxThresholdFormer_trackBar->Value = vol_max;
  maxThresholdLatter_trackBar->Value = vol_max;

  minThreshold_textBox->Text = minThreshold_trackBar->Value.ToString();
  maxThresholdFormer_textBox->Text = maxThresholdFormer_trackBar->Value.ToString();
  maxThresholdLatter_textBox->Text = maxThresholdLatter_trackBar->Value.ToString();
}


System::Void RoiPainter4D::FormSegBronchi::file_loadCp_Click(System::Object^ sender, System::EventArgs^ e)
{
  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = "load cp file (*.txt;)|*.txt;";
  dlg->Multiselect = false;
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeSegBronchi::GetInst()->LoadCp(fname);

  return System::Void();
}


System::Void RoiPainter4D::FormSegBronchi::file_saveCp_Click(System::Object^ sender, System::EventArgs^ e)
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "save path file (*.txt;)|*.txt;";
  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  ModeSegBronchi::GetInst()->SaveCp(fname);

  return System::Void();
}


System::Void RoiPainter4D::FormSegBronchi::copyPrevFrameButton_Click(System::Object^ sender, System::EventArgs^ e)
{
  std::cout << "Push copy prev frame\n";
  ModeSegBronchi::GetInst()->CopyPrevFrameCp();
  return System::Void();
}


System::Void RoiPainter4D::FormSegBronchi::upControllPointSize_buttonClick(System::Object^ sender, System::EventArgs^ e)
{
  ModeSegBronchi::GetInst()->UpControlPointSize();

  return System::Void();
}


System::Void RoiPainter4D::FormSegBronchi::downControllPointSize_buttonClick(System::Object^ sender, System::EventArgs^ e)
{
  ModeSegBronchi::GetInst()->DownControlPointSize();

  return System::Void();
}


System::Void RoiPainter4D::FormSegBronchi::upPathWidth_buttonClick(System::Object^ sender, System::EventArgs^ e)
{
  ModeSegBronchi::GetInst()->UpPathWidth();

  return System::Void();
}


System::Void RoiPainter4D::FormSegBronchi::downPathWidth_buttonClick(System::Object^ sender, System::EventArgs^ e)
{
  ModeSegBronchi::GetInst()->DownPathWidth();

  return System::Void();
}


System::Void RoiPainter4D::FormSegBronchi::runCurrentFrameRGrow_buttonClick(System::Object^ sender, System::EventArgs^ e)
{
  EVec3i thresh_minmax_fl;
  int::TryParse(minThreshold_textBox->Text, thresh_minmax_fl[0]);
  int::TryParse(maxThresholdFormer_textBox->Text, thresh_minmax_fl[1]);
  int::TryParse(maxThresholdLatter_textBox->Text, thresh_minmax_fl[2]);
  std::cout << "thresh_minmax_fl " << thresh_minmax_fl[0] << " " << thresh_minmax_fl[1] << " " << thresh_minmax_fl[2] << "\n";;

  int former_age, latter_age, RGrow_layer, Growth_width_layer;
  int::TryParse(this->formerAge_textBox->Text, former_age);
  int::TryParse(this->latterAge_textBox->Text, latter_age);
  int::TryParse(this->latterLayerRGrow_textBox->Text, RGrow_layer);
  int::TryParse(this->latterLayerB_textBox->Text, Growth_width_layer);

  std::cout << "former_age " << former_age << " latter_age " << latter_age
            << " 成長幅の後半レイヤー " << RGrow_layer << " 閾値の後半レイヤー " << Growth_width_layer <<"\n";
  if (RGrow_layer < 2 || Growth_width_layer < 1)
  {
    std::cout << "成長幅の後半レイヤーは2以上、閾値の後半レイヤーは1以上を設定してください\n";
    return;
  }

  ModeSegBronchi::GetInst()->RunOneFrameRegionGrow(thresh_minmax_fl, formVisParam_getframeI(), former_age, latter_age, Growth_width_layer, RGrow_layer);
}


System::Void RoiPainter4D::FormSegBronchi::finishSegmentation_buttonClick(System::Object^ sender, System::EventArgs^ e)
{
  ModeSegBronchi::GetInst()->FinishSegmentation();

  return System::Void();
}


System::Void RoiPainter4D::FormSegBronchi::minThresholdTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
  int x;
  if (Int32::TryParse(minThreshold_textBox->Text, x))
  {
    if (x < minThreshold_trackBar->Minimum)
    {
      x = minThreshold_trackBar->Minimum;
      minThreshold_textBox->Text = x.ToString();
    }
    else if (x > minThreshold_trackBar->Maximum)
    {
      x = minThreshold_trackBar->Maximum;
      minThreshold_textBox->Text = x.ToString();
    }
    minThreshold_trackBar->Value = x;
  }
  else if (minThreshold_textBox->Text == "-")
  {
    x = 0;
  }
  else
  {
    minThreshold_textBox->Text = "0";
    minThreshold_trackBar->Value = 0;
  }
}


System::Void RoiPainter4D::FormSegBronchi::maxThresholdFormerTextBox_textChanged(System::Object^ sender, System::EventArgs^ e)
{
  int x;
  if (Int32::TryParse(maxThresholdFormer_textBox->Text, x))
  {
    if (x < maxThresholdFormer_trackBar->Minimum)
    {
      x = maxThresholdFormer_trackBar->Minimum;
      maxThresholdFormer_textBox->Text = x.ToString();
    }
    else if (x > maxThresholdFormer_trackBar->Maximum)
    {
      x = maxThresholdFormer_trackBar->Maximum;
      maxThresholdFormer_textBox->Text = x.ToString();
    }
    maxThresholdFormer_trackBar->Value = x;
  }
  else if (maxThresholdFormer_textBox->Text == "-")
  {
    x = 0;
  }
  else
  {
    maxThresholdFormer_textBox->Text = "0";
    maxThresholdFormer_trackBar->Value = 0;
  }
}


System::Void RoiPainter4D::FormSegBronchi::maxThresholdLatterTextBox_textChanged(System::Object^ sender, System::EventArgs^ e)
{
  int x;
  if (Int32::TryParse(maxThresholdLatter_textBox->Text, x))
  {
    if (x < maxThresholdLatter_trackBar->Minimum)
    {
      x = maxThresholdLatter_trackBar->Minimum;
      maxThresholdLatter_textBox->Text = x.ToString();
    }
    else if (x > maxThresholdLatter_trackBar->Maximum)
    {
      x = maxThresholdLatter_trackBar->Maximum;
      maxThresholdLatter_textBox->Text = x.ToString();
    }
    maxThresholdLatter_trackBar->Value = x;
  }
  else if (maxThresholdLatter_textBox->Text == "-")
  {
    x = 0;
  }
  else
  {
    maxThresholdLatter_textBox->Text = "0";
    maxThresholdLatter_trackBar->Value = 0;
  }
}


System::Void RoiPainter4D::FormSegBronchi::minThreshold_Scroll(System::Object^ sender, System::EventArgs^ e)
{
  minThreshold_textBox->Text = minThreshold_trackBar->Value.ToString();
  minThreshold_textBox->Refresh();
}

System::Void RoiPainter4D::FormSegBronchi::maxThresholdFormer_Scroll(System::Object^ sender, System::EventArgs^ e)
{
  maxThresholdFormer_textBox->Text = maxThresholdFormer_trackBar->Value.ToString();
  maxThresholdFormer_textBox->Refresh();
}

System::Void RoiPainter4D::FormSegBronchi::maxThresholdLatter_Scroll(System::Object^ sender, System::EventArgs^ e)
{
  maxThresholdLatter_textBox->Text = maxThresholdLatter_trackBar->Value.ToString();
  maxThresholdLatter_textBox->Refresh();
}

System::Void RoiPainter4D::FormSegBronchi::isRendTreeStructure_CheckedChange(System::Object^ sender, System::EventArgs^ e)
{
  formMain_RedrawMainPanel();
}

System::Void RoiPainter4D::FormSegBronchi::isRendPath_CheckedChange(System::Object^ sender, System::EventArgs^ e)
{
  formMain_RedrawMainPanel();
}

System::Void RoiPainter4D::FormSegBronchi::MoveCpMode_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
  if (moveCpButton->Checked)
  {
    ModeSegBronchi::GetInst()->ClearSelectedCp();
  }
  formMain_RedrawMainPanel();
}
