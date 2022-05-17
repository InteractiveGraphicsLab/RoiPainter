#pragma once

namespace RoiPainter4D {

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
  private:
    FormVisNorm(void)
    {
      InitializeComponent();
      this->ControlBox = false;
      this->MaximizeBox = false;
    }

  private: 
    System::Windows::Forms::Label^  label1;
    System::Windows::Forms::Label^  label2;
    System::Windows::Forms::Label^  label3;
    System::Windows::Forms::Label^  label4;
    System::Windows::Forms::Label^  label5;
    System::Windows::Forms::Label^  label6;
    System::Windows::Forms::TextBox^  textBox_voxelvalue;

    static FormVisNorm^ m_singleton;
  public:
    static FormVisNorm^ getInst() {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormVisNorm();
      }
      return m_singleton;
    }

    void setVolumeValVis(short v)
    {
      textBox_voxelvalue->Text = v.ToString();
      textBox_voxelvalue->Refresh();
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
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->textBox_voxelvalue = (gcnew System::Windows::Forms::TextBox());
      this->label5 = (gcnew System::Windows::Forms::Label());
      this->label4 = (gcnew System::Windows::Forms::Label());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->label6 = (gcnew System::Windows::Forms::Label());
      this->SuspendLayout();
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label1->Location = System::Drawing::Point(-1, 152);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(100, 20);
      this->label1->TabIndex = 1;
      this->label1->Text = L"Volume Value";
      // 
      // textBox_voxelvalue
      // 
      this->textBox_voxelvalue->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->textBox_voxelvalue->Location = System::Drawing::Point(105, 149);
      this->textBox_voxelvalue->Name = L"textBox_voxelvalue";
      this->textBox_voxelvalue->ReadOnly = true;
      this->textBox_voxelvalue->Size = System::Drawing::Size(100, 27);
      this->textBox_voxelvalue->TabIndex = 2;
      // 
      // label5
      // 
      this->label5->AutoSize = true;
      this->label5->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->label5->Location = System::Drawing::Point(11, 92);
      this->label5->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label5->Name = L"label5";
      this->label5->Size = System::Drawing::Size(194, 20);
      this->label5->TabIndex = 11;
      this->label5->Text = L"Wheel : move cross section";
      // 
      // label4
      // 
      this->label4->AutoSize = true;
      this->label4->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->label4->Location = System::Drawing::Point(11, 71);
      this->label4->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label4->Name = L"label4";
      this->label4->Size = System::Drawing::Size(178, 20);
      this->label4->TabIndex = 10;
      this->label4->Text = L"ctrl + L-drag : cut stroke";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->label3->Location = System::Drawing::Point(11, 49);
      this->label3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(171, 20);
      this->label3->TabIndex = 9;
      this->label3->Text = L"R-drag : Rotate Camera";
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->label2->Location = System::Drawing::Point(11, 29);
      this->label2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(166, 20);
      this->label2->TabIndex = 8;
      this->label2->Text = L"M-drag : Zoom Camera";
      // 
      // label6
      // 
      this->label6->AutoSize = true;
      this->label6->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->label6->Location = System::Drawing::Point(11, 9);
      this->label6->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label6->Name = L"label6";
      this->label6->Size = System::Drawing::Size(183, 20);
      this->label6->TabIndex = 7;
      this->label6->Text = L"L-drag : translate camera";
      // 
      // FormVisNorm
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(221, 195);
      this->Controls->Add(this->label5);
      this->Controls->Add(this->label4);
      this->Controls->Add(this->label3);
      this->Controls->Add(this->label2);
      this->Controls->Add(this->label6);
      this->Controls->Add(this->textBox_voxelvalue);
      this->Controls->Add(this->label1);
      this->Name = L"FormVisNorm";
      this->Text = L"FormVisNorm";
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  };

  inline void formVisNorm_Show() { FormVisNorm::getInst()->Show(); }
  inline void formVisNorm_Hide() { FormVisNorm::getInst()->Hide(); }
  inline void formVisNorm_setVoxelVis(short v){
    FormVisNorm::getInst()->setVolumeValVis(v);
  }

}
