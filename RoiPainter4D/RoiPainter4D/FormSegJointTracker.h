#pragma once

namespace RoiPainter4D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegJointTracker の概要
	/// </summary>
	public ref class FormSegJointTracker : public System::Windows::Forms::Form
	{

  private: 
    static FormSegJointTracker^ m_singleton;

		FormSegJointTracker(void)
		{
			InitializeComponent();
			//
			//TODO: ここにコンストラクター コードを追加します
			//
		}

  public:
    void InitializeItems(int volminv, int volmaxv);

  public:
    static FormSegJointTracker^ GetInst() {
      if (m_singleton == nullptr) m_singleton = gcnew FormSegJointTracker();
      return m_singleton;
    }


	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormSegJointTracker()
		{
			if (components)
			{
				delete components;
			}
		}



  private: System::Windows::Forms::Label^  m_label0;
  private: System::Windows::Forms::Label^  m_label1;
  private: System::Windows::Forms::Label^  m_label2;
  private: System::Windows::Forms::Label^  m_label3;
  private: System::Windows::Forms::Label^  m_label4;
  private: System::Windows::Forms::Label^  m_label5;
  private: System::Windows::Forms::Label^  m_label6;

  private: System::Windows::Forms::GroupBox^  m_groupBox_manipmode;
  private: System::Windows::Forms::GroupBox^  m_groupbox_isosurface;
  private: System::Windows::Forms::GroupBox^  m_groupBox_runicp;
  private: System::Windows::Forms::GroupBox^  m_groupBox_analysis;

  private: System::Windows::Forms::RadioButton^  m_mode_placecp_bone1;
  private: System::Windows::Forms::RadioButton^  m_mode_placecp_bone2;
  private: System::Windows::Forms::RadioButton^  m_mode_rotbone1;
  private: System::Windows::Forms::RadioButton^  m_mode_transbone1;
  private: System::Windows::Forms::RadioButton^  m_mode_rotbone2;
  private: System::Windows::Forms::RadioButton^  m_mode_transbone2;
  private: System::Windows::Forms::RadioButton^  m_mode_rotobj;
  private: System::Windows::Forms::RadioButton^  m_mode_transobj;

  private: System::Windows::Forms::NumericUpDown^  m_updown_icplv;
  private: System::Windows::Forms::NumericUpDown^  m_updown_rejectscale;
  private: System::Windows::Forms::NumericUpDown^  m_updown_isovalue;

  private: System::Windows::Forms::Button^  m_button_genisosurf;
  private: System::Windows::Forms::Button^  m_button_runicp;
  private: System::Windows::Forms::Button^  m_button_exportcps;
  private: System::Windows::Forms::Button^  m_button_importcps;
  private: System::Windows::Forms::Button^  m_button_loadobj;
  private: System::Windows::Forms::Button^  m_button_runicp_curronly;

  private: System::Windows::Forms::CheckBox^  m_bvis_cps;
  private: System::Windows::Forms::CheckBox^  m_bvis_cptrajectory;
  private: System::Windows::Forms::CheckBox^  m_bvis_isosurface;
  private: System::Windows::Forms::CheckBox^  m_bvis_fitsurface;
  private: System::Windows::Forms::CheckBox^  m_bvis_evalsurface;
  private: System::Windows::Forms::CheckBox^  m_bvis_evaldiff;
  private: System::Windows::Forms::NumericUpDown^  m_updown_trajectorycpid;

  private: System::Windows::Forms::Button^  m_button_fitobjcalcerror;

public:
    bool bModePlaceCPBone1(){ return m_mode_placecp_bone1->Checked; }
    bool bModePlaceCPBone2(){ return m_mode_placecp_bone2->Checked; }
    bool bModeTransBone1()  { return m_mode_transbone1->Checked; }
    bool bModeTransBone2()  { return m_mode_transbone2->Checked; }
    bool bModeTransObj ()   { return m_mode_transobj  ->Checked; }
    bool bModeRotBone1()    { return m_mode_rotbone1  ->Checked; }
    bool bModeRotBone2()    { return m_mode_rotbone2  ->Checked; }
    bool bModeRotObj()      { return m_mode_rotobj    ->Checked; }


    bool bVisCps(){ return m_bvis_cps->Checked;}
    bool bVisCpTrajecory(){ return m_bvis_cptrajectory->Checked;}
    bool bVisIsoSurface() { return m_bvis_isosurface->Checked;} 
    bool bVisFitSurface() { return m_bvis_fitsurface->Checked;}
    bool bVisEvalSurf()   { return m_bvis_evalsurface->Checked;}
    bool bVisEvalDiff()   { return m_bvis_evaldiff->Checked ;} 
    int  GetVisTrajCpid() { return (int)m_updown_trajectorycpid->Value;}

    void SetModePlaceCPBone2(){
      m_mode_placecp_bone1->Checked = false;
      m_mode_placecp_bone2->Checked = true;
      m_mode_transbone1->Checked = false;
      m_mode_transbone2->Checked = false;
      m_mode_transobj->Checked = false;
      m_mode_rotbone1->Checked = false;
      m_mode_rotbone2->Checked = false;
      m_mode_rotobj->Checked = false;
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
      this->m_groupbox_isosurface = (gcnew System::Windows::Forms::GroupBox());
      this->m_updown_isovalue = (gcnew System::Windows::Forms::NumericUpDown());
      this->m_button_genisosurf = (gcnew System::Windows::Forms::Button());
      this->m_label3 = (gcnew System::Windows::Forms::Label());
      this->m_groupBox_manipmode = (gcnew System::Windows::Forms::GroupBox());
      this->m_label0 = (gcnew System::Windows::Forms::Label());
      this->m_mode_rotobj = (gcnew System::Windows::Forms::RadioButton());
      this->m_mode_transobj = (gcnew System::Windows::Forms::RadioButton());
      this->m_label2 = (gcnew System::Windows::Forms::Label());
      this->m_label1 = (gcnew System::Windows::Forms::Label());
      this->m_mode_rotbone2 = (gcnew System::Windows::Forms::RadioButton());
      this->m_mode_transbone2 = (gcnew System::Windows::Forms::RadioButton());
      this->m_mode_rotbone1 = (gcnew System::Windows::Forms::RadioButton());
      this->m_mode_transbone1 = (gcnew System::Windows::Forms::RadioButton());
      this->m_mode_placecp_bone2 = (gcnew System::Windows::Forms::RadioButton());
      this->m_mode_placecp_bone1 = (gcnew System::Windows::Forms::RadioButton());
      this->m_groupBox_runicp = (gcnew System::Windows::Forms::GroupBox());
      this->m_button_runicp_curronly = (gcnew System::Windows::Forms::Button());
      this->m_label6 = (gcnew System::Windows::Forms::Label());
      this->m_label5 = (gcnew System::Windows::Forms::Label());
      this->m_button_runicp = (gcnew System::Windows::Forms::Button());
      this->m_updown_icplv = (gcnew System::Windows::Forms::NumericUpDown());
      this->m_updown_rejectscale = (gcnew System::Windows::Forms::NumericUpDown());
      this->m_label4 = (gcnew System::Windows::Forms::Label());
      this->m_groupBox_analysis = (gcnew System::Windows::Forms::GroupBox());
      this->m_button_fitobjcalcerror = (gcnew System::Windows::Forms::Button());
      this->m_button_importcps = (gcnew System::Windows::Forms::Button());
      this->m_button_loadobj = (gcnew System::Windows::Forms::Button());
      this->m_button_exportcps = (gcnew System::Windows::Forms::Button());
      this->m_bvis_cps = (gcnew System::Windows::Forms::CheckBox());
      this->m_bvis_cptrajectory = (gcnew System::Windows::Forms::CheckBox());
      this->m_updown_trajectorycpid = (gcnew System::Windows::Forms::NumericUpDown());
      this->m_bvis_isosurface = (gcnew System::Windows::Forms::CheckBox());
      this->m_bvis_fitsurface = (gcnew System::Windows::Forms::CheckBox());
      this->m_bvis_evalsurface = (gcnew System::Windows::Forms::CheckBox());
      this->m_bvis_evaldiff = (gcnew System::Windows::Forms::CheckBox());
      this->m_groupbox_isosurface->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_updown_isovalue))->BeginInit();
      this->m_groupBox_manipmode->SuspendLayout();
      this->m_groupBox_runicp->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_updown_icplv))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_updown_rejectscale))->BeginInit();
      this->m_groupBox_analysis->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_updown_trajectorycpid))->BeginInit();
      this->SuspendLayout();
      // 
      // m_groupbox_isosurface
      // 
      this->m_groupbox_isosurface->Controls->Add(this->m_updown_isovalue);
      this->m_groupbox_isosurface->Controls->Add(this->m_button_genisosurf);
      this->m_groupbox_isosurface->Controls->Add(this->m_label3);
      this->m_groupbox_isosurface->Location = System::Drawing::Point(191, 3);
      this->m_groupbox_isosurface->Name = L"m_groupbox_isosurface";
      this->m_groupbox_isosurface->Size = System::Drawing::Size(205, 70);
      this->m_groupbox_isosurface->TabIndex = 6;
      this->m_groupbox_isosurface->TabStop = false;
      this->m_groupbox_isosurface->Text = L"Step1 - Iso Surface Generation";
      // 
      // m_updown_isovalue
      // 
      this->m_updown_isovalue->Location = System::Drawing::Point(64, 19);
      this->m_updown_isovalue->Name = L"m_updown_isovalue";
      this->m_updown_isovalue->Size = System::Drawing::Size(68, 19);
      this->m_updown_isovalue->TabIndex = 15;
      this->m_updown_isovalue->ValueChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_updown_isovalue_ValueChanged);
      // 
      // m_button_genisosurf
      // 
      this->m_button_genisosurf->Location = System::Drawing::Point(5, 41);
      this->m_button_genisosurf->Name = L"m_button_genisosurf";
      this->m_button_genisosurf->Size = System::Drawing::Size(187, 23);
      this->m_button_genisosurf->TabIndex = 10;
      this->m_button_genisosurf->Text = L"Generate Iso Surfaces";
      this->m_button_genisosurf->UseVisualStyleBackColor = true;
      this->m_button_genisosurf->Click += gcnew System::EventHandler(this, &FormSegJointTracker::m_button_genisosurf_Click);
      // 
      // m_label3
      // 
      this->m_label3->AutoSize = true;
      this->m_label3->Location = System::Drawing::Point(7, 21);
      this->m_label3->Name = L"m_label3";
      this->m_label3->Size = System::Drawing::Size(51, 12);
      this->m_label3->TabIndex = 6;
      this->m_label3->Text = L"iso value";
      // 
      // m_groupBox_manipmode
      // 
      this->m_groupBox_manipmode->Controls->Add(this->m_label0);
      this->m_groupBox_manipmode->Controls->Add(this->m_mode_rotobj);
      this->m_groupBox_manipmode->Controls->Add(this->m_mode_transobj);
      this->m_groupBox_manipmode->Controls->Add(this->m_label2);
      this->m_groupBox_manipmode->Controls->Add(this->m_label1);
      this->m_groupBox_manipmode->Controls->Add(this->m_mode_rotbone2);
      this->m_groupBox_manipmode->Controls->Add(this->m_mode_transbone2);
      this->m_groupBox_manipmode->Controls->Add(this->m_mode_rotbone1);
      this->m_groupBox_manipmode->Controls->Add(this->m_mode_transbone1);
      this->m_groupBox_manipmode->Controls->Add(this->m_mode_placecp_bone2);
      this->m_groupBox_manipmode->Controls->Add(this->m_mode_placecp_bone1);
      this->m_groupBox_manipmode->Location = System::Drawing::Point(5, 3);
      this->m_groupBox_manipmode->Name = L"m_groupBox_manipmode";
      this->m_groupBox_manipmode->Size = System::Drawing::Size(178, 166);
      this->m_groupBox_manipmode->TabIndex = 7;
      this->m_groupBox_manipmode->TabStop = false;
      this->m_groupBox_manipmode->Text = L"Manipuration Modes";
      // 
      // m_label0
      // 
      this->m_label0->AutoSize = true;
      this->m_label0->Location = System::Drawing::Point(5, 19);
      this->m_label0->Name = L"m_label0";
      this->m_label0->Size = System::Drawing::Size(72, 12);
      this->m_label0->TabIndex = 10;
      this->m_label0->Text = L"Control Point";
      // 
      // m_mode_rotobj
      // 
      this->m_mode_rotobj->AutoSize = true;
      this->m_mode_rotobj->Location = System::Drawing::Point(94, 139);
      this->m_mode_rotobj->Name = L"m_mode_rotobj";
      this->m_mode_rotobj->Size = System::Drawing::Size(68, 16);
      this->m_mode_rotobj->TabIndex = 9;
      this->m_mode_rotobj->TabStop = true;
      this->m_mode_rotobj->Text = L"rot mesh";
      this->m_mode_rotobj->UseVisualStyleBackColor = true;
      this->m_mode_rotobj->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_mode_rotobj_CheckedChanged);
      // 
      // m_mode_transobj
      // 
      this->m_mode_transobj->AutoSize = true;
      this->m_mode_transobj->Location = System::Drawing::Point(6, 139);
      this->m_mode_transobj->Name = L"m_mode_transobj";
      this->m_mode_transobj->Size = System::Drawing::Size(80, 16);
      this->m_mode_transobj->TabIndex = 8;
      this->m_mode_transobj->TabStop = true;
      this->m_mode_transobj->Text = L"trans mesh";
      this->m_mode_transobj->UseVisualStyleBackColor = true;
      this->m_mode_transobj->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_mode_transobj_CheckedChanged);
      // 
      // m_label2
      // 
      this->m_label2->AutoSize = true;
      this->m_label2->Location = System::Drawing::Point(4, 126);
      this->m_label2->Name = L"m_label2";
      this->m_label2->Size = System::Drawing::Size(167, 12);
      this->m_label2->TabIndex = 7;
      this->m_label2->Text = L"Edit loaded mesh (Shift+L drag)";
      // 
      // m_label1
      // 
      this->m_label1->AutoSize = true;
      this->m_label1->Location = System::Drawing::Point(4, 70);
      this->m_label1->Name = L"m_label1";
      this->m_label1->Size = System::Drawing::Size(157, 12);
      this->m_label1->TabIndex = 6;
      this->m_label1->Text = L"Edit fit results  (Shift+L drag)";
      // 
      // m_mode_rotbone2
      // 
      this->m_mode_rotbone2->AutoSize = true;
      this->m_mode_rotbone2->Location = System::Drawing::Point(94, 102);
      this->m_mode_rotbone2->Name = L"m_mode_rotbone2";
      this->m_mode_rotbone2->Size = System::Drawing::Size(71, 16);
      this->m_mode_rotbone2->TabIndex = 5;
      this->m_mode_rotbone2->TabStop = true;
      this->m_mode_rotbone2->Text = L"rot bone2";
      this->m_mode_rotbone2->UseVisualStyleBackColor = true;
      this->m_mode_rotbone2->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_mode_rotbone2_CheckedChanged);
      // 
      // m_mode_transbone2
      // 
      this->m_mode_transbone2->AutoSize = true;
      this->m_mode_transbone2->Location = System::Drawing::Point(7, 102);
      this->m_mode_transbone2->Name = L"m_mode_transbone2";
      this->m_mode_transbone2->Size = System::Drawing::Size(83, 16);
      this->m_mode_transbone2->TabIndex = 4;
      this->m_mode_transbone2->TabStop = true;
      this->m_mode_transbone2->Text = L"trans bone2";
      this->m_mode_transbone2->UseVisualStyleBackColor = true;
      this->m_mode_transbone2->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_mode_transbone2_CheckedChanged);
      // 
      // m_mode_rotbone1
      // 
      this->m_mode_rotbone1->AutoSize = true;
      this->m_mode_rotbone1->Location = System::Drawing::Point(94, 84);
      this->m_mode_rotbone1->Name = L"m_mode_rotbone1";
      this->m_mode_rotbone1->Size = System::Drawing::Size(71, 16);
      this->m_mode_rotbone1->TabIndex = 3;
      this->m_mode_rotbone1->TabStop = true;
      this->m_mode_rotbone1->Text = L"rot bone1";
      this->m_mode_rotbone1->UseVisualStyleBackColor = true;
      this->m_mode_rotbone1->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_mode_rotbone1_CheckedChanged);
      // 
      // m_mode_transbone1
      // 
      this->m_mode_transbone1->AutoSize = true;
      this->m_mode_transbone1->Location = System::Drawing::Point(7, 84);
      this->m_mode_transbone1->Name = L"m_mode_transbone1";
      this->m_mode_transbone1->Size = System::Drawing::Size(83, 16);
      this->m_mode_transbone1->TabIndex = 2;
      this->m_mode_transbone1->TabStop = true;
      this->m_mode_transbone1->Text = L"trans bone1";
      this->m_mode_transbone1->UseVisualStyleBackColor = true;
      this->m_mode_transbone1->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_mode_transbone1_CheckedChanged);
      // 
      // m_mode_placecp_bone2
      // 
      this->m_mode_placecp_bone2->AutoSize = true;
      this->m_mode_placecp_bone2->Location = System::Drawing::Point(8, 48);
      this->m_mode_placecp_bone2->Name = L"m_mode_placecp_bone2";
      this->m_mode_placecp_bone2->Size = System::Drawing::Size(106, 16);
      this->m_mode_placecp_bone2->TabIndex = 1;
      this->m_mode_placecp_bone2->TabStop = true;
      this->m_mode_placecp_bone2->Text = L"place cps bone2";
      this->m_mode_placecp_bone2->UseVisualStyleBackColor = true;
      this->m_mode_placecp_bone2->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_mode_placecp_bone2_CheckedChanged);
      // 
      // m_mode_placecp_bone1
      // 
      this->m_mode_placecp_bone1->AutoSize = true;
      this->m_mode_placecp_bone1->Location = System::Drawing::Point(8, 32);
      this->m_mode_placecp_bone1->Name = L"m_mode_placecp_bone1";
      this->m_mode_placecp_bone1->Size = System::Drawing::Size(106, 16);
      this->m_mode_placecp_bone1->TabIndex = 0;
      this->m_mode_placecp_bone1->TabStop = true;
      this->m_mode_placecp_bone1->Text = L"place cps bone1";
      this->m_mode_placecp_bone1->UseVisualStyleBackColor = true;
      this->m_mode_placecp_bone1->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_mode_placecp_bone1_CheckedChanged);
      // 
      // m_groupBox_runicp
      // 
      this->m_groupBox_runicp->Controls->Add(this->m_button_runicp_curronly);
      this->m_groupBox_runicp->Controls->Add(this->m_label6);
      this->m_groupBox_runicp->Controls->Add(this->m_label5);
      this->m_groupBox_runicp->Controls->Add(this->m_button_runicp);
      this->m_groupBox_runicp->Controls->Add(this->m_updown_icplv);
      this->m_groupBox_runicp->Controls->Add(this->m_updown_rejectscale);
      this->m_groupBox_runicp->Controls->Add(this->m_label4);
      this->m_groupBox_runicp->Location = System::Drawing::Point(192, 79);
      this->m_groupBox_runicp->Name = L"m_groupBox_runicp";
      this->m_groupBox_runicp->Size = System::Drawing::Size(203, 142);
      this->m_groupBox_runicp->TabIndex = 8;
      this->m_groupBox_runicp->TabStop = false;
      this->m_groupBox_runicp->Text = L"Step2 - Run ICP rigid fitting ";
      // 
      // m_button_runicp_curronly
      // 
      this->m_button_runicp_curronly->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 8.25F, System::Drawing::FontStyle::Regular,
        System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(128)));
      this->m_button_runicp_curronly->Location = System::Drawing::Point(100, 107);
      this->m_button_runicp_curronly->Name = L"m_button_runicp_curronly";
      this->m_button_runicp_curronly->Size = System::Drawing::Size(89, 31);
      this->m_button_runicp_curronly->TabIndex = 15;
      this->m_button_runicp_curronly->Text = L"Run ICP             only this frame";
      this->m_button_runicp_curronly->UseVisualStyleBackColor = true;
      this->m_button_runicp_curronly->Click += gcnew System::EventHandler(this, &FormSegJointTracker::m_button_runicp_curronly_Click);
      // 
      // m_label6
      // 
      this->m_label6->AutoSize = true;
      this->m_label6->Location = System::Drawing::Point(91, 68);
      this->m_label6->Name = L"m_label6";
      this->m_label6->Size = System::Drawing::Size(39, 12);
      this->m_label6->TabIndex = 14;
      this->m_label6->Text = L"ICP Lv";
      // 
      // m_label5
      // 
      this->m_label5->AutoSize = true;
      this->m_label5->Location = System::Drawing::Point(10, 68);
      this->m_label5->Name = L"m_label5";
      this->m_label5->Size = System::Drawing::Size(69, 12);
      this->m_label5->TabIndex = 14;
      this->m_label5->Text = L"Reject scale";
      // 
      // m_button_runicp
      // 
      this->m_button_runicp->Location = System::Drawing::Point(8, 109);
      this->m_button_runicp->Name = L"m_button_runicp";
      this->m_button_runicp->Size = System::Drawing::Size(88, 28);
      this->m_button_runicp->TabIndex = 13;
      this->m_button_runicp->Text = L"Run ICP ALL";
      this->m_button_runicp->UseVisualStyleBackColor = true;
      this->m_button_runicp->Click += gcnew System::EventHandler(this, &FormSegJointTracker::m_button_runicp_Click);
      // 
      // m_updown_icplv
      // 
      this->m_updown_icplv->Location = System::Drawing::Point(90, 84);
      this->m_updown_icplv->Name = L"m_updown_icplv";
      this->m_updown_icplv->Size = System::Drawing::Size(45, 19);
      this->m_updown_icplv->TabIndex = 12;
      this->m_updown_icplv->ValueChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_updown_icplv_ValueChanged);
      // 
      // m_updown_rejectscale
      // 
      this->m_updown_rejectscale->Location = System::Drawing::Point(10, 84);
      this->m_updown_rejectscale->Name = L"m_updown_rejectscale";
      this->m_updown_rejectscale->Size = System::Drawing::Size(57, 19);
      this->m_updown_rejectscale->TabIndex = 11;
      this->m_updown_rejectscale->ValueChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_updown_rejectscale_ValueChanged);
      // 
      // m_label4
      // 
      this->m_label4->AutoSize = true;
      this->m_label4->Location = System::Drawing::Point(5, 16);
      this->m_label4->Name = L"m_label4";
      this->m_label4->Size = System::Drawing::Size(197, 48);
      this->m_label4->TabIndex = 10;
      this->m_label4->Text = L"1. Place cp for bone 1 and bone 2\r\n    - Shift+Ldblclk: activate iso surf\r\n    - "
        L"Lclick         : add cp on bone12 \r\n2. run ICP";
      // 
      // m_groupBox_analysis
      // 
      this->m_groupBox_analysis->Controls->Add(this->m_button_fitobjcalcerror);
      this->m_groupBox_analysis->Controls->Add(this->m_button_importcps);
      this->m_groupBox_analysis->Controls->Add(this->m_button_loadobj);
      this->m_groupBox_analysis->Controls->Add(this->m_button_exportcps);
      this->m_groupBox_analysis->Location = System::Drawing::Point(191, 228);
      this->m_groupBox_analysis->Name = L"m_groupBox_analysis";
      this->m_groupBox_analysis->Size = System::Drawing::Size(205, 103);
      this->m_groupBox_analysis->TabIndex = 9;
      this->m_groupBox_analysis->TabStop = false;
      this->m_groupBox_analysis->Text = L"Step3. Analysis";
      // 
      // m_button_fitobjcalcerror
      // 
      this->m_button_fitobjcalcerror->Location = System::Drawing::Point(5, 78);
      this->m_button_fitobjcalcerror->Name = L"m_button_fitobjcalcerror";
      this->m_button_fitobjcalcerror->Size = System::Drawing::Size(146, 22);
      this->m_button_fitobjcalcerror->TabIndex = 16;
      this->m_button_fitobjcalcerror->Text = L"Fit Obj and compute error ";
      this->m_button_fitobjcalcerror->UseVisualStyleBackColor = true;
      this->m_button_fitobjcalcerror->Click += gcnew System::EventHandler(this, &FormSegJointTracker::m_button_fitobjcalcerror_Click);
      // 
      // m_button_importcps
      // 
      this->m_button_importcps->Location = System::Drawing::Point(109, 18);
      this->m_button_importcps->Name = L"m_button_importcps";
      this->m_button_importcps->Size = System::Drawing::Size(89, 22);
      this->m_button_importcps->TabIndex = 15;
      this->m_button_importcps->Text = L"ImportCPs";
      this->m_button_importcps->UseVisualStyleBackColor = true;
      this->m_button_importcps->Click += gcnew System::EventHandler(this, &FormSegJointTracker::m_button_importcps_Click);
      // 
      // m_button_loadobj
      // 
      this->m_button_loadobj->Location = System::Drawing::Point(5, 54);
      this->m_button_loadobj->Name = L"m_button_loadobj";
      this->m_button_loadobj->Size = System::Drawing::Size(146, 22);
      this->m_button_loadobj->TabIndex = 15;
      this->m_button_loadobj->Text = L"Load Obj for calc error";
      this->m_button_loadobj->UseVisualStyleBackColor = true;
      this->m_button_loadobj->Click += gcnew System::EventHandler(this, &FormSegJointTracker::m_button_loadobj_Click);
      // 
      // m_button_exportcps
      // 
      this->m_button_exportcps->Location = System::Drawing::Point(6, 18);
      this->m_button_exportcps->Name = L"m_button_exportcps";
      this->m_button_exportcps->Size = System::Drawing::Size(99, 22);
      this->m_button_exportcps->TabIndex = 15;
      this->m_button_exportcps->Text = L"ExportCPs";
      this->m_button_exportcps->UseVisualStyleBackColor = true;
      this->m_button_exportcps->Click += gcnew System::EventHandler(this, &FormSegJointTracker::m_button_exportcps_Click);
      // 
      // m_bvis_cps
      // 
      this->m_bvis_cps->AutoSize = true;
      this->m_bvis_cps->Checked = true;
      this->m_bvis_cps->CheckState = System::Windows::Forms::CheckState::Checked;
      this->m_bvis_cps->Location = System::Drawing::Point(5, 211);
      this->m_bvis_cps->Name = L"m_bvis_cps";
      this->m_bvis_cps->Size = System::Drawing::Size(66, 16);
      this->m_bvis_cps->TabIndex = 10;
      this->m_bvis_cps->Text = L"Vis CPs";
      this->m_bvis_cps->UseVisualStyleBackColor = true;
      this->m_bvis_cps->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_bvis_cps_CheckedChanged);
      // 
      // m_bvis_cptrajectory
      // 
      this->m_bvis_cptrajectory->AutoSize = true;
      this->m_bvis_cptrajectory->Checked = true;
      this->m_bvis_cptrajectory->CheckState = System::Windows::Forms::CheckState::Checked;
      this->m_bvis_cptrajectory->Location = System::Drawing::Point(5, 230);
      this->m_bvis_cptrajectory->Name = L"m_bvis_cptrajectory";
      this->m_bvis_cptrajectory->Size = System::Drawing::Size(116, 16);
      this->m_bvis_cptrajectory->TabIndex = 11;
      this->m_bvis_cptrajectory->Text = L"Vis CP Trajectory";
      this->m_bvis_cptrajectory->UseVisualStyleBackColor = true;
      this->m_bvis_cptrajectory->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_bvis_cptrajectory_CheckedChanged);
      // 
      // m_updown_trajectorycpid
      // 
      this->m_updown_trajectorycpid->Location = System::Drawing::Point(6, 247);
      this->m_updown_trajectorycpid->Name = L"m_updown_trajectorycpid";
      this->m_updown_trajectorycpid->Size = System::Drawing::Size(57, 19);
      this->m_updown_trajectorycpid->TabIndex = 16;
      this->m_updown_trajectorycpid->ValueChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_updown_trajectorycpid_ValueChanged);
      // 
      // m_bvis_isosurface
      // 
      this->m_bvis_isosurface->AutoSize = true;
      this->m_bvis_isosurface->Checked = true;
      this->m_bvis_isosurface->CheckState = System::Windows::Forms::CheckState::Checked;
      this->m_bvis_isosurface->Location = System::Drawing::Point(5, 175);
      this->m_bvis_isosurface->Name = L"m_bvis_isosurface";
      this->m_bvis_isosurface->Size = System::Drawing::Size(99, 16);
      this->m_bvis_isosurface->TabIndex = 17;
      this->m_bvis_isosurface->Text = L"Vis IsoSurface";
      this->m_bvis_isosurface->UseVisualStyleBackColor = true;
      this->m_bvis_isosurface->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_bvis_isosurface_CheckedChanged);
      // 
      // m_bvis_fitsurface
      // 
      this->m_bvis_fitsurface->AutoSize = true;
      this->m_bvis_fitsurface->Checked = true;
      this->m_bvis_fitsurface->CheckState = System::Windows::Forms::CheckState::Checked;
      this->m_bvis_fitsurface->Location = System::Drawing::Point(5, 192);
      this->m_bvis_fitsurface->Name = L"m_bvis_fitsurface";
      this->m_bvis_fitsurface->Size = System::Drawing::Size(117, 16);
      this->m_bvis_fitsurface->TabIndex = 18;
      this->m_bvis_fitsurface->Text = L"Vis fitting surface";
      this->m_bvis_fitsurface->UseVisualStyleBackColor = true;
      this->m_bvis_fitsurface->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_bvis_fitsurface_CheckedChanged);
      // 
      // m_bvis_evalsurface
      // 
      this->m_bvis_evalsurface->AutoSize = true;
      this->m_bvis_evalsurface->Checked = true;
      this->m_bvis_evalsurface->CheckState = System::Windows::Forms::CheckState::Checked;
      this->m_bvis_evalsurface->Location = System::Drawing::Point(5, 272);
      this->m_bvis_evalsurface->Name = L"m_bvis_evalsurface";
      this->m_bvis_evalsurface->Size = System::Drawing::Size(139, 16);
      this->m_bvis_evalsurface->TabIndex = 19;
      this->m_bvis_evalsurface->Text = L"Vis evaluation surface";
      this->m_bvis_evalsurface->UseVisualStyleBackColor = true;
      this->m_bvis_evalsurface->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_bvis_evalsurface_CheckedChanged);
      // 
      // m_bvis_evaldiff
      // 
      this->m_bvis_evaldiff->AutoSize = true;
      this->m_bvis_evaldiff->Checked = true;
      this->m_bvis_evaldiff->CheckState = System::Windows::Forms::CheckState::Checked;
      this->m_bvis_evaldiff->Location = System::Drawing::Point(5, 292);
      this->m_bvis_evaldiff->Name = L"m_bvis_evaldiff";
      this->m_bvis_evaldiff->Size = System::Drawing::Size(120, 16);
      this->m_bvis_evaldiff->TabIndex = 20;
      this->m_bvis_evaldiff->Text = L"Vis evaluation Diff";
      this->m_bvis_evaldiff->UseVisualStyleBackColor = true;
      this->m_bvis_evaldiff->CheckedChanged += gcnew System::EventHandler(this, &FormSegJointTracker::m_bvis_evaldiff_CheckedChanged);
      // 
      // FormSegJointTracker
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(403, 335);
      this->Controls->Add(this->m_bvis_evaldiff);
      this->Controls->Add(this->m_bvis_evalsurface);
      this->Controls->Add(this->m_bvis_fitsurface);
      this->Controls->Add(this->m_bvis_isosurface);
      this->Controls->Add(this->m_updown_trajectorycpid);
      this->Controls->Add(this->m_bvis_cptrajectory);
      this->Controls->Add(this->m_bvis_cps);
      this->Controls->Add(this->m_groupBox_analysis);
      this->Controls->Add(this->m_groupBox_runicp);
      this->Controls->Add(this->m_groupBox_manipmode);
      this->Controls->Add(this->m_groupbox_isosurface);
      this->Name = L"FormSegJointTracker";
      this->Text = L"FormSegJointTracker";
      this->m_groupbox_isosurface->ResumeLayout(false);
      this->m_groupbox_isosurface->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_updown_isovalue))->EndInit();
      this->m_groupBox_manipmode->ResumeLayout(false);
      this->m_groupBox_manipmode->PerformLayout();
      this->m_groupBox_runicp->ResumeLayout(false);
      this->m_groupBox_runicp->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_updown_icplv))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_updown_rejectscale))->EndInit();
      this->m_groupBox_analysis->ResumeLayout(false);
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_updown_trajectorycpid))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private: 
    System::Void m_mode_placecp_bone1_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_mode_placecp_bone2_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_mode_transbone1_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_mode_rotbone1_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_mode_transbone2_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_mode_rotbone2_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_mode_transobj_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_mode_rotobj_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_updown_isovalue_ValueChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_button_genisosurf_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_updown_rejectscale_ValueChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_updown_icplv_ValueChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_button_runicp_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_button_exportcps_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_button_importcps_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_button_loadobj_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_button_fitobjcalcerror_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_button_runicp_curronly_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void m_bvis_isosurface_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_bvis_fitsurface_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_bvis_cps_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
    System::Void m_bvis_cptrajectory_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_bvis_evalsurface_CheckedChanged(System::Object^  sender, System::EventArgs^  e); 
    System::Void m_bvis_evaldiff_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_updown_trajectorycpid_ValueChanged(System::Object^  sender, System::EventArgs^  e) ;
};

  inline void  FormSegJointTracker_Show() { FormSegJointTracker::GetInst()->Show(); }
  inline void  FormSegJointTracker_Hide() { FormSegJointTracker::GetInst()->Hide(); }
  inline void  FormSegJointTracker_InitializeItems(int minv, int maxv) {
    FormSegJointTracker::GetInst()->InitializeItems(minv, maxv);
  }

  //interface for mode
  inline bool FormJTrack_bModePlaceCpBone1(){ return FormSegJointTracker::GetInst()->bModePlaceCPBone1();}
  inline bool FormJTrack_bModePlaceCpBone2(){ return FormSegJointTracker::GetInst()->bModePlaceCPBone2();}

  inline bool FormJTrack_bModeTransBone1  (){ return FormSegJointTracker::GetInst()->bModeTransBone1();}
  inline bool FormJTrack_bModeTransBone2  (){ return FormSegJointTracker::GetInst()->bModeTransBone2();}
  inline bool FormJTrack_bModeTransObj    (){ return FormSegJointTracker::GetInst()->bModeTransObj  ();}
  inline bool FormJTrack_bModeRotBone1    (){ return FormSegJointTracker::GetInst()->bModeRotBone1();}
  inline bool FormJTrack_bModeRotBone2    (){ return FormSegJointTracker::GetInst()->bModeRotBone2();}
  inline bool FormJTrack_bModeRotObj      (){ return FormSegJointTracker::GetInst()->bModeRotObj();}
  inline void FormJTrack_SetModePlaceCpBone2(){ FormSegJointTracker::GetInst()->SetModePlaceCPBone2();}


  inline bool FormJTrack_bVisCps()        { return FormSegJointTracker::GetInst()->bVisCps();}
  inline bool FormJTrack_bVisCpTrajecory(){ return FormSegJointTracker::GetInst()->bVisCpTrajecory();}
  inline bool FormJTrack_bVisIsoSurface() { return FormSegJointTracker::GetInst()->bVisIsoSurface();}
  inline bool FormJTrack_bVisFitSurface() { return FormSegJointTracker::GetInst()->bVisFitSurface();}
  inline bool FormJTrack_bVisEvalSurf()   { return FormSegJointTracker::GetInst()->bVisEvalSurf() ;}
  inline bool FormJTrack_bVisEvalDiff()   { return FormSegJointTracker::GetInst()->bVisEvalDiff();}
  inline int  FormJTrack_GetVisTrajCpid() { return FormSegJointTracker::GetInst()->GetVisTrajCpid();}
    


}
