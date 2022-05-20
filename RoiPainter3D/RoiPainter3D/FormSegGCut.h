#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegGCut の概要
	/// </summary>
	public ref class FormSegGCut : public System::Windows::Forms::Form
	{    
  private:
    static FormSegGCut^ m_singleton;
		FormSegGCut(void);
  public:
    static FormSegGCut^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormSegGCut();
      return m_singleton;
    }

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormSegGCut()
		{
			if (components)
			{
				delete components;
			}
		}

  private:
    System::Windows::Forms::Label^  label1;
    System::Windows::Forms::Label^  label2;
    System::Windows::Forms::Label^  label3;
    System::Windows::Forms::Label^  label4;
    System::Windows::Forms::Button^  btn_clearAllCPs;
    System::Windows::Forms::Button^  btn_runGraphCutWS;
    System::Windows::Forms::Button^  btn_runGraphCutVoxel;
    System::Windows::Forms::Button^  btn_canel;
    System::Windows::Forms::Button^  btn_finish;
    System::Windows::Forms::TextBox^  textBox_lambda;
    System::Windows::Forms::TrackBar^  trackBar_lambda;
    System::Windows::Forms::RadioButton^  rbtn_thick1;
    System::Windows::Forms::RadioButton^  rbtn_thick2;
    System::Windows::Forms::RadioButton^  rbtn_thick3;
    System::Windows::Forms::CheckBox^  checkBox_bandOnlyBack;

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
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->btn_clearAllCPs = (gcnew System::Windows::Forms::Button());
      this->label4 = (gcnew System::Windows::Forms::Label());
      this->textBox_lambda = (gcnew System::Windows::Forms::TextBox());
      this->trackBar_lambda = (gcnew System::Windows::Forms::TrackBar());
      this->rbtn_thick1 = (gcnew System::Windows::Forms::RadioButton());
      this->btn_runGraphCutWS = (gcnew System::Windows::Forms::Button());
      this->btn_runGraphCutVoxel = (gcnew System::Windows::Forms::Button());
      this->rbtn_thick2 = (gcnew System::Windows::Forms::RadioButton());
      this->rbtn_thick3 = (gcnew System::Windows::Forms::RadioButton());
      this->checkBox_bandOnlyBack = (gcnew System::Windows::Forms::CheckBox());
      this->btn_canel = (gcnew System::Windows::Forms::Button());
      this->btn_finish = (gcnew System::Windows::Forms::Button());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_lambda))->BeginInit();
      this->SuspendLayout();
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label1->Location = System::Drawing::Point(6, 3);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(177, 20);
      this->label1->TabIndex = 2;
      this->label1->Text = L"shift L-drag : add Fore CPs";
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label2->Location = System::Drawing::Point(4, 20);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(182, 20);
      this->label2->TabIndex = 3;
      this->label2->Text = L"shift R-drag : add Back CPs";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label3->Location = System::Drawing::Point(1, 37);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(161, 20);
      this->label3->TabIndex = 4;
      this->label3->Text = L"shift M-drag : erase CPs";
      // 
      // btn_clearAllCPs
      // 
      this->btn_clearAllCPs->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_clearAllCPs->Location = System::Drawing::Point(122, 60);
      this->btn_clearAllCPs->Name = L"btn_clearAllCPs";
      this->btn_clearAllCPs->Size = System::Drawing::Size(87, 21);
      this->btn_clearAllCPs->TabIndex = 5;
      this->btn_clearAllCPs->Text = L"clear all CPs";
      this->btn_clearAllCPs->UseVisualStyleBackColor = true;
      this->btn_clearAllCPs->Click += gcnew System::EventHandler(this, &FormSegGCut::btn_clearAllCPs_Click);
      // 
      // label4
      // 
      this->label4->AutoSize = true;
      this->label4->Location = System::Drawing::Point(68, 91);
      this->label4->Name = L"label4";
      this->label4->Size = System::Drawing::Size(151, 12);
      this->label4->TabIndex = 6;
      this->label4->Text = L"lambda : E = E1(i) + λE2(i,j)";
      // 
      // textBox_lambda
      // 
      this->textBox_lambda->Location = System::Drawing::Point(10, 88);
      this->textBox_lambda->Name = L"textBox_lambda";
      this->textBox_lambda->ReadOnly = true;
      this->textBox_lambda->Size = System::Drawing::Size(58, 19);
      this->textBox_lambda->TabIndex = 7;
      // 
      // trackBar_lambda
      // 
      this->trackBar_lambda->Location = System::Drawing::Point(3, 106);
      this->trackBar_lambda->Name = L"trackBar_lambda";
      this->trackBar_lambda->Size = System::Drawing::Size(178, 45);
      this->trackBar_lambda->TabIndex = 8;
      this->trackBar_lambda->TickStyle = System::Windows::Forms::TickStyle::None;
      this->trackBar_lambda->Scroll += gcnew System::EventHandler(this, &FormSegGCut::trackBar_lambda_Scroll);
      // 
      // rbtn_thick1
      // 
      this->rbtn_thick1->AutoSize = true;
      this->rbtn_thick1->Location = System::Drawing::Point(6, 193);
      this->rbtn_thick1->Name = L"rbtn_thick1";
      this->rbtn_thick1->Size = System::Drawing::Size(62, 16);
      this->rbtn_thick1->TabIndex = 9;
      this->rbtn_thick1->TabStop = true;
      this->rbtn_thick1->Text = L"bandW1";
      this->rbtn_thick1->UseVisualStyleBackColor = true;
      this->rbtn_thick1->CheckedChanged += gcnew System::EventHandler(this, &FormSegGCut::rbtn_thick1_CheckedChanged);
      // 
      // btn_runGraphCutWS
      // 
      this->btn_runGraphCutWS->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_runGraphCutWS->Location = System::Drawing::Point(6, 130);
      this->btn_runGraphCutWS->Name = L"btn_runGraphCutWS";
      this->btn_runGraphCutWS->Size = System::Drawing::Size(207, 27);
      this->btn_runGraphCutWS->TabIndex = 5;
      this->btn_runGraphCutWS->Text = L"1. run Graph Cut Watashed Lv.";
      this->btn_runGraphCutWS->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
      this->btn_runGraphCutWS->UseVisualStyleBackColor = true;
      this->btn_runGraphCutWS->Click += gcnew System::EventHandler(this, &FormSegGCut::btn_runGraphCutWS_Click);
      // 
      // btn_runGraphCutVoxel
      // 
      this->btn_runGraphCutVoxel->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_runGraphCutVoxel->Location = System::Drawing::Point(7, 161);
      this->btn_runGraphCutVoxel->Name = L"btn_runGraphCutVoxel";
      this->btn_runGraphCutVoxel->Size = System::Drawing::Size(206, 26);
      this->btn_runGraphCutVoxel->TabIndex = 13;
      this->btn_runGraphCutVoxel->Text = L"2. run Graph Cut voxel level";
      this->btn_runGraphCutVoxel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
      this->btn_runGraphCutVoxel->UseVisualStyleBackColor = true;
      this->btn_runGraphCutVoxel->Click += gcnew System::EventHandler(this, &FormSegGCut::btn_runGraphCutVoxel_Click);
      // 
      // rbtn_thick2
      // 
      this->rbtn_thick2->AutoSize = true;
      this->rbtn_thick2->Location = System::Drawing::Point(75, 193);
      this->rbtn_thick2->Name = L"rbtn_thick2";
      this->rbtn_thick2->Size = System::Drawing::Size(62, 16);
      this->rbtn_thick2->TabIndex = 9;
      this->rbtn_thick2->TabStop = true;
      this->rbtn_thick2->Text = L"bandW2";
      this->rbtn_thick2->UseVisualStyleBackColor = true;
      this->rbtn_thick2->CheckedChanged += gcnew System::EventHandler(this, &FormSegGCut::rbtn_thick2_CheckedChanged);
      // 
      // rbtn_thick3
      // 
      this->rbtn_thick3->AutoSize = true;
      this->rbtn_thick3->Location = System::Drawing::Point(147, 193);
      this->rbtn_thick3->Name = L"rbtn_thick3";
      this->rbtn_thick3->Size = System::Drawing::Size(62, 16);
      this->rbtn_thick3->TabIndex = 9;
      this->rbtn_thick3->TabStop = true;
      this->rbtn_thick3->Text = L"bandW3";
      this->rbtn_thick3->UseVisualStyleBackColor = true;
      this->rbtn_thick3->CheckedChanged += gcnew System::EventHandler(this, &FormSegGCut::rbtn_thick3_CheckedChanged);
      // 
      // checkBox_bandOnlyBack
      // 
      this->checkBox_bandOnlyBack->AutoSize = true;
      this->checkBox_bandOnlyBack->Location = System::Drawing::Point(113, 215);
      this->checkBox_bandOnlyBack->Name = L"checkBox_bandOnlyBack";
      this->checkBox_bandOnlyBack->Size = System::Drawing::Size(101, 16);
      this->checkBox_bandOnlyBack->TabIndex = 14;
      this->checkBox_bandOnlyBack->Text = L"bond only back";
      this->checkBox_bandOnlyBack->UseVisualStyleBackColor = true;
      this->checkBox_bandOnlyBack->CheckedChanged += gcnew System::EventHandler(this, &FormSegGCut::checkBox_bandOnlyBack_CheckedChanged);
      // 
      // btn_canel
      // 
      this->btn_canel->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_canel->Location = System::Drawing::Point(3, 246);
      this->btn_canel->Name = L"btn_canel";
      this->btn_canel->Size = System::Drawing::Size(56, 28);
      this->btn_canel->TabIndex = 5;
      this->btn_canel->Text = L"cancel";
      this->btn_canel->UseVisualStyleBackColor = true;
      this->btn_canel->Click += gcnew System::EventHandler(this, &FormSegGCut::btn_canel_Click);
      // 
      // btn_finish
      // 
      this->btn_finish->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_finish->Location = System::Drawing::Point(60, 246);
      this->btn_finish->Name = L"btn_finish";
      this->btn_finish->Size = System::Drawing::Size(155, 28);
      this->btn_finish->TabIndex = 15;
      this->btn_finish->Text = L"End ＆ Store Region";
      this->btn_finish->UseVisualStyleBackColor = true;
      this->btn_finish->Click += gcnew System::EventHandler(this, &FormSegGCut::btn_finish_Click);
      // 
      // FormSegGCut
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(217, 278);
      this->Controls->Add(this->btn_finish);
      this->Controls->Add(this->checkBox_bandOnlyBack);
      this->Controls->Add(this->btn_runGraphCutVoxel);
      this->Controls->Add(this->btn_runGraphCutWS);
      this->Controls->Add(this->rbtn_thick3);
      this->Controls->Add(this->rbtn_thick2);
      this->Controls->Add(this->rbtn_thick1);
      this->Controls->Add(this->trackBar_lambda);
      this->Controls->Add(this->textBox_lambda);
      this->Controls->Add(this->label4);
      this->Controls->Add(this->btn_canel);
      this->Controls->Add(this->btn_clearAllCPs);
      this->Controls->Add(this->label3);
      this->Controls->Add(this->label2);
      this->Controls->Add(this->label1);
      this->Name = L"FormSegGCut";
      this->Text = L"FormSegGCut";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_lambda))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private: 
    System::Void btn_clearAllCPs_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void trackBar_lambda_Scroll(System::Object^  sender, System::EventArgs^  e) ;
    System::Void btn_runGraphCutWS_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void btn_runGraphCutVoxel_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void rbtn_thick1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void rbtn_thick2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void rbtn_thick3_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void checkBox_bandOnlyBack_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void btn_canel_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void btn_finish_Click(System::Object^  sender, System::EventArgs^  e) ;
};
  inline void formSegGCut_Hide(){
    FormSegGCut::getInst()->Hide();
  }
  inline void formSegGCut_Show(){
    FormSegGCut::getInst()->Show();
  }
}
