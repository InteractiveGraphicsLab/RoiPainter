#pragma once

//enum OGL_IMAGE_CH : int;
//template<OGL_IMAGE_CH T> class OglImage1D;

namespace RoiPainter4D {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  /// <summary>
  /// FormVisParam の概要
  /// </summary>
  public ref class FormVisParam : public System::Windows::Forms::Form
  {
  private: 
    System::Windows::Forms::Label^		planeLabel;
    System::Windows::Forms::Label^		sizePanel;
    System::Windows::Forms::Label^		pitchLabel;
    System::Windows::Forms::Label^		bgColorLabel;
    System::Windows::Forms::Label^		transLabel;
    System::Windows::Forms::Label^		sliceLabel;
    System::Windows::Forms::Label^		winLevelLabel;
    System::Windows::Forms::Label^		winLevelMinLabel;
    System::Windows::Forms::Label^		winLevelMaxLabel;
    System::Windows::Forms::Label^    timeLabel;
    System::Windows::Forms::TextBox^	sizeX;
    System::Windows::Forms::TextBox^	sizeY;
    System::Windows::Forms::TextBox^	sizeZ;
    System::Windows::Forms::TextBox^	pitchX;
    System::Windows::Forms::TextBox^	pitchY;
    System::Windows::Forms::TextBox^	pitchZ;
    System::Windows::Forms::TextBox^	winLevelMinTextBox;
    System::Windows::Forms::TextBox^	winLevelMaxTextBox;
    System::Windows::Forms::CheckBox^ isRendFrame;
    System::Windows::Forms::CheckBox^ isRendVolume;
    System::Windows::Forms::CheckBox^ isRendPseudo;
    System::Windows::Forms::CheckBox^ isRendIndi;
    System::Windows::Forms::CheckBox^ isRendPlaneXY;
    System::Windows::Forms::CheckBox^ isRendPlaneYZ;
    System::Windows::Forms::CheckBox^ isRendPlaneZX;
    System::Windows::Forms::TrackBar^ timeSlider;
    System::Windows::Forms::TrackBar^ transBar;
    System::Windows::Forms::TrackBar^ sliceBar;
    System::Windows::Forms::TrackBar^ winLevelMinBar;
    System::Windows::Forms::TrackBar^ winLevelMaxBar;
    System::Windows::Forms::PictureBox^  pictBox;
    System::Windows::Forms::RadioButton^ isWhite;
    System::Windows::Forms::RadioButton^ isBlack;
    System::Windows::Forms::RadioButton^ isGray;
    System::Windows::Forms::Label^ label1;


  private:
    Bitmap ^m_HistgramBmp;
    bool    m_isMouseOn;
    int     m_mouseX, m_mouseY;
    void*   m_imgTf;//OglImage1D<OGL_IMAGE_CH::CH_RGBA = 4>
    void*   m_imgPsu;//OglImage1D<OGL_IMAGE_CH::CH_RGBA = 4>

    static FormVisParam^ m_singleton;
  public:
    static FormVisParam^ GetInst() {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormVisParam();
      }
      return m_singleton;
    }

  protected:
    ~FormVisParam();

  private:
    FormVisParam(void);
    void RedrawTransFuncPictBox();
    void UpdateHistogramBmp();
    void PitchTextBoxChanged();

  public:
    void InitAllItemsForNewImg();
    void BindImgTf(); 
    void BindImgPsu(); 
    inline int   GetframeI  () { return timeSlider->Value; }
    inline int   GetWinLvMax() { return winLevelMaxBar->Value; }
    inline int   GetWinLvMin() { return winLevelMinBar->Value; }
    inline void  ChangeFrameI(int ofst);
    inline float GetAlpha()    { return transBar->Value * 0.01f; }
    inline int   GetSliceNum() { return sliceBar->Value; }
    bool  bRendFrame(){ return isRendFrame->Checked; }
    bool  bRendVol()  { return isRendVolume->Checked; }
    bool  bDoPsued()  { return isRendPseudo->Checked; }
    bool  bRendIndi() { return isRendIndi->Checked; }
    bool  bGradMag()  { return false; }
    bool  bOnManip()  { return m_isMouseOn; }
    bool  bPlaneXY()  { return isRendPlaneXY->Checked; }
    bool  bPlaneYZ()  { return isRendPlaneYZ->Checked; }
    bool  bPlaneZX()  { return isRendPlaneZX->Checked; }

