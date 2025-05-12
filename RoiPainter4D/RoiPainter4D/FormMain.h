#pragma once

class OglForCLI;

namespace RoiPainter4D {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  /// <summary>
  /// FormMain の概要
  /// </summary>
  public ref class FormMain : public System::Windows::Forms::Form
  {
    OglForCLI *m_ogl;

  private:
    System::Windows::Forms::Panel^ FormMainPanel;
    System::Windows::Forms::MenuStrip^ mainmenu;
    System::Windows::Forms::ToolStripMenuItem^ menu_file;
    System::Windows::Forms::ToolStripMenuItem^ modeSwitchToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ switch_visNorm;
    System::Windows::Forms::ToolStripMenuItem^ switch_visMask;
    System::Windows::Forms::ToolStripMenuItem^ switch_segPaint;
    System::Windows::Forms::ToolStripMenuItem^ switch_visRegionGrow;
    System::Windows::Forms::ToolStripMenuItem^ switch_segRigidICP;
    System::Windows::Forms::ToolStripMenuItem^ switch_segClosestPix;
    System::Windows::Forms::ToolStripMenuItem^ switch_refStrokeTrim;
    System::Windows::Forms::ToolStripMenuItem^ switch_segStrokeFFD;
    System::Windows::Forms::ToolStripMenuItem^ switch_segLocalRrow;
    System::Windows::Forms::ToolStripMenuItem^ switch_placeCPs;
    System::Windows::Forms::ToolStripMenuItem^ switch_RefCurveDeform;

    System::Windows::Forms::ToolStripMenuItem^ file_dcm3d;
    System::Windows::Forms::ToolStripMenuItem^ file_dcm2D;
    System::Windows::Forms::ToolStripMenuItem^ file_mha;
    System::Windows::Forms::ToolStripMenuItem^ file_traw3d;
    System::Windows::Forms::ToolStripMenuItem^ file_saveMask;
    System::Windows::Forms::ToolStripMenuItem^ file_saveMaskMha;
    System::Windows::Forms::ToolStripMenuItem^ file_saveMaskTrawub;
    System::Windows::Forms::ToolStripMenuItem^ file_loadMask;
    System::Windows::Forms::ToolStripMenuItem^ file_loadMaskMha;
    System::Windows::Forms::ToolStripMenuItem^ file_loadMaskTrawub;
    System::Windows::Forms::ToolStripMenuItem^ file_export4dct_mha;
    System::Windows::Forms::ToolStripMenuItem^ file_export4dct_trawss;

    System::Windows::Forms::ToolStripMenuItem^ switch_segBolus;
    System::Windows::Forms::ToolStripSeparator^ toolStripSeparator1;
    System::Windows::Forms::ToolStripSeparator^ toolStripSeparator2;
    System::Windows::Forms::ToolStripSeparator^ toolStripSeparator3;

    System::Windows::Forms::ProgressBar^ m_progressbar;
    System::Windows::Forms::ToolStripMenuItem^ switch_segSwallowOrgans;
    System::Windows::Forms::ToolStripMenuItem^ switch_segJointTracker;
    System::Windows::Forms::ToolStripMenuItem^ segBronchiToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ open3D8bitRawrawToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ switch_segSwallowTempGen;
    System::Windows::Forms::ToolStripMenuItem^ switch_segBronchi;

    System::Windows::Forms::ToolStripSeparator^ toolStripSeparator4;
    System::Windows::Forms::ToolStripSeparator^ toolStripSeparator5;
    System::Windows::Forms::ToolStripSeparator^ toolStripSeparator6;
  private: System::Windows::Forms::ToolStripMenuItem^ exportMaskCentroidcsvToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^ exportMaskEigenvaluecsvToolStripMenuItem;


  private:
    static FormMain^ m_singleton;
    FormMain(void);

  public:
    static FormMain^ GetInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormMain();
      return m_singleton;
    }

