#pragma once

namespace RoiPainter4D {

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
    System::Windows::Forms::DataGridView^  maskList;
    System::Windows::Forms::CheckBox^  checkbox_lock;
    System::Windows::Forms::TrackBar^  trackbar_alpha;
    System::Windows::Forms::TextBox^  alpha;
    System::Windows::Forms::Button^  btnThisErode;
    System::Windows::Forms::Button^  btnThisDilate;
    System::Windows::Forms::Button^  btnThisFill;
    System::Windows::Forms::Button^  btnThisExpObj;
    System::Windows::Forms::Button^  btnAllErode;
    System::Windows::Forms::Button^  btnAllDilate;
    System::Windows::Forms::Button^  btnAllFill;
    System::Windows::Forms::Button^  btnAllExpObj;
    System::Windows::Forms::Button^  btnDelete;
    System::Windows::Forms::Button^  btnMargeTo;
    System::Windows::Forms::Button^  btnColorPallet;
    System::Windows::Forms::Button^  btnTestAdd;
    System::Windows::Forms::ColorDialog^  colorDialog1;
    System::Windows::Forms::BindingSource^  bindingSource1;
    System::Windows::Forms::DataGridViewTextBoxColumn^  regionIDColumn;
    System::Windows::Forms::DataGridViewTextBoxColumn^  colorColumn;
    System::Windows::Forms::GroupBox^  groupBox1;
    System::Windows::Forms::GroupBox^  groupBox2;
    System::Windows::Forms::Label^  label1;

  private:
    bool m_bListUpdating;
  private: System::Windows::Forms::Button^ btnThisImpObj;
  private: System::Windows::Forms::Button^ btnAllImpObj;
         static FormVisMask^ m_singleton;
    FormVisMask(void)
    {
      InitializeComponent();
      this->ControlBox = false;
      this->MaximizeBox = false;
      m_bListUpdating = false;

      trackbar_alpha->SetRange(0, 100);
    }

