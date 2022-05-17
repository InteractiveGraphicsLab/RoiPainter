#pragma once

namespace RoiPainter4D {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  /// <summary>
  /// FormSegRigidICP の概要
  /// </summary>
  public ref class FormSegRigidICP : public System::Windows::Forms::Form
  {

  private: 
    System::Windows::Forms::Button^ button_generateisosurfaces_;
    System::Windows::Forms::Button^ button_loadsourcesurface_;
    System::Windows::Forms::Button^ button_cancel_;
    System::Windows::Forms::Button^ button_generate_test_;
    System::Windows::Forms::Button^ button_runicp_current_;
    System::Windows::Forms::Button^ button_runicp_range_;
    System::Windows::Forms::Button^ button_runicp_allPara_;
    System::Windows::Forms::Button^ button_finish_currentframe;
    System::Windows::Forms::Button^ button_finish_;
    System::Windows::Forms::Button^ button_export_mats_;
    System::Windows::Forms::Button^ button_import_mats_;

    System::Windows::Forms::Label^ labelTransform;
    System::Windows::Forms::Label^ labelSetIsoValue;
    System::Windows::Forms::Label^ label1;
    System::Windows::Forms::RadioButton^ radiobutton_translate_;
    System::Windows::Forms::RadioButton^ radiobutton_rotate_;
    System::Windows::Forms::RadioButton^ radiobutton_transpivot_;

    System::Windows::Forms::TrackBar^ trackbar_setisoValue_;
    System::Windows::Forms::TextBox^ textbox_setisovalue_;
    System::Windows::Forms::GroupBox^ groupBoxSrcSurf;
    System::Windows::Forms::GroupBox^ groupBox1;
    System::Windows::Forms::GroupBox^ groupBox2;


    FormSegRigidICP(void)
    {
      InitializeComponent();
      this->ControlBox = false;
      this->MaximizeBox = false;
    }

    static FormSegRigidICP^ m_singleton;
  public:
    static FormSegRigidICP^ GetInst() {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormSegRigidICP();
      }
      return m_singleton;
    }

    void InitializeParameters();
    void ReusePrevIsoSurface( const int isovalue );


    bool isTrans()   { return FormSegRigidICP::GetInst()->radiobutton_translate_ ->Checked; };
    bool isRot()     { return FormSegRigidICP::GetInst()->radiobutton_rotate_    ->Checked; };
    bool isTransPiv(){ return FormSegRigidICP::GetInst()->radiobutton_transpivot_->Checked; };

  private:
    bool b_loaded_source_surface_;
    bool b_generated_all_isosurfaces_;    

