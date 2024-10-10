#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegRGrow の概要
	/// </summary>
	public ref class FormSegRGrow : public System::Windows::Forms::Form
	{
  private:
    static FormSegRGrow^ m_singleton;
    FormSegRGrow(void)
		{
			InitializeComponent();
		}
  public:
    static FormSegRGrow^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormSegRGrow();
      return m_singleton;
    }

    void InitAllItems(short volMin, short volMax);
    bool DoLimitIteration(){ return checkBox_limititeration->Checked; }
    int  GetMaxIteration() { return trackBar_maxiteration->Value; } 

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormSegRGrow()
		{
			if (components)
			{
				delete components;
			}
		}

  private: 
    System::Windows::Forms::Label^  label1;
    System::Windows::Forms::Label^  label2;
    System::Windows::Forms::Label^  label3;
    System::Windows::Forms::Label^  label4;
    System::Windows::Forms::TextBox^  textBox_min;
    System::Windows::Forms::TextBox^  textBox_max;
    System::Windows::Forms::TrackBar^  trackbar_min;
    System::Windows::Forms::TrackBar^  trackbar_max;
    System::Windows::Forms::GroupBox^  groupBox;
    System::Windows::Forms::Button^  btn_thresholding;
    System::Windows::Forms::Button^  btn_rgrow8;
    System::Windows::Forms::Button^  btn_rgrow26;
    System::Windows::Forms::Button^  btn_erode;
    System::Windows::Forms::Button^  btn_fillhole;
    System::Windows::Forms::Button^  btn_dilate;
    System::Windows::Forms::Button^  btn_finish;
    System::Windows::Forms::CheckBox^  checkBox_limititeration;
    System::Windows::Forms::TrackBar^  trackBar_maxiteration;
    System::Windows::Forms::TextBox^  textBox_maxiteration;


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
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->trackbar_min = (gcnew System::Windows::Forms::TrackBar());
      this->trackbar_max = (gcnew System::Windows::Forms::TrackBar());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->label4 = (gcnew System::Windows::Forms::Label());
      this->textBox_min = (gcnew System::Windows::Forms::TextBox());
      this->textBox_max = (gcnew System::Windows::Forms::TextBox());
      this->groupBox = (gcnew System::Windows::Forms::GroupBox());
      this->btn_thresholding = (gcnew System::Windows::Forms::Button());
      this->btn_rgrow8 = (gcnew System::Windows::Forms::Button());
      this->btn_rgrow26 = (gcnew System::Windows::Forms::Button());
      this->btn_erode = (gcnew System::Windows::Forms::Button());
      this->btn_fillhole = (gcnew System::Windows::Forms::Button());
      this->btn_dilate = (gcnew System::Windows::Forms::Button());
      this->btn_finish = (gcnew System::Windows::Forms::Button());
      this->checkBox_limititeration = (gcnew System::Windows::Forms::CheckBox());
      this->trackBar_maxiteration = (gcnew System::Windows::Forms::TrackBar());
      this->textBox_maxiteration = (gcnew System::Windows::Forms::TextBox());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_min))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_max))->BeginInit();
      this->groupBox->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_maxiteration))->BeginInit();
      this->SuspendLayout();
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label1->Location = System::Drawing::Point(15, 9);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(191, 20);
      this->label1->TabIndex = 0;
      this->label1->Text = L"L-dblclik : Place/delete Seed";
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label2->Location = System::Drawing::Point(15, 29);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(210, 20);
      this->label2->TabIndex = 1;
      this->label2->Text = L"L-drag   : select and move seed";
      // 
      // trackbar_min
      // 
      this->trackbar_min->Location = System::Drawing::Point(12, 39);
      this->trackbar_min->Name = L"trackbar_min";
      this->trackbar_min->Size = System::Drawing::Size(206, 45);
      this->trackbar_min->TabIndex = 2;
      this->trackbar_min->TickStyle = System::Windows::Forms::TickStyle::None;
      this->trackbar_min->Scroll += gcnew System::EventHandler(this, &FormSegRGrow::trackbar_min_Scroll);
      // 
      // trackbar_max
      // 
      this->trackbar_max->Location = System::Drawing::Point(12, 90);
      this->trackbar_max->Name = L"trackbar_max";
      this->trackbar_max->Size = System::Drawing::Size(206, 45);
      this->trackbar_max->TabIndex = 3;
      this->trackbar_max->TickStyle = System::Windows::Forms::TickStyle::None;
      this->trackbar_max->Scroll += gcnew System::EventHandler(this, &FormSegRGrow::trackBar_max_Scroll);
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label3->Location = System::Drawing::Point(8, 18);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(32, 20);
      this->label3->TabIndex = 1;
      this->label3->Text = L"min";
      // 
      // label4
      // 
      this->label4->AutoSize = true;
      this->label4->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label4->Location = System::Drawing::Point(8, 69);
      this->label4->Name = L"label4";
      this->label4->Size = System::Drawing::Size(36, 20);
      this->label4->TabIndex = 4;
      this->label4->Text = L"max";
      // 
      // textBox_min
      // 
      this->textBox_min->Location = System::Drawing::Point(46, 18);
      this->textBox_min->Name = L"textBox_min";
      this->textBox_min->Size = System::Drawing::Size(54, 19);
      this->textBox_min->TabIndex = 5;
      this->textBox_min->TextChanged += gcnew System::EventHandler(this, &FormSegRGrow::textBox_min_TextChanged);
      // 
      // textBox_max
      // 
      this->textBox_max->Location = System::Drawing::Point(46, 69);
      this->textBox_max->Name = L"textBox_max";
      this->textBox_max->Size = System::Drawing::Size(54, 19);
      this->textBox_max->TabIndex = 6;
      this->textBox_max->TextChanged += gcnew System::EventHandler(this, &FormSegRGrow::textBox_max_TextChanged);
      // 
      // groupBox
      // 
      this->groupBox->Controls->Add(this->textBox_min);
      this->groupBox->Controls->Add(this->textBox_max);
      this->groupBox->Controls->Add(this->label3);
      this->groupBox->Controls->Add(this->label4);
      this->groupBox->Controls->Add(this->trackbar_min);
      this->groupBox->Controls->Add(this->trackbar_max);
      this->groupBox->Location = System::Drawing::Point(6, 61);
      this->groupBox->Name = L"groupBox";
      this->groupBox->Size = System::Drawing::Size(245, 124);
      this->groupBox->TabIndex = 7;
      this->groupBox->TabStop = false;
      this->groupBox->Text = L"Threshold (閾値)";
      // 
      // btn_thresholding
      // 
      this->btn_thresholding->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_thresholding->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
      this->btn_thresholding->Location = System::Drawing::Point(8, 195);
      this->btn_thresholding->Name = L"btn_thresholding";
      this->btn_thresholding->Size = System::Drawing::Size(247, 28);
      this->btn_thresholding->TabIndex = 8;
      this->btn_thresholding->Text = L"run thresholding";
      this->btn_thresholding->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
      this->btn_thresholding->UseVisualStyleBackColor = true;
      this->btn_thresholding->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_thresholding_Click);
      // 
      // btn_rgrow8
      // 
      this->btn_rgrow8->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_rgrow8->Location = System::Drawing::Point(8, 224);
      this->btn_rgrow8->Name = L"btn_rgrow8";
      this->btn_rgrow8->Size = System::Drawing::Size(247, 28);
      this->btn_rgrow8->TabIndex = 9;
      this->btn_rgrow8->Text = L"run region growing 6-neighbors";
      this->btn_rgrow8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
      this->btn_rgrow8->UseVisualStyleBackColor = true;
      this->btn_rgrow8->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_rgrow6_Click);
      // 
      // btn_rgrow26
      // 
      this->btn_rgrow26->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_rgrow26->Location = System::Drawing::Point(8, 253);
      this->btn_rgrow26->Name = L"btn_rgrow26";
      this->btn_rgrow26->Size = System::Drawing::Size(247, 28);
      this->btn_rgrow26->TabIndex = 10;
      this->btn_rgrow26->Text = L"run region growing 26-neighbors";
      this->btn_rgrow26->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
      this->btn_rgrow26->UseVisualStyleBackColor = true;
      this->btn_rgrow26->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_rgrow26_Click);
      // 
      // btn_erode
      // 
      this->btn_erode->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_erode->Location = System::Drawing::Point(34, 343);
      this->btn_erode->Name = L"btn_erode";
      this->btn_erode->Size = System::Drawing::Size(69, 28);
      this->btn_erode->TabIndex = 11;
      this->btn_erode->Text = L"erode";
      this->btn_erode->UseVisualStyleBackColor = true;
      this->btn_erode->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_erode_Click);
      // 
      // btn_fillhole
      // 
      this->btn_fillhole->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_fillhole->Location = System::Drawing::Point(184, 343);
      this->btn_fillhole->Name = L"btn_fillhole";
      this->btn_fillhole->Size = System::Drawing::Size(69, 28);
      this->btn_fillhole->TabIndex = 12;
      this->btn_fillhole->Text = L"fillhole";
      this->btn_fillhole->UseVisualStyleBackColor = true;
      this->btn_fillhole->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_fillhole_Click);
      // 
      // btn_dilate
      // 
      this->btn_dilate->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_dilate->Location = System::Drawing::Point(109, 343);
      this->btn_dilate->Name = L"btn_dilate";
      this->btn_dilate->Size = System::Drawing::Size(69, 28);
      this->btn_dilate->TabIndex = 13;
      this->btn_dilate->Text = L"dilate";
      this->btn_dilate->UseVisualStyleBackColor = true;
      this->btn_dilate->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_dilate_Click);
      // 
      // btn_finish
      // 
      this->btn_finish->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_finish->Location = System::Drawing::Point(89, 380);
      this->btn_finish->Name = L"btn_finish";
      this->btn_finish->Size = System::Drawing::Size(162, 28);
      this->btn_finish->TabIndex = 14;
      this->btn_finish->Text = L"End ＆ Store Region";
      this->btn_finish->UseVisualStyleBackColor = true;
      this->btn_finish->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_finish_Click);
      // 
      // checkBox_limititeration
      // 
      this->checkBox_limititeration->AutoSize = true;
      this->checkBox_limititeration->Location = System::Drawing::Point(8, 297);
      this->checkBox_limititeration->Name = L"checkBox_limititeration";
      this->checkBox_limititeration->Size = System::Drawing::Size(133, 16);
      this->checkBox_limititeration->TabIndex = 15;
      this->checkBox_limititeration->Text = L"Limit growth iteration";
      this->checkBox_limititeration->UseVisualStyleBackColor = true;
      // 
      // trackBar_maxiteration
      // 
      this->trackBar_maxiteration->Location = System::Drawing::Point(64, 314);
      this->trackBar_maxiteration->Name = L"trackBar_maxiteration";
      this->trackBar_maxiteration->Size = System::Drawing::Size(187, 45);
      this->trackBar_maxiteration->TabIndex = 7;
      this->trackBar_maxiteration->TickStyle = System::Windows::Forms::TickStyle::None;
      this->trackBar_maxiteration->Scroll += gcnew System::EventHandler(this, &FormSegRGrow::trackBar_maxiteration_Scroll);
      // 
      // textBox_maxiteration
      // 
      this->textBox_maxiteration->Location = System::Drawing::Point(6, 315);
      this->textBox_maxiteration->Name = L"textBox_maxiteration";
      this->textBox_maxiteration->ReadOnly = true;
      this->textBox_maxiteration->Size = System::Drawing::Size(54, 19);
      this->textBox_maxiteration->TabIndex = 7;
      // 
      // FormSegRGrow
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(263, 424);
      this->Controls->Add(this->textBox_maxiteration);
      this->Controls->Add(this->checkBox_limititeration);
      this->Controls->Add(this->groupBox);
      this->Controls->Add(this->btn_finish);
      this->Controls->Add(this->btn_dilate);
      this->Controls->Add(this->btn_fillhole);
      this->Controls->Add(this->btn_erode);
      this->Controls->Add(this->btn_rgrow26);
      this->Controls->Add(this->btn_rgrow8);
      this->Controls->Add(this->btn_thresholding);
      this->Controls->Add(this->label2);
      this->Controls->Add(this->label1);
      this->Controls->Add(this->trackBar_maxiteration);
      this->Name = L"FormSegRGrow";
      this->Text = L"FormSegRGrow";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_min))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_max))->EndInit();
      this->groupBox->ResumeLayout(false);
      this->groupBox->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_maxiteration))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
