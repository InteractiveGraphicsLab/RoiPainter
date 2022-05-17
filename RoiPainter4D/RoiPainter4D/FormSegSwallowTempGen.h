#pragma once

namespace RoiPainter4D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegSwallowTempGen の概要
	/// </summary>
	public ref class FormSegSwallowTempGen : public System::Windows::Forms::Form
	{

	private:
		FormSegSwallowTempGen(void)
		{
			InitializeComponent();
		}
	private: System::Windows::Forms::Button^ m_btn_export_motion;

	private: System::Windows::Forms::Button^ m_btn_addzetumesh;
	private: System::Windows::Forms::Button^ m_btn_addnankmesh;
	private: System::Windows::Forms::Button^ m_btn_addkoutmesh;
	private: System::Windows::Forms::Button^ m_btn_addintomesh;
	private: System::Windows::Forms::GroupBox^ groupBox2;

	private: System::Windows::Forms::GroupBox^ m_groupbox_deformation;
	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::Label^ m_label1;
	private: System::Windows::Forms::RadioButton^ m_radiobtn_translatecage;
	private: System::Windows::Forms::RadioButton^ m_radiobtn_rotatecage;
	private: System::Windows::Forms::RadioButton^ m_radiobtn_scalecage;

	private: System::Windows::Forms::CheckBox^ m_chooseinto;
	private: System::Windows::Forms::CheckBox^ m_choosekout;
	private: System::Windows::Forms::CheckBox^ m_choosenank;
	private: System::Windows::Forms::CheckBox^ m_choosezetu;
	private: System::Windows::Forms::CheckBox^ m_check_allframe;

	private: System::Windows::Forms::NumericUpDown^ m_numeric1;
	private: System::Windows::Forms::NumericUpDown^ m_numeric2;
	private: System::Windows::Forms::NumericUpDown^ m_numeric3;
	private: System::Windows::Forms::NumericUpDown^ m_numeric4;

	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::Label^ label4;
	private: System::Windows::Forms::Label^ label5;
	private: System::Windows::Forms::Button^ m_btn_update_timing;


	static FormSegSwallowTempGen^ m_singleton;

	public:
		static FormSegSwallowTempGen^ getInst()
		{
			if (m_singleton == nullptr) 
				m_singleton = gcnew FormSegSwallowTempGen();
			return m_singleton;
		}

		bool isChecked_TransZetsu() { return m_choosezetu->Checked; }
		bool isChecked_TransNank () { return m_choosenank->Checked; }
		bool isChecked_TransKoto () { return m_choosekout->Checked; }
		bool isChecked_TransInto () { return m_chooseinto->Checked; }
		bool isChecked_TransAllFrame(){ return m_check_allframe->Checked;}
	
	
		bool isModeTrans() { return m_radiobtn_translatecage->Checked; }
		bool isModeRot  () { return m_radiobtn_rotatecage->Checked; }
		bool isModeScale() { return m_radiobtn_scalecage->Checked; }
		
		void initialize_spin_control(int num_frames, int f1, int f2, int f3, int f4);
	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormSegSwallowTempGen()
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
			this->m_btn_export_motion = (gcnew System::Windows::Forms::Button());
			this->m_btn_addzetumesh = (gcnew System::Windows::Forms::Button());
			this->m_btn_addnankmesh = (gcnew System::Windows::Forms::Button());
			this->m_btn_addkoutmesh = (gcnew System::Windows::Forms::Button());
			this->m_btn_addintomesh = (gcnew System::Windows::Forms::Button());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->m_groupbox_deformation = (gcnew System::Windows::Forms::GroupBox());
			this->m_check_allframe = (gcnew System::Windows::Forms::CheckBox());
			this->m_chooseinto = (gcnew System::Windows::Forms::CheckBox());
			this->m_choosekout = (gcnew System::Windows::Forms::CheckBox());
			this->m_choosenank = (gcnew System::Windows::Forms::CheckBox());
			this->m_choosezetu = (gcnew System::Windows::Forms::CheckBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->m_label1 = (gcnew System::Windows::Forms::Label());
			this->m_radiobtn_translatecage = (gcnew System::Windows::Forms::RadioButton());
			this->m_radiobtn_rotatecage = (gcnew System::Windows::Forms::RadioButton());
			this->m_radiobtn_scalecage = (gcnew System::Windows::Forms::RadioButton());
			this->m_numeric1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->m_numeric2 = (gcnew System::Windows::Forms::NumericUpDown());
			this->m_numeric3 = (gcnew System::Windows::Forms::NumericUpDown());
			this->m_numeric4 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->m_btn_update_timing = (gcnew System::Windows::Forms::Button());
			this->groupBox2->SuspendLayout();
			this->m_groupbox_deformation->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numeric1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numeric2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numeric3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numeric4))->BeginInit();
			this->SuspendLayout();
			// 
			// m_btn_export_motion
			// 
			this->m_btn_export_motion->Location = System::Drawing::Point(200, 292);
			this->m_btn_export_motion->Name = L"m_btn_export_motion";
			this->m_btn_export_motion->Size = System::Drawing::Size(116, 41);
			this->m_btn_export_motion->TabIndex = 0;
			this->m_btn_export_motion->Text = L"Export Cage Motion (txt)";
			this->m_btn_export_motion->UseVisualStyleBackColor = true;
			this->m_btn_export_motion->Click += gcnew System::EventHandler(this, &FormSegSwallowTempGen::m_btn_export_motion_Click);
			// 
			// m_btn_addzetumesh
			// 
			this->m_btn_addzetumesh->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->m_btn_addzetumesh->Location = System::Drawing::Point(5, 22);
			this->m_btn_addzetumesh->Margin = System::Windows::Forms::Padding(2);
			this->m_btn_addzetumesh->Name = L"m_btn_addzetumesh";
			this->m_btn_addzetumesh->Size = System::Drawing::Size(67, 27);
			this->m_btn_addzetumesh->TabIndex = 10;
			this->m_btn_addzetumesh->Text = L"舌";
			this->m_btn_addzetumesh->UseVisualStyleBackColor = true;
			this->m_btn_addzetumesh->Click += gcnew System::EventHandler(this, &FormSegSwallowTempGen::m_btn_addzetumesh_Click);
			// 
			// m_btn_addnankmesh
			// 
			this->m_btn_addnankmesh->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->m_btn_addnankmesh->Location = System::Drawing::Point(76, 22);
			this->m_btn_addnankmesh->Margin = System::Windows::Forms::Padding(2);
			this->m_btn_addnankmesh->Name = L"m_btn_addnankmesh";
			this->m_btn_addnankmesh->Size = System::Drawing::Size(66, 27);
			this->m_btn_addnankmesh->TabIndex = 11;
			this->m_btn_addnankmesh->Text = L"軟口蓋";
			this->m_btn_addnankmesh->UseVisualStyleBackColor = true;
			this->m_btn_addnankmesh->Click += gcnew System::EventHandler(this, &FormSegSwallowTempGen::m_btn_addnankmesh_Click);
			// 
			// m_btn_addkoutmesh
			// 
			this->m_btn_addkoutmesh->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->m_btn_addkoutmesh->Location = System::Drawing::Point(146, 22);
			this->m_btn_addkoutmesh->Margin = System::Windows::Forms::Padding(2);
			this->m_btn_addkoutmesh->Name = L"m_btn_addkoutmesh";
			this->m_btn_addkoutmesh->Size = System::Drawing::Size(66, 27);
			this->m_btn_addkoutmesh->TabIndex = 12;
			this->m_btn_addkoutmesh->Text = L"喉頭蓋";
			this->m_btn_addkoutmesh->UseVisualStyleBackColor = true;
			this->m_btn_addkoutmesh->Click += gcnew System::EventHandler(this, &FormSegSwallowTempGen::m_btn_addkoutmesh_Click);
			// 
			// m_btn_addintomesh
			// 
			this->m_btn_addintomesh->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->m_btn_addintomesh->Location = System::Drawing::Point(216, 22);
			this->m_btn_addintomesh->Margin = System::Windows::Forms::Padding(2);
			this->m_btn_addintomesh->Name = L"m_btn_addintomesh";
			this->m_btn_addintomesh->Size = System::Drawing::Size(71, 27);
			this->m_btn_addintomesh->TabIndex = 13;
			this->m_btn_addintomesh->Text = L"咽頭";
			this->m_btn_addintomesh->UseVisualStyleBackColor = true;
			this->m_btn_addintomesh->Click += gcnew System::EventHandler(this, &FormSegSwallowTempGen::m_btn_addintomesh_Click);
			// 
			// groupBox2
			// 
			this->groupBox2->BackColor = System::Drawing::Color::LightSteelBlue;
			this->groupBox2->Controls->Add(this->m_btn_addkoutmesh);
			this->groupBox2->Controls->Add(this->m_btn_addintomesh);
			this->groupBox2->Controls->Add(this->m_btn_addnankmesh);
			this->groupBox2->Controls->Add(this->m_btn_addzetumesh);
			this->groupBox2->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->groupBox2->Location = System::Drawing::Point(20, 19);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(296, 64);
			this->groupBox2->TabIndex = 24;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Cage ＆ Mesh 読み込み";
			// 
			// m_groupbox_deformation
			// 
			this->m_groupbox_deformation->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->m_groupbox_deformation->Controls->Add(this->m_check_allframe);
			this->m_groupbox_deformation->Controls->Add(this->m_chooseinto);
			this->m_groupbox_deformation->Controls->Add(this->m_choosekout);
			this->m_groupbox_deformation->Controls->Add(this->m_choosenank);
			this->m_groupbox_deformation->Controls->Add(this->m_choosezetu);
			this->m_groupbox_deformation->Controls->Add(this->label2);
			this->m_groupbox_deformation->Controls->Add(this->m_label1);
			this->m_groupbox_deformation->Controls->Add(this->m_radiobtn_translatecage);
			this->m_groupbox_deformation->Controls->Add(this->m_radiobtn_rotatecage);
			this->m_groupbox_deformation->Controls->Add(this->m_radiobtn_scalecage);
			this->m_groupbox_deformation->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->m_groupbox_deformation->Location = System::Drawing::Point(20, 95);
			this->m_groupbox_deformation->Name = L"m_groupbox_deformation";
			this->m_groupbox_deformation->Size = System::Drawing::Size(296, 104);
			this->m_groupbox_deformation->TabIndex = 25;
			this->m_groupbox_deformation->TabStop = false;
			this->m_groupbox_deformation->Text = L"選択器官の変形";
			// 
			// m_check_allframe
			// 
			this->m_check_allframe->AutoSize = true;
			this->m_check_allframe->Checked = true;
			this->m_check_allframe->CheckState = System::Windows::Forms::CheckState::Checked;
			this->m_check_allframe->Location = System::Drawing::Point(197, 18);
			this->m_check_allframe->Name = L"m_check_allframe";
			this->m_check_allframe->Size = System::Drawing::Size(93, 24);
			this->m_check_allframe->TabIndex = 30;
			this->m_check_allframe->Text = L"All Frame";
			this->m_check_allframe->UseVisualStyleBackColor = true;
			// 
			// m_chooseinto
			// 
			this->m_chooseinto->AutoSize = true;
			this->m_chooseinto->Checked = true;
			this->m_chooseinto->CheckState = System::Windows::Forms::CheckState::Checked;
			this->m_chooseinto->Location = System::Drawing::Point(204, 73);
			this->m_chooseinto->Name = L"m_chooseinto";
			this->m_chooseinto->Size = System::Drawing::Size(54, 24);
			this->m_chooseinto->TabIndex = 29;
			this->m_chooseinto->Text = L"咽頭";
			this->m_chooseinto->UseVisualStyleBackColor = true;
			// 
			// m_choosekout
			// 
			this->m_choosekout->AutoSize = true;
			this->m_choosekout->Checked = true;
			this->m_choosekout->CheckState = System::Windows::Forms::CheckState::Checked;
			this->m_choosekout->Location = System::Drawing::Point(131, 73);
			this->m_choosekout->Name = L"m_choosekout";
			this->m_choosekout->Size = System::Drawing::Size(67, 24);
			this->m_choosekout->TabIndex = 28;
			this->m_choosekout->Text = L"喉頭蓋";
			this->m_choosekout->UseVisualStyleBackColor = true;
			// 
			// m_choosenank
			// 
			this->m_choosenank->AutoSize = true;
			this->m_choosenank->Checked = true;
			this->m_choosenank->CheckState = System::Windows::Forms::CheckState::Checked;
			this->m_choosenank->Location = System::Drawing::Point(58, 74);
			this->m_choosenank->Name = L"m_choosenank";
			this->m_choosenank->Size = System::Drawing::Size(67, 24);
			this->m_choosenank->TabIndex = 27;
			this->m_choosenank->Text = L"軟口蓋";
			this->m_choosenank->UseVisualStyleBackColor = true;
			this->m_choosenank->CheckedChanged += gcnew System::EventHandler(this, &FormSegSwallowTempGen::m_choosenank_CheckedChanged);
			// 
			// m_choosezetu
			// 
			this->m_choosezetu->AutoSize = true;
			this->m_choosezetu->Checked = true;
			this->m_choosezetu->CheckState = System::Windows::Forms::CheckState::Checked;
			this->m_choosezetu->Location = System::Drawing::Point(9, 75);
			this->m_choosezetu->Name = L"m_choosezetu";
			this->m_choosezetu->Size = System::Drawing::Size(41, 24);
			this->m_choosezetu->TabIndex = 26;
			this->m_choosezetu->Text = L"舌";
			this->m_choosezetu->UseVisualStyleBackColor = true;
			this->m_choosezetu->CheckedChanged += gcnew System::EventHandler(this, &FormSegSwallowTempGen::m_choosezetu_CheckedChanged);
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
			// m_radiobtn_translatecage
			// 
			this->m_radiobtn_translatecage->AutoSize = true;
			this->m_radiobtn_translatecage->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->m_radiobtn_translatecage->Location = System::Drawing::Point(132, 49);
			this->m_radiobtn_translatecage->Margin = System::Windows::Forms::Padding(2);
			this->m_radiobtn_translatecage->Name = L"m_radiobtn_translatecage";
			this->m_radiobtn_translatecage->Size = System::Drawing::Size(74, 22);
			this->m_radiobtn_translatecage->TabIndex = 5;
			this->m_radiobtn_translatecage->Text = L"平行移動";
			this->m_radiobtn_translatecage->UseVisualStyleBackColor = true;
			// 
			// m_radiobtn_rotatecage
			// 
			this->m_radiobtn_rotatecage->AutoSize = true;
			this->m_radiobtn_rotatecage->Checked = true;
			this->m_radiobtn_rotatecage->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->m_radiobtn_rotatecage->Location = System::Drawing::Point(68, 49);
			this->m_radiobtn_rotatecage->Margin = System::Windows::Forms::Padding(2);
			this->m_radiobtn_rotatecage->Name = L"m_radiobtn_rotatecage";
			this->m_radiobtn_rotatecage->Size = System::Drawing::Size(50, 22);
			this->m_radiobtn_rotatecage->TabIndex = 4;
			this->m_radiobtn_rotatecage->TabStop = true;
			this->m_radiobtn_rotatecage->Text = L"回転";
			this->m_radiobtn_rotatecage->UseVisualStyleBackColor = true;
			// 
			// m_radiobtn_scalecage
			// 
			this->m_radiobtn_scalecage->AutoSize = true;
			this->m_radiobtn_scalecage->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->m_radiobtn_scalecage->Location = System::Drawing::Point(10, 49);
			this->m_radiobtn_scalecage->Margin = System::Windows::Forms::Padding(2);
			this->m_radiobtn_scalecage->Name = L"m_radiobtn_scalecage";
			this->m_radiobtn_scalecage->Size = System::Drawing::Size(50, 22);
			this->m_radiobtn_scalecage->TabIndex = 14;
			this->m_radiobtn_scalecage->Text = L"拡縮";
			this->m_radiobtn_scalecage->UseVisualStyleBackColor = true;
			// 
			// m_numeric1
			// 
			this->m_numeric1->Location = System::Drawing::Point(92, 239);
			this->m_numeric1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 30, 0, 0, 0 });
			this->m_numeric1->Name = L"m_numeric1";
			this->m_numeric1->Size = System::Drawing::Size(50, 19);
			this->m_numeric1->TabIndex = 26;
			// 
			// m_numeric2
			// 
			this->m_numeric2->Location = System::Drawing::Point(92, 264);
			this->m_numeric2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 30, 0, 0, 0 });
			this->m_numeric2->Name = L"m_numeric2";
			this->m_numeric2->Size = System::Drawing::Size(50, 19);
			this->m_numeric2->TabIndex = 27;
			// 
			// m_numeric3
			// 
			this->m_numeric3->Location = System::Drawing::Point(92, 289);
			this->m_numeric3->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 30, 0, 0, 0 });
			this->m_numeric3->Name = L"m_numeric3";
			this->m_numeric3->Size = System::Drawing::Size(50, 19);
			this->m_numeric3->TabIndex = 28;
			// 
			// m_numeric4
			// 
			this->m_numeric4->Location = System::Drawing::Point(92, 314);
			this->m_numeric4->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 30, 0, 0, 0 });
			this->m_numeric4->Name = L"m_numeric4";
			this->m_numeric4->Size = System::Drawing::Size(50, 19);
			this->m_numeric4->TabIndex = 29;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"メイリオ", 6.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->label1->Location = System::Drawing::Point(22, 241);
			this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(61, 14);
			this->label1->TabIndex = 30;
			this->label1->Text = L"食塊動き始め";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"メイリオ", 6.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->label3->Location = System::Drawing::Point(19, 266);
			this->label3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(70, 14);
			this->label3->TabIndex = 31;
			this->label3->Text = L"舌骨上がり切る";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"メイリオ", 6.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->label4->Location = System::Drawing::Point(17, 291);
			this->label4->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(70, 14);
			this->label4->TabIndex = 31;
			this->label4->Text = L"舌骨下がり始め";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"メイリオ", 6.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->label5->Location = System::Drawing::Point(17, 316);
			this->label5->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(70, 14);
			this->label5->TabIndex = 32;
			this->label5->Text = L"舌骨下がり切る";
			// 
			// m_btn_update_timing
			// 
			this->m_btn_update_timing->Location = System::Drawing::Point(157, 239);
			this->m_btn_update_timing->Name = L"m_btn_update_timing";
			this->m_btn_update_timing->Size = System::Drawing::Size(135, 41);
			this->m_btn_update_timing->TabIndex = 33;
			this->m_btn_update_timing->Text = L"Update Timing ";
			this->m_btn_update_timing->UseVisualStyleBackColor = true;
			this->m_btn_update_timing->Click += gcnew System::EventHandler(this, &FormSegSwallowTempGen::m_btn_update_timing_Click);
			// 
			// FormSegSwallowTempGen
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(325, 345);
			this->Controls->Add(this->m_btn_update_timing);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->m_numeric4);
			this->Controls->Add(this->m_numeric3);
			this->Controls->Add(this->m_numeric2);
			this->Controls->Add(this->m_numeric1);
			this->Controls->Add(this->m_groupbox_deformation);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->m_btn_export_motion);
			this->Name = L"FormSegSwallowTempGen";
			this->Text = L"FormSegSwallowTempGen";
			this->groupBox2->ResumeLayout(false);
			this->m_groupbox_deformation->ResumeLayout(false);
			this->m_groupbox_deformation->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numeric1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numeric2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numeric3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_numeric4))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private:
		System::Void m_btn_addzetumesh_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_addnankmesh_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_addkoutmesh_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_addintomesh_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_choosezetu_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void m_choosenank_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void m_choosekout_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void m_chooseinto_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_export_motion_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void m_btn_update_timing_Click(System::Object^ sender, System::EventArgs^ e);
};

	
	
	inline void FormSegSwallowTempGen_Hide() {
		FormSegSwallowTempGen::getInst()->Hide();
	}

	inline void FormSegSwallowTempGen_Show() {
		FormSegSwallowTempGen::getInst()->Show();
	}
	inline bool FormSegSwallowTempGen_TransZetsu(){ 
		return FormSegSwallowTempGen::getInst()->isChecked_TransZetsu();
	}
	inline bool FormSegSwallowTempGen_TransNank() {
		return FormSegSwallowTempGen::getInst()->isChecked_TransNank();
	}
	inline bool FormSegSwallowTempGen_TransKoto() {
		return FormSegSwallowTempGen::getInst()->isChecked_TransKoto();
	}
	inline bool FormSegSwallowTempGen_TransInto() {
		return FormSegSwallowTempGen::getInst()->isChecked_TransInto();
	}
	inline bool FormSegSwallowTempGen_TransAllFrame() {
		return FormSegSwallowTempGen::getInst()->isChecked_TransAllFrame();
	}
	
	inline bool FormSegSwallowTempGen_isModeTrans() {
		return FormSegSwallowTempGen::getInst()->isModeTrans();
	}
	inline bool FormSegSwallowTempGen_isModeRot() {
		return FormSegSwallowTempGen::getInst()->isModeRot();
	}
	inline bool FormSegSwallowTempGen_isModeScale() {
		return FormSegSwallowTempGen::getInst()->isModeScale();
	}

	inline void FormSegSwallowTempGen_InitSpins(int num_frames, int f1, int f2, int f3, int f4) {
		FormSegSwallowTempGen::getInst()->initialize_spin_control(num_frames, f1,f2,f3,f4);
	}
	
}
