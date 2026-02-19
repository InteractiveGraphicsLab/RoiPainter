#pragma once

namespace RoiPainter4D {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  /// <summary>
  /// FormRefCurveDeform の概要
  /// </summary>
  public ref class FormRefCurveDeform : public System::Windows::Forms::Form
  {
  private:
    static FormRefCurveDeform^ m_singleton;
    FormRefCurveDeform(void)
    {
      InitializeComponent();
    }

  public: 
    static FormRefCurveDeform^ GetInst()
    {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormRefCurveDeform();
      }
      return m_singleton;
    }

    void InitAllItems();

  protected:
  	/// <summary>
  	/// 使用中のリソースをすべてクリーンアップします。
  	/// </summary>
  	~FormRefCurveDeform()
  	{
  		if (components)
  		{
  			delete components;
  		}
  	}
  private: System::Windows::Forms::NumericUpDown^ m_numbox_cpsize;
  private: System::Windows::Forms::TrackBar^ m_trackbar_mcstride;
  
  private: System::Windows::Forms::Label^ label1;
  private: System::Windows::Forms::Label^ label2;
  private: System::Windows::Forms::Label^ label3;
  private: System::Windows::Forms::Label^ label4;
  private: System::Windows::Forms::Label^ label5;
  
  private: System::Windows::Forms::Button^ m_btn_reload_mesh;
  private: System::Windows::Forms::Button^ m_btn_sharestroke;
  private: System::Windows::Forms::Button^ m_btn_loadstate;
  private: System::Windows::Forms::Button^ m_btn_savestate;
  private: System::Windows::Forms::Button^ m_btn_flipnormal;
  private: System::Windows::Forms::Button^ m_btn_genmesh;
  private: System::Windows::Forms::Button^ m_btn_deform;
  private: System::Windows::Forms::Button^ m_btn_undo;
  
  private: System::Windows::Forms::Button^ m_btn_copy_from_preframe;
  private: System::Windows::Forms::Button^ m_btn_copy_to_allframe;
  
