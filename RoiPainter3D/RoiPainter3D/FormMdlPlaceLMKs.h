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

	private:
		static FormMdlPlaceLMKs^ m_singleton;

		FormMdlPlaceLMKs(void)
		{
			InitializeComponent();
			m_trackbar_setIsoValue->SetRange(-100, 1000);
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
	

	protected:




	private: System::Windows::Forms::Label^ m_thresh;


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
			this->m_btn_genIsosurface = (gcnew System::Windows::Forms::Button());
			this->m_trackbar_setIsoValue = (gcnew System::Windows::Forms::TrackBar());
			this->m_thresh = (gcnew System::Windows::Forms::Label());
			this->m_textbox_setIsoValue = (gcnew System::Windows::Forms::TextBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_setIsoValue))->BeginInit();
			this->SuspendLayout();
			// 
			// m_btn_genIsosurface
			// 
			this->m_btn_genIsosurface->Location = System::Drawing::Point(43, 106);
			this->m_btn_genIsosurface->Name = L"m_btn_genIsosurface";
			this->m_btn_genIsosurface->Size = System::Drawing::Size(278, 23);
			this->m_btn_genIsosurface->TabIndex = 10;
			this->m_btn_genIsosurface->Text = L"generate isosurface";
			this->m_btn_genIsosurface->UseVisualStyleBackColor = true;
			this->m_btn_genIsosurface->Click += gcnew System::EventHandler(this, &FormMdlPlaceLMKs::m_btn_genIsosurface_Click);
			// 
			// m_trackbar_setIsoValue
			// 
			this->m_trackbar_setIsoValue->Location = System::Drawing::Point(34, 55);
			this->m_trackbar_setIsoValue->Name = L"m_trackbar_setIsoValue";
			this->m_trackbar_setIsoValue->Size = System::Drawing::Size(278, 45);
			this->m_trackbar_setIsoValue->TabIndex = 8;
			this->m_trackbar_setIsoValue->TabStop = false;
			this->m_trackbar_setIsoValue->TickStyle = System::Windows::Forms::TickStyle::None;
			this->m_trackbar_setIsoValue->Scroll += gcnew System::EventHandler(this, &FormMdlPlaceLMKs::m_trackbar_setIsoValue__Scroll);
			// 
			// m_thresh
			// 
			this->m_thresh->AutoSize = true;
			this->m_thresh->Location = System::Drawing::Point(32, 9);
			this->m_thresh->Name = L"m_thresh";
			this->m_thresh->Size = System::Drawing::Size(72, 12);
			this->m_thresh->TabIndex = 2;
			this->m_thresh->Text = L"Thresh(閾値)";
			this->m_thresh->Click += gcnew System::EventHandler(this, &FormMdlPlaceLMKs::label1_Click);
			// 
			// m_textbox_setIsoValue
			// 
			this->m_textbox_setIsoValue->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->m_textbox_setIsoValue->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
			this->m_textbox_setIsoValue->Location = System::Drawing::Point(29, 32);
			this->m_textbox_setIsoValue->Name = L"m_textbox_setIsoValue";
			this->m_textbox_setIsoValue->Size = System::Drawing::Size(61, 19);
			this->m_textbox_setIsoValue->TabIndex = 7;
			this->m_textbox_setIsoValue->TextChanged += gcnew System::EventHandler(this, &FormMdlPlaceLMKs::m_textbox_setIsoValue_TextChanged);
			// 
			// FormMdlPlaceLMKs
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(371, 385);
			this->Controls->Add(this->m_textbox_setIsoValue);
			this->Controls->Add(this->m_thresh);
			this->Controls->Add(this->m_trackbar_setIsoValue);
			this->Controls->Add(this->m_btn_genIsosurface);
			this->Name = L"FormMdlPlaceLMKs";
			this->Text = L"FormMdlPlaceLMKs";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_setIsoValue))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void m_btn_genIsosurface_Click(System::Object^ sender, System::EventArgs^ e);

	private: System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
	}
private: System::Void m_trackbar_setIsoValue__Scroll(System::Object^ sender, System::EventArgs^ e);
private: System::Void m_textbox_setIsoValue_TextChanged(System::Object^ sender, System::EventArgs^ e) {
}
};
	inline void formMdlPlaceLMKs_Hide() { FormMdlPlaceLMKs::getInst()->Hide(); }
	inline void formMdlPlaceLMKs_Show() { FormMdlPlaceLMKs::getInst()->Show(); }

}
