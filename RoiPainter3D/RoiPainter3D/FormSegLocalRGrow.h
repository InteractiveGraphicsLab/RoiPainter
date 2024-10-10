#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegLocalRGrow の概要
	/// </summary>
	public ref class FormSegLocalRGrow : public System::Windows::Forms::Form
	{
  private:
    System::Windows::Forms::Label^ m_label1;
    System::Windows::Forms::Label^ m_label3;
    System::Windows::Forms::Label^ m_label4;
    System::Windows::Forms::Label^ m_label5;
    System::Windows::Forms::Label^ m_label6;
    System::Windows::Forms::TextBox^ textBox_minV;
    System::Windows::Forms::TextBox^ textBox_maxV;
    System::Windows::Forms::TextBox^ textBox_radius;
    System::Windows::Forms::TrackBar^ trackBar_minV;
    System::Windows::Forms::TrackBar^ trackBar_maxV;
    System::Windows::Forms::TrackBar^ trackBar_radius;
    System::Windows::Forms::Button^ btn_cancel;
    System::Windows::Forms::Button^ btn_finish;
    System::Windows::Forms::Button^ button_addforeseed;
    System::Windows::Forms::Button^ button_addbackseed;
    System::Windows::Forms::Button^ btn_runLocalRGrow;
    System::Windows::Forms::Button^ m_btn_loadseeds;
    System::Windows::Forms::Button^ m_btn_saveseeds;
    System::Windows::Forms::DataGridView^ maskList;
    System::Windows::Forms::DataGridViewTextBoxColumn^ seedColumn;
    System::Windows::Forms::DataGridViewTextBoxColumn^ fbColumn;
    System::ComponentModel::Container^ components;

    bool m_bListUpdating;
    bool m_trackbarscrolling;

    static FormSegLocalRGrow^ m_singleton;
  public:
    static FormSegLocalRGrow^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormSegLocalRGrow();
      return m_singleton;
    }
    
    void UpdateAllItems();
    void UpdateList();
    void UpdateSliders();
    void SetSliderRange(float maxRadius, short minV, short maxV);

  private:
		FormSegLocalRGrow(void)
		{
      InitializeComponent();
      m_bListUpdating = false;
      m_trackbarscrolling = false;
		}

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormSegLocalRGrow()
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

