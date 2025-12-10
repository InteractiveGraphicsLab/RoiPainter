#pragma once

namespace RoiPainter4D {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  /// <summary>
  /// FormRefPixPaint の概要
  /// </summary>
  public ref class FormRefPixPaint : public System::Windows::Forms::Form
  {
  private:
    bool m_item_updating;

    System::Windows::Forms::Label^ label1;
    System::Windows::Forms::Button^ btn_ok;
    System::Windows::Forms::Button^ btn_cancel;
    System::Windows::Forms::GroupBox^ groupBox1;
    System::Windows::Forms::Label^ label2;

  private: System::Windows::Forms::Label^ minThreshLabel;
  private: System::Windows::Forms::TextBox^ maxThreshTextBox;
  private: System::Windows::Forms::Label^ maxThreshLabel;
  private: System::Windows::Forms::TextBox^ minThreshTextBox;
  private: System::Windows::Forms::TrackBar^ minThresholdBar;
  private: System::Windows::Forms::TrackBar^ maxThresholdBar;
  private: System::Windows::Forms::RadioButton^ thresholdRadioButton;

  private: System::Windows::Forms::RadioButton^ eraseRadioButton;

  private: System::Windows::Forms::RadioButton^ paintRadioButton;
  private: System::Windows::Forms::CheckBox^ m_checkbox_forcedoverwrite;
  private: System::Windows::Forms::GroupBox^ groupBox2;
  private: System::Windows::Forms::Label^ label3;
  private: System::Windows::Forms::GroupBox^ groupBox3;
  private: System::Windows::Forms::GroupBox^ groupBox4;

         System::Windows::Forms::CheckBox^ m_checkbox_userough;

  private:
    FormRefPixPaint(void)
    {
      InitializeComponent();
      m_item_updating = false;
      this->ControlBox = false;
      this->MaximizeBox = false;
    }

    static FormRefPixPaint^ m_singleton;
  public:
    static FormRefPixPaint^ GetInst() {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormRefPixPaint();
      }
      return m_singleton;
    }

    bool DoVisSurf() { return m_checkbox_userough->Checked; }
    bool bForcedOverwite() { return m_checkbox_forcedoverwrite->Checked; }

  public:
    void InitAllItems(int volmin, int volmax);
    bool bPaintMode() { return paintRadioButton->Checked; }
    bool bEraseMode() { return eraseRadioButton->Checked; }
    bool bThresholdMode() { return thresholdRadioButton->Checked; }
    short maxthreshold() { return maxThresholdBar->Value; }
    short minthreshold() { return minThresholdBar->Value; }

