#pragma once

namespace RoiPainter4D {

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
    
    // dialog itemを更新する際に trueになるフラグ
    // textbox->Value = newtext　などでtextchangedイベントが呼ばれてしまうことなどがあるのでこれを回避するために利用する
    bool m_item_updating; 

  private: 
    System::Windows::Forms::DataGridView^ m_list_seeds;
    System::Windows::Forms::CheckBox^ m_checkbox_allframesamepos;
    System::Windows::Forms::CheckBox^ m_checkbox_allframesamerad;
    System::Windows::Forms::CheckBox^ m_checkbox_allframesamethresh;
    System::Windows::Forms::TrackBar^ m_bar_radius;
    System::Windows::Forms::TrackBar^ m_bar_threshmax;
    System::Windows::Forms::TrackBar^ m_bar_threshmin;
    System::Windows::Forms::TextBox^ m_text_radius;
    System::Windows::Forms::TextBox^ m_text_threshmin;
    System::Windows::Forms::TextBox^ m_text_threshmax;
    System::Windows::Forms::Label^ m_label_radius;
    System::Windows::Forms::Label^ m_label_threshmin;
    System::Windows::Forms::Label^ m_label_threshmax;
    System::Windows::Forms::Label^ label2;
    System::Windows::Forms::Label^ label1;
    System::Windows::Forms::Label^ label3;
    System::Windows::Forms::Label^ label5;
    System::Windows::Forms::Button^ m_btn_loadseedinfo;
    System::Windows::Forms::Button^ m_btn_runlocalregiongrow_allframes;
    System::Windows::Forms::Button^ m_btn_cancel;
    System::Windows::Forms::Button^ m_btn_finish;
    System::Windows::Forms::Button^ m_btn_runlocalregiongrow_onlyone;
    System::Windows::Forms::GroupBox^ m_groupbox_runrgrow;
    System::Windows::Forms::Button^ m_btn_saveseedinfo;
    System::Windows::Forms::DataGridViewTextBoxColumn^ radius;
    System::Windows::Forms::DataGridViewTextBoxColumn^ editframe;

    static FormSegLocalRGrow^ m_singleton;

    FormSegLocalRGrow(void)
    {
      InitializeComponent();
      m_item_updating    = false;
			this->ControlBox  = false;
			this->MaximizeBox = false;
    }

  public:
    static FormSegLocalRGrow^ getInst() {
      if (m_singleton == nullptr) m_singleton = gcnew FormSegLocalRGrow();
      return m_singleton;
    }

    void InitAllItems(float max_radius, int volmin, int volmax);
    void UpdateSeedList();
    void UpdateSeedInfoItems();

    void TextboxRadiusChangedLeave(bool b_Leave);

    void SetChecboxState( const bool usesamerad, const bool usesamepos, const bool usesamethresh ){
      m_checkbox_allframesamerad->Checked    = usesamerad;
      m_checkbox_allframesamepos->Checked    = usesamepos;
      m_checkbox_allframesamethresh->Checked = usesamethresh;
    }

