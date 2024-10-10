#pragma once

namespace RoiPainter4D {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  /// <summary>
  /// FormSelectMskId の概要
  /// </summary>
  public ref class FormSelectMskId : public System::Windows::Forms::Form
  {
    bool m_bListInit;
    int  m_activeId;

  public:
    FormSelectMskId()
    {
      InitializeComponent();
      //
      //
      //
      initList();
      btnOk->Select();
    }

    void initList();
    int  getTrgtID() { return m_activeId; } //should be called after the OK btn presssed


  protected:
    /// <summary>
    /// 使用中のリソースをすべてクリーンアップします。
    /// </summary>
    ~FormSelectMskId()
    {
      if (components)
      {
        delete components;
      }
    }
  private: System::Windows::Forms::DataGridView^  maskList;
  private: System::Windows::Forms::Button^  btnOk;
  private: System::Windows::Forms::Button^  btnCancel;
  private: System::Windows::Forms::DataGridViewTextBoxColumn^  maskIDColumn;
  private: System::Windows::Forms::DataGridViewTextBoxColumn^  colorColumn;

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
      this->maskList = (gcnew System::Windows::Forms::DataGridView());
      this->maskIDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->colorColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->btnOk = (gcnew System::Windows::Forms::Button());
      this->btnCancel = (gcnew System::Windows::Forms::Button());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskList))->BeginInit();
      this->SuspendLayout();
      // 
      // maskList
      // 
      this->maskList->AllowUserToAddRows = false;
      this->maskList->AllowUserToDeleteRows = false;
      this->maskList->AllowUserToResizeColumns = false;
      this->maskList->AllowUserToResizeRows = false;
      this->maskList->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
      this->maskList->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {
        this->maskIDColumn,
          this->colorColumn
      });
      this->maskList->Location = System::Drawing::Point(13, 13);
      this->maskList->Name = L"maskList";
      this->maskList->ReadOnly = true;
      this->maskList->RowHeadersVisible = false;
      this->maskList->RowTemplate->Height = 21;
      this->maskList->Size = System::Drawing::Size(97, 206);
      this->maskList->TabIndex = 0;
      this->maskList->SelectionChanged += gcnew System::EventHandler(this, &FormSelectMskId::maskList_SelectionChanged);
      // 
      // maskIDColumn
      // 
      this->maskIDColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      this->maskIDColumn->HeaderText = L"maskId";
      this->maskIDColumn->Name = L"maskIDColumn";
      this->maskIDColumn->ReadOnly = true;
      // 
      // colorColumn
      // 
      this->colorColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      this->colorColumn->FillWeight = 70;
      this->colorColumn->HeaderText = L"color";
      this->colorColumn->Name = L"colorColumn";
      this->colorColumn->ReadOnly = true;
      // 
      // btnOk
      // 
      this->btnOk->DialogResult = System::Windows::Forms::DialogResult::OK;
      this->btnOk->Location = System::Drawing::Point(140, 89);
      this->btnOk->Name = L"btnOk";
      this->btnOk->Size = System::Drawing::Size(63, 32);
      this->btnOk->TabIndex = 1;
      this->btnOk->Text = L"OK";
      this->btnOk->UseVisualStyleBackColor = true;
      // 
      // btnCancel
      // 
      this->btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
      this->btnCancel->Location = System::Drawing::Point(140, 127);
      this->btnCancel->Name = L"btnCancel";
      this->btnCancel->Size = System::Drawing::Size(63, 32);
      this->btnCancel->TabIndex = 2;
      this->btnCancel->Text = L"CANCEL";
      this->btnCancel->UseVisualStyleBackColor = true;
      // 
      // FormSelectMskId
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(233, 233);
      this->Controls->Add(this->btnCancel);
      this->Controls->Add(this->btnOk);
      this->Controls->Add(this->maskList);
      this->Name = L"FormSelectMskId";
      this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
      this->Text = L"Select Target Mask";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskList))->EndInit();
      this->ResumeLayout(false);

    }
#pragma endregion

  private: System::Void maskList_SelectionChanged(System::Object^  sender, System::EventArgs^  e);
  };

  //選択されなかった場合は-1が返る
  inline int formSelectMasdskID_showModalDialog()
  {
    FormSelectMskId ^modal = gcnew FormSelectMskId();
    if (modal->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return -1;

    int trgtId = modal->getTrgtID();
    modal->Close();
    return trgtId;
  }

}