  protected:
    /// <summary>
    /// 使用中のリソースをすべてクリーンアップします。
    /// </summary>
    ~FormRefPixPaint()
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
    System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
    /// <summary>
    /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
    /// コード エディターで変更しないでください。
    /// </summary>
    void InitializeComponent(void)
    {
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->btn_ok = (gcnew System::Windows::Forms::Button());
      this->btn_cancel = (gcnew System::Windows::Forms::Button());
      this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
      this->m_checkbox_userough = (gcnew System::Windows::Forms::CheckBox());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->minThreshLabel = (gcnew System::Windows::Forms::Label());
      this->maxThreshTextBox = (gcnew System::Windows::Forms::TextBox());
      this->maxThreshLabel = (gcnew System::Windows::Forms::Label());
      this->minThreshTextBox = (gcnew System::Windows::Forms::TextBox());
      this->minThresholdBar = (gcnew System::Windows::Forms::TrackBar());
      this->maxThresholdBar = (gcnew System::Windows::Forms::TrackBar());
      this->thresholdRadioButton = (gcnew System::Windows::Forms::RadioButton());
      this->eraseRadioButton = (gcnew System::Windows::Forms::RadioButton());
      this->paintRadioButton = (gcnew System::Windows::Forms::RadioButton());
      this->m_checkbox_forcedoverwrite = (gcnew System::Windows::Forms::CheckBox());
      this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
      this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
      this->groupBox1->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->minThresholdBar))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maxThresholdBar))->BeginInit();
      this->groupBox2->SuspendLayout();
      this->groupBox3->SuspendLayout();
      this->groupBox4->SuspendLayout();
      this->SuspendLayout();
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label1->Location = System::Drawing::Point(13, 29);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(415, 110);
      this->label1->TabIndex = 0;
      this->label1->Text = L"[Shift L-drag] : draw lasso.\r\n\r\n[R click(mouse)] cancel current stroke.\r\n\r\n[r key"
        L"] apply previous lasso to current slice.";
      this->label1->Click += gcnew System::EventHandler(this, &FormRefPixPaint::label1_Click);
      // 
      // btn_ok
      // 
      this->btn_ok->Location = System::Drawing::Point(22, 803);
      this->btn_ok->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->btn_ok->Name = L"btn_ok";
      this->btn_ok->Size = System::Drawing::Size(218, 75);
      this->btn_ok->TabIndex = 1;
      this->btn_ok->Text = L"Finish and Store";
      this->btn_ok->UseVisualStyleBackColor = true;
      this->btn_ok->Click += gcnew System::EventHandler(this, &FormRefPixPaint::btn_ok_Click);
      // 
      // btn_cancel
      // 
      this->btn_cancel->Location = System::Drawing::Point(317, 803);
      this->btn_cancel->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->btn_cancel->Name = L"btn_cancel";
      this->btn_cancel->Size = System::Drawing::Size(133, 75);
      this->btn_cancel->TabIndex = 2;
      this->btn_cancel->Text = L"Cancel";
      this->btn_cancel->UseVisualStyleBackColor = true;
      this->btn_cancel->Click += gcnew System::EventHandler(this, &FormRefPixPaint::btn_cancel_Click);
      // 
      // groupBox1
      // 
      this->groupBox1->Controls->Add(this->m_checkbox_userough);
      this->groupBox1->Controls->Add(this->label2);
      this->groupBox1->Location = System::Drawing::Point(22, 525);
      this->groupBox1->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->groupBox1->Name = L"groupBox1";
      this->groupBox1->Padding = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->groupBox1->Size = System::Drawing::Size(462, 270);
      this->groupBox1->TabIndex = 3;
      this->groupBox1->TabStop = false;
      this->groupBox1->Text = L"Other";
      // 
      // m_checkbox_userough
      // 
      this->m_checkbox_userough->AutoSize = true;
      this->m_checkbox_userough->Location = System::Drawing::Point(10, 228);
      this->m_checkbox_userough->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->m_checkbox_userough->Name = L"m_checkbox_userough";
      this->m_checkbox_userough->Size = System::Drawing::Size(230, 22);
      this->m_checkbox_userough->TabIndex = 1;
      this->m_checkbox_userough->Text = L"Render Rough while manip";
      this->m_checkbox_userough->UseVisualStyleBackColor = true;
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label2->Location = System::Drawing::Point(8, 22);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(319, 180);
      this->label2->TabIndex = 0;
      this->label2->Text = L"[Ctrl z key] undo operation.\r\n\r\n[Tab key] CT image On/Off.\r\n\r\n[s key] hide mask.\r"
        L"\n\r\n[d key] hide previous stroke.\r\n\r\n[Space key] hide volume temporarily.";
      this->label2->Click += gcnew System::EventHandler(this, &FormRefPixPaint::label1_Click);
      // 
      // minThreshLabel
      // 
      this->minThreshLabel->AutoSize = true;
      this->minThreshLabel->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->minThreshLabel->Location = System::Drawing::Point(12, 29);
      this->minThreshLabel->Name = L"minThreshLabel";
      this->minThreshLabel->Size = System::Drawing::Size(48, 26);
      this->minThreshLabel->TabIndex = 23;
      this->minThreshLabel->Text = L"min";
      // 
      // maxThreshTextBox
      // 
      this->maxThreshTextBox->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9));
      this->maxThreshTextBox->Location = System::Drawing::Point(71, 64);
      this->maxThreshTextBox->Name = L"maxThreshTextBox";
      this->maxThreshTextBox->Size = System::Drawing::Size(74, 25);
      this->maxThreshTextBox->TabIndex = 22;
      this->maxThreshTextBox->TextChanged += gcnew System::EventHandler(this, &FormRefPixPaint::maxThreshTextBox_TextChanged);
      // 
      // maxThreshLabel
      // 
      this->maxThreshLabel->AutoSize = true;
      this->maxThreshLabel->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->maxThreshLabel->Location = System::Drawing::Point(12, 62);
      this->maxThreshLabel->Name = L"maxThreshLabel";
      this->maxThreshLabel->Size = System::Drawing::Size(53, 26);
      this->maxThreshLabel->TabIndex = 24;
      this->maxThreshLabel->Text = L"max";
      // 
      // minThreshTextBox
      // 
      this->minThreshTextBox->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9));
      this->minThreshTextBox->Location = System::Drawing::Point(71, 31);
      this->minThreshTextBox->Name = L"minThreshTextBox";
      this->minThreshTextBox->Size = System::Drawing::Size(74, 25);
      this->minThreshTextBox->TabIndex = 21;
      this->minThreshTextBox->TextChanged += gcnew System::EventHandler(this, &FormRefPixPaint::minThreshTextBox_TextChanged);
      // 
      // minThresholdBar
      // 
      this->minThresholdBar->Location = System::Drawing::Point(151, 31);
      this->minThresholdBar->Name = L"minThresholdBar";
      this->minThresholdBar->Size = System::Drawing::Size(277, 69);
      this->minThresholdBar->TabIndex = 25;
      this->minThresholdBar->TabStop = false;
      this->minThresholdBar->TickStyle = System::Windows::Forms::TickStyle::None;
      this->minThresholdBar->Scroll += gcnew System::EventHandler(this, &FormRefPixPaint::minThreshold_Scroll);
      // 
      // maxThresholdBar
      // 
      this->maxThresholdBar->Location = System::Drawing::Point(151, 68);
      this->maxThresholdBar->Name = L"maxThresholdBar";
      this->maxThresholdBar->Size = System::Drawing::Size(277, 69);
      this->maxThresholdBar->TabIndex = 27;
      this->maxThresholdBar->TabStop = false;
      this->maxThresholdBar->TickStyle = System::Windows::Forms::TickStyle::None;
      this->maxThresholdBar->Scroll += gcnew System::EventHandler(this, &FormRefPixPaint::maxThreshold_Scroll);
      // 
      // thresholdRadioButton
      // 
      this->thresholdRadioButton->AutoSize = true;
      this->thresholdRadioButton->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->thresholdRadioButton->Location = System::Drawing::Point(8, 192);
      this->thresholdRadioButton->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->thresholdRadioButton->Name = L"thresholdRadioButton";
      this->thresholdRadioButton->Size = System::Drawing::Size(193, 30);
      this->thresholdRadioButton->TabIndex = 30;
      this->thresholdRadioButton->Text = L"LassoThreshold";
      this->thresholdRadioButton->UseVisualStyleBackColor = true;
      // 
      // eraseRadioButton
      // 
      this->eraseRadioButton->AutoSize = true;
      this->eraseRadioButton->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->eraseRadioButton->Location = System::Drawing::Point(214, 154);
      this->eraseRadioButton->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->eraseRadioButton->Name = L"eraseRadioButton";
      this->eraseRadioButton->Size = System::Drawing::Size(151, 30);
      this->eraseRadioButton->TabIndex = 29;
      this->eraseRadioButton->Text = L"LassoErase";
      this->eraseRadioButton->UseVisualStyleBackColor = true;
      // 
      // paintRadioButton
      // 
      this->paintRadioButton->AutoSize = true;
      this->paintRadioButton->Checked = true;
      this->paintRadioButton->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->paintRadioButton->Location = System::Drawing::Point(10, 154);
      this->paintRadioButton->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->paintRadioButton->Name = L"paintRadioButton";
      this->paintRadioButton->Size = System::Drawing::Size(147, 30);
      this->paintRadioButton->TabIndex = 28;
      this->paintRadioButton->TabStop = true;
      this->paintRadioButton->Text = L"LassoPaint";
      this->paintRadioButton->UseVisualStyleBackColor = true;
      // 
      // m_checkbox_forcedoverwrite
      // 
      this->m_checkbox_forcedoverwrite->AutoSize = true;
      this->m_checkbox_forcedoverwrite->Location = System::Drawing::Point(8, 241);
      this->m_checkbox_forcedoverwrite->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->m_checkbox_forcedoverwrite->Name = L"m_checkbox_forcedoverwrite";
      this->m_checkbox_forcedoverwrite->Size = System::Drawing::Size(329, 22);
      this->m_checkbox_forcedoverwrite->TabIndex = 31;
      this->m_checkbox_forcedoverwrite->Text = L"Allow forced overwrite of locked voxels.";
      this->m_checkbox_forcedoverwrite->UseVisualStyleBackColor = true;
      // 
      // groupBox2
      // 
      this->groupBox2->Controls->Add(this->label3);
      this->groupBox2->Location = System::Drawing::Point(22, 437);
      this->groupBox2->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->groupBox2->Name = L"groupBox2";
      this->groupBox2->Padding = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->groupBox2->Size = System::Drawing::Size(462, 69);
      this->groupBox2->TabIndex = 32;
      this->groupBox2->TabStop = false;
      this->groupBox2->Text = L"Region grow tool";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label3->Location = System::Drawing::Point(13, 33);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(418, 20);
      this->label3->TabIndex = 0;
      this->label3->Text = L"[Ctrl Shift R click] Region grow 3D current frame\r\n";
      // 
      // groupBox3
      // 
      this->groupBox3->Controls->Add(this->label1);
      this->groupBox3->Controls->Add(this->paintRadioButton);
      this->groupBox3->Controls->Add(this->m_checkbox_forcedoverwrite);
      this->groupBox3->Controls->Add(this->eraseRadioButton);
      this->groupBox3->Controls->Add(this->thresholdRadioButton);
      this->groupBox3->Location = System::Drawing::Point(22, 132);
      this->groupBox3->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->groupBox3->Name = L"groupBox3";
      this->groupBox3->Padding = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->groupBox3->Size = System::Drawing::Size(462, 283);
      this->groupBox3->TabIndex = 33;
      this->groupBox3->TabStop = false;
      this->groupBox3->Text = L"Lasso tool";
      // 
      // groupBox4
      // 
      this->groupBox4->Controls->Add(this->maxThresholdBar);
      this->groupBox4->Controls->Add(this->minThreshLabel);
      this->groupBox4->Controls->Add(this->maxThreshLabel);
      this->groupBox4->Controls->Add(this->minThreshTextBox);
      this->groupBox4->Controls->Add(this->maxThreshTextBox);
      this->groupBox4->Controls->Add(this->minThresholdBar);
      this->groupBox4->Location = System::Drawing::Point(22, 13);
      this->groupBox4->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->groupBox4->Name = L"groupBox4";
      this->groupBox4->Padding = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->groupBox4->Size = System::Drawing::Size(462, 101);
      this->groupBox4->TabIndex = 32;
      this->groupBox4->TabStop = false;
      this->groupBox4->Text = L"Threshold setting (Lasso && Region grow)";
      // 
      // FormRefPixPaint
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(10, 18);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(491, 891);
      this->Controls->Add(this->groupBox3);
      this->Controls->Add(this->groupBox4);
      this->Controls->Add(this->groupBox2);
      this->Controls->Add(this->groupBox1);
      this->Controls->Add(this->btn_cancel);
      this->Controls->Add(this->btn_ok);
      this->Name = L"FormRefPixPaint";
      this->Text = L"FormRefPixPaint";
      this->Load += gcnew System::EventHandler(this, &FormRefPixPaint::FormRefPixPaint_Load);
      this->groupBox1->ResumeLayout(false);
      this->groupBox1->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->minThresholdBar))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maxThresholdBar))->EndInit();
      this->groupBox2->ResumeLayout(false);
      this->groupBox2->PerformLayout();
      this->groupBox3->ResumeLayout(false);
      this->groupBox3->PerformLayout();
      this->groupBox4->ResumeLayout(false);
      this->groupBox4->PerformLayout();
      this->ResumeLayout(false);

    }
