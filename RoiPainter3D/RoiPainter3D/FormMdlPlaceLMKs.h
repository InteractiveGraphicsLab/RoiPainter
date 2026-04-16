#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormMdlPlaceLMKs の概要
	/// </summary>
	public ref class FormMdlPlaceLMKs : public System::Windows::Forms::Form
	{
	private: 
		System::Windows::Forms::Button^ m_btn_genIsosurface;
		System::Windows::Forms::TrackBar^ m_trackbar_setIsoValue;
	private: System::Windows::Forms::TextBox^ m_textbox_setIsoValue;
	private: System::Windows::Forms::GroupBox^ m_groupbox_isosurface;
	private: System::Windows::Forms::Button^ m_btn_import;
	private: System::Windows::Forms::Button^ m_btn_export;
	private: System::Windows::Forms::Button^ m_btn_finish;



	private:
		static FormMdlPlaceLMKs^ m_singleton;

		FormMdlPlaceLMKs(void)
		{
			InitializeComponent();
			m_trackbar_setIsoValue->SetRange(-1500, 4400);
			m_trackbar_setIsoValue->Value = 100;
			m_textbox_setIsoValue->Text = (100).ToString();
		}

	public:
		static FormMdlPlaceLMKs^ getInst()
		{
			if (m_singleton == nullptr) m_singleton = gcnew FormMdlPlaceLMKs();
			return m_singleton;
		}

		//void InitParams();


	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormMdlPlaceLMKs()
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
			this->m_btn_genIsosurface = (gcnew System::Windows::Forms::Button());
			this->m_trackbar_setIsoValue = (gcnew System::Windows::Forms::TrackBar());
			this->m_textbox_setIsoValue = (gcnew System::Windows::Forms::TextBox());
			this->m_groupbox_isosurface = (gcnew System::Windows::Forms::GroupBox());
			this->m_btn_import = (gcnew System::Windows::Forms::Button());
			this->m_btn_export = (gcnew System::Windows::Forms::Button());
			this->m_btn_finish = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_setIsoValue))->BeginInit();
			this->m_groupbox_isosurface->SuspendLayout();
			this->SuspendLayout();
			// 
			// m_btn_genIsosurface
			// 
			this->m_btn_genIsosurface->Location = System::Drawing::Point(25, 93);
			this->m_btn_genIsosurface->Name = L"m_btn_genIsosurface";
			this->m_btn_genIsosurface->Size = System::Drawing::Size(278, 23);
			this->m_btn_genIsosurface->TabIndex = 10;
			this->m_btn_genIsosurface->Text = L"generate isosurface";
			this->m_btn_genIsosurface->UseVisualStyleBackColor = true;
			this->m_btn_genIsosurface->Click += gcnew System::EventHandler(this, &FormMdlPlaceLMKs::m_btn_genIsosurface_Click);
			// 
			// m_trackbar_setIsoValue
			// 
			this->m_trackbar_setIsoValue->Location = System::Drawing::Point(16, 42);
			this->m_trackbar_setIsoValue->Name = L"m_trackbar_setIsoValue";
			this->m_trackbar_setIsoValue->Size = System::Drawing::Size(278, 45);
			this->m_trackbar_setIsoValue->TabIndex = 8;
			this->m_trackbar_setIsoValue->TabStop = false;
			this->m_trackbar_setIsoValue->TickStyle = System::Windows::Forms::TickStyle::None;
			this->m_trackbar_setIsoValue->Scroll += gcnew System::EventHandler(this, &FormMdlPlaceLMKs::m_trackbar_setIsoValue__Scroll);
			// 
			// m_textbox_setIsoValue
			// 
			this->m_textbox_setIsoValue->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->m_textbox_setIsoValue->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
			this->m_textbox_setIsoValue->Location = System::Drawing::Point(11, 19);
			this->m_textbox_setIsoValue->Name = L"m_textbox_setIsoValue";
			this->m_textbox_setIsoValue->Size = System::Drawing::Size(61, 23);
			this->m_textbox_setIsoValue->TabIndex = 7;
			this->m_textbox_setIsoValue->TextChanged += gcnew System::EventHandler(this, &FormMdlPlaceLMKs::m_textbox_setIsoValue_TextChanged);
			// 
			// m_groupbox_isosurface
			// 
			this->m_groupbox_isosurface->Controls->Add(this->m_textbox_setIsoValue);
			this->m_groupbox_isosurface->Controls->Add(this->m_trackbar_setIsoValue);
			this->m_groupbox_isosurface->Controls->Add(this->m_btn_genIsosurface);
			this->m_groupbox_isosurface->Location = System::Drawing::Point(12, 12);
			this->m_groupbox_isosurface->Name = L"m_groupbox_isosurface";
			this->m_groupbox_isosurface->Size = System::Drawing::Size(335, 147);
			this->m_groupbox_isosurface->TabIndex = 11;
			this->m_groupbox_isosurface->TabStop = false;
			this->m_groupbox_isosurface->Text = L"IsoSurface";
			// 
			// m_btn_import
			// 
			this->m_btn_import->Location = System::Drawing::Point(37, 205);
			this->m_btn_import->Name = L"m_btn_import";
			this->m_btn_import->Size = System::Drawing::Size(122, 27);
			this->m_btn_import->TabIndex = 12;
			this->m_btn_import->Text = L"Import Landmark";
			this->m_btn_import->UseVisualStyleBackColor = true;
			this->m_btn_import->Click += gcnew System::EventHandler(this, &FormMdlPlaceLMKs::m_btn_import_Click);
			// 
			// m_btn_export
			// 
			this->m_btn_export->Location = System::Drawing::Point(192, 205);
			this->m_btn_export->Name = L"m_btn_export";
			this->m_btn_export->Size = System::Drawing::Size(123, 27);
			this->m_btn_export->TabIndex = 13;
			this->m_btn_export->Text = L"Export Landmark";
			this->m_btn_export->UseVisualStyleBackColor = true;
			this->m_btn_export->Click += gcnew System::EventHandler(this, &FormMdlPlaceLMKs::m_btn_export_Click);
			// 
			// m_btn_finish
			// 
			this->m_btn_finish->Location = System::Drawing::Point(111, 291);
			this->m_btn_finish->Name = L"m_btn_finish";
			this->m_btn_finish->Size = System::Drawing::Size(144, 30);
			this->m_btn_finish->TabIndex = 14;
			this->m_btn_finish->Text = L"Finish PlaceLMKs";
			this->m_btn_finish->UseVisualStyleBackColor = true;
			this->m_btn_finish->Click += gcnew System::EventHandler(this, &FormMdlPlaceLMKs::m_btn_finish_Click);
			// 
			// FormMdlPlaceLMKs
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(371, 385);
			this->Controls->Add(this->m_btn_finish);
			this->Controls->Add(this->m_btn_export);
			this->Controls->Add(this->m_btn_import);
			this->Controls->Add(this->m_groupbox_isosurface);
			this->Name = L"FormMdlPlaceLMKs";
			this->Text = L"FormMdlPlaceLMKs";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_setIsoValue))->EndInit();
			this->m_groupbox_isosurface->ResumeLayout(false);
			this->m_groupbox_isosurface->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void m_btn_genIsosurface_Click(System::Object^ sender, System::EventArgs^ e);

	private: System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
	}
private: System::Void m_trackbar_setIsoValue__Scroll(System::Object^ sender, System::EventArgs^ e);
private: System::Void m_textbox_setIsoValue_TextChanged(System::Object^ sender, System::EventArgs^ e);
private: System::Void m_btn_import_Click(System::Object^ sender, System::EventArgs^ e);
private: System::Void m_btn_export_Click(System::Object^ sender, System::EventArgs^ e);
private: System::Void m_btn_finish_Click(System::Object^ sender, System::EventArgs^ e);
};
	inline void formMdlPlaceLMKs_Hide() { FormMdlPlaceLMKs::getInst()->Hide(); }
	inline void formMdlPlaceLMKs_Show() { FormMdlPlaceLMKs::getInst()->Show(); }

}