#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
      System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
      System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(FormSegLocalRGrow::typeid));
      this->maskList = (gcnew System::Windows::Forms::DataGridView());
      this->seedColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->fbColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->m_label1 = (gcnew System::Windows::Forms::Label());
      this->m_label3 = (gcnew System::Windows::Forms::Label());
      this->m_label4 = (gcnew System::Windows::Forms::Label());
      this->m_label5 = (gcnew System::Windows::Forms::Label());
      this->m_label6 = (gcnew System::Windows::Forms::Label());
      this->btn_cancel = (gcnew System::Windows::Forms::Button());
      this->btn_finish = (gcnew System::Windows::Forms::Button());
      this->btn_runLocalRGrow = (gcnew System::Windows::Forms::Button());
      this->textBox_minV = (gcnew System::Windows::Forms::TextBox());
      this->textBox_maxV = (gcnew System::Windows::Forms::TextBox());
      this->textBox_radius = (gcnew System::Windows::Forms::TextBox());
      this->trackBar_minV = (gcnew System::Windows::Forms::TrackBar());
      this->trackBar_maxV = (gcnew System::Windows::Forms::TrackBar());
      this->trackBar_radius = (gcnew System::Windows::Forms::TrackBar());
      this->button_addforeseed = (gcnew System::Windows::Forms::Button());
      this->button_addbackseed = (gcnew System::Windows::Forms::Button());
      this->m_btn_loadseeds = (gcnew System::Windows::Forms::Button());
      this->m_btn_saveseeds = (gcnew System::Windows::Forms::Button());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskList))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_minV))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_maxV))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_radius))->BeginInit();
      this->SuspendLayout();
      // 
      // maskList
      // 
      this->maskList->AllowUserToAddRows = false;
      this->maskList->AllowUserToDeleteRows = false;
      this->maskList->AllowUserToResizeColumns = false;
      this->maskList->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::AllCells;
      this->maskList->AutoSizeRowsMode = System::Windows::Forms::DataGridViewAutoSizeRowsMode::AllCells;
      this->maskList->BackgroundColor = System::Drawing::SystemColors::ButtonShadow;
      this->maskList->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
      this->maskList->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {
        this->seedColumn,
          this->fbColumn
      });
      this->maskList->Location = System::Drawing::Point(10, 10);
      this->maskList->MultiSelect = false;
      this->maskList->Name = L"maskList";
      this->maskList->ReadOnly = true;
      this->maskList->RowHeadersVisible = false;
      this->maskList->RowTemplate->Height = 21;
      this->maskList->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
      this->maskList->Size = System::Drawing::Size(109, 399);
      this->maskList->TabIndex = 26;
      this->maskList->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &FormSegLocalRGrow::maskList_CellContentClick);
      this->maskList->SelectionChanged += gcnew System::EventHandler(this, &FormSegLocalRGrow::maskList_SelectionChanged);
      // 
      // seedColumn
      // 
      this->seedColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      this->seedColumn->HeaderText = L"seed";
      this->seedColumn->Name = L"seedColumn";
      this->seedColumn->ReadOnly = true;
      this->seedColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
      this->seedColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
      // 
      // fbColumn
      // 
      this->fbColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      dataGridViewCellStyle2->BackColor = System::Drawing::Color::White;
      dataGridViewCellStyle2->ForeColor = System::Drawing::Color::Black;
      dataGridViewCellStyle2->SelectionBackColor = System::Drawing::Color::Transparent;
      dataGridViewCellStyle2->SelectionForeColor = System::Drawing::Color::Black;
      this->fbColumn->DefaultCellStyle = dataGridViewCellStyle2;
      this->fbColumn->FillWeight = 80;
      this->fbColumn->HeaderText = L"F/B";
      this->fbColumn->Name = L"fbColumn";
      this->fbColumn->ReadOnly = true;
      this->fbColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
      this->fbColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
      // 
      // m_label1
      // 
      this->m_label1->AutoSize = true;
      this->m_label1->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_label1->Location = System::Drawing::Point(125, 8);
      this->m_label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->m_label1->Name = L"m_label1";
      this->m_label1->Size = System::Drawing::Size(187, 144);
      this->m_label1->TabIndex = 28;
      this->m_label1->Text = resources->GetString(L"m_label1.Text");
      // 
      // m_label3
      // 
      this->m_label3->AutoSize = true;
      this->m_label3->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_label3->Location = System::Drawing::Point(125, 203);
      this->m_label3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->m_label3->Name = L"m_label3";
      this->m_label3->Size = System::Drawing::Size(30, 18);
      this->m_label3->TabIndex = 33;
      this->m_label3->Text = L"min";
      // 
      // m_label4
      // 
      this->m_label4->AutoSize = true;
      this->m_label4->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_label4->Location = System::Drawing::Point(124, 249);
      this->m_label4->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->m_label4->Name = L"m_label4";
      this->m_label4->Size = System::Drawing::Size(34, 18);
      this->m_label4->TabIndex = 34;
      this->m_label4->Text = L"max";
      // 
      // m_label5
      // 
      this->m_label5->AutoSize = true;
      this->m_label5->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_label5->Location = System::Drawing::Point(121, 296);
      this->m_label5->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->m_label5->Name = L"m_label5";
      this->m_label5->Size = System::Drawing::Size(43, 18);
      this->m_label5->TabIndex = 35;
      this->m_label5->Text = L"radius";
      // 
      // m_label6
      // 
      this->m_label6->AutoSize = true;
      this->m_label6->Enabled = false;
      this->m_label6->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_label6->Location = System::Drawing::Point(166, 155);
      this->m_label6->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->m_label6->Name = L"m_label6";
      this->m_label6->Size = System::Drawing::Size(110, 20);
      this->m_label6->TabIndex = 31;
      this->m_label6->Text = L"Add New Seed";
      // 
      // btn_cancel
      // 
      this->btn_cancel->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_cancel->Location = System::Drawing::Point(16, 413);
      this->btn_cancel->Margin = System::Windows::Forms::Padding(2);
      this->btn_cancel->Name = L"btn_cancel";
      this->btn_cancel->Size = System::Drawing::Size(94, 38);
      this->btn_cancel->TabIndex = 27;
      this->btn_cancel->Text = L"CANCEL";
      this->btn_cancel->UseVisualStyleBackColor = true;
      this->btn_cancel->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::btn_cancel_Click);
      // 
      // btn_finish
      // 
      this->btn_finish->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_finish->Location = System::Drawing::Point(114, 413);
      this->btn_finish->Margin = System::Windows::Forms::Padding(2);
      this->btn_finish->Name = L"btn_finish";
      this->btn_finish->Size = System::Drawing::Size(202, 38);
      this->btn_finish->TabIndex = 27;
      this->btn_finish->Text = L"FINISH and store region";
      this->btn_finish->UseVisualStyleBackColor = true;
      this->btn_finish->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::btn_finish_Click);
      // 
      // btn_runLocalRGrow
      // 
      this->btn_runLocalRGrow->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_runLocalRGrow->Location = System::Drawing::Point(127, 342);
      this->btn_runLocalRGrow->Margin = System::Windows::Forms::Padding(2);
      this->btn_runLocalRGrow->Name = L"btn_runLocalRGrow";
      this->btn_runLocalRGrow->Size = System::Drawing::Size(189, 30);
      this->btn_runLocalRGrow->TabIndex = 32;
      this->btn_runLocalRGrow->Text = L"Run Local Region Grow";
      this->btn_runLocalRGrow->UseVisualStyleBackColor = true;
      this->btn_runLocalRGrow->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::btn_runLocalRGrow_Click);
      // 
      // textBox_minV
      // 
      this->textBox_minV->Location = System::Drawing::Point(165, 203);
      this->textBox_minV->Margin = System::Windows::Forms::Padding(2);
      this->textBox_minV->Name = L"textBox_minV";
      this->textBox_minV->Size = System::Drawing::Size(76, 19);
      this->textBox_minV->TabIndex = 36;
      this->textBox_minV->TextChanged += gcnew System::EventHandler(this, &FormSegLocalRGrow::textBox_minV_TextChanged);
      // 
      // textBox_maxV
      // 
      this->textBox_maxV->Location = System::Drawing::Point(165, 249);
      this->textBox_maxV->Margin = System::Windows::Forms::Padding(2);
      this->textBox_maxV->Name = L"textBox_maxV";
      this->textBox_maxV->Size = System::Drawing::Size(76, 19);
      this->textBox_maxV->TabIndex = 36;
      this->textBox_maxV->TextChanged += gcnew System::EventHandler(this, &FormSegLocalRGrow::textBox_maxV_TextChanged);
      // 
      // textBox_radius
      // 
      this->textBox_radius->Location = System::Drawing::Point(164, 296);
      this->textBox_radius->Margin = System::Windows::Forms::Padding(2);
      this->textBox_radius->Name = L"textBox_radius";
      this->textBox_radius->Size = System::Drawing::Size(76, 19);
      this->textBox_radius->TabIndex = 36;
      this->textBox_radius->TextChanged += gcnew System::EventHandler(this, &FormSegLocalRGrow::textBox_radius_TextChanged);
      // 
      // trackBar_minV
      // 
      this->trackBar_minV->Location = System::Drawing::Point(127, 223);
      this->trackBar_minV->Margin = System::Windows::Forms::Padding(2);
      this->trackBar_minV->Name = L"trackBar_minV";
      this->trackBar_minV->Size = System::Drawing::Size(191, 45);
      this->trackBar_minV->TabIndex = 37;
      this->trackBar_minV->TickStyle = System::Windows::Forms::TickStyle::None;
      this->trackBar_minV->Scroll += gcnew System::EventHandler(this, &FormSegLocalRGrow::trackBar_minV_Scroll);
      // 
      // trackBar_maxV
      // 
      this->trackBar_maxV->Location = System::Drawing::Point(127, 268);
      this->trackBar_maxV->Margin = System::Windows::Forms::Padding(2);
      this->trackBar_maxV->Name = L"trackBar_maxV";
      this->trackBar_maxV->Size = System::Drawing::Size(191, 45);
      this->trackBar_maxV->TabIndex = 38;
      this->trackBar_maxV->TickStyle = System::Windows::Forms::TickStyle::None;
      this->trackBar_maxV->Scroll += gcnew System::EventHandler(this, &FormSegLocalRGrow::trackBar_maxV_Scroll);
      // 
      // trackBar_radius
      // 
      this->trackBar_radius->Location = System::Drawing::Point(128, 315);
      this->trackBar_radius->Margin = System::Windows::Forms::Padding(2);
      this->trackBar_radius->Name = L"trackBar_radius";
      this->trackBar_radius->Size = System::Drawing::Size(190, 45);
      this->trackBar_radius->TabIndex = 39;
      this->trackBar_radius->TickStyle = System::Windows::Forms::TickStyle::None;
      this->trackBar_radius->Scroll += gcnew System::EventHandler(this, &FormSegLocalRGrow::trackBar_radius_Scroll);
      // 
      // button_addforeseed
      // 
      this->button_addforeseed->Enabled = false;
      this->button_addforeseed->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->button_addforeseed->Location = System::Drawing::Point(131, 171);
      this->button_addforeseed->Margin = System::Windows::Forms::Padding(2);
      this->button_addforeseed->Name = L"button_addforeseed";
      this->button_addforeseed->Size = System::Drawing::Size(87, 24);
      this->button_addforeseed->TabIndex = 40;
      this->button_addforeseed->Text = L"Fore Seed";
      this->button_addforeseed->UseVisualStyleBackColor = true;
      this->button_addforeseed->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::button_addforeseed_Click);
      // 
      // button_addbackseed
      // 
      this->button_addbackseed->Enabled = false;
      this->button_addbackseed->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->button_addbackseed->Location = System::Drawing::Point(222, 171);
      this->button_addbackseed->Margin = System::Windows::Forms::Padding(2);
      this->button_addbackseed->Name = L"button_addbackseed";
      this->button_addbackseed->Size = System::Drawing::Size(87, 24);
      this->button_addbackseed->TabIndex = 41;
      this->button_addbackseed->Text = L"Back Seed";
      this->button_addbackseed->UseVisualStyleBackColor = true;
      this->button_addbackseed->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::button_addbackseed_Click);
      // 
      // m_btn_loadseeds
      // 
      this->m_btn_loadseeds->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_btn_loadseeds->Location = System::Drawing::Point(124, 376);
      this->m_btn_loadseeds->Margin = System::Windows::Forms::Padding(2);
      this->m_btn_loadseeds->Name = L"m_btn_loadseeds";
      this->m_btn_loadseeds->Size = System::Drawing::Size(94, 29);
      this->m_btn_loadseeds->TabIndex = 42;
      this->m_btn_loadseeds->Text = L"load seeds";
      this->m_btn_loadseeds->UseVisualStyleBackColor = true;
      this->m_btn_loadseeds->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::m_btn_loadseeds_Click);
      // 
      // m_btn_saveseeds
      // 
      this->m_btn_saveseeds->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_btn_saveseeds->Location = System::Drawing::Point(222, 377);
      this->m_btn_saveseeds->Margin = System::Windows::Forms::Padding(2);
      this->m_btn_saveseeds->Name = L"m_btn_saveseeds";
      this->m_btn_saveseeds->Size = System::Drawing::Size(94, 29);
      this->m_btn_saveseeds->TabIndex = 43;
      this->m_btn_saveseeds->Text = L"save seeds";
      this->m_btn_saveseeds->UseVisualStyleBackColor = true;
      this->m_btn_saveseeds->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::m_btn_saveseeds_Click);
      // 
      // FormSegLocalRGrow
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(334, 458);
      this->Controls->Add(this->m_btn_saveseeds);
      this->Controls->Add(this->m_btn_loadseeds);
      this->Controls->Add(this->btn_runLocalRGrow);
      this->Controls->Add(this->button_addbackseed);
      this->Controls->Add(this->button_addforeseed);
      this->Controls->Add(this->m_label6);
      this->Controls->Add(this->trackBar_radius);
      this->Controls->Add(this->textBox_radius);
      this->Controls->Add(this->textBox_maxV);
      this->Controls->Add(this->textBox_minV);
      this->Controls->Add(this->m_label5);
      this->Controls->Add(this->m_label4);
      this->Controls->Add(this->m_label3);
      this->Controls->Add(this->m_label1);
      this->Controls->Add(this->btn_finish);
      this->Controls->Add(this->btn_cancel);
      this->Controls->Add(this->maskList);
      this->Controls->Add(this->trackBar_minV);
      this->Controls->Add(this->trackBar_maxV);
      this->Margin = System::Windows::Forms::Padding(2);
      this->Name = L"FormSegLocalRGrow";
      this->Text = L"FormSegLocalRGrow";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskList))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_minV))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_maxV))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_radius))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private:
    System::Void maskList_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) ;
    System::Void maskList_SelectionChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void trackBar_minV_Scroll(System::Object^  sender, System::EventArgs^  e) ;
    System::Void trackBar_maxV_Scroll(System::Object^  sender, System::EventArgs^  e) ;
    System::Void trackBar_radius_Scroll(System::Object^  sender, System::EventArgs^  e);
    System::Void btn_runLocalRGrow_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void btn_cancel_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void btn_finish_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void button_addforeseed_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void button_addbackseed_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void textBox_minV_TextChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void textBox_maxV_TextChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void textBox_radius_TextChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_btn_loadseeds_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_btn_saveseeds_Click(System::Object^  sender, System::EventArgs^  e);
};

  inline void formSegLocalRGrow_Show(){ FormSegLocalRGrow::getInst()->Show();}
  inline void formSegLocalRGrow_Hide(){ FormSegLocalRGrow::getInst()->Hide();}
 
  inline void formSegLocalRGrow_updateAllItems  (){
    FormSegLocalRGrow::getInst()->UpdateAllItems();
  } 
  inline void formSegLocalRGrow_setSliderRange(float maxRadius, short minV, short maxV){
    FormSegLocalRGrow::getInst()->SetSliderRange(maxRadius, minV, maxV);
  }
  inline void formSegLocalRGrow_updateSliders(){
    FormSegLocalRGrow::getInst()->UpdateSliders();
  }
}