  protected:
    /// <summary>
    /// 使用中のリソースをすべてクリーンアップします。
    /// </summary>
    ~FormSegRigidICP()
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
        this->button_generateisosurfaces_ = (gcnew System::Windows::Forms::Button());
        this->button_loadsourcesurface_ = (gcnew System::Windows::Forms::Button());
        this->labelTransform = (gcnew System::Windows::Forms::Label());
        this->radiobutton_translate_ = (gcnew System::Windows::Forms::RadioButton());
        this->radiobutton_rotate_ = (gcnew System::Windows::Forms::RadioButton());
        this->button_cancel_ = (gcnew System::Windows::Forms::Button());
        this->trackbar_setisoValue_ = (gcnew System::Windows::Forms::TrackBar());
        this->textbox_setisovalue_ = (gcnew System::Windows::Forms::TextBox());
        this->labelSetIsoValue = (gcnew System::Windows::Forms::Label());
        this->groupBoxSrcSurf = (gcnew System::Windows::Forms::GroupBox());
        this->radiobutton_transpivot_ = (gcnew System::Windows::Forms::RadioButton());
        this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
        this->button_generate_test_ = (gcnew System::Windows::Forms::Button());
        this->label1 = (gcnew System::Windows::Forms::Label());
        this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
        this->button_runicp_current_ = (gcnew System::Windows::Forms::Button());
        this->button_runicp_allPara_ = (gcnew System::Windows::Forms::Button());
        this->button_runicp_range_ = (gcnew System::Windows::Forms::Button());
        this->button_finish_currentframe = (gcnew System::Windows::Forms::Button());
        this->button_finish_ = (gcnew System::Windows::Forms::Button());
        this->button_export_mats_ = (gcnew System::Windows::Forms::Button());
        this->button_import_mats_ = (gcnew System::Windows::Forms::Button());
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_setisoValue_))->BeginInit();
        this->groupBoxSrcSurf->SuspendLayout();
        this->groupBox1->SuspendLayout();
        this->groupBox2->SuspendLayout();
        this->SuspendLayout();
        // 
        // button_generateisosurfaces_
        // 
        this->button_generateisosurfaces_->Location = System::Drawing::Point(16, 80);
        this->button_generateisosurfaces_->Name = L"button_generateisosurfaces_";
        this->button_generateisosurfaces_->Size = System::Drawing::Size(156, 23);
        this->button_generateisosurfaces_->TabIndex = 10;
        this->button_generateisosurfaces_->Text = L"Generate Iso Surfaces";
        this->button_generateisosurfaces_->UseVisualStyleBackColor = true;
        this->button_generateisosurfaces_->Click += gcnew System::EventHandler(this, &FormSegRigidICP::BtnGenIsoSurfClick);
        // 
        // button_loadsourcesurface_
        // 
        this->button_loadsourcesurface_->Location = System::Drawing::Point(18, 13);
        this->button_loadsourcesurface_->Name = L"button_loadsourcesurface_";
        this->button_loadsourcesurface_->Size = System::Drawing::Size(154, 23);
        this->button_loadsourcesurface_->TabIndex = 1;
        this->button_loadsourcesurface_->Text = L"Load Source Surface";
        this->button_loadsourcesurface_->UseVisualStyleBackColor = true;
        this->button_loadsourcesurface_->Click += gcnew System::EventHandler(this, &FormSegRigidICP::BtnLoadSurfaceClick);
        // 
        // labelTransform
        // 
        this->labelTransform->AutoSize = true;
        this->labelTransform->Location = System::Drawing::Point(15, 42);
        this->labelTransform->Name = L"labelTransform";
        this->labelTransform->Size = System::Drawing::Size(131, 12);
        this->labelTransform->TabIndex = 2;
        this->labelTransform->Text = L"Transform (Shift + Drag)";
        // 
        // radiobutton_translate_
        // 
        this->radiobutton_translate_->AutoSize = true;
        this->radiobutton_translate_->Checked = true;
        this->radiobutton_translate_->Location = System::Drawing::Point(19, 58);
        this->radiobutton_translate_->Name = L"radiobutton_translate_";
        this->radiobutton_translate_->Size = System::Drawing::Size(68, 16);
        this->radiobutton_translate_->TabIndex = 3;
        this->radiobutton_translate_->TabStop = true;
        this->radiobutton_translate_->Text = L"translate";
        this->radiobutton_translate_->UseVisualStyleBackColor = true;
        this->radiobutton_translate_->CheckedChanged += gcnew System::EventHandler(this, &FormSegRigidICP::radioBtnTranslateCheckedChanged);
        // 
        // radiobutton_rotate_
        // 
        this->radiobutton_rotate_->AutoSize = true;
        this->radiobutton_rotate_->Location = System::Drawing::Point(96, 58);
        this->radiobutton_rotate_->Name = L"radiobutton_rotate_";
        this->radiobutton_rotate_->Size = System::Drawing::Size(53, 16);
        this->radiobutton_rotate_->TabIndex = 4;
        this->radiobutton_rotate_->Text = L"rotate";
        this->radiobutton_rotate_->UseVisualStyleBackColor = true;
        this->radiobutton_rotate_->CheckedChanged += gcnew System::EventHandler(this, &FormSegRigidICP::RadioBtnRotateCheckedChanged);
        // 
        // button_cancel_
        // 
        this->button_cancel_->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->button_cancel_->Location = System::Drawing::Point(140, 398);
        this->button_cancel_->Name = L"button_cancel_";
        this->button_cancel_->Size = System::Drawing::Size(48, 33);
        this->button_cancel_->TabIndex = 14;
        this->button_cancel_->Text = L"Cancel";
        this->button_cancel_->UseVisualStyleBackColor = true;
        this->button_cancel_->Click += gcnew System::EventHandler(this, &FormSegRigidICP::BtnCancelClick);
        // 
        // trackbar_setisoValue_
        // 
        this->trackbar_setisoValue_->Location = System::Drawing::Point(76, 29);
        this->trackbar_setisoValue_->Name = L"trackbar_setisoValue_";
        this->trackbar_setisoValue_->Size = System::Drawing::Size(104, 45);
        this->trackbar_setisoValue_->TabIndex = 8;
        this->trackbar_setisoValue_->TabStop = false;
        this->trackbar_setisoValue_->TickStyle = System::Windows::Forms::TickStyle::None;
        this->trackbar_setisoValue_->Scroll += gcnew System::EventHandler(this, &FormSegRigidICP::TrackBarSetIsoValueScroll);
        // 
        // textbox_setisovalue_
        // 
        this->textbox_setisovalue_->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
        this->textbox_setisovalue_->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
        this->textbox_setisovalue_->Location = System::Drawing::Point(18, 28);
        this->textbox_setisovalue_->Name = L"textbox_setisovalue_";
        this->textbox_setisovalue_->Size = System::Drawing::Size(57, 23);
        this->textbox_setisovalue_->TabIndex = 7;
        this->textbox_setisovalue_->TextChanged += gcnew System::EventHandler(this, &FormSegRigidICP::TextBoxSetIsoValueTextChanged);
        // 
        // labelSetIsoValue
        // 
        this->labelSetIsoValue->AutoSize = true;
        this->labelSetIsoValue->Location = System::Drawing::Point(16, 13);
        this->labelSetIsoValue->Name = L"labelSetIsoValue";
        this->labelSetIsoValue->Size = System::Drawing::Size(29, 12);
        this->labelSetIsoValue->TabIndex = 6;
        this->labelSetIsoValue->Text = L"閾値";
        // 
        // groupBoxSrcSurf
        // 
        this->groupBoxSrcSurf->Controls->Add(this->radiobutton_transpivot_);
        this->groupBoxSrcSurf->Controls->Add(this->button_loadsourcesurface_);
        this->groupBoxSrcSurf->Controls->Add(this->labelTransform);
        this->groupBoxSrcSurf->Controls->Add(this->radiobutton_translate_);
        this->groupBoxSrcSurf->Controls->Add(this->radiobutton_rotate_);
        this->groupBoxSrcSurf->Location = System::Drawing::Point(12, 5);
        this->groupBoxSrcSurf->Name = L"groupBoxSrcSurf";
        this->groupBoxSrcSurf->Size = System::Drawing::Size(182, 100);
        this->groupBoxSrcSurf->TabIndex = 0;
        this->groupBoxSrcSurf->TabStop = false;
        this->groupBoxSrcSurf->Text = L"Source Surface";
        // 
        // radiobutton_transpivot_
        // 
        this->radiobutton_transpivot_->AutoSize = true;
        this->radiobutton_transpivot_->Location = System::Drawing::Point(19, 80);
        this->radiobutton_transpivot_->Name = L"radiobutton_transpivot_";
        this->radiobutton_transpivot_->Size = System::Drawing::Size(115, 16);
        this->radiobutton_transpivot_->TabIndex = 5;
        this->radiobutton_transpivot_->Text = L"translate rot pivot";
        this->radiobutton_transpivot_->UseVisualStyleBackColor = true;
        // 
        // groupBox1
        // 
        this->groupBox1->Controls->Add(this->button_generate_test_);
        this->groupBox1->Controls->Add(this->button_generateisosurfaces_);
        this->groupBox1->Controls->Add(this->textbox_setisovalue_);
        this->groupBox1->Controls->Add(this->labelSetIsoValue);
        this->groupBox1->Controls->Add(this->trackbar_setisoValue_);
        this->groupBox1->Location = System::Drawing::Point(12, 108);
        this->groupBox1->Name = L"groupBox1";
        this->groupBox1->Size = System::Drawing::Size(182, 110);
        this->groupBox1->TabIndex = 5;
        this->groupBox1->TabStop = false;
        this->groupBox1->Text = L"IsoSurface";
        // 
        // button_generate_test_
        // 
        this->button_generate_test_->Location = System::Drawing::Point(16, 55);
        this->button_generate_test_->Name = L"button_generate_test_";
        this->button_generate_test_->Size = System::Drawing::Size(156, 23);
        this->button_generate_test_->TabIndex = 9;
        this->button_generate_test_->Text = L"Gen Test (only 1frame)";
        this->button_generate_test_->UseVisualStyleBackColor = true;
        this->button_generate_test_->Click += gcnew System::EventHandler(this, &FormSegRigidICP::btnGenTest_Click);
        // 
        // label1
        // 
        this->label1->AutoSize = true;
        this->label1->Location = System::Drawing::Point(17, 222);
        this->label1->Name = L"label1";
        this->label1->Size = System::Drawing::Size(140, 12);
        this->label1->TabIndex = 6;
        this->label1->Text = L"Tab: Show / hide surfaces";
        // 
        // groupBox2
        // 
        this->groupBox2->Controls->Add(this->button_runicp_current_);
        this->groupBox2->Controls->Add(this->button_runicp_allPara_);
        this->groupBox2->Controls->Add(this->button_runicp_range_);
        this->groupBox2->Location = System::Drawing::Point(14, 244);
        this->groupBox2->Name = L"groupBox2";
        this->groupBox2->Size = System::Drawing::Size(180, 134);
        this->groupBox2->TabIndex = 11;
        this->groupBox2->TabStop = false;
        this->groupBox2->Text = L"Run Tracking with ICP";
        // 
        // button_runicp_current_
        // 
        this->button_runicp_current_->Location = System::Drawing::Point(14, 73);
        this->button_runicp_current_->Name = L"button_runicp_current_";
        this->button_runicp_current_->Size = System::Drawing::Size(160, 20);
        this->button_runicp_current_->TabIndex = 13;
        this->button_runicp_current_->Text = L"Current frame only";
        this->button_runicp_current_->UseVisualStyleBackColor = true;
        this->button_runicp_current_->Click += gcnew System::EventHandler(this, &FormSegRigidICP::BtnRunICPCurrentClick);
        // 
        // button_runicp_allPara_
        // 
        this->button_runicp_allPara_->Location = System::Drawing::Point(14, 15);
        this->button_runicp_allPara_->Name = L"button_runicp_allPara_";
        this->button_runicp_allPara_->Size = System::Drawing::Size(160, 26);
        this->button_runicp_allPara_->TabIndex = 12;
        this->button_runicp_allPara_->Text = L"Parallel (for small motion)";
        this->button_runicp_allPara_->UseVisualStyleBackColor = true;
        this->button_runicp_allPara_->Click += gcnew System::EventHandler(this, &FormSegRigidICP::BtnRunICPAllParaClick);
        // 
        // button_runicp_range_
        // 
        this->button_runicp_range_->Location = System::Drawing::Point(14, 44);
        this->button_runicp_range_->Name = L"button_runicp_range_";
        this->button_runicp_range_->Size = System::Drawing::Size(160, 26);
        this->button_runicp_range_->TabIndex = 12;
        this->button_runicp_range_->Text = L"Sequential (for large motion)";
        this->button_runicp_range_->UseVisualStyleBackColor = true;
        this->button_runicp_range_->Click += gcnew System::EventHandler(this, &FormSegRigidICP::BtnRunICPClick);
        // 
        // button_finish_currentframe
        // 
        this->button_finish_currentframe->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 8.25F, System::Drawing::FontStyle::Regular,
            System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(128)));
        this->button_finish_currentframe->Location = System::Drawing::Point(12, 386);
        this->button_finish_currentframe->Name = L"button_finish_currentframe";
        this->button_finish_currentframe->Size = System::Drawing::Size(117, 24);
        this->button_finish_currentframe->TabIndex = 16;
        this->button_finish_currentframe->Text = L"Store current frame";
        this->button_finish_currentframe->UseVisualStyleBackColor = true;
        this->button_finish_currentframe->Click += gcnew System::EventHandler(this, &FormSegRigidICP::BtnFinishandStoreCurrentFrameClick);
        // 
        // button_finish_
        // 
        this->button_finish_->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->button_finish_->Location = System::Drawing::Point(13, 413);
        this->button_finish_->Name = L"button_finish_";
        this->button_finish_->Size = System::Drawing::Size(116, 29);
        this->button_finish_->TabIndex = 14;
        this->button_finish_->Text = L"Store All Frame";
        this->button_finish_->UseVisualStyleBackColor = true;
        this->button_finish_->Click += gcnew System::EventHandler(this, &FormSegRigidICP::BtnFinishandStoreAllFrameClick);
        // 
        // button_export_mats_
        // 
        this->button_export_mats_->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->button_export_mats_->Location = System::Drawing::Point(28, 340);
        this->button_export_mats_->Name = L"button_export_mats_";
        this->button_export_mats_->Size = System::Drawing::Size(70, 31);
        this->button_export_mats_->TabIndex = 17;
        this->button_export_mats_->Text = L"Export     ICP mats";
        this->button_export_mats_->UseVisualStyleBackColor = true;
        this->button_export_mats_->Click += gcnew System::EventHandler(this, &FormSegRigidICP::button_export_mats__Click);
        // 
        // button_import_mats_
        // 
        this->button_import_mats_->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
            static_cast<System::Byte>(128)));
        this->button_import_mats_->Location = System::Drawing::Point(104, 340);
        this->button_import_mats_->Name = L"button_import_mats_";
        this->button_import_mats_->Size = System::Drawing::Size(70, 31);
        this->button_import_mats_->TabIndex = 18;
        this->button_import_mats_->Text = L"Import     ICP mats";
        this->button_import_mats_->UseVisualStyleBackColor = true;
        this->button_import_mats_->Click += gcnew System::EventHandler(this, &FormSegRigidICP::button_import_mats__Click);
        // 
        // FormSegRigidICP
        // 
        this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
        this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
        this->ClientSize = System::Drawing::Size(201, 448);
        this->Controls->Add(this->button_import_mats_);
        this->Controls->Add(this->button_export_mats_);
        this->Controls->Add(this->button_finish_);
        this->Controls->Add(this->button_finish_currentframe);
        this->Controls->Add(this->groupBox2);
        this->Controls->Add(this->groupBox1);
        this->Controls->Add(this->label1);
        this->Controls->Add(this->groupBoxSrcSurf);
        this->Controls->Add(this->button_cancel_);
        this->Name = L"FormSegRigidICP";
        this->Text = L"FormSegRigidICP";
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_setisoValue_))->EndInit();
        this->groupBoxSrcSurf->ResumeLayout(false);
        this->groupBoxSrcSurf->PerformLayout();
        this->groupBox1->ResumeLayout(false);
        this->groupBox1->PerformLayout();
        this->groupBox2->ResumeLayout(false);
        this->ResumeLayout(false);
        this->PerformLayout();

    }
