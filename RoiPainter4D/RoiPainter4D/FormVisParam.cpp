#pragma managed
#include "FormVisParam.h"
#include "FormMain.h"
#include "FormSegLocalRGrow.h"
#include "FormSegSwallowOrgans.h"
#include "FormSegSwallowOrganTimeline.h"
#include "FormSegStrokeFfd.h"

#pragma unmanaged
#include <iostream>
#include <vector>
#include <string>
#include "ImageCore.h"
#include "ModeCore.h"
#pragma managed

using namespace RoiPainter4D;

#define TRANS_FUNC_SIZE 256

FormVisParam::FormVisParam(void)
{
  InitializeComponent();
  this->ControlBox  = false;
  this->MaximizeBox = false;
  m_isMouseOn = false;

  //transfer functions
  OglImage1D<CH_RGBA>* trans_func = new OglImage1D<CH_RGBA>();
  OglImage1D<CH_RGBA>* psuedo_color = new OglImage1D<CH_RGBA>();
  m_imgTf = trans_func;
  m_imgPsu = psuedo_color;
  
  trans_func->Allocate(TRANS_FUNC_SIZE);

  psuedo_color->Allocate("./shader/psued_bar.bmp");
  if (psuedo_color->GetResolution() == 0) 
    psuedo_color->AllocateHeuImg(256);
 
  for (int i = 0; i < TRANS_FUNC_SIZE; ++i) 
    (*trans_func)[4 * i] = (*trans_func)[4 * i + 1] = i;
}


void FormVisParam::BindImgTf()
{
  if (m_imgTf == nullptr) return;
  ((OglImage1D<CH_RGBA>*)m_imgTf)->BindOgl(false);
}

void FormVisParam::BindImgPsu()
{
  if (m_imgPsu == nullptr) return;
  ((OglImage1D<CH_RGBA>*)m_imgPsu)->BindOgl();
}

FormVisParam::~FormVisParam()
{
  if (components)
  {
    delete components;
  }
  delete[] m_imgPsu;
  delete[] m_imgTf;
}



System::Void FormVisParam::isRendFrame_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  FormMain::GetInst()->RedrawMainPanel();
}

System::Void FormVisParam::isRendVolume_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  FormMain::GetInst()->RedrawMainPanel();
}

System::Void FormVisParam::isRendPseudo_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  FormMain::GetInst()->RedrawMainPanel();
}

System::Void FormVisParam::isRendIndi_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  FormMain::GetInst()->RedrawMainPanel();
}

System::Void FormVisParam::isRendPlaneXY_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  FormMain::GetInst()->RedrawMainPanel();
}

System::Void FormVisParam::isRendPlaneYZ_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  FormMain::GetInst()->RedrawMainPanel();
}

System::Void FormVisParam::isRendPlaneZX_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  FormMain::GetInst()->RedrawMainPanel();
}


System::Void FormVisParam::isGray_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  FormMain::GetInst()->SetBackColor(0.3f, 0.3f, 0.3f, 0.5f);
  FormMain::GetInst()->RedrawMainPanel();
}

System::Void FormVisParam::isBlack_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  FormMain::GetInst()->SetBackColor(0.f, 0.f, 0.f, 0.5f);
  FormMain::GetInst()->RedrawMainPanel();
}

System::Void FormVisParam::isWhite_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  FormMain::GetInst()->SetBackColor(1.0f, 1.0f, 1.0f, 0.5f);
  FormMain::GetInst()->RedrawMainPanel();
}

System::Void FormVisParam::transBar_Scroll(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  FormMain::GetInst()->RedrawMainPanel();
}

System::Void FormVisParam::sliceBar_Scroll(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  FormMain::GetInst()->RedrawMainPanel();
}



void FormVisParam::UpdateHistogramBmp()
{
  std::vector<float> hist = ImageCore::GetInst()->Calc8bitHistogram_visvol();
  int histbin_size = (int) hist.size();

  //gen histgram bitmap
  const int W = pictBox->Width;
  const int H = pictBox->Height;
  m_HistgramBmp = gcnew Bitmap(W, H);

  for (int x = 0; x < W; ++x)
  {
    int xPos = (int)(x / (double)W * (histbin_size - 1));
    int histVal = (int)(hist[xPos] * H);

    for (int y = 0; y < H; ++y)
    {
      if ((H - y) / (double)H > hist[xPos]) 
        m_HistgramBmp->SetPixel(x, y, Color::Gray);
      else
        m_HistgramBmp->SetPixel(x, y, Color::SkyBlue);
    }
  }
}