  private:
    /// <summary>
    /// 必要なデザイナー変数です。
    /// </summary>
    System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
    /// <summary>
    /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
    /// コード エディターで変更しないでください。
    /// </summary>
    void InitializeComponent(void)
    {
      this->isRendFrame = (gcnew System::Windows::Forms::CheckBox());
      this->isRendVolume = (gcnew System::Windows::Forms::CheckBox());
      this->isRendPseudo = (gcnew System::Windows::Forms::CheckBox());
      this->isRendPlaneXY = (gcnew System::Windows::Forms::CheckBox());
      this->isRendPlaneYZ = (gcnew System::Windows::Forms::CheckBox());
      this->isRendPlaneZX = (gcnew System::Windows::Forms::CheckBox());
      this->timeLabel = (gcnew System::Windows::Forms::Label());
      this->timeSlider = (gcnew System::Windows::Forms::TrackBar());
      this->planeLabel = (gcnew System::Windows::Forms::Label());
      this->sizePanel = (gcnew System::Windows::Forms::Label());
      this->pitchLabel = (gcnew System::Windows::Forms::Label());
      this->sizeX = (gcnew System::Windows::Forms::TextBox());
      this->sizeY = (gcnew System::Windows::Forms::TextBox());
      this->sizeZ = (gcnew System::Windows::Forms::TextBox());
      this->pitchZ = (gcnew System::Windows::Forms::TextBox());
      this->pitchY = (gcnew System::Windows::Forms::TextBox());
      this->pitchX = (gcnew System::Windows::Forms::TextBox());
      this->bgColorLabel = (gcnew System::Windows::Forms::Label());
      this->winLevelLabel = (gcnew System::Windows::Forms::Label());
      this->winLevelMinLabel = (gcnew System::Windows::Forms::Label());
      this->winLevelMaxLabel = (gcnew System::Windows::Forms::Label());
      this->winLevelMinTextBox = (gcnew System::Windows::Forms::TextBox());
      this->winLevelMaxTextBox = (gcnew System::Windows::Forms::TextBox());
      this->transLabel = (gcnew System::Windows::Forms::Label());
      this->sliceLabel = (gcnew System::Windows::Forms::Label());
      this->transBar = (gcnew System::Windows::Forms::TrackBar());
      this->sliceBar = (gcnew System::Windows::Forms::TrackBar());
      this->winLevelMinBar = (gcnew System::Windows::Forms::TrackBar());
      this->winLevelMaxBar = (gcnew System::Windows::Forms::TrackBar());
      this->pictBox = (gcnew System::Windows::Forms::PictureBox());
      this->isRendIndi = (gcnew System::Windows::Forms::CheckBox());
      this->isWhite = (gcnew System::Windows::Forms::RadioButton());
      this->isBlack = (gcnew System::Windows::Forms::RadioButton());
      this->isGray = (gcnew System::Windows::Forms::RadioButton());
      this->label1 = (gcnew System::Windows::Forms::Label());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->timeSlider))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->transBar))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->sliceBar))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->winLevelMinBar))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->winLevelMaxBar))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictBox))->BeginInit();
      this->SuspendLayout();
      // 
      // isRendFrame
      // 
      this->isRendFrame->AutoSize = true;
      this->isRendFrame->Checked = true;
      this->isRendFrame->CheckState = System::Windows::Forms::CheckState::Checked;
      this->isRendFrame->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->isRendFrame->Location = System::Drawing::Point(2, 110);
      this->isRendFrame->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
      this->isRendFrame->Name = L"isRendFrame";
      this->isRendFrame->Size = System::Drawing::Size(59, 21);
      this->isRendFrame->TabIndex = 0;
      this->isRendFrame->Text = L"frame";
      this->isRendFrame->UseVisualStyleBackColor = true;
      this->isRendFrame->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendFrame_CheckedChanged);
      // 
      // isRendVolume
      // 
      this->isRendVolume->AutoSize = true;
      this->isRendVolume->Checked = true;
      this->isRendVolume->CheckState = System::Windows::Forms::CheckState::Checked;
      this->isRendVolume->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->isRendVolume->Location = System::Drawing::Point(65, 110);
      this->isRendVolume->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
      this->isRendVolume->Name = L"isRendVolume";
      this->isRendVolume->Size = System::Drawing::Size(67, 21);
      this->isRendVolume->TabIndex = 1;
      this->isRendVolume->Text = L"volume";
      this->isRendVolume->UseVisualStyleBackColor = true;
      this->isRendVolume->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendVolume_CheckedChanged);
      // 
      // isRendPseudo
      // 
      this->isRendPseudo->AutoSize = true;
      this->isRendPseudo->Checked = true;
      this->isRendPseudo->CheckState = System::Windows::Forms::CheckState::Checked;
      this->isRendPseudo->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->isRendPseudo->Location = System::Drawing::Point(138, 110);
      this->isRendPseudo->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
      this->isRendPseudo->Name = L"isRendPseudo";
      this->isRendPseudo->Size = System::Drawing::Size(66, 21);
      this->isRendPseudo->TabIndex = 2;
      this->isRendPseudo->Text = L"pseudo";
      this->isRendPseudo->UseVisualStyleBackColor = true;
      this->isRendPseudo->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendPseudo_CheckedChanged);
      // 
      // isRendPlaneXY
      // 
      this->isRendPlaneXY->AutoSize = true;
      this->isRendPlaneXY->Checked = true;
      this->isRendPlaneXY->CheckState = System::Windows::Forms::CheckState::Checked;
      this->isRendPlaneXY->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->isRendPlaneXY->Location = System::Drawing::Point(93, 136);
      this->isRendPlaneXY->Name = L"isRendPlaneXY";
      this->isRendPlaneXY->Size = System::Drawing::Size(39, 21);
      this->isRendPlaneXY->TabIndex = 3;
      this->isRendPlaneXY->Text = L"xy";
      this->isRendPlaneXY->UseVisualStyleBackColor = true;
      this->isRendPlaneXY->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendPlaneXY_CheckedChanged);
      // 
      // isRendPlaneYZ
      // 
      this->isRendPlaneYZ->AutoSize = true;
      this->isRendPlaneYZ->Checked = true;
      this->isRendPlaneYZ->CheckState = System::Windows::Forms::CheckState::Checked;
      this->isRendPlaneYZ->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->isRendPlaneYZ->Location = System::Drawing::Point(155, 136);
      this->isRendPlaneYZ->Name = L"isRendPlaneYZ";
      this->isRendPlaneYZ->Size = System::Drawing::Size(39, 21);
      this->isRendPlaneYZ->TabIndex = 4;
      this->isRendPlaneYZ->Text = L"yz";
      this->isRendPlaneYZ->UseVisualStyleBackColor = true;
      this->isRendPlaneYZ->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendPlaneYZ_CheckedChanged);
      // 
      // isRendPlaneZX
      // 
      this->isRendPlaneZX->AutoSize = true;
      this->isRendPlaneZX->Checked = true;
      this->isRendPlaneZX->CheckState = System::Windows::Forms::CheckState::Checked;
      this->isRendPlaneZX->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->isRendPlaneZX->Location = System::Drawing::Point(216, 136);
      this->isRendPlaneZX->Name = L"isRendPlaneZX";
      this->isRendPlaneZX->Size = System::Drawing::Size(39, 21);
      this->isRendPlaneZX->TabIndex = 5;
      this->isRendPlaneZX->Text = L"zx";
      this->isRendPlaneZX->UseVisualStyleBackColor = true;
      this->isRendPlaneZX->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendPlaneZX_CheckedChanged);
      // 
      // timeLabel
      // 
      this->timeLabel->AutoSize = true;
      this->timeLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->timeLabel->Location = System::Drawing::Point(64, 305);
      this->timeLabel->Name = L"timeLabel";
      this->timeLabel->Size = System::Drawing::Size(18, 20);
      this->timeLabel->TabIndex = 7;
      this->timeLabel->Text = L"0";
      // 
      // timeSlider
      // 
      this->timeSlider->Location = System::Drawing::Point(6, 322);
      this->timeSlider->Name = L"timeSlider";
      this->timeSlider->Size = System::Drawing::Size(287, 45);
      this->timeSlider->TabIndex = 8;
      this->timeSlider->TickStyle = System::Windows::Forms::TickStyle::None;
      this->timeSlider->Scroll += gcnew System::EventHandler(this, &FormVisParam::scrollTime);
      // 
      // planeLabel
      // 
      this->planeLabel->AutoSize = true;
      this->planeLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->planeLabel->Location = System::Drawing::Point(11, 140);
      this->planeLabel->Name = L"planeLabel";
      this->planeLabel->Size = System::Drawing::Size(37, 17);
      this->planeLabel->TabIndex = 9;
      this->planeLabel->Text = L"Plane";
      // 
      // sizePanel
      // 
      this->sizePanel->AutoSize = true;
      this->sizePanel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->sizePanel->Location = System::Drawing::Point(14, 6);
      this->sizePanel->Name = L"sizePanel";
      this->sizePanel->Size = System::Drawing::Size(65, 17);
      this->sizePanel->TabIndex = 10;
      this->sizePanel->Text = L"Size[pixel]";
      // 
      // pitchLabel
      // 
      this->pitchLabel->AutoSize = true;
      this->pitchLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->pitchLabel->Location = System::Drawing::Point(14, 30);
      this->pitchLabel->Name = L"pitchLabel";
      this->pitchLabel->Size = System::Drawing::Size(67, 17);
      this->pitchLabel->TabIndex = 11;
      this->pitchLabel->Text = L"Pitch[mm]";
      // 
      // sizeX
      // 
      this->sizeX->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->sizeX->Location = System::Drawing::Point(94, 3);
      this->sizeX->Name = L"sizeX";
      this->sizeX->ReadOnly = true;
      this->sizeX->Size = System::Drawing::Size(51, 23);
      this->sizeX->TabIndex = 12;
      // 
      // sizeY
      // 
      this->sizeY->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->sizeY->Location = System::Drawing::Point(151, 3);
      this->sizeY->Name = L"sizeY";
      this->sizeY->ReadOnly = true;
      this->sizeY->Size = System::Drawing::Size(51, 23);
      this->sizeY->TabIndex = 13;
      // 
      // sizeZ
      // 
      this->sizeZ->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->sizeZ->Location = System::Drawing::Point(208, 3);
      this->sizeZ->Name = L"sizeZ";
      this->sizeZ->ReadOnly = true;
      this->sizeZ->Size = System::Drawing::Size(51, 23);
      this->sizeZ->TabIndex = 14;
      // 
      // pitchZ
      // 
      this->pitchZ->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->pitchZ->Location = System::Drawing::Point(208, 27);
      this->pitchZ->Name = L"pitchZ";
      this->pitchZ->Size = System::Drawing::Size(51, 23);
      this->pitchZ->TabIndex = 17;
      this->pitchZ->TextChanged += gcnew System::EventHandler(this, &FormVisParam::pitchZ_TextChanged);
      this->pitchZ->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &FormVisParam::pitch_KeyPress);
      // 
      // pitchY
      // 
      this->pitchY->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->pitchY->Location = System::Drawing::Point(151, 27);
      this->pitchY->Name = L"pitchY";
      this->pitchY->Size = System::Drawing::Size(51, 23);
      this->pitchY->TabIndex = 16;
      this->pitchY->TextChanged += gcnew System::EventHandler(this, &FormVisParam::pitchY_TextChanged);
      this->pitchY->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &FormVisParam::pitch_KeyPress);
      // 
      // pitchX
      // 
      this->pitchX->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->pitchX->Location = System::Drawing::Point(93, 27);
      this->pitchX->Name = L"pitchX";
      this->pitchX->Size = System::Drawing::Size(51, 23);
      this->pitchX->TabIndex = 15;
      this->pitchX->TextChanged += gcnew System::EventHandler(this, &FormVisParam::pitchX_TextChanged);
      this->pitchX->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &FormVisParam::pitch_KeyPress);
      // 
      // bgColorLabel
      // 
      this->bgColorLabel->AutoSize = true;
      this->bgColorLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->bgColorLabel->Location = System::Drawing::Point(9, 157);
      this->bgColorLabel->Name = L"bgColorLabel";
      this->bgColorLabel->Size = System::Drawing::Size(74, 17);
      this->bgColorLabel->TabIndex = 18;
      this->bgColorLabel->Text = L"BackGround";
      // 
      // winLevelLabel
      // 
      this->winLevelLabel->AutoSize = true;
      this->winLevelLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->winLevelLabel->Location = System::Drawing::Point(3, 73);
      this->winLevelLabel->Name = L"winLevelLabel";
      this->winLevelLabel->Size = System::Drawing::Size(41, 17);
      this->winLevelLabel->TabIndex = 22;
      this->winLevelLabel->Text = L"WinLv";
      // 
      // winLevelMinLabel
      // 
      this->winLevelMinLabel->AutoSize = true;
      this->winLevelMinLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->winLevelMinLabel->Location = System::Drawing::Point(43, 59);
      this->winLevelMinLabel->Name = L"winLevelMinLabel";
      this->winLevelMinLabel->Size = System::Drawing::Size(27, 17);
      this->winLevelMinLabel->TabIndex = 23;
      this->winLevelMinLabel->Text = L"Min";
      // 
      // winLevelMaxLabel
      // 
      this->winLevelMaxLabel->AutoSize = true;
      this->winLevelMaxLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->winLevelMaxLabel->Location = System::Drawing::Point(43, 85);
      this->winLevelMaxLabel->Name = L"winLevelMaxLabel";
      this->winLevelMaxLabel->Size = System::Drawing::Size(29, 17);
      this->winLevelMaxLabel->TabIndex = 24;
      this->winLevelMaxLabel->Text = L"Max";
      // 
      // winLevelMinTextBox
      // 
      this->winLevelMinTextBox->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->winLevelMinTextBox->Location = System::Drawing::Point(76, 56);
      this->winLevelMinTextBox->Name = L"winLevelMinTextBox";
      this->winLevelMinTextBox->ReadOnly = true;
      this->winLevelMinTextBox->Size = System::Drawing::Size(64, 23);
      this->winLevelMinTextBox->TabIndex = 27;
      // 
      // winLevelMaxTextBox
      // 
      this->winLevelMaxTextBox->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->winLevelMaxTextBox->Location = System::Drawing::Point(77, 82);
      this->winLevelMaxTextBox->Name = L"winLevelMaxTextBox";
      this->winLevelMaxTextBox->ReadOnly = true;
      this->winLevelMaxTextBox->Size = System::Drawing::Size(63, 23);
      this->winLevelMaxTextBox->TabIndex = 28;
      // 
      // transLabel
      // 
      this->transLabel->AutoSize = true;
      this->transLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->transLabel->Location = System::Drawing::Point(9, 183);
      this->transLabel->Name = L"transLabel";
      this->transLabel->Size = System::Drawing::Size(48, 17);
      this->transLabel->TabIndex = 32;
      this->transLabel->Text = L"Opacify";
      // 
      // sliceLabel
      // 
      this->sliceLabel->AutoSize = true;
      this->sliceLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->sliceLabel->Location = System::Drawing::Point(145, 183);
      this->sliceLabel->Name = L"sliceLabel";
      this->sliceLabel->Size = System::Drawing::Size(76, 17);
      this->sliceLabel->TabIndex = 33;
      this->sliceLabel->Text = L"Num of slice";
      this->sliceLabel->Click += gcnew System::EventHandler(this, &FormVisParam::sliceLabel_Click);
      // 
      // transBar
      // 
      this->transBar->Location = System::Drawing::Point(6, 199);
      this->transBar->Name = L"transBar";
      this->transBar->Size = System::Drawing::Size(126, 45);
      this->transBar->TabIndex = 34;
      this->transBar->TickStyle = System::Windows::Forms::TickStyle::None;
      this->transBar->Scroll += gcnew System::EventHandler(this, &FormVisParam::transBar_Scroll);
      // 
      // sliceBar
      // 
      this->sliceBar->Location = System::Drawing::Point(138, 199);
      this->sliceBar->Name = L"sliceBar";
      this->sliceBar->Size = System::Drawing::Size(140, 45);
      this->sliceBar->TabIndex = 35;
      this->sliceBar->TickStyle = System::Windows::Forms::TickStyle::None;
      this->sliceBar->Scroll += gcnew System::EventHandler(this, &FormVisParam::sliceBar_Scroll);
      // 
      // winLevelMinBar
      // 
      this->winLevelMinBar->Location = System::Drawing::Point(141, 56);
      this->winLevelMinBar->Name = L"winLevelMinBar";
      this->winLevelMinBar->Size = System::Drawing::Size(148, 45);
      this->winLevelMinBar->TabIndex = 37;
      this->winLevelMinBar->TickStyle = System::Windows::Forms::TickStyle::None;
      this->winLevelMinBar->Scroll += gcnew System::EventHandler(this, &FormVisParam::scrollWinLevelMin);
      this->winLevelMinBar->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &FormVisParam::winLevelMinBar_MouseUp);
      // 
      // winLevelMaxBar
      // 
      this->winLevelMaxBar->Location = System::Drawing::Point(140, 86);
      this->winLevelMaxBar->Name = L"winLevelMaxBar";
      this->winLevelMaxBar->Size = System::Drawing::Size(148, 45);
      this->winLevelMaxBar->TabIndex = 38;
      this->winLevelMaxBar->TickStyle = System::Windows::Forms::TickStyle::None;
      this->winLevelMaxBar->Scroll += gcnew System::EventHandler(this, &FormVisParam::scrollWinLevelMax);
      this->winLevelMaxBar->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &FormVisParam::winLevelMaxBar_MouseUp);
      // 
      // pictBox
      // 
      this->pictBox->Location = System::Drawing::Point(12, 225);
      this->pictBox->Name = L"pictBox";
      this->pictBox->Size = System::Drawing::Size(271, 72);
      this->pictBox->TabIndex = 39;
      this->pictBox->TabStop = false;
      this->pictBox->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &FormVisParam::pictBox_MouseDown);
      this->pictBox->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &FormVisParam::pictBox_MouseMove);
      this->pictBox->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &FormVisParam::pictBox_MouseUp);
      // 
      // isRendIndi
      // 
      this->isRendIndi->AutoSize = true;
      this->isRendIndi->Checked = true;
      this->isRendIndi->CheckState = System::Windows::Forms::CheckState::Checked;
      this->isRendIndi->Font = (gcnew System::Drawing::Font(L"メイリオ", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->isRendIndi->Location = System::Drawing::Point(210, 110);
      this->isRendIndi->Name = L"isRendIndi";
      this->isRendIndi->Size = System::Drawing::Size(75, 21);
      this->isRendIndi->TabIndex = 40;
      this->isRendIndi->Text = L"indicator";
      this->isRendIndi->UseVisualStyleBackColor = true;
      this->isRendIndi->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendIndi_CheckedChanged);
      // 
      // isWhite
      // 
      this->isWhite->AutoSize = true;
      this->isWhite->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->isWhite->Location = System::Drawing::Point(93, 157);
      this->isWhite->Name = L"isWhite";
      this->isWhite->Size = System::Drawing::Size(58, 22);
      this->isWhite->TabIndex = 41;
      this->isWhite->Text = L"white";
      this->isWhite->UseVisualStyleBackColor = true;
      this->isWhite->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isWhite_CheckedChanged);
      // 
      // isBlack
      // 
      this->isBlack->AutoSize = true;
      this->isBlack->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->isBlack->Location = System::Drawing::Point(156, 157);
      this->isBlack->Name = L"isBlack";
      this->isBlack->Size = System::Drawing::Size(56, 22);
      this->isBlack->TabIndex = 42;
      this->isBlack->Text = L"black";
      this->isBlack->UseVisualStyleBackColor = true;
      this->isBlack->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isBlack_CheckedChanged);
      // 
      // isGray
      // 
      this->isGray->AutoSize = true;
      this->isGray->Checked = true;
      this->isGray->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->isGray->Location = System::Drawing::Point(218, 157);
      this->isGray->Name = L"isGray";
      this->isGray->Size = System::Drawing::Size(52, 22);
      this->isGray->TabIndex = 43;
      this->isGray->TabStop = true;
      this->isGray->Text = L"gray";
      this->isGray->UseVisualStyleBackColor = true;
      this->isGray->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isGray_CheckedChanged);
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label1->Location = System::Drawing::Point(14, 304);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(49, 20);
      this->label1->TabIndex = 44;
      this->label1->Text = L"frame";
      // 
      // FormVisParam
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(8, 20);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(296, 347);
      this->Controls->Add(this->label1);
      this->Controls->Add(this->isGray);
      this->Controls->Add(this->isBlack);
      this->Controls->Add(this->isWhite);
      this->Controls->Add(this->isRendIndi);
      this->Controls->Add(this->pictBox);
      this->Controls->Add(this->isRendVolume);
      this->Controls->Add(this->isRendPseudo);
      this->Controls->Add(this->winLevelMaxBar);
      this->Controls->Add(this->winLevelMinBar);
      this->Controls->Add(this->sliceBar);
      this->Controls->Add(this->transBar);
      this->Controls->Add(this->sliceLabel);
      this->Controls->Add(this->transLabel);
      this->Controls->Add(this->winLevelMaxTextBox);
      this->Controls->Add(this->winLevelMinTextBox);
      this->Controls->Add(this->winLevelMaxLabel);
      this->Controls->Add(this->winLevelMinLabel);
      this->Controls->Add(this->winLevelLabel);
      this->Controls->Add(this->bgColorLabel);
      this->Controls->Add(this->pitchZ);
      this->Controls->Add(this->pitchY);
      this->Controls->Add(this->pitchX);
      this->Controls->Add(this->sizeZ);
      this->Controls->Add(this->sizeY);
      this->Controls->Add(this->sizeX);
      this->Controls->Add(this->pitchLabel);
      this->Controls->Add(this->sizePanel);
      this->Controls->Add(this->planeLabel);
      this->Controls->Add(this->timeSlider);
      this->Controls->Add(this->timeLabel);
      this->Controls->Add(this->isRendPlaneZX);
      this->Controls->Add(this->isRendPlaneYZ);
      this->Controls->Add(this->isRendPlaneXY);
      this->Controls->Add(this->isRendFrame);
      this->Font = (gcnew System::Drawing::Font(L"メイリオ", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
      this->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
      this->Name = L"FormVisParam";
      this->Text = L"Rendering Pram";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->timeSlider))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->transBar))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->sliceBar))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->winLevelMinBar))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->winLevelMaxBar))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictBox))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private: System::Void isRendFrame_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void isRendVolume_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void isRendPseudo_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void isRendIndi_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void isRendPlaneXY_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void isRendPlaneYZ_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void isRendPlaneZX_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  //private: System::Void isWhite_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  //private: System::Void isBlack_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  //private: System::Void isGray_CheckedChanged(System::Object^   sender, System::EventArgs^  e);
  private: System::Void pitch_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
  private: System::Void scrollWinLevelMin(System::Object^  sender, System::EventArgs^  e);
  private: System::Void scrollWinLevelMax(System::Object^  sender, System::EventArgs^  e);
  private: System::Void scrollTime(System::Object^  sender, System::EventArgs^  e);
  private: System::Void winLevelMinBar_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
  private: System::Void winLevelMaxBar_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
  private: System::Void transBar_Scroll(System::Object^  sender, System::EventArgs^  e);
  private: System::Void sliceBar_Scroll(System::Object^  sender, System::EventArgs^  e);
  private: System::Void pictBox_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
  private: System::Void pictBox_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
  private: System::Void pictBox_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
  private: System::Void sliceLabel_Click(System::Object^  sender, System::EventArgs^  e) {
  }

