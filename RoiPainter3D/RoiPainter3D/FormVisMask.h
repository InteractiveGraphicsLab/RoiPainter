#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormVisMask の概要
	/// </summary>
	public ref class FormVisMask : public System::Windows::Forms::Form
	{


  private:
    static FormVisMask^ m_singleton;
    bool                m_bListUpdating;
    System::Windows::Forms::Button^  btnExpObj;
    System::Windows::Forms::Button^  btnErode;
    System::Windows::Forms::Button^  btnDilate;
    System::Windows::Forms::Button^  btnFillHole;
    System::Windows::Forms::Button^  btnMargeTo;
    System::Windows::Forms::Button^  btnDelete;

    System::Windows::Forms::Button^  btnColorPallet;
    System::Windows::Forms::CheckBox^  checkbox_lock;
    System::Windows::Forms::DataGridView^  maskList;
    System::Windows::Forms::DataGridViewTextBoxColumn^  regionIDColumn;
    System::Windows::Forms::DataGridViewTextBoxColumn^  colorColumn;
    System::Windows::Forms::TrackBar^  trackbar_alpha;
    System::Windows::Forms::GroupBox^  groupBox1;
    System::Windows::Forms::Button^  btnSmartFillHole;
    System::Windows::Forms::TextBox^  alpha;

    FormVisMask(void)
		{
			InitializeComponent();

      this->ControlBox  = false;
      this->MaximizeBox = false;
      m_bListUpdating   = false;

      trackbar_alpha->SetRange(0, 100);
		}

  public:


    static FormVisMask^ getInst() 
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormVisMask();
      return m_singleton;
    }

    void updateList();

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormVisMask()
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
      System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
      this->btnMargeTo = (gcnew System::Windows::Forms::Button());
      this->btnDelete = (gcnew System::Windows::Forms::Button());
      this->btnColorPallet = (gcnew System::Windows::Forms::Button());
      this->checkbox_lock = (gcnew System::Windows::Forms::CheckBox());
      this->maskList = (gcnew System::Windows::Forms::DataGridView());
      this->regionIDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->colorColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->trackbar_alpha = (gcnew System::Windows::Forms::TrackBar());
      this->alpha = (gcnew System::Windows::Forms::TextBox());
      this->btnExpObj = (gcnew System::Windows::Forms::Button());
      this->btnErode = (gcnew System::Windows::Forms::Button());
      this->btnDilate = (gcnew System::Windows::Forms::Button());
      this->btnFillHole = (gcnew System::Windows::Forms::Button());
      this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
      this->btnSmartFillHole = (gcnew System::Windows::Forms::Button());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskList))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_alpha))->BeginInit();
      this->groupBox1->SuspendLayout();
      this->SuspendLayout();
      // 
      // btnMargeTo
      // 
      this->btnMargeTo->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btnMargeTo->Location = System::Drawing::Point(143, 240);
      this->btnMargeTo->Name = L"btnMargeTo";
      this->btnMargeTo->Size = System::Drawing::Size(137, 25);
      this->btnMargeTo->TabIndex = 34;
      this->btnMargeTo->Text = L"MARGE ids";
      this->btnMargeTo->UseVisualStyleBackColor = true;
      this->btnMargeTo->Click += gcnew System::EventHandler(this, &FormVisMask::btnMargeTo_Click);
      // 
      // btnDelete
      // 
      this->btnDelete->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btnDelete->Location = System::Drawing::Point(6, 178);
      this->btnDelete->Name = L"btnDelete";
      this->btnDelete->Size = System::Drawing::Size(114, 23);
      this->btnDelete->TabIndex = 33;
      this->btnDelete->Text = L"! DELETE !";
      this->btnDelete->UseVisualStyleBackColor = true;
      this->btnDelete->Click += gcnew System::EventHandler(this, &FormVisMask::btnDelete_Click);
      // 
      // btnColorPallet
      // 
      this->btnColorPallet->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btnColorPallet->Location = System::Drawing::Point(65, 17);
      this->btnColorPallet->Name = L"btnColorPallet";
      this->btnColorPallet->Size = System::Drawing::Size(75, 23);
      this->btnColorPallet->TabIndex = 29;
      this->btnColorPallet->Text = L"color";
      this->btnColorPallet->UseVisualStyleBackColor = true;
      this->btnColorPallet->Click += gcnew System::EventHandler(this, &FormVisMask::btnColorPallet_Click);
      // 
      // checkbox_lock
      // 
      this->checkbox_lock->AutoSize = true;
      this->checkbox_lock->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->checkbox_lock->Location = System::Drawing::Point(6, 18);
      this->checkbox_lock->Name = L"checkbox_lock";
      this->checkbox_lock->Size = System::Drawing::Size(57, 22);
      this->checkbox_lock->TabIndex = 26;
      this->checkbox_lock->Text = L"Lock";
      this->checkbox_lock->UseVisualStyleBackColor = true;
      this->checkbox_lock->CheckedChanged += gcnew System::EventHandler(this, &FormVisMask::checkbox_lock_CheckedChanged);
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
        this->regionIDColumn,
          this->colorColumn
      });
      this->maskList->Location = System::Drawing::Point(4, 6);
      this->maskList->MultiSelect = false;
      this->maskList->Name = L"maskList";
      this->maskList->ReadOnly = true;
      this->maskList->RowHeadersVisible = false;
      this->maskList->RowTemplate->Height = 21;
      this->maskList->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
      this->maskList->Size = System::Drawing::Size(128, 392);
      this->maskList->TabIndex = 25;
      this->maskList->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &FormVisMask::maskList_CellContentClick);
      this->maskList->SelectionChanged += gcnew System::EventHandler(this, &FormVisMask::maskList_SelectionChanged);
      // 
      // regionIDColumn
      // 
      this->regionIDColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      this->regionIDColumn->HeaderText = L"region ID";
      this->regionIDColumn->Name = L"regionIDColumn";
      this->regionIDColumn->ReadOnly = true;
      this->regionIDColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
      this->regionIDColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
      // 
      // colorColumn
      // 
      this->colorColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      dataGridViewCellStyle2->BackColor = System::Drawing::Color::White;
      dataGridViewCellStyle2->ForeColor = System::Drawing::Color::Black;
      dataGridViewCellStyle2->SelectionBackColor = System::Drawing::Color::Transparent;
      dataGridViewCellStyle2->SelectionForeColor = System::Drawing::Color::Black;
      this->colorColumn->DefaultCellStyle = dataGridViewCellStyle2;
      this->colorColumn->FillWeight = 80;
      this->colorColumn->HeaderText = L"color";
      this->colorColumn->Name = L"colorColumn";
      this->colorColumn->ReadOnly = true;
      this->colorColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
      this->colorColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
      // 
      // trackbar_alpha
      // 
      this->trackbar_alpha->Location = System::Drawing::Point(1, 64);
      this->trackbar_alpha->Name = L"trackbar_alpha";
      this->trackbar_alpha->Size = System::Drawing::Size(143, 45);
      this->trackbar_alpha->TabIndex = 27;
      this->trackbar_alpha->TickStyle = System::Windows::Forms::TickStyle::None;
      this->trackbar_alpha->Scroll += gcnew System::EventHandler(this, &FormVisMask::trackbar_alpha_Scroll);
      // 
      // alpha
      // 
      this->alpha->BackColor = System::Drawing::SystemColors::Menu;
      this->alpha->BorderStyle = System::Windows::Forms::BorderStyle::None;
      this->alpha->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->alpha->Location = System::Drawing::Point(10, 48);
      this->alpha->Name = L"alpha";
      this->alpha->ReadOnly = true;
      this->alpha->Size = System::Drawing::Size(44, 18);
      this->alpha->TabIndex = 28;
      this->alpha->Text = L"alpha";
      // 
      // btnExpObj
      // 
      this->btnExpObj->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btnExpObj->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
      this->btnExpObj->Location = System::Drawing::Point(6, 142);
      this->btnExpObj->Name = L"btnExpObj";
      this->btnExpObj->Size = System::Drawing::Size(115, 25);
      this->btnExpObj->TabIndex = 35;
      this->btnExpObj->Text = L"Export as obj";
      this->btnExpObj->UseVisualStyleBackColor = true;
      this->btnExpObj->Click += gcnew System::EventHandler(this, &FormVisMask::btnExpObj_Click);
      // 
      // btnErode
      // 
      this->btnErode->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btnErode->Location = System::Drawing::Point(6, 89);
      this->btnErode->Name = L"btnErode";
      this->btnErode->Size = System::Drawing::Size(114, 25);
      this->btnErode->TabIndex = 36;
      this->btnErode->Text = L"Erode ";
      this->btnErode->UseVisualStyleBackColor = true;
      this->btnErode->Click += gcnew System::EventHandler(this, &FormVisMask::btnErode_Click);
      // 
      // btnDilate
      // 
      this->btnDilate->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btnDilate->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
      this->btnDilate->Location = System::Drawing::Point(6, 115);
      this->btnDilate->Name = L"btnDilate";
      this->btnDilate->Size = System::Drawing::Size(114, 25);
      this->btnDilate->TabIndex = 37;
      this->btnDilate->Text = L"Dilate ";
      this->btnDilate->UseVisualStyleBackColor = true;
      this->btnDilate->Click += gcnew System::EventHandler(this, &FormVisMask::btnDilate_Click);
      // 
      // btnFillHole
      // 
      this->btnFillHole->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btnFillHole->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
      this->btnFillHole->Location = System::Drawing::Point(142, 266);
      this->btnFillHole->Name = L"btnFillHole";
      this->btnFillHole->Size = System::Drawing::Size(137, 25);
      this->btnFillHole->TabIndex = 38;
      this->btnFillHole->Text = L"Fill hole ";
      this->btnFillHole->UseVisualStyleBackColor = true;
      this->btnFillHole->Click += gcnew System::EventHandler(this, &FormVisMask::btnFillHole_Click);
      // 
      // groupBox1
      // 
      this->groupBox1->Controls->Add(this->btnErode);
      this->groupBox1->Controls->Add(this->btnExpObj);
      this->groupBox1->Controls->Add(this->checkbox_lock);
      this->groupBox1->Controls->Add(this->alpha);
      this->groupBox1->Controls->Add(this->btnDelete);
      this->groupBox1->Controls->Add(this->trackbar_alpha);
      this->groupBox1->Controls->Add(this->btnColorPallet);
      this->groupBox1->Controls->Add(this->btnDilate);
      this->groupBox1->Location = System::Drawing::Point(142, 12);
      this->groupBox1->Name = L"groupBox1";
      this->groupBox1->Size = System::Drawing::Size(150, 206);
      this->groupBox1->TabIndex = 39;
      this->groupBox1->TabStop = false;
      this->groupBox1->Text = L"Manip. Selected ID";
      // 
      // btnSmartFillHole
      // 
      this->btnSmartFillHole->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btnSmartFillHole->Location = System::Drawing::Point(142, 293);
      this->btnSmartFillHole->Name = L"btnSmartFillHole";
      this->btnSmartFillHole->Size = System::Drawing::Size(137, 25);
      this->btnSmartFillHole->TabIndex = 40;
      this->btnSmartFillHole->Text = L"Smart Fill hole";
      this->btnSmartFillHole->UseVisualStyleBackColor = true;
      this->btnSmartFillHole->Click += gcnew System::EventHandler(this, &FormVisMask::btnSmartFillHole_Click);
      // 
      // FormVisMask
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(300, 404);
      this->Controls->Add(this->btnSmartFillHole);
      this->Controls->Add(this->groupBox1);
      this->Controls->Add(this->btnMargeTo);
      this->Controls->Add(this->maskList);
      this->Controls->Add(this->btnFillHole);
      this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
      this->Margin = System::Windows::Forms::Padding(2);
      this->Name = L"FormVisMask";
      this->Text = L"FormVisMask";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskList))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_alpha))->EndInit();
      this->groupBox1->ResumeLayout(false);
      this->groupBox1->PerformLayout();
      this->ResumeLayout(false);

    }
#pragma endregion
  private: 
    System::Void maskList_SelectionChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void maskList_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e);
    System::Void checkbox_lock_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void trackbar_alpha_Scroll (System::Object^  sender, System::EventArgs^  e) ;
    System::Void btnColorPallet_Click  (System::Object^  sender, System::EventArgs^  e) ;
    System::Void btnDelete_Click       (System::Object^  sender, System::EventArgs^  e) ;
    System::Void btnMargeTo_Click      (System::Object^  sender, System::EventArgs^  e) ;
    System::Void btnErode_Click        (System::Object^  sender, System::EventArgs^  e) ;
    System::Void btnDilate_Click       (System::Object^  sender, System::EventArgs^  e);
    System::Void btnFillHole_Click     (System::Object^  sender, System::EventArgs^  e);
    System::Void btnExpObj_Click       (System::Object^  sender, System::EventArgs^  e) ;
    System::Void btnSmartFillHole_Click(System::Object^  sender, System::EventArgs^  e);
};

  inline void formVisMask_Show() { FormVisMask::getInst()->Show(); }
  inline void formVisMask_Hide() { FormVisMask::getInst()->Hide(); }
  inline void formVisMask_updateList() { FormVisMask::getInst()->updateList(); }

}
