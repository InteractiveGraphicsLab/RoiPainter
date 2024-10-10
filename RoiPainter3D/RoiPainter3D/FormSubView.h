#pragma once

#pragma unmanaged
class OglCameraParam;
#include <windows.h>
#undef min
#undef max
#pragma managed

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSubView の概要
	/// </summary>
	public ref class FormSubView : public System::Windows::Forms::Form
	{

private:
    enum class FORM_SUB_ID : int {
      FSI_X, FSI_Y, FSI_Z
    };
    const FORM_SUB_ID m_form_id;
    
    //インスタンスはもてないのでポインタで  
    OglCameraParam *m_cam;
    HDC m_dc;
    
    int  m_mouse_x, m_mouse_y;
    bool m_bL, m_bR, m_bM;

    //三個のインスタンスのみを生成
    static FormSubView ^m_form_x, ^m_form_y, ^m_form_z;

  public:
    static FormSubView^ getInstX()
    {
      if (m_form_x == nullptr) m_form_x = gcnew FormSubView( FORM_SUB_ID::FSI_X);
      return m_form_x;
    }
    static FormSubView^ getInstY()
    {
      if (m_form_y == nullptr) m_form_y = gcnew FormSubView( FORM_SUB_ID::FSI_Y);
      return m_form_y;
    }
    static FormSubView^ getInstZ()
    {
      if (m_form_z == nullptr) m_form_z = gcnew FormSubView( FORM_SUB_ID::FSI_Z);
      return m_form_z;
    }

  void RedrawPanel();
  void InitCamera();

	private:
		FormSubView(FORM_SUB_ID id);


	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormSubView()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::Panel^  m_panel;
  protected:

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
      this->m_panel = (gcnew System::Windows::Forms::Panel());
      this->SuspendLayout();
      // 
      // m_panel
      // 
      this->m_panel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
        | System::Windows::Forms::AnchorStyles::Left)
        | System::Windows::Forms::AnchorStyles::Right));
      this->m_panel->Location = System::Drawing::Point(2, 2);
      this->m_panel->Name = L"m_panel";
      this->m_panel->Size = System::Drawing::Size(552, 552);
      this->m_panel->TabIndex = 0;
      this->m_panel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &FormSubView::m_panel_Paint);
      this->m_panel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &FormSubView::m_panel_MouseDown);
      this->m_panel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &FormSubView::m_panel_MouseMove);
      this->m_panel->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &FormSubView::m_panel_MouseUp);
      // 
      // FormSubView
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(554, 557);
      this->Controls->Add(this->m_panel);
      this->Name = L"FormSubView";
      this->Text = L"FormSubView";
      this->ResumeLayout(false);

    }

#pragma endregion
  private: 
    System::Void m_panel_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
    {
      RedrawPanel();
    }
private: 
    System::Void m_panel_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
    System::Void m_panel_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) ;
    System::Void m_panel_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
};

  inline void FormSubViewX_Show(){
    FormSubView::getInstX()->Show();
  }
  inline void FormSubViewX_Hide(){
    FormSubView::getInstX()->Hide();
  }
  inline void FormSubViewY_Show(){
    FormSubView::getInstY()->Show();
  }
  inline void FormSubViewY_Hide(){
    FormSubView::getInstY()->Hide();
  }  
  inline void FormSubViewZ_Show(){
    FormSubView::getInstZ()->Show();
  }
  inline void FormSubViewZ_Hide(){
    FormSubView::getInstZ()->Hide();
  }
  inline void FormSubViewX_InitCamera(){
    FormSubView::getInstX()->InitCamera();
  }
  inline void FormSubViewY_InitCamera(){
    FormSubView::getInstY()->InitCamera();
  }
  inline void FormSubViewZ_InitCamera(){
    FormSubView::getInstZ()->InitCamera();
  }

}
