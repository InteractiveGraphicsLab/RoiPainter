#pragma once

namespace RoiPainter4D {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  /// <summary>
  /// FormSegPixPaint の概要
  /// </summary>
  public ref class FormSegPixPaint : public System::Windows::Forms::Form
  {
  private:
    System::Windows::Forms::Button^ okButton;
    System::Windows::Forms::Button^ cancelButton;
    System::Windows::Forms::RadioButton^ paintRadioButton;
    System::Windows::Forms::RadioButton^ lassoRadioButton;
    System::Windows::Forms::RadioButton^ lassoAllRadioButton;
    System::Windows::Forms::TextBox^ textBox1;
    System::Windows::Forms::TextBox^ textBox2;
    System::Windows::Forms::TextBox^ textBox3;
    System::Windows::Forms::TextBox^ textBox4;


  private:
    FormSegPixPaint(void)
    {
      InitializeComponent();
      this->ControlBox = false;
      this->MaximizeBox = false;
    }

    static FormSegPixPaint^ m_singleton;

  public:
    static FormSegPixPaint^ getInst() {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormSegPixPaint();
      }
      return m_singleton;
    }

  protected:
    /// <summary>
    /// 使用中のリソースをすべてクリーンアップします。
    /// </summary>
    ~FormSegPixPaint()
    {
      if (components)
      {
        delete components;
      }
    }