#pragma endregion
  private: System::Void BtnLoadSurfaceClick		(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnGenTest_Click				(System::Object^  sender, System::EventArgs^  e);
	private: System::Void BtnGenIsoSurfClick			(System::Object^  sender, System::EventArgs^  e);
  private: System::Void BtnRunICPClick					(System::Object^  sender, System::EventArgs^  e);
  private: System::Void BtnFinishandStoreAllFrameClick			(System::Object^  sender, System::EventArgs^  e);
  private: System::Void BtnFinishandStoreCurrentFrameClick(System::Object^ sender, System::EventArgs^ e);
  private: System::Void BtnCancelClick	(System::Object^  sender, System::EventArgs^  e);

  private: System::Void TextBoxSetIsoValueTextChanged	(System::Object^  sender, System::EventArgs^  e);
  private: System::Void TrackBarSetIsoValueScroll			(System::Object^  sender, System::EventArgs^  e);
	private: System::Void RadioBtnRotateCheckedChanged		(System::Object^  sender, System::EventArgs^  e) {}
  private: System::Void radioBtnTranslateCheckedChanged(System::Object^  sender, System::EventArgs^  e) {}
  private: System::Void BtnRunICPAllParaClick(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void BtnRunICPCurrentClick(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void button_export_mats__Click(System::Object^  sender, System::EventArgs^  e); 
  private: System::Void button_import_mats__Click(System::Object^  sender, System::EventArgs^  e);
};


  inline void FormSegRigidICPShow()			 { FormSegRigidICP::GetInst()->Show(); }
  inline void FormSegRigidICPHide()			 { FormSegRigidICP::GetInst()->Hide(); }
  inline void FormSegRigidICPInitializeParameters() { FormSegRigidICP::GetInst()->InitializeParameters(); }
  inline bool FormSegRigidICPBTrans()		 { return FormSegRigidICP::GetInst()->isTrans(); }
  inline bool FormSegRigidICPBRot()			 { return FormSegRigidICP::GetInst()->isRot(); }
  inline bool FormSegRigidICPBTransPiv() { return FormSegRigidICP::GetInst()->isTransPiv(); }


  inline void FormSegRigidICP_ReusePrevIsoSurface( const int isovalue ){
    FormSegRigidICP::GetInst()->ReusePrevIsoSurface(isovalue);
  }

}