private: 
  System::Void isGray_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  System::Void isBlack_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  System::Void isWhite_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  System::Void pitchX_TextChanged(System::Object^  sender, System::EventArgs^  e) ;
  System::Void pitchY_TextChanged(System::Object^  sender, System::EventArgs^  e) ;
  System::Void pitchZ_TextChanged(System::Object^  sender, System::EventArgs^  e);
};
  inline bool  formVisParam_bPlaneXY() { return FormVisParam::GetInst()->bPlaneXY(); }
  inline bool  formVisParam_bPlaneYZ() { return FormVisParam::GetInst()->bPlaneYZ(); }
  inline bool  formVisParam_bPlaneZX() { return FormVisParam::GetInst()->bPlaneZX(); }
  inline bool  formVisParam_bRendVol() { return FormVisParam::GetInst()->bRendVol(); }
  inline bool  formVisParam_bGradMag() { return FormVisParam::GetInst()->bGradMag(); }
  inline bool  formVisParam_bDoPsued() { return FormVisParam::GetInst()->bDoPsued(); }
  inline bool  formVisParam_bRendIndi(){ return FormVisParam::GetInst()->bRendIndi(); }
  inline float formVisParam_getAlpha() { return FormVisParam::GetInst()->GetAlpha(); }

  inline int   formVisParam_getframeI()   { return FormVisParam::GetInst()->GetframeI(); }
  inline int   formVisParam_getWinLvMin() { return FormVisParam::GetInst()->GetWinLvMin(); }
  inline int   formVisParam_getWinLvMax() { return FormVisParam::GetInst()->GetWinLvMax(); }
  inline int   formVisParam_getSliceNum() { return FormVisParam::GetInst()->GetSliceNum(); }
  inline bool  formVisParam_bOnManip()    { return FormVisParam::GetInst()->bOnManip(); }
  inline void  formVisParam_bindTfImg()   { FormVisParam::GetInst()->BindImgTf(); }
  inline void  formVisParam_bindPsuImg()  { FormVisParam::GetInst()->BindImgPsu(); }
}

