#pragma once

namespace RoiPainter4D {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  /// <summary>
  /// FormSegClosestPix の概要
  /// </summary>
  public ref class FormSegClosestPix : public System::Windows::Forms::Form
  {

  private:
    System::Windows::Forms::Button^ btnOk;
    System::Windows::Forms::Button^ btnCancel;
    System::Windows::Forms::Button^ btnGo;
    System::Windows::Forms::Label^ startFrameLabel;
    System::Windows::Forms::Label^ baseFrameLabel;
    System::Windows::Forms::TextBox^ baseFrameTextBox;
    System::Windows::Forms::NumericUpDown^ startFrameUpDown;
    System::Windows::Forms::NumericUpDown^ endFrameUpDown;
    System::Windows::Forms::Label^ endFrameLabel;

    FormSegClosestPix(void)
    {
      InitializeComponent();
      this->ControlBox = false;
      this->MaximizeBox = false;
    }
    int m_startI, m_endI;

    static FormSegClosestPix^ m_singleton;

  public:
    static FormSegClosestPix^ getInst() {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormSegClosestPix();
      }
      return m_singleton;
    }

    void initItems(const int &frameN);

  protected:
    /// <summary>
    /// 使用中のリソースをすべてクリーンアップします。
    /// </summary>
    ~FormSegClosestPix()
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
      this->btnOk = (gcnew System::Windows::Forms::Button());
      this->btnCancel = (gcnew System::Windows::Forms::Button());
      this->btnGo = (gcnew System::Windows::Forms::Button());
      this->startFrameLabel = (gcnew System::Windows::Forms::Label());
      this->baseFrameLabel = (gcnew System::Windows::Forms::Label());
      this->baseFrameTextBox = (gcnew System::Windows::Forms::TextBox());
      this->startFrameUpDown = (gcnew System::Windows::Forms::NumericUpDown());
      this->endFrameUpDown = (gcnew System::Windows::Forms::NumericUpDown());
      this->endFrameLabel = (gcnew System::Windows::Forms::Label());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->startFrameUpDown))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->endFrameUpDown))->BeginInit();
      this->SuspendLayout();
      // 
      // btnOk
      // 
      this->btnOk->Location = System::Drawing::Point(14, 151);
      this->btnOk->Name = L"btnOk";
      this->btnOk->Size = System::Drawing::Size(85, 23);
      this->btnOk->TabIndex = 0;
      this->btnOk->Text = L"OK";
      this->btnOk->UseVisualStyleBackColor = true;
      this->btnOk->Click += gcnew System::EventHandler(this, &FormSegClosestPix::btnOk_Click);
      // 
      // btnCancel
      // 
      this->btnCancel->Location = System::Drawing::Point(107, 151);
      this->btnCancel->Name = L"btnCancel";
      this->btnCancel->Size = System::Drawing::Size(87, 23);
      this->btnCancel->TabIndex = 1;
      this->btnCancel->Text = L"CANCEL";
      this->btnCancel->UseVisualStyleBackColor = true;
      this->btnCancel->Click += gcnew System::EventHandler(this, &FormSegClosestPix::btnCancel_Click);
      // 
      // btnGo
      // 
      this->btnGo->Location = System::Drawing::Point(14, 116);
      this->btnGo->Name = L"btnGo";
      this->btnGo->Size = System::Drawing::Size(180, 29);
      this->btnGo->TabIndex = 2;
      this->btnGo->Text = L"GO";
      this->btnGo->UseVisualStyleBackColor = true;
      this->btnGo->Click += gcnew System::EventHandler(this, &FormSegClosestPix::btnGo_Click);
      // 
      // startFrameLabel
      // 
      this->startFrameLabel->AutoSize = true;
      this->startFrameLabel->Location = System::Drawing::Point(12, 15);
      this->startFrameLabel->Name = L"startFrameLabel";
      this->startFrameLabel->Size = System::Drawing::Size(66, 12);
      this->startFrameLabel->TabIndex = 3;
      this->startFrameLabel->Text = L"Start Frame";
      // 
      // baseFrameLabel
      // 
      this->baseFrameLabel->AutoSize = true;
      this->baseFrameLabel->Location = System::Drawing::Point(12, 85);
      this->baseFrameLabel->Name = L"baseFrameLabel";
      this->baseFrameLabel->Size = System::Drawing::Size(67, 12);
      this->baseFrameLabel->TabIndex = 4;
      this->baseFrameLabel->Text = L"Base Frame";
      // 
      // baseFrameTextBox
      // 
      this->baseFrameTextBox->Location = System::Drawing::Point(94, 82);
      this->baseFrameTextBox->Name = L"baseFrameTextBox";
      this->baseFrameTextBox->ReadOnly = true;
      this->baseFrameTextBox->Size = System::Drawing::Size(100, 19);
      this->baseFrameTextBox->TabIndex = 6;
      // 
      // startFrameUpDown
      // 
      this->startFrameUpDown->Location = System::Drawing::Point(94, 13);
      this->startFrameUpDown->Name = L"startFrameUpDown";
      this->startFrameUpDown->Size = System::Drawing::Size(100, 19);
      this->startFrameUpDown->TabIndex = 7;
      this->startFrameUpDown->ValueChanged += gcnew System::EventHandler(this, &FormSegClosestPix::startFrameUpDown_ValueChanged);
      // 
      // endFrameUpDown
      // 
      this->endFrameUpDown->Location = System::Drawing::Point(94, 48);
      this->endFrameUpDown->Name = L"endFrameUpDown";
      this->endFrameUpDown->Size = System::Drawing::Size(100, 19);
      this->endFrameUpDown->TabIndex = 9;
      this->endFrameUpDown->ValueChanged += gcnew System::EventHandler(this, &FormSegClosestPix::numericUpDown1_ValueChanged);
      // 
      // endFrameLabel
      // 
      this->endFrameLabel->AutoSize = true;
      this->endFrameLabel->Location = System::Drawing::Point(12, 50);
      this->endFrameLabel->Name = L"endFrameLabel";
      this->endFrameLabel->Size = System::Drawing::Size(60, 12);
      this->endFrameLabel->TabIndex = 8;
      this->endFrameLabel->Text = L"End Frame";
      // 
      // FormSegClosestPix
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(206, 186);
      this->Controls->Add(this->endFrameUpDown);
      this->Controls->Add(this->endFrameLabel);
      this->Controls->Add(this->startFrameUpDown);
      this->Controls->Add(this->baseFrameTextBox);
      this->Controls->Add(this->baseFrameLabel);
      this->Controls->Add(this->startFrameLabel);
      this->Controls->Add(this->btnGo);
      this->Controls->Add(this->btnCancel);
      this->Controls->Add(this->btnOk);
      this->Name = L"FormSegClosestPix";
      this->Text = L"SegClosestPix";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->startFrameUpDown))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->endFrameUpDown))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private: System::Void btnGo_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnOk_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnCancel_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void startFrameUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void numericUpDown1_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
  }
  };


  inline void formSegClosestPix_Show() { FormSegClosestPix::getInst()->Show(); }
  inline void formSegClosestPix_Hide() { FormSegClosestPix::getInst()->Hide(); }

}
