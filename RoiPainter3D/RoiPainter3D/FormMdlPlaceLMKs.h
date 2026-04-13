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
		static FormMdlPlaceLMKs^ m_singleton;

		FormMdlPlaceLMKs(void)
		{
			InitializeComponent();
			//
			//TODO: ここにコンストラクター コードを追加します
			//
		}

	public:
		static FormMdlPlaceLMKs^ getInst()
		{
			if (m_singleton == nullptr) m_singleton = gcnew FormMdlPlaceLMKs();
			return m_singleton;
		}


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


	private: System::Windows::Forms::Button^ m_btn_aaa;
	private: System::Windows::Forms::TrackBar^ trackBar1;
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
			this->m_btn_aaa = (gcnew System::Windows::Forms::Button());
			this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->BeginInit();
			this->SuspendLayout();
			// 
			// m_btn_aaa
			// 
			this->m_btn_aaa->Location = System::Drawing::Point(12, 350);
			this->m_btn_aaa->Name = L"m_btn_aaa";
			this->m_btn_aaa->Size = System::Drawing::Size(75, 23);
			this->m_btn_aaa->TabIndex = 0;
			this->m_btn_aaa->Text = L"button1";
			this->m_btn_aaa->UseVisualStyleBackColor = true;
			this->m_btn_aaa->Click += gcnew System::EventHandler(this, &FormMdlPlaceLMKs::m_btn_aaa_Click);
			// 
			// trackBar1
			// 
			this->trackBar1->Location = System::Drawing::Point(62, 131);
			this->trackBar1->Name = L"trackBar1";
			this->trackBar1->Size = System::Drawing::Size(146, 45);
			this->trackBar1->TabIndex = 1;
			// 
			// FormMdlPlaceLMKs
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(371, 385);
			this->Controls->Add(this->trackBar1);
			this->Controls->Add(this->m_btn_aaa);
			this->Name = L"FormMdlPlaceLMKs";
			this->Text = L"FormMdlPlaceLMKs";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void m_btn_aaa_Click(System::Object^ sender, System::EventArgs^ e) {}

	};
	inline void formMdlPlaceLMKs_Hide() { FormMdlPlaceLMKs::getInst()->Hide(); }
	inline void formMdlPlaceLMKs_Show() { FormMdlPlaceLMKs::getInst()->Show(); }

}