  public:
    bool bPaintMode   () { return paintRadioButton->Checked; }
    bool bLassoMode   () { return lassoRadioButton->Checked; }
    bool bLassoAllMode() { return lassoAllRadioButton->Checked; }


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
        this->okButton = (gcnew System::Windows::Forms::Button());
        this->cancelButton = (gcnew System::Windows::Forms::Button());
        this->paintRadioButton = (gcnew System::Windows::Forms::RadioButton());
        this->lassoRadioButton = (gcnew System::Windows::Forms::RadioButton());
        this->textBox1 = (gcnew System::Windows::Forms::TextBox());
        this->textBox2 = (gcnew System::Windows::Forms::TextBox());
        this->textBox3 = (gcnew System::Windows::Forms::TextBox());
        this->textBox4 = (gcnew System::Windows::Forms::TextBox());
        this->lassoAllRadioButton = (gcnew System::Windows::Forms::RadioButton());
        this->SuspendLayout();
        // 
        // okButton
        // 
        this->okButton->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->okButton->Location = System::Drawing::Point(12, 200);
        this->okButton->Name = L"okButton";
        this->okButton->Size = System::Drawing::Size(138, 39);
        this->okButton->TabIndex = 0;
        this->okButton->Text = L"Finish and Store";
        this->okButton->UseVisualStyleBackColor = true;
        this->okButton->Click += gcnew System::EventHandler(this, &FormSegPixPaint::okButton_Click);
        // 
        // cancelButton
        // 
        this->cancelButton->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->cancelButton->Location = System::Drawing::Point(156, 207);
        this->cancelButton->Name = L"cancelButton";
        this->cancelButton->Size = System::Drawing::Size(75, 28);
        this->cancelButton->TabIndex = 1;
        this->cancelButton->Text = L"CANCEL";
        this->cancelButton->UseVisualStyleBackColor = true;
        this->cancelButton->Click += gcnew System::EventHandler(this, &FormSegPixPaint::cancelButton_Click);
        // 
        // paintRadioButton
        // 
        this->paintRadioButton->AutoSize = true;
        this->paintRadioButton->Checked = true;
        this->paintRadioButton->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->paintRadioButton->Location = System::Drawing::Point(12, 4);
        this->paintRadioButton->Name = L"paintRadioButton";
        this->paintRadioButton->Size = System::Drawing::Size(99, 21);
        this->paintRadioButton->TabIndex = 2;
        this->paintRadioButton->TabStop = true;
        this->paintRadioButton->Text = L"Paint Voxel";
        this->paintRadioButton->UseVisualStyleBackColor = true;
        this->paintRadioButton->CheckedChanged += gcnew System::EventHandler(this, &FormSegPixPaint::paintRadioButton_CheckedChanged);
        // 
        // lassoRadioButton
        // 
        this->lassoRadioButton->AutoSize = true;
        this->lassoRadioButton->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->lassoRadioButton->Location = System::Drawing::Point(12, 70);
        this->lassoRadioButton->Name = L"lassoRadioButton";
        this->lassoRadioButton->Size = System::Drawing::Size(110, 21);
        this->lassoRadioButton->TabIndex = 3;
        this->lassoRadioButton->Text = L"Lasso Stroke";
        this->lassoRadioButton->UseVisualStyleBackColor = true;
        this->lassoRadioButton->CheckedChanged += gcnew System::EventHandler(this, &FormSegPixPaint::LassoRadioButton_CheckedChanged);
        // 
        // textBox1
        // 
        this->textBox1->BackColor = System::Drawing::SystemColors::MenuBar;
        this->textBox1->BorderStyle = System::Windows::Forms::BorderStyle::None;
        this->textBox1->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->textBox1->ForeColor = System::Drawing::SystemColors::MenuText;
        this->textBox1->Location = System::Drawing::Point(13, 25);
        this->textBox1->Name = L"textBox1";
        this->textBox1->Size = System::Drawing::Size(221, 21);
        this->textBox1->TabIndex = 4;
        this->textBox1->Text = L"Shift + L drag : paint fore voxel";
        // 
        // textBox2
        // 
        this->textBox2->BackColor = System::Drawing::SystemColors::MenuBar;
        this->textBox2->BorderStyle = System::Windows::Forms::BorderStyle::None;
        this->textBox2->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->textBox2->Location = System::Drawing::Point(13, 43);
        this->textBox2->Name = L"textBox2";
        this->textBox2->Size = System::Drawing::Size(221, 21);
        this->textBox2->TabIndex = 5;
        this->textBox2->Text = L"Shift + R drag : paint back voxel";
        // 
        // textBox3
        // 
        this->textBox3->BackColor = System::Drawing::SystemColors::MenuBar;
        this->textBox3->BorderStyle = System::Windows::Forms::BorderStyle::None;
        this->textBox3->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->textBox3->Location = System::Drawing::Point(13, 110);
        this->textBox3->Name = L"textBox3";
        this->textBox3->Size = System::Drawing::Size(221, 21);
        this->textBox3->TabIndex = 7;
        this->textBox3->Text = L"Shift + R drag : draw back lasso";
        // 
        // textBox4
        // 
        this->textBox4->BackColor = System::Drawing::SystemColors::MenuBar;
        this->textBox4->BorderStyle = System::Windows::Forms::BorderStyle::None;
        this->textBox4->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->textBox4->ForeColor = System::Drawing::SystemColors::MenuText;
        this->textBox4->Location = System::Drawing::Point(13, 92);
        this->textBox4->Name = L"textBox4";
        this->textBox4->Size = System::Drawing::Size(221, 21);
        this->textBox4->TabIndex = 6;
        this->textBox4->Text = L"Shift + L drag : draw fore lasso";
        // 
        // lassoAllRadioButton
        // 
        this->lassoAllRadioButton->AutoSize = true;
        this->lassoAllRadioButton->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->lassoAllRadioButton->Location = System::Drawing::Point(13, 137);
        this->lassoAllRadioButton->Name = L"lassoAllRadioButton";
        this->lassoAllRadioButton->Size = System::Drawing::Size(191, 21);
        this->lassoAllRadioButton->TabIndex = 8;
        this->lassoAllRadioButton->Text = L"Lasso Stroke (ALL frame)";
        this->lassoAllRadioButton->UseVisualStyleBackColor = true;
        // 
        // FormSegPixPaint
        // 
        this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
        this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
        this->ClientSize = System::Drawing::Size(236, 251);
        this->Controls->Add(this->lassoAllRadioButton);
        this->Controls->Add(this->textBox3);
        this->Controls->Add(this->textBox4);
        this->Controls->Add(this->textBox2);
        this->Controls->Add(this->textBox1);
        this->Controls->Add(this->lassoRadioButton);
        this->Controls->Add(this->paintRadioButton);
        this->Controls->Add(this->cancelButton);
        this->Controls->Add(this->okButton);
        this->Name = L"FormSegPixPaint";
        this->Text = L"FormSegPixPaint";
        this->Load += gcnew System::EventHandler(this, &FormSegPixPaint::FormSegPixPaint_Load);
        this->ResumeLayout(false);
        this->PerformLayout();

    }
#pragma endregion
  private: System::Void paintRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {}
  private: System::Void LassoRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {}
  private: System::Void okButton_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void cancelButton_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void FormSegPixPaint_Load(System::Object^  sender, System::EventArgs^  e) {}
};


  inline void formSegPixPaint_Show() { FormSegPixPaint::getInst()->Show(); }
  inline void formSegPixPaint_Hide() { FormSegPixPaint::getInst()->Hide(); }
  inline bool formSegPixPaint_bPaintMode   () { return FormSegPixPaint::getInst()->bPaintMode   (); }
  inline bool formSegPixPaint_bLassoMode   () { return FormSegPixPaint::getInst()->bLassoMode   (); }
  inline bool formSegPixPaint_bLassoAllMode() { return FormSegPixPaint::getInst()->bLassoAllMode(); }

}
