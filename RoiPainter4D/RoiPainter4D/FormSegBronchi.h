#pragma once

namespace RoiPainter4D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegBronchi の概要
	/// </summary>
	public ref class FormSegBronchi : public System::Windows::Forms::Form
	{

	private:
		FormSegBronchi(void)
		{
			InitializeComponent();
			//
			//TODO: ここにコンストラクター コードを追加します
			//
		}
	private: 
		System::Windows::Forms::Button^ file_saveCp;

		System::Windows::Forms::BindingSource^ bindingSource1;
		System::Windows::Forms::Button^ copyPrevFrameButton;
	  System::Windows::Forms::Button^ upControllPointSize_button;
	  System::Windows::Forms::Button^ downControllPoint_button;
		System::Windows::Forms::Button^ upPathWidth_button;
		System::Windows::Forms::Button^ downPathWidth_button;
		System::Windows::Forms::Button^ runCurrentFrameRGrow_button;
		System::Windows::Forms::Button^ finishSegmentation_button;
		System::Windows::Forms::Button^ file_loadCp;
		System::Windows::Forms::Label^ changeCpSize;
	  System::Windows::Forms::Label^ changePathWidth;
		System::Windows::Forms::Label^ formerAge;
		System::Windows::Forms::Label^ latterAge;
		System::Windows::Forms::Label^ threshMin;
		System::Windows::Forms::Label^ threshMax_former;
		System::Windows::Forms::Label^ latterBLayer;
		System::Windows::Forms::Label^ threshMax_latter;
		System::Windows::Forms::Label^ growth_label;
		System::Windows::Forms::Label^ regionGrowLabel;
		System::Windows::Forms::Label^ addLabel;
		System::Windows::Forms::Label^ moveLabel;
		System::Windows::Forms::Label^ ChangeParentLabel;
		System::Windows::Forms::Label^ deleteLabel;
		System::Windows::Forms::Label^ latterALabel;

	  System::Windows::Forms::TextBox^ formerAge_textBox;
	  System::Windows::Forms::TextBox^ latterAge_textBox;
	  System::Windows::Forms::TextBox^ minThreshold_textBox;
	  System::Windows::Forms::TextBox^ maxThresholdFormer_textBox;
		System::Windows::Forms::TextBox^ latterLayerB_textBox;
		System::Windows::Forms::TextBox^ maxThresholdLatter_textBox;
		System::Windows::Forms::TrackBar^ minThreshold_trackBar;
		System::Windows::Forms::TrackBar^ maxThresholdFormer_trackBar;
	  System::Windows::Forms::CheckBox^ isRendTreeStructure;
	  System::Windows::Forms::CheckBox^ isRendPath;
	  System::Windows::Forms::TrackBar^ maxThresholdLatter_trackBar;
	  System::Windows::Forms::TextBox^ latterLayerRGrow_textBox;
	  System::Windows::Forms::RadioButton^ addCpButton;
	  System::Windows::Forms::RadioButton^ moveCpButton;

    static FormSegBronchi^ m_singleton;

  public:
    static FormSegBronchi^ GetInst() {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormSegBronchi();
      }
      return m_singleton;
    }

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormSegBronchi()
		{
			if (components)
			{
				delete components;
			}
		}
