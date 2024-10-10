#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegParallelWires の概要
	/// </summary>
	public ref class FormSegParallelWires : public System::Windows::Forms::Form
	{

  private:
    static FormSegParallelWires^ m_singleton;

	private:
		FormSegParallelWires(void);

  public:
    static FormSegParallelWires^ GetInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormSegParallelWires();
      return m_singleton;
    }

  bool IsTrgetXY(){
    return m_radiobtn_planexy->Checked;
  }
  bool IsTrgetYZ(){
    return m_radiobtn_planeyz->Checked;
  }
  bool IsTrgetZX(){
    return m_radiobtn_planezx->Checked;
  }
  bool IsDrawAllWires(){
    return m_checkbox_showwires->Checked;
  }


	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormSegParallelWires()
		{
			if (components)
			{
				delete components;
			}
		}

  private: 



    System::Windows::Forms::Label^  m_label1;
    System::Windows::Forms::Label^  m_label2;
    System::Windows::Forms::Label^  m_label3;
    System::Windows::Forms::GroupBox^  m_groupbox;
    System::Windows::Forms::Button^  m_btn_exportwires;
    System::Windows::Forms::Button^  m_btn_importwires;
    System::Windows::Forms::Button^  m_btn_finish;
    System::Windows::Forms::Button^  m_btn_cancel;
    System::Windows::Forms::RadioButton^  m_radiobtn_planexy;
    System::Windows::Forms::RadioButton^  m_radiobtn_planeyz;
    System::Windows::Forms::RadioButton^  m_radiobtn_planezx;
  private: System::Windows::Forms::Label^  m_label4;
  private: System::Windows::Forms::TrackBar^  m_trackbar_cpsize;
  private: System::Windows::Forms::CheckBox^  m_checkbox_showwires;





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
      this->m_label1 = (gcnew System::Windows::Forms::Label());
      this->m_label3 = (gcnew System::Windows::Forms::Label());
      this->m_label2 = (gcnew System::Windows::Forms::Label());
      this->m_groupbox = (gcnew System::Windows::Forms::GroupBox());
      this->m_radiobtn_planezx = (gcnew System::Windows::Forms::RadioButton());
      this->m_radiobtn_planeyz = (gcnew System::Windows::Forms::RadioButton());
      this->m_radiobtn_planexy = (gcnew System::Windows::Forms::RadioButton());
      this->m_btn_exportwires = (gcnew System::Windows::Forms::Button());
      this->m_btn_importwires = (gcnew System::Windows::Forms::Button());
      this->m_btn_finish = (gcnew System::Windows::Forms::Button());
      this->m_btn_cancel = (gcnew System::Windows::Forms::Button());
      this->m_label4 = (gcnew System::Windows::Forms::Label());
      this->m_trackbar_cpsize = (gcnew System::Windows::Forms::TrackBar());
      this->m_checkbox_showwires = (gcnew System::Windows::Forms::CheckBox());
      this->m_groupbox->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_cpsize))->BeginInit();
      this->SuspendLayout();
      // 
      // m_label1
      // 
      this->m_label1->AutoSize = true;
      this->m_label1->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_label1->Location = System::Drawing::Point(2, 7);
      this->m_label1->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
      this->m_label1->Name = L"m_label1";
      this->m_label1->Size = System::Drawing::Size(297, 60);
      this->m_label1->TabIndex = 1;
      this->m_label1->Text = L"●Shift + L click : add control points\r\n# click on a cirve -> \"insert\" a point\r\n# "
        L"otherwise         -> add a point at the end";
      // 
      // m_label3
      // 
      this->m_label3->AutoSize = true;
      this->m_label3->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_label3->Location = System::Drawing::Point(2, 92);
      this->m_label3->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
      this->m_label3->Name = L"m_label3";
      this->m_label3->Size = System::Drawing::Size(246, 20);
      this->m_label3->TabIndex = 2;
      this->m_label3->Text = L"●Shift + R/M click : remove point ";
      // 
      // m_label2
      // 
      this->m_label2->AutoSize = true;
      this->m_label2->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_label2->Location = System::Drawing::Point(2, 66);
      this->m_label2->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
      this->m_label2->Name = L"m_label2";
      this->m_label2->Size = System::Drawing::Size(268, 20);
      this->m_label2->TabIndex = 1;
      this->m_label2->Text = L"●Shift + L drag : move a control point";
      // 
      // m_groupbox
      // 
      this->m_groupbox->Controls->Add(this->m_radiobtn_planezx);
      this->m_groupbox->Controls->Add(this->m_radiobtn_planeyz);
      this->m_groupbox->Controls->Add(this->m_radiobtn_planexy);
      this->m_groupbox->Location = System::Drawing::Point(11, 124);
      this->m_groupbox->Name = L"m_groupbox";
      this->m_groupbox->Size = System::Drawing::Size(205, 58);
      this->m_groupbox->TabIndex = 5;
      this->m_groupbox->TabStop = false;
      this->m_groupbox->Text = L"Target Plane";
      // 
      // m_radiobtn_planezx
      // 
      this->m_radiobtn_planezx->AutoSize = true;
      this->m_radiobtn_planezx->Location = System::Drawing::Point(147, 27);
      this->m_radiobtn_planezx->Name = L"m_radiobtn_planezx";
      this->m_radiobtn_planezx->Size = System::Drawing::Size(53, 26);
      this->m_radiobtn_planezx->TabIndex = 32;
      this->m_radiobtn_planezx->Text = L"ZX";
      this->m_radiobtn_planezx->UseVisualStyleBackColor = true;
      this->m_radiobtn_planezx->CheckedChanged += gcnew System::EventHandler(this, &FormSegParallelWires::m_radiobtn_planezx_CheckedChanged);
      // 
      // m_radiobtn_planeyz
      // 
      this->m_radiobtn_planeyz->AutoSize = true;
      this->m_radiobtn_planeyz->Location = System::Drawing::Point(80, 27);
      this->m_radiobtn_planeyz->Name = L"m_radiobtn_planeyz";
      this->m_radiobtn_planeyz->Size = System::Drawing::Size(53, 26);
      this->m_radiobtn_planeyz->TabIndex = 31;
      this->m_radiobtn_planeyz->Text = L"YZ";
      this->m_radiobtn_planeyz->UseVisualStyleBackColor = true;
      this->m_radiobtn_planeyz->CheckedChanged += gcnew System::EventHandler(this, &FormSegParallelWires::m_radiobtn_planeyz_CheckedChanged);
      // 
      // m_radiobtn_planexy
      // 
      this->m_radiobtn_planexy->AutoSize = true;
      this->m_radiobtn_planexy->Checked = true;
      this->m_radiobtn_planexy->Location = System::Drawing::Point(10, 27);
      this->m_radiobtn_planexy->Name = L"m_radiobtn_planexy";
      this->m_radiobtn_planexy->Size = System::Drawing::Size(53, 26);
      this->m_radiobtn_planexy->TabIndex = 30;
      this->m_radiobtn_planexy->TabStop = true;
      this->m_radiobtn_planexy->Text = L"XY";
      this->m_radiobtn_planexy->UseVisualStyleBackColor = true;
      this->m_radiobtn_planexy->CheckedChanged += gcnew System::EventHandler(this, &FormSegParallelWires::m_radiobtn_planexy_CheckedChanged);
      // 
      // m_btn_exportwires
      // 
      this->m_btn_exportwires->Location = System::Drawing::Point(10, 280);
      this->m_btn_exportwires->Name = L"m_btn_exportwires";
      this->m_btn_exportwires->Size = System::Drawing::Size(112, 31);
      this->m_btn_exportwires->TabIndex = 6;
      this->m_btn_exportwires->Text = L"Export Wire";
      this->m_btn_exportwires->UseVisualStyleBackColor = true;
      this->m_btn_exportwires->Click += gcnew System::EventHandler(this, &FormSegParallelWires::m_btn_exportwires_Click);
      // 
      // m_btn_importwires
      // 
      this->m_btn_importwires->Location = System::Drawing::Point(129, 280);
      this->m_btn_importwires->Name = L"m_btn_importwires";
      this->m_btn_importwires->Size = System::Drawing::Size(112, 31);
      this->m_btn_importwires->TabIndex = 7;
      this->m_btn_importwires->Text = L"Import Wire";
      this->m_btn_importwires->UseVisualStyleBackColor = true;
      this->m_btn_importwires->Click += gcnew System::EventHandler(this, &FormSegParallelWires::m_btn_importwires_Click);
      // 
      // m_btn_finish
      // 
      this->m_btn_finish->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_btn_finish->Location = System::Drawing::Point(106, 331);
      this->m_btn_finish->Margin = System::Windows::Forms::Padding(2);
      this->m_btn_finish->Name = L"m_btn_finish";
      this->m_btn_finish->Size = System::Drawing::Size(193, 38);
      this->m_btn_finish->TabIndex = 28;
      this->m_btn_finish->Text = L"FINISH and store region";
      this->m_btn_finish->UseVisualStyleBackColor = true;
      this->m_btn_finish->Click += gcnew System::EventHandler(this, &FormSegParallelWires::m_btn_finish_Click);
      // 
      // m_btn_cancel
      // 
      this->m_btn_cancel->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_btn_cancel->Location = System::Drawing::Point(8, 331);
      this->m_btn_cancel->Margin = System::Windows::Forms::Padding(2);
      this->m_btn_cancel->Name = L"m_btn_cancel";
      this->m_btn_cancel->Size = System::Drawing::Size(94, 38);
      this->m_btn_cancel->TabIndex = 29;
      this->m_btn_cancel->Text = L"CANCEL";
      this->m_btn_cancel->UseVisualStyleBackColor = true;
      this->m_btn_cancel->Click += gcnew System::EventHandler(this, &FormSegParallelWires::m_btn_cancel_Click);
      // 
      // m_label4
      // 
      this->m_label4->AutoSize = true;
      this->m_label4->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_label4->Location = System::Drawing::Point(4, 189);
      this->m_label4->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
      this->m_label4->Name = L"m_label4";
      this->m_label4->Size = System::Drawing::Size(130, 20);
      this->m_label4->TabIndex = 30;
      this->m_label4->Text = L"Control Point Size";
      // 
      // m_trackbar_cpsize
      // 
      this->m_trackbar_cpsize->Location = System::Drawing::Point(6, 210);
      this->m_trackbar_cpsize->Name = L"m_trackbar_cpsize";
      this->m_trackbar_cpsize->Size = System::Drawing::Size(210, 56);
      this->m_trackbar_cpsize->TabIndex = 31;
      this->m_trackbar_cpsize->TickStyle = System::Windows::Forms::TickStyle::None;
      this->m_trackbar_cpsize->Scroll += gcnew System::EventHandler(this, &FormSegParallelWires::m_trackbar_cpsize_Scroll);
      // 
      // m_checkbox_showwires
      // 
      this->m_checkbox_showwires->AutoSize = true;
      this->m_checkbox_showwires->Checked = true;
      this->m_checkbox_showwires->CheckState = System::Windows::Forms::CheckState::Checked;
      this->m_checkbox_showwires->Location = System::Drawing::Point(10, 251);
      this->m_checkbox_showwires->Name = L"m_checkbox_showwires";
      this->m_checkbox_showwires->Size = System::Drawing::Size(142, 26);
      this->m_checkbox_showwires->TabIndex = 32;
      this->m_checkbox_showwires->Text = L"show all wires";
      this->m_checkbox_showwires->UseVisualStyleBackColor = true;
      this->m_checkbox_showwires->CheckedChanged += gcnew System::EventHandler(this, &FormSegParallelWires::m_checkbox_showwires_CheckedChanged);
      // 
      // FormSegParallelWires
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(9, 21);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(309, 377);
      this->Controls->Add(this->m_checkbox_showwires);
      this->Controls->Add(this->m_trackbar_cpsize);
      this->Controls->Add(this->m_label4);
      this->Controls->Add(this->m_btn_finish);
      this->Controls->Add(this->m_btn_cancel);
      this->Controls->Add(this->m_btn_importwires);
      this->Controls->Add(this->m_btn_exportwires);
      this->Controls->Add(this->m_groupbox);
      this->Controls->Add(this->m_label3);
      this->Controls->Add(this->m_label2);
      this->Controls->Add(this->m_label1);
      this->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->Name = L"FormSegParallelWires";
      this->Text = L"FormSegParallelWires";
      this->m_groupbox->ResumeLayout(false);
      this->m_groupbox->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_cpsize))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private: 
    System::Void m_btn_exportwires_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_btn_importwires_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_btn_cancel_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_btn_finish_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_radiobtn_planexy_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_radiobtn_planeyz_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_radiobtn_planezx_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_checkbox_showwires_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_trackbar_cpsize_Scroll(System::Object^  sender, System::EventArgs^  e);
};

  inline void FormSegParallelWires_Hide(){
    FormSegParallelWires::GetInst()->Hide();
  }
  inline void FormSegParallelWires_Show(){
    FormSegParallelWires::GetInst()->Show();
  }
  
  inline bool FormParallelWires_bPlaneXY(){ 
    return FormSegParallelWires::GetInst()->IsTrgetXY();
  }
  inline bool FormParallelWires_bPlaneYZ(){ 
    return FormSegParallelWires::GetInst()->IsTrgetYZ();
  }
  inline bool FormParallelWires_bPlaneZX(){ 
    return FormSegParallelWires::GetInst()->IsTrgetZX();
  }

  inline bool FormParallelWires_bDrawAllWires(){
    return FormSegParallelWires::GetInst()->IsDrawAllWires();
  }


}
