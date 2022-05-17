#pragma once

namespace RoiPainter4D {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  /// <summary>
  /// FormRefStrokeTrim の概要
  /// </summary>
  public ref class FormRefStrokeTrim : public System::Windows::Forms::Form
  {
  private:
    System::Windows::Forms::Label^ label1;
    System::Windows::Forms::Button^ btn_ok;
    System::Windows::Forms::Button^ btn_cancel;
    System::Windows::Forms::GroupBox^ groupBox1;
    System::Windows::Forms::Label^ label2;
    System::Windows::Forms::CheckBox^ m_checkbox_userough;

  private:
    FormRefStrokeTrim(void)
    {
      InitializeComponent();

      this->ControlBox = false;
      this->MaximizeBox = false;
    }

    static FormRefStrokeTrim^ m_singleton;
  public:
    static FormRefStrokeTrim^ getInst() {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormRefStrokeTrim();
      }
      return m_singleton;
    }
    
  bool DoVisSurf (){ return m_checkbox_userough ->Checked; }

  protected:
    /// <summary>
    /// 使用中のリソースをすべてクリーンアップします。
    /// </summary>
    ~FormRefStrokeTrim()
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
        this->label1 = (gcnew System::Windows::Forms::Label());
        this->btn_ok = (gcnew System::Windows::Forms::Button());
        this->btn_cancel = (gcnew System::Windows::Forms::Button());
        this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
        this->m_checkbox_userough = (gcnew System::Windows::Forms::CheckBox());
        this->label2 = (gcnew System::Windows::Forms::Label());
        this->groupBox1->SuspendLayout();
        this->SuspendLayout();
        // 
        // label1
        // 
        this->label1->AutoSize = true;
        this->label1->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->label1->Location = System::Drawing::Point(11, 14);
        this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
        this->label1->Name = L"label1";
        this->label1->Size = System::Drawing::Size(219, 75);
        this->label1->TabIndex = 0;
        this->label1->Text = L"[Shift L-drag] draw trim stroke.\r\n\r\n[R click] cancel current stroke.\r\n\r\n[Ctrl z k"
            L"ey] undo trime operation.";
        this->label1->Click += gcnew System::EventHandler(this, &FormRefStrokeTrim::label1_Click);
        // 
        // btn_ok
        // 
        this->btn_ok->Location = System::Drawing::Point(12, 237);
        this->btn_ok->Name = L"btn_ok";
        this->btn_ok->Size = System::Drawing::Size(131, 50);
        this->btn_ok->TabIndex = 1;
        this->btn_ok->Text = L"Finish and Store";
        this->btn_ok->UseVisualStyleBackColor = true;
        this->btn_ok->Click += gcnew System::EventHandler(this, &FormRefStrokeTrim::btn_ok_Click);
        // 
        // btn_cancel
        // 
        this->btn_cancel->Location = System::Drawing::Point(149, 237);
        this->btn_cancel->Name = L"btn_cancel";
        this->btn_cancel->Size = System::Drawing::Size(80, 50);
        this->btn_cancel->TabIndex = 2;
        this->btn_cancel->Text = L"Cancel";
        this->btn_cancel->UseVisualStyleBackColor = true;
        this->btn_cancel->Click += gcnew System::EventHandler(this, &FormRefStrokeTrim::btn_cancel_Click);
        // 
        // groupBox1
        // 
        this->groupBox1->Controls->Add(this->m_checkbox_userough);
        this->groupBox1->Controls->Add(this->label2);
        this->groupBox1->Location = System::Drawing::Point(26, 109);
        this->groupBox1->Name = L"groupBox1";
        this->groupBox1->Size = System::Drawing::Size(181, 123);
        this->groupBox1->TabIndex = 3;
        this->groupBox1->TabStop = false;
        this->groupBox1->Text = L"Display control";
        // 
        // m_checkbox_userough
        // 
        this->m_checkbox_userough->AutoSize = true;
        this->m_checkbox_userough->Checked = true;
        this->m_checkbox_userough->CheckState = System::Windows::Forms::CheckState::Checked;
        this->m_checkbox_userough->Location = System::Drawing::Point(12, 94);
        this->m_checkbox_userough->Name = L"m_checkbox_userough";
        this->m_checkbox_userough->Size = System::Drawing::Size(160, 16);
        this->m_checkbox_userough->TabIndex = 1;
        this->m_checkbox_userough->Text = L"Render Rough while manip";
        this->m_checkbox_userough->UseVisualStyleBackColor = true;
        // 
        // label2
        // 
        this->label2->AutoSize = true;
        this->label2->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->label2->Location = System::Drawing::Point(9, 19);
        this->label2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
        this->label2->Name = L"label2";
        this->label2->Size = System::Drawing::Size(162, 65);
        this->label2->TabIndex = 0;
        this->label2->Text = L"[Tab key] CT image On/Off.\r\n\r\n[Space key] hide volume.\r\n\r\n[Ctrl + L-drag] cut str"
            L"oke";
        this->label2->Click += gcnew System::EventHandler(this, &FormRefStrokeTrim::label1_Click);
        // 
        // FormRefStrokeTrim
        // 
        this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
        this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
        this->ClientSize = System::Drawing::Size(244, 296);
        this->Controls->Add(this->groupBox1);
        this->Controls->Add(this->btn_cancel);
        this->Controls->Add(this->btn_ok);
        this->Controls->Add(this->label1);
        this->Margin = System::Windows::Forms::Padding(2);
        this->Name = L"FormRefStrokeTrim";
        this->Text = L"FormRefStrokeTrim";
        this->Load += gcnew System::EventHandler(this, &FormRefStrokeTrim::FormRefStrokeTrim_Load);
        this->groupBox1->ResumeLayout(false);
        this->groupBox1->PerformLayout();
        this->ResumeLayout(false);
        this->PerformLayout();

    }
#pragma endregion
  private: System::Void FormRefStrokeTrim_Load(System::Object^  sender, System::EventArgs^  e) {
  }
  private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
  }
  private: System::Void btn_ok_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btn_cancel_Click(System::Object^  sender, System::EventArgs^  e);
  };


  inline void formRefStrokeTrim_Hide()
  {
    FormRefStrokeTrim::getInst()->Hide();
  }
  inline void formRefStrokeTrim_Show()
  {
    FormRefStrokeTrim::getInst()->Show();
  }
  
  inline bool FormRefStrokeTrim_bUseRoughRend() { 
    return FormRefStrokeTrim::getInst()->DoVisSurf(); 
  }

}
