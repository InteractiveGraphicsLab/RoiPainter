#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegVoxelPaint の概要
	/// </summary>
	public ref class FormSegVoxelPaint : public System::Windows::Forms::Form
	{

    
    static FormSegVoxelPaint^ m_singleton;
   
  public:
    static FormSegVoxelPaint^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormSegVoxelPaint();
      return m_singleton;
    }

    bool bLassoMode(){ return rbtn_lasso->Checked;}




	private:
		FormSegVoxelPaint(void)
		{
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormSegVoxelPaint()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::RadioButton^  rbtn_paint;
  private: System::Windows::Forms::RadioButton^  rbtn_lasso;
  private: System::Windows::Forms::Label^  label1;
  private: System::Windows::Forms::Label^  label2;
  private: System::Windows::Forms::Label^  label3;
  private: System::Windows::Forms::Label^  label4;
  private: System::Windows::Forms::Button^  btn_Cancel;
  private: System::Windows::Forms::Button^  btn_finish;


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
      this->rbtn_paint = (gcnew System::Windows::Forms::RadioButton());
      this->rbtn_lasso = (gcnew System::Windows::Forms::RadioButton());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->label4 = (gcnew System::Windows::Forms::Label());
      this->btn_Cancel = (gcnew System::Windows::Forms::Button());
      this->btn_finish = (gcnew System::Windows::Forms::Button());
      this->SuspendLayout();
      // 
      // rbtn_paint
      // 
      this->rbtn_paint->AutoSize = true;
      this->rbtn_paint->Checked = true;
      this->rbtn_paint->Font = (gcnew System::Drawing::Font(L"メイリオ", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->rbtn_paint->Location = System::Drawing::Point(12, 12);
      this->rbtn_paint->Name = L"rbtn_paint";
      this->rbtn_paint->Size = System::Drawing::Size(134, 29);
      this->rbtn_paint->TabIndex = 0;
      this->rbtn_paint->TabStop = true;
      this->rbtn_paint->Text = L"Paint Mode";
      this->rbtn_paint->UseVisualStyleBackColor = true;
      this->rbtn_paint->CheckedChanged += gcnew System::EventHandler(this, &FormSegVoxelPaint::rbtn_paint_CheckedChanged);
      // 
      // rbtn_lasso
      // 
      this->rbtn_lasso->AutoSize = true;
      this->rbtn_lasso->Font = (gcnew System::Drawing::Font(L"メイリオ", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->rbtn_lasso->Location = System::Drawing::Point(12, 108);
      this->rbtn_lasso->Name = L"rbtn_lasso";
      this->rbtn_lasso->Size = System::Drawing::Size(137, 29);
      this->rbtn_lasso->TabIndex = 1;
      this->rbtn_lasso->Text = L"Lasso Mode";
      this->rbtn_lasso->UseVisualStyleBackColor = true;
      this->rbtn_lasso->CheckedChanged += gcnew System::EventHandler(this, &FormSegVoxelPaint::rbtn_lasso_CheckedChanged);
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label1->Location = System::Drawing::Point(8, 44);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(265, 23);
      this->label1->TabIndex = 2;
      this->label1->Text = L"Shift + L-drag : Paint Fore Voxel";
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label2->Location = System::Drawing::Point(7, 67);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(270, 23);
      this->label2->TabIndex = 3;
      this->label2->Text = L"Shift + R-drag : Erase Fore Voxel";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label3->Location = System::Drawing::Point(8, 163);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(299, 23);
      this->label3->TabIndex = 5;
      this->label3->Text = L"Shift + R-drag : Draw lasso for erase";
      // 
      // label4
      // 
      this->label4->AutoSize = true;
      this->label4->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label4->Location = System::Drawing::Point(9, 140);
      this->label4->Name = L"label4";
      this->label4->Size = System::Drawing::Size(290, 23);
      this->label4->TabIndex = 4;
      this->label4->Text = L"Shift + L-drag : Draw lasso for Fore";
      // 
      // btn_Cancel
      // 
      this->btn_Cancel->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_Cancel->Location = System::Drawing::Point(7, 219);
      this->btn_Cancel->Name = L"btn_Cancel";
      this->btn_Cancel->Size = System::Drawing::Size(91, 34);
      this->btn_Cancel->TabIndex = 6;
      this->btn_Cancel->Text = L"CANCEL";
      this->btn_Cancel->UseVisualStyleBackColor = true;
      this->btn_Cancel->Click += gcnew System::EventHandler(this, &FormSegVoxelPaint::btn_Cancel_Click);
      // 
      // btn_finish
      // 
      this->btn_finish->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_finish->Location = System::Drawing::Point(101, 219);
      this->btn_finish->Name = L"btn_finish";
      this->btn_finish->Size = System::Drawing::Size(202, 34);
      this->btn_finish->TabIndex = 7;
      this->btn_finish->Text = L"FINISH segmentation";
      this->btn_finish->UseVisualStyleBackColor = true;
      this->btn_finish->Click += gcnew System::EventHandler(this, &FormSegVoxelPaint::btn_finish_Click);
      // 
      // FormSegVoxelPaint
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(312, 277);
      this->Controls->Add(this->btn_finish);
      this->Controls->Add(this->btn_Cancel);
      this->Controls->Add(this->label3);
      this->Controls->Add(this->label4);
      this->Controls->Add(this->label2);
      this->Controls->Add(this->label1);
      this->Controls->Add(this->rbtn_lasso);
      this->Controls->Add(this->rbtn_paint);
      this->Name = L"FormSegVoxelPaint";
      this->Text = L"FormSegVoxelPaint";
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private: System::Void rbtn_paint_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void rbtn_lasso_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void btn_Cancel_Click(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void btn_finish_Click(System::Object^  sender, System::EventArgs^  e);
};

  inline void formSegVoxelPaint_Hide(){
    FormSegVoxelPaint::getInst()->Hide();
  }

  inline void formSegVoxelPaint_Show(){
    FormSegVoxelPaint::getInst()->Show();
  }
  inline bool formSegVoxelPaint_bLassoMode(){
    return FormSegVoxelPaint::getInst()->bLassoMode();
  }
}
