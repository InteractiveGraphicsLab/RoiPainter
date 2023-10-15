#pragma once

namespace RoiPainter4D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormPlaceCPs の概要
	/// </summary>
	public ref class FormPlaceCPs : public System::Windows::Forms::Form
	{

	private:

		System::Windows::Forms::GroupBox^ groupBox1;
		System::Windows::Forms::TextBox^	textbox_setisovalue_;
		System::Windows::Forms::Label^		labelSetIsoValue;
		System::Windows::Forms::TrackBar^ trackbar_setisoValue_;

		System::Windows::Forms::Button^ button_generate_test_;
		System::Windows::Forms::Button^ button_generateisosurfaces_;
		System::Windows::Forms::Button^ button_export_cpinfo_;
		System::Windows::Forms::Button^ button_finish_placement_;
	private: System::Windows::Forms::Button^ button_load_mesh_;
	private: System::Windows::Forms::Button^ button_fit_template_;
	private: System::Windows::Forms::GroupBox^ groupBox2;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::CheckBox^ checkbox_modify_scale_;
				 System::Windows::Forms::Button^ button_inport_cpinfo_;


		FormPlaceCPs(void)
		{
			InitializeComponent();
			trackbar_setisoValue_->SetRange(-100, 1000);
			trackbar_setisoValue_->Value = 100;
			textbox_setisovalue_->Text = (100).ToString();
		}

	

		static FormPlaceCPs^ m_singleton;
	public:
		static FormPlaceCPs^ GetInst() {
			if (m_singleton == nullptr) {
				m_singleton = gcnew FormPlaceCPs();
			}
			return m_singleton;
		}

		void InitParams();


	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormPlaceCPs()
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
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->button_generate_test_ = (gcnew System::Windows::Forms::Button());
			this->button_generateisosurfaces_ = (gcnew System::Windows::Forms::Button());
			this->textbox_setisovalue_ = (gcnew System::Windows::Forms::TextBox());
			this->labelSetIsoValue = (gcnew System::Windows::Forms::Label());
			this->trackbar_setisoValue_ = (gcnew System::Windows::Forms::TrackBar());
			this->button_export_cpinfo_ = (gcnew System::Windows::Forms::Button());
			this->button_finish_placement_ = (gcnew System::Windows::Forms::Button());
			this->button_inport_cpinfo_ = (gcnew System::Windows::Forms::Button());
			this->button_load_mesh_ = (gcnew System::Windows::Forms::Button());
			this->button_fit_template_ = (gcnew System::Windows::Forms::Button());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->checkbox_modify_scale_ = (gcnew System::Windows::Forms::CheckBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_setisoValue_))->BeginInit();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->button_generate_test_);
			this->groupBox1->Controls->Add(this->button_generateisosurfaces_);
			this->groupBox1->Controls->Add(this->textbox_setisovalue_);
			this->groupBox1->Controls->Add(this->labelSetIsoValue);
			this->groupBox1->Controls->Add(this->trackbar_setisoValue_);
			this->groupBox1->Location = System::Drawing::Point(20, 6);
			this->groupBox1->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Padding = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->groupBox1->Size = System::Drawing::Size(341, 181);
			this->groupBox1->TabIndex = 6;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"IsoSurface";
			this->groupBox1->Enter += gcnew System::EventHandler(this, &FormPlaceCPs::groupBox1_Enter);
			// 
			// button_generate_test_
			// 
			this->button_generate_test_->Location = System::Drawing::Point(30, 96);
			this->button_generate_test_->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->button_generate_test_->Name = L"button_generate_test_";
			this->button_generate_test_->Size = System::Drawing::Size(295, 34);
			this->button_generate_test_->TabIndex = 9;
			this->button_generate_test_->Text = L"Gen Test (only 1frame)";
			this->button_generate_test_->UseVisualStyleBackColor = true;
			this->button_generate_test_->Click += gcnew System::EventHandler(this, &FormPlaceCPs::button_generate_test__Click);
			// 
			// button_generateisosurfaces_
			// 
			this->button_generateisosurfaces_->Location = System::Drawing::Point(30, 133);
			this->button_generateisosurfaces_->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->button_generateisosurfaces_->Name = L"button_generateisosurfaces_";
			this->button_generateisosurfaces_->Size = System::Drawing::Size(295, 34);
			this->button_generateisosurfaces_->TabIndex = 10;
			this->button_generateisosurfaces_->Text = L"Generate Iso Surfaces";
			this->button_generateisosurfaces_->UseVisualStyleBackColor = true;
			this->button_generateisosurfaces_->Click += gcnew System::EventHandler(this, &FormPlaceCPs::button_generateisosurfaces__Click);
			// 
			// textbox_setisovalue_
			// 
			this->textbox_setisovalue_->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->textbox_setisovalue_->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
			this->textbox_setisovalue_->Location = System::Drawing::Point(74, 19);
			this->textbox_setisovalue_->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->textbox_setisovalue_->Name = L"textbox_setisovalue_";
			this->textbox_setisovalue_->Size = System::Drawing::Size(94, 31);
			this->textbox_setisovalue_->TabIndex = 7;
			this->textbox_setisovalue_->TextChanged += gcnew System::EventHandler(this, &FormPlaceCPs::textbox_setisovalue__TextChanged);
			// 
			// labelSetIsoValue
			// 
			this->labelSetIsoValue->AutoSize = true;
			this->labelSetIsoValue->Location = System::Drawing::Point(27, 27);
			this->labelSetIsoValue->Margin = System::Windows::Forms::Padding(5, 0, 5, 0);
			this->labelSetIsoValue->Name = L"labelSetIsoValue";
			this->labelSetIsoValue->Size = System::Drawing::Size(44, 18);
			this->labelSetIsoValue->TabIndex = 6;
			this->labelSetIsoValue->Text = L"閾値";
			// 
			// trackbar_setisoValue_
			// 
			this->trackbar_setisoValue_->Location = System::Drawing::Point(30, 56);
			this->trackbar_setisoValue_->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->trackbar_setisoValue_->Name = L"trackbar_setisoValue_";
			this->trackbar_setisoValue_->Size = System::Drawing::Size(295, 69);
			this->trackbar_setisoValue_->TabIndex = 8;
			this->trackbar_setisoValue_->TabStop = false;
			this->trackbar_setisoValue_->TickStyle = System::Windows::Forms::TickStyle::None;
			this->trackbar_setisoValue_->Scroll += gcnew System::EventHandler(this, &FormPlaceCPs::trackbar_setisoValue__Scroll);
			// 
			// button_export_cpinfo_
			// 
			this->button_export_cpinfo_->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->button_export_cpinfo_->Location = System::Drawing::Point(17, 332);
			this->button_export_cpinfo_->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->button_export_cpinfo_->Name = L"button_export_cpinfo_";
			this->button_export_cpinfo_->Size = System::Drawing::Size(158, 45);
			this->button_export_cpinfo_->TabIndex = 20;
			this->button_export_cpinfo_->Text = L"Export  CP info";
			this->button_export_cpinfo_->UseVisualStyleBackColor = true;
			this->button_export_cpinfo_->Click += gcnew System::EventHandler(this, &FormPlaceCPs::button_export_cpinfo__Click);
			// 
			// button_finish_placement_
			// 
			this->button_finish_placement_->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(128)));
			this->button_finish_placement_->Location = System::Drawing::Point(180, 378);
			this->button_finish_placement_->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->button_finish_placement_->Name = L"button_finish_placement_";
			this->button_finish_placement_->Size = System::Drawing::Size(188, 46);
			this->button_finish_placement_->TabIndex = 19;
			this->button_finish_placement_->Text = L"Finish place CP";
			this->button_finish_placement_->UseVisualStyleBackColor = true;
			this->button_finish_placement_->Click += gcnew System::EventHandler(this, &FormPlaceCPs::button_finish_placement__Click);
			// 
			// button_inport_cpinfo_
			// 
			this->button_inport_cpinfo_->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->button_inport_cpinfo_->Location = System::Drawing::Point(188, 332);
			this->button_inport_cpinfo_->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->button_inport_cpinfo_->Name = L"button_inport_cpinfo_";
			this->button_inport_cpinfo_->Size = System::Drawing::Size(170, 45);
			this->button_inport_cpinfo_->TabIndex = 21;
			this->button_inport_cpinfo_->Text = L"Import CP info";
			this->button_inport_cpinfo_->UseVisualStyleBackColor = true;
			this->button_inport_cpinfo_->Click += gcnew System::EventHandler(this, &FormPlaceCPs::button_inport_cpinfo__Click);
			// 
			// button_load_mesh_
			// 
			this->button_load_mesh_->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->button_load_mesh_->Location = System::Drawing::Point(10, 38);
			this->button_load_mesh_->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->button_load_mesh_->Name = L"button_load_mesh_";
			this->button_load_mesh_->Size = System::Drawing::Size(158, 45);
			this->button_load_mesh_->TabIndex = 22;
			this->button_load_mesh_->Text = L"Load Template";
			this->button_load_mesh_->UseVisualStyleBackColor = true;
			this->button_load_mesh_->Click += gcnew System::EventHandler(this, &FormPlaceCPs::button_load_mesh__Click);
			// 
			// button_fit_template_
			// 
			this->button_fit_template_->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->button_fit_template_->Location = System::Drawing::Point(183, 54);
			this->button_fit_template_->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->button_fit_template_->Name = L"button_fit_template_";
			this->button_fit_template_->Size = System::Drawing::Size(148, 45);
			this->button_fit_template_->TabIndex = 23;
			this->button_fit_template_->Text = L"Fit template";
			this->button_fit_template_->UseVisualStyleBackColor = true;
			this->button_fit_template_->Click += gcnew System::EventHandler(this, &FormPlaceCPs::button_fit_template__Click);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->checkbox_modify_scale_);
			this->groupBox2->Controls->Add(this->label1);
			this->groupBox2->Controls->Add(this->button_fit_template_);
			this->groupBox2->Controls->Add(this->button_load_mesh_);
			this->groupBox2->Location = System::Drawing::Point(20, 204);
			this->groupBox2->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Padding = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->groupBox2->Size = System::Drawing::Size(341, 109);
			this->groupBox2->TabIndex = 11;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Template mesh";
			// 
			// checkbox_modify_scale_
			// 
			this->checkbox_modify_scale_->AutoSize = true;
			this->checkbox_modify_scale_->Location = System::Drawing::Point(183, 26);
			this->checkbox_modify_scale_->Name = L"checkbox_modify_scale_";
			this->checkbox_modify_scale_->Size = System::Drawing::Size(127, 22);
			this->checkbox_modify_scale_->TabIndex = 24;
			this->checkbox_modify_scale_->Text = L"modify scale";
			this->checkbox_modify_scale_->UseVisualStyleBackColor = true;
			this->checkbox_modify_scale_->CheckedChanged += gcnew System::EventHandler(this, &FormPlaceCPs::checkbox_modify_scale__CheckedChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(27, 27);
			this->label1->Margin = System::Windows::Forms::Padding(5, 0, 5, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(0, 18);
			this->label1->TabIndex = 6;
			// 
			// FormPlaceCPs
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(10, 18);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(383, 438);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->button_inport_cpinfo_);
			this->Controls->Add(this->button_export_cpinfo_);
			this->Controls->Add(this->button_finish_placement_);
			this->Controls->Add(this->groupBox1);
			this->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
			this->Name = L"FormPlaceCPs";
			this->Text = L"FormPlaceCPs";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_setisoValue_))->EndInit();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: 
		System::Void button_generateisosurfaces__Click(System::Object^ sender, System::EventArgs^ e);
		System::Void button_generate_test__Click(System::Object^ sender, System::EventArgs^ e);
		System::Void button_export_cpinfo__Click(System::Object^ sender, System::EventArgs^ e);
		System::Void button_finish_placement__Click(System::Object^ sender, System::EventArgs^ e) ;
		System::Void button_inport_cpinfo__Click(System::Object^ sender, System::EventArgs^ e);
		System::Void trackbar_setisoValue__Scroll(System::Object^ sender, System::EventArgs^ e);
		System::Void textbox_setisovalue__TextChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void button_fit_template__Click(System::Object^ sender, System::EventArgs^ e);
		System::Void groupBox1_Enter(System::Object^ sender, System::EventArgs^ e);
		System::Void button_load_mesh__Click(System::Object^ sender, System::EventArgs^ e);
		System::Void checkbox_modify_scale__CheckedChanged(System::Object^ sender, System::EventArgs^ e);
};

	inline void FormPlaceCPs_Show() { FormPlaceCPs::GetInst()->Show(); }
	inline void FormPlaceCPs_Hide() { FormPlaceCPs::GetInst()->Hide(); }
	inline void FormPlaceCPs_InitParam(){ FormPlaceCPs::GetInst()->InitParams(); }
}
