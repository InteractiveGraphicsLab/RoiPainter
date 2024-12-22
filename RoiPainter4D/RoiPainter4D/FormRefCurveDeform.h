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
	private: System::Windows::Forms::Button^ m_btn_convert_mask_mesh;
	private: System::Windows::Forms::Button^ m_btn_deform;
	private: System::Windows::Forms::Button^ m_btn_undo;
	private: System::Windows::Forms::Button^ m_btn_redo;
	private: System::Windows::Forms::Button^ m_copy_from_prev_frame;
	private: System::Windows::Forms::Button^ m_btn_copy_stroke_all_frame;
	private: System::Windows::Forms::NumericUpDown^ m_numbox_cpsize;
	private: System::Windows::Forms::Label^ label4;
	private: System::Windows::Forms::CheckBox^ m_checkbox_showonlyselectedstroke;
	private: System::Windows::Forms::Button^ m_btn_convert_mesh_mask;
	private: System::Windows::Forms::TrackBar^ m_trackbar_mcscale;

	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::Label^ label5;






	private: System::Windows::Forms::Button^ m_btn_reload_mesh;
	private: System::Windows::Forms::Button^ m_btn_sharestroke;
	private: System::Windows::Forms::Button^ m_btn_loadstate;
	private: System::Windows::Forms::Button^ m_btn_savestate;



	protected:

	protected:

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
			this->m_btn_convert_mask_mesh = (gcnew System::Windows::Forms::Button());
			this->m_btn_deform = (gcnew System::Windows::Forms::Button());
			this->m_btn_undo = (gcnew System::Windows::Forms::Button());
			this->m_btn_redo = (gcnew System::Windows::Forms::Button());
			this->m_copy_from_prev_frame = (gcnew System::Windows::Forms::Button());
			this->m_btn_copy_stroke_all_frame = (gcnew System::Windows::Forms::Button());
			this->m_numbox_cpsize = (gcnew System::Windows::Forms::NumericUpDown());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->m_checkbox_showonlyselectedstroke = (gcnew System::Windows::Forms::CheckBox());
			this->m_btn_convert_mesh_mask = (gcnew System::Windows::Forms::Button());
			this->m_trackbar_mcscale = (gcnew System::Windows::Forms::TrackBar());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->m_btn_reload_mesh = (gcnew System::Windows::Forms::Button());
			this->m_btn_sharestroke = (gcnew System::Windows::Forms::Button());
			this->m_btn_loadstate = (gcnew System::Windows::Forms::Button());
			this->m_btn_savestate = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numbox_cpsize))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_mcscale))->BeginInit();
			this->SuspendLayout();
			// 
			// m_btn_convert_mask_mesh
			// 
			this->m_btn_convert_mask_mesh->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->m_btn_convert_mask_mesh->Location = System::Drawing::Point(8, 10);
			this->m_btn_convert_mask_mesh->Name = L"m_btn_convert_mask_mesh";
			this->m_btn_convert_mask_mesh->Size = System::Drawing::Size(135, 23);
			this->m_btn_convert_mask_mesh->TabIndex = 0;
			this->m_btn_convert_mask_mesh->Text = L"Convert mask -> mesh";
			this->m_btn_convert_mask_mesh->UseVisualStyleBackColor = true;
			this->m_btn_convert_mask_mesh->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_convert_mask_mesh_Click);
			// 
			// m_btn_deform
			// 
			this->m_btn_deform->Location = System::Drawing::Point(8, 83);
			this->m_btn_deform->Name = L"m_btn_deform";
			this->m_btn_deform->Size = System::Drawing::Size(135, 23);
			this->m_btn_deform->TabIndex = 1;
			this->m_btn_deform->Text = L"Deform";
			this->m_btn_deform->UseVisualStyleBackColor = true;
			this->m_btn_deform->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_deform_Click);
			// 
			// m_btn_undo
			// 
			this->m_btn_undo->Location = System::Drawing::Point(8, 145);
			this->m_btn_undo->Name = L"m_btn_undo";
			this->m_btn_undo->Size = System::Drawing::Size(65, 23);
			this->m_btn_undo->TabIndex = 2;
			this->m_btn_undo->Text = L"UNDO";
			this->m_btn_undo->UseVisualStyleBackColor = true;
			this->m_btn_undo->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_undo_Click);
			// 
			// m_btn_redo
			// 
			this->m_btn_redo->Location = System::Drawing::Point(80, 145);
			this->m_btn_redo->Name = L"m_btn_redo";
			this->m_btn_redo->Size = System::Drawing::Size(63, 23);
			this->m_btn_redo->TabIndex = 2;
			this->m_btn_redo->Text = L"REDO";
			this->m_btn_redo->UseVisualStyleBackColor = true;
			this->m_btn_redo->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_redo_Click);
			// 
			// m_copy_from_prev_frame
			// 
			this->m_copy_from_prev_frame->Location = System::Drawing::Point(149, 145);
			this->m_copy_from_prev_frame->Name = L"m_copy_from_prev_frame";
			this->m_copy_from_prev_frame->Size = System::Drawing::Size(144, 23);
			this->m_copy_from_prev_frame->TabIndex = 1;
			this->m_copy_from_prev_frame->Text = L"Copy from prev frame";
			this->m_copy_from_prev_frame->UseVisualStyleBackColor = true;
			this->m_copy_from_prev_frame->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_copy_from_prev_frame_Click);
			// 
			// m_btn_copy_stroke_all_frame
			// 
			this->m_btn_copy_stroke_all_frame->Location = System::Drawing::Point(149, 174);
			this->m_btn_copy_stroke_all_frame->Name = L"m_btn_copy_stroke_all_frame";
			this->m_btn_copy_stroke_all_frame->Size = System::Drawing::Size(144, 23);
			this->m_btn_copy_stroke_all_frame->TabIndex = 1;
			this->m_btn_copy_stroke_all_frame->Text = L"Copy strokes to all frame";
			this->m_btn_copy_stroke_all_frame->UseVisualStyleBackColor = true;
			this->m_btn_copy_stroke_all_frame->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_copy_stroke_all_frame_Click);
			// 
			// m_numbox_cpsize
			// 
			this->m_numbox_cpsize->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.5F));
			this->m_numbox_cpsize->Location = System::Drawing::Point(233, 221);
			this->m_numbox_cpsize->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->m_numbox_cpsize->Name = L"m_numbox_cpsize";
			this->m_numbox_cpsize->Size = System::Drawing::Size(57, 23);
			this->m_numbox_cpsize->TabIndex = 25;
			this->m_numbox_cpsize->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->m_numbox_cpsize->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
			this->m_numbox_cpsize->ValueChanged += gcnew System::EventHandler(this, &FormRefCurveDeform::m_numbox_cpsize_ValueChanged);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(181, 226);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(46, 12);
			this->label4->TabIndex = 26;
			this->label4->Text = L"CP size:";
			// 
			// m_checkbox_showonlyselectedstroke
			// 
			this->m_checkbox_showonlyselectedstroke->AutoSize = true;
			this->m_checkbox_showonlyselectedstroke->Checked = true;
			this->m_checkbox_showonlyselectedstroke->CheckState = System::Windows::Forms::CheckState::Checked;
			this->m_checkbox_showonlyselectedstroke->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 8));
			this->m_checkbox_showonlyselectedstroke->Location = System::Drawing::Point(149, 250);
			this->m_checkbox_showonlyselectedstroke->Name = L"m_checkbox_showonlyselectedstroke";
			this->m_checkbox_showonlyselectedstroke->Size = System::Drawing::Size(148, 15);
			this->m_checkbox_showonlyselectedstroke->TabIndex = 27;
			this->m_checkbox_showonlyselectedstroke->Text = L"Show only selected stroke";
			this->m_checkbox_showonlyselectedstroke->UseVisualStyleBackColor = true;
			this->m_checkbox_showonlyselectedstroke->CheckedChanged += gcnew System::EventHandler(this, &FormRefCurveDeform::m_checkbox_showonlyselectedstroke_CheckedChanged);
			// 
			// m_btn_convert_mesh_mask
			// 
			this->m_btn_convert_mesh_mask->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->m_btn_convert_mesh_mask->Location = System::Drawing::Point(8, 245);
			this->m_btn_convert_mesh_mask->Name = L"m_btn_convert_mesh_mask";
			this->m_btn_convert_mesh_mask->Size = System::Drawing::Size(135, 23);
			this->m_btn_convert_mesh_mask->TabIndex = 0;
			this->m_btn_convert_mesh_mask->Text = L"Convert mesh -> mask";
			this->m_btn_convert_mesh_mask->UseVisualStyleBackColor = true;
			this->m_btn_convert_mesh_mask->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_convert_mesh_mask_Click);
			// 
			// m_trackbar_mcscale
			// 
			this->m_trackbar_mcscale->LargeChange = 1;
			this->m_trackbar_mcscale->Location = System::Drawing::Point(147, 10);
			this->m_trackbar_mcscale->Maximum = 4;
			this->m_trackbar_mcscale->Minimum = 1;
			this->m_trackbar_mcscale->Name = L"m_trackbar_mcscale";
			this->m_trackbar_mcscale->Size = System::Drawing::Size(139, 45);
			this->m_trackbar_mcscale->TabIndex = 28;
			this->m_trackbar_mcscale->Value = 2;
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
			this->m_btn_reload_mesh->Location = System::Drawing::Point(8, 112);
			this->m_btn_reload_mesh->Name = L"m_btn_reload_mesh";
			this->m_btn_reload_mesh->Size = System::Drawing::Size(135, 23);
			this->m_btn_reload_mesh->TabIndex = 0;
			this->m_btn_reload_mesh->Text = L"Reload mesh";
			this->m_btn_reload_mesh->UseVisualStyleBackColor = true;
			this->m_btn_reload_mesh->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_reload_mesh_Click);
			// 
			// m_btn_sharestroke
			// 
			this->m_btn_sharestroke->Location = System::Drawing::Point(8, 174);
			this->m_btn_sharestroke->Name = L"m_btn_sharestroke";
			this->m_btn_sharestroke->Size = System::Drawing::Size(135, 23);
			this->m_btn_sharestroke->TabIndex = 1;
			this->m_btn_sharestroke->Text = L"Set as all frame curve";
			this->m_btn_sharestroke->UseVisualStyleBackColor = true;
			this->m_btn_sharestroke->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_sharestroke_Click);
			// 
			// m_btn_loadstate
			// 
			this->m_btn_loadstate->Location = System::Drawing::Point(149, 83);
			this->m_btn_loadstate->Name = L"m_btn_loadstate";
			this->m_btn_loadstate->Size = System::Drawing::Size(144, 23);
			this->m_btn_loadstate->TabIndex = 1;
			this->m_btn_loadstate->Text = L"Load state";
			this->m_btn_loadstate->UseVisualStyleBackColor = true;
			this->m_btn_loadstate->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_loadstate_Click);
			// 
			// m_btn_savestate
			// 
			this->m_btn_savestate->Location = System::Drawing::Point(149, 112);
			this->m_btn_savestate->Name = L"m_btn_savestate";
			this->m_btn_savestate->Size = System::Drawing::Size(144, 23);
			this->m_btn_savestate->TabIndex = 1;
			this->m_btn_savestate->Text = L"Save state";
			this->m_btn_savestate->UseVisualStyleBackColor = true;
			this->m_btn_savestate->Click += gcnew System::EventHandler(this, &FormRefCurveDeform::m_btn_savestate_Click);
			// 
			// FormRefCurveDeform
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(300, 316);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->m_trackbar_mcscale);
			this->Controls->Add(this->m_checkbox_showonlyselectedstroke);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->m_numbox_cpsize);
			this->Controls->Add(this->m_btn_redo);
			this->Controls->Add(this->m_btn_undo);
			this->Controls->Add(this->m_btn_copy_stroke_all_frame);
			this->Controls->Add(this->m_btn_sharestroke);
			this->Controls->Add(this->m_btn_savestate);
			this->Controls->Add(this->m_btn_loadstate);
			this->Controls->Add(this->m_copy_from_prev_frame);
			this->Controls->Add(this->m_btn_deform);
			this->Controls->Add(this->m_btn_convert_mesh_mask);
			this->Controls->Add(this->m_btn_reload_mesh);
			this->Controls->Add(this->m_btn_convert_mask_mesh);
			this->Name = L"FormRefCurveDeform";
			this->Text = L"RefCurveDeform";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numbox_cpsize))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_mcscale))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private:
		System::Void m_numbox_cpsize_ValueChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_convert_mask_mesh_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_reload_mesh_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_deform_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_undo_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_redo_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_copy_from_prev_frame_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_convert_mesh_mask_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_copy_stroke_all_frame_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_sharestroke_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_loadstate_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_savestate_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_checkbox_showonlyselectedstroke_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
	public:
		void LoadState();
		void SaveState();
		int GetCPSize();
		bool GetShowOnlySelectedStroke();
		int GetMCScale();
};

	inline void FormRefCurveDeform_Show() { FormRefCurveDeform::GetInst()->Show(); }
  inline void FormRefCurveDeform_Hide() { FormRefCurveDeform::GetInst()->Hide(); }
  inline void FormRefCurveDeform_InitAllItems() { FormRefCurveDeform::GetInst()->InitAllItems(); }
	inline void FormRefCurveDeform_LoadState() { FormRefCurveDeform::GetInst()->LoadState(); }
	inline void FormRefCurveDeform_SaveState() { FormRefCurveDeform::GetInst()->SaveState(); }
	inline int FormRefCurveDeform_GetCPSize() { return FormRefCurveDeform::GetInst()->GetCPSize(); }
	inline bool FormRefCurveDeform_GetShowOnlySelectedStroke() { return FormRefCurveDeform::GetInst()->GetShowOnlySelectedStroke(); }
	inline int FormRefCurveDeform_GetMCScale() { return FormRefCurveDeform::GetInst()->GetMCScale(); }

}
