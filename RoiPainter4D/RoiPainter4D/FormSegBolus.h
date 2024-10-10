#pragma once

namespace RoiPainter4D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading::Tasks;

	std::string openFileDialog(const char* filterExpanded = "*", const char* dlgTitle = "");

	/// <summary>
	/// FormSegBolus の概要
	/// </summary>
	public ref class FormSegBolus : public System::Windows::Forms::Form
	{
  private:
    System::Windows::Forms::TabControl^  tabControl1;
    System::Windows::Forms::TabPage^     tabPage1;
    System::Windows::Forms::TabPage^     tabPage3;
    System::Windows::Forms::Panel^  panel1;
    System::Windows::Forms::Panel^  panel2;
    System::Windows::Forms::Panel^  panel3;
    System::Windows::Forms::Panel^  panel_segm;
    System::Windows::Forms::Label^  label1;
    System::Windows::Forms::Label^  label2;
    System::Windows::Forms::Label^  label3;
    System::Windows::Forms::Label^  label4;
    System::Windows::Forms::Label^  label5;
    System::Windows::Forms::Label^  label8;
    System::Windows::Forms::Label^  label9;
    System::Windows::Forms::Label^  label10;
    System::Windows::Forms::Label^  m_label_thresh;
    System::Windows::Forms::Label^  m_label_radius1;
    System::Windows::Forms::Label^  m_label_radius2;
    System::Windows::Forms::Label^  m_label_twistangle;
    System::Windows::Forms::Label^  m_label_transparency;
    System::Windows::Forms::CheckBox^ m_check_normalvolrend;
    System::Windows::Forms::CheckBox^ m_check_drawobbs;
    System::Windows::Forms::TextBox^   m_textbox_minthresh;
    System::Windows::Forms::TextBox^   m_textbox_maxthresh;
    System::Windows::Forms::TextBox^   m_textbox_lockareaband;
    System::Windows::Forms::TrackBar^  m_trackbar_minthresh;
    System::Windows::Forms::TrackBar^  m_trackbar_maxthresh;
    System::Windows::Forms::TrackBar^  m_trackbar_lockareaband;
    System::Windows::Forms::TrackBar^  m_trackbar_cylinder_radius1;
    System::Windows::Forms::TrackBar^  m_trackbar_cylinder_radius2;
    System::Windows::Forms::TrackBar^  m_trackbar_cylinder_twistangle;
    System::Windows::Forms::TrackBar^  m_trackbar_cylinder_trans;
    System::Windows::Forms::GroupBox^  m_groupbox_currframe;
    System::Windows::Forms::GroupBox^  m_groupbox_allframe;
    System::Windows::Forms::Button^  m_btn_loadcylinder;
    System::Windows::Forms::Button^  m_btn_savecylinder;
    System::Windows::Forms::Button^  m_btn_loadcylinder_fitting;
    System::Windows::Forms::Button^  m_btn_createnewcylinder;
    System::Windows::Forms::Button^  m_btn_regiongrow_currframe;
    System::Windows::Forms::Button^  m_btn_dilate_currframe;
    System::Windows::Forms::Button^  m_btn_erode_currframe;
    System::Windows::Forms::Button^  m_btn_thresholding_currframe;
    System::Windows::Forms::Button^  m_btn_regiongrow_allframe;
    System::Windows::Forms::Button^  m_btn_erode_allframe;
    System::Windows::Forms::Button^  m_btn_dilate_allframe;
    System::Windows::Forms::Button^  m_btn_thresholding_allframe;
    System::Windows::Forms::Button^  m_btn_regiongrow4d;
    System::Windows::Forms::Button^  m_btn_finish;
    System::Windows::Forms::Button^  m_btn_cancel;

  private:
    FormSegBolus(void);
    static FormSegBolus^ m_singleton_ = nullptr;

	public:
		static FormSegBolus^ GetInst() 
		{
			if(m_singleton_==nullptr) m_singleton_ = gcnew FormSegBolus();
			return m_singleton_;
		}
		
		//tab情報がほしければ下のようにする
		//return tabControl1->SelectedTab->Text->Equals("Segmentation");

    bool  IsDrawOBB();
    bool  IsNormalVolRend();
    float GetCylinderAlpha();
    int   GetLockAreaBand();

    void InitAllItems(int volmin, int volmax, float radmin, float radmax);
    void SetItemsForCylinderProparty(bool is_active, float radius1, float radius2, float twist_angle);

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
    ~FormSegBolus();

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
      this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
      this->m_textbox_lockareaband = (gcnew System::Windows::Forms::TextBox());
      this->label10 = (gcnew System::Windows::Forms::Label());
      this->m_trackbar_maxthresh = (gcnew System::Windows::Forms::TrackBar());
      this->m_trackbar_minthresh = (gcnew System::Windows::Forms::TrackBar());
      this->m_textbox_maxthresh = (gcnew System::Windows::Forms::TextBox());
      this->m_textbox_minthresh = (gcnew System::Windows::Forms::TextBox());
      this->panel_segm = (gcnew System::Windows::Forms::Panel());
      this->m_groupbox_currframe = (gcnew System::Windows::Forms::GroupBox());
      this->m_btn_regiongrow_currframe = (gcnew System::Windows::Forms::Button());
      this->m_btn_dilate_currframe = (gcnew System::Windows::Forms::Button());
      this->m_btn_erode_currframe = (gcnew System::Windows::Forms::Button());
      this->m_btn_thresholding_currframe = (gcnew System::Windows::Forms::Button());
      this->m_btn_regiongrow4d = (gcnew System::Windows::Forms::Button());
      this->m_btn_finish = (gcnew System::Windows::Forms::Button());
      this->m_btn_cancel = (gcnew System::Windows::Forms::Button());
      this->m_groupbox_allframe = (gcnew System::Windows::Forms::GroupBox());
      this->m_btn_regiongrow_allframe = (gcnew System::Windows::Forms::Button());
      this->m_btn_erode_allframe = (gcnew System::Windows::Forms::Button());
      this->m_btn_dilate_allframe = (gcnew System::Windows::Forms::Button());
      this->m_btn_thresholding_allframe = (gcnew System::Windows::Forms::Button());
      this->m_label_thresh = (gcnew System::Windows::Forms::Label());
      this->label8 = (gcnew System::Windows::Forms::Label());
      this->label9 = (gcnew System::Windows::Forms::Label());
      this->m_trackbar_lockareaband = (gcnew System::Windows::Forms::TrackBar());
      this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
      this->m_check_normalvolrend = (gcnew System::Windows::Forms::CheckBox());
      this->label5 = (gcnew System::Windows::Forms::Label());
      this->label4 = (gcnew System::Windows::Forms::Label());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->panel3 = (gcnew System::Windows::Forms::Panel());
      this->panel1 = (gcnew System::Windows::Forms::Panel());
      this->panel2 = (gcnew System::Windows::Forms::Panel());
      this->m_btn_loadcylinder = (gcnew System::Windows::Forms::Button());
      this->m_btn_savecylinder = (gcnew System::Windows::Forms::Button());
      this->m_btn_loadcylinder_fitting = (gcnew System::Windows::Forms::Button());
      this->m_btn_createnewcylinder = (gcnew System::Windows::Forms::Button());
      this->m_label_radius1 = (gcnew System::Windows::Forms::Label());
      this->m_check_drawobbs = (gcnew System::Windows::Forms::CheckBox());
      this->m_label_radius2 = (gcnew System::Windows::Forms::Label());
      this->m_label_twistangle = (gcnew System::Windows::Forms::Label());
      this->m_label_transparency = (gcnew System::Windows::Forms::Label());
      this->m_trackbar_cylinder_trans = (gcnew System::Windows::Forms::TrackBar());
      this->m_trackbar_cylinder_twistangle = (gcnew System::Windows::Forms::TrackBar());
      this->m_trackbar_cylinder_radius2 = (gcnew System::Windows::Forms::TrackBar());
      this->m_trackbar_cylinder_radius1 = (gcnew System::Windows::Forms::TrackBar());
      this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
      this->tabPage3->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_maxthresh))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_minthresh))->BeginInit();
      this->panel_segm->SuspendLayout();
      this->m_groupbox_currframe->SuspendLayout();
      this->m_groupbox_allframe->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_lockareaband))->BeginInit();
      this->tabPage1->SuspendLayout();
      this->panel1->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_cylinder_trans))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_cylinder_twistangle))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_cylinder_radius2))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_cylinder_radius1))->BeginInit();
      this->tabControl1->SuspendLayout();
      this->SuspendLayout();
      // 
      // tabPage3
      // 
      this->tabPage3->BackColor = System::Drawing::Color::Transparent;
      this->tabPage3->Controls->Add(this->m_textbox_lockareaband);
      this->tabPage3->Controls->Add(this->label10);
      this->tabPage3->Controls->Add(this->m_trackbar_maxthresh);
      this->tabPage3->Controls->Add(this->m_trackbar_minthresh);
      this->tabPage3->Controls->Add(this->m_textbox_maxthresh);
      this->tabPage3->Controls->Add(this->m_textbox_minthresh);
      this->tabPage3->Controls->Add(this->panel_segm);
      this->tabPage3->Controls->Add(this->m_label_thresh);
      this->tabPage3->Controls->Add(this->label8);
      this->tabPage3->Controls->Add(this->label9);
      this->tabPage3->Controls->Add(this->m_trackbar_lockareaband);
      this->tabPage3->Location = System::Drawing::Point(4, 22);
      this->tabPage3->Name = L"tabPage3";
      this->tabPage3->Size = System::Drawing::Size(219, 366);
      this->tabPage3->TabIndex = 2;
      this->tabPage3->Text = L"Segmentation";
      // 
      // m_textbox_lockareaband
      // 
      this->m_textbox_lockareaband->Location = System::Drawing::Point(23, 95);
      this->m_textbox_lockareaband->Name = L"m_textbox_lockareaband";
      this->m_textbox_lockareaband->Size = System::Drawing::Size(39, 19);
      this->m_textbox_lockareaband->TabIndex = 39;
      this->m_textbox_lockareaband->TextChanged += gcnew System::EventHandler(this, &FormSegBolus::m_textbox_lockareaband_TextChanged);
      // 
      // label10
      // 
      this->label10->AutoSize = true;
      this->label10->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
      this->label10->Location = System::Drawing::Point(2, 75);
      this->label10->Name = L"label10";
      this->label10->Size = System::Drawing::Size(122, 16);
      this->label10->TabIndex = 32;
      this->label10->Text = L"locked area band";
      // 
      // m_trackbar_maxthresh
      // 
      this->m_trackbar_maxthresh->Location = System::Drawing::Point(78, 53);
      this->m_trackbar_maxthresh->Name = L"m_trackbar_maxthresh";
      this->m_trackbar_maxthresh->Size = System::Drawing::Size(138, 45);
      this->m_trackbar_maxthresh->TabIndex = 38;
      this->m_trackbar_maxthresh->TickStyle = System::Windows::Forms::TickStyle::None;
      this->m_trackbar_maxthresh->Scroll += gcnew System::EventHandler(this, &FormSegBolus::m_trackbar_maxthresh_Scroll);
      // 
      // m_trackbar_minthresh
      // 
      this->m_trackbar_minthresh->Location = System::Drawing::Point(78, 27);
      this->m_trackbar_minthresh->Name = L"m_trackbar_minthresh";
      this->m_trackbar_minthresh->Size = System::Drawing::Size(138, 45);
      this->m_trackbar_minthresh->TabIndex = 37;
      this->m_trackbar_minthresh->TickStyle = System::Windows::Forms::TickStyle::None;
      this->m_trackbar_minthresh->Scroll += gcnew System::EventHandler(this, &FormSegBolus::m_trackbar_minthresh_Scroll);
      // 
      // m_textbox_maxthresh
      // 
      this->m_textbox_maxthresh->Location = System::Drawing::Point(37, 53);
      this->m_textbox_maxthresh->Name = L"m_textbox_maxthresh";
      this->m_textbox_maxthresh->Size = System::Drawing::Size(39, 19);
      this->m_textbox_maxthresh->TabIndex = 36;
      this->m_textbox_maxthresh->TextChanged += gcnew System::EventHandler(this, &FormSegBolus::m_textbox_maxthresh_TextChanged);
      // 
      // m_textbox_minthresh
      // 
      this->m_textbox_minthresh->Location = System::Drawing::Point(37, 27);
      this->m_textbox_minthresh->Name = L"m_textbox_minthresh";
      this->m_textbox_minthresh->Size = System::Drawing::Size(39, 19);
      this->m_textbox_minthresh->TabIndex = 35;
      this->m_textbox_minthresh->TextChanged += gcnew System::EventHandler(this, &FormSegBolus::m_textbox_minthresh_TextChanged);
      // 
      // panel_segm
      // 
      this->panel_segm->Controls->Add(this->m_groupbox_currframe);
      this->panel_segm->Controls->Add(this->m_btn_regiongrow4d);
      this->panel_segm->Controls->Add(this->m_btn_finish);
      this->panel_segm->Controls->Add(this->m_btn_cancel);
      this->panel_segm->Controls->Add(this->m_groupbox_allframe);
      this->panel_segm->Location = System::Drawing::Point(3, 122);
      this->panel_segm->Name = L"panel_segm";
      this->panel_segm->Size = System::Drawing::Size(213, 240);
      this->panel_segm->TabIndex = 21;
      // 
      // m_groupbox_currframe
      // 
      this->m_groupbox_currframe->Controls->Add(this->m_btn_regiongrow_currframe);
      this->m_groupbox_currframe->Controls->Add(this->m_btn_dilate_currframe);
      this->m_groupbox_currframe->Controls->Add(this->m_btn_erode_currframe);
      this->m_groupbox_currframe->Controls->Add(this->m_btn_thresholding_currframe);
      this->m_groupbox_currframe->Location = System::Drawing::Point(2, 11);
      this->m_groupbox_currframe->Name = L"m_groupbox_currframe";
      this->m_groupbox_currframe->Size = System::Drawing::Size(101, 143);
      this->m_groupbox_currframe->TabIndex = 24;
      this->m_groupbox_currframe->TabStop = false;
      this->m_groupbox_currframe->Text = L"CurrentFrame";
      // 
      // m_btn_regiongrow_currframe
      // 
      this->m_btn_regiongrow_currframe->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10));
      this->m_btn_regiongrow_currframe->Location = System::Drawing::Point(3, 49);
      this->m_btn_regiongrow_currframe->Name = L"m_btn_regiongrow_currframe";
      this->m_btn_regiongrow_currframe->Size = System::Drawing::Size(95, 30);
      this->m_btn_regiongrow_currframe->TabIndex = 14;
      this->m_btn_regiongrow_currframe->Text = L"Region Grow";
      this->m_btn_regiongrow_currframe->UseVisualStyleBackColor = true;
      this->m_btn_regiongrow_currframe->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_regiongrow_currframe_Clicked);
      // 
      // m_btn_dilate_currframe
      // 
      this->m_btn_dilate_currframe->Location = System::Drawing::Point(3, 112);
      this->m_btn_dilate_currframe->Name = L"m_btn_dilate_currframe";
      this->m_btn_dilate_currframe->Size = System::Drawing::Size(95, 23);
      this->m_btn_dilate_currframe->TabIndex = 13;
      this->m_btn_dilate_currframe->Text = L"dilate";
      this->m_btn_dilate_currframe->UseVisualStyleBackColor = true;
      this->m_btn_dilate_currframe->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_dilate_currframe_Clicked);
      // 
      // m_btn_erode_currframe
      // 
      this->m_btn_erode_currframe->Location = System::Drawing::Point(3, 85);
      this->m_btn_erode_currframe->Name = L"m_btn_erode_currframe";
      this->m_btn_erode_currframe->Size = System::Drawing::Size(95, 23);
      this->m_btn_erode_currframe->TabIndex = 12;
      this->m_btn_erode_currframe->Text = L"erode";
      this->m_btn_erode_currframe->UseVisualStyleBackColor = true;
      this->m_btn_erode_currframe->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_erode_currframe_Clicked);
      // 
      // m_btn_thresholding_currframe
      // 
      this->m_btn_thresholding_currframe->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10));
      this->m_btn_thresholding_currframe->Location = System::Drawing::Point(3, 17);
      this->m_btn_thresholding_currframe->Name = L"m_btn_thresholding_currframe";
      this->m_btn_thresholding_currframe->Size = System::Drawing::Size(95, 30);
      this->m_btn_thresholding_currframe->TabIndex = 11;
      this->m_btn_thresholding_currframe->Text = L"Thresholding";
      this->m_btn_thresholding_currframe->UseVisualStyleBackColor = true;
      this->m_btn_thresholding_currframe->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_thresholding_currframe_Clicked);
      // 
      // m_btn_regiongrow4d
      // 
      this->m_btn_regiongrow4d->Location = System::Drawing::Point(1, 160);
      this->m_btn_regiongrow4d->Name = L"m_btn_regiongrow4d";
      this->m_btn_regiongrow4d->Size = System::Drawing::Size(209, 32);
      this->m_btn_regiongrow4d->TabIndex = 16;
      this->m_btn_regiongrow4d->Text = L"Region Grow 4D";
      this->m_btn_regiongrow4d->UseVisualStyleBackColor = true;
      this->m_btn_regiongrow4d->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_regiongrow4d_Clicked);
      // 
      // m_btn_finish
      // 
      this->m_btn_finish->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10));
      this->m_btn_finish->Location = System::Drawing::Point(1, 203);
      this->m_btn_finish->Name = L"m_btn_finish";
      this->m_btn_finish->Size = System::Drawing::Size(120, 30);
      this->m_btn_finish->TabIndex = 26;
      this->m_btn_finish->Text = L"Finish and Store";
      this->m_btn_finish->UseVisualStyleBackColor = true;
      this->m_btn_finish->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_finish_Clicked);
      // 
      // m_btn_cancel
      // 
      this->m_btn_cancel->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10));
      this->m_btn_cancel->Location = System::Drawing::Point(127, 203);
      this->m_btn_cancel->Name = L"m_btn_cancel";
      this->m_btn_cancel->Size = System::Drawing::Size(83, 30);
      this->m_btn_cancel->TabIndex = 27;
      this->m_btn_cancel->Text = L"Cancel";
      this->m_btn_cancel->UseVisualStyleBackColor = true;
      this->m_btn_cancel->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_cancel_Clicked);
      // 
      // m_groupbox_allframe
      // 
      this->m_groupbox_allframe->Controls->Add(this->m_btn_regiongrow_allframe);
      this->m_groupbox_allframe->Controls->Add(this->m_btn_erode_allframe);
      this->m_groupbox_allframe->Controls->Add(this->m_btn_dilate_allframe);
      this->m_groupbox_allframe->Controls->Add(this->m_btn_thresholding_allframe);
      this->m_groupbox_allframe->Location = System::Drawing::Point(113, 11);
      this->m_groupbox_allframe->Name = L"m_groupbox_allframe";
      this->m_groupbox_allframe->Size = System::Drawing::Size(99, 143);
      this->m_groupbox_allframe->TabIndex = 25;
      this->m_groupbox_allframe->TabStop = false;
      this->m_groupbox_allframe->Text = L"AllFrame";
      // 
      // m_btn_regiongrow_allframe
      // 
      this->m_btn_regiongrow_allframe->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10));
      this->m_btn_regiongrow_allframe->Location = System::Drawing::Point(2, 49);
      this->m_btn_regiongrow_allframe->Name = L"m_btn_regiongrow_allframe";
      this->m_btn_regiongrow_allframe->Size = System::Drawing::Size(95, 30);
      this->m_btn_regiongrow_allframe->TabIndex = 15;
      this->m_btn_regiongrow_allframe->Text = L"Region Grow";
      this->m_btn_regiongrow_allframe->UseVisualStyleBackColor = true;
      this->m_btn_regiongrow_allframe->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_regiongrow_allframe_Clicked);
      // 
      // m_btn_erode_allframe
      // 
      this->m_btn_erode_allframe->Location = System::Drawing::Point(2, 85);
      this->m_btn_erode_allframe->Name = L"m_btn_erode_allframe";
      this->m_btn_erode_allframe->Size = System::Drawing::Size(95, 23);
      this->m_btn_erode_allframe->TabIndex = 14;
      this->m_btn_erode_allframe->Text = L"erode";
      this->m_btn_erode_allframe->UseVisualStyleBackColor = true;
      this->m_btn_erode_allframe->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_erode_allframe_Clicked);
      // 
      // m_btn_dilate_allframe
      // 
      this->m_btn_dilate_allframe->Location = System::Drawing::Point(2, 112);
      this->m_btn_dilate_allframe->Name = L"m_btn_dilate_allframe";
      this->m_btn_dilate_allframe->Size = System::Drawing::Size(95, 23);
      this->m_btn_dilate_allframe->TabIndex = 15;
      this->m_btn_dilate_allframe->Text = L"dilate";
      this->m_btn_dilate_allframe->UseVisualStyleBackColor = true;
      this->m_btn_dilate_allframe->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_dilate_allframe_Clicked);
      // 
      // m_btn_thresholding_allframe
      // 
      this->m_btn_thresholding_allframe->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10));
      this->m_btn_thresholding_allframe->Location = System::Drawing::Point(2, 17);
      this->m_btn_thresholding_allframe->Name = L"m_btn_thresholding_allframe";
      this->m_btn_thresholding_allframe->Size = System::Drawing::Size(95, 30);
      this->m_btn_thresholding_allframe->TabIndex = 12;
      this->m_btn_thresholding_allframe->Text = L"Thresholding";
      this->m_btn_thresholding_allframe->UseVisualStyleBackColor = true;
      this->m_btn_thresholding_allframe->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_thresholding_allframe_Clicked);
      // 
      // m_label_thresh
      // 
      this->m_label_thresh->AutoSize = true;
      this->m_label_thresh->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->m_label_thresh->Location = System::Drawing::Point(1, 8);
      this->m_label_thresh->Name = L"m_label_thresh";
      this->m_label_thresh->Size = System::Drawing::Size(78, 20);
      this->m_label_thresh->TabIndex = 28;
      this->m_label_thresh->Text = L"Threshold";
      // 
      // label8
      // 
      this->label8->AutoSize = true;
      this->label8->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
      this->label8->Location = System::Drawing::Point(2, 27);
      this->label8->Name = L"label8";
      this->label8->Size = System::Drawing::Size(31, 16);
      this->label8->TabIndex = 19;
      this->label8->Text = L"min";
      // 
      // label9
      // 
      this->label9->AutoSize = true;
      this->label9->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
      this->label9->Location = System::Drawing::Point(2, 53);
      this->label9->Name = L"label9";
      this->label9->Size = System::Drawing::Size(35, 16);
      this->label9->TabIndex = 18;
      this->label9->Text = L"max";
      // 
      // m_trackbar_lockareaband
      // 
      this->m_trackbar_lockareaband->Location = System::Drawing::Point(64, 95);
      this->m_trackbar_lockareaband->Name = L"m_trackbar_lockareaband";
      this->m_trackbar_lockareaband->Size = System::Drawing::Size(138, 45);
      this->m_trackbar_lockareaband->TabIndex = 40;
      this->m_trackbar_lockareaband->TickStyle = System::Windows::Forms::TickStyle::None;
      this->m_trackbar_lockareaband->Scroll += gcnew System::EventHandler(this, &FormSegBolus::m_trackbar_lockareaband_Scroll);
      // 
      // tabPage1
      // 
      this->tabPage1->BackColor = System::Drawing::Color::Transparent;
      this->tabPage1->Controls->Add(this->m_check_normalvolrend);
      this->tabPage1->Controls->Add(this->label5);
      this->tabPage1->Controls->Add(this->label4);
      this->tabPage1->Controls->Add(this->label3);
      this->tabPage1->Controls->Add(this->label2);
      this->tabPage1->Controls->Add(this->label1);
      this->tabPage1->Controls->Add(this->panel3);
      this->tabPage1->Controls->Add(this->panel1);
      this->tabPage1->Controls->Add(this->m_btn_loadcylinder);
      this->tabPage1->Controls->Add(this->m_btn_savecylinder);
      this->tabPage1->Controls->Add(this->m_btn_loadcylinder_fitting);
      this->tabPage1->Controls->Add(this->m_btn_createnewcylinder);
      this->tabPage1->Controls->Add(this->m_label_radius1);
      this->tabPage1->Controls->Add(this->m_check_drawobbs);
      this->tabPage1->Controls->Add(this->m_label_radius2);
      this->tabPage1->Controls->Add(this->m_label_twistangle);
      this->tabPage1->Controls->Add(this->m_label_transparency);
      this->tabPage1->Controls->Add(this->m_trackbar_cylinder_trans);
      this->tabPage1->Controls->Add(this->m_trackbar_cylinder_twistangle);
      this->tabPage1->Controls->Add(this->m_trackbar_cylinder_radius2);
      this->tabPage1->Controls->Add(this->m_trackbar_cylinder_radius1);
      this->tabPage1->Location = System::Drawing::Point(4, 22);
      this->tabPage1->Name = L"tabPage1";
      this->tabPage1->Padding = System::Windows::Forms::Padding(3);
      this->tabPage1->Size = System::Drawing::Size(219, 366);
      this->tabPage1->TabIndex = 0;
      this->tabPage1->Text = L"Edit Cylinder";
      // 
      // m_check_normalvolrend
      // 
      this->m_check_normalvolrend->AutoSize = true;
      this->m_check_normalvolrend->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10));
      this->m_check_normalvolrend->Location = System::Drawing::Point(90, 138);
      this->m_check_normalvolrend->Name = L"m_check_normalvolrend";
      this->m_check_normalvolrend->Size = System::Drawing::Size(107, 18);
      this->m_check_normalvolrend->TabIndex = 61;
      this->m_check_normalvolrend->Text = L"normal render";
      this->m_check_normalvolrend->UseVisualStyleBackColor = true;
      this->m_check_normalvolrend->CheckedChanged += gcnew System::EventHandler(this, &FormSegBolus::M_check_normalvolrend_CheckedChanged);
      // 
      // label5
      // 
      this->label5->AutoSize = true;
      this->label5->Location = System::Drawing::Point(5, 62);
      this->label5->Name = L"label5";
      this->label5->Size = System::Drawing::Size(196, 12);
      this->label5->TabIndex = 57;
      this->label5->Text = L"Shift + M dblclck --> Select Cylinder";
      // 
      // label4
      // 
      this->label4->AutoSize = true;
      this->label4->Location = System::Drawing::Point(5, 76);
      this->label4->Name = L"label4";
      this->label4->Size = System::Drawing::Size(174, 12);
      this->label4->TabIndex = 57;
      this->label4->Text = L"Shift + L drag --> move cp/seed";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Location = System::Drawing::Point(5, 48);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(211, 12);
      this->label3->TabIndex = 55;
      this->label3->Text = L"Shift + R dblclck --> Erase CP/cylinder";
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Location = System::Drawing::Point(117, 46);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(0, 12);
      this->label2->TabIndex = 54;
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Location = System::Drawing::Point(6, 33);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(200, 12);
      this->label1->TabIndex = 53;
      this->label1->Text = L"Shift + L dblclck --> Add cylinder/CP";
      // 
      // panel3
      // 
      this->panel3->BackColor = System::Drawing::Color::Aqua;
      this->panel3->Location = System::Drawing::Point(66, 193);
      this->panel3->Name = L"panel3";
      this->panel3->Size = System::Drawing::Size(10, 15);
      this->panel3->TabIndex = 52;
      // 
      // panel1
      // 
      this->panel1->BackColor = System::Drawing::Color::Yellow;
      this->panel1->Controls->Add(this->panel2);
      this->panel1->Location = System::Drawing::Point(66, 170);
      this->panel1->Name = L"panel1";
      this->panel1->Size = System::Drawing::Size(10, 15);
      this->panel1->TabIndex = 50;
      // 
      // panel2
      // 
      this->panel2->BackColor = System::Drawing::Color::Aqua;
      this->panel2->Location = System::Drawing::Point(0, 28);
      this->panel2->Name = L"panel2";
      this->panel2->Size = System::Drawing::Size(10, 14);
      this->panel2->TabIndex = 51;
      // 
      // m_btn_loadcylinder
      // 
      this->m_btn_loadcylinder->Location = System::Drawing::Point(0, 298);
      this->m_btn_loadcylinder->Name = L"m_btn_loadcylinder";
      this->m_btn_loadcylinder->Size = System::Drawing::Size(83, 24);
      this->m_btn_loadcylinder->TabIndex = 49;
      this->m_btn_loadcylinder->Text = L"Load Cylinder";
      this->m_btn_loadcylinder->UseVisualStyleBackColor = true;
      this->m_btn_loadcylinder->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_loadcylinder_Clicked);
      // 
      // m_btn_savecylinder
      // 
      this->m_btn_savecylinder->Location = System::Drawing::Point(0, 271);
      this->m_btn_savecylinder->Name = L"m_btn_savecylinder";
      this->m_btn_savecylinder->Size = System::Drawing::Size(94, 24);
      this->m_btn_savecylinder->TabIndex = 48;
      this->m_btn_savecylinder->Text = L"Save Cylinder";
      this->m_btn_savecylinder->UseVisualStyleBackColor = true;
      this->m_btn_savecylinder->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_savecylinder_Clicked);
      // 
      // m_btn_loadcylinder_fitting
      // 
      this->m_btn_loadcylinder_fitting->Location = System::Drawing::Point(87, 298);
      this->m_btn_loadcylinder_fitting->Name = L"m_btn_loadcylinder_fitting";
      this->m_btn_loadcylinder_fitting->Size = System::Drawing::Size(132, 24);
      this->m_btn_loadcylinder_fitting->TabIndex = 47;
      this->m_btn_loadcylinder_fitting->Text = L"Load Cylinder (auto fit)";
      this->m_btn_loadcylinder_fitting->UseVisualStyleBackColor = true;
      this->m_btn_loadcylinder_fitting->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_loadcylinder_fit_Clicked);
      // 
      // m_btn_createnewcylinder
      // 
      this->m_btn_createnewcylinder->Location = System::Drawing::Point(6, 93);
      this->m_btn_createnewcylinder->Name = L"m_btn_createnewcylinder";
      this->m_btn_createnewcylinder->Size = System::Drawing::Size(93, 29);
      this->m_btn_createnewcylinder->TabIndex = 46;
      this->m_btn_createnewcylinder->Text = L"create cylinder";
      this->m_btn_createnewcylinder->UseVisualStyleBackColor = true;
      this->m_btn_createnewcylinder->Click += gcnew System::EventHandler(this, &FormSegBolus::m_btn_createnewcylinder_Clicked);
      // 
      // m_label_radius1
      // 
      this->m_label_radius1->AutoSize = true;
      this->m_label_radius1->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
      this->m_label_radius1->Location = System::Drawing::Point(2, 191);
      this->m_label_radius1->Name = L"m_label_radius1";
      this->m_label_radius1->Size = System::Drawing::Size(65, 16);
      this->m_label_radius1->TabIndex = 45;
      this->m_label_radius1->Text = L"Radius 2";
      // 
      // m_check_drawobbs
      // 
      this->m_check_drawobbs->AutoSize = true;
      this->m_check_drawobbs->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 10));
      this->m_check_drawobbs->Location = System::Drawing::Point(4, 138);
      this->m_check_drawobbs->Name = L"m_check_drawobbs";
      this->m_check_drawobbs->Size = System::Drawing::Size(80, 18);
      this->m_check_drawobbs->TabIndex = 37;
      this->m_check_drawobbs->Text = L"show obb";
      this->m_check_drawobbs->UseVisualStyleBackColor = true;
      this->m_check_drawobbs->CheckedChanged += gcnew System::EventHandler(this, &FormSegBolus::M_check_drawobbs_CheckedChanged);
      // 
      // m_label_radius2
      // 
      this->m_label_radius2->AutoSize = true;
      this->m_label_radius2->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
      this->m_label_radius2->Location = System::Drawing::Point(2, 168);
      this->m_label_radius2->Name = L"m_label_radius2";
      this->m_label_radius2->Size = System::Drawing::Size(65, 16);
      this->m_label_radius2->TabIndex = 44;
      this->m_label_radius2->Text = L"Radius 1";
      // 
      // m_label_twistangle
      // 
      this->m_label_twistangle->AutoSize = true;
      this->m_label_twistangle->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
      this->m_label_twistangle->Location = System::Drawing::Point(1, 217);
      this->m_label_twistangle->Name = L"m_label_twistangle";
      this->m_label_twistangle->Size = System::Drawing::Size(82, 16);
      this->m_label_twistangle->TabIndex = 43;
      this->m_label_twistangle->Text = L"Twist angle";
      // 
      // m_label_transparency
      // 
      this->m_label_transparency->AutoSize = true;
      this->m_label_transparency->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12));
      this->m_label_transparency->Location = System::Drawing::Point(4, 249);
      this->m_label_transparency->Name = L"m_label_transparency";
      this->m_label_transparency->Size = System::Drawing::Size(97, 16);
      this->m_label_transparency->TabIndex = 42;
      this->m_label_transparency->Text = L"transparency";
      // 
      // m_trackbar_cylinder_trans
      // 
      this->m_trackbar_cylinder_trans->BackColor = System::Drawing::SystemColors::Control;
      this->m_trackbar_cylinder_trans->Location = System::Drawing::Point(102, 248);
      this->m_trackbar_cylinder_trans->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->m_trackbar_cylinder_trans->Maximum = 100;
      this->m_trackbar_cylinder_trans->Name = L"m_trackbar_cylinder_trans";
      this->m_trackbar_cylinder_trans->Size = System::Drawing::Size(114, 45);
      this->m_trackbar_cylinder_trans->TabIndex = 41;
      this->m_trackbar_cylinder_trans->TickStyle = System::Windows::Forms::TickStyle::None;
      this->m_trackbar_cylinder_trans->Scroll += gcnew System::EventHandler(this, &FormSegBolus::m_trackbar_cylinder_trans_Scroll);
      // 
      // m_trackbar_cylinder_twistangle
      // 
      this->m_trackbar_cylinder_twistangle->BackColor = System::Drawing::SystemColors::Control;
      this->m_trackbar_cylinder_twistangle->Location = System::Drawing::Point(82, 214);
      this->m_trackbar_cylinder_twistangle->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->m_trackbar_cylinder_twistangle->Maximum = 360;
      this->m_trackbar_cylinder_twistangle->Name = L"m_trackbar_cylinder_twistangle";
      this->m_trackbar_cylinder_twistangle->Size = System::Drawing::Size(129, 45);
      this->m_trackbar_cylinder_twistangle->TabIndex = 40;
      this->m_trackbar_cylinder_twistangle->TickStyle = System::Windows::Forms::TickStyle::None;
      this->m_trackbar_cylinder_twistangle->Scroll += gcnew System::EventHandler(this, &FormSegBolus::m_trackbar_cylinder_twistangle_Scroll);
      // 
      // m_trackbar_cylinder_radius2
      // 
      this->m_trackbar_cylinder_radius2->BackColor = System::Drawing::SystemColors::Control;
      this->m_trackbar_cylinder_radius2->Location = System::Drawing::Point(82, 190);
      this->m_trackbar_cylinder_radius2->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->m_trackbar_cylinder_radius2->Name = L"m_trackbar_cylinder_radius2";
      this->m_trackbar_cylinder_radius2->Size = System::Drawing::Size(129, 45);
      this->m_trackbar_cylinder_radius2->TabIndex = 39;
      this->m_trackbar_cylinder_radius2->TickStyle = System::Windows::Forms::TickStyle::None;
      this->m_trackbar_cylinder_radius2->Scroll += gcnew System::EventHandler(this, &FormSegBolus::m_trackbar_cylinder_radius2_Scroll);
      // 
      // m_trackbar_cylinder_radius1
      // 
      this->m_trackbar_cylinder_radius1->BackColor = System::Drawing::SystemColors::Control;
      this->m_trackbar_cylinder_radius1->Location = System::Drawing::Point(82, 165);
      this->m_trackbar_cylinder_radius1->Margin = System::Windows::Forms::Padding(5, 4, 5, 4);
      this->m_trackbar_cylinder_radius1->Name = L"m_trackbar_cylinder_radius1";
      this->m_trackbar_cylinder_radius1->Size = System::Drawing::Size(129, 45);
      this->m_trackbar_cylinder_radius1->TabIndex = 38;
      this->m_trackbar_cylinder_radius1->TickStyle = System::Windows::Forms::TickStyle::None;
      this->m_trackbar_cylinder_radius1->Scroll += gcnew System::EventHandler(this, &FormSegBolus::m_trackbar_cylinder_radius1_Scroll);
      // 
      // tabControl1
      // 
      this->tabControl1->Anchor = System::Windows::Forms::AnchorStyles::None;
      this->tabControl1->Controls->Add(this->tabPage1);
      this->tabControl1->Controls->Add(this->tabPage3);
      this->tabControl1->ItemSize = System::Drawing::Size(78, 18);
      this->tabControl1->Location = System::Drawing::Point(5, 9);
      this->tabControl1->Name = L"tabControl1";
      this->tabControl1->SelectedIndex = 0;
      this->tabControl1->Size = System::Drawing::Size(227, 392);
      this->tabControl1->TabIndex = 20;
      // 
      // FormSegBolus
      // 
      this->ClientSize = System::Drawing::Size(239, 402);
      this->Controls->Add(this->tabControl1);
      this->Name = L"FormSegBolus";
      this->tabPage3->ResumeLayout(false);
      this->tabPage3->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_maxthresh))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_minthresh))->EndInit();
      this->panel_segm->ResumeLayout(false);
      this->m_groupbox_currframe->ResumeLayout(false);
      this->m_groupbox_allframe->ResumeLayout(false);
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_lockareaband))->EndInit();
      this->tabPage1->ResumeLayout(false);
      this->tabPage1->PerformLayout();
      this->panel1->ResumeLayout(false);
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_cylinder_trans))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_cylinder_twistangle))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_cylinder_radius2))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_trackbar_cylinder_radius1))->EndInit();
      this->tabControl1->ResumeLayout(false);
      this->ResumeLayout(false);

    }

