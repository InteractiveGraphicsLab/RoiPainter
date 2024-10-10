#pragma once
#include <vector>
#include <string>

namespace RoiPainter4D {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  /// <summary>
  /// FormSortFiles �̊T�v
  /// </summary>
  public ref class FormSortFiles : public System::Windows::Forms::Form
  {
  private: 
    System::Windows::Forms::GroupBox^ groupBox1;
    System::Windows::Forms::GroupBox^ groupBox2;

    System::Windows::Forms::RadioButton^ radioBtnName;
    System::Windows::Forms::RadioButton^ radioBtnDate;
    System::Windows::Forms::RadioButton^ radioBtnUp;
    System::Windows::Forms::RadioButton^ radioBtnDown;

    System::Windows::Forms::Button^ btnSort;
    System::Windows::Forms::Button^ btnImport;
    System::Windows::Forms::Button^ btnCancel;
    System::Windows::Forms::DataGridViewTextBoxColumn^ fileNoColumn;
    System::Windows::Forms::DataGridViewTextBoxColumn^ fileNameColumn;
    System::Windows::Forms::DataGridViewTextBoxColumn^ ColumnCreationTime;
    System::Windows::Forms::DataGridView^ fnameList;

  protected:
    ~FormSortFiles()
    {
      if (components)
      {
        delete components;
      }
    }

  private:
    void initList(const std::vector<std::string> &fnames, const std::vector<std::string> &fDateTimes);
    void updateList(const std::vector<int> &idxs);

  public:
    void copyNameList(std::vector<std::string> &fNames);
    FormSortFiles(const std::vector<std::string>& fNames, const std::vector<std::string>& fDateTimes)
    {
      InitializeComponent();

      initList(fNames, fDateTimes);
    }


  private:
    System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
    /// <summary>
    /// �f�U�C�i�[ �T�|�[�g�ɕK�v�ȃ��\�b�h�ł��B���̃��\�b�h�̓�e��
    /// �R�[�h �G�f�B�^�[�ŕύX���Ȃ��ł��������B
    /// </summary>
    void InitializeComponent(void)
    {
      this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
      this->radioBtnDate = (gcnew System::Windows::Forms::RadioButton());
      this->radioBtnName = (gcnew System::Windows::Forms::RadioButton());
      this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
      this->radioBtnDown = (gcnew System::Windows::Forms::RadioButton());
      this->radioBtnUp = (gcnew System::Windows::Forms::RadioButton());
      this->fnameList = (gcnew System::Windows::Forms::DataGridView());
      this->fileNoColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->fileNameColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->ColumnCreationTime = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->btnSort = (gcnew System::Windows::Forms::Button());
      this->btnImport = (gcnew System::Windows::Forms::Button());
      this->btnCancel = (gcnew System::Windows::Forms::Button());
      this->groupBox1->SuspendLayout();
      this->groupBox2->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fnameList))->BeginInit();
      this->SuspendLayout();
      // 
      // groupBox1
      // 
      this->groupBox1->Controls->Add(this->radioBtnDate);
      this->groupBox1->Controls->Add(this->radioBtnName);
      this->groupBox1->Location = System::Drawing::Point(65, 203);
      this->groupBox1->Name = L"groupBox1";
      this->groupBox1->Size = System::Drawing::Size(115, 71);
      this->groupBox1->TabIndex = 0;
      this->groupBox1->TabStop = false;
      this->groupBox1->Text = L"Type";
      // 
      // radioBtnDate
      // 
      this->radioBtnDate->AutoSize = true;
      this->radioBtnDate->Location = System::Drawing::Point(11, 42);
      this->radioBtnDate->Name = L"radioBtnDate";
      this->radioBtnDate->Size = System::Drawing::Size(45, 16);
      this->radioBtnDate->TabIndex = 1;
      this->radioBtnDate->TabStop = true;
      this->radioBtnDate->Text = L"date";
      this->radioBtnDate->UseVisualStyleBackColor = true;
      // 
      // radioBtnName
      // 
      this->radioBtnName->AutoSize = true;
      this->radioBtnName->Checked = true;
      this->radioBtnName->Location = System::Drawing::Point(11, 20);
      this->radioBtnName->Name = L"radioBtnName";
      this->radioBtnName->Size = System::Drawing::Size(50, 16);
      this->radioBtnName->TabIndex = 0;
      this->radioBtnName->TabStop = true;
      this->radioBtnName->Text = L"name";
      this->radioBtnName->UseVisualStyleBackColor = true;
      // 
      // groupBox2
      // 
      this->groupBox2->Controls->Add(this->radioBtnDown);
      this->groupBox2->Controls->Add(this->radioBtnUp);
      this->groupBox2->Location = System::Drawing::Point(186, 203);
      this->groupBox2->Name = L"groupBox2";
      this->groupBox2->Size = System::Drawing::Size(115, 71);
      this->groupBox2->TabIndex = 1;
      this->groupBox2->TabStop = false;
      this->groupBox2->Text = L"UpDown";
      // 
      // radioBtnDown
      // 
      this->radioBtnDown->AutoSize = true;
      this->radioBtnDown->Location = System::Drawing::Point(11, 42);
      this->radioBtnDown->Name = L"radioBtnDown";
      this->radioBtnDown->Size = System::Drawing::Size(49, 16);
      this->radioBtnDown->TabIndex = 3;
      this->radioBtnDown->TabStop = true;
      this->radioBtnDown->Text = L"down";
      this->radioBtnDown->UseVisualStyleBackColor = true;
      // 
      // radioBtnUp
      // 
      this->radioBtnUp->AutoSize = true;
      this->radioBtnUp->Checked = true;
      this->radioBtnUp->Location = System::Drawing::Point(11, 20);
      this->radioBtnUp->Name = L"radioBtnUp";
      this->radioBtnUp->Size = System::Drawing::Size(35, 16);
      this->radioBtnUp->TabIndex = 2;
      this->radioBtnUp->TabStop = true;
      this->radioBtnUp->Text = L"up";
      this->radioBtnUp->UseVisualStyleBackColor = true;
      // 
      // fnameList
      // 
      this->fnameList->AllowUserToAddRows = false;
      this->fnameList->AllowUserToDeleteRows = false;
      this->fnameList->AllowUserToResizeColumns = false;
      this->fnameList->AllowUserToResizeRows = false;
      this->fnameList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
        | System::Windows::Forms::AnchorStyles::Right));
      this->fnameList->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;
      this->fnameList->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
      this->fnameList->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(3) {
        this->fileNoColumn,
          this->fileNameColumn, this->ColumnCreationTime
      });
      this->fnameList->Location = System::Drawing::Point(12, 18);
      this->fnameList->Name = L"fnameList";
      this->fnameList->ReadOnly = true;
      this->fnameList->RowHeadersVisible = false;
      this->fnameList->RowTemplate->Height = 21;
      this->fnameList->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
      this->fnameList->Size = System::Drawing::Size(588, 168);
      this->fnameList->TabIndex = 2;
      // 
      // fileNoColumn
      // 
      this->fileNoColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::None;
      this->fileNoColumn->FillWeight = 25;
      this->fileNoColumn->Frozen = true;
      this->fileNoColumn->HeaderText = L"No.";
      this->fileNoColumn->MinimumWidth = 30;
      this->fileNoColumn->Name = L"fileNoColumn";
      this->fileNoColumn->ReadOnly = true;
      this->fileNoColumn->Width = 30;
      // 
      // fileNameColumn
      // 
      this->fileNameColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      this->fileNameColumn->FillWeight = 60;
      this->fileNameColumn->HeaderText = L"name";
      this->fileNameColumn->MinimumWidth = 109;
      this->fileNameColumn->Name = L"fileNameColumn";
      this->fileNameColumn->ReadOnly = true;
      // 
      // ColumnCreationTime
      // 
      this->ColumnCreationTime->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::None;
      this->ColumnCreationTime->HeaderText = L"creation time";
      this->ColumnCreationTime->Name = L"ColumnCreationTime";
      this->ColumnCreationTime->ReadOnly = true;
      this->ColumnCreationTime->Width = 235;
      // 
      // btnSort
      // 
      this->btnSort->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
      this->btnSort->Location = System::Drawing::Point(316, 223);
      this->btnSort->Name = L"btnSort";
      this->btnSort->Size = System::Drawing::Size(142, 34);
      this->btnSort->TabIndex = 3;
      this->btnSort->Text = L"Sort";
      this->btnSort->UseVisualStyleBackColor = true;
      this->btnSort->Click += gcnew System::EventHandler(this, &FormSortFiles::btnSort_Click);
      // 
      // btnImport
      // 
      this->btnImport->DialogResult = System::Windows::Forms::DialogResult::OK;
      this->btnImport->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
      this->btnImport->Location = System::Drawing::Point(12, 292);
      this->btnImport->Name = L"btnImport";
      this->btnImport->Size = System::Drawing::Size(407, 34);
      this->btnImport->TabIndex = 0;
      this->btnImport->Text = L"Import Files";
      this->btnImport->UseVisualStyleBackColor = true;
      this->btnImport->Click += gcnew System::EventHandler(this, &FormSortFiles::btnImport_Click);
      // 
      // btnCancel
      // 
      this->btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
      this->btnCancel->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
      this->btnCancel->Location = System::Drawing::Point(425, 292);
      this->btnCancel->Name = L"btnCancel";
      this->btnCancel->Size = System::Drawing::Size(85, 34);
      this->btnCancel->TabIndex = 5;
      this->btnCancel->Text = L"Cancel";
      this->btnCancel->UseVisualStyleBackColor = true;
      this->btnCancel->Click += gcnew System::EventHandler(this, &FormSortFiles::btnCancel_Click);
      // 
      // FormSortFiles
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(612, 337);
      this->Controls->Add(this->btnCancel);
      this->Controls->Add(this->btnImport);
      this->Controls->Add(this->btnSort);
      this->Controls->Add(this->fnameList);
      this->Controls->Add(this->groupBox2);
      this->Controls->Add(this->groupBox1);
      this->Name = L"FormSortFiles";
      this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
      this->Text = L"FormSortFiles";
      this->groupBox1->ResumeLayout(false);
      this->groupBox1->PerformLayout();
      this->groupBox2->ResumeLayout(false);
      this->groupBox2->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fnameList))->EndInit();
      this->ResumeLayout(false);

    }
#pragma endregion
  private: System::Void btnSort_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnImport_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnCancel_Click(System::Object^  sender, System::EventArgs^  e);
};

  inline bool formSortFiles_showModalDialog(std::vector<std::string> &fNames, std::vector<std::string> &fDateTimes)
  {
    FormSortFiles ^modal = gcnew FormSortFiles(fNames, fDateTimes);
    if (modal->ShowDialog() == DialogResult::Cancel) return false;
    modal->copyNameList(fNames);
    modal->Close();
    return true;
  }
}
