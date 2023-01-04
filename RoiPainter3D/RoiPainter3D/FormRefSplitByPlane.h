#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormRefSplitByPlane の概要
	/// </summary>
	public ref class FormRefSplitByPlane : public System::Windows::Forms::Form
	{

	private:
		static FormRefSplitByPlane^ m_singleton;

		FormRefSplitByPlane(void)
		{
			InitializeComponent();
		}

	public:
		static FormRefSplitByPlane^ getInst()
		{
			if (m_singleton == nullptr) m_singleton = gcnew FormRefSplitByPlane();
			return m_singleton;
		}
	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormRefSplitByPlane()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^ btn_finish;
	private: System::Windows::Forms::Button^ btn_cancel;
	private: System::Windows::Forms::Button^ btn_gen_plane;
	private: System::Windows::Forms::Button^ btn_split_by_plane;
	private: System::Windows::Forms::Button^ btn_perform_analysis;
	private: System::Windows::Forms::Button^ btn_flip_plane;


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
			this->btn_finish = (gcnew System::Windows::Forms::Button());
			this->btn_cancel = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->btn_gen_plane = (gcnew System::Windows::Forms::Button());
			this->btn_split_by_plane = (gcnew System::Windows::Forms::Button());
			this->btn_perform_analysis = (gcnew System::Windows::Forms::Button());
			this->btn_flip_plane = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// btn_finish
			// 
			this->btn_finish->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->btn_finish->Location = System::Drawing::Point(96, 167);
			this->btn_finish->Margin = System::Windows::Forms::Padding(2);
			this->btn_finish->Name = L"btn_finish";
			this->btn_finish->Size = System::Drawing::Size(149, 30);
			this->btn_finish->TabIndex = 3;
			this->btn_finish->Text = L"FINISH refinement";
			this->btn_finish->UseVisualStyleBackColor = true;
			this->btn_finish->Click += gcnew System::EventHandler(this, &FormRefSplitByPlane::btn_finish_Click);
			// 
			// btn_cancel
			// 
			this->btn_cancel->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->btn_cancel->Location = System::Drawing::Point(11, 167);
			this->btn_cancel->Margin = System::Windows::Forms::Padding(2);
			this->btn_cancel->Name = L"btn_cancel";
			this->btn_cancel->Size = System::Drawing::Size(74, 30);
			this->btn_cancel->TabIndex = 2;
			this->btn_cancel->Text = L"CANCEL";
			this->btn_cancel->UseVisualStyleBackColor = true;
			this->btn_cancel->Click += gcnew System::EventHandler(this, &FormRefSplitByPlane::btn_cancel_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->label1->Location = System::Drawing::Point(16, 18);
			this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(246, 18);
			this->label1->TabIndex = 4;
			this->label1->Text = L"Shift L-dbl click  : add / delete points";
			// 
			// btn_gen_plane
			// 
			this->btn_gen_plane->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->btn_gen_plane->Location = System::Drawing::Point(11, 47);
			this->btn_gen_plane->Margin = System::Windows::Forms::Padding(2);
			this->btn_gen_plane->Name = L"btn_gen_plane";
			this->btn_gen_plane->Size = System::Drawing::Size(160, 30);
			this->btn_gen_plane->TabIndex = 5;
			this->btn_gen_plane->Text = L"Generate Plane";
			this->btn_gen_plane->UseVisualStyleBackColor = true;
			this->btn_gen_plane->Click += gcnew System::EventHandler(this, &FormRefSplitByPlane::btn_gen_plane_Click);
			// 
			// btn_split_by_plane
			// 
			this->btn_split_by_plane->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->btn_split_by_plane->Location = System::Drawing::Point(11, 79);
			this->btn_split_by_plane->Margin = System::Windows::Forms::Padding(2);
			this->btn_split_by_plane->Name = L"btn_split_by_plane";
			this->btn_split_by_plane->Size = System::Drawing::Size(160, 30);
			this->btn_split_by_plane->TabIndex = 6;
			this->btn_split_by_plane->Text = L"Split volume by plane";
			this->btn_split_by_plane->UseVisualStyleBackColor = true;
			this->btn_split_by_plane->Click += gcnew System::EventHandler(this, &FormRefSplitByPlane::btn_split_by_plane_Click);
			// 
			// btn_perform_analysis
			// 
			this->btn_perform_analysis->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->btn_perform_analysis->Location = System::Drawing::Point(11, 113);
			this->btn_perform_analysis->Margin = System::Windows::Forms::Padding(2);
			this->btn_perform_analysis->Name = L"btn_perform_analysis";
			this->btn_perform_analysis->Size = System::Drawing::Size(160, 30);
			this->btn_perform_analysis->TabIndex = 7;
			this->btn_perform_analysis->Text = L"Perform Analysis";
			this->btn_perform_analysis->UseVisualStyleBackColor = true;
			this->btn_perform_analysis->Click += gcnew System::EventHandler(this, &FormRefSplitByPlane::btn_perform_analysis_Click);
			// 
			// btn_flip_plane
			// 
			this->btn_flip_plane->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->btn_flip_plane->Location = System::Drawing::Point(175, 47);
			this->btn_flip_plane->Margin = System::Windows::Forms::Padding(2);
			this->btn_flip_plane->Name = L"btn_flip_plane";
			this->btn_flip_plane->Size = System::Drawing::Size(87, 30);
			this->btn_flip_plane->TabIndex = 8;
			this->btn_flip_plane->Text = L"Flip Plane";
			this->btn_flip_plane->UseVisualStyleBackColor = true;
			this->btn_flip_plane->Click += gcnew System::EventHandler(this, &FormRefSplitByPlane::btn_flip_plane_Click);
			// 
			// FormRefSplitByPlane
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(271, 220);
			this->Controls->Add(this->btn_flip_plane);
			this->Controls->Add(this->btn_perform_analysis);
			this->Controls->Add(this->btn_split_by_plane);
			this->Controls->Add(this->btn_gen_plane);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->btn_finish);
			this->Controls->Add(this->btn_cancel);
			this->Name = L"FormRefSplitByPlane";
			this->Text = L"FormRefSplitByPlane";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void btn_cancel_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void btn_finish_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void btn_gen_plane_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void btn_split_by_plane_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void btn_perform_analysis_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void btn_flip_plane_Click(System::Object^ sender, System::EventArgs^ e); 

};

	inline void formRefSplitByPlane_Hide() { FormRefSplitByPlane::getInst()->Hide(); }
	inline void formRefSplitByPlane_Show() { FormRefSplitByPlane::getInst()->Show(); }

}
