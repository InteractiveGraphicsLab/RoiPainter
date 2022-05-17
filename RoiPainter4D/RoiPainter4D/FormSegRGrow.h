#pragma once

namespace RoiPainter4D {
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
    bool m_item_updating;
    FormSegRGrow(void)
    {
      InitializeComponent();
      m_item_updating = false;
      this->ControlBox = false;
      this->MaximizeBox = false;
    }

private:
  System::Windows::Forms::Button^ btn_runThresholding;
  System::Windows::Forms::Button^ btn_runRegionGrow;
  System::Windows::Forms::Button^ btn_erode;
  System::Windows::Forms::Button^ btn_dilate;
  System::Windows::Forms::Button^ btn_fillHole;
  System::Windows::Forms::Button^ btn_ok;
  System::Windows::Forms::Button^ btn_cancel;
  System::Windows::Forms::Button^ btn_runThresholding_1frame;
  System::Windows::Forms::Button^ btn_runRegionGrow_1frame;

  System::Windows::Forms::Label^ minThreshLabel;
  System::Windows::Forms::Label^ maxThreshLabel;
  System::Windows::Forms::TextBox^ minThreshTextBox;
  System::Windows::Forms::TextBox^ maxThreshTextBox;

  System::Windows::Forms::TrackBar^ minThresholdBar;
  System::Windows::Forms::TrackBar^ maxThresholdBar;
  System::Windows::Forms::GroupBox^ groupBox1;
  System::Windows::Forms::GroupBox^ groupBox2;
  System::Windows::Forms::Label^ label1;
  System::Windows::Forms::Label^ label2;
  System::Windows::Forms::Label^ label3;
  System::Windows::Forms::Label^ label4;
  System::Windows::Forms::Label^ label5;

    static FormSegRGrow^ m_singleton;

