#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormMdlFitMuscleModel の概要
	/// </summary>
	public ref class FormMdlFitMuscleModel : public System::Windows::Forms::Form
	{


	private:
		static FormMdlFitMuscleModel^ m_singleton;
		FormMdlFitMuscleModel(void)
		{
			InitializeComponent();
		}


	public:
		static FormMdlFitMuscleModel^ getInst()
		{
			if (m_singleton == nullptr) m_singleton = gcnew FormMdlFitMuscleModel();
			return m_singleton;
		}


	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormMdlFitMuscleModel()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ m_btn_import_obj;
	private: System::Windows::Forms::Button^ m_btn_reset;

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
			this->m_btn_import_obj = (gcnew System::Windows::Forms::Button());
			this->m_btn_reset = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// m_btn_import_obj
			// 
			this->m_btn_import_obj->Location = System::Drawing::Point(140, 243);
			this->m_btn_import_obj->Name = L"m_btn_import_obj";
			this->m_btn_import_obj->Size = System::Drawing::Size(105, 24);
			this->m_btn_import_obj->TabIndex = 0;
			this->m_btn_import_obj->Text = L"Import Obj";
			this->m_btn_import_obj->UseVisualStyleBackColor = true;
			this->m_btn_import_obj->Click += gcnew System::EventHandler(this, &FormMdlFitMuscleModel::m_btn_import_obj_Click);
			// 
			// m_btn_reset
			// 
			this->m_btn_reset->Location = System::Drawing::Point(136, 150);
			this->m_btn_reset->Name = L"m_btn_reset";
			this->m_btn_reset->Size = System::Drawing::Size(108, 32);
			this->m_btn_reset->TabIndex = 1;
			this->m_btn_reset->Text = L"Models Reset";
			this->m_btn_reset->UseVisualStyleBackColor = true;
			this->m_btn_reset->Click += gcnew System::EventHandler(this, &FormMdlFitMuscleModel::m_btn_reset_Click);
			// 
			// FormMdlFitMuscleModel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(402, 342);
			this->Controls->Add(this->m_btn_reset);
			this->Controls->Add(this->m_btn_import_obj);
			this->Name = L"FormMdlFitMuscleModel";
			this->Text = L"FormMdlFitMuscleModel";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void m_btn_import_obj_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void m_btn_reset_Click(System::Object^ sender, System::EventArgs^ e);
	};

	inline void formMdlFitMuscle_Show() { FormMdlFitMuscleModel::getInst()->Show(); }
	inline void formMdlFitMuscle_Hide() { FormMdlFitMuscleModel::getInst()->Hide(); }


}
