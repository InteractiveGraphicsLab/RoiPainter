#pragma managed
#include "FormVisParam.h"
#include "FormMain.h"

#pragma unmanaged
#include "ImageCore.h"
#include "OglImage.h"

#pragma managed

using namespace RoiPainter3D;

FormVisParam::FormVisParam(void)
{
  InitializeComponent();

  this->ControlBox  = false;
  this->MaximizeBox = false;
  m_isMouseOn = false;

  isGray->Checked = true;

  auto* img_tf  = new OglImage1D<CH_RGBA>();
  img_tf->Allocate(TRANS_FUNC_SIZE);
	for (int i = 0; i < TRANS_FUNC_SIZE; ++i) (*img_tf)[4 * i] = (*img_tf)[4 * i + 1] = i;

  //load transfer function image 
  auto* img_psu = new OglImage1D<CH_RGBA>();
  img_psu->Allocate("shader/psued_bar.bmp");
  if (img_psu->GetResolution() == 0 )
  {
    img_psu->AllocateHeuImg(TRANS_FUNC_SIZE);
  }
  m_imgTf = img_tf;
  m_imgPsu = img_psu;
}


void FormVisParam::bindTfImg()
{
  ((OglImage1D<CH_RGBA>*)m_imgTf)->BindOgl(false);
}


void FormVisParam::bindPsuImg() 
{ 
  ((OglImage1D<CH_RGBA>*)m_imgPsu)->BindOgl(true);
}



void FormVisParam::initAllItemsForNewImg()
{
  EVec3i reso      = ImageCore::GetInst()->GetResolution();
  EVec3f pitch     = ImageCore::GetInst()->GetPitch();
  EVec2i volMinMax = ImageCore::GetInst()->GetVolMinMax();

  sizeX->Text = reso.x().ToString();
  sizeY->Text = reso.y().ToString();
  sizeZ->Text = reso.z().ToString();
  pitchX->Text = pitch.x().ToString();
  pitchY->Text = pitch.y().ToString();
  pitchZ->Text = pitch.z().ToString();

  winLevelMinTextBox->Text = volMinMax[0].ToString();
  winLevelMaxTextBox->Text = volMinMax[1].ToString();

  winLevelMinBar->SetRange(volMinMax[0], volMinMax[1]);
  winLevelMaxBar->SetRange(volMinMax[0], volMinMax[1]);
  winLevelMinBar->Value = volMinMax[0];
  winLevelMaxBar->Value = volMinMax[1];

  transBar->SetRange(0, 100);
  transBar->Value = 20;
  sliceBar->SetRange(128, 512);
  sliceBar->Value = 256;

  updateHistogramBmp();
  redrawTransFuncPictBox();
}



void FormVisParam::PitchUpdated()
{
  EVec3f pitch = ImageCore::GetInst()->GetPitch();
  pitchX->Text = pitch.x().ToString();
  pitchY->Text = pitch.y().ToString();
  pitchZ->Text = pitch.z().ToString();

}


void FormVisParam::updateHistogramBmp()
{
  const OglImage3D &vol   = ImageCore::GetInst()->m_vol    ;
  const OglImage3D &volGM = ImageCore::GetInst()->m_vol_gm;
  const int N = vol.GetW() * vol.GetH()* vol.GetD();

  //1. compute normalized histogram
  float histVol[HIST_BIN_SIZE];
  float histGM [HIST_BIN_SIZE];
  memset(histVol, 0, sizeof(float) * HIST_BIN_SIZE);
  memset(histGM , 0, sizeof(float) * HIST_BIN_SIZE);

  for (int i = 0; i < N; ++i)  histVol[  vol[i]] += 1;
  for (int i = 0; i < N; ++i)  histGM [volGM[i]] += 1;

  float maxV = 0, maxG = 0;
  for (int i = 5; i < HIST_BIN_SIZE - 5; ++i) {
    maxV = std::max(maxV, histVol[i]);
    maxG = std::max(maxG, histGM [i]);
  }
  for (int i = 0; i < HIST_BIN_SIZE; ++i) {
    histVol[i] /= maxV;
    histGM [i] /= maxG;
  }

  //2. gen histgram bitmap
  const int W = pictBox1->Width;
  const int H = pictBox1->Height;
  m_HistBmp   = gcnew Bitmap(W, H);
  m_HistGMBmp = gcnew Bitmap(W, H);

  for (int x = 0; x < W; ++x)
  {
    int xPos = (int)(x / (double)W * (HIST_BIN_SIZE - 1));

    for (int y = 0; y < H; ++y)
    {
      m_HistBmp  ->SetPixel(x, y, ((H - y) > histVol[xPos] * H) ? Color::Gray : Color::SkyBlue);
      m_HistGMBmp->SetPixel(x, y, ((H - y) > histGM [xPos] * H) ? Color::Gray : Color::SkyBlue);
    }
  }
}