#pragma endregion
  private: System::Void FormRefPixPaint_Load(System::Object^ sender, System::EventArgs^ e) {
  }
  private: System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
  }
  private: System::Void btn_ok_Click(System::Object^ sender, System::EventArgs^ e);
  private: System::Void btn_cancel_Click(System::Object^ sender, System::EventArgs^ e);


  private: System::Void minThreshold_Scroll(System::Object^ sender, System::EventArgs^ e);
  private: System::Void maxThreshold_Scroll(System::Object^ sender, System::EventArgs^ e);
  private: System::Void minThreshTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e);
  private: System::Void maxThreshTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e);


  };


  inline void formRefPixPaint_Hide()
  {
    FormRefPixPaint::GetInst()->Hide();
  }
  inline void formRefPixPaint_Show()
  {
    FormRefPixPaint::GetInst()->Show();
  }

  inline bool FormRefPixPaint_bUseRoughRend() {
    return FormRefPixPaint::GetInst()->DoVisSurf();
  }
  inline bool formRefPixPaint_bForcedOverwite() { return FormRefPixPaint::GetInst()->bForcedOverwite(); }

  inline void formRefPixPaint_InitAllItems(int volmin, int volmax)
  {
    FormRefPixPaint::GetInst()->InitAllItems(volmin, volmax);
  }
  inline bool formRefPixPaint_bPaintMode() { return FormRefPixPaint::GetInst()->bPaintMode(); }
  inline bool formRefPixPaint_bEraseMode() { return FormRefPixPaint::GetInst()->bEraseMode(); }
  inline bool formRefPixPaint_bThresholdMode() { return FormRefPixPaint::GetInst()->bThresholdMode(); }
  inline short formRefPixPaint_maxThreshValue() { return FormRefPixPaint::GetInst()->maxthreshold(); }
  inline short formRefPixPaint_minThreshValue() { return FormRefPixPaint::GetInst()->minthreshold(); }
}
