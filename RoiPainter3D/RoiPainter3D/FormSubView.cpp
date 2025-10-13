#pragma managed
#include "FormSubView.h"
#include "FormMain.h"
#include "FormVisParam.h"

#pragma unmanaged
#include "ImageCore.h"
#include "CrsSecCore.h"
#include "ModeCore.h"
#include "ViewAngleCore.h"
#include "OglForCLI.h"

#pragma managed


using namespace RoiPainter3D;

FormSubView::FormSubView(FORM_SUB_ID id) : m_form_id(id)
{
  //std::cout << "FormSubView  " << (int) id << "!!!!!!!!!!!!!!!\n";
	InitializeComponent();
  m_cam = new OglCameraParam();

  if ( id == FORM_SUB_ID::FSI_X)
    m_cam->Set( EVec3f(10,0,0), EVec3f(0,0,0), EVec3f(0,1,0));
  if ( id == FORM_SUB_ID::FSI_Y)
    m_cam->Set( EVec3f(0,10,0), EVec3f(0,0,0), EVec3f(0,1,0));
  if ( id == FORM_SUB_ID::FSI_Z)
    m_cam->Set( EVec3f(0,0,10), EVec3f(0,0,0), EVec3f(0,1,0));

  m_dc = GetDC((HWND)m_panel->Handle.ToPointer());


  static PIXELFORMATDESCRIPTOR pfd =
  {
    sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
    1,                              // version number
    PFD_DRAW_TO_WINDOW |            // support window
    PFD_SUPPORT_OPENGL |            // support OpenGL
    PFD_DOUBLEBUFFER,               // double buffered
    PFD_TYPE_RGBA,                  // RGBA type
    32,                             // 24-bit color depth
    0, 0, 0, 0, 0, 0,               // color bits ignored
    0,                              // no alpha buffer
    0,                              // shift bit ignored
    0,                              // no accumulation buffer
    0, 0, 0, 0,                     // accum bits ignored
    32,                             // 32-bit z-buffer (or 16, 24 bit)
    0,                              // stencil buffer (no)
    0,                              // auxiliary buffer (no)
    PFD_MAIN_PLANE,                 // main layer
    0,                              // reserved
    0, 0, 0                         // layer masks ignored
  };

  int pfmt = ChoosePixelFormat(m_dc, &pfd);
  if (pfmt == 0) return;
  if (!SetPixelFormat(m_dc, pfmt, &pfd)) return;
  
  //std::cout << "DONE\n";
} 


void FormSubView::RedrawPanel()
{
  if ( !this->Visible ) return;

  OglForCLI *ogl = FormMain::getInst()->GetOgl();
  if ( ogl == 0 ) return;
   
  std::cout << "Redraw Sub" << (int)m_form_id << "  " << this->Visible << "\n"; 

  const EVec3f cuboid   = ImageCore::GetInst()->GetCuboid();
  const float  nearDist = (cuboid[0] + cuboid[1] + cuboid[2]) / 3.0f * 0.01f;
  const float  farDist  = (cuboid[0] + cuboid[1] + cuboid[2]) / 3.0f * 8;
  const int    viewW    = m_panel->Width;
  const int    viewH    = m_panel->Height;

  ogl->OnDrawBeginByOtherForm(m_dc, viewW, viewH, *m_cam, 45.0, nearDist, farDist);
  //initialize lights	
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);
  float p0[4] = {  1000,  1000, -1000, 1 };
  float p1[4] = { -1000,  1000, -1000, 1 };
  float p2[4] = {  1000, -1000, -1000, 1 };
  glLightfv(GL_LIGHT0, GL_POSITION, p0);
  glLightfv(GL_LIGHT1, GL_POSITION, p1);
  glLightfv(GL_LIGHT2, GL_POSITION, p2);

  if (FormVisParam::getInst()->bRendFrame()) t_DrawCuboidFrame(cuboid);

  ModeCore::GetInst()->DrawScene(cuboid, m_cam->m_pos, m_cam->m_cnt);

  if (FormVisParam::getInst()->bRendIndi())
  {
    ViewAngleCore::GetInst()->DrawIndicator(viewW, viewH, m_cam->m_pos, m_cam->m_cnt, m_cam->m_up );
  }
  
  ogl->OnDrawEndByOtherForm( m_dc );

  std::cout << "DONE !! \n"; 
  
}


void FormSubView::InitCamera()
{
  EVec3f pos, up;
  if ( m_form_id == FORM_SUB_ID::FSI_X){
    pos << 1, 0, 0;  
    up  << 0, 0, 1;
  }
  if ( m_form_id == FORM_SUB_ID::FSI_Y){
    pos << 0, 1, 0;
    up  << 0, 0, 1;
  }
  if ( m_form_id == FORM_SUB_ID::FSI_Z){
    pos << 0, 0, 1;  
    up  << 0,-1, 0;
  }

  EVec3f cuboid = ImageCore::GetInst()->GetCuboid();
  double  D  = cuboid.norm() * 1.0;
  EVec3f cnt = 0.5 * cuboid;
  pos = cnt + (float)D * pos;
  m_cam->Set(pos,cnt, up );
}


System::Void FormSubView::m_panel_MouseDown(
    System::Object^  sender, 
    System::Windows::Forms::MouseEventArgs^  e)
{
  if (e->Button == System::Windows::Forms::MouseButtons::Left  ) 
    m_bL = true;
  if (e->Button == System::Windows::Forms::MouseButtons::Middle) 
    m_bM = true;
  if (e->Button == System::Windows::Forms::MouseButtons::Right ) 
    m_bR = true;

  m_mouse_x = e->X;
  m_mouse_y = e->Y;

}


System::Void FormSubView::m_panel_MouseUp(
    System::Object^  sender, 
    System::Windows::Forms::MouseEventArgs^  e) 
{
  m_bL = m_bR = m_bM = false;  
}


System::Void FormSubView::m_panel_MouseMove(
    System::Object^  sender, 
    System::Windows::Forms::MouseEventArgs^  e)
{
  if ( !m_bL && !m_bR && !m_bM ) return;

  int dx = e->X - m_mouse_x;
  int dy = e->Y - m_mouse_y;
  if ( m_bL ) m_cam->TranslateCamera(dx,dy);
  if ( m_bM ) m_cam->ZoomCamera(dy);
  if ( m_bR ) m_cam->RotateCamera(dx,dy);

  m_mouse_x = e->X;
  m_mouse_y = e->Y;
  RedrawPanel();
}