void FormVisParam::redrawTransFuncPictBox()
{
  Pen ^blackPen = gcnew Pen(Color::Black, 2);
  Pen ^bluePen  = gcnew Pen(Color::Blue, 3);

  const int W = pictBox1->Width;
  const int H = pictBox1->Height;

  Bitmap ^bmp1 = gcnew Bitmap(W, H);
  Bitmap ^bmp2 = gcnew Bitmap(W, H);
  pictBox1->Image = bmp1;
  pictBox2->Image = bmp2;
  Graphics^ graphics1 = Graphics::FromImage(pictBox1->Image);
  Graphics^ graphics2 = Graphics::FromImage(pictBox2->Image);

  //draw histogram
  if (m_HistBmp   != nullptr) graphics1->DrawImage(m_HistBmp  , 0, 0, W, H);
  if (m_HistGMBmp != nullptr) graphics2->DrawImage(m_HistGMBmp, 0, 0, W, H);

  //draw frame
  graphics1->DrawLine(blackPen, 0, 0, W - 1, 0);
  graphics1->DrawLine(blackPen, W - 1, 0, W - 1, H - 1);
  graphics1->DrawLine(blackPen, W - 1, H - 1, 0, H - 1);
  graphics1->DrawLine(blackPen, 0, H - 1, 0, 0);

  auto* img_tf  = (OGLImage1D4*)m_imgTf;

  //draw tranfer function
  for (int i = 1; i < TRANS_FUNC_SIZE; ++i)
  {
    int x1 = (int)((i-1) / (double)TRANS_FUNC_SIZE * W);
    int x2 = (int)(( i ) / (double)TRANS_FUNC_SIZE * W);
    int y1 = (int)((*img_tf)[4 * (i-1)] / 255.0 * H);
    int y2 = (int)((*img_tf)[4 * ( i )] / 255.0 * H);
    graphics1->DrawLine(bluePen, x1, H - y1, x2, H - y2);

    y1 = (int)((*img_tf)[4 * (i-1)+1] / 255.0 * H);
    y2 = (int)((*img_tf)[4 * ( i )+1] / 255.0 * H);
    graphics2->DrawLine(bluePen, x1, H - y1, x2, H - y2);
  }
  pictBox1->Refresh();
  pictBox2->Refresh();
}







void FormVisParam::pitchTextBoxChanged()
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
  RedrawScene();
}
System::Void FormVisParam::pitchX_TextChanged(System::Object^  sender, System::EventArgs^  e) {
  pitchTextBoxChanged();
}
System::Void FormVisParam::pitchY_TextChanged(System::Object^  sender, System::EventArgs^  e) {
  pitchTextBoxChanged();
}
System::Void FormVisParam::pitchZ_TextChanged(System::Object^  sender, System::EventArgs^  e) {
  pitchTextBoxChanged();
}


System::Void FormVisParam::winLevelMinBar_Scroll(System::Object^  sender, System::EventArgs^  e) 
{
  winLevelMinTextBox->Text = winLevelMinBar->Value.ToString();
  winLevelMinTextBox->Refresh();
}

System::Void FormVisParam::winLevelMaxBar_Scroll(System::Object^  sender, System::EventArgs^  e) 
{
  winLevelMaxTextBox->Text = winLevelMaxBar->Value.ToString();
  winLevelMaxTextBox->Refresh();
}

System::Void FormVisParam::winLevelMinBar_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
  winLevelMinTextBox->Text = winLevelMinBar->Value.ToString();
  winLevelMinTextBox->Refresh();
  ImageCore::GetInst()->UpdateOGLVolume(winLevelMinBar->Value, winLevelMaxBar->Value);

  updateHistogramBmp();
  redrawTransFuncPictBox();
  RedrawScene();
}

System::Void FormVisParam::winLevelMaxBar_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
  winLevelMaxTextBox->Text = winLevelMaxBar->Value.ToString();
  winLevelMaxTextBox->Refresh();

  ImageCore::GetInst()->UpdateOGLVolume(winLevelMinBar->Value, winLevelMaxBar->Value);

  updateHistogramBmp();
  redrawTransFuncPictBox();
  RedrawScene();
}



