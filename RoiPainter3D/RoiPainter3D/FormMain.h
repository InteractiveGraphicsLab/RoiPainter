#pragma once

class OglForCLI;



namespace RoiPainter3D {

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
  private:

    OglForCLI *m_ogl;
    int        m_prevKeyID;
    System::Windows::Forms::Panel^ FormMainPanel;
    System::Windows::Forms::MenuStrip^ menuStrip1;
    System::Windows::Forms::ToolStripMenuItem^  segmentationThreshFieldPaintToolStripMenuItem;
    System::Windows::Forms::ToolStripSeparator^  toolStripSeparator4;
    System::Windows::Forms::ToolStripMenuItem^  refinementStrokeTrimingToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^  refinementVoxelPaintToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^  open3DVolumepvmSphToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^  exportROIsAsMesh;
    System::Windows::Forms::ToolStripMenuItem^  segmentationParallelWiresToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^  exportROIsAsBmpImage;
    System::Windows::Forms::ToolStripMenuItem^  exportAROIAsBmpImage;
    System::Windows::Forms::ToolStripMenuItem^  m_menuitem_subview_x;
    System::Windows::Forms::ToolStripMenuItem^  m_menuitem_subview_y;
    System::Windows::Forms::ToolStripMenuItem^  m_menuitem_subview_z;
    System::Windows::Forms::ToolStripMenuItem^  exportVolumeAsBmpSlicesToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^  exportVolumeXyToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^  exportVolumeYzToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^  exportVolumeZxToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ fileToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ open2DSlicesToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ modeToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ open2DSlicesdcmToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ open3DVolumetraw3DToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ open3DColumedcmToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ open3DVolumefavToolStripMenuItem;
    System::Windows::Forms::ToolStripSeparator^ toolStripSeparator1;
    System::Windows::Forms::ToolStripMenuItem^ saveMaskmskToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ loadMaskmskToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ saveMaskfavbToolStripMenuItem;
    System::Windows::Forms::ToolStripSeparator^ toolStripSeparator2;
    System::Windows::Forms::ToolStripMenuItem^ exportVolumeAsTraw3dssToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ visualizationStandardToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ visualizationMaskToolStripMenuItem;
    System::Windows::Forms::ToolStripSeparator^ toolStripSeparator3;
    System::Windows::Forms::ToolStripMenuItem^ segmentationToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ segmentationThresholdToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ segmentationGraphCutToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ segmentationToolStripMenuItem1;
    System::Windows::Forms::ToolStripMenuItem^ miscsToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ exportCurrentCameraPosToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ importCurrentCameraPosToolStripMenuItem;
    private: System::Windows::Forms::ToolStripMenuItem^ refinementSplitByPlaneToolStripMenuItem;



