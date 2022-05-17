#pragma once

namespace RoiPainter4D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegSwallowOrgans の概要
	/// </summary>
	public ref class FormSegSwallowOrgans : public System::Windows::Forms::Form
	{
  private: System::Windows::Forms::Label^  m_label1;
  private: System::Windows::Forms::Label^  m_label2;
  private: System::Windows::Forms::Label^  m_label3;
  private: System::Windows::Forms::Label^  m_label4;
  private: System::Windows::Forms::Label^  label1;
  private: System::Windows::Forms::Label^ label2;

  private: System::Windows::Forms::RadioButton^  m_radiobtn_scalecage;
  private: System::Windows::Forms::RadioButton^  m_radiobtn_rotatecage;
  private: System::Windows::Forms::RadioButton^  m_radiobtn_translatecage;

  private: System::Windows::Forms::Button^  m_btn_addmesh;
  private: System::Windows::Forms::Button^  m_btn_savecagetxt;
  private: System::Windows::Forms::Button^  m_btn_loadcagetxt;
  private: System::Windows::Forms::Button^  m_btn_cancel;
  private: System::Windows::Forms::Button^  m_btn_finish;
  private: System::Windows::Forms::Button^  m_btn_copytoallframes;
  private: System::Windows::Forms::Button^  m_btn_clearselection;
  private: System::Windows::Forms::Button^  m_btn_registgroup;
  private: System::Windows::Forms::Button^  m_btn_loadgroup;
  private: System::Windows::Forms::Button^  m_btn_savegroup;
  private: System::Windows::Forms::Button^  m_btn_savemeshtxt;
  private: System::Windows::Forms::Button^  m_btn_savemeshobj;
  private: System::Windows::Forms::Button^  m_btn_loadvisobjs;
  private: System::Windows::Forms::Button^  m_btn_clearvisobjs;

  private: System::Windows::Forms::GroupBox^  m_groupbox_selection;
  private: System::Windows::Forms::GroupBox^  m_groupbox_deformation;
  private: System::Windows::Forms::TextBox^   m_textbox_selection;
  private: System::Windows::Forms::GroupBox^  groupBox1;

  private: System::Windows::Forms::DataGridView^  m_grouplist;
  private: System::Windows::Forms::DataGridViewTextBoxColumn^  m_cageidgroup;

  private: System::Windows::Forms::CheckBox^  m_checkbox_visbound;
  private: System::Windows::Forms::CheckBox^  m_checkbox_fit_crssec;
  private: System::Windows::Forms::CheckBox^  m_checkbox_show_only_selected_points;
  private: System::Windows::Forms::CheckBox^  m_checkbox_vissurf_trans;
  private: System::Windows::Forms::CheckBox^  m_checkbox_vissurf_solid;
  private: System::Windows::Forms::CheckBox^  m_checkbox_vismeshes;

  bool m_bListUpdating;
  private: System::Windows::Forms::GroupBox^  groupBox2;
  private: System::Windows::Forms::NumericUpDown^  m_updown_cpsize;

  private: System::Windows::Forms::Label^  m_label5;
  private: System::Windows::Forms::Button^ m_btn_loaddeform_nextcage;



    static FormSegSwallowOrgans^ m_singleton;

  public:
    static FormSegSwallowOrgans^ GetInst() {
      if (m_singleton == nullptr) {
        m_singleton = gcnew FormSegSwallowOrgans();
      }
      return m_singleton;
    }

    void InitAllItems();
    void SetNumSelectVtx(int num_select_vtx);
    void FitCrssecToSelectedCageVtx();
    void UpdateList();
    bool IsModeScale    () { return m_radiobtn_scalecage    ->Checked ; }
    bool IsModeRotate   () { return m_radiobtn_rotatecage   ->Checked ; }
    bool IsModeTranslate() { return m_radiobtn_translatecage->Checked ; }
    bool DoVisSurfTrans () { return m_checkbox_vissurf_trans->Checked ; }
    bool DoVisSurfSolid () { return m_checkbox_vissurf_solid->Checked ; }
    bool DoVisMeshes    () { return m_checkbox_vismeshes    ->Checked ; }

    bool DoVisBound(){ return m_checkbox_visbound->Checked; }
    bool DoFitCrssec(){ return m_checkbox_fit_crssec->Checked; }
    bool DoShowOnlySelectedPts(){ return m_checkbox_show_only_selected_points->Checked; }
    int  GetCpSize( ) { return (int)m_updown_cpsize->Value; }
	public:
		FormSegSwallowOrgans(void)
		{
      m_bListUpdating = false;

			InitializeComponent();
		}

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormSegSwallowOrgans()
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
            this->m_label1 = (gcnew System::Windows::Forms::Label());
            this->m_label3 = (gcnew System::Windows::Forms::Label());
            this->m_radiobtn_rotatecage = (gcnew System::Windows::Forms::RadioButton());
            this->m_radiobtn_translatecage = (gcnew System::Windows::Forms::RadioButton());
            this->m_radiobtn_scalecage = (gcnew System::Windows::Forms::RadioButton());
            this->m_btn_addmesh = (gcnew System::Windows::Forms::Button());
            this->m_btn_savecagetxt = (gcnew System::Windows::Forms::Button());
            this->m_btn_loadcagetxt = (gcnew System::Windows::Forms::Button());
            this->m_btn_cancel = (gcnew System::Windows::Forms::Button());
            this->m_btn_finish = (gcnew System::Windows::Forms::Button());
            this->m_btn_copytoallframes = (gcnew System::Windows::Forms::Button());
            this->m_btn_clearselection = (gcnew System::Windows::Forms::Button());
            this->m_textbox_selection = (gcnew System::Windows::Forms::TextBox());
            this->m_groupbox_selection = (gcnew System::Windows::Forms::GroupBox());
            this->m_label4 = (gcnew System::Windows::Forms::Label());
            this->m_label2 = (gcnew System::Windows::Forms::Label());
            this->m_groupbox_deformation = (gcnew System::Windows::Forms::GroupBox());
            this->label2 = (gcnew System::Windows::Forms::Label());
            this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
            this->m_btn_savegroup = (gcnew System::Windows::Forms::Button());
            this->m_btn_loadgroup = (gcnew System::Windows::Forms::Button());
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->m_btn_registgroup = (gcnew System::Windows::Forms::Button());
            this->m_grouplist = (gcnew System::Windows::Forms::DataGridView());
            this->m_cageidgroup = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
            this->m_checkbox_vissurf_trans = (gcnew System::Windows::Forms::CheckBox());
            this->m_checkbox_visbound = (gcnew System::Windows::Forms::CheckBox());
            this->m_btn_savemeshtxt = (gcnew System::Windows::Forms::Button());
            this->m_btn_savemeshobj = (gcnew System::Windows::Forms::Button());
            this->m_checkbox_fit_crssec = (gcnew System::Windows::Forms::CheckBox());
            this->m_checkbox_show_only_selected_points = (gcnew System::Windows::Forms::CheckBox());
            this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
            this->m_btn_loaddeform_nextcage = (gcnew System::Windows::Forms::Button());
            this->m_updown_cpsize = (gcnew System::Windows::Forms::NumericUpDown());
            this->m_label5 = (gcnew System::Windows::Forms::Label());
            this->m_btn_loadvisobjs = (gcnew System::Windows::Forms::Button());
            this->m_btn_clearvisobjs = (gcnew System::Windows::Forms::Button());
            this->m_checkbox_vissurf_solid = (gcnew System::Windows::Forms::CheckBox());
            this->m_checkbox_vismeshes = (gcnew System::Windows::Forms::CheckBox());
            this->m_groupbox_selection->SuspendLayout();
            this->m_groupbox_deformation->SuspendLayout();
            this->groupBox1->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_grouplist))->BeginInit();
            this->groupBox2->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_updown_cpsize))->BeginInit();
            this->SuspendLayout();
            // 
            // m_label1
            // 
            this->m_label1->AutoSize = true;
            this->m_label1->Font = (gcnew System::Drawing::Font(L"メイリオ", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_label1->Location = System::Drawing::Point(7, 17);
            this->m_label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
            this->m_label1->Name = L"m_label1";
            this->m_label1->Size = System::Drawing::Size(175, 17);
            this->m_label1->TabIndex = 6;
            this->m_label1->Text = L"Shift + L-drag : ハンドルを移動";
            // 
            // m_label3
            // 
            this->m_label3->AutoSize = true;
            this->m_label3->Font = (gcnew System::Drawing::Font(L"メイリオ", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_label3->Location = System::Drawing::Point(7, 18);
            this->m_label3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
            this->m_label3->Name = L"m_label3";
            this->m_label3->Size = System::Drawing::Size(135, 17);
            this->m_label3->TabIndex = 7;
            this->m_label3->Text = L"Shift L-click : 1点を選択";
            // 
            // m_radiobtn_rotatecage
            // 
            this->m_radiobtn_rotatecage->AutoSize = true;
            this->m_radiobtn_rotatecage->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_radiobtn_rotatecage->Location = System::Drawing::Point(66, 49);
            this->m_radiobtn_rotatecage->Margin = System::Windows::Forms::Padding(2);
            this->m_radiobtn_rotatecage->Name = L"m_radiobtn_rotatecage";
            this->m_radiobtn_rotatecage->Size = System::Drawing::Size(50, 22);
            this->m_radiobtn_rotatecage->TabIndex = 4;
            this->m_radiobtn_rotatecage->TabStop = true;
            this->m_radiobtn_rotatecage->Text = L"回転";
            this->m_radiobtn_rotatecage->UseVisualStyleBackColor = true;
            this->m_radiobtn_rotatecage->CheckedChanged += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_radiobtn_rotatecage_CheckedChanged);
            // 
            // m_radiobtn_translatecage
            // 
            this->m_radiobtn_translatecage->AutoSize = true;
            this->m_radiobtn_translatecage->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_radiobtn_translatecage->Location = System::Drawing::Point(122, 49);
            this->m_radiobtn_translatecage->Margin = System::Windows::Forms::Padding(2);
            this->m_radiobtn_translatecage->Name = L"m_radiobtn_translatecage";
            this->m_radiobtn_translatecage->Size = System::Drawing::Size(74, 22);
            this->m_radiobtn_translatecage->TabIndex = 5;
            this->m_radiobtn_translatecage->TabStop = true;
            this->m_radiobtn_translatecage->Text = L"平行移動";
            this->m_radiobtn_translatecage->UseVisualStyleBackColor = true;
            this->m_radiobtn_translatecage->CheckedChanged += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_radiobtn_translatecage_CheckedChanged);
            // 
            // m_radiobtn_scalecage
            // 
            this->m_radiobtn_scalecage->AutoSize = true;
            this->m_radiobtn_scalecage->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_radiobtn_scalecage->Location = System::Drawing::Point(6, 49);
            this->m_radiobtn_scalecage->Margin = System::Windows::Forms::Padding(2);
            this->m_radiobtn_scalecage->Name = L"m_radiobtn_scalecage";
            this->m_radiobtn_scalecage->Size = System::Drawing::Size(50, 22);
            this->m_radiobtn_scalecage->TabIndex = 14;
            this->m_radiobtn_scalecage->TabStop = true;
            this->m_radiobtn_scalecage->Text = L"拡縮";
            this->m_radiobtn_scalecage->UseVisualStyleBackColor = true;
            this->m_radiobtn_scalecage->CheckedChanged += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_radiobtn_scalecage_CheckedChanged);
            // 
            // m_btn_addmesh
            // 
            this->m_btn_addmesh->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_btn_addmesh->Location = System::Drawing::Point(4, 10);
            this->m_btn_addmesh->Margin = System::Windows::Forms::Padding(2);
            this->m_btn_addmesh->Name = L"m_btn_addmesh";
            this->m_btn_addmesh->Size = System::Drawing::Size(204, 27);
            this->m_btn_addmesh->TabIndex = 9;
            this->m_btn_addmesh->Text = L"Mesh＆Cage 読み込み";
            this->m_btn_addmesh->UseVisualStyleBackColor = true;
            this->m_btn_addmesh->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_addmesh_Click);
            // 
            // m_btn_savecagetxt
            // 
            this->m_btn_savecagetxt->Font = (gcnew System::Drawing::Font(L"メイリオ", 6.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_btn_savecagetxt->Location = System::Drawing::Point(3, 19);
            this->m_btn_savecagetxt->Margin = System::Windows::Forms::Padding(2);
            this->m_btn_savecagetxt->Name = L"m_btn_savecagetxt";
            this->m_btn_savecagetxt->Size = System::Drawing::Size(93, 24);
            this->m_btn_savecagetxt->TabIndex = 10;
            this->m_btn_savecagetxt->Text = L"Save Cage (txt)";
            this->m_btn_savecagetxt->UseVisualStyleBackColor = true;
            this->m_btn_savecagetxt->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_savecagetxt_Click);
            // 
            // m_btn_loadcagetxt
            // 
            this->m_btn_loadcagetxt->Font = (gcnew System::Drawing::Font(L"メイリオ", 6.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_btn_loadcagetxt->Location = System::Drawing::Point(100, 19);
            this->m_btn_loadcagetxt->Margin = System::Windows::Forms::Padding(2);
            this->m_btn_loadcagetxt->Name = L"m_btn_loadcagetxt";
            this->m_btn_loadcagetxt->Size = System::Drawing::Size(93, 24);
            this->m_btn_loadcagetxt->TabIndex = 11;
            this->m_btn_loadcagetxt->Text = L"Load Cage (txt)";
            this->m_btn_loadcagetxt->UseVisualStyleBackColor = true;
            this->m_btn_loadcagetxt->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_loadcagetxt_Click);
            // 
            // m_btn_cancel
            // 
            this->m_btn_cancel->Location = System::Drawing::Point(4, 399);
            this->m_btn_cancel->Margin = System::Windows::Forms::Padding(2);
            this->m_btn_cancel->Name = L"m_btn_cancel";
            this->m_btn_cancel->Size = System::Drawing::Size(73, 27);
            this->m_btn_cancel->TabIndex = 12;
            this->m_btn_cancel->Text = L"Cancel";
            this->m_btn_cancel->UseVisualStyleBackColor = true;
            this->m_btn_cancel->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_cancel_Click);
            // 
            // m_btn_finish
            // 
            this->m_btn_finish->Location = System::Drawing::Point(81, 399);
            this->m_btn_finish->Margin = System::Windows::Forms::Padding(2);
            this->m_btn_finish->Name = L"m_btn_finish";
            this->m_btn_finish->Size = System::Drawing::Size(276, 27);
            this->m_btn_finish->TabIndex = 13;
            this->m_btn_finish->Text = L"Finish and store";
            this->m_btn_finish->UseVisualStyleBackColor = true;
            this->m_btn_finish->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_finish_Click);
            // 
            // m_btn_copytoallframes
            // 
            this->m_btn_copytoallframes->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_btn_copytoallframes->Location = System::Drawing::Point(4, 80);
            this->m_btn_copytoallframes->Name = L"m_btn_copytoallframes";
            this->m_btn_copytoallframes->Size = System::Drawing::Size(178, 21);
            this->m_btn_copytoallframes->TabIndex = 16;
            this->m_btn_copytoallframes->Text = L"現在のケージ形状を全Frameにコピー";
            this->m_btn_copytoallframes->UseVisualStyleBackColor = true;
            this->m_btn_copytoallframes->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_copytoallframes_Click);
            // 
            // m_btn_clearselection
            // 
            this->m_btn_clearselection->Font = (gcnew System::Drawing::Font(L"メイリオ", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_btn_clearselection->Location = System::Drawing::Point(74, 85);
            this->m_btn_clearselection->Name = L"m_btn_clearselection";
            this->m_btn_clearselection->Size = System::Drawing::Size(122, 29);
            this->m_btn_clearselection->TabIndex = 18;
            this->m_btn_clearselection->Text = L"選択を解除 (c-key)";
            this->m_btn_clearselection->UseVisualStyleBackColor = true;
            this->m_btn_clearselection->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_clearselection_Click);
            // 
            // m_textbox_selection
            // 
            this->m_textbox_selection->Location = System::Drawing::Point(6, 86);
            this->m_textbox_selection->Name = L"m_textbox_selection";
            this->m_textbox_selection->ReadOnly = true;
            this->m_textbox_selection->Size = System::Drawing::Size(66, 27);
            this->m_textbox_selection->TabIndex = 18;
            // 
            // m_groupbox_selection
            // 
            this->m_groupbox_selection->BackColor = System::Drawing::SystemColors::Control;
            this->m_groupbox_selection->Controls->Add(this->m_btn_clearselection);
            this->m_groupbox_selection->Controls->Add(this->m_label4);
            this->m_groupbox_selection->Controls->Add(this->m_label2);
            this->m_groupbox_selection->Controls->Add(this->m_label3);
            this->m_groupbox_selection->Controls->Add(this->m_textbox_selection);
            this->m_groupbox_selection->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_groupbox_selection->Location = System::Drawing::Point(4, 42);
            this->m_groupbox_selection->Name = L"m_groupbox_selection";
            this->m_groupbox_selection->Size = System::Drawing::Size(204, 126);
            this->m_groupbox_selection->TabIndex = 17;
            this->m_groupbox_selection->TabStop = false;
            this->m_groupbox_selection->Text = L"頂点選択";
            // 
            // m_label4
            // 
            this->m_label4->AutoSize = true;
            this->m_label4->Font = (gcnew System::Drawing::Font(L"メイリオ", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_label4->Location = System::Drawing::Point(20, 51);
            this->m_label4->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
            this->m_label4->Name = L"m_label4";
            this->m_label4->Size = System::Drawing::Size(145, 34);
            this->m_label4->TabIndex = 7;
            this->m_label4->Text = L"--w/o  s-key : 追加選択\r\n--with  s-key : 新たに選択";
            // 
            // m_label2
            // 
            this->m_label2->AutoSize = true;
            this->m_label2->Font = (gcnew System::Drawing::Font(L"メイリオ", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_label2->Location = System::Drawing::Point(7, 33);
            this->m_label2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
            this->m_label2->Name = L"m_label2";
            this->m_label2->Size = System::Drawing::Size(162, 17);
            this->m_label2->TabIndex = 7;
            this->m_label2->Text = L"Shift L-drag : 矩形による選択";
            // 
            // m_groupbox_deformation
            // 
            this->m_groupbox_deformation->BackColor = System::Drawing::SystemColors::InactiveCaption;
            this->m_groupbox_deformation->Controls->Add(this->label2);
            this->m_groupbox_deformation->Controls->Add(this->m_label1);
            this->m_groupbox_deformation->Controls->Add(this->m_radiobtn_translatecage);
            this->m_groupbox_deformation->Controls->Add(this->m_radiobtn_rotatecage);
            this->m_groupbox_deformation->Controls->Add(this->m_radiobtn_scalecage);
            this->m_groupbox_deformation->Controls->Add(this->m_btn_copytoallframes);
            this->m_groupbox_deformation->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_groupbox_deformation->Location = System::Drawing::Point(4, 174);
            this->m_groupbox_deformation->Name = L"m_groupbox_deformation";
            this->m_groupbox_deformation->Size = System::Drawing::Size(204, 106);
            this->m_groupbox_deformation->TabIndex = 18;
            this->m_groupbox_deformation->TabStop = false;
            this->m_groupbox_deformation->Text = L"選択頂点の移動";
            // 
            // label2
            // 
            this->label2->AutoSize = true;
            this->label2->Font = (gcnew System::Drawing::Font(L"メイリオ", 6.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->label2->Location = System::Drawing::Point(4, 33);
            this->label2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
            this->label2->Name = L"label2";
            this->label2->Size = System::Drawing::Size(168, 14);
            this->label2->TabIndex = 6;
            this->label2->Text = L"ドラッグ中に R-click で操作キャンセル";
            // 
            // groupBox1
            // 
            this->groupBox1->BackColor = System::Drawing::SystemColors::Control;
            this->groupBox1->Controls->Add(this->m_btn_savegroup);
            this->groupBox1->Controls->Add(this->m_btn_loadgroup);
            this->groupBox1->Controls->Add(this->label1);
            this->groupBox1->Controls->Add(this->m_btn_registgroup);
            this->groupBox1->Controls->Add(this->m_grouplist);
            this->groupBox1->Font = (gcnew System::Drawing::Font(L"游ゴシック", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->groupBox1->Location = System::Drawing::Point(216, 9);
            this->groupBox1->Name = L"groupBox1";
            this->groupBox1->Size = System::Drawing::Size(140, 352);
            this->groupBox1->TabIndex = 18;
            this->groupBox1->TabStop = false;
            this->groupBox1->Text = L"Select Point Group";
            // 
            // m_btn_savegroup
            // 
            this->m_btn_savegroup->Location = System::Drawing::Point(68, 317);
            this->m_btn_savegroup->Margin = System::Windows::Forms::Padding(2);
            this->m_btn_savegroup->Name = L"m_btn_savegroup";
            this->m_btn_savegroup->Size = System::Drawing::Size(56, 22);
            this->m_btn_savegroup->TabIndex = 21;
            this->m_btn_savegroup->Text = L"Save ";
            this->m_btn_savegroup->UseVisualStyleBackColor = true;
            this->m_btn_savegroup->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_savegroup_Click);
            // 
            // m_btn_loadgroup
            // 
            this->m_btn_loadgroup->Location = System::Drawing::Point(7, 317);
            this->m_btn_loadgroup->Margin = System::Windows::Forms::Padding(2);
            this->m_btn_loadgroup->Name = L"m_btn_loadgroup";
            this->m_btn_loadgroup->Size = System::Drawing::Size(56, 22);
            this->m_btn_loadgroup->TabIndex = 20;
            this->m_btn_loadgroup->Text = L"Load ";
            this->m_btn_loadgroup->UseVisualStyleBackColor = true;
            this->m_btn_loadgroup->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_loadgroup_Click);
            // 
            // label1
            // 
            this->label1->AutoSize = true;
            this->label1->Font = (gcnew System::Drawing::Font(L"游ゴシック", 7.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->label1->Location = System::Drawing::Point(5, 284);
            this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
            this->label1->Name = L"label1";
            this->label1->Size = System::Drawing::Size(131, 28);
            this->label1->TabIndex = 19;
            this->label1->Text = L"#delete-key to delete \r\n#The group can be saved";
            // 
            // m_btn_registgroup
            // 
            this->m_btn_registgroup->Location = System::Drawing::Point(6, 19);
            this->m_btn_registgroup->Margin = System::Windows::Forms::Padding(2);
            this->m_btn_registgroup->Name = L"m_btn_registgroup";
            this->m_btn_registgroup->Size = System::Drawing::Size(128, 24);
            this->m_btn_registgroup->TabIndex = 19;
            this->m_btn_registgroup->Text = L"Regist current group";
            this->m_btn_registgroup->UseVisualStyleBackColor = true;
            this->m_btn_registgroup->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_registgroup_Click);
            // 
            // m_grouplist
            // 
            this->m_grouplist->AllowUserToAddRows = false;
            this->m_grouplist->AllowUserToDeleteRows = false;
            this->m_grouplist->AllowUserToResizeColumns = false;
            this->m_grouplist->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::AllCells;
            this->m_grouplist->AutoSizeRowsMode = System::Windows::Forms::DataGridViewAutoSizeRowsMode::AllCells;
            this->m_grouplist->BackgroundColor = System::Drawing::SystemColors::ButtonShadow;
            this->m_grouplist->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
            this->m_grouplist->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { this->m_cageidgroup });
            this->m_grouplist->Location = System::Drawing::Point(6, 47);
            this->m_grouplist->MultiSelect = false;
            this->m_grouplist->Name = L"m_grouplist";
            this->m_grouplist->ReadOnly = true;
            this->m_grouplist->RowHeadersVisible = false;
            this->m_grouplist->RowTemplate->Height = 21;
            this->m_grouplist->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
            this->m_grouplist->Size = System::Drawing::Size(128, 234);
            this->m_grouplist->TabIndex = 19;
            this->m_grouplist->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &FormSegSwallowOrgans::m_grouplist_CellContentClick);
            this->m_grouplist->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &FormSegSwallowOrgans::m_grouplist_KeyDown);
            this->m_grouplist->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &FormSegSwallowOrgans::m_grouplist_MouseDoubleClick);
            // 
            // m_cageidgroup
            // 
            this->m_cageidgroup->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
            this->m_cageidgroup->HeaderText = L"hotkey, num, name";
            this->m_cageidgroup->Name = L"m_cageidgroup";
            this->m_cageidgroup->ReadOnly = true;
            this->m_cageidgroup->Resizable = System::Windows::Forms::DataGridViewTriState::False;
            this->m_cageidgroup->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
            // 
            // m_checkbox_vissurf_trans
            // 
            this->m_checkbox_vissurf_trans->AutoSize = true;
            this->m_checkbox_vissurf_trans->Location = System::Drawing::Point(362, 108);
            this->m_checkbox_vissurf_trans->Name = L"m_checkbox_vissurf_trans";
            this->m_checkbox_vissurf_trans->Size = System::Drawing::Size(122, 16);
            this->m_checkbox_vissurf_trans->TabIndex = 20;
            this->m_checkbox_vissurf_trans->Text = L"show surface trans";
            this->m_checkbox_vissurf_trans->UseVisualStyleBackColor = true;
            this->m_checkbox_vissurf_trans->CheckedChanged += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_checkbox_vissurf_CheckedChanged);
            // 
            // m_checkbox_visbound
            // 
            this->m_checkbox_visbound->AutoSize = true;
            this->m_checkbox_visbound->Location = System::Drawing::Point(362, 44);
            this->m_checkbox_visbound->Name = L"m_checkbox_visbound";
            this->m_checkbox_visbound->Size = System::Drawing::Size(100, 16);
            this->m_checkbox_visbound->TabIndex = 21;
            this->m_checkbox_visbound->Text = L"show boundary";
            this->m_checkbox_visbound->UseVisualStyleBackColor = true;
            this->m_checkbox_visbound->CheckedChanged += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_checkbox_visbound_CheckedChanged);
            // 
            // m_btn_savemeshtxt
            // 
            this->m_btn_savemeshtxt->Enabled = false;
            this->m_btn_savemeshtxt->Font = (gcnew System::Drawing::Font(L"メイリオ", 6.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_btn_savemeshtxt->ImageAlign = System::Drawing::ContentAlignment::BottomLeft;
            this->m_btn_savemeshtxt->Location = System::Drawing::Point(100, 47);
            this->m_btn_savemeshtxt->Margin = System::Windows::Forms::Padding(2);
            this->m_btn_savemeshtxt->Name = L"m_btn_savemeshtxt";
            this->m_btn_savemeshtxt->Size = System::Drawing::Size(94, 23);
            this->m_btn_savemeshtxt->TabIndex = 10;
            this->m_btn_savemeshtxt->Text = L"Save Mesh (txt)";
            this->m_btn_savemeshtxt->UseVisualStyleBackColor = true;
            this->m_btn_savemeshtxt->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_savemeshtxt_Click);
            // 
            // m_btn_savemeshobj
            // 
            this->m_btn_savemeshobj->Font = (gcnew System::Drawing::Font(L"メイリオ", 6.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_btn_savemeshobj->Location = System::Drawing::Point(3, 47);
            this->m_btn_savemeshobj->Margin = System::Windows::Forms::Padding(2);
            this->m_btn_savemeshobj->Name = L"m_btn_savemeshobj";
            this->m_btn_savemeshobj->Size = System::Drawing::Size(93, 24);
            this->m_btn_savemeshobj->TabIndex = 10;
            this->m_btn_savemeshobj->Text = L"Save Mesh (obj)";
            this->m_btn_savemeshobj->UseVisualStyleBackColor = true;
            this->m_btn_savemeshobj->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_savemeshobj_Click);
            // 
            // m_checkbox_fit_crssec
            // 
            this->m_checkbox_fit_crssec->AutoSize = true;
            this->m_checkbox_fit_crssec->Location = System::Drawing::Point(362, 78);
            this->m_checkbox_fit_crssec->Name = L"m_checkbox_fit_crssec";
            this->m_checkbox_fit_crssec->Size = System::Drawing::Size(108, 16);
            this->m_checkbox_fit_crssec->TabIndex = 22;
            this->m_checkbox_fit_crssec->Text = L"fit cross section";
            this->m_checkbox_fit_crssec->UseVisualStyleBackColor = true;
            this->m_checkbox_fit_crssec->CheckedChanged += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_checkbox_fit_crssec_CheckedChanged);
            // 
            // m_checkbox_show_only_selected_points
            // 
            this->m_checkbox_show_only_selected_points->AutoSize = true;
            this->m_checkbox_show_only_selected_points->Location = System::Drawing::Point(362, 61);
            this->m_checkbox_show_only_selected_points->Name = L"m_checkbox_show_only_selected_points";
            this->m_checkbox_show_only_selected_points->Size = System::Drawing::Size(136, 16);
            this->m_checkbox_show_only_selected_points->TabIndex = 22;
            this->m_checkbox_show_only_selected_points->Text = L"show only selected pt";
            this->m_checkbox_show_only_selected_points->UseVisualStyleBackColor = true;
            this->m_checkbox_show_only_selected_points->CheckedChanged += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_checkbox_show_only_selected_points_CheckedChanged);
            // 
            // groupBox2
            // 
            this->groupBox2->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)), static_cast<System::Int32>(static_cast<System::Byte>(219)),
                static_cast<System::Int32>(static_cast<System::Byte>(192)));
            this->groupBox2->Controls->Add(this->m_btn_loaddeform_nextcage);
            this->groupBox2->Controls->Add(this->m_btn_savecagetxt);
            this->groupBox2->Controls->Add(this->m_btn_savemeshobj);
            this->groupBox2->Controls->Add(this->m_btn_savemeshtxt);
            this->groupBox2->Controls->Add(this->m_btn_loadcagetxt);
            this->groupBox2->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->groupBox2->Location = System::Drawing::Point(5, 288);
            this->groupBox2->Name = L"groupBox2";
            this->groupBox2->Size = System::Drawing::Size(203, 101);
            this->groupBox2->TabIndex = 23;
            this->groupBox2->TabStop = false;
            this->groupBox2->Text = L"変形情報の Save/Load";
            // 
            // m_btn_loaddeform_nextcage
            // 
            this->m_btn_loaddeform_nextcage->Font = (gcnew System::Drawing::Font(L"メイリオ", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->m_btn_loaddeform_nextcage->Location = System::Drawing::Point(2, 73);
            this->m_btn_loaddeform_nextcage->Margin = System::Windows::Forms::Padding(2);
            this->m_btn_loaddeform_nextcage->Name = L"m_btn_loaddeform_nextcage";
            this->m_btn_loaddeform_nextcage->Size = System::Drawing::Size(150, 23);
            this->m_btn_loaddeform_nextcage->TabIndex = 12;
            this->m_btn_loaddeform_nextcage->Text = L"次のケージに変形を適用";
            this->m_btn_loaddeform_nextcage->UseVisualStyleBackColor = true;
            this->m_btn_loaddeform_nextcage->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_loaddeform_nextcage_Click);
            // 
            // m_updown_cpsize
            // 
            this->m_updown_cpsize->Location = System::Drawing::Point(408, 18);
            this->m_updown_cpsize->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 20, 0, 0, 0 });
            this->m_updown_cpsize->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            this->m_updown_cpsize->Name = L"m_updown_cpsize";
            this->m_updown_cpsize->Size = System::Drawing::Size(62, 19);
            this->m_updown_cpsize->TabIndex = 24;
            this->m_updown_cpsize->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            this->m_updown_cpsize->ValueChanged += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_updown_cpsize_ValueChanged);
            // 
            // m_label5
            // 
            this->m_label5->AutoSize = true;
            this->m_label5->Location = System::Drawing::Point(361, 20);
            this->m_label5->Name = L"m_label5";
            this->m_label5->Size = System::Drawing::Size(41, 12);
            this->m_label5->TabIndex = 25;
            this->m_label5->Text = L"cp size";
            // 
            // m_btn_loadvisobjs
            // 
            this->m_btn_loadvisobjs->Location = System::Drawing::Point(362, 164);
            this->m_btn_loadvisobjs->Margin = System::Windows::Forms::Padding(2);
            this->m_btn_loadvisobjs->Name = L"m_btn_loadvisobjs";
            this->m_btn_loadvisobjs->Size = System::Drawing::Size(91, 24);
            this->m_btn_loadvisobjs->TabIndex = 22;
            this->m_btn_loadvisobjs->Text = L"Load Vis Objs";
            this->m_btn_loadvisobjs->UseVisualStyleBackColor = true;
            this->m_btn_loadvisobjs->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_loadvisobjs_Click);
            // 
            // m_btn_clearvisobjs
            // 
            this->m_btn_clearvisobjs->Location = System::Drawing::Point(362, 192);
            this->m_btn_clearvisobjs->Margin = System::Windows::Forms::Padding(2);
            this->m_btn_clearvisobjs->Name = L"m_btn_clearvisobjs";
            this->m_btn_clearvisobjs->Size = System::Drawing::Size(91, 24);
            this->m_btn_clearvisobjs->TabIndex = 26;
            this->m_btn_clearvisobjs->Text = L"Clear Vis Objs";
            this->m_btn_clearvisobjs->UseVisualStyleBackColor = true;
            this->m_btn_clearvisobjs->Click += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_btn_clearvisobjs_Click);
            // 
            // m_checkbox_vissurf_solid
            // 
            this->m_checkbox_vissurf_solid->AutoSize = true;
            this->m_checkbox_vissurf_solid->Location = System::Drawing::Point(362, 124);
            this->m_checkbox_vissurf_solid->Name = L"m_checkbox_vissurf_solid";
            this->m_checkbox_vissurf_solid->Size = System::Drawing::Size(120, 16);
            this->m_checkbox_vissurf_solid->TabIndex = 27;
            this->m_checkbox_vissurf_solid->Text = L"show surface solid";
            this->m_checkbox_vissurf_solid->UseVisualStyleBackColor = true;
            this->m_checkbox_vissurf_solid->CheckedChanged += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_checkbox_vissurf_solid_CheckedChanged);
            // 
            // m_checkbox_vismeshes
            // 
            this->m_checkbox_vismeshes->AutoSize = true;
            this->m_checkbox_vismeshes->Location = System::Drawing::Point(362, 218);
            this->m_checkbox_vismeshes->Name = L"m_checkbox_vismeshes";
            this->m_checkbox_vismeshes->Size = System::Drawing::Size(112, 16);
            this->m_checkbox_vismeshes->TabIndex = 28;
            this->m_checkbox_vismeshes->Text = L"show vis meshes";
            this->m_checkbox_vismeshes->UseVisualStyleBackColor = true;
            this->m_checkbox_vismeshes->CheckedChanged += gcnew System::EventHandler(this, &FormSegSwallowOrgans::m_checkbox_vismeshes_CheckedChanged);
            // 
            // FormSegSwallowOrgans
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(495, 432);
            this->ControlBox = false;
            this->Controls->Add(this->m_checkbox_vismeshes);
            this->Controls->Add(this->m_checkbox_vissurf_solid);
            this->Controls->Add(this->m_btn_clearvisobjs);
            this->Controls->Add(this->m_btn_loadvisobjs);
            this->Controls->Add(this->m_label5);
            this->Controls->Add(this->m_updown_cpsize);
            this->Controls->Add(this->groupBox2);
            this->Controls->Add(this->m_checkbox_show_only_selected_points);
            this->Controls->Add(this->m_checkbox_fit_crssec);
            this->Controls->Add(this->m_checkbox_visbound);
            this->Controls->Add(this->m_checkbox_vissurf_trans);
            this->Controls->Add(this->groupBox1);
            this->Controls->Add(this->m_groupbox_deformation);
            this->Controls->Add(this->m_groupbox_selection);
            this->Controls->Add(this->m_btn_finish);
            this->Controls->Add(this->m_btn_cancel);
            this->Controls->Add(this->m_btn_addmesh);
            this->Margin = System::Windows::Forms::Padding(2);
            this->Name = L"FormSegSwallowOrgans";
            this->Text = L"Mode Segm Swallowing Organs";
            this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &FormSegSwallowOrgans::FormSegSwallowOrgans_KeyDown);
            this->m_groupbox_selection->ResumeLayout(false);
            this->m_groupbox_selection->PerformLayout();
            this->m_groupbox_deformation->ResumeLayout(false);
            this->m_groupbox_deformation->PerformLayout();
            this->groupBox1->ResumeLayout(false);
            this->groupBox1->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_grouplist))->EndInit();
            this->groupBox2->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_updown_cpsize))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
  private: 
    System::Void m_radiobtn_rotatecage_CheckedChanged   (System::Object^  sender, System::EventArgs^  e);
    System::Void m_radiobtn_translatecage_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_radiobtn_scalecage_CheckedChanged    (System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_btn_addmesh_Click     (System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_btn_cancel_Click      (System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_btn_finish_Click      (System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_btn_savecagetxt_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_btn_loadcagetxt_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_btn_savemeshtxt_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_btn_savemeshobj_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_btn_savegroup_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void m_btn_copytoallframes_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_btn_clearselection_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_btn_registgroup_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_btn_loadgroup_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_btn_loaddeform_nextcage_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void m_grouplist_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e);
    System::Void m_grouplist_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) ;
    System::Void m_grouplist_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) ;
    System::Void FormSegSwallowOrgans_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) ;
    System::Void m_checkbox_vissurf_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_checkbox_visbound_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_checkbox_fit_crssec_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
    System::Void m_checkbox_show_only_selected_points_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
    System::Void m_updown_cpsize_ValueChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_btn_loadvisobjs_Click(System::Object^  sender, System::EventArgs^ e);
    System::Void m_btn_clearvisobjs_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void m_checkbox_vissurf_solid_CheckedChanged(System::Object^ sender, System::EventArgs^ e) ;
    System::Void m_checkbox_vismeshes_CheckedChanged(System::Object^ sender, System::EventArgs^ e) ;

};

  inline void FormSegSwallowOrgans_Show() { 
    FormSegSwallowOrgans::GetInst()->Show(); 
  }
  inline void FormSegSwallowOrgans_Hide() { 
    FormSegSwallowOrgans::GetInst()->Hide(); 
  }
  inline void FormSegSwallowOrgans_InitAllItems() { 
    FormSegSwallowOrgans::GetInst()->InitAllItems(); 
  }
  inline void FormSegSwallowOrgans_UpdateList(){
    FormSegSwallowOrgans::GetInst()->UpdateList();
  }

  inline void FormSegSwallowOrgans_SetNumSelectVtx(int num_select_vtx){
    FormSegSwallowOrgans::GetInst()->SetNumSelectVtx(num_select_vtx);
  }

  inline void FormSegSwallowOrgans_FitCrssecToSelectedCageVtx(){
    FormSegSwallowOrgans::GetInst()->FitCrssecToSelectedCageVtx();
  }

  //ui flags
  inline bool FormSegSwallowOrgans_IsModeRotate    () { 
    return FormSegSwallowOrgans::GetInst()->IsModeRotate   (); 
  }
  
  inline bool FormSegSwallowOrgans_IsModeTranslate () { 
    return FormSegSwallowOrgans::GetInst()->IsModeTranslate(); 
  }
  
  inline bool FormSegSwallowOrgans_IsModeScale     () { 
    return FormSegSwallowOrgans::GetInst()->IsModeScale    (); 
  }

  inline bool FormSegSwallowOrgans_bVisSurfTrans() { 
    return FormSegSwallowOrgans::GetInst()->DoVisSurfTrans();
  }

  inline bool FormSegSwallowOrgans_bVisSurfSolid() {
    return FormSegSwallowOrgans::GetInst()->DoVisSurfSolid();
  }

  inline bool FormSegSwallowOrgans_bVisMeshes() {
    return FormSegSwallowOrgans::GetInst()->DoVisMeshes();
  }

  inline bool FormSegSwallowOrgans_bVisBound() { 
    return FormSegSwallowOrgans::GetInst()->DoVisBound(); 
  }

  inline bool FormSegSwallowOrgans_bFitCrssec() { 
    return FormSegSwallowOrgans::GetInst()->DoFitCrssec(); 
  }
  
  inline bool FormSegSwallowOrgans_bShowOnlySelectedPts() { 
    return FormSegSwallowOrgans::GetInst()->DoShowOnlySelectedPts(); 
  }

  inline int  FormSegSwqllowOrgans_GetCpSize(){ 
    return FormSegSwallowOrgans::GetInst()->GetCpSize(); 
  }
}