System::Void FormVisParam::isRendFrame_CheckedChanged    (System::Object^  sender, System::EventArgs^  e){ RedrawScene();}
System::Void FormVisParam::isRendVolume_CheckedChanged   (System::Object^  sender, System::EventArgs^  e){ RedrawScene();}
System::Void FormVisParam::isRendPseudo_CheckedChanged   (System::Object^  sender, System::EventArgs^  e){ RedrawScene();}
System::Void FormVisParam::isRendIndi_CheckedChanged     (System::Object^  sender, System::EventArgs^  e){ RedrawScene();}
System::Void FormVisParam::isRendPlaneXY_CheckedChanged  (System::Object^  sender, System::EventArgs^  e){ RedrawScene();}
System::Void FormVisParam::isRendPlaneYZ_CheckedChanged  (System::Object^  sender, System::EventArgs^  e){ RedrawScene();}
System::Void FormVisParam::isRendPlaneZX_CheckedChanged  (System::Object^  sender, System::EventArgs^  e){ RedrawScene();}
System::Void FormVisParam::isRendGradMag_CheckedChanged  (System::Object^  sender, System::EventArgs^  e){ RedrawScene();}
System::Void FormVisParam::doInterpolation_CheckedChanged(System::Object^  sender, System::EventArgs^  e){ RedrawScene();}
System::Void FormVisParam::isRendOtherRoi_CheckedChanged (System::Object^  sender, System::EventArgs^  e){ RedrawScene();}
System::Void FormVisParam::transBar_Scroll(System::Object^  sender, System::EventArgs^  e) { RedrawScene(); }
System::Void FormVisParam::sliceBar_Scroll(System::Object^  sender, System::EventArgs^  e) { RedrawScene(); }

System::Void FormVisParam::isWhite_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
  FormMain_SetBkColor(1,1,1);
  RedrawScene();
}
System::Void FormVisParam::isBlack_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
  FormMain_SetBkColor(  0,  0,  0);
  RedrawScene();
}
System::Void FormVisParam::isGray_CheckedChanged(System::Object^  sender, System::EventArgs^  e)  {
  FormMain_SetBkColor(0.5f,0.5f,0.5f);
  RedrawScene();
}



// mouse downの瞬間から，downが起きたpanelのみに対してmoveイベントが飛ぶので，
// 対象panelを明示する必要はない

System::Void FormVisParam::pictBox1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
  m_isMouseOn = true;
  m_mouseX = e->Location.X;
  m_mouseY = e->Location.Y;
}

System::Void FormVisParam::pictBox2_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
  m_isMouseOn = true;
  m_mouseX = e->Location.X;
  m_mouseY = e->Location.Y;
}


System::Void FormVisParam::pictBox1_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
  m_isMouseOn = false;
}
System::Void FormVisParam::pictBox2_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
  m_isMouseOn = false;
}


System::Void FormVisParam::pictBox1_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
 if (!m_isMouseOn) return;

  const int W = pictBox1->Width;
  const int H = pictBox1->Height;

  int x1 = t_crop(0, TRANS_FUNC_SIZE, (int)(m_mouseX / (double)W * TRANS_FUNC_SIZE));
  int x2 = t_crop(0, TRANS_FUNC_SIZE, (int)(e->Location.X / (double)W * TRANS_FUNC_SIZE));
  int y1 = t_crop(0, 255            , (int)((H - m_mouseY) / (double)H * 255));
  int y2 = t_crop(0, 255            , (int)((H - e->Location.Y) / (double)H * 255));

  if (x1 > x2) 
  {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }
  auto* img_tf = (OGLImage1D4*)m_imgTf;

  for (int i = x1; i < x2; ++i) 
  {
    double y = (double)(i - x1) / (double)(x2 - x1) * (y2 - y1) + y1;
    (*img_tf)[4 * i] = (int)y;
  }

  m_mouseX = e->Location.X;
  m_mouseY = e->Location.Y;

  img_tf->SetUpdated();
  redrawTransFuncPictBox();
  FormMain::getInst()->redrawMainPanel();
}


System::Void FormVisParam::pictBox2_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
 if (!m_isMouseOn) return;

  const int W = pictBox2->Width;
  const int H = pictBox2->Height;

  int x1 = t_crop(0, TRANS_FUNC_SIZE, (int)(m_mouseX / (double)W * TRANS_FUNC_SIZE));
  int x2 = t_crop(0, TRANS_FUNC_SIZE, (int)(e->Location.X / (double)W * TRANS_FUNC_SIZE));
  int y1 = t_crop(0, 255            , (int)((H - m_mouseY) / (double)H * 255));
  int y2 = t_crop(0, 255            , (int)((H - e->Location.Y) / (double)H * 255));

  if (x1 > x2) 
  {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }
  
  auto* img_tf = (OGLImage1D4*)m_imgTf;

  for (int i = x1; i < x2; ++i) {
    double y = (double)(i - x1) / (double)(x2 - x1) * (y2 - y1) + y1;
    (*img_tf)[4 * i+1] = (int)y;
  }

  m_mouseX = e->Location.X;
  m_mouseY = e->Location.Y;

  img_tf->SetUpdated();
  redrawTransFuncPictBox();
  FormMain::getInst()->redrawMainPanel();
}

