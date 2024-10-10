#pragma once

namespace RoiPainter4D {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  /// <summary>
  /// FormSegStrokeFfd の概要
  /// </summary>
  public ref class FormSegStrokeFfd : public System::Windows::Forms::Form
  {
  private:
    static FormSegStrokeFfd^ m_singleton;
    FormSegStrokeFfd(void)
    {
      InitializeComponent();
    }

  public: 
    static FormSegStrokeFfd^ GetInst()
    {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormSegStrokeFfd();
      }
      return m_singleton;
    }

    void InitAllItems();


  protected:
    /// <summary>
    /// 使用中のリソースをすべてクリーンアップします。
    /// </summary>
    ~FormSegStrokeFfd()
    {
      if (components)
      {
        delete components;
      }
    }
  private: System::Windows::Forms::Button^ m_btn_loadmeshandcage;
  private: System::Windows::Forms::Button^ m_btn_savemeshandcage;
  private: System::Windows::Forms::Button^ m_btn_loadstate;
  private: System::Windows::Forms::Button^ m_btn_savestate;
  private: System::Windows::Forms::TextBox^ m_textbox_alpha;
  private: System::Windows::Forms::TextBox^ m_textbox_beta;
  private: System::Windows::Forms::TextBox^ m_textbox_gamma;
  private: System::Windows::Forms::Button^ m_btn_deform;
  private: System::Windows::Forms::Label^ label1;
  private: System::Windows::Forms::Label^ label2;
  private: System::Windows::Forms::Label^ label3;
  private: System::Windows::Forms::Button^ m_btn_undo;
  private: System::Windows::Forms::Button^ m_btn_redo;
  private: System::Windows::Forms::Button^ m_btn_copyfromprevframe;
  private: System::Windows::Forms::RadioButton^ m_radiobtn_movement;
  private: System::Windows::Forms::RadioButton^ m_radiobtn_rotation;
  private: System::Windows::Forms::RadioButton^ m_radiobtn_scaling;

  private: System::Windows::Forms::Button^ m_btn_sharestroke;

  private: System::Windows::Forms::RadioButton^ m_radiobtn_stroke;
  private: System::Windows::Forms::RadioButton^ m_radiobtn_cage;
  private: System::Windows::Forms::GroupBox^ groupBox1;
  private: System::Windows::Forms::GroupBox^ groupBox2;
  private: System::Windows::Forms::NumericUpDown^ m_numbox_cpsize;



  private: System::Windows::Forms::Label^ label4;
  private: System::Windows::Forms::CheckBox^ m_checkbox_showonlyselectedstroke;
  private: System::Windows::Forms::Button^ m_btn_copycagetoallframes;