void FormVisParam::RedrawTransFuncPictBox()
{
  const int W = pictBox->Width;
  const int H = pictBox->Height;

  Bitmap ^bmp = gcnew Bitmap(W, H);
  pictBox->Image = bmp;
  Graphics^ graphics = Graphics::FromImage(pictBox->Image);

  //draw histogram
  if (m_HistgramBmp != nullptr) graphics->DrawImage(m_HistgramBmp, 0, 0, W, H);

  //draw frame
  Pen ^blackPen = gcnew Pen(Color::Black, 2);
  Pen ^bluePen = gcnew Pen(Color::Blue, 3);
  graphics->DrawLine(blackPen, 0, 0, W - 1, 0);
  graphics->DrawLine(blackPen, W - 1, 0, W - 1, H - 1);
  graphics->DrawLine(blackPen, W - 1, H - 1, 0, H - 1);
  graphics->DrawLine(blackPen, 0, H - 1, 0, 0);


  //draw tranfer function
  OglImage1D<CH_RGBA>* trans_func = (OglImage1D<CH_RGBA>*)m_imgTf;
  for (int i = 1; i < TRANS_FUNC_SIZE; ++i)
  {
    int x1 = (int)((i - 1) / (double)TRANS_FUNC_SIZE * W);
    int x2 = (int)(i / (double)TRANS_FUNC_SIZE * W);
    int y1 = (int)((*trans_func)[4 * (i - 1)] / 255.0 * H);
    int y2 = (int)((*trans_func)[4 * (i)] / 255.0 * H);
    graphics->DrawLine(bluePen, x1, H - y1, x2, H - y2);
  }

  pictBox->Refresh();
}


System::Void FormVisParam::pictBox_MouseDown(
    System::Object^  sender, 
    System::Windows::Forms::MouseEventArgs^  e)
{
  m_isMouseOn = true;
  m_mouseX = e->Location.X;
  m_mouseY = e->Location.Y;
}

System::Void FormVisParam::pictBox_MouseMove(
    System::Object^  sender, 
    System::Windows::Forms::MouseEventArgs^  e)
{
  if (!m_isMouseOn) return;

  const int W = pictBox->Width;
  const int H = pictBox->Height;
  int x1 = Crop(0, TRANS_FUNC_SIZE, (int)(     m_mouseX / (double)W * TRANS_FUNC_SIZE));
  int x2 = Crop(0, TRANS_FUNC_SIZE, (int)(e->Location.X / (double)W * TRANS_FUNC_SIZE));
  int y1 = Crop(0, 255, (int)((H - m_mouseY) / (double)H * 255));
  int y2 = Crop(0, 255, (int)((H - e->Location.Y) / (double)H * 255));

  if (x1 > x2) {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }

  OglImage1D<CH_RGBA>* trans_func = (OglImage1D<CH_RGBA>*)m_imgTf;

  for (int i = x1; i < x2; ++i) {

    double y = (double)(i - x1) / (double)(x2 - x1) * (y2 - y1) + y1;

    (*trans_func)[4 * i] = (int)y;
  }

  m_mouseX = e->Location.X;
  m_mouseY = e->Location.Y;

  trans_func->SetUpdated();
  RedrawTransFuncPictBox();
  FormMain::GetInst()->RedrawMainPanel();
}


System::Void FormVisParam::pictBox_MouseUp(
    System::Object^  sender, 
    System::Windows::Forms::MouseEventArgs^  e)
{
  std::cout << "mouse_up " << e->Location.X << " " << e->Location.Y << "\n";
  m_mouseX, m_mouseY;
  m_isMouseOn = false;
}


void FormVisParam::ChangeFrameI(int ofst)
{
  if(ofst!=0)
  {
    int v = timeSlider->Value + ofst;
    if (v > timeSlider->Maximum) v = timeSlider->Maximum;
    if (v < timeSlider->Minimum) v = timeSlider->Minimum;
    timeSlider->Value = v;
    timeLabel->Text = timeSlider->Value.ToString();
  }
  //TODO 本来は ModeCore経由で通知したい(大幅なrefactoringが必用)
  if ( ModeCore::GetInst()->getCurrentMode() == MODE_SEG_LCLRGROW )
    FormSegLocalRGrow_FrameIdxChanged();

  else if ( ModeCore::GetInst()->getCurrentMode() == MODE_SEG_SWALLOW)
  {
    FormSegSwallowOrgans_FitCrssecToSelectedCageVtx();
    FormSegSwallowTimeline_RedrawPanel();
  }

  ImageCore::GetInst()->UpdateVisVolume(winLevelMinBar->Value, winLevelMaxBar->Value, timeSlider->Value);
}


System::Void FormVisParam::scrollWinLevelMin(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  winLevelMinTextBox->Text = winLevelMinBar->Value.ToString();
  winLevelMinTextBox->Refresh();
}

