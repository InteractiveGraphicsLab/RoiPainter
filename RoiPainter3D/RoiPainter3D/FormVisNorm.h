#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormVisNorm の概要
	/// </summary>
	public ref class FormVisNorm : public System::Windows::Forms::Form
	{

   
    FormVisNorm(void)
		{
			InitializeComponent();
		}
  private: System::Windows::Forms::Label^  label1;
  private: System::Windows::Forms::Label^  label2;
  private: System::Windows::Forms::Label^  label3;
  private: System::Windows::Forms::Label^  label4;
  private: System::Windows::Forms::Label^  label5;

    static FormVisNorm^ m_singleton;
   
  public:
    static FormVisNorm^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormVisNorm();
      return m_singleton;
    }

    void setVolumeValVis(short v)
    {
      textBox->Text = v.ToString();
      textBox->Refresh();
    }


	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormVisNorm()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::Label^    label;
  private: System::Windows::Forms::TextBox^  textBox;
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
      this->label = (gcnew System::Windows::Forms::Label());
      this->textBox = (gcnew System::Windows::Forms::TextBox());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->label4 = (gcnew System::Windows::Forms::Label());
      this->label5 = (gcnew System::Windows::Forms::Label());
      this->SuspendLayout();
      // 
      // label
      // 
      this->label->AutoSize = true;
      this->label->Location = System::Drawing::Point(7, 134);
      this->label->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label->Name = L"label";
      this->label->Size = System::Drawing::Size(72, 12);
      this->label->TabIndex = 0;
      this->label->Text = L"volume value";
      // 
      // textBox
      // 
      this->textBox->Location = System::Drawing::Point(80, 134);
      this->textBox->Margin = System::Windows::Forms::Padding(2);
      this->textBox->Name = L"textBox";
      this->textBox->ReadOnly = true;
      this->textBox->Size = System::Drawing::Size(76, 19);
      this->textBox->TabIndex = 1;
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->label1->Location = System::Drawing::Point(1, 9);
      this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(183, 20);
      this->label1->TabIndex = 2;
      this->label1->Text = L"L-drag : translate camera";
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->label2->Location = System::Drawing::Point(1, 29);
      this->label2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(166, 20);
      this->label2->TabIndex = 3;
      this->label2->Text = L"M-drag : Zoom Camera";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->label3->Location = System::Drawing::Point(1, 49);
      this->label3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(171, 20);
      this->label3->TabIndex = 4;
      this->label3->Text = L"R-drag : Rotate Camera";
      // 
      // label4
      // 
      this->label4->AutoSize = true;
      this->label4->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->label4->Location = System::Drawing::Point(1, 71);
      this->label4->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label4->Name = L"label4";
      this->label4->Size = System::Drawing::Size(178, 20);
      this->label4->TabIndex = 5;
      this->label4->Text = L"ctrl + L-drag : cut stroke";
      // 
      // label5
      // 
      this->label5->AutoSize = true;
      this->label5->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->label5->Location = System::Drawing::Point(1, 92);
      this->label5->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label5->Name = L"label5";
      this->label5->Size = System::Drawing::Size(194, 20);
      this->label5->TabIndex = 6;
      this->label5->Text = L"Wheel : move cross section";
      // 
      // FormVisNorm
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(193, 176);
      this->Controls->Add(this->label5);
      this->Controls->Add(this->label4);
      this->Controls->Add(this->label3);
      this->Controls->Add(this->label2);
      this->Controls->Add(this->label1);
      this->Controls->Add(this->textBox);
      this->Controls->Add(this->label);
      this->Margin = System::Windows::Forms::Padding(2);
      this->Name = L"FormVisNorm";
      this->Text = L"FormVisNorm";
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
	};

  inline void formVisNorm_Show(){
    FormVisNorm::getInst()->Show();
  }
  inline void formVisNorm_Hide(){
    FormVisNorm::getInst()->Hide();    
  }
  inline void formVisNorm_setVoxelVis(short v){
    FormVisNorm::getInst()->setVolumeValVis(v);
  }


}
