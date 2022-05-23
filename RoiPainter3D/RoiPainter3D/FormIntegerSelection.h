#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormIntegerSelection の概要
	/// </summary>
	public ref class FormIntegerSelection : public System::Windows::Forms::Form
	{

    int m_value;

	public:
		FormIntegerSelection(int init_value, int min_value, int max_value, const char* message)
		{
			InitializeComponent();
      
      m_numericUpDown->Minimum = min_value;
      m_numericUpDown->Maximum = max_value;
      m_numericUpDown->Value   = init_value;

      m_label->Text = gcnew System::String(message);
		}
    
    int GetValue(){ 
      return m_value; 
    }

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormIntegerSelection()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::NumericUpDown^  m_numericUpDown;
  private: System::Windows::Forms::Label^  m_label;
  private: System::Windows::Forms::Button^  btnCancel;
  private: System::Windows::Forms::Button^  btnOk;
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
      this->m_numericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
      this->m_label = (gcnew System::Windows::Forms::Label());
      this->btnCancel = (gcnew System::Windows::Forms::Button());
      this->btnOk = (gcnew System::Windows::Forms::Button());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numericUpDown))->BeginInit();
      this->SuspendLayout();
      // 
      // m_numericUpDown
      // 
      this->m_numericUpDown->Font = (gcnew System::Drawing::Font(L"游ゴシック", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_numericUpDown->Location = System::Drawing::Point(11, 38);
      this->m_numericUpDown->Name = L"m_numericUpDown";
      this->m_numericUpDown->Size = System::Drawing::Size(120, 35);
      this->m_numericUpDown->TabIndex = 1;
      this->m_numericUpDown->ValueChanged += gcnew System::EventHandler(this, &FormIntegerSelection::m_numericUpDown_ValueChanged);
      // 
      // m_label
      // 
      this->m_label->AutoSize = true;
      this->m_label->Font = (gcnew System::Drawing::Font(L"メイリオ", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_label->Location = System::Drawing::Point(12, 9);
      this->m_label->Name = L"m_label";
      this->m_label->Size = System::Drawing::Size(261, 25);
      this->m_label->TabIndex = 2;
      this->m_label->Text = L"message message message";
      // 
      // btnCancel
      // 
      this->btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
      this->btnCancel->Location = System::Drawing::Point(138, 99);
      this->btnCancel->Margin = System::Windows::Forms::Padding(4);
      this->btnCancel->Name = L"btnCancel";
      this->btnCancel->Size = System::Drawing::Size(119, 40);
      this->btnCancel->TabIndex = 7;
      this->btnCancel->Text = L"CANCEL";
      this->btnCancel->UseVisualStyleBackColor = true;
      this->btnCancel->Click += gcnew System::EventHandler(this, &FormIntegerSelection::btnCancel_Click);
      // 
      // btnOk
      // 
      this->btnOk->DialogResult = System::Windows::Forms::DialogResult::OK;
      this->btnOk->Location = System::Drawing::Point(11, 99);
      this->btnOk->Margin = System::Windows::Forms::Padding(4);
      this->btnOk->Name = L"btnOk";
      this->btnOk->Size = System::Drawing::Size(119, 40);
      this->btnOk->TabIndex = 6;
      this->btnOk->Text = L"OK";
      this->btnOk->UseVisualStyleBackColor = true;
      this->btnOk->Click += gcnew System::EventHandler(this, &FormIntegerSelection::btnOk_Click);
      // 
      // FormIntegerSelection
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(289, 145);
      this->Controls->Add(this->btnCancel);
      this->Controls->Add(this->btnOk);
      this->Controls->Add(this->m_label);
      this->Controls->Add(this->m_numericUpDown);
      this->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
      this->Name = L"FormIntegerSelection";
      this->Text = L"FormIntegerSelection";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numericUpDown))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private: System::Void m_numericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
  {
    m_value = System::Decimal::ToInt32( m_numericUpDown->Value);
  }
  private: System::Void btnOk_Click(System::Object^  sender, System::EventArgs^  e) {}
  private: System::Void btnCancel_Click(System::Object^  sender, System::EventArgs^  e) {}

};

inline bool FormIntegerSelection_doModal(
    const int init_value,
    const int min_value,
    const int max_value,
    const char* message,
    int &value)
{
  FormIntegerSelection ^dlg 
    = gcnew FormIntegerSelection(init_value, min_value ,max_value , message);
  
  value = 0;
  if( dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return false;

  value = dlg->GetValue();
  return true;
}
}