private: 
  System::Void textBox_min_TextChanged(System::Object^  sender, System::EventArgs^  e) ;
  System::Void trackbar_min_Scroll(System::Object^  sender, System::EventArgs^  e) ;
  System::Void textBox_max_TextChanged(System::Object^  sender, System::EventArgs^  e); 
  System::Void trackBar_max_Scroll(System::Object^  sender, System::EventArgs^  e) ;
  System::Void btn_thresholding_Click(System::Object^  sender, System::EventArgs^  e) ;
  System::Void btn_rgrow6_Click(System::Object^  sender, System::EventArgs^  e) ;
  System::Void btn_rgrow26_Click(System::Object^  sender, System::EventArgs^  e) ;
  System::Void btn_erode_Click(System::Object^  sender, System::EventArgs^  e) ;
  System::Void btn_dilate_Click(System::Object^  sender, System::EventArgs^  e) ;
  System::Void btn_fillhole_Click(System::Object^  sender, System::EventArgs^  e); 
  System::Void btn_finish_Click(System::Object^  sender, System::EventArgs^  e) ;
  System::Void trackBar_maxiteration_Scroll(System::Object^  sender, System::EventArgs^  e) ;
  };

  inline void formSegRGrow_Show() { FormSegRGrow::getInst()->Show(); }
  inline void formSegRGrow_Hide() { FormSegRGrow::getInst()->Hide(); }
  inline void formSegRGrow_InitAllItems(short volMin, short volMax) { FormSegRGrow::getInst()->InitAllItems(volMin, volMax); }

  inline bool formSetRGrow_DoLimitIteration(){ return FormSegRGrow::getInst()->DoLimitIteration(); }
  inline int  formSetRGrow_GetMaxIteration() { return FormSegRGrow::getInst()->GetMaxIteration(); } 

}