System::Void FormVisParam::scrollWinLevelMax(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  winLevelMaxTextBox->Text = winLevelMaxBar->Value.ToString();
  winLevelMaxTextBox->Refresh();
}

System::Void FormVisParam::winLevelMinBar_MouseUp(
    System::Object^  sender, 
    System::Windows::Forms::MouseEventArgs^  e)
{
  winLevelMinTextBox->Text = winLevelMinBar->Value.ToString();
  winLevelMinTextBox->Refresh();

  ImageCore::GetInst()->UpdateVisVolume(winLevelMinBar->Value, winLevelMaxBar->Value, timeSlider->Value);
  UpdateHistogramBmp();
  RedrawTransFuncPictBox();
  FormMain::GetInst()->RedrawMainPanel();
}

System::Void FormVisParam::winLevelMaxBar_MouseUp(
    System::Object^  sender, 
    System::Windows::Forms::MouseEventArgs^  e)
{
  winLevelMaxTextBox->Text = winLevelMaxBar->Value.ToString();
  winLevelMaxTextBox->Refresh();

  ImageCore::GetInst()->UpdateVisVolume(winLevelMinBar->Value, winLevelMaxBar->Value, timeSlider->Value);
  UpdateHistogramBmp();
  RedrawTransFuncPictBox();
  FormMain::GetInst()->RedrawMainPanel();
}


System::Void FormVisParam::scrollTime(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  timeLabel->Text = timeSlider->Value.ToString();
  FormVisParam::ChangeFrameI(0);
  FormSegStrokeFfd::GetInst()->FrameChanged();
  FormMain::GetInst()->RedrawMainPanel();
}

// TODO !! 変更したpitchを反映させなければ...
System::Void FormVisParam::pitch_KeyPress(
    System::Object^ sender, 
    System::Windows::Forms::KeyPressEventArgs^  e)
{
}


void FormVisParam::InitAllItemsForNewImg()
{
  int    frameN = ImageCore::GetInst()->GetNumFrames();
  EVec3i reso   = ImageCore::GetInst()->GetReso();
  EVec3f pitch  = ImageCore::GetInst()->GetPitch();
  EVec2i winLv  = ImageCore::GetInst()->GetVolumeMinMax();
  winLv[0] = std::max(-5000, winLv[0]);
  winLv[1] = std::min( 5000, winLv[1]);


  std::cout << "New4DCTLoaded frameN=" << frameN << "\n";

  sizeX->Text = reso.x().ToString();
  sizeY->Text = reso.y().ToString();
  sizeZ->Text = reso.z().ToString();
  pitchX->Text = pitch.x().ToString();
  pitchY->Text = pitch.y().ToString();
  pitchZ->Text = pitch.z().ToString();

  winLevelMinTextBox->Text = winLv[0].ToString();
  winLevelMaxTextBox->Text = winLv[1].ToString();

  winLevelMinBar->SetRange(winLv[0], winLv[1]);
  winLevelMaxBar->SetRange(winLv[0], winLv[1]);
  winLevelMinBar->Value = winLv[0];
  winLevelMaxBar->Value = winLv[1];

  timeSlider->Value = 0;
  timeSlider->SetRange(0, frameN - 1);

  transBar->SetRange(0, 100);
  transBar->Value = 20;
  sliceBar->SetRange(128, 512);
  sliceBar->Value = 256;

  ImageCore::GetInst()->UpdateVisVolume(winLevelMinBar->Value, winLevelMaxBar->Value, timeSlider->Value);
  UpdateHistogramBmp();

  RedrawTransFuncPictBox();
}


void FormVisParam::PitchTextBoxChanged()
{
  EVec3f pitch = ImageCore::GetInst()->GetPitch();
 
  float px, py, pz;
  if( float::TryParse( pitchX->Text, px ) ) pitch[0] = px;
  if( float::TryParse( pitchY->Text, py ) ) pitch[1] = py;
  if( float::TryParse( pitchZ->Text, pz ) ) pitch[2] = pz;
  if(pitch[0] <= 0 ) pitch[0] = .0001f;
  if(pitch[1] <= 0 ) pitch[1] = .0001f;
  if(pitch[2] <= 0 ) pitch[2] = .0001f;
  pitchX->Text = pitch[0].ToString("F5");
  pitchY->Text = pitch[1].ToString("F5");
  pitchZ->Text = pitch[2].ToString("F5");
  ImageCore::GetInst()->SetPitch( pitch );
  formMain_RedrawMainPanel();
}


System::Void FormVisParam::pitchX_TextChanged(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  PitchTextBoxChanged();
}

System::Void FormVisParam::pitchY_TextChanged(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  PitchTextBoxChanged();
}

System::Void FormVisParam::pitchZ_TextChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  PitchTextBoxChanged();
}