    static FormMain^ m_singleton;
    FormMain(void);
  public:
    static FormMain^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormMain();
      return m_singleton;
    }

  public:
    void redrawMainPanel();
    void initCameraPosition();
    void setBkColor(float r, float g, float b);
    OglForCLI *GetOgl(){ return m_ogl;}

  private:
    void initializeOtherForms();
    void replaceOtherForms();
    bool pickViewAngleIndicator(int x, int y);

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
    ~FormMain();

  protected:

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
            this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
            this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->open2DSlicesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->open2DSlicesdcmToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->open3DVolumetraw3DToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->open3DColumedcmToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->open3DVolumefavToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->open3DVolumepvmSphToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
            this->saveMaskmskToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->loadMaskmskToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
            this->saveMaskfavbToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->exportROIsAsMesh = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->exportROIsAsBmpImage = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->exportAROIAsBmpImage = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->exportVolumeAsTraw3dssToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->exportVolumeAsBmpSlicesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->exportVolumeXyToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->exportVolumeYzToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->exportVolumeZxToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->modeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->visualizationStandardToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->visualizationMaskToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
            this->segmentationThresholdToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->segmentationGraphCutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->segmentationToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->segmentationToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->segmentationThreshFieldPaintToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->segmentationParallelWiresToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->toolStripSeparator4 = (gcnew System::Windows::Forms::ToolStripSeparator());
            this->refinementStrokeTrimingToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->refinementVoxelPaintToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->miscsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->exportCurrentCameraPosToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->importCurrentCameraPosToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->m_menuitem_subview_x = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->m_menuitem_subview_y = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->m_menuitem_subview_z = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->refinementSplitByPlaneToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->menuStrip1->SuspendLayout();
            this->SuspendLayout();
            // 
            // FormMainPanel
            // 
            this->FormMainPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
                | System::Windows::Forms::AnchorStyles::Left)
                | System::Windows::Forms::AnchorStyles::Right));
            this->FormMainPanel->Location = System::Drawing::Point(2, 24);
            this->FormMainPanel->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
            this->FormMainPanel->Name = L"FormMainPanel";
            this->FormMainPanel->Size = System::Drawing::Size(666, 481);
            this->FormMainPanel->TabIndex = 0;
            this->FormMainPanel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &FormMain::FormMainPanel_Paint);
            this->FormMainPanel->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMainPanel_MouseDoubleClick);
            this->FormMainPanel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMainPanel_MouseDown);
            this->FormMainPanel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMainPanel_MouseMove);
            this->FormMainPanel->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMainPanel_MouseUp);
            this->FormMainPanel->Resize += gcnew System::EventHandler(this, &FormMain::FormMainPanel_Resize);
            // 
            // menuStrip1
            // 
            this->menuStrip1->ImageScalingSize = System::Drawing::Size(20, 20);
            this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
                this->fileToolStripMenuItem,
                    this->modeToolStripMenuItem, this->miscsToolStripMenuItem
            });
            this->menuStrip1->Location = System::Drawing::Point(0, 0);
            this->menuStrip1->Name = L"menuStrip1";
            this->menuStrip1->Padding = System::Windows::Forms::Padding(4, 2, 0, 2);
            this->menuStrip1->Size = System::Drawing::Size(668, 24);
            this->menuStrip1->TabIndex = 1;
            this->menuStrip1->Text = L"menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(16) {
                this->open2DSlicesToolStripMenuItem,
                    this->open2DSlicesdcmToolStripMenuItem, this->open3DVolumetraw3DToolStripMenuItem, this->open3DColumedcmToolStripMenuItem, this->open3DVolumefavToolStripMenuItem,
                    this->open3DVolumepvmSphToolStripMenuItem, this->toolStripSeparator1, this->saveMaskmskToolStripMenuItem, this->loadMaskmskToolStripMenuItem,
                    this->toolStripSeparator2, this->saveMaskfavbToolStripMenuItem, this->exportROIsAsMesh, this->exportROIsAsBmpImage, this->exportAROIAsBmpImage,
                    this->exportVolumeAsTraw3dssToolStripMenuItem, this->exportVolumeAsBmpSlicesToolStripMenuItem
            });
            this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
            this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
            this->fileToolStripMenuItem->Text = L"File";
            // 
            // open2DSlicesToolStripMenuItem
            // 
            this->open2DSlicesToolStripMenuItem->Name = L"open2DSlicesToolStripMenuItem";
            this->open2DSlicesToolStripMenuItem->Size = System::Drawing::Size(300, 22);
            this->open2DSlicesToolStripMenuItem->Text = L"open 2D slices (bmp, tif)";
            this->open2DSlicesToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::open2DSlicesToolStripMenuItem_Click);
            // 
            // open2DSlicesdcmToolStripMenuItem
            // 
            this->open2DSlicesdcmToolStripMenuItem->Name = L"open2DSlicesdcmToolStripMenuItem";
            this->open2DSlicesdcmToolStripMenuItem->Size = System::Drawing::Size(300, 22);
            this->open2DSlicesdcmToolStripMenuItem->Text = L"open 2D slices (dcm)";
            this->open2DSlicesdcmToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::open2DSlicesdcmToolStripMenuItem_Click);
            // 
            // open3DVolumetraw3DToolStripMenuItem
            // 
            this->open3DVolumetraw3DToolStripMenuItem->Name = L"open3DVolumetraw3DToolStripMenuItem";
            this->open3DVolumetraw3DToolStripMenuItem->Size = System::Drawing::Size(300, 22);
            this->open3DVolumetraw3DToolStripMenuItem->Text = L"open 3D volume (traw3D)";
            this->open3DVolumetraw3DToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::open3DVolumetraw3DToolStripMenuItem_Click);
            // 
            // open3DColumedcmToolStripMenuItem
            // 
            this->open3DColumedcmToolStripMenuItem->Name = L"open3DColumedcmToolStripMenuItem";
            this->open3DColumedcmToolStripMenuItem->Size = System::Drawing::Size(300, 22);
            this->open3DColumedcmToolStripMenuItem->Text = L"open 3D volume (dcm)";
            this->open3DColumedcmToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::open3DColumedcmToolStripMenuItem_Click);
            // 
            // open3DVolumefavToolStripMenuItem
            // 
            this->open3DVolumefavToolStripMenuItem->Name = L"open3DVolumefavToolStripMenuItem";
            this->open3DVolumefavToolStripMenuItem->Size = System::Drawing::Size(300, 22);
            this->open3DVolumefavToolStripMenuItem->Text = L"open 3D volume (fav)";
            this->open3DVolumefavToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::open3DVolumefavToolStripMenuItem_Click);
            // 
            // open3DVolumepvmSphToolStripMenuItem
            // 
            this->open3DVolumepvmSphToolStripMenuItem->Name = L"open3DVolumepvmSphToolStripMenuItem";
            this->open3DVolumepvmSphToolStripMenuItem->Size = System::Drawing::Size(300, 22);
            this->open3DVolumepvmSphToolStripMenuItem->Text = L"open 3D volume (pvm, sph)";
            this->open3DVolumepvmSphToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::open3DVolumepvmSphToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this->toolStripSeparator1->Name = L"toolStripSeparator1";
            this->toolStripSeparator1->Size = System::Drawing::Size(297, 6);
            // 
            // saveMaskmskToolStripMenuItem
            // 
            this->saveMaskmskToolStripMenuItem->Name = L"saveMaskmskToolStripMenuItem";
            this->saveMaskmskToolStripMenuItem->Size = System::Drawing::Size(300, 22);
            this->saveMaskmskToolStripMenuItem->Text = L"save mask (msk)";
            this->saveMaskmskToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::saveMaskmskToolStripMenuItem_Click);
            // 
            // loadMaskmskToolStripMenuItem
            // 
            this->loadMaskmskToolStripMenuItem->Name = L"loadMaskmskToolStripMenuItem";
            this->loadMaskmskToolStripMenuItem->Size = System::Drawing::Size(300, 22);
            this->loadMaskmskToolStripMenuItem->Text = L"load mask (msk)";
            this->loadMaskmskToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::loadMaskmskToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this->toolStripSeparator2->Name = L"toolStripSeparator2";
            this->toolStripSeparator2->Size = System::Drawing::Size(297, 6);
            // 
            // saveMaskfavbToolStripMenuItem
            // 
            this->saveMaskfavbToolStripMenuItem->Name = L"saveMaskfavbToolStripMenuItem";
            this->saveMaskfavbToolStripMenuItem->Size = System::Drawing::Size(300, 22);
            this->saveMaskfavbToolStripMenuItem->Text = L"export a ROI (mask ID) as fav ";
            this->saveMaskfavbToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::saveMaskfavbToolStripMenuItem_Click);
            // 
            // exportROIsAsMesh
            // 
            this->exportROIsAsMesh->Name = L"exportROIsAsMesh";
            this->exportROIsAsMesh->Size = System::Drawing::Size(300, 22);
            this->exportROIsAsMesh->Text = L"export some ROIs (mask IDs) as a obj mesh ";
            this->exportROIsAsMesh->Click += gcnew System::EventHandler(this, &FormMain::exportROIsAsMesh_Click);
            // 
            // exportROIsAsBmpImage
            // 
            this->exportROIsAsBmpImage->Name = L"exportROIsAsBmpImage";
            this->exportROIsAsBmpImage->Size = System::Drawing::Size(300, 22);
            this->exportROIsAsBmpImage->Text = L"export All ROIs (mask IDs) as color bmp";
            this->exportROIsAsBmpImage->Click += gcnew System::EventHandler(this, &FormMain::exportROIsAsBmpImage_Click);
            // 
            // exportAROIAsBmpImage
            // 
            this->exportAROIAsBmpImage->Name = L"exportAROIAsBmpImage";
            this->exportAROIAsBmpImage->Size = System::Drawing::Size(300, 22);
            this->exportAROIAsBmpImage->Text = L"export a ROI (mask ID) as binary bmp";
            this->exportAROIAsBmpImage->Click += gcnew System::EventHandler(this, &FormMain::exportAROIAsBmpImage_Click);
            // 
            // exportVolumeAsTraw3dssToolStripMenuItem
            // 
            this->exportVolumeAsTraw3dssToolStripMenuItem->Name = L"exportVolumeAsTraw3dssToolStripMenuItem";
            this->exportVolumeAsTraw3dssToolStripMenuItem->Size = System::Drawing::Size(300, 22);
            this->exportVolumeAsTraw3dssToolStripMenuItem->Text = L"export volume as traw3d_ss";
            this->exportVolumeAsTraw3dssToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::exportVolumeAsTraw3dssToolStripMenuItem_Click);
            // 
            // exportVolumeAsBmpSlicesToolStripMenuItem
            // 
            this->exportVolumeAsBmpSlicesToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
                this->exportVolumeXyToolStripMenuItem,
                    this->exportVolumeYzToolStripMenuItem, this->exportVolumeZxToolStripMenuItem
            });
            this->exportVolumeAsBmpSlicesToolStripMenuItem->Name = L"exportVolumeAsBmpSlicesToolStripMenuItem";
            this->exportVolumeAsBmpSlicesToolStripMenuItem->Size = System::Drawing::Size(300, 22);
            this->exportVolumeAsBmpSlicesToolStripMenuItem->Text = L"export volume as bmp slices";
            // 
            // exportVolumeXyToolStripMenuItem
            // 
            this->exportVolumeXyToolStripMenuItem->Name = L"exportVolumeXyToolStripMenuItem";
            this->exportVolumeXyToolStripMenuItem->Size = System::Drawing::Size(86, 22);
            this->exportVolumeXyToolStripMenuItem->Text = L"xy";
            this->exportVolumeXyToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::exportVolumeXyToolStripMenuItem_Click);
            // 
            // exportVolumeYzToolStripMenuItem
            // 
            this->exportVolumeYzToolStripMenuItem->Name = L"exportVolumeYzToolStripMenuItem";
            this->exportVolumeYzToolStripMenuItem->Size = System::Drawing::Size(86, 22);
            this->exportVolumeYzToolStripMenuItem->Text = L"yz";
            this->exportVolumeYzToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::exportVolumeYzToolStripMenuItem_Click);
            // 
            // exportVolumeZxToolStripMenuItem
            // 
            this->exportVolumeZxToolStripMenuItem->Name = L"exportVolumeZxToolStripMenuItem";
            this->exportVolumeZxToolStripMenuItem->Size = System::Drawing::Size(86, 22);
            this->exportVolumeZxToolStripMenuItem->Text = L"zx";
            this->exportVolumeZxToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::exportVolumeZxToolStripMenuItem_Click);
            // 
            // modeToolStripMenuItem
            // 
            this->modeToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(13) {
                this->visualizationStandardToolStripMenuItem,
                    this->visualizationMaskToolStripMenuItem, this->toolStripSeparator3, this->segmentationThresholdToolStripMenuItem, this->segmentationGraphCutToolStripMenuItem,
                    this->segmentationToolStripMenuItem, this->segmentationToolStripMenuItem1, this->segmentationThreshFieldPaintToolStripMenuItem,
                    this->segmentationParallelWiresToolStripMenuItem, this->toolStripSeparator4, this->refinementStrokeTrimingToolStripMenuItem,
                    this->refinementVoxelPaintToolStripMenuItem, this->refinementSplitByPlaneToolStripMenuItem
            });
            this->modeToolStripMenuItem->Name = L"modeToolStripMenuItem";
            this->modeToolStripMenuItem->Size = System::Drawing::Size(50, 20);
            this->modeToolStripMenuItem->Text = L"Mode";
            // 
            // visualizationStandardToolStripMenuItem
            // 
            this->visualizationStandardToolStripMenuItem->Name = L"visualizationStandardToolStripMenuItem";
            this->visualizationStandardToolStripMenuItem->Size = System::Drawing::Size(266, 22);
            this->visualizationStandardToolStripMenuItem->Text = L"Visualization Standard";
            this->visualizationStandardToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::visualizationStandardToolStripMenuItem_Click);
            // 
            // visualizationMaskToolStripMenuItem
            // 
            this->visualizationMaskToolStripMenuItem->Name = L"visualizationMaskToolStripMenuItem";
            this->visualizationMaskToolStripMenuItem->Size = System::Drawing::Size(266, 22);
            this->visualizationMaskToolStripMenuItem->Text = L"Visualization Mask";
            this->visualizationMaskToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::visualizationMaskToolStripMenuItem_Click);
            // 
            // toolStripSeparator3
            // 
            this->toolStripSeparator3->Name = L"toolStripSeparator3";
            this->toolStripSeparator3->Size = System::Drawing::Size(263, 6);
            // 
            // segmentationThresholdToolStripMenuItem
            // 
            this->segmentationThresholdToolStripMenuItem->Name = L"segmentationThresholdToolStripMenuItem";
            this->segmentationThresholdToolStripMenuItem->Size = System::Drawing::Size(266, 22);
            this->segmentationThresholdToolStripMenuItem->Text = L"Segmentation Region Growing";
            this->segmentationThresholdToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::segmentationThresholdToolStripMenuItem_Click);
            // 
            // segmentationGraphCutToolStripMenuItem
            // 
            this->segmentationGraphCutToolStripMenuItem->Name = L"segmentationGraphCutToolStripMenuItem";
            this->segmentationGraphCutToolStripMenuItem->Size = System::Drawing::Size(266, 22);
            this->segmentationGraphCutToolStripMenuItem->Text = L"Segmentation Graph Cut";
            this->segmentationGraphCutToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::segmentationGraphCutToolStripMenuItem_Click);
            // 
            // segmentationToolStripMenuItem
            // 
            this->segmentationToolStripMenuItem->Name = L"segmentationToolStripMenuItem";
            this->segmentationToolStripMenuItem->Size = System::Drawing::Size(266, 22);
            this->segmentationToolStripMenuItem->Text = L"Segmentation Voxel Paint";
            this->segmentationToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::segmentationToolStripMenuItem_Click);
            // 
            // segmentationToolStripMenuItem1
            // 
            this->segmentationToolStripMenuItem1->Name = L"segmentationToolStripMenuItem1";
            this->segmentationToolStripMenuItem1->Size = System::Drawing::Size(266, 22);
            this->segmentationToolStripMenuItem1->Text = L"Segmentation Local Region Growing";
            this->segmentationToolStripMenuItem1->Click += gcnew System::EventHandler(this, &FormMain::segmentationToolStripMenuItem1_Click);
            // 
            // segmentationThreshFieldPaintToolStripMenuItem
            // 
            this->segmentationThreshFieldPaintToolStripMenuItem->Name = L"segmentationThreshFieldPaintToolStripMenuItem";
            this->segmentationThreshFieldPaintToolStripMenuItem->Size = System::Drawing::Size(266, 22);
            this->segmentationThreshFieldPaintToolStripMenuItem->Text = L"Segmentation Thresh Field Paint";
            // 
            // segmentationParallelWiresToolStripMenuItem
            // 
            this->segmentationParallelWiresToolStripMenuItem->Name = L"segmentationParallelWiresToolStripMenuItem";
            this->segmentationParallelWiresToolStripMenuItem->Size = System::Drawing::Size(266, 22);
            this->segmentationParallelWiresToolStripMenuItem->Text = L"Segmentation Parallel Wires";
            this->segmentationParallelWiresToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::segmentationParallelWiresToolStripMenuItem_Click);
            // 
            // toolStripSeparator4
            // 
            this->toolStripSeparator4->Name = L"toolStripSeparator4";
            this->toolStripSeparator4->Size = System::Drawing::Size(263, 6);
            // 
            // refinementStrokeTrimingToolStripMenuItem
            // 
            this->refinementStrokeTrimingToolStripMenuItem->Name = L"refinementStrokeTrimingToolStripMenuItem";
            this->refinementStrokeTrimingToolStripMenuItem->Size = System::Drawing::Size(266, 22);
            this->refinementStrokeTrimingToolStripMenuItem->Text = L"Refinement Stroke Triming";
            this->refinementStrokeTrimingToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::refinementStrokeTrimingToolStripMenuItem_Click);
            // 
            // refinementVoxelPaintToolStripMenuItem
            // 
            this->refinementVoxelPaintToolStripMenuItem->Name = L"refinementVoxelPaintToolStripMenuItem";
            this->refinementVoxelPaintToolStripMenuItem->Size = System::Drawing::Size(266, 22);
            this->refinementVoxelPaintToolStripMenuItem->Text = L"Refinement Voxel Paint";
            this->refinementVoxelPaintToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::refinementVoxelPaintToolStripMenuItem_Click);
            // 
            // miscsToolStripMenuItem
            // 
            this->miscsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {
                this->exportCurrentCameraPosToolStripMenuItem,
                    this->importCurrentCameraPosToolStripMenuItem, this->m_menuitem_subview_x, this->m_menuitem_subview_y, this->m_menuitem_subview_z
            });
            this->miscsToolStripMenuItem->Name = L"miscsToolStripMenuItem";
            this->miscsToolStripMenuItem->Size = System::Drawing::Size(48, 20);
            this->miscsToolStripMenuItem->Text = L"miscs";
            // 
            // exportCurrentCameraPosToolStripMenuItem
            // 
            this->exportCurrentCameraPosToolStripMenuItem->Name = L"exportCurrentCameraPosToolStripMenuItem";
            this->exportCurrentCameraPosToolStripMenuItem->Size = System::Drawing::Size(213, 22);
            this->exportCurrentCameraPosToolStripMenuItem->Text = L"export current camera pos";
            // 
            // importCurrentCameraPosToolStripMenuItem
            // 
            this->importCurrentCameraPosToolStripMenuItem->Name = L"importCurrentCameraPosToolStripMenuItem";
            this->importCurrentCameraPosToolStripMenuItem->Size = System::Drawing::Size(213, 22);
            this->importCurrentCameraPosToolStripMenuItem->Text = L"import current camera pos";
            // 
            // m_menuitem_subview_x
            // 
            this->m_menuitem_subview_x->Name = L"m_menuitem_subview_x";
            this->m_menuitem_subview_x->Size = System::Drawing::Size(213, 22);
            this->m_menuitem_subview_x->Text = L"Show SubView X";
            this->m_menuitem_subview_x->Click += gcnew System::EventHandler(this, &FormMain::m_menuitem_subview_x_Click);
            // 
            // m_menuitem_subview_y
            // 
            this->m_menuitem_subview_y->Name = L"m_menuitem_subview_y";
            this->m_menuitem_subview_y->Size = System::Drawing::Size(213, 22);
            this->m_menuitem_subview_y->Text = L"Show SubView Y";
            this->m_menuitem_subview_y->Click += gcnew System::EventHandler(this, &FormMain::m_menuitem_subview_y_Click);
            // 
            // m_menuitem_subview_z
            // 
            this->m_menuitem_subview_z->Name = L"m_menuitem_subview_z";
            this->m_menuitem_subview_z->Size = System::Drawing::Size(213, 22);
            this->m_menuitem_subview_z->Text = L"Show SubView Z";
            this->m_menuitem_subview_z->Click += gcnew System::EventHandler(this, &FormMain::m_menuitem_subview_z_Click);
            // 
            // refinementSplitByPlaneToolStripMenuItem
            // 
            this->refinementSplitByPlaneToolStripMenuItem->Name = L"refinementSplitByPlaneToolStripMenuItem";
            this->refinementSplitByPlaneToolStripMenuItem->Size = System::Drawing::Size(266, 22);
            this->refinementSplitByPlaneToolStripMenuItem->Text = L"Refinement Split By Plane";
            this->refinementSplitByPlaneToolStripMenuItem->Click += gcnew System::EventHandler(this, &FormMain::refinementSplitByPlaneToolStripMenuItem_Click);
            // 
            // FormMain
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(668, 508);
            this->Controls->Add(this->FormMainPanel);
            this->Controls->Add(this->menuStrip1);
            this->MainMenuStrip = this->menuStrip1;
            this->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
            this->Name = L"FormMain";
            this->Text = L"FormMain";
            this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &FormMain::FormMain_KeyDown);
            this->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &FormMain::FormMain_KeyUp);
            this->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMain_MouseWheel);
            this->Move += gcnew System::EventHandler(this, &FormMain::FormMain_Move);
            this->Resize += gcnew System::EventHandler(this, &FormMain::FormMain_Resize);
            this->menuStrip1->ResumeLayout(false);
            this->menuStrip1->PerformLayout();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
  private: 
    System::Void FormMainPanel_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
    System::Void FormMainPanel_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
    System::Void FormMainPanel_MouseUp  (System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
    System::Void FormMainPanel_Resize   (System::Object^  sender, System::EventArgs^  e);
    System::Void FormMainPanel_Paint    (System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
    System::Void FormMainPanel_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);

    System::Void FormMain_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
    System::Void FormMain_Resize(System::Object^  sender, System::EventArgs^  e);
    System::Void FormMain_Move  (System::Object^  sender, System::EventArgs^  e);
    System::Void open2DSlicesToolStripMenuItem_Click      (System::Object^  sender, System::EventArgs^  e) ;
    System::Void open2DSlicesdcmToolStripMenuItem_Click   (System::Object^  sender, System::EventArgs^  e) ;
    System::Void open3DVolumetraw3DToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void open3DColumedcmToolStripMenuItem_Click   (System::Object^  sender, System::EventArgs^  e) ;
    System::Void open3DVolumefavToolStripMenuItem_Click   (System::Object^  sender, System::EventArgs^  e) ;
    System::Void open3DVolumepvmSphToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void saveMaskmskToolStripMenuItem_Click       (System::Object^  sender, System::EventArgs^  e);
    System::Void loadMaskmskToolStripMenuItem_Click       (System::Object^  sender, System::EventArgs^  e) ;
    System::Void saveMaskfavbToolStripMenuItem_Click      (System::Object^  sender, System::EventArgs^  e);
    System::Void exportVolumeAsTraw3dssToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) ;

    System::Void visualizationStandardToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void visualizationMaskToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void segmentationThresholdToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void segmentationGraphCutToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^   e) ;
    System::Void segmentationToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void segmentationToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void segmentationParallelWiresToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void refinementVoxelPaintToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void refinementStrokeTrimingToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void refinementSplitByPlaneToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);

    System::Void FormMain_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) ;
    System::Void FormMain_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) ;
    System::Void exportROIsAsMesh_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void exportROIsAsBmpImage_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void exportAROIAsBmpImage_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_menuitem_subview_x_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_menuitem_subview_y_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void m_menuitem_subview_z_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void exportVolumeXyToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void exportVolumeYzToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void exportVolumeZxToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) ;
};


  void FormMain_SetBkColor(float r, float g, float b);
  void RedrawScene(bool redraw_subview = true);

}