public: 
	void InitAllItems(int vol_min, int vol_max);
	bool GetIsRendTreeStructure() { return isRendTreeStructure->Checked; };
	bool GetIsRendPath() { return isRendPath->Checked; }
	bool GetIsAddCpMode() { return addCpButton->Checked; }
	bool GetIsMoveCpMode() { return moveCpButton->Checked; }

	private: System::ComponentModel::IContainer^ components;
	protected:

	private:
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->file_saveCp = (gcnew System::Windows::Forms::Button());
			this->copyPrevFrameButton = (gcnew System::Windows::Forms::Button());
			this->bindingSource1 = (gcnew System::Windows::Forms::BindingSource(this->components));
			this->upControllPointSize_button = (gcnew System::Windows::Forms::Button());
			this->downControllPoint_button = (gcnew System::Windows::Forms::Button());
			this->changeCpSize = (gcnew System::Windows::Forms::Label());
			this->changePathWidth = (gcnew System::Windows::Forms::Label());
			this->upPathWidth_button = (gcnew System::Windows::Forms::Button());
			this->downPathWidth_button = (gcnew System::Windows::Forms::Button());
			this->runCurrentFrameRGrow_button = (gcnew System::Windows::Forms::Button());
			this->finishSegmentation_button = (gcnew System::Windows::Forms::Button());
			this->formerAge_textBox = (gcnew System::Windows::Forms::TextBox());
			this->formerAge = (gcnew System::Windows::Forms::Label());
			this->latterAge = (gcnew System::Windows::Forms::Label());
			this->latterAge_textBox = (gcnew System::Windows::Forms::TextBox());
			this->file_loadCp = (gcnew System::Windows::Forms::Button());
			this->threshMin = (gcnew System::Windows::Forms::Label());
			this->threshMax_former = (gcnew System::Windows::Forms::Label());
			this->minThreshold_textBox = (gcnew System::Windows::Forms::TextBox());
			this->minThreshold_trackBar = (gcnew System::Windows::Forms::TrackBar());
			this->maxThresholdFormer_textBox = (gcnew System::Windows::Forms::TextBox());
			this->maxThresholdFormer_trackBar = (gcnew System::Windows::Forms::TrackBar());
			this->isRendTreeStructure = (gcnew System::Windows::Forms::CheckBox());
			this->isRendPath = (gcnew System::Windows::Forms::CheckBox());
			this->latterBLayer = (gcnew System::Windows::Forms::Label());
			this->latterLayerB_textBox = (gcnew System::Windows::Forms::TextBox());
			this->threshMax_latter = (gcnew System::Windows::Forms::Label());
			this->maxThresholdLatter_textBox = (gcnew System::Windows::Forms::TextBox());
			this->maxThresholdLatter_trackBar = (gcnew System::Windows::Forms::TrackBar());
			this->growth_label = (gcnew System::Windows::Forms::Label());
			this->regionGrowLabel = (gcnew System::Windows::Forms::Label());
			this->latterLayerRGrow_textBox = (gcnew System::Windows::Forms::TextBox());
			this->addLabel = (gcnew System::Windows::Forms::Label());
			this->moveLabel = (gcnew System::Windows::Forms::Label());
			this->ChangeParentLabel = (gcnew System::Windows::Forms::Label());
			this->deleteLabel = (gcnew System::Windows::Forms::Label());
			this->latterALabel = (gcnew System::Windows::Forms::Label());
			this->addCpButton = (gcnew System::Windows::Forms::RadioButton());
			this->moveCpButton = (gcnew System::Windows::Forms::RadioButton());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->bindingSource1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->minThreshold_trackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maxThresholdFormer_trackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maxThresholdLatter_trackBar))->BeginInit();
			this->SuspendLayout();
			// 
			// file_saveCp
			// 
			this->file_saveCp->Location = System::Drawing::Point(11, 379);
			this->file_saveCp->Name = L"file_saveCp";
			this->file_saveCp->Size = System::Drawing::Size(138, 41);
			this->file_saveCp->TabIndex = 0;
			this->file_saveCp->Text = L"Save CP";
			this->file_saveCp->UseVisualStyleBackColor = true;
			this->file_saveCp->Click += gcnew System::EventHandler(this, &FormSegBronchi::file_saveCp_Click);
			// 
			// copyPrevFrameButton
			// 
			this->copyPrevFrameButton->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->copyPrevFrameButton->Location = System::Drawing::Point(13, 446);
			this->copyPrevFrameButton->Name = L"copyPrevFrameButton";
			this->copyPrevFrameButton->Size = System::Drawing::Size(138, 38);
			this->copyPrevFrameButton->TabIndex = 4;
			this->copyPrevFrameButton->Text = L"Copy Prev frame CP";
			this->copyPrevFrameButton->UseVisualStyleBackColor = true;
			this->copyPrevFrameButton->Click += gcnew System::EventHandler(this, &FormSegBronchi::copyPrevFrameButton_Click);
			// 
			// upControllPointSize_button
			// 
			this->upControllPointSize_button->Location = System::Drawing::Point(11, 242);
			this->upControllPointSize_button->Name = L"upControllPointSize_button";
			this->upControllPointSize_button->Size = System::Drawing::Size(62, 29);
			this->upControllPointSize_button->TabIndex = 5;
			this->upControllPointSize_button->Text = L"Up";
			this->upControllPointSize_button->UseVisualStyleBackColor = true;
			this->upControllPointSize_button->Click += gcnew System::EventHandler(this, &FormSegBronchi::upControllPointSize_buttonClick);
			// 
			// downControllPoint_button
			// 
			this->downControllPoint_button->Location = System::Drawing::Point(79, 242);
			this->downControllPoint_button->Name = L"downControllPoint_button";
			this->downControllPoint_button->Size = System::Drawing::Size(61, 29);
			this->downControllPoint_button->TabIndex = 6;
			this->downControllPoint_button->Text = L"Down";
			this->downControllPoint_button->UseVisualStyleBackColor = true;
			this->downControllPoint_button->Click += gcnew System::EventHandler(this, &FormSegBronchi::downControllPointSize_buttonClick);
			// 
			// changeCpSize
			// 
			this->changeCpSize->AutoSize = true;
			this->changeCpSize->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->changeCpSize->Location = System::Drawing::Point(16, 225);
			this->changeCpSize->Name = L"changeCpSize";
			this->changeCpSize->Size = System::Drawing::Size(100, 14);
			this->changeCpSize->TabIndex = 7;
			this->changeCpSize->Text = L"Change CP Size";
			// 
			// changePathWidth
			// 
			this->changePathWidth->AutoSize = true;
			this->changePathWidth->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->changePathWidth->Location = System::Drawing::Point(16, 281);
			this->changePathWidth->Name = L"changePathWidth";
			this->changePathWidth->Size = System::Drawing::Size(118, 14);
			this->changePathWidth->TabIndex = 8;
			this->changePathWidth->Text = L"Change Path Width";
			// 
			// upPathWidth_button
			// 
			this->upPathWidth_button->Location = System::Drawing::Point(13, 298);
			this->upPathWidth_button->Name = L"upPathWidth_button";
			this->upPathWidth_button->Size = System::Drawing::Size(60, 27);
			this->upPathWidth_button->TabIndex = 9;
			this->upPathWidth_button->Text = L"Up";
			this->upPathWidth_button->UseVisualStyleBackColor = true;
			this->upPathWidth_button->Click += gcnew System::EventHandler(this, &FormSegBronchi::upPathWidth_buttonClick);
			// 
			// downPathWidth_button
			// 
			this->downPathWidth_button->Location = System::Drawing::Point(79, 298);
			this->downPathWidth_button->Name = L"downPathWidth_button";
			this->downPathWidth_button->Size = System::Drawing::Size(60, 27);
			this->downPathWidth_button->TabIndex = 10;
			this->downPathWidth_button->Text = L"Down";
			this->downPathWidth_button->UseVisualStyleBackColor = true;
			this->downPathWidth_button->Click += gcnew System::EventHandler(this, &FormSegBronchi::downPathWidth_buttonClick);
			// 
			// runCurrentFrameRGrow_button
			// 
			this->runCurrentFrameRGrow_button->Location = System::Drawing::Point(169, 419);
			this->runCurrentFrameRGrow_button->Name = L"runCurrentFrameRGrow_button";
			this->runCurrentFrameRGrow_button->Size = System::Drawing::Size(120, 65);
			this->runCurrentFrameRGrow_button->TabIndex = 11;
			this->runCurrentFrameRGrow_button->Text = L"Run Current Frame  Region Grow";
			this->runCurrentFrameRGrow_button->UseVisualStyleBackColor = true;
			this->runCurrentFrameRGrow_button->Click += gcnew System::EventHandler(this, &FormSegBronchi::runCurrentFrameRGrow_buttonClick);
			// 
			// finishSegmentation_button
			// 
			this->finishSegmentation_button->Location = System::Drawing::Point(306, 419);
			this->finishSegmentation_button->Name = L"finishSegmentation_button";
			this->finishSegmentation_button->Size = System::Drawing::Size(96, 65);
			this->finishSegmentation_button->TabIndex = 12;
			this->finishSegmentation_button->Text = L"Finish Segmentation";
			this->finishSegmentation_button->UseVisualStyleBackColor = true;
			this->finishSegmentation_button->Click += gcnew System::EventHandler(this, &FormSegBronchi::finishSegmentation_buttonClick);
			// 
			// formerAge_textBox
			// 
			this->formerAge_textBox->Location = System::Drawing::Point(333, 364);
			this->formerAge_textBox->Name = L"formerAge_textBox";
			this->formerAge_textBox->Size = System::Drawing::Size(38, 19);
			this->formerAge_textBox->TabIndex = 13;
			// 
			// formerAge
			// 
			this->formerAge->AutoSize = true;
			this->formerAge->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->formerAge->Location = System::Drawing::Point(166, 367);
			this->formerAge->Name = L"formerAge";
			this->formerAge->Size = System::Drawing::Size(162, 14);
			this->formerAge->TabIndex = 14;
			this->formerAge->Text = L"上部領域の初期拡張サイズ";
			// 
			// latterAge
			// 
			this->latterAge->AutoSize = true;
			this->latterAge->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->latterAge->Location = System::Drawing::Point(167, 388);
			this->latterAge->Name = L"latterAge";
			this->latterAge->Size = System::Drawing::Size(134, 14);
			this->latterAge->TabIndex = 16;
			this->latterAge->Text = L"下部領域の拡張サイズ";
			// 
			// latterAge_textBox
			// 
			this->latterAge_textBox->Location = System::Drawing::Point(333, 385);
			this->latterAge_textBox->Name = L"latterAge_textBox";
			this->latterAge_textBox->Size = System::Drawing::Size(38, 19);
			this->latterAge_textBox->TabIndex = 15;
			// 
			// file_loadCp
			// 
			this->file_loadCp->Location = System::Drawing::Point(11, 337);
			this->file_loadCp->Name = L"file_loadCp";
			this->file_loadCp->Size = System::Drawing::Size(138, 41);
			this->file_loadCp->TabIndex = 20;
			this->file_loadCp->Text = L"Load CP";
			this->file_loadCp->UseVisualStyleBackColor = true;
			this->file_loadCp->Click += gcnew System::EventHandler(this, &FormSegBronchi::file_loadCp_Click);
			// 
			// threshMin
			// 
			this->threshMin->AutoSize = true;
			this->threshMin->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->threshMin->Location = System::Drawing::Point(160, 155);
			this->threshMin->Name = L"threshMin";
			this->threshMin->Size = System::Drawing::Size(71, 14);
			this->threshMin->TabIndex = 22;
			this->threshMin->Text = L"Thresh Min";
			// 
			// threshMax_former
			// 
			this->threshMax_former->AutoSize = true;
			this->threshMax_former->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->threshMax_former->Location = System::Drawing::Point(160, 195);
			this->threshMax_former->Name = L"threshMax_former";
			this->threshMax_former->Size = System::Drawing::Size(113, 14);
			this->threshMax_former->TabIndex = 23;
			this->threshMax_former->Text = L"Thresh Max  1 - A";
			// 
			// minThreshold_textBox
			// 
			this->minThreshold_textBox->Location = System::Drawing::Point(167, 170);
			this->minThreshold_textBox->Name = L"minThreshold_textBox";
			this->minThreshold_textBox->Size = System::Drawing::Size(45, 19);
			this->minThreshold_textBox->TabIndex = 24;
			this->minThreshold_textBox->TextChanged += gcnew System::EventHandler(this, &FormSegBronchi::minThresholdTextBox_TextChanged);
			// 
			// minThreshold_trackBar
			// 
			this->minThreshold_trackBar->BackColor = System::Drawing::SystemColors::Control;
			this->minThreshold_trackBar->Location = System::Drawing::Point(212, 170);
			this->minThreshold_trackBar->Name = L"minThreshold_trackBar";
			this->minThreshold_trackBar->Size = System::Drawing::Size(199, 45);
			this->minThreshold_trackBar->TabIndex = 25;
			this->minThreshold_trackBar->TickStyle = System::Windows::Forms::TickStyle::None;
			this->minThreshold_trackBar->Scroll += gcnew System::EventHandler(this, &FormSegBronchi::minThreshold_Scroll);
			// 
			// maxThresholdFormer_textBox
			// 
			this->maxThresholdFormer_textBox->Location = System::Drawing::Point(167, 210);
			this->maxThresholdFormer_textBox->Name = L"maxThresholdFormer_textBox";
			this->maxThresholdFormer_textBox->Size = System::Drawing::Size(45, 19);
			this->maxThresholdFormer_textBox->TabIndex = 26;
			this->maxThresholdFormer_textBox->TextChanged += gcnew System::EventHandler(this, &FormSegBronchi::maxThresholdFormerTextBox_textChanged);
			// 
			// maxThresholdFormer_trackBar
			// 
			this->maxThresholdFormer_trackBar->BackColor = System::Drawing::SystemColors::Control;
			this->maxThresholdFormer_trackBar->Location = System::Drawing::Point(212, 210);
			this->maxThresholdFormer_trackBar->Name = L"maxThresholdFormer_trackBar";
			this->maxThresholdFormer_trackBar->Size = System::Drawing::Size(199, 45);
			this->maxThresholdFormer_trackBar->TabIndex = 27;
			this->maxThresholdFormer_trackBar->TickStyle = System::Windows::Forms::TickStyle::None;
			this->maxThresholdFormer_trackBar->Scroll += gcnew System::EventHandler(this, &FormSegBronchi::maxThresholdFormer_Scroll);
			// 
			// isRendTreeStructure
			// 
			this->isRendTreeStructure->AutoSize = true;
			this->isRendTreeStructure->Location = System::Drawing::Point(13, 127);
			this->isRendTreeStructure->Name = L"isRendTreeStructure";
			this->isRendTreeStructure->Size = System::Drawing::Size(98, 16);
			this->isRendTreeStructure->TabIndex = 28;
			this->isRendTreeStructure->Text = L"Tree Structure";
			this->isRendTreeStructure->UseVisualStyleBackColor = true;
			this->isRendTreeStructure->CheckedChanged += gcnew System::EventHandler(this, &FormSegBronchi::isRendTreeStructure_CheckedChange);
			// 
			// isRendPath
			// 
			this->isRendPath->AutoSize = true;
			this->isRendPath->Checked = true;
			this->isRendPath->CheckState = System::Windows::Forms::CheckState::Checked;
			this->isRendPath->Location = System::Drawing::Point(13, 106);
			this->isRendPath->Name = L"isRendPath";
			this->isRendPath->Size = System::Drawing::Size(47, 16);
			this->isRendPath->TabIndex = 29;
			this->isRendPath->Text = L"Path";
			this->isRendPath->UseVisualStyleBackColor = true;
			this->isRendPath->CheckedChanged += gcnew System::EventHandler(this, &FormSegBronchi::isRendPath_CheckedChange);
			// 
			// latterBLayer
			// 
			this->latterBLayer->AutoSize = true;
			this->latterBLayer->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->latterBLayer->Location = System::Drawing::Point(165, 347);
			this->latterBLayer->Name = L"latterBLayer";
			this->latterBLayer->Size = System::Drawing::Size(70, 14);
			this->latterBLayer->TabIndex = 31;
			this->latterBLayer->Text = L"境界の深さ";
			// 
			// latterLayerB_textBox
			// 
			this->latterLayerB_textBox->Location = System::Drawing::Point(333, 343);
			this->latterLayerB_textBox->Name = L"latterLayerB_textBox";
			this->latterLayerB_textBox->Size = System::Drawing::Size(22, 19);
			this->latterLayerB_textBox->TabIndex = 32;
			// 
			// threshMax_latter
			// 
			this->threshMax_latter->AutoSize = true;
			this->threshMax_latter->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->threshMax_latter->Location = System::Drawing::Point(160, 239);
			this->threshMax_latter->Name = L"threshMax_latter";
			this->threshMax_latter->Size = System::Drawing::Size(119, 14);
			this->threshMax_latter->TabIndex = 33;
			this->threshMax_latter->Text = L"Thresh Max  A以降";
			// 
			// maxThresholdLatter_textBox
			// 
			this->maxThresholdLatter_textBox->Location = System::Drawing::Point(167, 254);
			this->maxThresholdLatter_textBox->Name = L"maxThresholdLatter_textBox";
			this->maxThresholdLatter_textBox->Size = System::Drawing::Size(45, 19);
			this->maxThresholdLatter_textBox->TabIndex = 34;
			this->maxThresholdLatter_textBox->TextChanged += gcnew System::EventHandler(this, &FormSegBronchi::maxThresholdLatterTextBox_textChanged);
			// 
			// maxThresholdLatter_trackBar
			// 
			this->maxThresholdLatter_trackBar->BackColor = System::Drawing::SystemColors::Control;
			this->maxThresholdLatter_trackBar->Location = System::Drawing::Point(212, 254);
			this->maxThresholdLatter_trackBar->Name = L"maxThresholdLatter_trackBar";
			this->maxThresholdLatter_trackBar->Size = System::Drawing::Size(199, 45);
			this->maxThresholdLatter_trackBar->TabIndex = 35;
			this->maxThresholdLatter_trackBar->TickStyle = System::Windows::Forms::TickStyle::None;
			this->maxThresholdLatter_trackBar->Scroll += gcnew System::EventHandler(this, &FormSegBronchi::maxThresholdLatter_Scroll);
			// 
			// growth_label
			// 
			this->growth_label->AutoSize = true;
			this->growth_label->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12, System::Drawing::FontStyle::Bold));
			this->growth_label->Location = System::Drawing::Point(165, 321);
			this->growth_label->Name = L"growth_label";
			this->growth_label->Size = System::Drawing::Size(83, 16);
			this->growth_label->TabIndex = 36;
			this->growth_label->Text = L"拡張サイズ";
			// 
			// regionGrowLabel
			// 
			this->regionGrowLabel->AutoSize = true;
			this->regionGrowLabel->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->regionGrowLabel->Location = System::Drawing::Point(160, 112);
			this->regionGrowLabel->Name = L"regionGrowLabel";
			this->regionGrowLabel->Size = System::Drawing::Size(103, 16);
			this->regionGrowLabel->TabIndex = 37;
			this->regionGrowLabel->Text = L"Region Grow";
			// 
			// latterLayerRGrow_textBox
			// 
			this->latterLayerRGrow_textBox->Location = System::Drawing::Point(265, 133);
			this->latterLayerRGrow_textBox->Name = L"latterLayerRGrow_textBox";
			this->latterLayerRGrow_textBox->Size = System::Drawing::Size(22, 19);
			this->latterLayerRGrow_textBox->TabIndex = 38;
			// 
			// addLabel
			// 
			this->addLabel->AutoSize = true;
			this->addLabel->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Bold));
			this->addLabel->Location = System::Drawing::Point(5, 9);
			this->addLabel->Name = L"addLabel";
			this->addLabel->Size = System::Drawing::Size(275, 14);
			this->addLabel->TabIndex = 39;
			this->addLabel->Text = L"Shift + L-click : ADD Control Point(CP)";
			// 
			// moveLabel
			// 
			this->moveLabel->AutoSize = true;
			this->moveLabel->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Bold));
			this->moveLabel->Location = System::Drawing::Point(5, 30);
			this->moveLabel->Name = L"moveLabel";
			this->moveLabel->Size = System::Drawing::Size(183, 14);
			this->moveLabel->TabIndex = 40;
			this->moveLabel->Text = L"Shift + L-drag : MOVE CP";
			// 
			// ChangeParentLabel
			// 
			this->ChangeParentLabel->AutoSize = true;
			this->ChangeParentLabel->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Bold));
			this->ChangeParentLabel->Location = System::Drawing::Point(5, 52);
			this->ChangeParentLabel->Name = L"ChangeParentLabel";
			this->ChangeParentLabel->Size = System::Drawing::Size(256, 14);
			this->ChangeParentLabel->TabIndex = 41;
			this->ChangeParentLabel->Text = L"Shift + R-click : CHANGE Parent CP";
			// 
			// deleteLabel
			// 
			this->deleteLabel->AutoSize = true;
			this->deleteLabel->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Bold));
			this->deleteLabel->Location = System::Drawing::Point(5, 74);
			this->deleteLabel->Name = L"deleteLabel";
			this->deleteLabel->Size = System::Drawing::Size(244, 14);
			this->deleteLabel->TabIndex = 42;
			this->deleteLabel->Text = L"Ctrl + Shift + R-click : DELETE CP";
			// 
			// latterALabel
			// 
			this->latterALabel->AutoSize = true;
			this->latterALabel->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->latterALabel->Location = System::Drawing::Point(164, 135);
			this->latterALabel->Name = L"latterALabel";
			this->latterALabel->Size = System::Drawing::Size(94, 14);
			this->latterALabel->TabIndex = 43;
			this->latterALabel->Text = L"後半レイヤー：A";
			// 
			// addCpButton
			// 
			this->addCpButton->AutoSize = true;
			this->addCpButton->Checked = true;
			this->addCpButton->Location = System::Drawing::Point(13, 167);
			this->addCpButton->Name = L"addCpButton";
			this->addCpButton->Size = System::Drawing::Size(92, 16);
			this->addCpButton->TabIndex = 44;
			this->addCpButton->TabStop = true;
			this->addCpButton->Text = L"Add Cp Mode";
			this->addCpButton->UseVisualStyleBackColor = true;
			// 
			// moveCpButton
			// 
			this->moveCpButton->AutoSize = true;
			this->moveCpButton->Location = System::Drawing::Point(13, 189);
			this->moveCpButton->Name = L"moveCpButton";
			this->moveCpButton->Size = System::Drawing::Size(99, 16);
			this->moveCpButton->TabIndex = 45;
			this->moveCpButton->Text = L"Move Cp Mode";
			this->moveCpButton->UseVisualStyleBackColor = true;
			this->moveCpButton->CheckedChanged += gcnew System::EventHandler(this, &FormSegBronchi::MoveCpMode_CheckedChanged);
			// 
			// FormSegBronchi
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(414, 489);
			this->Controls->Add(this->moveCpButton);
			this->Controls->Add(this->addCpButton);
			this->Controls->Add(this->latterALabel);
			this->Controls->Add(this->deleteLabel);
			this->Controls->Add(this->ChangeParentLabel);
			this->Controls->Add(this->moveLabel);
			this->Controls->Add(this->addLabel);
			this->Controls->Add(this->latterLayerRGrow_textBox);
			this->Controls->Add(this->regionGrowLabel);
			this->Controls->Add(this->growth_label);
			this->Controls->Add(this->maxThresholdLatter_textBox);
			this->Controls->Add(this->threshMax_latter);
			this->Controls->Add(this->latterLayerB_textBox);
			this->Controls->Add(this->latterBLayer);
			this->Controls->Add(this->isRendPath);
			this->Controls->Add(this->isRendTreeStructure);
			this->Controls->Add(this->maxThresholdFormer_trackBar);
			this->Controls->Add(this->maxThresholdFormer_textBox);
			this->Controls->Add(this->minThreshold_textBox);
			this->Controls->Add(this->threshMax_former);
			this->Controls->Add(this->threshMin);
			this->Controls->Add(this->file_loadCp);
			this->Controls->Add(this->latterAge);
			this->Controls->Add(this->latterAge_textBox);
			this->Controls->Add(this->formerAge);
			this->Controls->Add(this->formerAge_textBox);
			this->Controls->Add(this->finishSegmentation_button);
			this->Controls->Add(this->runCurrentFrameRGrow_button);
			this->Controls->Add(this->downPathWidth_button);
			this->Controls->Add(this->upPathWidth_button);
			this->Controls->Add(this->changePathWidth);
			this->Controls->Add(this->changeCpSize);
			this->Controls->Add(this->downControllPoint_button);
			this->Controls->Add(this->upControllPointSize_button);
			this->Controls->Add(this->copyPrevFrameButton);
			this->Controls->Add(this->file_saveCp);
			this->Controls->Add(this->minThreshold_trackBar);
			this->Controls->Add(this->maxThresholdLatter_trackBar);
			this->Name = L"FormSegBronchi";
			this->Text = L"FormSegBronchi";
			this->Load += gcnew System::EventHandler(this, &FormSegBronchi::FormSegBronchi_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->bindingSource1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->minThreshold_trackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maxThresholdFormer_trackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maxThresholdLatter_trackBar))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void FormSegBronchi_Load(System::Object^ sender, System::EventArgs^ e) {
	}
  private: System::Void file_loadCp_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void file_saveCp_Click(System::Object^ sender, System::EventArgs^ e);

  private: System::Void copyPrevFrameButton_Click(System::Object^ sender, System::EventArgs^ e);
  
  private: System::Void upControllPointSize_buttonClick(System::Object^ sender, System::EventArgs^ e);
  private: System::Void downControllPointSize_buttonClick(System::Object^ sender, System::EventArgs^ e);

	private: System::Void upPathWidth_buttonClick(System::Object^ sender, System::EventArgs^ e);
  private: System::Void downPathWidth_buttonClick(System::Object^ sender, System::EventArgs^ e);
  private: System::Void runCurrentFrameRGrow_buttonClick(System::Object^ sender, System::EventArgs^ e);
  private: System::Void finishSegmentation_buttonClick(System::Object^ sender, System::EventArgs^ e);

  private: System::Void minThresholdTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e);
	private: System::Void maxThresholdFormerTextBox_textChanged(System::Object^ sender, System::EventArgs^ e);
  private: System::Void maxThresholdLatterTextBox_textChanged(System::Object^ sender, System::EventArgs^ e);
  private: System::Void minThreshold_Scroll(System::Object^ sender, System::EventArgs^ e);
	private: System::Void maxThresholdFormer_Scroll(System::Object^ sender, System::EventArgs^ e);
	private: System::Void maxThresholdLatter_Scroll(System::Object^ sender, System::EventArgs^ e);

  private: System::Void isRendTreeStructure_CheckedChange(System::Object^ sender, System::EventArgs^ e);
  private: System::Void isRendPath_CheckedChange(System::Object^ sender, System::EventArgs^ e);
  private: System::Void MoveCpMode_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
};

  inline void formSegBronchi_Show() { FormSegBronchi::GetInst()->Show(); }
  inline void formSegBronchi_Hide() { FormSegBronchi::GetInst()->Hide(); }
	inline void formSegBronchi_InitAllItems(int vol_min, int vol_max) { FormSegBronchi::GetInst()->InitAllItems(vol_min, vol_max); }
	inline bool formSegBronchi_GetisRendTreeStructure() { return FormSegBronchi::GetInst()->GetIsRendTreeStructure(); }
	inline bool formSegBronchi_GetisRendPath() { return FormSegBronchi::GetInst()->GetIsRendPath(); }
	inline bool formSegBronchi_GetIsAddCpMode() { return FormSegBronchi::GetInst()->GetIsAddCpMode();}
	inline bool formSegBronchi_GetIsMoveCpMode() { return FormSegBronchi::GetInst()->GetIsMoveCpMode();}
}
