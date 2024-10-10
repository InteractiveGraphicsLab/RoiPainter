#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormRefStrokeTrim の概要
	/// </summary>
	public ref class FormRefStrokeTrim : public System::Windows::Forms::Form
	{

  private:
    static FormRefStrokeTrim^ m_singleton;
    FormRefStrokeTrim(void)
    {
      InitializeComponent();
    }

  public:
    static FormRefStrokeTrim^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormRefStrokeTrim();
      return m_singleton;
    }

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormRefStrokeTrim()
		{
			if (components)
			{
				delete components;
			}
		}
  private: 
    System::Windows::Forms::Button^  btn_cancel;
    System::ComponentModel::Container^ components;
    System::Windows::Forms::Button^  btn_finish;
    System::Windows::Forms::Label^  label1;
    System::Windows::Forms::Label^  label2;
    System::Windows::Forms::Label^  label3;

		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>

#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
      this->btn_cancel = (gcnew System::Windows::Forms::Button());
      this->btn_finish = (gcnew System::Windows::Forms::Button());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->SuspendLayout();
      // 
      // btn_cancel
      // 
      this->btn_cancel->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_cancel->Location = System::Drawing::Point(12, 120);
      this->btn_cancel->Name = L"btn_cancel";
      this->btn_cancel->Size = System::Drawing::Size(98, 38);
      this->btn_cancel->TabIndex = 0;
      this->btn_cancel->Text = L"CANCEL";
      this->btn_cancel->UseVisualStyleBackColor = true;
      this->btn_cancel->Click += gcnew System::EventHandler(this, &FormRefStrokeTrim::btn_cancel_Click);
      // 
      // btn_finish
      // 
      this->btn_finish->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_finish->Location = System::Drawing::Point(116, 120);
      this->btn_finish->Name = L"btn_finish";
      this->btn_finish->Size = System::Drawing::Size(199, 38);
      this->btn_finish->TabIndex = 1;
      this->btn_finish->Text = L"FINISH refinement";
      this->btn_finish->UseVisualStyleBackColor = true;
      this->btn_finish->Click += gcnew System::EventHandler(this, &FormRefStrokeTrim::btn_finish_Click);
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label1->Location = System::Drawing::Point(12, 19);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(250, 23);
      this->label1->TabIndex = 2;
      this->label1->Text = L"Shift L-drag : draw trim stroke";
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label2->Location = System::Drawing::Point(12, 45);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(297, 23);
      this->label2->TabIndex = 3;
      this->label2->Text = L"R- click during stroke : cencel stroke";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label3->Location = System::Drawing::Point(12, 73);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(217, 23);
      this->label3->TabIndex = 4;
      this->label3->Text = L"z-key : undo (only 1 time)";
      // 
      // FormRefStrokeTrim
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(323, 188);
      this->Controls->Add(this->label3);
      this->Controls->Add(this->label2);
      this->Controls->Add(this->label1);
      this->Controls->Add(this->btn_finish);
      this->Controls->Add(this->btn_cancel);
      this->Name = L"FormRefStrokeTrim";
      this->Text = L"FormRefStrokeTrim";
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private: System::Void btn_cancel_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btn_finish_Click(System::Object^  sender, System::EventArgs^  e) ;
};

  inline void formRefStrokeTrim_Hide(){ FormRefStrokeTrim::getInst()->Hide();}
  inline void formRefStrokeTrim_Show(){ FormRefStrokeTrim::getInst()->Show();}
}