  public:
    static FormVisMask^ GetInst() {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormVisMask();
      }
      return m_singleton;
    }

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

  public:
    void updateList();
  private: System::ComponentModel::IContainer^  components;
  protected:

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
      this->components = (gcnew System::ComponentModel::Container());
      System::Windows::Forms::DataGridViewCellStyle^ dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
      this->maskList = (gcnew System::Windows::Forms::DataGridView());
      this->regionIDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->colorColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->checkbox_lock = (gcnew System::Windows::Forms::CheckBox());
      this->trackbar_alpha = (gcnew System::Windows::Forms::TrackBar());
      this->alpha = (gcnew System::Windows::Forms::TextBox());
      this->btnColorPallet = (gcnew System::Windows::Forms::Button());
      this->btnThisExpObj = (gcnew System::Windows::Forms::Button());
      this->btnThisErode = (gcnew System::Windows::Forms::Button());
      this->btnThisDilate = (gcnew System::Windows::Forms::Button());
      this->btnThisFill = (gcnew System::Windows::Forms::Button());
      this->colorDialog1 = (gcnew System::Windows::Forms::ColorDialog());
      this->btnAllFill = (gcnew System::Windows::Forms::Button());
      this->btnAllDilate = (gcnew System::Windows::Forms::Button());
      this->btnAllErode = (gcnew System::Windows::Forms::Button());
      this->btnTestAdd = (gcnew System::Windows::Forms::Button());
      this->bindingSource1 = (gcnew System::Windows::Forms::BindingSource(this->components));
      this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
      this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
      this->btnAllExpObj = (gcnew System::Windows::Forms::Button());
      this->btnDelete = (gcnew System::Windows::Forms::Button());
      this->btnMargeTo = (gcnew System::Windows::Forms::Button());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->btnThisImpObj = (gcnew System::Windows::Forms::Button());
      this->btnAllImpObj = (gcnew System::Windows::Forms::Button());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskList))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_alpha))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->bindingSource1))->BeginInit();
      this->groupBox1->SuspendLayout();
      this->groupBox2->SuspendLayout();
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
        this->regionIDColumn,
          this->colorColumn
      });
      this->maskList->Location = System::Drawing::Point(14, 13);
      this->maskList->MultiSelect = false;
      this->maskList->Name = L"maskList";
      this->maskList->ReadOnly = true;
      this->maskList->RowHeadersVisible = false;
      this->maskList->RowTemplate->Height = 21;
      this->maskList->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
      this->maskList->Size = System::Drawing::Size(128, 392);
      this->maskList->TabIndex = 0;
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
      dataGridViewCellStyle1->BackColor = System::Drawing::Color::White;
      dataGridViewCellStyle1->ForeColor = System::Drawing::Color::Black;
      dataGridViewCellStyle1->SelectionBackColor = System::Drawing::Color::Transparent;
      dataGridViewCellStyle1->SelectionForeColor = System::Drawing::Color::Black;
      this->colorColumn->DefaultCellStyle = dataGridViewCellStyle1;
      this->colorColumn->FillWeight = 80;
      this->colorColumn->HeaderText = L"color";
      this->colorColumn->Name = L"colorColumn";
      this->colorColumn->ReadOnly = true;
      this->colorColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
      this->colorColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
      // 
      // checkbox_lock
      // 
      this->checkbox_lock->AutoSize = true;
      this->checkbox_lock->Location = System::Drawing::Point(148, 16);
      this->checkbox_lock->Name = L"checkbox_lock";
      this->checkbox_lock->Size = System::Drawing::Size(48, 16);
      this->checkbox_lock->TabIndex = 1;
      this->checkbox_lock->Text = L"Lock";
      this->checkbox_lock->UseVisualStyleBackColor = true;
      this->checkbox_lock->CheckedChanged += gcnew System::EventHandler(this, &FormVisMask::checkbox_lock_CheckedChanged);
      // 
      // trackbar_alpha
      // 
      this->trackbar_alpha->Location = System::Drawing::Point(172, 40);
      this->trackbar_alpha->Name = L"trackbar_alpha";
      this->trackbar_alpha->Size = System::Drawing::Size(111, 45);
      this->trackbar_alpha->TabIndex = 2;
      this->trackbar_alpha->Scroll += gcnew System::EventHandler(this, &FormVisMask::trackbar_alpha_Scroll);
      // 
      // alpha
      // 
      this->alpha->BackColor = System::Drawing::SystemColors::Menu;
      this->alpha->BorderStyle = System::Windows::Forms::BorderStyle::None;
      this->alpha->Location = System::Drawing::Point(145, 52);
      this->alpha->Name = L"alpha";
      this->alpha->ReadOnly = true;
      this->alpha->Size = System::Drawing::Size(44, 12);
      this->alpha->TabIndex = 3;
      this->alpha->Text = L"alpha";
      // 
      // btnColorPallet
      // 
      this->btnColorPallet->Location = System::Drawing::Point(201, 12);
      this->btnColorPallet->Name = L"btnColorPallet";
      this->btnColorPallet->Size = System::Drawing::Size(75, 23);
      this->btnColorPallet->TabIndex = 4;
      this->btnColorPallet->Text = L"color";
      this->btnColorPallet->UseVisualStyleBackColor = true;
      this->btnColorPallet->Click += gcnew System::EventHandler(this, &FormVisMask::btnColorPallet_Click);
      // 
      // btnThisExpObj
      // 
      this->btnThisExpObj->Location = System::Drawing::Point(6, 105);
      this->btnThisExpObj->Name = L"btnThisExpObj";
      this->btnThisExpObj->Size = System::Drawing::Size(59, 23);
      this->btnThisExpObj->TabIndex = 5;
      this->btnThisExpObj->Text = L"Exp Obj";
      this->btnThisExpObj->UseVisualStyleBackColor = true;
      this->btnThisExpObj->Click += gcnew System::EventHandler(this, &FormVisMask::btnThisExpObj_Click);
      // 
      // btnThisErode
      // 
      this->btnThisErode->Location = System::Drawing::Point(7, 18);
      this->btnThisErode->Name = L"btnThisErode";
      this->btnThisErode->Size = System::Drawing::Size(52, 23);
      this->btnThisErode->TabIndex = 8;
      this->btnThisErode->Text = L"erode";
      this->btnThisErode->UseVisualStyleBackColor = true;
      this->btnThisErode->Click += gcnew System::EventHandler(this, &FormVisMask::btnThisErode_Click);
      // 
      // btnThisDilate
      // 
      this->btnThisDilate->Location = System::Drawing::Point(7, 47);
      this->btnThisDilate->Name = L"btnThisDilate";
      this->btnThisDilate->Size = System::Drawing::Size(52, 23);
      this->btnThisDilate->TabIndex = 9;
      this->btnThisDilate->Text = L"dilate";
      this->btnThisDilate->UseVisualStyleBackColor = true;
      this->btnThisDilate->Click += gcnew System::EventHandler(this, &FormVisMask::btnThisDilate_Click);
      // 
      // btnThisFill
      // 
      this->btnThisFill->Location = System::Drawing::Point(7, 76);
      this->btnThisFill->Name = L"btnThisFill";
      this->btnThisFill->Size = System::Drawing::Size(52, 23);
      this->btnThisFill->TabIndex = 10;
      this->btnThisFill->Text = L"fill hole";
      this->btnThisFill->UseVisualStyleBackColor = true;
      this->btnThisFill->Click += gcnew System::EventHandler(this, &FormVisMask::btnThisFill_Click);
      // 
      // btnAllFill
      // 
      this->btnAllFill->Location = System::Drawing::Point(6, 75);
      this->btnAllFill->Name = L"btnAllFill";
      this->btnAllFill->Size = System::Drawing::Size(55, 23);
      this->btnAllFill->TabIndex = 19;
      this->btnAllFill->Text = L"fill hole";
      this->btnAllFill->UseVisualStyleBackColor = true;
      this->btnAllFill->Click += gcnew System::EventHandler(this, &FormVisMask::btnAllFill_Click);
      // 
      // btnAllDilate
      // 
      this->btnAllDilate->Location = System::Drawing::Point(6, 47);
      this->btnAllDilate->Name = L"btnAllDilate";
      this->btnAllDilate->Size = System::Drawing::Size(55, 23);
      this->btnAllDilate->TabIndex = 18;
      this->btnAllDilate->Text = L"dilate";
      this->btnAllDilate->UseVisualStyleBackColor = true;
      this->btnAllDilate->Click += gcnew System::EventHandler(this, &FormVisMask::btnAllDilate_Click);
      // 
      // btnAllErode
      // 
      this->btnAllErode->Location = System::Drawing::Point(6, 18);
      this->btnAllErode->Name = L"btnAllErode";
      this->btnAllErode->Size = System::Drawing::Size(55, 23);
      this->btnAllErode->TabIndex = 17;
      this->btnAllErode->Text = L"erode";
      this->btnAllErode->UseVisualStyleBackColor = true;
      this->btnAllErode->Click += gcnew System::EventHandler(this, &FormVisMask::btnAllErode_Click);
      // 
      // btnTestAdd
      // 
      this->btnTestAdd->Location = System::Drawing::Point(208, 378);
      this->btnTestAdd->Name = L"btnTestAdd";
      this->btnTestAdd->Size = System::Drawing::Size(75, 23);
      this->btnTestAdd->TabIndex = 20;
      this->btnTestAdd->Text = L"Add";
      this->btnTestAdd->UseVisualStyleBackColor = true;
      this->btnTestAdd->Click += gcnew System::EventHandler(this, &FormVisMask::btnTestAdd_Click);
      // 
      // groupBox1
      // 
      this->groupBox1->Controls->Add(this->btnThisExpObj);
      this->groupBox1->Controls->Add(this->btnThisErode);
      this->groupBox1->Controls->Add(this->btnThisDilate);
      this->groupBox1->Controls->Add(this->btnThisFill);
      this->groupBox1->Location = System::Drawing::Point(138, 145);
      this->groupBox1->Name = L"groupBox1";
      this->groupBox1->Size = System::Drawing::Size(72, 165);
      this->groupBox1->TabIndex = 21;
      this->groupBox1->TabStop = false;
      this->groupBox1->Text = L"this frame";
      // 
      // groupBox2
      // 
      this->groupBox2->Controls->Add(this->btnAllExpObj);
      this->groupBox2->Controls->Add(this->btnAllErode);
      this->groupBox2->Controls->Add(this->btnAllFill);
      this->groupBox2->Controls->Add(this->btnAllDilate);
      this->groupBox2->Location = System::Drawing::Point(216, 145);
      this->groupBox2->Name = L"groupBox2";
      this->groupBox2->Size = System::Drawing::Size(75, 165);
      this->groupBox2->TabIndex = 22;
      this->groupBox2->TabStop = false;
      this->groupBox2->Text = L"All frames";
      // 
      // btnAllExpObj
      // 
      this->btnAllExpObj->Location = System::Drawing::Point(6, 104);
      this->btnAllExpObj->Name = L"btnAllExpObj";
      this->btnAllExpObj->Size = System::Drawing::Size(59, 23);
      this->btnAllExpObj->TabIndex = 23;
      this->btnAllExpObj->Text = L"Exp Obj";
      this->btnAllExpObj->UseVisualStyleBackColor = true;
      this->btnAllExpObj->Click += gcnew System::EventHandler(this, &FormVisMask::btnAllExpObj_Click);
      // 
      // btnDelete
      // 
      this->btnDelete->Location = System::Drawing::Point(144, 97);
      this->btnDelete->Name = L"btnDelete";
      this->btnDelete->Size = System::Drawing::Size(66, 23);
      this->btnDelete->TabIndex = 23;
      this->btnDelete->Text = L"DELETE";
      this->btnDelete->UseVisualStyleBackColor = true;
      this->btnDelete->Click += gcnew System::EventHandler(this, &FormVisMask::btnDelete_Click);
      // 
      // btnMargeTo
      // 
      this->btnMargeTo->Location = System::Drawing::Point(213, 97);
      this->btnMargeTo->Name = L"btnMargeTo";
      this->btnMargeTo->Size = System::Drawing::Size(74, 23);
      this->btnMargeTo->TabIndex = 24;
      this->btnMargeTo->Text = L"MARGE TO";
      this->btnMargeTo->UseVisualStyleBackColor = true;
      this->btnMargeTo->Click += gcnew System::EventHandler(this, &FormVisMask::btnMargeTo_Click);
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Location = System::Drawing::Point(148, 347);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(96, 12);
      this->label1->TabIndex = 25;
      this->label1->Text = L"Shift: Hide Mask";
      // 
      // btnThisImpObj
      // 
      this->btnThisImpObj->Location = System::Drawing::Point(145, 279);
      this->btnThisImpObj->Name = L"btnThisImpObj";
      this->btnThisImpObj->Size = System::Drawing::Size(59, 23);
      this->btnThisImpObj->TabIndex = 5;
      this->btnThisImpObj->Text = L"Imp Obj";
      this->btnThisImpObj->UseVisualStyleBackColor = true;
      this->btnThisImpObj->Click += gcnew System::EventHandler(this, &FormVisMask::btnThisImpObj_Click);
      // 
      // btnAllImpObj
      // 
      this->btnAllImpObj->Location = System::Drawing::Point(222, 279);
      this->btnAllImpObj->Name = L"btnAllImpObj";
      this->btnAllImpObj->Size = System::Drawing::Size(59, 23);
      this->btnAllImpObj->TabIndex = 5;
      this->btnAllImpObj->Text = L"Imp Obj";
      this->btnAllImpObj->UseVisualStyleBackColor = true;
      this->btnAllImpObj->Click += gcnew System::EventHandler(this, &FormVisMask::btnAllImpObj_Click);
      // 
      // FormVisMask
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->BackColor = System::Drawing::SystemColors::Control;
      this->ClientSize = System::Drawing::Size(293, 414);
      this->Controls->Add(this->btnAllImpObj);
      this->Controls->Add(this->btnThisImpObj);
      this->Controls->Add(this->label1);
      this->Controls->Add(this->btnMargeTo);
      this->Controls->Add(this->btnDelete);
      this->Controls->Add(this->btnTestAdd);
      this->Controls->Add(this->btnColorPallet);
      this->Controls->Add(this->checkbox_lock);
      this->Controls->Add(this->maskList);
      this->Controls->Add(this->trackbar_alpha);
      this->Controls->Add(this->groupBox2);
      this->Controls->Add(this->groupBox1);
      this->Controls->Add(this->alpha);
      this->Name = L"FormVisMask";
      this->Text = L"FormVisMask";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskList))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_alpha))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->bindingSource1))->EndInit();
      this->groupBox1->ResumeLayout(false);
      this->groupBox2->ResumeLayout(false);
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion

  private: System::Void btnTestAdd_Click(System::Object^  sender, System::EventArgs^  e) {
    maskList->Rows->Add("1", "赤");
    //	maskList[1, 0]->Style->BackColor = Color::FromArgb(255, 255, 0, 0);
  }

  private: System::Void btnColorPallet_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void maskList_SelectionChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void checkbox_lock_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void trackbar_alpha_Scroll(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnDelete_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnMargeTo_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnThisErode_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnThisDilate_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnThisFill_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnThisExpObj_Click(System::Object^ sender, System::EventArgs^ e);
  private: System::Void btnThisImpObj_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnAllErode_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnAllDilate_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnAllFill_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnAllExpObj_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnAllImpObj_Click(System::Object^ sender, System::EventArgs^ e);
  private: System::Void maskList_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
  }
  };
  inline void formVisMask_Show() { FormVisMask::GetInst()->Show(); }
  inline void formVisMask_Hide() { FormVisMask::GetInst()->Hide(); }
  inline void formVisMask_updateList() { FormVisMask::GetInst()->updateList(); }
}