#pragma endregion


	//イベントハンドラ
	private: System::Void m_btn_finish_Clicked(System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btn_cancel_Clicked(System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btn_thresholding_allframe_Clicked (System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btn_dilate_allframe_Clicked       (System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btn_erode_allframe_Clicked        (System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btn_regiongrow_allframe_Clicked   (System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btn_thresholding_currframe_Clicked(System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btn_erode_currframe_Clicked       (System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btn_dilate_currframe_Clicked      (System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btn_regiongrow_currframe_Clicked  (System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btn_createnewcylinder_Clicked     (System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btn_savecylinder_Clicked          (System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btn_regiongrow4d_Clicked          (System::Object^  sender, System::EventArgs^  e);
  private: System::Void m_btn_loadcylinder_fit_Clicked      (System::Object^  sender, System::EventArgs^  e);
  private: System::Void m_btn_loadcylinder_Clicked          (System::Object^  sender, System::EventArgs^  e);
  private: System::Void m_textbox_minthresh_TextChanged     (System::Object^  sender, System::EventArgs^  e);
  private: System::Void m_textbox_maxthresh_TextChanged     (System::Object^  sender, System::EventArgs^  e); 
  private: System::Void m_trackbar_cylinder_radius1_Scroll  (System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_trackbar_cylinder_radius2_Scroll  (System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_trackbar_cylinder_twistangle_Scroll(System::Object^  sender, System::EventArgs^  e);
  private: System::Void m_trackbar_cylinder_trans_Scroll     (System::Object^  sender, System::EventArgs^  e);
  private: System::Void m_trackbar_minthresh_Scroll          (System::Object^  sender, System::EventArgs^  e);
  private: System::Void m_trackbar_maxthresh_Scroll          (System::Object^  sender, System::EventArgs^  e);
  private: System::Void m_trackbar_lockareaband_Scroll       (System::Object^  sender, System::EventArgs^  e);
  private: System::Void m_textbox_lockareaband_TextChanged   (System::Object^ sender, System::EventArgs^ e) ;
  private: System::Void M_check_drawobbs_CheckedChanged      (System::Object^  sender, System::EventArgs^  e);
  private: System::Void M_check_normalvolrend_CheckedChanged (System::Object^  sender, System::EventArgs^  e);
};

  //外部から呼び出すための関数郡
  inline void formSegBolus_Show()           { FormSegBolus::GetInst()->Show();      }
	inline void formSegBolus_Hide()           { FormSegBolus::GetInst()->Hide();      }
  inline bool formSegBolus_IsDrawOBB()      { return FormSegBolus::GetInst()->IsDrawOBB();       }
	inline bool formSegBolus_IsNormalVolRend(){ return FormSegBolus::GetInst()->IsNormalVolRend(); }
  inline int  formSegBolus_GetLockAreaBand(){ return FormSegBolus::GetInst()->GetLockAreaBand(); }

  inline float formSegBolus_GetCylinderTransparency(){
    return FormSegBolus::GetInst()->GetCylinderAlpha();
  }
  inline void formSegBolus_InitAllItems(
      int volmin, int volmax, float radmin, float radmax)
  { 
    FormSegBolus::GetInst()->InitAllItems(volmin, volmax, radmin, radmax);
  }
	inline void formSegBolus_SetItemsForCylinderPropaty(bool is_active, float radius1, float radius2, float angle){
		FormSegBolus::GetInst()->SetItemsForCylinderProparty( is_active, radius1, radius2, angle);
	}
}