  public:
    static FormSegRGrow^ getInst() {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormSegRGrow();
      }
      return m_singleton;
    }


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
  public:
    void InitAllItems(int volmin, int volmax);

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
      this->btn_runThresholding = (gcnew System::Windows::Forms::Button());
      this->btn_runRegionGrow = (gcnew System::Windows::Forms::Button());
      this->btn_erode = (gcnew System::Windows::Forms::Button());
      this->btn_dilate = (gcnew System::Windows::Forms::Button());
      this->btn_fillHole = (gcnew System::Windows::Forms::Button());
      this->btn_ok = (gcnew System::Windows::Forms::Button());
      this->btn_cancel = (gcnew System::Windows::Forms::Button());
      this->minThreshLabel = (gcnew System::Windows::Forms::Label());
      this->maxThreshLabel = (gcnew System::Windows::Forms::Label());
      this->minThreshTextBox = (gcnew System::Windows::Forms::TextBox());
      this->maxThreshTextBox = (gcnew System::Windows::Forms::TextBox());
      this->minThresholdBar = (gcnew System::Windows::Forms::TrackBar());
      this->maxThresholdBar = (gcnew System::Windows::Forms::TrackBar());
      this->btn_runThresholding_1frame = (gcnew System::Windows::Forms::Button());
      this->btn_runRegionGrow_1frame = (gcnew System::Windows::Forms::Button());
      this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
      this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->label4 = (gcnew System::Windows::Forms::Label());
      this->label5 = (gcnew System::Windows::Forms::Label());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->minThresholdBar))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maxThresholdBar))->BeginInit();
      this->groupBox1->SuspendLayout();
      this->groupBox2->SuspendLayout();
      this->SuspendLayout();
      // 
      // btn_runThresholding
      // 
      this->btn_runThresholding->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_runThresholding->Location = System::Drawing::Point(14, 22);
      this->btn_runThresholding->Name = L"btn_runThresholding";
      this->btn_runThresholding->Size = System::Drawing::Size(98, 30);
      this->btn_runThresholding->TabIndex = 4;
      this->btn_runThresholding->TabStop = false;
      this->btn_runThresholding->Text = L"Threshold";
      this->btn_runThresholding->UseVisualStyleBackColor = true;
      this->btn_runThresholding->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_runThresholding_Click);
      // 
      // btn_runRegionGrow
      // 
      this->btn_runRegionGrow->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_runRegionGrow->Location = System::Drawing::Point(14, 55);
      this->btn_runRegionGrow->Name = L"btn_runRegionGrow";
      this->btn_runRegionGrow->Size = System::Drawing::Size(98, 30);
      this->btn_runRegionGrow->TabIndex = 5;
      this->btn_runRegionGrow->TabStop = false;
      this->btn_runRegionGrow->Text = L"Region Grow";
      this->btn_runRegionGrow->UseVisualStyleBackColor = true;
      this->btn_runRegionGrow->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_runRegionGrow_Click);
      // 
      // btn_erode
      // 
      this->btn_erode->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_erode->Location = System::Drawing::Point(14, 90);
      this->btn_erode->Name = L"btn_erode";
      this->btn_erode->Size = System::Drawing::Size(98, 22);
      this->btn_erode->TabIndex = 15;
      this->btn_erode->TabStop = false;
      this->btn_erode->Text = L"erode";
      this->btn_erode->UseVisualStyleBackColor = true;
      this->btn_erode->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_erode_Click);
      // 
      // btn_dilate
      // 
      this->btn_dilate->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_dilate->Location = System::Drawing::Point(14, 113);
      this->btn_dilate->Name = L"btn_dilate";
      this->btn_dilate->Size = System::Drawing::Size(98, 22);
      this->btn_dilate->TabIndex = 14;
      this->btn_dilate->TabStop = false;
      this->btn_dilate->Text = L"dilate";
      this->btn_dilate->UseVisualStyleBackColor = true;
      this->btn_dilate->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_dilate_Click);
      // 
      // btn_fillHole
      // 
      this->btn_fillHole->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_fillHole->Location = System::Drawing::Point(14, 136);
      this->btn_fillHole->Name = L"btn_fillHole";
      this->btn_fillHole->Size = System::Drawing::Size(98, 22);
      this->btn_fillHole->TabIndex = 13;
      this->btn_fillHole->TabStop = false;
      this->btn_fillHole->Text = L"fill hole";
      this->btn_fillHole->UseVisualStyleBackColor = true;
      this->btn_fillHole->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_fillHole_Click);
      // 
      // btn_ok
      // 
      this->btn_ok->Font = (gcnew System::Drawing::Font(L"游ゴシック", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_ok->Location = System::Drawing::Point(8, 314);
      this->btn_ok->Name = L"btn_ok";
      this->btn_ok->Size = System::Drawing::Size(174, 33);
      this->btn_ok->TabIndex = 4;
      this->btn_ok->TabStop = false;
      this->btn_ok->Text = L"Finish and Store";
      this->btn_ok->UseVisualStyleBackColor = true;
      this->btn_ok->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_ok_Click);
      // 
      // btn_cancel
      // 
      this->btn_cancel->Font = (gcnew System::Drawing::Font(L"游ゴシック", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_cancel->Location = System::Drawing::Point(182, 314);
      this->btn_cancel->Name = L"btn_cancel";
      this->btn_cancel->Size = System::Drawing::Size(85, 33);
      this->btn_cancel->TabIndex = 5;
      this->btn_cancel->TabStop = false;
      this->btn_cancel->Text = L"Cancel";
      this->btn_cancel->UseVisualStyleBackColor = true;
      this->btn_cancel->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_cancel_Click);
      // 
      // minThreshLabel
      // 
      this->minThreshLabel->AutoSize = true;
      this->minThreshLabel->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->minThreshLabel->Location = System::Drawing::Point(12, 98);
      this->minThreshLabel->Name = L"minThreshLabel";
      this->minThreshLabel->Size = System::Drawing::Size(32, 17);
      this->minThreshLabel->TabIndex = 8;
      this->minThreshLabel->Text = L"min";
      // 
      // maxThreshLabel
      // 
      this->maxThreshLabel->AutoSize = true;
      this->maxThreshLabel->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->maxThreshLabel->Location = System::Drawing::Point(12, 119);
      this->maxThreshLabel->Name = L"maxThreshLabel";
      this->maxThreshLabel->Size = System::Drawing::Size(35, 17);
      this->maxThreshLabel->TabIndex = 9;
      this->maxThreshLabel->Text = L"max";
      // 
      // minThreshTextBox
      // 
      this->minThreshTextBox->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9));
      this->minThreshTextBox->Location = System::Drawing::Point(47, 97);
      this->minThreshTextBox->Name = L"minThreshTextBox";
      this->minThreshTextBox->Size = System::Drawing::Size(54, 19);
      this->minThreshTextBox->TabIndex = 0;
      this->minThreshTextBox->TextChanged += gcnew System::EventHandler(this, &FormSegRGrow::minThreshTextBox_TextChanged);
      // 
      // maxThreshTextBox
      // 
      this->maxThreshTextBox->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9));
      this->maxThreshTextBox->Location = System::Drawing::Point(47, 118);
      this->maxThreshTextBox->Name = L"maxThreshTextBox";
      this->maxThreshTextBox->Size = System::Drawing::Size(54, 19);
      this->maxThreshTextBox->TabIndex = 1;
      this->maxThreshTextBox->TextChanged += gcnew System::EventHandler(this, &FormSegRGrow::maxThreshTextBox_TextChanged);
      // 
      // minThresholdBar
      // 
      this->minThresholdBar->Location = System::Drawing::Point(106, 95);
      this->minThresholdBar->Name = L"minThresholdBar";
      this->minThresholdBar->Size = System::Drawing::Size(156, 45);
      this->minThresholdBar->TabIndex = 16;
      this->minThresholdBar->TabStop = false;
      this->minThresholdBar->TickStyle = System::Windows::Forms::TickStyle::None;
      this->minThresholdBar->Scroll += gcnew System::EventHandler(this, &FormSegRGrow::minThreshold_Scroll);
      // 
      // maxThresholdBar
      // 
      this->maxThresholdBar->Location = System::Drawing::Point(106, 117);
      this->maxThresholdBar->Name = L"maxThresholdBar";
      this->maxThresholdBar->Size = System::Drawing::Size(156, 45);
      this->maxThresholdBar->TabIndex = 17;
      this->maxThresholdBar->TabStop = false;
      this->maxThresholdBar->TickStyle = System::Windows::Forms::TickStyle::None;
      this->maxThresholdBar->Scroll += gcnew System::EventHandler(this, &FormSegRGrow::maxThreshold_Scroll);
      // 
      // btn_runThresholding_1frame
      // 
      this->btn_runThresholding_1frame->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_runThresholding_1frame->Location = System::Drawing::Point(13, 22);
      this->btn_runThresholding_1frame->Name = L"btn_runThresholding_1frame";
      this->btn_runThresholding_1frame->Size = System::Drawing::Size(98, 30);
      this->btn_runThresholding_1frame->TabIndex = 2;
      this->btn_runThresholding_1frame->Text = L"Threshold";
      this->btn_runThresholding_1frame->UseVisualStyleBackColor = true;
      this->btn_runThresholding_1frame->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_runThresholding_1frame_Click);
      // 
      // btn_runRegionGrow_1frame
      // 
      this->btn_runRegionGrow_1frame->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_runRegionGrow_1frame->Location = System::Drawing::Point(13, 55);
      this->btn_runRegionGrow_1frame->Name = L"btn_runRegionGrow_1frame";
      this->btn_runRegionGrow_1frame->Size = System::Drawing::Size(98, 30);
      this->btn_runRegionGrow_1frame->TabIndex = 3;
      this->btn_runRegionGrow_1frame->TabStop = false;
      this->btn_runRegionGrow_1frame->Text = L"Region Grow";
      this->btn_runRegionGrow_1frame->UseVisualStyleBackColor = true;
      this->btn_runRegionGrow_1frame->Click += gcnew System::EventHandler(this, &FormSegRGrow::btn_runRegionGrow_1frame_Click);
      // 
      // groupBox1
      // 
      this->groupBox1->Controls->Add(this->btn_fillHole);
      this->groupBox1->Controls->Add(this->btn_dilate);
      this->groupBox1->Controls->Add(this->btn_erode);
      this->groupBox1->Controls->Add(this->btn_runRegionGrow);
      this->groupBox1->Controls->Add(this->btn_runThresholding);
      this->groupBox1->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->groupBox1->Location = System::Drawing::Point(140, 145);
      this->groupBox1->Name = L"groupBox1";
      this->groupBox1->RightToLeft = System::Windows::Forms::RightToLeft::No;
      this->groupBox1->Size = System::Drawing::Size(127, 161);
      this->groupBox1->TabIndex = 18;
      this->groupBox1->TabStop = false;
      this->groupBox1->Text = L"All frame";
      // 
      // groupBox2
      // 
      this->groupBox2->Controls->Add(this->btn_runRegionGrow_1frame);
      this->groupBox2->Controls->Add(this->btn_runThresholding_1frame);
      this->groupBox2->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->groupBox2->Location = System::Drawing::Point(8, 145);
      this->groupBox2->Name = L"groupBox2";
      this->groupBox2->Size = System::Drawing::Size(127, 161);
      this->groupBox2->TabIndex = 19;
      this->groupBox2->TabStop = false;
      this->groupBox2->Text = L"Current frame";
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label1->Location = System::Drawing::Point(11, 76);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(73, 17);
      this->label1->TabIndex = 20;
      this->label1->Text = L"Threshold";
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label2->Location = System::Drawing::Point(8, 2);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(227, 17);
      this->label2->TabIndex = 20;
      this->label2->Text = L"Shift + L-click : ADD control point";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label3->Location = System::Drawing::Point(8, 19);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(233, 17);
      this->label3->TabIndex = 21;
      this->label3->Text = L"Shift + L-drag : move control point";
      // 
      // label4
      // 
      this->label4->AutoSize = true;
      this->label4->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label4->Location = System::Drawing::Point(8, 35);
      this->label4->Name = L"label4";
      this->label4->Size = System::Drawing::Size(250, 17);
      this->label4->TabIndex = 22;
      this->label4->Text = L"Shift + R-click : DELETE control point";
      // 
      // label5
      // 
      this->label5->AutoSize = true;
      this->label5->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label5->Location = System::Drawing::Point(8, 51);
      this->label5->Name = L"label5";
      this->label5->Size = System::Drawing::Size(162, 17);
      this->label5->TabIndex = 23;
      this->label5->Text = L"Ctrl + L-drag : cut stroke";
      // 
      // FormSegRGrow
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(7, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(276, 353);
      this->Controls->Add(this->label5);
      this->Controls->Add(this->label4);
      this->Controls->Add(this->label3);
      this->Controls->Add(this->label2);
      this->Controls->Add(this->label1);
      this->Controls->Add(this->minThreshLabel);
      this->Controls->Add(this->groupBox2);
      this->Controls->Add(this->maxThreshTextBox);
      this->Controls->Add(this->groupBox1);
      this->Controls->Add(this->maxThreshLabel);
      this->Controls->Add(this->btn_cancel);
      this->Controls->Add(this->minThreshTextBox);
      this->Controls->Add(this->btn_ok);
      this->Controls->Add(this->maxThresholdBar);
      this->Controls->Add(this->minThresholdBar);
      this->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->Name = L"FormSegRGrow";
      this->Text = L"FormSegRGrow";
      this->Load += gcnew System::EventHandler(this, &FormSegRGrow::FormSegRGrow_Load);
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->minThresholdBar))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maxThresholdBar))->EndInit();
      this->groupBox1->ResumeLayout(false);
      this->groupBox2->ResumeLayout(false);
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion

  private: System::Void btn_runThresholding_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btn_runRegionGrow_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btn_erode_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btn_dilate_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btn_fillHole_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btn_ok_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btn_cancel_Click(System::Object^  sender, System::EventArgs^  e);

  private: System::Void minThreshold_Scroll(System::Object^  sender, System::EventArgs^  e);
  private: System::Void maxThreshold_Scroll(System::Object^  sender, System::EventArgs^  e);
  private: System::Void minThreshTextBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void maxThreshTextBox_TextChanged(System::Object^  sender, System::EventArgs^  e);

  private: System::Void btn_runThresholding_1frame_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btn_runRegionGrow_1frame_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void FormSegRGrow_Load(System::Object^  sender, System::EventArgs^  e) {
  }
};

  inline void formSegRGrow_Show() { FormSegRGrow::getInst()->Show(); }
  inline void formSegRGrow_Hide() { FormSegRGrow::getInst()->Hide(); }
  inline void formSegRGrow_InitAllItems(int volmin, int volmax)
  { 
    FormSegRGrow::getInst()->InitAllItems(volmin, volmax); 
  }
}