    //getter for flgs 
    bool IsSameRadiusAllFrame  (){ return m_checkbox_allframesamerad   ->Checked; }
    bool IsSameThreshAllFrame  (){ return m_checkbox_allframesamethresh->Checked; }
    bool IsSamePositionAllFrame(){ return m_checkbox_allframesamepos   ->Checked; }

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
    System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
    /// <summary>
    /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
    /// コード エディターで変更しないでください。
    /// </summary>
    void InitializeComponent(void)
    {
      this->m_list_seeds = (gcnew System::Windows::Forms::DataGridView());
      this->radius = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->editframe = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->m_btn_runlocalregiongrow_allframes = (gcnew System::Windows::Forms::Button());
      this->m_btn_cancel = (gcnew System::Windows::Forms::Button());
      this->m_btn_finish = (gcnew System::Windows::Forms::Button());
      this->m_btn_runlocalregiongrow_onlyone = (gcnew System::Windows::Forms::Button());
      this->m_btn_loadseedinfo = (gcnew System::Windows::Forms::Button());
      this->m_groupbox_runrgrow = (gcnew System::Windows::Forms::GroupBox());
      this->m_bar_threshmax = (gcnew System::Windows::Forms::TrackBar());
      this->m_bar_threshmin = (gcnew System::Windows::Forms::TrackBar());
      this->m_text_threshmin = (gcnew System::Windows::Forms::TextBox());
      this->m_text_threshmax = (gcnew System::Windows::Forms::TextBox());
      this->m_checkbox_allframesamethresh = (gcnew System::Windows::Forms::CheckBox());
      this->m_bar_radius = (gcnew System::Windows::Forms::TrackBar());
      this->m_checkbox_allframesamerad = (gcnew System::Windows::Forms::CheckBox());
      this->m_text_radius = (gcnew System::Windows::Forms::TextBox());
      this->m_label_radius = (gcnew System::Windows::Forms::Label());
      this->m_label_threshmin = (gcnew System::Windows::Forms::Label());
      this->m_label_threshmax = (gcnew System::Windows::Forms::Label());
      this->m_checkbox_allframesamepos = (gcnew System::Windows::Forms::CheckBox());
      this->m_btn_saveseedinfo = (gcnew System::Windows::Forms::Button());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->label5 = (gcnew System::Windows::Forms::Label());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_list_seeds))->BeginInit();
      this->m_groupbox_runrgrow->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_bar_threshmax))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_bar_threshmin))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_bar_radius))->BeginInit();
      this->SuspendLayout();
      // 
      // m_list_seeds
      // 
      this->m_list_seeds->AllowUserToAddRows = false;
      this->m_list_seeds->AllowUserToDeleteRows = false;
      this->m_list_seeds->AllowUserToResizeColumns = false;
      this->m_list_seeds->AllowUserToResizeRows = false;
      this->m_list_seeds->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::AllCells;
      this->m_list_seeds->AutoSizeRowsMode = System::Windows::Forms::DataGridViewAutoSizeRowsMode::AllCells;
      this->m_list_seeds->BackgroundColor = System::Drawing::SystemColors::ButtonShadow;
      this->m_list_seeds->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
      this->m_list_seeds->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {
        this->radius,
          this->editframe
      });
      this->m_list_seeds->Location = System::Drawing::Point(4, 67);
      this->m_list_seeds->MultiSelect = false;
      this->m_list_seeds->Name = L"m_list_seeds";
      this->m_list_seeds->ReadOnly = true;
      this->m_list_seeds->RowHeadersVisible = false;
      this->m_list_seeds->RowTemplate->Height = 21;
      this->m_list_seeds->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
      this->m_list_seeds->Size = System::Drawing::Size(141, 204);
      this->m_list_seeds->TabIndex = 0;
      this->m_list_seeds->SelectionChanged += gcnew System::EventHandler(this, &FormSegLocalRGrow::SeedListSelectionChanged);
      // 
      // radius
      // 
      this->radius->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::None;
      this->radius->Frozen = true;
      this->radius->HeaderText = L"rad";
      this->radius->Name = L"radius";
      this->radius->ReadOnly = true;
      this->radius->Resizable = System::Windows::Forms::DataGridViewTriState::False;
      this->radius->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
      this->radius->Width = 60;
      // 
      // editframe
      // 
      this->editframe->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::None;
      this->editframe->Frozen = true;
      this->editframe->HeaderText = L"edit frame";
      this->editframe->Name = L"editframe";
      this->editframe->ReadOnly = true;
      this->editframe->Width = 200;
      // 
      // m_btn_runlocalregiongrow_allframes
      // 
      this->m_btn_runlocalregiongrow_allframes->Location = System::Drawing::Point(144, 19);
      this->m_btn_runlocalregiongrow_allframes->Name = L"m_btn_runlocalregiongrow_allframes";
      this->m_btn_runlocalregiongrow_allframes->Size = System::Drawing::Size(130, 35);
      this->m_btn_runlocalregiongrow_allframes->TabIndex = 2;
      this->m_btn_runlocalregiongrow_allframes->Text = L"all frame";
      this->m_btn_runlocalregiongrow_allframes->UseVisualStyleBackColor = true;
      this->m_btn_runlocalregiongrow_allframes->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::BtnRunLocalRGrowAllFrameClick);
      // 
      // m_btn_cancel
      // 
      this->m_btn_cancel->Location = System::Drawing::Point(199, 353);
      this->m_btn_cancel->Name = L"m_btn_cancel";
      this->m_btn_cancel->Size = System::Drawing::Size(82, 33);
      this->m_btn_cancel->TabIndex = 3;
      this->m_btn_cancel->Text = L"cancel";
      this->m_btn_cancel->UseVisualStyleBackColor = true;
      this->m_btn_cancel->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::BtnCancelClick);
      // 
      // m_btn_finish
      // 
      this->m_btn_finish->Location = System::Drawing::Point(17, 353);
      this->m_btn_finish->Name = L"m_btn_finish";
      this->m_btn_finish->Size = System::Drawing::Size(178, 33);
      this->m_btn_finish->TabIndex = 4;
      this->m_btn_finish->Text = L"Finish And Store";
      this->m_btn_finish->UseVisualStyleBackColor = true;
      this->m_btn_finish->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::BtnFinishClick);
      // 
      // m_btn_runlocalregiongrow_onlyone
      // 
      this->m_btn_runlocalregiongrow_onlyone->Location = System::Drawing::Point(10, 19);
      this->m_btn_runlocalregiongrow_onlyone->Name = L"m_btn_runlocalregiongrow_onlyone";
      this->m_btn_runlocalregiongrow_onlyone->Size = System::Drawing::Size(128, 35);
      this->m_btn_runlocalregiongrow_onlyone->TabIndex = 0;
      this->m_btn_runlocalregiongrow_onlyone->Text = L"current frame";
      this->m_btn_runlocalregiongrow_onlyone->UseVisualStyleBackColor = true;
      this->m_btn_runlocalregiongrow_onlyone->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::BtnRunLocalRGrowCurrentFrameClick);
      // 
      // m_btn_loadseedinfo
      // 
      this->m_btn_loadseedinfo->Location = System::Drawing::Point(209, 254);
      this->m_btn_loadseedinfo->Name = L"m_btn_loadseedinfo";
      this->m_btn_loadseedinfo->Size = System::Drawing::Size(40, 23);
      this->m_btn_loadseedinfo->TabIndex = 17;
      this->m_btn_loadseedinfo->Text = L"Load";
      this->m_btn_loadseedinfo->UseVisualStyleBackColor = true;
      this->m_btn_loadseedinfo->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::BtnLoadSeedInfoClick);
      // 
      // m_groupbox_runrgrow
      // 
      this->m_groupbox_runrgrow->Controls->Add(this->m_btn_runlocalregiongrow_onlyone);
      this->m_groupbox_runrgrow->Controls->Add(this->m_btn_runlocalregiongrow_allframes);
      this->m_groupbox_runrgrow->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic)),
        System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(128)));
      this->m_groupbox_runrgrow->Location = System::Drawing::Point(7, 286);
      this->m_groupbox_runrgrow->Name = L"m_groupbox_runrgrow";
      this->m_groupbox_runrgrow->Size = System::Drawing::Size(280, 61);
      this->m_groupbox_runrgrow->TabIndex = 5;
      this->m_groupbox_runrgrow->TabStop = false;
      this->m_groupbox_runrgrow->Text = L"Run Local Region Grow";
      // 
      // m_bar_threshmax
      // 
      this->m_bar_threshmax->BackColor = System::Drawing::SystemColors::Control;
      this->m_bar_threshmax->Location = System::Drawing::Point(150, 164);
      this->m_bar_threshmax->Name = L"m_bar_threshmax";
      this->m_bar_threshmax->Size = System::Drawing::Size(152, 45);
      this->m_bar_threshmax->TabIndex = 0;
      this->m_bar_threshmax->TickStyle = System::Windows::Forms::TickStyle::None;
      this->m_bar_threshmax->Scroll += gcnew System::EventHandler(this, &FormSegLocalRGrow::BarThreshMaxScroll);
      // 
      // m_bar_threshmin
      // 
      this->m_bar_threshmin->BackColor = System::Drawing::SystemColors::Control;
      this->m_bar_threshmin->Location = System::Drawing::Point(150, 124);
      this->m_bar_threshmin->Name = L"m_bar_threshmin";
      this->m_bar_threshmin->Size = System::Drawing::Size(152, 45);
      this->m_bar_threshmin->TabIndex = 1;
      this->m_bar_threshmin->TickStyle = System::Windows::Forms::TickStyle::None;
      this->m_bar_threshmin->Scroll += gcnew System::EventHandler(this, &FormSegLocalRGrow::BarThreshMinScroll);
      // 
      // m_text_threshmin
      // 
      this->m_text_threshmin->Location = System::Drawing::Point(221, 104);
      this->m_text_threshmin->Name = L"m_text_threshmin";
      this->m_text_threshmin->Size = System::Drawing::Size(42, 19);
      this->m_text_threshmin->TabIndex = 6;
      this->m_text_threshmin->TextChanged += gcnew System::EventHandler(this, &FormSegLocalRGrow::textboxThreshMinChanged);
      // 
      // m_text_threshmax
      // 
      this->m_text_threshmax->Location = System::Drawing::Point(221, 145);
      this->m_text_threshmax->Name = L"m_text_threshmax";
      this->m_text_threshmax->Size = System::Drawing::Size(42, 19);
      this->m_text_threshmax->TabIndex = 7;
      this->m_text_threshmax->TextChanged += gcnew System::EventHandler(this, &FormSegLocalRGrow::textboxThreshMaxChanged);
      // 
      // m_checkbox_allframesamethresh
      // 
      this->m_checkbox_allframesamethresh->AutoSize = true;
      this->m_checkbox_allframesamethresh->Checked = true;
      this->m_checkbox_allframesamethresh->CheckState = System::Windows::Forms::CheckState::Checked;
      this->m_checkbox_allframesamethresh->Font = (gcnew System::Drawing::Font(L"Calibri", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->m_checkbox_allframesamethresh->Location = System::Drawing::Point(151, 211);
      this->m_checkbox_allframesamethresh->Name = L"m_checkbox_allframesamethresh";
      this->m_checkbox_allframesamethresh->Size = System::Drawing::Size(151, 18);
      this->m_checkbox_allframesamethresh->TabIndex = 10;
      this->m_checkbox_allframesamethresh->Text = L"Same Thresh for All Frms";
      this->m_checkbox_allframesamethresh->UseVisualStyleBackColor = true;
      // 
      // m_bar_radius
      // 
      this->m_bar_radius->BackColor = System::Drawing::SystemColors::Control;
      this->m_bar_radius->Location = System::Drawing::Point(150, 79);
      this->m_bar_radius->Name = L"m_bar_radius";
      this->m_bar_radius->Size = System::Drawing::Size(153, 45);
      this->m_bar_radius->TabIndex = 2;
      this->m_bar_radius->TickStyle = System::Windows::Forms::TickStyle::None;
      this->m_bar_radius->Scroll += gcnew System::EventHandler(this, &FormSegLocalRGrow::BarRadiusScroll);
      // 
      // m_checkbox_allframesamerad
      // 
      this->m_checkbox_allframesamerad->AutoSize = true;
      this->m_checkbox_allframesamerad->Checked = true;
      this->m_checkbox_allframesamerad->CheckState = System::Windows::Forms::CheckState::Checked;
      this->m_checkbox_allframesamerad->Font = (gcnew System::Drawing::Font(L"Calibri", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->m_checkbox_allframesamerad->Location = System::Drawing::Point(151, 192);
      this->m_checkbox_allframesamerad->Name = L"m_checkbox_allframesamerad";
      this->m_checkbox_allframesamerad->Size = System::Drawing::Size(151, 18);
      this->m_checkbox_allframesamerad->TabIndex = 12;
      this->m_checkbox_allframesamerad->Text = L"Same Radius for All Frms";
      this->m_checkbox_allframesamerad->UseVisualStyleBackColor = true;
      // 
      // m_text_radius
      // 
      this->m_text_radius->Location = System::Drawing::Point(190, 60);
      this->m_text_radius->Name = L"m_text_radius";
      this->m_text_radius->Size = System::Drawing::Size(42, 19);
      this->m_text_radius->TabIndex = 8;
      this->m_text_radius->TextChanged += gcnew System::EventHandler(this, &FormSegLocalRGrow::textboxRadiusChanged);
      this->m_text_radius->Leave += gcnew System::EventHandler(this, &FormSegLocalRGrow::textboxRadiusLeave);
      // 
      // m_label_radius
      // 
      this->m_label_radius->AutoSize = true;
      this->m_label_radius->Location = System::Drawing::Point(150, 63);
      this->m_label_radius->Name = L"m_label_radius";
      this->m_label_radius->Size = System::Drawing::Size(36, 12);
      this->m_label_radius->TabIndex = 11;
      this->m_label_radius->Text = L"radius";
      // 
      // m_label_threshmin
      // 
      this->m_label_threshmin->AutoSize = true;
      this->m_label_threshmin->Location = System::Drawing::Point(150, 107);
      this->m_label_threshmin->Name = L"m_label_threshmin";
      this->m_label_threshmin->Size = System::Drawing::Size(64, 12);
      this->m_label_threshmin->TabIndex = 18;
      this->m_label_threshmin->Text = L"Thresh MIN";
      // 
      // m_label_threshmax
      // 
      this->m_label_threshmax->AutoSize = true;
      this->m_label_threshmax->Location = System::Drawing::Point(150, 149);
      this->m_label_threshmax->Name = L"m_label_threshmax";
      this->m_label_threshmax->Size = System::Drawing::Size(68, 12);
      this->m_label_threshmax->TabIndex = 19;
      this->m_label_threshmax->Text = L"Thresh MAX";
      // 
      // m_checkbox_allframesamepos
      // 
      this->m_checkbox_allframesamepos->AutoSize = true;
      this->m_checkbox_allframesamepos->Font = (gcnew System::Drawing::Font(L"Calibri", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->m_checkbox_allframesamepos->Location = System::Drawing::Point(151, 230);
      this->m_checkbox_allframesamepos->Name = L"m_checkbox_allframesamepos";
      this->m_checkbox_allframesamepos->Size = System::Drawing::Size(157, 18);
      this->m_checkbox_allframesamepos->TabIndex = 20;
      this->m_checkbox_allframesamepos->Text = L"Same Position for All Frms";
      this->m_checkbox_allframesamepos->UseVisualStyleBackColor = true;
      // 
      // m_btn_saveseedinfo
      // 
      this->m_btn_saveseedinfo->Location = System::Drawing::Point(255, 254);
      this->m_btn_saveseedinfo->Name = L"m_btn_saveseedinfo";
      this->m_btn_saveseedinfo->Size = System::Drawing::Size(40, 23);
      this->m_btn_saveseedinfo->TabIndex = 21;
      this->m_btn_saveseedinfo->Text = L"Save";
      this->m_btn_saveseedinfo->UseVisualStyleBackColor = true;
      this->m_btn_saveseedinfo->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::BtnSaveSeedInfoClick);
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label2->Location = System::Drawing::Point(4, 3);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(299, 17);
      this->label2->TabIndex = 22;
      this->label2->Text = L"Shift + L-Dblclick : ADD/Delete control point";
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label1->Location = System::Drawing::Point(4, 20);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(282, 17);
      this->label1->TabIndex = 23;
      this->label1->Text = L"Shift + L-drag : move point/change radius";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label3->Location = System::Drawing::Point(4, 37);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(167, 17);
      this->label3->TabIndex = 23;
      this->label3->Text = L"s key : hide control point";
      // 
      // label5
      // 
      this->label5->AutoSize = true;
      this->label5->Location = System::Drawing::Point(151, 259);
      this->label5->Name = L"label5";
      this->label5->Size = System::Drawing::Size(53, 12);
      this->label5->TabIndex = 19;
      this->label5->Text = L"Seed Info";
      // 
      // FormSegLocalRGrow
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(307, 394);
      this->Controls->Add(this->label3);
      this->Controls->Add(this->label1);
      this->Controls->Add(this->label2);
      this->Controls->Add(this->m_btn_saveseedinfo);
      this->Controls->Add(this->m_checkbox_allframesamepos);
      this->Controls->Add(this->m_checkbox_allframesamethresh);
      this->Controls->Add(this->m_checkbox_allframesamerad);
      this->Controls->Add(this->m_bar_threshmax);
      this->Controls->Add(this->label5);
      this->Controls->Add(this->m_label_threshmax);
      this->Controls->Add(this->m_text_threshmin);
      this->Controls->Add(this->m_label_threshmin);
      this->Controls->Add(this->m_text_threshmax);
      this->Controls->Add(this->m_label_radius);
      this->Controls->Add(this->m_text_radius);
      this->Controls->Add(this->m_btn_loadseedinfo);
      this->Controls->Add(this->m_bar_radius);
      this->Controls->Add(this->m_groupbox_runrgrow);
      this->Controls->Add(this->m_btn_finish);
      this->Controls->Add(this->m_btn_cancel);
      this->Controls->Add(this->m_list_seeds);
      this->Controls->Add(this->m_bar_threshmin);
      this->Name = L"FormSegLocalRGrow";
      this->Text = L"FormSegLocalRGrow";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_list_seeds))->EndInit();
      this->m_groupbox_runrgrow->ResumeLayout(false);
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_bar_threshmax))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_bar_threshmin))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_bar_radius))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }


