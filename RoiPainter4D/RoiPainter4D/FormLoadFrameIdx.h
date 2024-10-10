#pragma once

namespace RoiPainter4D {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  /// <summary>
  /// FormLoadFrameIdx の概要
  /// </summary>
  public ref class FormLoadFrameIdx : public System::Windows::Forms::Form
  {
  private:
    int m_frameN; //これfieldにする必要ある？？

  private: System::Windows::Forms::Label^  startIdxLabel;
  private: System::Windows::Forms::Label^  endIdxLabel;
  private: System::Windows::Forms::NumericUpDown^  startIdxUpDown;
  private: System::Windows::Forms::NumericUpDown^  endIdxUpDown;
  private: System::Windows::Forms::Button^  btn_ok;


  public:
    FormLoadFrameIdx(const int &frameN, const int &startI, const int &endI)
    {
      InitializeComponent();

      this->ControlBox = false;
      this->MaximizeBox = false;

      m_frameN = frameN;

      startIdxUpDown->Minimum = 0;
      startIdxUpDown->Maximum = m_frameN - 1;
      startIdxUpDown->Value = startI;

      endIdxUpDown->Minimum = 0;
      endIdxUpDown->Maximum = m_frameN - 1;
      endIdxUpDown->Value = endI;
    }

    FormLoadFrameIdx(const int &frameN) : FormLoadFrameIdx(frameN, 0, frameN - 1){}

    void setIndex(int &startI, int &endI);


  protected:
    /// <summary>
    /// 使用中のリソースをすべてクリーンアップします。
    /// </summary>
    ~FormLoadFrameIdx()
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
      this->startIdxLabel = (gcnew System::Windows::Forms::Label());
      this->endIdxLabel = (gcnew System::Windows::Forms::Label());
      this->startIdxUpDown = (gcnew System::Windows::Forms::NumericUpDown());
      this->endIdxUpDown = (gcnew System::Windows::Forms::NumericUpDown());
      this->btn_ok = (gcnew System::Windows::Forms::Button());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->startIdxUpDown))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->endIdxUpDown))->BeginInit();
      this->SuspendLayout();
      // 
      // startIdxLabel
      // 
      this->startIdxLabel->AutoSize = true;
      this->startIdxLabel->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->startIdxLabel->Location = System::Drawing::Point(13, 22);
      this->startIdxLabel->Name = L"startIdxLabel";
      this->startIdxLabel->Size = System::Drawing::Size(84, 16);
      this->startIdxLabel->TabIndex = 0;
      this->startIdxLabel->Text = L"Start Index";
      // 
      // endIdxLabel
      // 
      this->endIdxLabel->AutoSize = true;
      this->endIdxLabel->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->endIdxLabel->Location = System::Drawing::Point(13, 56);
      this->endIdxLabel->Name = L"endIdxLabel";
      this->endIdxLabel->Size = System::Drawing::Size(73, 16);
      this->endIdxLabel->TabIndex = 1;
      this->endIdxLabel->Text = L"End Index";
      // 
      // startIdxUpDown
      // 
      this->startIdxUpDown->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->startIdxUpDown->Location = System::Drawing::Point(116, 19);
      this->startIdxUpDown->Name = L"startIdxUpDown";
      this->startIdxUpDown->ReadOnly = true;
      this->startIdxUpDown->Size = System::Drawing::Size(82, 28);
      this->startIdxUpDown->TabIndex = 2;
      // 
      // endIdxUpDown
      // 
      this->endIdxUpDown->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->endIdxUpDown->Location = System::Drawing::Point(116, 58);
      this->endIdxUpDown->Name = L"endIdxUpDown";
      this->endIdxUpDown->ReadOnly = true;
      this->endIdxUpDown->Size = System::Drawing::Size(82, 28);
      this->endIdxUpDown->TabIndex = 3;
      // 
      // btn_ok
      // 
      this->btn_ok->DialogResult = System::Windows::Forms::DialogResult::OK;
      this->btn_ok->Location = System::Drawing::Point(220, 22);
      this->btn_ok->Name = L"btn_ok";
      this->btn_ok->Size = System::Drawing::Size(83, 50);
      this->btn_ok->TabIndex = 0;
      this->btn_ok->Text = L"OK";
      this->btn_ok->UseVisualStyleBackColor = true;
      // 
      // FormLoadFrameIdx
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(320, 98);
      this->Controls->Add(this->btn_ok);
      this->Controls->Add(this->endIdxUpDown);
      this->Controls->Add(this->startIdxUpDown);
      this->Controls->Add(this->endIdxLabel);
      this->Controls->Add(this->startIdxLabel);
      this->Name = L"FormLoadFrameIdx";
      this->Text = L"load frames";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->startIdxUpDown))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->endIdxUpDown))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion

  };

}