  private: System::Windows::Forms::CheckBox^ m_cb_only_select_curve;
  private: System::Windows::Forms::Button^ m_btn_deform_all;
  private: System::Windows::Forms::Button^ m_btn_cancel;
  private: System::Windows::Forms::Button^ m_btn_finish;
  
  
  
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
      this->m_btn_genmesh = (gcnew System::Windows::Forms::Button());
      this->m_btn_deform = (gcnew System::Windows::Forms::Button());
      this->m_btn_undo = (gcnew System::Windows::Forms::Button());
      this->m_btn_copy_from_preframe = (gcnew System::Windows::Forms::Button());
      this->m_btn_copy_to_allframe = (gcnew System::Windows::Forms::Button());
      this->m_numbox_cpsize = (gcnew System::Windows::Forms::NumericUpDown());
      this->m_cb_only_select_curve = (gcnew System::Windows::Forms::CheckBox());
      this->m_trackbar_mcstride = (gcnew System::Windows::Forms::TrackBar());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->label4 = (gcnew System::Windows::Forms::Label());
      this->label5 = (gcnew System::Windows::Forms::Label());
      this->m_btn_reload_mesh = (gcnew System::Windows::Forms::Button());
      this->m_btn_sharestroke = (gcnew System::Windows::Forms::Button());
      this->m_btn_loadstate = (gcnew System::Windows::Forms::Button());
      this->m_btn_savestate = (gcnew System::Windows::Forms::Button());
      this->m_btn_flipnormal = (gcnew System::Windows::Forms::Button());
      this->m_btn_deform_all = (gcnew System::Windows::Forms::Button());
      this->m_btn_cancel = (gcnew System::Windows::Forms::Button());
      this->m_btn_finish = (gcnew System::Windows::Forms::Button());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numbox_cpsize))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_mcstride))->BeginInit();
      this->SuspendLayout();
      // 
      // m_btn_genmesh
      // 
      this->m_btn_genmesh->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
    	static_cast<System::Byte>(128)));
      this->m_btn_genmesh->Location = System::Drawing::Point(8, 10);
      this->m_btn_genmesh->Name = L"m_btn_genmesh";
      this->m_btn_genmesh->Size = System::Drawing::Size(135, 23);
      this->m_btn_genmesh->TabIndex = 0;
      this->m_btn_genmesh->Text = L"Create Mesh Models";
      this->m_btn_genmesh->UseVisualStyleBackColor = true;
      this->m_btn_genmesh->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_genmesh_Click);
      // 
      // m_btn_deform
      // 
      this->m_btn_deform->Location = System::Drawing::Point(4, 252);
      this->m_btn_deform->Name = L"m_btn_deform";
      this->m_btn_deform->Size = System::Drawing::Size(135, 32);
      this->m_btn_deform->TabIndex = 1;
      this->m_btn_deform->Text = L"Deform Current Frame";
      this->m_btn_deform->UseVisualStyleBackColor = true;
      this->m_btn_deform->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_deform_Click);
      // 
      // m_btn_undo
      // 
      this->m_btn_undo->Location = System::Drawing::Point(28, 187);
      this->m_btn_undo->Name = L"m_btn_undo";
      this->m_btn_undo->Size = System::Drawing::Size(89, 23);
      this->m_btn_undo->TabIndex = 2;
      this->m_btn_undo->Text = L"UNDO";
      this->m_btn_undo->UseVisualStyleBackColor = true;
      this->m_btn_undo->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_undo_Click);
      // 
      // m_btn_copy_from_preframe
      // 
      this->m_btn_copy_from_preframe->Location = System::Drawing::Point(153, 118);
      this->m_btn_copy_from_preframe->Name = L"m_btn_copy_from_preframe";
      this->m_btn_copy_from_preframe->Size = System::Drawing::Size(142, 36);
      this->m_btn_copy_from_preframe->TabIndex = 1;
      this->m_btn_copy_from_preframe->Text = L"Copy curves from prev frame";
      this->m_btn_copy_from_preframe->UseVisualStyleBackColor = true;
      this->m_btn_copy_from_preframe->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_copy_from_preframe_Click);
      // 
      // m_btn_copy_to_allframe
      // 
      this->m_btn_copy_to_allframe->Location = System::Drawing::Point(153, 160);
      this->m_btn_copy_to_allframe->Name = L"m_btn_copy_to_allframe";
      this->m_btn_copy_to_allframe->Size = System::Drawing::Size(142, 36);
      this->m_btn_copy_to_allframe->TabIndex = 1;
      this->m_btn_copy_to_allframe->Text = L"Copy curves in this fame to all frame";
      this->m_btn_copy_to_allframe->UseVisualStyleBackColor = true;
      this->m_btn_copy_to_allframe->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_copy_to_allframe_Click);
      // 
      // m_numbox_cpsize
      // 
      this->m_numbox_cpsize->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.5F));
      this->m_numbox_cpsize->Location = System::Drawing::Point(69, 81);
      this->m_numbox_cpsize->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
      this->m_numbox_cpsize->Name = L"m_numbox_cpsize";
      this->m_numbox_cpsize->Size = System::Drawing::Size(57, 23);
      this->m_numbox_cpsize->TabIndex = 25;
      this->m_numbox_cpsize->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
      this->m_numbox_cpsize->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
      this->m_numbox_cpsize->ValueChanged += gcnew System::EventHandler(this, &FormRefCurveDeform::m_numbox_cpsize_ValueChanged);
      // 
      // m_cb_only_select_curve
      // 
      this->m_cb_only_select_curve->AutoSize = true;
      this->m_cb_only_select_curve->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 8));
      this->m_cb_only_select_curve->Location = System::Drawing::Point(147, 85);
      this->m_cb_only_select_curve->Name = L"m_cb_only_select_curve";
      this->m_cb_only_select_curve->Size = System::Drawing::Size(148, 15);
      this->m_cb_only_select_curve->TabIndex = 27;
      this->m_cb_only_select_curve->Text = L"Show only selected stroke";
      this->m_cb_only_select_curve->UseVisualStyleBackColor = true;
      this->m_cb_only_select_curve->CheckedChanged += gcnew System::EventHandler(this, &FormRefCurveDeform::m_cb_onlyselectedcurve_CheckedChanged);
      // 
      // m_trackbar_mcstride
      // 
      this->m_trackbar_mcstride->LargeChange = 1;
      this->m_trackbar_mcstride->Location = System::Drawing::Point(147, 10);
      this->m_trackbar_mcstride->Maximum = 4;
      this->m_trackbar_mcstride->Minimum = 1;
      this->m_trackbar_mcstride->Name = L"m_trackbar_mcstride";
      this->m_trackbar_mcstride->Size = System::Drawing::Size(139, 45);
      this->m_trackbar_mcstride->TabIndex = 28;
      this->m_trackbar_mcstride->Value = 2;
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Location = System::Drawing::Point(152, 43);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(17, 12);
      this->label1->TabIndex = 29;
      this->label1->Text = L"x1";
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Location = System::Drawing::Point(190, 43);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(17, 12);
      this->label2->TabIndex = 29;
      this->label2->Text = L"x2";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Location = System::Drawing::Point(228, 43);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(17, 12);
      this->label3->TabIndex = 29;
      this->label3->Text = L"x4";
      // 
      // label4
      // 
      this->label4->AutoSize = true;
      this->label4->Location = System::Drawing::Point(17, 85);
      this->label4->Name = L"label4";
      this->label4->Size = System::Drawing::Size(46, 12);
      this->label4->TabIndex = 26;
      this->label4->Text = L"CP size:";
      // 
      // label5
      // 
      this->label5->AutoSize = true;
      this->label5->Location = System::Drawing::Point(265, 43);
      this->label5->Name = L"label5";
      this->label5->Size = System::Drawing::Size(17, 12);
      this->label5->TabIndex = 29;
      this->label5->Text = L"x8";
      // 
      // m_btn_reload_mesh
      // 
      this->m_btn_reload_mesh->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
    	static_cast<System::Byte>(128)));
      this->m_btn_reload_mesh->Location = System::Drawing::Point(8, 38);
      this->m_btn_reload_mesh->Name = L"m_btn_reload_mesh";
      this->m_btn_reload_mesh->Size = System::Drawing::Size(135, 23);
      this->m_btn_reload_mesh->TabIndex = 0;
      this->m_btn_reload_mesh->Text = L"Reload mesh";
      this->m_btn_reload_mesh->UseVisualStyleBackColor = true;
      this->m_btn_reload_mesh->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_reload_mesh_Click);
      // 
      // m_btn_sharestroke
      // 
      this->m_btn_sharestroke->Location = System::Drawing::Point(6, 129);
      this->m_btn_sharestroke->Name = L"m_btn_sharestroke";
      this->m_btn_sharestroke->Size = System::Drawing::Size(133, 23);
      this->m_btn_sharestroke->TabIndex = 1;
      this->m_btn_sharestroke->Text = L"Set as all frame curve";
      this->m_btn_sharestroke->UseVisualStyleBackColor = true;
      this->m_btn_sharestroke->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_sharestroke_Click);
      // 
      // m_btn_loadstate
      // 
      this->m_btn_loadstate->Location = System::Drawing::Point(153, 202);
      this->m_btn_loadstate->Name = L"m_btn_loadstate";
      this->m_btn_loadstate->Size = System::Drawing::Size(68, 32);
      this->m_btn_loadstate->TabIndex = 1;
      this->m_btn_loadstate->Text = L"Load state";
      this->m_btn_loadstate->UseVisualStyleBackColor = true;
      this->m_btn_loadstate->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_loadstate_Click);
      // 
      // m_btn_savestate
      // 
      this->m_btn_savestate->Location = System::Drawing::Point(227, 202);
      this->m_btn_savestate->Name = L"m_btn_savestate";
      this->m_btn_savestate->Size = System::Drawing::Size(68, 32);
      this->m_btn_savestate->TabIndex = 1;
      this->m_btn_savestate->Text = L"Save state";
      this->m_btn_savestate->UseVisualStyleBackColor = true;
      this->m_btn_savestate->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_savestate_Click);
      // 
      // m_btn_flipnormal
      // 
      this->m_btn_flipnormal->Location = System::Drawing::Point(6, 158);
      this->m_btn_flipnormal->Name = L"m_btn_flipnormal";
      this->m_btn_flipnormal->Size = System::Drawing::Size(133, 23);
      this->m_btn_flipnormal->TabIndex = 30;
      this->m_btn_flipnormal->Text = L"flip normals";
      this->m_btn_flipnormal->UseVisualStyleBackColor = true;
      this->m_btn_flipnormal->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_flipnormal_Click);
      // 
      // m_btn_deform_all
      // 
      this->m_btn_deform_all->Location = System::Drawing::Point(154, 252);
      this->m_btn_deform_all->Name = L"m_btn_deform_all";
      this->m_btn_deform_all->Size = System::Drawing::Size(135, 32);
      this->m_btn_deform_all->TabIndex = 31;
      this->m_btn_deform_all->Text = L"Deform All Frame";
      this->m_btn_deform_all->UseVisualStyleBackColor = true;
      this->m_btn_deform_all->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_deform_all_Click);
      // 
      // m_cancel
      // 
      this->m_btn_cancel->Location = System::Drawing::Point(46, 309);
      this->m_btn_cancel->Name = L"m_cancel";
      this->m_btn_cancel->Size = System::Drawing::Size(93, 32);
      this->m_btn_cancel->TabIndex = 32;
      this->m_btn_cancel->Text = L"Cancel";
      this->m_btn_cancel->UseVisualStyleBackColor = true;
      this->m_btn_cancel->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_cancel_Click);
      // 
      // m_finish
      // 
      this->m_btn_finish->Location = System::Drawing::Point(154, 309);
      this->m_btn_finish->Name = L"m_finish";
      this->m_btn_finish->Size = System::Drawing::Size(135, 32);
      this->m_btn_finish->TabIndex = 33;
      this->m_btn_finish->Text = L"Finish Segmentation";
      this->m_btn_finish->UseVisualStyleBackColor = true;
      this->m_btn_finish->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_finish_Click);
      // 
      // FormRefCurveDeform
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(300, 397);
      this->Controls->Add(this->m_btn_finish);
      this->Controls->Add(this->m_btn_cancel);
      this->Controls->Add(this->m_btn_deform_all);
      this->Controls->Add(this->m_btn_flipnormal);
      this->Controls->Add(this->label5);
      this->Controls->Add(this->label3);
      this->Controls->Add(this->label2);
      this->Controls->Add(this->label1);
      this->Controls->Add(this->m_trackbar_mcstride);
      this->Controls->Add(this->m_cb_only_select_curve);
      this->Controls->Add(this->label4);
      this->Controls->Add(this->m_numbox_cpsize);
      this->Controls->Add(this->m_btn_undo);
      this->Controls->Add(this->m_btn_copy_to_allframe);
      this->Controls->Add(this->m_btn_sharestroke);
      this->Controls->Add(this->m_btn_savestate);
      this->Controls->Add(this->m_btn_loadstate);
      this->Controls->Add(this->m_btn_copy_from_preframe);
      this->Controls->Add(this->m_btn_deform);
      this->Controls->Add(this->m_btn_reload_mesh);
      this->Controls->Add(this->m_btn_genmesh);
      this->Name = L"FormRefCurveDeform";
      this->Text = L"RefCurveDeform";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numbox_cpsize))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_mcstride))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();
    
    }
