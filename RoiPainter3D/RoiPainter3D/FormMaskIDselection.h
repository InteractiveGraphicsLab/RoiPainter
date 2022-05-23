#pragma once

#pragma unmanaged
#include <vector>
#include "MaskData.h"
#pragma managed

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormMaskIDselection の概要
	/// </summary>
	public ref class FormMaskIDselection : public System::Windows::Forms::Form
	{
  private:
    bool m_bListInit;
    bool m_bMultiSelect;
  private: System::Windows::Forms::Label^  m_label;
           Generic::List<int>^ m_active_ids;
    
    void initList(
      const std::vector<MaskData> &mask_set, 
      const int default_mask_id,
      const int b_multiselect);

	public:

		FormMaskIDselection(
      const std::vector<MaskData> &mask_set, 
      const int default_mask_id,
      const bool b_multiselect)
		{
			InitializeComponent();

      m_bListInit = false;
      m_bMultiSelect = b_multiselect;
      m_active_ids = gcnew Generic::List<int>();
      initList(mask_set, default_mask_id, b_multiselect);

      if ( b_multiselect )
      {
        m_label->Text = 
          "Select multi IDs\n"
          "by ctrl + click";
      }
      else
      {
        m_label->Text = 
          "Select one ID\n"
          "by click";

      }
		}

    void getTrgtIDs( std::set<int> &selected_ids ) 
    { 
      selected_ids.clear();

      for( int i = 0; i < m_active_ids->Count; ++i)
      {
        selected_ids.insert(m_active_ids[i]);
      }
    }

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormMaskIDselection()
		{
			if (components)
			{
				delete components;
			}
		}

  private: 
    System::Windows::Forms::Button^  btnCancel;
    System::Windows::Forms::Button^  btnOk;
    System::Windows::Forms::DataGridView^  maskIdList;

    System::Windows::Forms::DataGridViewTextBoxColumn^  maskIDColumn;
System::Windows::Forms::DataGridViewTextBoxColumn^  colorColumn;

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
      this->btnCancel = (gcnew System::Windows::Forms::Button());
      this->btnOk = (gcnew System::Windows::Forms::Button());
      this->maskIdList = (gcnew System::Windows::Forms::DataGridView());
      this->maskIDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->colorColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->m_label = (gcnew System::Windows::Forms::Label());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskIdList))->BeginInit();
      this->SuspendLayout();
      // 
      // btnCancel
      // 
      this->btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
      this->btnCancel->Location = System::Drawing::Point(145, 222);
      this->btnCancel->Name = L"btnCancel";
      this->btnCancel->Size = System::Drawing::Size(89, 32);
      this->btnCancel->TabIndex = 5;
      this->btnCancel->Text = L"CANCEL";
      this->btnCancel->UseVisualStyleBackColor = true;
      this->btnCancel->Click += gcnew System::EventHandler(this, &FormMaskIDselection::btnCancel_Click);
      // 
      // btnOk
      // 
      this->btnOk->DialogResult = System::Windows::Forms::DialogResult::OK;
      this->btnOk->Location = System::Drawing::Point(145, 184);
      this->btnOk->Name = L"btnOk";
      this->btnOk->Size = System::Drawing::Size(89, 32);
      this->btnOk->TabIndex = 4;
      this->btnOk->Text = L"OK";
      this->btnOk->UseVisualStyleBackColor = true;
      this->btnOk->Click += gcnew System::EventHandler(this, &FormMaskIDselection::btnOk_Click);
      // 
      // maskIdList
      // 
      this->maskIdList->AllowUserToAddRows = false;
      this->maskIdList->AllowUserToDeleteRows = false;
      this->maskIdList->AllowUserToResizeColumns = false;
      this->maskIdList->AllowUserToResizeRows = false;
      this->maskIdList->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
      this->maskIdList->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {
        this->maskIDColumn,
          this->colorColumn
      });
      this->maskIdList->Location = System::Drawing::Point(4, 6);
      this->maskIdList->Name = L"maskIdList";
      this->maskIdList->ReadOnly = true;
      this->maskIdList->RowHeadersVisible = false;
      this->maskIdList->RowTemplate->Height = 21;
      this->maskIdList->Size = System::Drawing::Size(131, 302);
      this->maskIdList->TabIndex = 3;
      this->maskIdList->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &FormMaskIDselection::maskIdList_CellContentClick);
      this->maskIdList->SelectionChanged += gcnew System::EventHandler(this, &FormMaskIDselection::maskIdList_SelectionChanged);
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
      // m_label
      // 
      this->m_label->AutoSize = true;
      this->m_label->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->m_label->Location = System::Drawing::Point(141, 20);
      this->m_label->Name = L"m_label";
      this->m_label->Size = System::Drawing::Size(95, 32);
      this->m_label->TabIndex = 6;
      this->m_label->Text = L"label label label\r\nlabel label label";
      // 
      // FormMaskIDselection
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(240, 312);
      this->Controls->Add(this->m_label);
      this->Controls->Add(this->btnCancel);
      this->Controls->Add(this->btnOk);
      this->Controls->Add(this->maskIdList);
      this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
      this->Name = L"FormMaskIDselection";
      this->Text = L"FormMaskIDselection";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskIdList))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private: System::Void btnOk_Click(
    System::Object^  sender, 
    System::EventArgs^  e) {}
  private: System::Void btnCancel_Click(
    System::Object^  sender, 
    System::EventArgs^  e){}
  private: System::Void maskIdList_CellContentClick(
    System::Object^  sender, 
    System::Windows::Forms::DataGridViewCellEventArgs^  e){}
  private: System::Void maskIdList_SelectionChanged(
    System::Object^  sender, 
    System::EventArgs^  e);
};

  //Show modal dialog for selecting mask id 
  //return -1 when no id is selected
  inline int formMaskIdSelection_DoSingleSelection(
    const std::vector<MaskData> &mask_set, 
    const int default_mask_id)
  {
    FormMaskIDselection ^modal 
      = gcnew FormMaskIDselection(mask_set, default_mask_id, false);
    
    if (modal->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) 
      return -1;

    std::set<int> selected_ids;
    modal->getTrgtIDs(selected_ids);
    modal->Close();

    if ( selected_ids.empty() ) return -1;
    else return *selected_ids.begin();
  }


  //選択されなかった場合は空のvectorが返る
  inline std::set<int> formMaskIdSelection_DoMultiSelection(
    const std::vector<MaskData> &mask_set, 
    const int default_mask_id)
  {
    std::cout << "start multi maskid selection\n";
    FormMaskIDselection ^modal 
      = gcnew FormMaskIDselection(mask_set, default_mask_id, true);
    
    std::set<int> selected_ids;

    if (modal->ShowDialog() != System::Windows::Forms::DialogResult::Cancel) 
    {
      modal->getTrgtIDs(selected_ids);
      modal->Close();
    }

    std::cout << "multi maskid selection finished\n";
    return selected_ids;
  }

}
