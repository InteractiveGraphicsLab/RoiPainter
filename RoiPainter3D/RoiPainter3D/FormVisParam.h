#pragma once

// FormVisParam (managed class)
//
// 右上の可視化パラメータ用のダイアログ
// Volume rendering にかかわるパラメータのUIを提供する
// 
// Singletonで実装
// パラメータ変更に伴い、ImageCoreとFormMainに classにアクセスする

#define TRANS_FUNC_SIZE 256
#define HIST_BIN_SIZE   256

namespace RoiPainter3D {

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
      System::Windows::Forms::Label^  label_reso;
      System::Windows::Forms::Label^  label_pitch;
      System::Windows::Forms::Label^  sliceLabel;
      System::Windows::Forms::Label^  transLabel;
      System::Windows::Forms::Label^  winLevelMaxLabel;
      System::Windows::Forms::Label^  winLevelMinLabel;
      System::Windows::Forms::Label^  winLevelLabel;
      System::Windows::Forms::Label^  bgColorLabel;
      System::Windows::Forms::Label^  planeLabel;

      System::Windows::Forms::TextBox^  pitchZ;
      System::Windows::Forms::TextBox^  pitchY;
      System::Windows::Forms::TextBox^  pitchX;
      System::Windows::Forms::TextBox^  sizeZ;
      System::Windows::Forms::TextBox^  sizeY;
      System::Windows::Forms::TextBox^  sizeX;
      System::Windows::Forms::TextBox^  winLevelMaxTextBox;
      System::Windows::Forms::TextBox^  winLevelMinTextBox;

      System::Windows::Forms::CheckBox^  isRendIndi;
      System::Windows::Forms::CheckBox^  isRendVolume;
      System::Windows::Forms::CheckBox^  isRendPseudo;
      System::Windows::Forms::CheckBox^  isRendPlaneZX;
      System::Windows::Forms::CheckBox^  isRendPlaneYZ;
      System::Windows::Forms::CheckBox^  isRendPlaneXY;
      System::Windows::Forms::CheckBox^  isRendFrame;
      System::Windows::Forms::CheckBox^  isRendGradMag;

      System::Windows::Forms::RadioButton^  isGray;
      System::Windows::Forms::RadioButton^  isBlack;
      System::Windows::Forms::RadioButton^  isWhite;
      System::Windows::Forms::TrackBar^     winLevelMaxBar;
      System::Windows::Forms::TrackBar^     winLevelMinBar;
      System::Windows::Forms::TrackBar^     sliceBar;
      System::Windows::Forms::TrackBar^     transBar;
      System::Windows::Forms::PictureBox^  pictBox1;
      System::Windows::Forms::PictureBox^  pictBox2;
      System::Windows::Forms::CheckBox^ doInterpolation;
      System::Windows::Forms::CheckBox^ isRendOtherRoi;

      Bitmap ^m_HistBmp, ^m_HistGMBmp;
      bool    m_isMouseOn;
      int     m_mouseX, m_mouseY;
      void* m_imgTf ; // OglImage1D<CH_RGBA> 1st ch(intensity-->trans), 2nd (grad mag-->trans) 
      void* m_imgPsu; // OglImage1D<CH_RGBA> func: intensity --> psude color 

    static FormVisParam^ m_singleton;

    FormVisParam(void);
    void pitchTextBoxChanged();