  private:
    // functions 
    int  m_prevKeyID;
    void InitializeSingletons();
    void ReplaceOtherForms();
    bool PickViewIndicator(int px, int py);
    void InitCameraPosition();

  public:
    OglForCLI* GetOgl();
    void RedrawMainPanel();
    void ActivateMainForm();
    void SetBackColor(float r, float g, float b, float a);
    void SetCursorNESW();
    void SetCursorDefault();
    void SetProgressValue(float value0to1);

  protected:
    /// <summary>
    /// 使用中のリソースをすべてクリーンアップします。
    /// </summary>
    ~FormMain();

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
      this->FormMainPanel = (gcnew System::Windows::Forms::Panel());
      this->mainmenu = (gcnew System::Windows::Forms::MenuStrip());
      this->menu_file = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->file_dcm3d = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->file_dcm2D = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->file_traw3d = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->file_mha = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->toolStripSeparator6 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->toolStripSeparator5 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->file_saveMask = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->file_saveMaskMha = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->file_saveMaskTrawub = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->file_loadMask = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->file_loadMaskMha = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->file_loadMaskTrawub = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->toolStripSeparator4 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->file_export4dct_trawss = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->file_export4dct_mha = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->open3D8bitRawrawToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->exportMaskCentroidcsvToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->exportMaskEigenvaluecsvToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->modeSwitchToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->switch_visNorm = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->switch_visMask = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->switch_segPaint = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->switch_visRegionGrow = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->switch_segRigidICP = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->switch_segLocalRrow = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->switch_segBolus = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->switch_refStrokeTrim = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->switch_segStrokeFFD = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->switch_segBronchi = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->switch_segSwallowOrgans = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->switch_segSwallowTempGen = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->switch_segClosestPix = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->switch_segJointTracker = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->switch_placeCPs = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->switch_RefCurveDeform = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->m_progressbar = (gcnew System::Windows::Forms::ProgressBar());
      this->mainmenu->SuspendLayout();
      this->SuspendLayout();
      // 
      // FormMainPanel
      // 
      this->FormMainPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
        | System::Windows::Forms::AnchorStyles::Left)
        | System::Windows::Forms::AnchorStyles::Right));
      this->FormMainPanel->Location = System::Drawing::Point(0, 27);
      this->FormMainPanel->Name = L"FormMainPanel";
      this->FormMainPanel->Size = System::Drawing::Size(751, 675);
      this->FormMainPanel->TabIndex = 0;
      this->FormMainPanel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &FormMain::FormMainPanel_Paint);
      this->FormMainPanel->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMainPanel_DblClick);
      this->FormMainPanel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMainPanel_MouseDown);
      this->FormMainPanel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMainPanel_MouseMove);
      this->FormMainPanel->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMainPanel_MouseUp);
      this->FormMainPanel->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMainPanel_MouseWheel);
      this->FormMainPanel->Resize += gcnew System::EventHandler(this, &FormMain::FormMainPanel_Resize);
      // 
      // mainmenu
      // 
      this->mainmenu->ImageScalingSize = System::Drawing::Size(24, 24);
      this->mainmenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) { this->menu_file, this->modeSwitchToolStripMenuItem });
      this->mainmenu->Location = System::Drawing::Point(0, 0);
      this->mainmenu->Name = L"mainmenu";
      this->mainmenu->Size = System::Drawing::Size(751, 24);
      this->mainmenu->TabIndex = 1;
      this->mainmenu->Text = L"menuStrip1";
      // 
      // menu_file
      // 
      this->menu_file->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(18) {
        this->file_dcm3d,
          this->file_dcm2D, this->file_traw3d, this->file_mha, this->toolStripSeparator6, this->toolStripSeparator5, this->file_saveMask,
          this->file_saveMaskMha, this->file_saveMaskTrawub, this->file_loadMask, this->file_loadMaskMha, this->file_loadMaskTrawub, this->toolStripSeparator4,
          this->file_export4dct_trawss, this->file_export4dct_mha, this->open3D8bitRawrawToolStripMenuItem, this->exportMaskCentroidcsvToolStripMenuItem,
          this->exportMaskEigenvaluecsvToolStripMenuItem
      });
      this->menu_file->Name = L"menu_file";
      this->menu_file->Size = System::Drawing::Size(37, 20);
      this->menu_file->Text = L"File";
      // 
      // file_dcm3d
      // 
      this->file_dcm3d->Name = L"file_dcm3d";
      this->file_dcm3d->Size = System::Drawing::Size(230, 22);
      this->file_dcm3d->Text = L"Open 4DCT (dcm 3D files)";
      this->file_dcm3d->Click += gcnew System::EventHandler(this, &FormMain::file_dcm3d_Click);
      // 
      // file_dcm2D
      // 
      this->file_dcm2D->Name = L"file_dcm2D";
      this->file_dcm2D->Size = System::Drawing::Size(230, 22);
      this->file_dcm2D->Text = L"Open 4DCT (dcm 2D files)";
      this->file_dcm2D->Click += gcnew System::EventHandler(this, &FormMain::file_dcm2D_Click);
      // 
      // file_traw3d
      // 
      this->file_traw3d->Name = L"file_traw3d";
      this->file_traw3d->Size = System::Drawing::Size(230, 22);
      this->file_traw3d->Text = L"Open 4DCT (traw3D files)";
      this->file_traw3d->Click += gcnew System::EventHandler(this, &FormMain::file_traw3d_Click);
      // 
      // file_mha
      // 
      this->file_mha->Name = L"file_mha";
      this->file_mha->Size = System::Drawing::Size(230, 22);
      this->file_mha->Text = L"Open 4DCT (mha files)";
      this->file_mha->Click += gcnew System::EventHandler(this, &FormMain::file_mha_Click);
      // 
      // toolStripSeparator6
      // 
      this->toolStripSeparator6->Name = L"toolStripSeparator6";
      this->toolStripSeparator6->Size = System::Drawing::Size(227, 6);
      // 
      // toolStripSeparator5
      // 
      this->toolStripSeparator5->Name = L"toolStripSeparator5";
      this->toolStripSeparator5->Size = System::Drawing::Size(227, 6);
      // 
      // file_saveMask
      // 
      this->file_saveMask->Name = L"file_saveMask";
      this->file_saveMask->Size = System::Drawing::Size(230, 22);
      this->file_saveMask->Text = L"Save Mask (.msk4)";
      this->file_saveMask->Click += gcnew System::EventHandler(this, &FormMain::file_saveMask_Click);
      // 
      // file_saveMaskMha
      // 
      this->file_saveMaskMha->Name = L"file_saveMaskMha";
      this->file_saveMaskMha->Size = System::Drawing::Size(230, 22);
      this->file_saveMaskMha->Text = L"Save Mask (.mha)";
      this->file_saveMaskMha->Click += gcnew System::EventHandler(this, &FormMain::file_saveMaskMha_Click);
      // 
      // file_saveMaskTrawub
      // 
      this->file_saveMaskTrawub->Name = L"file_saveMaskTrawub";
      this->file_saveMaskTrawub->Size = System::Drawing::Size(230, 22);
      this->file_saveMaskTrawub->Text = L"Save Mask (traw_ub)";
      this->file_saveMaskTrawub->Click += gcnew System::EventHandler(this, &FormMain::file_saveMasktrawub_Click);
      // 
      // file_loadMask
      // 
      this->file_loadMask->Name = L"file_loadMask";
      this->file_loadMask->Size = System::Drawing::Size(230, 22);
      this->file_loadMask->Text = L"Load Mask (.msk4)";
      this->file_loadMask->Click += gcnew System::EventHandler(this, &FormMain::file_loadMask_Click);
      // 
      // file_loadMaskMha
      // 
      this->file_loadMaskMha->Name = L"file_loadMaskMha";
      this->file_loadMaskMha->Size = System::Drawing::Size(230, 22);
      this->file_loadMaskMha->Text = L"Load Mask (.mha)";
      this->file_loadMaskMha->Click += gcnew System::EventHandler(this, &FormMain::file_loadMaskMha_Click);
      // 
      // file_loadMaskTrawub
      // 
      this->file_loadMaskTrawub->Name = L"file_loadMaskTrawub";
      this->file_loadMaskTrawub->Size = System::Drawing::Size(230, 22);
      this->file_loadMaskTrawub->Text = L"Load Mask (traw_ub)";
      this->file_loadMaskTrawub->Click += gcnew System::EventHandler(this, &FormMain::file_loadMasktrawub_Click);
      // 
      // toolStripSeparator4
      // 
      this->toolStripSeparator4->Name = L"toolStripSeparator4";
      this->toolStripSeparator4->Size = System::Drawing::Size(227, 6);
      // 
      // file_export4dct_trawss
      // 
      this->file_export4dct_trawss->Name = L"file_export4dct_trawss";
      this->file_export4dct_trawss->Size = System::Drawing::Size(230, 22);
      this->file_export4dct_trawss->Text = L"Export 4DCT (.traw_ss)";
      this->file_export4dct_trawss->Click += gcnew System::EventHandler(this, &FormMain::file_export4dcttraw_Click);
      // 
      // file_export4dct_mha
      // 
      this->file_export4dct_mha->Name = L"file_export4dct_mha";
      this->file_export4dct_mha->Size = System::Drawing::Size(230, 22);
      this->file_export4dct_mha->Text = L"Export 4DCT (.mha)";
      this->file_export4dct_mha->Click += gcnew System::EventHandler(this, &FormMain::file_export4dct_mha_Click);
      // 
      // open3D8bitRawrawToolStripMenuItem
      // 
      this->open3D8bitRawrawToolStripMenuItem->Name = L"open3D8bitRawrawToolStripMenuItem";
      this->open3D8bitRawrawToolStripMenuItem->Size = System::Drawing::Size(230, 22);
      this->open3D8bitRawrawToolStripMenuItem->Text = L"Open 3D 8bit raw (.raw)";
      this->open3D8bitRawrawToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::file_raw8bit_Click);
      // 
      // exportMaskCentroidcsvToolStripMenuItem
      // 
      this->exportMaskCentroidcsvToolStripMenuItem->Name = L"exportMaskCentroidcsvToolStripMenuItem";
      this->exportMaskCentroidcsvToolStripMenuItem->Size = System::Drawing::Size(230, 22);
      this->exportMaskCentroidcsvToolStripMenuItem->Text = L"Export Mask Centroid (.csv)";
      this->exportMaskCentroidcsvToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::exportMaskCentroidcsvToolStripMenuItem_Click);
      // 
      // exportMaskEigenvaluecsvToolStripMenuItem
      // 
      this->exportMaskEigenvaluecsvToolStripMenuItem->Name = L"exportMaskEigenvaluecsvToolStripMenuItem";
      this->exportMaskEigenvaluecsvToolStripMenuItem->Size = System::Drawing::Size(230, 22);
      this->exportMaskEigenvaluecsvToolStripMenuItem->Text = L"Export Mask Eigenvalue (.csv)";
      this->exportMaskEigenvaluecsvToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::exportMaskEigenvaluecsvToolStripMenuItem_Click);
      // 
      // modeSwitchToolStripMenuItem
      // 
      this->modeSwitchToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(19) {
        this->switch_visNorm,
          this->switch_visMask, this->toolStripSeparator1, this->switch_segPaint, this->switch_visRegionGrow, this->switch_segRigidICP,
          this->switch_segLocalRrow, this->switch_segBolus, this->switch_segSwallowOrgans, this->switch_segStrokeFFD, this->toolStripSeparator2,
          this->switch_refStrokeTrim, this->switch_RefCurveDeform, this->toolStripSeparator3, this->switch_placeCPs, this->switch_segBronchi,
          this->switch_segSwallowTempGen, this->switch_segClosestPix, this->switch_segJointTracker
      });
      this->modeSwitchToolStripMenuItem->Name = L"modeSwitchToolStripMenuItem";
      this->modeSwitchToolStripMenuItem->Size = System::Drawing::Size(86, 20);
      this->modeSwitchToolStripMenuItem->Text = L"mode switch";
      // 
      // switch_visNorm
      // 
      this->switch_visNorm->Name = L"switch_visNorm";
      this->switch_visNorm->Size = System::Drawing::Size(304, 22);
      this->switch_visNorm->Text = L"Vis Normal";
      this->switch_visNorm->Click += gcnew System::EventHandler(this, &FormMain::switch_visNorm_Click);
      // 
      // switch_visMask
      // 
      this->switch_visMask->Name = L"switch_visMask";
      this->switch_visMask->Size = System::Drawing::Size(304, 22);
      this->switch_visMask->Text = L"Vis Mask";
      this->switch_visMask->Click += gcnew System::EventHandler(this, &FormMain::switch_visMask_Click);
      // 
      // toolStripSeparator1
      // 
      this->toolStripSeparator1->Name = L"toolStripSeparator1";
      this->toolStripSeparator1->Size = System::Drawing::Size(301, 6);
      // 
      // switch_segPaint
      // 
      this->switch_segPaint->Name = L"switch_segPaint";
      this->switch_segPaint->Size = System::Drawing::Size(304, 22);
      this->switch_segPaint->Text = L"Seg Paint and Lasso";
      this->switch_segPaint->Click += gcnew System::EventHandler(this, &FormMain::switch_segPaint_Click);
      // 
      // switch_visRegionGrow
      // 
      this->switch_visRegionGrow->Name = L"switch_visRegionGrow";
      this->switch_visRegionGrow->Size = System::Drawing::Size(304, 22);
      this->switch_visRegionGrow->Text = L"Seg Threshold and Region grow";
      this->switch_visRegionGrow->Click += gcnew System::EventHandler(this, &FormMain::switch_visRegionGrow_Click);
      // 
      // switch_segRigidICP
      // 
      this->switch_segRigidICP->Name = L"switch_segRigidICP";
      this->switch_segRigidICP->Size = System::Drawing::Size(304, 22);
      this->switch_segRigidICP->Text = L"Seg Rigid Tracking (ICP)";
      this->switch_segRigidICP->Click += gcnew System::EventHandler(this, &FormMain::switch_segRigidICP_Click);
      // 
      // switch_segLocalRrow
      // 
      this->switch_segLocalRrow->Name = L"switch_segLocalRrow";
      this->switch_segLocalRrow->Size = System::Drawing::Size(304, 22);
      this->switch_segLocalRrow->Text = L"Seg Local Region Growing (Sphere)";
      this->switch_segLocalRrow->Click += gcnew System::EventHandler(this, &FormMain::switch_segLocalRrow_Click);
      // 
      // switch_segBolus
      // 
      this->switch_segBolus->Name = L"switch_segBolus";
      this->switch_segBolus->Size = System::Drawing::Size(304, 22);
      this->switch_segBolus->Text = L"Seg Local Region Growing (curved cylinder)";
      this->switch_segBolus->Click += gcnew System::EventHandler(this, &FormMain::switch_SegBolus_Click);
      // 
      // toolStripSeparator2
      // 
      this->toolStripSeparator2->Name = L"toolStripSeparator2";
      this->toolStripSeparator2->Size = System::Drawing::Size(301, 6);
      // 
      // switch_refStrokeTrim
      // 
      this->switch_refStrokeTrim->Name = L"switch_refStrokeTrim";
      this->switch_refStrokeTrim->Size = System::Drawing::Size(304, 22);
      this->switch_refStrokeTrim->Text = L"Ref Stroke Trim";
      this->switch_refStrokeTrim->Click += gcnew System::EventHandler(this, &FormMain::switch_refStrokeTrim_Click);
      // 
      // toolStripSeparator3
      // 
      this->toolStripSeparator3->Name = L"toolStripSeparator3";
      this->toolStripSeparator3->Size = System::Drawing::Size(301, 6);
      // 
      // switch_segStrokeFFD
      // 
      this->switch_segStrokeFFD->Name = L"switch_segStrokeFFD";
      this->switch_segStrokeFFD->Size = System::Drawing::Size(304, 22);
      this->switch_segStrokeFFD->Text = L"Seg Swallow Organs (Stroke FFD)";
      this->switch_segStrokeFFD->Click += gcnew System::EventHandler(this, &FormMain::switch_segStrokeFFD_Click);
      // 
      // switch_segBronchi
      // 
      this->switch_segBronchi->Name = L"switch_segBronchi";
      this->switch_segBronchi->Size = System::Drawing::Size(304, 22);
      this->switch_segBronchi->Text = L"Seg Bronchi";
      this->switch_segBronchi->Click += gcnew System::EventHandler(this, &FormMain::switch_segBronchi_Click);
      // 
      // switch_segSwallowOrgans
      // 
      this->switch_segSwallowOrgans->Name = L"switch_segSwallowOrgans";
      this->switch_segSwallowOrgans->Size = System::Drawing::Size(304, 22);
      this->switch_segSwallowOrgans->Text = L"Seg Swallow Organs (FFD)";
      this->switch_segSwallowOrgans->Click += gcnew System::EventHandler(this, &FormMain::switch_segSwallowOrgans_Click);
      // 
      // switch_segSwallowTempGen
      // 
      this->switch_segSwallowTempGen->Name = L"switch_segSwallowTempGen";
      this->switch_segSwallowTempGen->Size = System::Drawing::Size(304, 22);
      this->switch_segSwallowTempGen->Text = L"Seg Swallow Template Gen";
      this->switch_segSwallowTempGen->Click += gcnew System::EventHandler(this, &FormMain::switch_segSwallowTempGen_Click);
      // 
      // switch_segClosestPix
      // 
      this->switch_segClosestPix->Enabled = false;
      this->switch_segClosestPix->Name = L"switch_segClosestPix";
      this->switch_segClosestPix->Size = System::Drawing::Size(304, 22);
      this->switch_segClosestPix->Text = L"Seg Closest Pixel (Sprout Project)";
      this->switch_segClosestPix->Click += gcnew System::EventHandler(this, &FormMain::switch_segClosestPix_Click);
      // 
      // switch_segJointTracker
      // 
      this->switch_segJointTracker->Enabled = false;
      this->switch_segJointTracker->Name = L"switch_segJointTracker";
      this->switch_segJointTracker->Size = System::Drawing::Size(304, 22);
      this->switch_segJointTracker->Text = L"Seg Joint Tracker";
      this->switch_segJointTracker->Click += gcnew System::EventHandler(this, &FormMain::switch_segJointTracker_Click);
      // 
      // switch_placeCPs
      // 
      this->switch_placeCPs->Name = L"switch_placeCPs";
      this->switch_placeCPs->Size = System::Drawing::Size(304, 22);
      this->switch_placeCPs->Text = L"Place Points";
      this->switch_placeCPs->Click += gcnew System::EventHandler(this, &FormMain::switch_placeCPs_Click);
      // 
      // switch_RefCurveDeform
      // 
      this->switch_RefCurveDeform->Name = L"switch_RefCurveDeform";
      this->switch_RefCurveDeform->Size = System::Drawing::Size(304, 22);
      this->switch_RefCurveDeform->Text = L"Ref Curve Deform";
      this->switch_RefCurveDeform->Click += gcnew System::EventHandler(this, &FormMain::switch_RefCurveDeform_Click);
      // 
      // m_progressbar
      // 
      this->m_progressbar->Location = System::Drawing::Point(632, 0);
      this->m_progressbar->Name = L"m_progressbar";
      this->m_progressbar->Size = System::Drawing::Size(118, 23);
      this->m_progressbar->TabIndex = 2;
      // 
      // FormMain
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(751, 702);
      this->Controls->Add(this->m_progressbar);
      this->Controls->Add(this->FormMainPanel);
      this->Controls->Add(this->mainmenu);
      this->MainMenuStrip = this->mainmenu;
      this->Name = L"FormMain";
      this->Text = L"FormMain";
      this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &FormMain::FormMain_KeyDown);
      this->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &FormMain::FormMain_KeyPress);
      this->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &FormMain::FormMain_KeyUp);
      this->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMain_MouseWheel);
      this->Move += gcnew System::EventHandler(this, &FormMain::FormMain_Move);
      this->Resize += gcnew System::EventHandler(this, &FormMain::FormMain_Resize);
      this->mainmenu->ResumeLayout(false);
      this->mainmenu->PerformLayout();
      this->ResumeLayout(false);
      this->PerformLayout();

    }

