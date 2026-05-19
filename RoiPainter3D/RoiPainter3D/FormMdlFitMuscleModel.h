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
			m_trackBar_isovalue->SetRange(-1500, 1500);
			m_trackBar_isovalue->Value = 300;
			m_textBox_isovalue->Text = (300).ToString();
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
	private: System::Windows::Forms::GroupBox^ m_groopbox_isosurface;
	private: System::Windows::Forms::Button^ m_btn_gen_iso;
	private: System::Windows::Forms::TrackBar^ m_trackBar_isovalue;
	private: System::Windows::Forms::TextBox^ m_textBox_isovalue;
	private: System::Windows::Forms::Button^ m_btn_import_lmks;
	private: System::Windows::Forms::Button^ m_btn_export_lmks;
	private: System::Windows::Forms::CheckBox^ m_checkBox_half_iso;
	private: System::Windows::Forms::Button^ m_btn_finish;
	private: System::Windows::Forms::TreeView^ m_treeView_models;


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
			this->m_groopbox_isosurface = (gcnew System::Windows::Forms::GroupBox());
			this->m_checkBox_half_iso = (gcnew System::Windows::Forms::CheckBox());
			this->m_textBox_isovalue = (gcnew System::Windows::Forms::TextBox());
			this->m_trackBar_isovalue = (gcnew System::Windows::Forms::TrackBar());
			this->m_btn_gen_iso = (gcnew System::Windows::Forms::Button());
			this->m_btn_import_lmks = (gcnew System::Windows::Forms::Button());
			this->m_btn_export_lmks = (gcnew System::Windows::Forms::Button());
			this->m_btn_finish = (gcnew System::Windows::Forms::Button());
			this->m_treeView_models = (gcnew System::Windows::Forms::TreeView());
			this->m_groopbox_isosurface->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackBar_isovalue))->BeginInit();
			this->SuspendLayout();
			// 
			// m_btn_import_obj
			// 
			this->m_btn_import_obj->Location = System::Drawing::Point(256, 174);
			this->m_btn_import_obj->Name = L"m_btn_import_obj";
			this->m_btn_import_obj->Size = System::Drawing::Size(108, 32);
			this->m_btn_import_obj->TabIndex = 0;
			this->m_btn_import_obj->Text = L"Import Obj";
			this->m_btn_import_obj->UseVisualStyleBackColor = true;
			this->m_btn_import_obj->Click += gcnew System::EventHandler(this, &FormMdlFitMuscleModel::m_btn_import_obj_Click);
			// 
			// m_btn_reset
			// 
			this->m_btn_reset->Location = System::Drawing::Point(256, 212);
			this->m_btn_reset->Name = L"m_btn_reset";
			this->m_btn_reset->Size = System::Drawing::Size(108, 32);
			this->m_btn_reset->TabIndex = 1;
			this->m_btn_reset->Text = L"Models Reset";
			this->m_btn_reset->UseVisualStyleBackColor = true;
			this->m_btn_reset->Click += gcnew System::EventHandler(this, &FormMdlFitMuscleModel::m_btn_reset_Click);
			// 
			// m_groopbox_isosurface
			// 
			this->m_groopbox_isosurface->Controls->Add(this->m_checkBox_half_iso);
			this->m_groopbox_isosurface->Controls->Add(this->m_textBox_isovalue);
			this->m_groopbox_isosurface->Controls->Add(this->m_trackBar_isovalue);
			this->m_groopbox_isosurface->Controls->Add(this->m_btn_gen_iso);
			this->m_groopbox_isosurface->Location = System::Drawing::Point(34, 24);
			this->m_groopbox_isosurface->Name = L"m_groopbox_isosurface";
			this->m_groopbox_isosurface->Size = System::Drawing::Size(323, 130);
			this->m_groopbox_isosurface->TabIndex = 2;
			this->m_groopbox_isosurface->TabStop = false;
			this->m_groopbox_isosurface->Text = L"iso surface";
			// 
			// m_checkBox_half_iso
			// 
			this->m_checkBox_half_iso->AutoSize = true;
			this->m_checkBox_half_iso->Checked = true;
			this->m_checkBox_half_iso->CheckState = System::Windows::Forms::CheckState::Checked;
			this->m_checkBox_half_iso->Location = System::Drawing::Point(214, 26);
			this->m_checkBox_half_iso->Name = L"m_checkBox_half_iso";
			this->m_checkBox_half_iso->Size = System::Drawing::Size(103, 16);
			this->m_checkBox_half_iso->TabIndex = 3;
			this->m_checkBox_half_iso->Text = L"Half IsoSurface";
			this->m_checkBox_half_iso->UseVisualStyleBackColor = true;
			// 
			// m_textBox_isovalue
			// 
			this->m_textBox_isovalue->Location = System::Drawing::Point(16, 24);
			this->m_textBox_isovalue->Name = L"m_textBox_isovalue";
			this->m_textBox_isovalue->Size = System::Drawing::Size(58, 19);
			this->m_textBox_isovalue->TabIndex = 2;
			this->m_textBox_isovalue->TextChanged += gcnew System::EventHandler(this, &FormMdlFitMuscleModel::m_textBox_isovalue_TextChanged);
			// 
			// m_trackBar_isovalue
			// 
			this->m_trackBar_isovalue->Location = System::Drawing::Point(29, 49);
			this->m_trackBar_isovalue->Name = L"m_trackBar_isovalue";
			this->m_trackBar_isovalue->Size = System::Drawing::Size(272, 45);
			this->m_trackBar_isovalue->TabIndex = 1;
			this->m_trackBar_isovalue->Scroll += gcnew System::EventHandler(this, &FormMdlFitMuscleModel::m_trackBar_isovalue_Scroll);
			// 
			// m_btn_gen_iso
			// 
			this->m_btn_gen_iso->Location = System::Drawing::Point(90, 100);
			this->m_btn_gen_iso->Name = L"m_btn_gen_iso";
			this->m_btn_gen_iso->Size = System::Drawing::Size(130, 24);
			this->m_btn_gen_iso->TabIndex = 0;
			this->m_btn_gen_iso->Text = L"Generate IsoSurface";
			this->m_btn_gen_iso->UseVisualStyleBackColor = true;
			this->m_btn_gen_iso->Click += gcnew System::EventHandler(this, &FormMdlFitMuscleModel::m_btn_gen_iso_Click);
			// 
			// m_btn_import_lmks
			// 
			this->m_btn_import_lmks->Location = System::Drawing::Point(256, 250);
			this->m_btn_import_lmks->Name = L"m_btn_import_lmks";
			this->m_btn_import_lmks->Size = System::Drawing::Size(108, 25);
			this->m_btn_import_lmks->TabIndex = 3;
			this->m_btn_import_lmks->Text = L"Import LMKs";
			this->m_btn_import_lmks->UseVisualStyleBackColor = true;
			this->m_btn_import_lmks->Click += gcnew System::EventHandler(this, &FormMdlFitMuscleModel::m_btn_import_lmks_Click);
			// 
			// m_btn_export_lmks
			// 
			this->m_btn_export_lmks->Location = System::Drawing::Point(256, 281);
			this->m_btn_export_lmks->Name = L"m_btn_export_lmks";
			this->m_btn_export_lmks->Size = System::Drawing::Size(101, 24);
			this->m_btn_export_lmks->TabIndex = 4;
			this->m_btn_export_lmks->Text = L"Export LMKs";
			this->m_btn_export_lmks->UseVisualStyleBackColor = true;
			this->m_btn_export_lmks->Click += gcnew System::EventHandler(this, &FormMdlFitMuscleModel::m_btn_export_lmks_Click);
			// 
			// m_btn_finish
			// 
			this->m_btn_finish->Location = System::Drawing::Point(264, 311);
			this->m_btn_finish->Name = L"m_btn_finish";
			this->m_btn_finish->Size = System::Drawing::Size(93, 28);
			this->m_btn_finish->TabIndex = 5;
			this->m_btn_finish->Text = L"Finish";
			this->m_btn_finish->UseVisualStyleBackColor = true;
			this->m_btn_finish->Click += gcnew System::EventHandler(this, &FormMdlFitMuscleModel::m_btn_finish_Click);
			// 
			// m_treeView_models
			// 
			this->m_treeView_models->CheckBoxes = true;
			this->m_treeView_models->Location = System::Drawing::Point(34, 174);
			this->m_treeView_models->Name = L"m_treeView_models";
			this->m_treeView_models->Size = System::Drawing::Size(165, 143);
			this->m_treeView_models->TabIndex = 6;
			this->m_treeView_models->AfterCheck += gcnew System::Windows::Forms::TreeViewEventHandler(this, &FormMdlFitMuscleModel::m_treeView_models_AfterCheck);
			// 
			// FormMdlFitMuscleModel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(393, 342);
			this->Controls->Add(this->m_treeView_models);
			this->Controls->Add(this->m_btn_finish);
			this->Controls->Add(this->m_btn_export_lmks);
			this->Controls->Add(this->m_btn_import_lmks);
			this->Controls->Add(this->m_groopbox_isosurface);
			this->Controls->Add(this->m_btn_reset);
			this->Controls->Add(this->m_btn_import_obj);
			this->Name = L"FormMdlFitMuscleModel";
			this->Text = L"FormMdlFitMuscleModel";
			this->m_groopbox_isosurface->ResumeLayout(false);
			this->m_groopbox_isosurface->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackBar_isovalue))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void m_btn_import_obj_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void m_btn_gen_iso_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void m_trackBar_isovalue_Scroll(System::Object^ sender, System::EventArgs^ e);
	private: System::Void m_textBox_isovalue_TextChanged(System::Object^ sender, System::EventArgs^ e);
	private: System::Void m_btn_export_lmks_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void m_btn_import_lmks_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void m_btn_reset_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void m_btn_finish_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void m_treeView_models_AfterCheck(System::Object^ sender, System::Windows::Forms::TreeViewEventArgs^ e);

	private: void CreateTreeView(String^ dirPath, TreeNode^ parentNode);
};

	inline void formMdlFitMuscle_Show() { FormMdlFitMuscleModel::getInst()->Show(); }
	inline void formMdlFitMuscle_Hide() { FormMdlFitMuscleModel::getInst()->Hide(); }


}