  public:
    static FormVisParam^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormVisParam();
      return m_singleton;
    }

    void initAllItemsForNewImg ();
    void updateHistogramBmp    ();
    void redrawTransFuncPictBox();
    void PitchUpdated();

    bool  bRendFrame(){ return isRendFrame  ->Checked; }
    bool  bRendIndi (){ return isRendIndi   ->Checked; }   
    bool  bPlaneXY  (){ return isRendPlaneXY->Checked; }
    bool  bPlaneYZ  (){ return isRendPlaneYZ->Checked; }
    bool  bPlaneZX  (){ return isRendPlaneZX->Checked; }
    bool  bRendVol  (){ return isRendVolume ->Checked; }
    bool  bGradMag  (){ return isRendGradMag->Checked; }
    bool  bDoPsued  (){ return isRendPseudo ->Checked; }
    bool  bOtherRoi (){ return isRendOtherRoi->Checked;}
    bool  bOnManip  (){ return m_isMouseOn;}
    bool  doImgInterpolation(){ return doInterpolation->Checked;}

    float getAlpha   (){return transBar->Value * 0.01f;}
    int   getSliceNum(){return sliceBar->Value;        }

    void bindTfImg(); 
    void bindPsuImg();


    void LockPitchBox(){
      pitchX->ReadOnly = true;
      pitchY->ReadOnly = true;
      pitchZ->ReadOnly = true;
    }

    void UnlockPitchBox(){
      pitchX->ReadOnly = false;
      pitchY->ReadOnly = false;
      pitchZ->ReadOnly = false;
    }

    void SetPlanesCheck(bool bxy, bool byz, bool bzx){
      isRendPlaneXY->Checked = bxy;
      isRendPlaneYZ->Checked = byz;
      isRendPlaneZX->Checked = bzx;
    }
    
	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormVisParam()
		{
			if (components)
			{
				delete components;
			}
		}

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
      this->label_reso = (gcnew System::Windows::Forms::Label());
      this->label_pitch = (gcnew System::Windows::Forms::Label());
      this->pitchZ = (gcnew System::Windows::Forms::TextBox());
      this->pitchY = (gcnew System::Windows::Forms::TextBox());
      this->pitchX = (gcnew System::Windows::Forms::TextBox());
      this->sizeZ = (gcnew System::Windows::Forms::TextBox());
      this->sizeY = (gcnew System::Windows::Forms::TextBox());
      this->sizeX = (gcnew System::Windows::Forms::TextBox());
      this->isGray = (gcnew System::Windows::Forms::RadioButton());
      this->isBlack = (gcnew System::Windows::Forms::RadioButton());
      this->isWhite = (gcnew System::Windows::Forms::RadioButton());
      this->isRendIndi = (gcnew System::Windows::Forms::CheckBox());
      this->pictBox1 = (gcnew System::Windows::Forms::PictureBox());
      this->isRendVolume = (gcnew System::Windows::Forms::CheckBox());
      this->isRendPseudo = (gcnew System::Windows::Forms::CheckBox());
      this->winLevelMaxBar = (gcnew System::Windows::Forms::TrackBar());
      this->winLevelMinBar = (gcnew System::Windows::Forms::TrackBar());
      this->sliceBar = (gcnew System::Windows::Forms::TrackBar());
      this->transBar = (gcnew System::Windows::Forms::TrackBar());
      this->sliceLabel = (gcnew System::Windows::Forms::Label());
      this->transLabel = (gcnew System::Windows::Forms::Label());
      this->winLevelMaxTextBox = (gcnew System::Windows::Forms::TextBox());
      this->winLevelMinTextBox = (gcnew System::Windows::Forms::TextBox());
      this->winLevelMaxLabel = (gcnew System::Windows::Forms::Label());
      this->winLevelMinLabel = (gcnew System::Windows::Forms::Label());
      this->winLevelLabel = (gcnew System::Windows::Forms::Label());
      this->bgColorLabel = (gcnew System::Windows::Forms::Label());
      this->planeLabel = (gcnew System::Windows::Forms::Label());
      this->isRendPlaneZX = (gcnew System::Windows::Forms::CheckBox());
      this->isRendPlaneYZ = (gcnew System::Windows::Forms::CheckBox());
      this->isRendPlaneXY = (gcnew System::Windows::Forms::CheckBox());
      this->isRendFrame = (gcnew System::Windows::Forms::CheckBox());
      this->pictBox2 = (gcnew System::Windows::Forms::PictureBox());
      this->isRendGradMag = (gcnew System::Windows::Forms::CheckBox());
      this->doInterpolation = (gcnew System::Windows::Forms::CheckBox());
      this->isRendOtherRoi = (gcnew System::Windows::Forms::CheckBox());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictBox1))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->winLevelMaxBar))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->winLevelMinBar))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->sliceBar))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->transBar))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictBox2))->BeginInit();
      this->SuspendLayout();
      // 
      // label_reso
      // 
      this->label_reso->AutoSize = true;
      this->label_reso->Location = System::Drawing::Point(12, 9);
      this->label_reso->Name = L"label_reso";
      this->label_reso->Size = System::Drawing::Size(67, 12);
      this->label_reso->TabIndex = 0;
      this->label_reso->Text = L"size [pixels]";
      // 
      // label_pitch
      // 
      this->label_pitch->AutoSize = true;
      this->label_pitch->Location = System::Drawing::Point(18, 34);
      this->label_pitch->Name = L"label_pitch";
      this->label_pitch->Size = System::Drawing::Size(60, 12);
      this->label_pitch->TabIndex = 1;
      this->label_pitch->Text = L"pitch [mm]";
      // 
      // pitchZ
      // 
      this->pitchZ->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->pitchZ->Location = System::Drawing::Point(210, 29);
      this->pitchZ->Name = L"pitchZ";
      this->pitchZ->Size = System::Drawing::Size(60, 23);
      this->pitchZ->TabIndex = 23;
      this->pitchZ->TextChanged += gcnew System::EventHandler(this, &FormVisParam::pitchZ_TextChanged);
      // 
      // pitchY
      // 
      this->pitchY->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->pitchY->Location = System::Drawing::Point(146, 29);
      this->pitchY->Name = L"pitchY";
      this->pitchY->Size = System::Drawing::Size(60, 23);
      this->pitchY->TabIndex = 22;
      this->pitchY->TextChanged += gcnew System::EventHandler(this, &FormVisParam::pitchY_TextChanged);
      // 
      // pitchX
      // 
      this->pitchX->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->pitchX->Location = System::Drawing::Point(81, 29);
      this->pitchX->Name = L"pitchX";
      this->pitchX->Size = System::Drawing::Size(60, 23);
      this->pitchX->TabIndex = 21;
      this->pitchX->TextChanged += gcnew System::EventHandler(this, &FormVisParam::pitchX_TextChanged);
      // 
      // sizeZ
      // 
      this->sizeZ->BackColor = System::Drawing::SystemColors::Info;
      this->sizeZ->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->sizeZ->Location = System::Drawing::Point(212, 3);
      this->sizeZ->Name = L"sizeZ";
      this->sizeZ->ReadOnly = true;
      this->sizeZ->Size = System::Drawing::Size(59, 23);
      this->sizeZ->TabIndex = 20;
      // 
      // sizeY
      // 
      this->sizeY->BackColor = System::Drawing::SystemColors::Info;
      this->sizeY->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->sizeY->Location = System::Drawing::Point(147, 3);
      this->sizeY->Name = L"sizeY";
      this->sizeY->ReadOnly = true;
      this->sizeY->Size = System::Drawing::Size(59, 23);
      this->sizeY->TabIndex = 19;
      // 
      // sizeX
      // 
      this->sizeX->BackColor = System::Drawing::SystemColors::Info;
      this->sizeX->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->sizeX->Location = System::Drawing::Point(82, 3);
      this->sizeX->Name = L"sizeX";
      this->sizeX->ReadOnly = true;
      this->sizeX->Size = System::Drawing::Size(59, 23);
      this->sizeX->TabIndex = 18;
      // 
      // isGray
      // 
      this->isGray->AutoSize = true;
      this->isGray->Checked = true;
      this->isGray->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->isGray->Location = System::Drawing::Point(215, 179);
      this->isGray->Name = L"isGray";
      this->isGray->Size = System::Drawing::Size(52, 22);
      this->isGray->TabIndex = 67;
      this->isGray->TabStop = true;
      this->isGray->Text = L"gray";
      this->isGray->UseVisualStyleBackColor = true;
      this->isGray->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isGray_CheckedChanged);
      // 
      // isBlack
      // 
      this->isBlack->AutoSize = true;
      this->isBlack->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->isBlack->Location = System::Drawing::Point(153, 179);
      this->isBlack->Name = L"isBlack";
      this->isBlack->Size = System::Drawing::Size(56, 22);
      this->isBlack->TabIndex = 66;
      this->isBlack->Text = L"black";
      this->isBlack->UseVisualStyleBackColor = true;
      this->isBlack->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isBlack_CheckedChanged);
      // 
      // isWhite
      // 
      this->isWhite->AutoSize = true;
      this->isWhite->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->isWhite->Location = System::Drawing::Point(90, 179);
      this->isWhite->Name = L"isWhite";
      this->isWhite->Size = System::Drawing::Size(58, 22);
      this->isWhite->TabIndex = 65;
      this->isWhite->Text = L"white";
      this->isWhite->UseVisualStyleBackColor = true;
      this->isWhite->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isWhite_CheckedChanged);
      // 
      // isRendIndi
      // 
      this->isRendIndi->AutoSize = true;
      this->isRendIndi->Checked = true;
      this->isRendIndi->CheckState = System::Windows::Forms::CheckState::Checked;
      this->isRendIndi->Font = (gcnew System::Drawing::Font(L"メイリオ", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->isRendIndi->Location = System::Drawing::Point(172, 135);
      this->isRendIndi->Name = L"isRendIndi";
      this->isRendIndi->Size = System::Drawing::Size(75, 21);
      this->isRendIndi->TabIndex = 64;
      this->isRendIndi->Text = L"indicator";
      this->isRendIndi->UseVisualStyleBackColor = true;
      this->isRendIndi->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendIndi_CheckedChanged);
      // 
      // pictBox1
      // 
      this->pictBox1->Location = System::Drawing::Point(13, 253);
      this->pictBox1->Name = L"pictBox1";
      this->pictBox1->Size = System::Drawing::Size(251, 60);
      this->pictBox1->TabIndex = 63;
      this->pictBox1->TabStop = false;
      this->pictBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &FormVisParam::pictBox1_MouseDown);
      this->pictBox1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &FormVisParam::pictBox1_MouseMove);
      this->pictBox1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &FormVisParam::pictBox1_MouseUp);
      // 
      // isRendVolume
      // 
      this->isRendVolume->AutoSize = true;
      this->isRendVolume->Checked = true;
      this->isRendVolume->CheckState = System::Windows::Forms::CheckState::Checked;
      this->isRendVolume->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->isRendVolume->Location = System::Drawing::Point(68, 113);
      this->isRendVolume->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
      this->isRendVolume->Name = L"isRendVolume";
      this->isRendVolume->Size = System::Drawing::Size(67, 21);
      this->isRendVolume->TabIndex = 45;
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
      this->isRendPseudo->Location = System::Drawing::Point(135, 113);
      this->isRendPseudo->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
      this->isRendPseudo->Name = L"isRendPseudo";
      this->isRendPseudo->Size = System::Drawing::Size(66, 21);
      this->isRendPseudo->TabIndex = 46;
      this->isRendPseudo->Text = L"pseudo";
      this->isRendPseudo->UseVisualStyleBackColor = true;
      this->isRendPseudo->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendPseudo_CheckedChanged);
      // 
      // winLevelMaxBar
      // 
      this->winLevelMaxBar->Location = System::Drawing::Point(125, 87);
      this->winLevelMaxBar->Name = L"winLevelMaxBar";
      this->winLevelMaxBar->Size = System::Drawing::Size(167, 45);
      this->winLevelMaxBar->TabIndex = 62;
      this->winLevelMaxBar->TickStyle = System::Windows::Forms::TickStyle::None;
      this->winLevelMaxBar->Scroll += gcnew System::EventHandler(this, &FormVisParam::winLevelMaxBar_Scroll);
      this->winLevelMaxBar->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &FormVisParam::winLevelMaxBar_MouseUp);
      // 
      // winLevelMinBar
      // 
      this->winLevelMinBar->Location = System::Drawing::Point(126, 57);
      this->winLevelMinBar->Name = L"winLevelMinBar";
      this->winLevelMinBar->Size = System::Drawing::Size(166, 45);
      this->winLevelMinBar->TabIndex = 61;
      this->winLevelMinBar->TickStyle = System::Windows::Forms::TickStyle::None;
      this->winLevelMinBar->Scroll += gcnew System::EventHandler(this, &FormVisParam::winLevelMinBar_Scroll);
      this->winLevelMinBar->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &FormVisParam::winLevelMinBar_MouseUp);
      // 
      // sliceBar
      // 
      this->sliceBar->Location = System::Drawing::Point(135, 220);
      this->sliceBar->Name = L"sliceBar";
      this->sliceBar->Size = System::Drawing::Size(140, 45);
      this->sliceBar->TabIndex = 60;
      this->sliceBar->TickStyle = System::Windows::Forms::TickStyle::None;
      this->sliceBar->Scroll += gcnew System::EventHandler(this, &FormVisParam::sliceBar_Scroll);
      // 
      // transBar
      // 
      this->transBar->Location = System::Drawing::Point(3, 220);
      this->transBar->Name = L"transBar";
      this->transBar->Size = System::Drawing::Size(126, 45);
      this->transBar->TabIndex = 59;
      this->transBar->TickStyle = System::Windows::Forms::TickStyle::None;
      this->transBar->Scroll += gcnew System::EventHandler(this, &FormVisParam::transBar_Scroll);
      // 
      // sliceLabel
      // 
      this->sliceLabel->AutoSize = true;
      this->sliceLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->sliceLabel->Location = System::Drawing::Point(134, 205);
      this->sliceLabel->Name = L"sliceLabel";
      this->sliceLabel->Size = System::Drawing::Size(33, 17);
      this->sliceLabel->TabIndex = 58;
      this->sliceLabel->Text = L"Slice";
      // 
      // transLabel
      // 
      this->transLabel->AutoSize = true;
      this->transLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->transLabel->Location = System::Drawing::Point(6, 205);
      this->transLabel->Name = L"transLabel";
      this->transLabel->Size = System::Drawing::Size(37, 17);
      this->transLabel->TabIndex = 57;
      this->transLabel->Text = L"Trans";
      // 
      // winLevelMaxTextBox
      // 
      this->winLevelMaxTextBox->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->winLevelMaxTextBox->Location = System::Drawing::Point(61, 83);
      this->winLevelMaxTextBox->Name = L"winLevelMaxTextBox";
      this->winLevelMaxTextBox->ReadOnly = true;
      this->winLevelMaxTextBox->Size = System::Drawing::Size(63, 23);
      this->winLevelMaxTextBox->TabIndex = 56;
      // 
      // winLevelMinTextBox
      // 
      this->winLevelMinTextBox->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->winLevelMinTextBox->Location = System::Drawing::Point(60, 57);
      this->winLevelMinTextBox->Name = L"winLevelMinTextBox";
      this->winLevelMinTextBox->ReadOnly = true;
      this->winLevelMinTextBox->Size = System::Drawing::Size(64, 23);
      this->winLevelMinTextBox->TabIndex = 55;
      // 
      // winLevelMaxLabel
      // 
      this->winLevelMaxLabel->AutoSize = true;
      this->winLevelMaxLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->winLevelMaxLabel->Location = System::Drawing::Point(30, 86);
      this->winLevelMaxLabel->Name = L"winLevelMaxLabel";
      this->winLevelMaxLabel->Size = System::Drawing::Size(29, 17);
      this->winLevelMaxLabel->TabIndex = 54;
      this->winLevelMaxLabel->Text = L"Max";
      // 
      // winLevelMinLabel
      // 
      this->winLevelMinLabel->AutoSize = true;
      this->winLevelMinLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->winLevelMinLabel->Location = System::Drawing::Point(32, 60);
      this->winLevelMinLabel->Name = L"winLevelMinLabel";
      this->winLevelMinLabel->Size = System::Drawing::Size(27, 17);
      this->winLevelMinLabel->TabIndex = 53;
      this->winLevelMinLabel->Text = L"Min";
      // 
      // winLevelLabel
      // 
      this->winLevelLabel->AutoSize = true;
      this->winLevelLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->winLevelLabel->Location = System::Drawing::Point(4, 74);
      this->winLevelLabel->Name = L"winLevelLabel";
      this->winLevelLabel->Size = System::Drawing::Size(41, 17);
      this->winLevelLabel->TabIndex = 52;
      this->winLevelLabel->Text = L"WinLv";
      // 
      // bgColorLabel
      // 
      this->bgColorLabel->AutoSize = true;
      this->bgColorLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->bgColorLabel->Location = System::Drawing::Point(6, 179);
      this->bgColorLabel->Name = L"bgColorLabel";
      this->bgColorLabel->Size = System::Drawing::Size(74, 17);
      this->bgColorLabel->TabIndex = 51;
      this->bgColorLabel->Text = L"BackGround";
      // 
      // planeLabel
      // 
      this->planeLabel->AutoSize = true;
      this->planeLabel->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->planeLabel->Location = System::Drawing::Point(8, 162);
      this->planeLabel->Name = L"planeLabel";
      this->planeLabel->Size = System::Drawing::Size(37, 17);
      this->planeLabel->TabIndex = 50;
      this->planeLabel->Text = L"Plane";
      // 
      // isRendPlaneZX
      // 
      this->isRendPlaneZX->AutoSize = true;
      this->isRendPlaneZX->Checked = true;
      this->isRendPlaneZX->CheckState = System::Windows::Forms::CheckState::Checked;
      this->isRendPlaneZX->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->isRendPlaneZX->Location = System::Drawing::Point(213, 158);
      this->isRendPlaneZX->Name = L"isRendPlaneZX";
      this->isRendPlaneZX->Size = System::Drawing::Size(39, 21);
      this->isRendPlaneZX->TabIndex = 49;
      this->isRendPlaneZX->Text = L"zx";
      this->isRendPlaneZX->UseVisualStyleBackColor = true;
      this->isRendPlaneZX->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendPlaneZX_CheckedChanged);
      // 
      // isRendPlaneYZ
      // 
      this->isRendPlaneYZ->AutoSize = true;
      this->isRendPlaneYZ->Checked = true;
      this->isRendPlaneYZ->CheckState = System::Windows::Forms::CheckState::Checked;
      this->isRendPlaneYZ->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->isRendPlaneYZ->Location = System::Drawing::Point(152, 158);
      this->isRendPlaneYZ->Name = L"isRendPlaneYZ";
      this->isRendPlaneYZ->Size = System::Drawing::Size(39, 21);
      this->isRendPlaneYZ->TabIndex = 48;
      this->isRendPlaneYZ->Text = L"yz";
      this->isRendPlaneYZ->UseVisualStyleBackColor = true;
      this->isRendPlaneYZ->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendPlaneYZ_CheckedChanged);
      // 
      // isRendPlaneXY
      // 
      this->isRendPlaneXY->AutoSize = true;
      this->isRendPlaneXY->Checked = true;
      this->isRendPlaneXY->CheckState = System::Windows::Forms::CheckState::Checked;
      this->isRendPlaneXY->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->isRendPlaneXY->Location = System::Drawing::Point(90, 158);
      this->isRendPlaneXY->Name = L"isRendPlaneXY";
      this->isRendPlaneXY->Size = System::Drawing::Size(39, 21);
      this->isRendPlaneXY->TabIndex = 47;
      this->isRendPlaneXY->Text = L"xy";
      this->isRendPlaneXY->UseVisualStyleBackColor = true;
      this->isRendPlaneXY->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendPlaneXY_CheckedChanged);
      // 
      // isRendFrame
      // 
      this->isRendFrame->AutoSize = true;
      this->isRendFrame->Checked = true;
      this->isRendFrame->CheckState = System::Windows::Forms::CheckState::Checked;
      this->isRendFrame->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->isRendFrame->Location = System::Drawing::Point(6, 113);
      this->isRendFrame->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
      this->isRendFrame->Name = L"isRendFrame";
      this->isRendFrame->Size = System::Drawing::Size(59, 21);
      this->isRendFrame->TabIndex = 44;
      this->isRendFrame->Text = L"frame";
      this->isRendFrame->UseVisualStyleBackColor = true;
      this->isRendFrame->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendFrame_CheckedChanged);
      // 
      // pictBox2
      // 
      this->pictBox2->Location = System::Drawing::Point(13, 319);
      this->pictBox2->Name = L"pictBox2";
      this->pictBox2->Size = System::Drawing::Size(251, 60);
      this->pictBox2->TabIndex = 68;
      this->pictBox2->TabStop = false;
      this->pictBox2->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &FormVisParam::pictBox2_MouseDown);
      this->pictBox2->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &FormVisParam::pictBox2_MouseMove);
      this->pictBox2->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &FormVisParam::pictBox2_MouseUp);
      // 
      // isRendGradMag
      // 
      this->isRendGradMag->AutoSize = true;
      this->isRendGradMag->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->isRendGradMag->Location = System::Drawing::Point(6, 135);
      this->isRendGradMag->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
      this->isRendGradMag->Name = L"isRendGradMag";
      this->isRendGradMag->Size = System::Drawing::Size(52, 21);
      this->isRendGradMag->TabIndex = 69;
      this->isRendGradMag->Text = L"grad";
      this->isRendGradMag->UseVisualStyleBackColor = true;
      this->isRendGradMag->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendGradMag_CheckedChanged);
      // 
      // doInterpolation
      // 
      this->doInterpolation->AutoSize = true;
      this->doInterpolation->Checked = true;
      this->doInterpolation->CheckState = System::Windows::Forms::CheckState::Checked;
      this->doInterpolation->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->doInterpolation->Location = System::Drawing::Point(68, 135);
      this->doInterpolation->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
      this->doInterpolation->Name = L"doInterpolation";
      this->doInterpolation->Size = System::Drawing::Size(97, 21);
      this->doInterpolation->TabIndex = 70;
      this->doInterpolation->Text = L"Interpolation";
      this->doInterpolation->UseVisualStyleBackColor = true;
      this->doInterpolation->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::doInterpolation_CheckedChanged);
      // 
      // isRendOtherRoi
      // 
      this->isRendOtherRoi->AutoSize = true;
      this->isRendOtherRoi->Font = (gcnew System::Drawing::Font(L"メイリオ", 8));
      this->isRendOtherRoi->Location = System::Drawing::Point(204, 113);
      this->isRendOtherRoi->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
      this->isRendOtherRoi->Name = L"isRendOtherRoi";
      this->isRendOtherRoi->Size = System::Drawing::Size(81, 21);
      this->isRendOtherRoi->TabIndex = 71;
      this->isRendOtherRoi->Text = L"otherROIs";
      this->isRendOtherRoi->UseVisualStyleBackColor = true;
      this->isRendOtherRoi->CheckedChanged += gcnew System::EventHandler(this, &FormVisParam::isRendOtherRoi_CheckedChanged);
      // 
      // FormVisParam
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->BackColor = System::Drawing::SystemColors::Info;
      this->ClientSize = System::Drawing::Size(298, 388);
      this->Controls->Add(this->isRendOtherRoi);
      this->Controls->Add(this->doInterpolation);
      this->Controls->Add(this->isRendIndi);
      this->Controls->Add(this->isRendGradMag);
      this->Controls->Add(this->isRendPseudo);
      this->Controls->Add(this->isRendVolume);
      this->Controls->Add(this->pictBox2);
      this->Controls->Add(this->isGray);
      this->Controls->Add(this->isBlack);
      this->Controls->Add(this->isWhite);
      this->Controls->Add(this->pictBox1);
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
      this->Controls->Add(this->planeLabel);
      this->Controls->Add(this->isRendPlaneZX);
      this->Controls->Add(this->isRendPlaneYZ);
      this->Controls->Add(this->isRendPlaneXY);
      this->Controls->Add(this->isRendFrame);
      this->Controls->Add(this->pitchZ);
      this->Controls->Add(this->pitchY);
      this->Controls->Add(this->pitchX);
      this->Controls->Add(this->sizeZ);
      this->Controls->Add(this->sizeY);
      this->Controls->Add(this->sizeX);
      this->Controls->Add(this->label_pitch);
      this->Controls->Add(this->label_reso);
      this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
      this->Name = L"FormVisParam";
      this->Text = L"FormVisParam";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictBox1))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->winLevelMaxBar))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->winLevelMinBar))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->sliceBar))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->transBar))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictBox2))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private: System::Void pitchX_TextChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void pitchY_TextChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void pitchZ_TextChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void winLevelMinBar_Scroll(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void winLevelMaxBar_Scroll(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void winLevelMinBar_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) ;
  private: System::Void winLevelMaxBar_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) ;
  private: System::Void isRendFrame_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void isRendVolume_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void isRendPseudo_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void isRendIndi_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void isRendPlaneXY_CheckedChanged(System::Object^  sender, System::EventArgs^  e); 
  private: System::Void isRendPlaneYZ_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void isRendPlaneZX_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void isRendOtherRoi_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void doInterpolation_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void isWhite_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void isBlack_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void isGray_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void isRendGradMag_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void transBar_Scroll(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void sliceBar_Scroll(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void pictBox1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) ;
  private: System::Void pictBox1_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) ;
  private: System::Void pictBox1_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) ;
  private: System::Void pictBox2_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) ;
  private: System::Void pictBox2_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) ;
  private: System::Void pictBox2_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) ;
};

  inline bool  formVisParam_bPlaneXY   (){return FormVisParam::getInst()->bPlaneXY();}
  inline bool  formVisParam_bPlaneYZ   (){return FormVisParam::getInst()->bPlaneYZ();}
  inline bool  FormVisParam_bPlaneZX   (){return FormVisParam::getInst()->bPlaneZX();}
  inline bool  formVisParam_bRendVol   (){return FormVisParam::getInst()->bRendVol();}
  inline bool  formVisParam_bGradMag   (){return FormVisParam::getInst()->bGradMag();}
  inline bool  formVisParam_bDoPsued   (){return FormVisParam::getInst()->bDoPsued();}
  inline bool  formVisParam_bOnManip   (){return FormVisParam::getInst()->bOnManip();}
  inline bool  formVisParam_GetOtherROI(){return FormVisParam::getInst()->bOtherRoi();}
  inline float formVisParam_getAlpha   (){return FormVisParam::getInst()->getAlpha();}
  inline int   formVisParam_getSliceNum(){return FormVisParam::getInst()->getSliceNum();}
  inline int   formVisParam_doInterpolation(){return FormVisParam::getInst()->doImgInterpolation();}
  inline void  formVisParam_bindTfImg (){ FormVisParam::getInst()->bindTfImg ();}
  inline void  formVisParam_bindPsuImg(){ FormVisParam::getInst()->bindPsuImg();}
  inline void  formVisParam_LockPitchBox() { FormVisParam::getInst()->LockPitchBox(); }
  inline void  formVisParam_UnlockPitchBox() { FormVisParam::getInst()->UnlockPitchBox();}
  inline void  formVisParam_SetPlanesCheck( bool xy, bool yz, bool zx) { 
                                              FormVisParam::getInst()->SetPlanesCheck(xy,yz,zx);}
  inline void formVisParam_initAllItemsForNewImg() { FormVisParam::getInst()->initAllItemsForNewImg(); }
  
}