#pragma endregion
  private: System::Void BarRadiusScroll   (System::Object^  sender, System::EventArgs^  e);
  private: System::Void BarThreshMinScroll(System::Object^  sender, System::EventArgs^  e);
  private: System::Void BarThreshMaxScroll(System::Object^  sender, System::EventArgs^  e);
  private: System::Void textboxRadiusChanged   (System::Object^  sender, System::EventArgs^  e);
  private: System::Void textboxThreshMaxChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void textboxThreshMinChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void BtnCancelClick      (System::Object^  sender, System::EventArgs^  e);
  private: System::Void BtnFinishClick      (System::Object^  sender, System::EventArgs^  e);
  private: System::Void BtnLoadSeedInfoClick(System::Object^  sender, System::EventArgs^  e);
  private: System::Void BtnSaveSeedInfoClick(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void BtnRunLocalRGrowAllFrameClick    (System::Object^  sender, System::EventArgs^  e);
  private: System::Void BtnRunLocalRGrowCurrentFrameClick(System::Object^  sender, System::EventArgs^  e);
  private: System::Void CheckBoxMeshDrawChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void SeedListSelectionChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void textboxRadiusLeave(System::Object^ sender, System::EventArgs^ e) ;

};

  inline void  FormSegLocalRGrow_Show() { FormSegLocalRGrow::getInst()->Show(); }
  inline void  FormSegLocalRGrow_Hide() { FormSegLocalRGrow::getInst()->Hide(); }
  inline void  FormSegLocalRGrow_InitAllItems(float max_radius, int volmin, int volmax)
  { 
    FormSegLocalRGrow::getInst()->InitAllItems(max_radius, volmin, volmax);
  }
  inline bool  FormSegLocalRGrow_DoSetSamePosForAllFrame   () { return FormSegLocalRGrow::getInst()->IsSamePositionAllFrame(); }
  inline bool  FormSegLocalRGrow_DoSetSameThreshForAllFrame() { return FormSegLocalRGrow::getInst()->IsSameThreshAllFrame  (); }
  inline bool  FormSegLocalRGrow_DoSetSameRadForAllFrame   () { return FormSegLocalRGrow::getInst()->IsSameRadiusAllFrame  (); }
  inline void  FormSegLocalRGrow_UpdateSeedSelection() { 
    FormSegLocalRGrow::getInst()->UpdateSeedInfoItems(); 
    FormSegLocalRGrow::getInst()->UpdateSeedList(); 
  }
  inline void  FormSegLocalRGrow_FrameIdxChanged() { 
    FormSegLocalRGrow::getInst()->UpdateSeedInfoItems(); 
    FormSegLocalRGrow::getInst()->UpdateSeedList(); 
  }



  inline void ForlSegLocalRGrow_SetChecboxState(const bool usesamerad, const bool usesamepos, const bool usesamethresh){
    FormSegLocalRGrow::getInst()->SetChecboxState(usesamerad, usesamepos, usesamethresh);
  }

}