  protected:

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
      this->m_btn_loadmeshandcage = (gcnew System::Windows::Forms::Button());
      this->m_btn_savemeshandcage = (gcnew System::Windows::Forms::Button());
      this->m_btn_loadstate = (gcnew System::Windows::Forms::Button());
      this->m_btn_savestate = (gcnew System::Windows::Forms::Button());
      this->m_textbox_alpha = (gcnew System::Windows::Forms::TextBox());
      this->m_textbox_beta = (gcnew System::Windows::Forms::TextBox());
      this->m_textbox_gamma = (gcnew System::Windows::Forms::TextBox());
      this->m_btn_deform = (gcnew System::Windows::Forms::Button());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->m_btn_undo = (gcnew System::Windows::Forms::Button());
      this->m_btn_redo = (gcnew System::Windows::Forms::Button());
      this->m_btn_copyfromprevframe = (gcnew System::Windows::Forms::Button());
      this->m_radiobtn_movement = (gcnew System::Windows::Forms::RadioButton());
      this->m_radiobtn_rotation = (gcnew System::Windows::Forms::RadioButton());
      this->m_radiobtn_scaling = (gcnew System::Windows::Forms::RadioButton());
      this->m_btn_sharestroke = (gcnew System::Windows::Forms::Button());
      this->m_radiobtn_stroke = (gcnew System::Windows::Forms::RadioButton());
      this->m_radiobtn_cage = (gcnew System::Windows::Forms::RadioButton());
      this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
      this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
      this->m_numbox_cpsize = (gcnew System::Windows::Forms::NumericUpDown());
      this->label4 = (gcnew System::Windows::Forms::Label());
      this->m_checkbox_showonlyselectedstroke = (gcnew System::Windows::Forms::CheckBox());
      this->m_btn_copycagetoallframes = (gcnew System::Windows::Forms::Button());
      this->groupBox1->SuspendLayout();
      this->groupBox2->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numbox_cpsize))->BeginInit();
      this->SuspendLayout();
      // 
      // m_btn_loadmeshandcage
      // 
      this->m_btn_loadmeshandcage->Location = System::Drawing::Point(12, 12);
      this->m_btn_loadmeshandcage->Name = L"m_btn_loadmeshandcage";
      this->m_btn_loadmeshandcage->Size = System::Drawing::Size(135, 23);
      this->m_btn_loadmeshandcage->TabIndex = 0;
      this->m_btn_loadmeshandcage->Text = L"Import mesh and cage";
      this->m_btn_loadmeshandcage->UseVisualStyleBackColor = true;
      this->m_btn_loadmeshandcage->Click += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_btn_loadmeshandcage_Click);
      // 
      // m_btn_savemeshandcage
      // 
      this->m_btn_savemeshandcage->Location = System::Drawing::Point(12, 40);
      this->m_btn_savemeshandcage->Margin = System::Windows::Forms::Padding(2);
      this->m_btn_savemeshandcage->Name = L"m_btn_savemeshandcage";
      this->m_btn_savemeshandcage->Size = System::Drawing::Size(135, 23);
      this->m_btn_savemeshandcage->TabIndex = 1;
      this->m_btn_savemeshandcage->Text = L"Export mesh and cage";
      this->m_btn_savemeshandcage->UseVisualStyleBackColor = true;
      this->m_btn_savemeshandcage->Click += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_btn_savemeshandcage_Click);
      // 
      // m_btn_loadstate
      // 
      this->m_btn_loadstate->Location = System::Drawing::Point(12, 67);
      this->m_btn_loadstate->Margin = System::Windows::Forms::Padding(2);
      this->m_btn_loadstate->Name = L"m_btn_loadstate";
      this->m_btn_loadstate->Size = System::Drawing::Size(135, 23);
      this->m_btn_loadstate->TabIndex = 2;
      this->m_btn_loadstate->Text = L"Load state";
      this->m_btn_loadstate->UseVisualStyleBackColor = true;
      this->m_btn_loadstate->Click += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_btn_loadstate_Click);
      // 
      // m_btn_savestate
      // 
      this->m_btn_savestate->Location = System::Drawing::Point(12, 94);
      this->m_btn_savestate->Margin = System::Windows::Forms::Padding(2);
      this->m_btn_savestate->Name = L"m_btn_savestate";
      this->m_btn_savestate->Size = System::Drawing::Size(135, 23);
      this->m_btn_savestate->TabIndex = 3;
      this->m_btn_savestate->Text = L"Save state";
      this->m_btn_savestate->UseVisualStyleBackColor = true;
      this->m_btn_savestate->Click += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_btn_savestate_Click);
      // 
      // m_textbox_alpha
      // 
      this->m_textbox_alpha->Location = System::Drawing::Point(41, 122);
      this->m_textbox_alpha->Name = L"m_textbox_alpha";
      this->m_textbox_alpha->Size = System::Drawing::Size(106, 19);
      this->m_textbox_alpha->TabIndex = 4;
      this->m_textbox_alpha->Text = L"1.0";
      // 
      // m_textbox_beta
      // 
      this->m_textbox_beta->Location = System::Drawing::Point(41, 147);
      this->m_textbox_beta->Name = L"m_textbox_beta";
      this->m_textbox_beta->Size = System::Drawing::Size(106, 19);
      this->m_textbox_beta->TabIndex = 5;
      this->m_textbox_beta->Text = L"1.0";
      // 
      // m_textbox_gamma
      // 
      this->m_textbox_gamma->Location = System::Drawing::Point(41, 172);
      this->m_textbox_gamma->Name = L"m_textbox_gamma";
      this->m_textbox_gamma->Size = System::Drawing::Size(106, 19);
      this->m_textbox_gamma->TabIndex = 6;
      this->m_textbox_gamma->Text = L"5.0";
      // 
      // m_btn_deform
      // 
      this->m_btn_deform->Location = System::Drawing::Point(12, 196);
      this->m_btn_deform->Margin = System::Windows::Forms::Padding(2);
      this->m_btn_deform->Name = L"m_btn_deform";
      this->m_btn_deform->Size = System::Drawing::Size(135, 23);
      this->m_btn_deform->TabIndex = 7;
      this->m_btn_deform->Text = L"Deform";
      this->m_btn_deform->UseVisualStyleBackColor = true;
      this->m_btn_deform->Click += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_btn_deform_Click);
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Location = System::Drawing::Point(12, 125);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(23, 12);
      this->label1->TabIndex = 8;
      this->label1->Text = L"α :";
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Location = System::Drawing::Point(10, 150);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(23, 12);
      this->label2->TabIndex = 9;
      this->label2->Text = L"β :";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Location = System::Drawing::Point(10, 175);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(23, 12);
      this->label3->TabIndex = 10;
      this->label3->Text = L"γ :";
      // 
      // m_btn_undo
      // 
      this->m_btn_undo->Location = System::Drawing::Point(12, 223);
      this->m_btn_undo->Margin = System::Windows::Forms::Padding(2);
      this->m_btn_undo->Name = L"m_btn_undo";
      this->m_btn_undo->Size = System::Drawing::Size(65, 23);
      this->m_btn_undo->TabIndex = 11;
      this->m_btn_undo->Text = L"UNDO";
      this->m_btn_undo->UseVisualStyleBackColor = true;
      this->m_btn_undo->Click += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_btn_undo_Click);
      // 
      // m_btn_redo
      // 
      this->m_btn_redo->Location = System::Drawing::Point(82, 223);
      this->m_btn_redo->Margin = System::Windows::Forms::Padding(2);
      this->m_btn_redo->Name = L"m_btn_redo";
      this->m_btn_redo->Size = System::Drawing::Size(65, 23);
      this->m_btn_redo->TabIndex = 12;
      this->m_btn_redo->Text = L"REDO";
      this->m_btn_redo->UseVisualStyleBackColor = true;
      this->m_btn_redo->Click += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_btn_redo_Click);
      // 
      // m_btn_copyfromprevframe
      // 
      this->m_btn_copyfromprevframe->Location = System::Drawing::Point(12, 250);
      this->m_btn_copyfromprevframe->Margin = System::Windows::Forms::Padding(2);
      this->m_btn_copyfromprevframe->Name = L"m_btn_copyfromprevframe";
      this->m_btn_copyfromprevframe->Size = System::Drawing::Size(135, 23);
      this->m_btn_copyfromprevframe->TabIndex = 13;
      this->m_btn_copyfromprevframe->Text = L"Copy from prev frame";
      this->m_btn_copyfromprevframe->UseVisualStyleBackColor = true;
      this->m_btn_copyfromprevframe->Click += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_btn_copyfromprevframe_Click);
      // 
      // m_radiobtn_movement
      // 
      this->m_radiobtn_movement->AutoSize = true;
      this->m_radiobtn_movement->Checked = true;
      this->m_radiobtn_movement->Location = System::Drawing::Point(6, 18);
      this->m_radiobtn_movement->Name = L"m_radiobtn_movement";
      this->m_radiobtn_movement->Size = System::Drawing::Size(80, 16);
      this->m_radiobtn_movement->TabIndex = 14;
      this->m_radiobtn_movement->TabStop = true;
      this->m_radiobtn_movement->Text = L"Translation";
      this->m_radiobtn_movement->UseVisualStyleBackColor = true;
      this->m_radiobtn_movement->CheckedChanged += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_radiobtn_movement_CheckedChanged);
      // 
      // m_radiobtn_rotation
      // 
      this->m_radiobtn_rotation->AutoSize = true;
      this->m_radiobtn_rotation->Location = System::Drawing::Point(6, 40);
      this->m_radiobtn_rotation->Name = L"m_radiobtn_rotation";
      this->m_radiobtn_rotation->Size = System::Drawing::Size(66, 16);
      this->m_radiobtn_rotation->TabIndex = 15;
      this->m_radiobtn_rotation->Text = L"Rotation";
      this->m_radiobtn_rotation->UseVisualStyleBackColor = true;
      this->m_radiobtn_rotation->CheckedChanged += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_radiobtn_rotation_CheckedChanged);
      // 
      // m_radiobtn_scaling
      // 
      this->m_radiobtn_scaling->AutoSize = true;
      this->m_radiobtn_scaling->Location = System::Drawing::Point(6, 62);
      this->m_radiobtn_scaling->Name = L"m_radiobtn_scaling";
      this->m_radiobtn_scaling->Size = System::Drawing::Size(60, 16);
      this->m_radiobtn_scaling->TabIndex = 16;
      this->m_radiobtn_scaling->Text = L"Scaling";
      this->m_radiobtn_scaling->UseVisualStyleBackColor = true;
      this->m_radiobtn_scaling->CheckedChanged += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_radiobtn_scaling_CheckedChanged);
      // 
      // m_btn_sharestroke
      // 
      this->m_btn_sharestroke->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9));
      this->m_btn_sharestroke->Location = System::Drawing::Point(12, 278);
      this->m_btn_sharestroke->Name = L"m_btn_sharestroke";
      this->m_btn_sharestroke->Size = System::Drawing::Size(135, 23);
      this->m_btn_sharestroke->TabIndex = 18;
      this->m_btn_sharestroke->Text = L"Set as all frame curve";
      this->m_btn_sharestroke->UseVisualStyleBackColor = true;
      this->m_btn_sharestroke->Click += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_btn_sharestroke_Click);
      // 
      // m_radiobtn_stroke
      // 
      this->m_radiobtn_stroke->AutoSize = true;
      this->m_radiobtn_stroke->Checked = true;
      this->m_radiobtn_stroke->Location = System::Drawing::Point(6, 18);
      this->m_radiobtn_stroke->Name = L"m_radiobtn_stroke";
      this->m_radiobtn_stroke->Size = System::Drawing::Size(53, 16);
      this->m_radiobtn_stroke->TabIndex = 20;
      this->m_radiobtn_stroke->TabStop = true;
      this->m_radiobtn_stroke->Text = L"Curve";
      this->m_radiobtn_stroke->UseVisualStyleBackColor = true;
      this->m_radiobtn_stroke->CheckedChanged += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_radiobtn_stroke_CheckedChanged);
      // 
      // m_radiobtn_cage
      // 
      this->m_radiobtn_cage->AutoSize = true;
      this->m_radiobtn_cage->Location = System::Drawing::Point(6, 40);
      this->m_radiobtn_cage->Name = L"m_radiobtn_cage";
      this->m_radiobtn_cage->Size = System::Drawing::Size(49, 16);
      this->m_radiobtn_cage->TabIndex = 21;
      this->m_radiobtn_cage->Text = L"Cage";
      this->m_radiobtn_cage->UseVisualStyleBackColor = true;
      this->m_radiobtn_cage->CheckedChanged += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_radiobtn_cage_CheckedChanged);
      // 
      // groupBox1
      // 
      this->groupBox1->Controls->Add(this->m_radiobtn_movement);
      this->groupBox1->Controls->Add(this->m_radiobtn_rotation);
      this->groupBox1->Controls->Add(this->m_radiobtn_scaling);
      this->groupBox1->Location = System::Drawing::Point(182, 159);
      this->groupBox1->Name = L"groupBox1";
      this->groupBox1->Size = System::Drawing::Size(107, 85);
      this->groupBox1->TabIndex = 22;
      this->groupBox1->TabStop = false;
      this->groupBox1->Text = L"Transform mode";
      // 
      // groupBox2
      // 
      this->groupBox2->Controls->Add(this->m_radiobtn_cage);
      this->groupBox2->Controls->Add(this->m_radiobtn_stroke);
      this->groupBox2->Location = System::Drawing::Point(182, 90);
      this->groupBox2->Name = L"groupBox2";
      this->groupBox2->Size = System::Drawing::Size(107, 63);
      this->groupBox2->TabIndex = 23;
      this->groupBox2->TabStop = false;
      this->groupBox2->Text = L"Deform mode";
      // 
      // m_numbox_cpsize
      // 
      this->m_numbox_cpsize->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.5F));
      this->m_numbox_cpsize->Location = System::Drawing::Point(232, 250);
      this->m_numbox_cpsize->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
      this->m_numbox_cpsize->Name = L"m_numbox_cpsize";
      this->m_numbox_cpsize->Size = System::Drawing::Size(57, 23);
      this->m_numbox_cpsize->TabIndex = 24;
      this->m_numbox_cpsize->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
      this->m_numbox_cpsize->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
      this->m_numbox_cpsize->ValueChanged += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_numbox_cpsize_ValueChanged);
      // 
      // label4
      // 
      this->label4->AutoSize = true;
      this->label4->Location = System::Drawing::Point(180, 255);
      this->label4->Name = L"label4";
      this->label4->Size = System::Drawing::Size(46, 12);
      this->label4->TabIndex = 25;
      this->label4->Text = L"CP size:";
      // 
      // m_checkbox_showonlyselectedstroke
      // 
      this->m_checkbox_showonlyselectedstroke->AutoSize = true;
      this->m_checkbox_showonlyselectedstroke->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 8));
      this->m_checkbox_showonlyselectedstroke->Location = System::Drawing::Point(153, 283);
      this->m_checkbox_showonlyselectedstroke->Name = L"m_checkbox_showonlyselectedstroke";
      this->m_checkbox_showonlyselectedstroke->Size = System::Drawing::Size(148, 15);
      this->m_checkbox_showonlyselectedstroke->TabIndex = 26;
      this->m_checkbox_showonlyselectedstroke->Text = L"Show only selected stroke";
      this->m_checkbox_showonlyselectedstroke->UseVisualStyleBackColor = true;
      this->m_checkbox_showonlyselectedstroke->CheckedChanged += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_checkbox_showonlyselectedstroke_CheckedChanged);
      // 
      // m_btn_copycagetoallframes
      // 
      this->m_btn_copycagetoallframes->Location = System::Drawing::Point(153, 12);
      this->m_btn_copycagetoallframes->Name = L"m_btn_copycagetoallframes";
      this->m_btn_copycagetoallframes->Size = System::Drawing::Size(136, 23);
      this->m_btn_copycagetoallframes->TabIndex = 27;
      this->m_btn_copycagetoallframes->Text = L"Copy cage to all frames";
      this->m_btn_copycagetoallframes->UseVisualStyleBackColor = true;
      this->m_btn_copycagetoallframes->Click += gcnew System::EventHandler(this, &FormSegStrokeFfd::m_btn_copycagetoallframes_Click);
      // 
      // FormSegStrokeFfd
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(300, 316);
      this->Controls->Add(this->m_btn_copycagetoallframes);
      this->Controls->Add(this->m_checkbox_showonlyselectedstroke);
      this->Controls->Add(this->label4);
      this->Controls->Add(this->m_numbox_cpsize);
      this->Controls->Add(this->groupBox2);
      this->Controls->Add(this->groupBox1);
      this->Controls->Add(this->m_btn_sharestroke);
      this->Controls->Add(this->m_btn_copyfromprevframe);
      this->Controls->Add(this->m_btn_redo);
      this->Controls->Add(this->m_btn_undo);
      this->Controls->Add(this->label3);
      this->Controls->Add(this->label2);
      this->Controls->Add(this->label1);
      this->Controls->Add(this->m_btn_deform);
      this->Controls->Add(this->m_textbox_gamma);
      this->Controls->Add(this->m_textbox_beta);
      this->Controls->Add(this->m_textbox_alpha);
      this->Controls->Add(this->m_btn_savestate);
      this->Controls->Add(this->m_btn_loadstate);
      this->Controls->Add(this->m_btn_savemeshandcage);
      this->Controls->Add(this->m_btn_loadmeshandcage);
      this->Name = L"FormSegStrokeFfd";
      this->Text = L"FormSegStrokeFfd";
      this->groupBox1->ResumeLayout(false);
      this->groupBox1->PerformLayout();
      this->groupBox2->ResumeLayout(false);
      this->groupBox2->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numbox_cpsize))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private:
    System::Void m_btn_loadmeshandcage_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void m_btn_savemeshandcage_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void m_btn_loadstate_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void m_btn_savestate_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void m_btn_deform_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void m_btn_undo_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void m_btn_redo_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void m_btn_copyfromprevframe_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void m_btn_sharestroke_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void m_btn_copycagetoallframes_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void m_radiobtn_stroke_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
    System::Void m_radiobtn_cage_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
    System::Void m_radiobtn_movement_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
    System::Void m_radiobtn_rotation_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
    System::Void m_radiobtn_scaling_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
    System::Void m_numbox_cpsize_ValueChanged(System::Object^ sender, System::EventArgs^ e);
    System::Void m_checkbox_showonlyselectedstroke_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
  public:
    std::string GetAlpha();
    std::string GetBeta();
    std::string GetGamma();
    int GetDeformMode();
    int GetTransformMode();
    int GetCPSize();
    bool GetShowOnlySelectedStroke();
    void SetDeformMode(const int& _mode);
    void SetTransformMode(const int& _mode);
    void FrameChanged();


};

  inline void FormSegStrokeFfd_Show() { FormSegStrokeFfd::GetInst()->Show(); }
  inline void FormSegStrokeFfd_Hide() { FormSegStrokeFfd::GetInst()->Hide(); }
  inline void FormSegStrokeFfd_InitAllItems() { FormSegStrokeFfd::GetInst()->InitAllItems(); }
  inline std::string FormSegStrokeFfd_GetAlpha() { return FormSegStrokeFfd::GetInst()->GetAlpha(); }
  inline std::string FormSegStrokeFfd_GetBeta() { return FormSegStrokeFfd::GetInst()->GetBeta(); }
  inline std::string FormSegStrokeFfd_GetGamma() { return FormSegStrokeFfd::GetInst()->GetGamma(); }
  inline int FormSegStrokeFfd_GetDeformMode() { return FormSegStrokeFfd::GetInst()->GetDeformMode(); }
  inline int FormSegStrokeFfd_GetTransformMode() { return FormSegStrokeFfd::GetInst()->GetTransformMode(); }
  inline int FormSegStrokeFfd_GetCPSize() { return FormSegStrokeFfd::GetInst()->GetCPSize(); }
  inline bool FormSegStrokeFfd_GetShowOnlySelectedStroke() { return FormSegStrokeFfd::GetInst()->GetShowOnlySelectedStroke(); }
  inline void FormSegStrokeFfd_SetDeformMode(const int& _mode) { return FormSegStrokeFfd::GetInst()->SetDeformMode(_mode); }
  inline void FormSegStrokeFfd_SetTransformMode(const int& _mode) { return FormSegStrokeFfd::GetInst()->SetTransformMode(_mode); }

}