#pragma endregion
	private:
	  System::Void m_numbox_cpsize_ValueChanged(System::Object^ sender, System::EventArgs^ e);
	  System::Void m_cancel_Click         (System::Object^ sender, System::EventArgs^ e);
	  System::Void m_finish_Click         (System::Object^ sender, System::EventArgs^ e);
	  System::Void m_btn_genmesh_Click    (System::Object^ sender, System::EventArgs^ e);
	  System::Void m_btn_reload_mesh_Click(System::Object^ sender, System::EventArgs^ e);
	  System::Void m_btn_deform_Click     (System::Object^ sender, System::EventArgs^ e);
	  System::Void m_btn_deform_all_Click(System::Object^ sender, System::EventArgs^ e);
	  System::Void m_btn_loadstate_Click  (System::Object^ sender, System::EventArgs^ e);
	  System::Void m_btn_savestate_Click  (System::Object^ sender, System::EventArgs^ e);
	  System::Void m_btn_undo_Click       (System::Object^ sender, System::EventArgs^ e);
	  System::Void m_btn_flipnormal_Click (System::Object^ sender, System::EventArgs^ e);
	  System::Void m_btn_copy_from_preframe_Click(System::Object^ sender, System::EventArgs^ e);
	  System::Void m_btn_copy_to_allframe_Click(System::Object^ sender, System::EventArgs^ e);
	  System::Void m_btn_sharestroke_Click(System::Object^ sender, System::EventArgs^ e);
	  System::Void m_cb_onlyselectedcurve_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
	public:
	  int  GetCpSize (){ return static_cast<int>(m_numbox_cpsize->Value); }
	  int  GetMcStride(){ return static_cast<int>(m_trackbar_mcstride->Value);}
	  bool bVisOnlySelectedCurve(){
	  	return static_cast<bool>(m_cb_only_select_curve->Checked);
	  }
};

	inline void FormRefCurveDeform_Show() { FormRefCurveDeform::GetInst()->Show(); }
	inline void FormRefCurveDeform_Hide() { FormRefCurveDeform::GetInst()->Hide(); }
	inline void FormRefCurveDeform_InitAllItems() { FormRefCurveDeform::GetInst()->InitAllItems(); }

	inline int  FormRefCurveDeform_GetCpSize()       { return FormRefCurveDeform::GetInst()->GetCpSize(); }
	inline bool FormRefCurveDeform_VisOnlySelCurve() { return FormRefCurveDeform::GetInst()->bVisOnlySelectedCurve(); }
	inline int  FormRefCurveDeform_GetMcStride()     { return FormRefCurveDeform::GetInst()->GetMcStride(); }
	
}


