#pragma once

namespace RoiPainter4D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegSwallowOrganTimeline の概要
	/// </summary>
	public ref class FormSegSwallowOrganTimeline : public System::Windows::Forms::Form
	{

    Bitmap^ m_background;
    static FormSegSwallowOrganTimeline^ m_singleton;
    
    bool m_bLbtn;
    int  m_pick_fi, m_pick_xyz;
    int  m_selectx0, m_selectx1; 

  private: 


  public:
    static FormSegSwallowOrganTimeline^ GetInst() {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormSegSwallowOrganTimeline();
      }
      return m_singleton;
    }

    void RepaintPanel(){
      m_panel->Invalidate();
      m_panel->Update();
    }

    void InitChart(int num_frames);
    

	private:
		FormSegSwallowOrganTimeline(void);

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormSegSwallowOrganTimeline()
		{
			if (components)
			{
				delete components;
			}
		}

  private: 
    System::Windows::Forms::Panel^     m_panel;
    System::Windows::Forms::Label^     label1;
    System::Windows::Forms::Label^     label2;
    System::Windows::Forms::Label^     label3;
    System::Windows::Forms::Button^    m_btn_smoothing;
    System::Windows::Forms::Button^    m_btm_copyfromleft;
    System::Windows::Forms::Button^    m_btn_copyfromright;
    System::Windows::Forms::CheckBox^  m_checkbox_showmulticps;
    System::Windows::Forms::NumericUpDown^  m_updown_yaxisscale;

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
      this->m_panel = (gcnew System::Windows::Forms::Panel());
      this->m_btn_smoothing = (gcnew System::Windows::Forms::Button());
      this->m_btm_copyfromleft = (gcnew System::Windows::Forms::Button());
      this->m_btn_copyfromright = (gcnew System::Windows::Forms::Button());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->m_updown_yaxisscale = (gcnew System::Windows::Forms::NumericUpDown());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->m_checkbox_showmulticps = (gcnew System::Windows::Forms::CheckBox());
      this->label3 = (gcnew System::Windows::Forms::Label());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_updown_yaxisscale))->BeginInit();
      this->SuspendLayout();
      // 
      // m_panel
      // 
      this->m_panel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
        | System::Windows::Forms::AnchorStyles::Left)
        | System::Windows::Forms::AnchorStyles::Right));
      this->m_panel->AutoSize = true;
      this->m_panel->BackColor = System::Drawing::SystemColors::ButtonHighlight;
      this->m_panel->Location = System::Drawing::Point(1, 1);
      this->m_panel->Name = L"m_panel";
      this->m_panel->Size = System::Drawing::Size(700, 274);
      this->m_panel->TabIndex = 0;
      this->m_panel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &FormSegSwallowOrganTimeline::m_panel_Paint);
      this->m_panel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &FormSegSwallowOrganTimeline::m_panel_MouseDown);
      this->m_panel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &FormSegSwallowOrganTimeline::m_panel_MouseMove);
      this->m_panel->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &FormSegSwallowOrganTimeline::m_panel_MouseUp);
      this->m_panel->Resize += gcnew System::EventHandler(this, &FormSegSwallowOrganTimeline::m_panel_Resize);
      // 
      // m_btn_smoothing
      // 
      this->m_btn_smoothing->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
      this->m_btn_smoothing->Location = System::Drawing::Point(223, 292);
      this->m_btn_smoothing->Name = L"m_btn_smoothing";
      this->m_btn_smoothing->Size = System::Drawing::Size(75, 23);
      this->m_btn_smoothing->TabIndex = 1;
      this->m_btn_smoothing->Text = L"Smoothing";
      this->m_btn_smoothing->UseVisualStyleBackColor = true;
      this->m_btn_smoothing->Click += gcnew System::EventHandler(this, &FormSegSwallowOrganTimeline::m_btn_smoothing_Click);
      this->m_btn_smoothing->Resize += gcnew System::EventHandler(this, &FormSegSwallowOrganTimeline::m_btn_smoothing_Resize);
      // 
      // m_btm_copyfromleft
      // 
      this->m_btm_copyfromleft->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
      this->m_btm_copyfromleft->Location = System::Drawing::Point(304, 292);
      this->m_btm_copyfromleft->Name = L"m_btm_copyfromleft";
      this->m_btm_copyfromleft->Size = System::Drawing::Size(103, 23);
      this->m_btm_copyfromleft->TabIndex = 1;
      this->m_btm_copyfromleft->Text = L"Copy from Left";
      this->m_btm_copyfromleft->UseVisualStyleBackColor = true;
      this->m_btm_copyfromleft->Click += gcnew System::EventHandler(this, &FormSegSwallowOrganTimeline::m_btm_copyfromleft_Click);
      this->m_btm_copyfromleft->Resize += gcnew System::EventHandler(this, &FormSegSwallowOrganTimeline::m_btn_smoothing_Resize);
      // 
      // m_btn_copyfromright
      // 
      this->m_btn_copyfromright->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
      this->m_btn_copyfromright->Location = System::Drawing::Point(413, 292);
      this->m_btn_copyfromright->Name = L"m_btn_copyfromright";
      this->m_btn_copyfromright->Size = System::Drawing::Size(103, 23);
      this->m_btn_copyfromright->TabIndex = 2;
      this->m_btn_copyfromright->Text = L"Copy from Right";
      this->m_btn_copyfromright->UseVisualStyleBackColor = true;
      this->m_btn_copyfromright->Click += gcnew System::EventHandler(this, &FormSegSwallowOrganTimeline::m_btn_copyfromright_Click);
      this->m_btn_copyfromright->Resize += gcnew System::EventHandler(this, &FormSegSwallowOrganTimeline::m_btn_smoothing_Resize);
      // 
      // label1
      // 
      this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
      this->label1->AutoSize = true;
      this->label1->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label1->Location = System::Drawing::Point(8, 287);
      this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(211, 28);
      this->label1->TabIndex = 3;
      this->label1->Text = L"shift+L drag : select sequence\r\n       L-drag : move cp";
      // 
      // m_updown_yaxisscale
      // 
      this->m_updown_yaxisscale->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
      this->m_updown_yaxisscale->DecimalPlaces = 1;
      this->m_updown_yaxisscale->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 65536 });
      this->m_updown_yaxisscale->Location = System::Drawing::Point(621, 277);
      this->m_updown_yaxisscale->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
      this->m_updown_yaxisscale->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 65536 });
      this->m_updown_yaxisscale->Name = L"m_updown_yaxisscale";
      this->m_updown_yaxisscale->Size = System::Drawing::Size(61, 19);
      this->m_updown_yaxisscale->TabIndex = 4;
      this->m_updown_yaxisscale->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
      this->m_updown_yaxisscale->ValueChanged += gcnew System::EventHandler(this, &FormSegSwallowOrganTimeline::m_updown_yaxisscale_ValueChanged);
      // 
      // label2
      // 
      this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
      this->label2->AutoSize = true;
      this->label2->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label2->Location = System::Drawing::Point(530, 279);
      this->label2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(89, 14);
      this->label2->TabIndex = 3;
      this->label2->Text = L"y-axis scale";
      // 
      // m_checkbox_showmulticps
      // 
      this->m_checkbox_showmulticps->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
      this->m_checkbox_showmulticps->AutoSize = true;
      this->m_checkbox_showmulticps->Checked = true;
      this->m_checkbox_showmulticps->CheckState = System::Windows::Forms::CheckState::Checked;
      this->m_checkbox_showmulticps->Location = System::Drawing::Point(531, 304);
      this->m_checkbox_showmulticps->Name = L"m_checkbox_showmulticps";
      this->m_checkbox_showmulticps->Size = System::Drawing::Size(99, 16);
      this->m_checkbox_showmulticps->TabIndex = 5;
      this->m_checkbox_showmulticps->Text = L"show multi pts";
      this->m_checkbox_showmulticps->UseVisualStyleBackColor = true;
      // 
      // label3
      // 
      this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
      this->label3->AutoSize = true;
      this->label3->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label3->Location = System::Drawing::Point(531, 319);
      this->label3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(168, 13);
      this->label3->TabIndex = 3;
      this->label3->Text = L"all selected pts or 1-ring pts";
      // 
      // FormSegSwallowOrganTimeline
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(704, 340);
      this->ControlBox = false;
      this->Controls->Add(this->m_checkbox_showmulticps);
      this->Controls->Add(this->m_updown_yaxisscale);
      this->Controls->Add(this->m_btn_copyfromright);
      this->Controls->Add(this->m_btm_copyfromleft);
      this->Controls->Add(this->m_btn_smoothing);
      this->Controls->Add(this->m_panel);
      this->Controls->Add(this->label3);
      this->Controls->Add(this->label2);
      this->Controls->Add(this->label1);
      this->Name = L"FormSegSwallowOrganTimeline";
      this->Text = L"Timeline";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_updown_yaxisscale))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private: 
    System::Void m_panel_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e); 
    System::Void m_btn_smoothing_Resize(System::Object^  sender, System::EventArgs^  e);
    System::Void m_panel_Resize(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_panel_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
    System::Void m_panel_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
    System::Void m_panel_MouseUp  (System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
    System::Void m_btn_smoothing_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_btm_copyfromleft_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_btn_copyfromright_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_updown_yaxisscale_ValueChanged(System::Object^  sender, System::EventArgs^  e);
};

  inline void FormSegSwallowTimeline_RedrawPanel(){
    FormSegSwallowOrganTimeline::GetInst()->RepaintPanel();
  }

  inline void FormSegSwallowTimeline_StartMode( int num_frames ){
    FormSegSwallowOrganTimeline::GetInst()->InitChart( num_frames );
  }

  inline void FormSegSwallowTimeline_Hide() {
    FormSegSwallowOrganTimeline::GetInst()->Hide();
  }

  inline void FormSegSwallowTimeline_Show() {
    FormSegSwallowOrganTimeline::GetInst()->Show();
  }

}