#pragma endregion

    //以下 イベントハンドラ
  private:
    System::Void FormMainPanel_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e);
    System::Void FormMainPanel_Resize(System::Object^ sender, System::EventArgs^ e);
    System::Void FormMainPanel_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
    System::Void FormMainPanel_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
    System::Void FormMainPanel_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
    System::Void FormMainPanel_DblClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
    System::Void FormMain_MouseWheel(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
    System::Void FormMainPanel_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {}

    System::Void FormMain_Move(System::Object^  sender, System::EventArgs^  e);
    System::Void FormMain_Resize(System::Object^  sender, System::EventArgs^  e);
    //Open 4DCT 
    System::Void file_dcm3d_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void file_dcm2D_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void file_traw3d_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void file_mha_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void file_export4dcttraw_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void file_export4dct_mha_Click(System::Object^ sender, System::EventArgs^ e);
    //Save Mask 
    System::Void file_saveMask_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void file_saveMaskMha_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void file_saveMasktrawub_Click(System::Object^ sender, System::EventArgs^ e);
    //Load Mask
    System::Void file_loadMask_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void file_loadMaskMha_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void file_loadMasktrawub_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void file_raw8bit_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void exportMaskCentroidcsvToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void exportMaskEigenvaluecsvToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);

    System::Void switch_visNorm_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_visMask_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_visRegionGrow_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_segPaint_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_segRigidICP_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_segClosestPix_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_segLocalRrow_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_segSwallowOrgans_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_deformHC_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_segJointTracker_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void switch_segSwallowTempGen_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void switch_segBronchi_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void switch_segStrokeFFD_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void switch_placeCPs_Click(System::Object^ sender, System::EventArgs^ e);
    System::Void switch_RefCurveDeform_Click(System::Object^ sender, System::EventArgs^ e);

  //edit shionozaki
		System::Void switch_SegBolus_Click(System::Object^  sender, System::EventArgs^  e) ;
		System::Void switch_refStrokeTrim_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void FormMain_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
    System::Void FormMain_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
    System::Void FormMain_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
};

  inline void formMain_RedrawMainPanel()
  {
    FormMain::GetInst()->RedrawMainPanel();
  }

  inline void formMain_ActivateMainForm()
  {
    FormMain::GetInst()->ActivateMainForm();
  }

  inline void formMain_setBackColorBlack(float r, float g, float b, float a)
  {
    FormMain::GetInst()->SetBackColor(r, g, b, a);
  }

  inline void formMain_setCursorNESW()
  {
    FormMain::GetInst()->SetCursorNESW();
  }

  inline void formMain_setCursorDefault()
  {
    FormMain::GetInst()->SetCursorDefault();
  }

  inline OglForCLI* formMain_GetOgl(){
    return FormMain::GetInst()->GetOgl();
  }

  inline void formMain_SetProgressValue(float value0to1)
  {
    FormMain::GetInst()->SetProgressValue(value0to1);
  }
}
