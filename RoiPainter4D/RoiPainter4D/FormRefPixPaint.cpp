#pragma managed
#include "FormRefPixPaint.h"
#include "FormSelectMskId.h"

#pragma unmanaged
#include "./Mode/ModeRefPixPaint.h"
#include "ImageCore.h"
#pragma managed

using namespace RoiPainter4D;

System::Void FormRefPixPaint::btn_ok_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeRefPixPaint::GetInst()->finishTrim();
}

System::Void FormRefPixPaint::btn_cancel_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeRefPixPaint::GetInst()->cancelBtnClick();
}

System::Void FormRefPixPaint::InitAllItems(int volmin, int volmax)
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


  FormSelectMskId^ modal = gcnew FormSelectMskId();
  if (modal->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  int trgtId = modal->getTrgtID();
  modal->Close();

  ImageCore::GetInst()->SetSelectMaskId(trgtId);
}

// minV, maxVの値は可能であれば手打ちと、track barを併用してほしい
System::Void FormRefPixPaint::minThreshold_Scroll(System::Object^ sender, System::EventArgs^ e) {
    if (m_item_updating) return;
    m_item_updating = true;

    minThreshTextBox->Text = minThresholdBar->Value.ToString();
    minThreshTextBox->Refresh();

    m_item_updating = false;
}

System::Void FormRefPixPaint::maxThreshold_Scroll(System::Object^ sender, System::EventArgs^ e) {
    if (m_item_updating) return;
    m_item_updating = true;

    maxThreshTextBox->Text = maxThresholdBar->Value.ToString();
    maxThreshTextBox->Refresh();

    m_item_updating = false;
}



System::Void FormRefPixPaint::minThreshTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
    if (m_item_updating) return;
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
    else if (minThreshTextBox->Text == "-")
    {
        x = 0;
    }
    else
    {
        minThreshTextBox->Text = "0";
        minThresholdBar->Value = 0;
    }

    m_item_updating = false;

}



System::Void FormRefPixPaint::maxThreshTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
    if (m_item_updating) return;
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
    else if (maxThreshTextBox->Text == "-")
    {
        x = 0;
    }
    else
    {
        maxThreshTextBox->Text = "0";
        maxThresholdBar->Value = 0;
    }

    m_item_updating = false;
}