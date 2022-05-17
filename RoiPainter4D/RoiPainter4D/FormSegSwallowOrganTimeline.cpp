#pragma managed
#include "FormSegSwallowOrganTimeline.h"
#include "FormVisParam.h"
#include "FormMain.h"

#pragma unmanaged
#include "Mode/ModeSegSwallowOrgans.h"
#include "Mode/ModeInterface.h"
#include "ImageCore.h"
#pragma managed

using namespace RoiPainter4D;
static int CP_RADIUS = 5;

//repaint should be called at 
// + ModeSegSwqllowOrgan when modification 
// + FormVisPanel        when timeline is modified (this could be slightly complicated. How to implement??) 
// + FormSegSwqllowOrganTimeline(*this) when mouse is clicked
// + 


//todo
// show this dialog
// show charts
// mouse manipuration (selection)
// remove copy function from maindialog
//

//移動前の gc sequence (右クリックキャンセル用)
static std::vector<EVec3f> m_prev_centers; 

//double buffering用のハック
//https://www.codeproject.com/Questions/226895/Double-buffered-painting-of-a-panel
public ref class MyPanel : public System::Windows::Forms::Panel
{
public:
    void SetStyle(System::Windows::Forms::ControlStyles flag, System::Boolean value)
    {
        System::Windows::Forms::Control::SetStyle(flag, value);
    }
};



FormSegSwallowOrganTimeline::FormSegSwallowOrganTimeline(void)
{
	InitializeComponent();
  m_background = nullptr;

  m_pick_fi = m_pick_xyz  = -1;
  m_selectx0 = m_selectx1 = -1;   

  m_bLbtn = false;
  //double buffering用のハック
  static_cast<MyPanel^>(m_panel)->SetStyle(ControlStyles::AllPaintingInWmPaint, true);
  static_cast<MyPanel^>(m_panel)->SetStyle(ControlStyles::DoubleBuffer, true);
}



static float CHART_W_RATE = 0.90f;
static float CHART_W_OFST = 0.05f;
static float CHART_H_RATE = 0.30f;
static float CHART_H_OFST = 0.05f;
/*
  -------------------------------------
  +------------------------------------  x axis
  -------------------------------------

  offset = H * OffsetRate 

  -------------------------------------
  +------------------------------------  y axis
  -------------------------------------

  offset = H * OffsetRate

  ------------------------------------- 
  +------------------------------------  z axis 
  -------------------------------------
*/


// calc fi --> time (横軸) of T-X, T-Y, T-Z chart 
inline int ChartPos_Time(
    const int panelW    , 
    const int num_frames,
    const int frame_idx 
)
{
  const int chart_w = (int)(panelW * CHART_W_RATE);  
  const int x_ofst  = (int)(panelW * CHART_W_OFST);
  int fi = (int)( frame_idx / (float) (num_frames -1)* chart_w) + x_ofst;
  return fi;
}


// pos in 3D --> pos in 2d chart
inline EVec3i ChartPos_XYZ(
    const int panelH    ,
    const EVec3f &cuboid, 
    const EVec3f &pos3d , 
    const float  &v_axis_rate)
{
  const int chart_h = (int)(panelH * CHART_H_RATE);  
  const int y_ofst  = (int)(panelH * CHART_H_OFST);

  float x_scale = 1.0f / (cuboid[0]/2) * (chart_h / 2) * v_axis_rate;
  float y_scale = 1.0f / (cuboid[1]/2) * (chart_h / 2) * v_axis_rate;
  float z_scale = 1.0f / (cuboid[2]/2) * (chart_h / 2) * v_axis_rate;

  int  x  = chart_h / 2            - (int)( (pos3d[0]-cuboid[0]/2 ) * x_scale);
  int  y  = 3*chart_h/2 + y_ofst   - (int)( (pos3d[1]-cuboid[1]/2 ) * y_scale);
  int  z  = 5*chart_h/2 + 2*y_ofst - (int)( (pos3d[2]-cuboid[2]/2 ) * z_scale);
  return EVec3i(x,y,z);
}


//  pos in 2d chart --> pos in 3D 
inline float Calc3DPos_XYZ
(
    const int H, 
    const EVec3f &cuboid,
    const int     posy_panel,
    const int     xyz, 
    const float   v_axis_rate
)
{
  const int y_ofst  = (int)(H * CHART_H_OFST);
  const int chart_h = (int)(H * CHART_H_RATE);  
  //const EVec3i bases( , , );

  int base = ( xyz == 0 ) ?   chart_h / 2          : 
             ( xyz == 1 ) ? 3*chart_h / 2 +   y_ofst :   
                            5*chart_h / 2 + 2*y_ofst ; 

  float xyz_scale = 1.0f / (cuboid[xyz]/2) * (chart_h / 2) * v_axis_rate;
  return -(posy_panel - base) / xyz_scale + cuboid[xyz] / 2;
}





inline std::vector<int> CalcSelectFIDs(
  const int W, 
  const int num_frames,
  const int selection_x0,
  const int selection_x1 )
{
  const int minv = std::min( selection_x0, selection_x1);
  const int maxv = std::max( selection_x0, selection_x1);

  std::vector<int> ids;
  for ( int fi = 0; fi < num_frames; ++fi)
  {
    int t = ChartPos_Time(W, num_frames, fi);
    if ( minv <= t && t <= maxv ) ids.push_back( fi );
  }
  return ids;
}

inline bool IsFidSelected(
  const int W, 
  const int num_frames,
  const int selection_startx,
  const int selection_endx , 
  const int frame_idx)
{
  const int minv = std::min( selection_startx, selection_endx);
  const int maxv = std::max( selection_startx, selection_endx);
  
  int t = ChartPos_Time(W, num_frames, frame_idx);
  return minv <= t && t <= maxv;
}











void FormSegSwallowOrganTimeline::InitChart(int num_frames)
{
  //0.allicate bitmap 
  const int     W    = std::max( 1, m_panel->Width) ;
  const int     H    = std::max( 1, m_panel->Height);
  const EVec3f &cube = ImageCore::GetInst()->GetCuboidF();
 
  if ( m_background != nullptr ) delete m_background;
  m_background = gcnew Bitmap(W,H,Imaging::PixelFormat::Format24bppRgb);
  
  Brush^ brush_white = gcnew SolidBrush( Color::White);
  Pen^   pen_red     = gcnew Pen( Color::Red  , 2);
  Pen^   pen_black   = gcnew Pen( Color::Black, 2);
  Pen^   pen_gray    = gcnew Pen( Color::Gray , 1);

  //draw base lines 
  Graphics ^g = Graphics::FromImage( m_background );

  System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, W,H);  
  g->FillRectangle(brush_white, rect);


  {
    //chart x 
    int left  = ChartPos_Time( W, num_frames, 0           ); //frame_idx = 9
    int right = ChartPos_Time( W, num_frames, num_frames-1); //frame_idx = n-1

    EVec3i chart_top = ChartPos_XYZ( H, cube, cube         , 1.0f); 
    EVec3i chart_btm = ChartPos_XYZ( H, cube, EVec3f(0,0,0), 1.0f); 
    EVec3i chart_mid = (chart_top + chart_btm)/2; 

    g->DrawLine( pen_black, left, chart_mid[0], right, chart_mid[0]); 
    g->DrawLine( pen_black, left, chart_top[0], left , chart_btm[0]); 
    g->DrawLine( pen_black, left, chart_mid[1], right, chart_mid[1]); 
    g->DrawLine( pen_black, left, chart_top[1], left , chart_btm[1]); 
    g->DrawLine( pen_black, left, chart_mid[2], right, chart_mid[2]); 
    g->DrawLine( pen_black, left, chart_top[2], left , chart_btm[2]); 

    for(int fi = 1; fi < num_frames; ++fi)
    {
      int t = ChartPos_Time( W, num_frames, fi);
      Pen^ p = fi % 5==0 ? pen_black : pen_gray;
      g->DrawLine( p, t, chart_top[0], t, chart_btm[0]);
      g->DrawLine( p, t, chart_top[1], t, chart_btm[1]);
      g->DrawLine( p, t, chart_top[2], t, chart_btm[2]);
    }
  } 

  delete brush_white;
  delete pen_red;
  delete pen_black;
  delete pen_gray;
}


//pixelへ直接書き込み （今回はしないのでコメントアウト）
//System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, W,H);
//Imaging::BitmapData^ bmpData = m_background->LockBits( rect, Imaging::ImageLockMode::ReadWrite, m_background->PixelFormat); 
//Byte* pBuf = (Byte*)bmpData->Scan0.ToPointer();

//for( int y=0; y < H; ++y)
//{
//  for( int x=0; x < W; ++x)
//  {
//    pBuf[y*bmpData->Stride + x*3 + 0] = x; 
//    pBuf[y*bmpData->Stride + x*3 + 1] = y; 
//    pBuf[y*bmpData->Stride + x*3 + 2] = x+y; 
//  }
//}


static void PaintElipses(
    Graphics ^g, 
    Brush^ brs_x, 
    Brush^ brs_y, 
    Brush^ brs_z, 
    std::vector<EVec4i> &points, //(t, v(x), v(y), v(z)) 
    int radius 
    )
{
  for (const auto &p : points){
    g->FillEllipse(brs_x  , p[0]-radius, p[1]-radius, 2*radius, 2*radius);
    g->FillEllipse(brs_y  , p[0]-radius, p[2]-radius, 2*radius, 2*radius);
    g->FillEllipse(brs_z  , p[0]-radius, p[3]-radius, 2*radius, 2*radius);
  }
}


System::Void FormSegSwallowOrganTimeline::m_panel_Paint(
    System::Object^  sender, 
    System::Windows::Forms::PaintEventArgs^  e) 
{
  Pen^   pen_r = gcnew Pen( Color::Red   , 2);
  Pen^   pen_y = gcnew Pen( Color::Yellow, 3); 
  Brush^ brs_r = gcnew SolidBrush( Color::Red    );
  Brush^ brs_g = gcnew SolidBrush( Color::Green  );
  Brush^ brs_b = gcnew SolidBrush( Color::Blue   );
  Brush^ brs_y = gcnew SolidBrush( Color::Yellow );
  Brush^ brs_h = gcnew SolidBrush( Color::Gray );

  
  const int    W    = std::max( 1, m_panel->Width );
  const int    H    = std::max( 1, m_panel->Height);
  const EVec3f cube = ImageCore::GetInst()->GetCuboidF();
  EVec3i chart_top  = ChartPos_XYZ( H, cube, cube         , 1.0f); 
  EVec3i chart_btm  = ChartPos_XYZ( H, cube, EVec3f(0,0,0), 1.0f); 
  int    chart_h    = chart_btm[0] - chart_top[0] ; 

  int select_x = (m_selectx0 < m_selectx1) ? m_selectx0 : m_selectx1;
  int select_w = abs( m_selectx1 - m_selectx0);
  
  //bitmapを作成
  Bitmap^ bmp = gcnew Bitmap(m_background);
  Graphics ^g = Graphics::FromImage( bmp );

  //0. draw selection rect
  g->DrawRectangle( pen_y, select_x, chart_top[0], select_w, chart_h);
  g->DrawRectangle( pen_y, select_x, chart_top[1], select_w, chart_h);
  g->DrawRectangle( pen_y, select_x, chart_top[2], select_w, chart_h);

  //1. draw timeline
  int num_frames = ImageCore::GetInst()->GetNumFrames();
  int idx_frames = FormVisParam::GetInst()->GetframeI();
  {
    int t = ChartPos_Time( W, num_frames, idx_frames);
    g->DrawLine( pen_r, t, chart_top[0], t, chart_btm[0] ); 
    g->DrawLine( pen_r, t, chart_top[1], t, chart_btm[1] ); 
    g->DrawLine( pen_r, t, chart_top[2], t, chart_btm[2] ); 
  }
  
  //2. 頂点群（重心）をペイント
  int   radius  = CP_RADIUS;
  float v_scale = (float)m_updown_yaxisscale->Value;
  
  if ( m_checkbox_showmulticps->Checked )
  {
    auto verts_seq = ModeSegSwallowOrgans::GetInst()->SelectedCageVtx_GetSeq();
    if ( verts_seq.size() != 0 && verts_seq.front().size() == 1 )
      verts_seq = ModeSegSwallowOrgans::GetInst()->SelectedCageVtx_Get1RingSeq();
    
    for ( int fi = 0; fi < verts_seq.size(); ++fi ) 
    {
      int t = ChartPos_Time( W, num_frames, fi);
      std::vector<EVec4i> pts(verts_seq[fi].size());

      for ( int i=0; i < verts_seq[fi].size(); ++i )
      {
        EVec3i xyz = ChartPos_XYZ( H, cube, verts_seq[fi][i], v_scale);
        pts[i] << t, xyz[0], xyz[1], xyz[2] ;
      }
      PaintElipses(g, brs_h,brs_h,brs_h, pts, radius-2);
    }
  }



  //3. 頂点群（重心）をペイント
  auto  verts  = ModeSegSwallowOrgans::GetInst()->SelectedCageVtx_GetCentroidSeq();
  std::vector<EVec4i> points( verts.size() ), points_hl;

  for ( int fi = 0; fi < (int)verts.size(); ++fi)
  {
    int      t = ChartPos_Time( W, num_frames, fi);
    EVec3i xyz = ChartPos_XYZ( H, cube, verts[fi], v_scale);
    points[fi] << t, xyz[0], xyz[1], xyz[2];

    if ( (m_selectx0 <= t && t <= m_selectx1 ) || 
         (m_selectx1 <= t && t <= m_selectx0 ) )
    {
      points_hl.push_back( points[fi] );
    }
  }

  PaintElipses(g, brs_y,brs_y,brs_y, points_hl, radius+3);
  PaintElipses(g, brs_r,brs_g,brs_b, points, radius);

  //bmpをpanelに描画
  System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, W,H);
  e->Graphics->InterpolationMode = Drawing2D::InterpolationMode::NearestNeighbor;
  e->Graphics->DrawImage(bmp, System::Drawing::Rectangle(0, 0,W,H), rect, GraphicsUnit::Pixel);
  delete bmp;
}



System::Void FormSegSwallowOrganTimeline::m_btn_smoothing_Resize(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  InitChart(ImageCore::GetInst()->GetNumFrames());
}



System::Void FormSegSwallowOrganTimeline::m_panel_Resize(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  InitChart(ImageCore::GetInst()->GetNumFrames());  
  RepaintPanel();
}





System::Void FormSegSwallowOrganTimeline::m_panel_MouseDown
(
    System::Object^  sender, 
    System::Windows::Forms::MouseEventArgs^  e
)
{
  const int px = e->X;
  const int py = e->Y;
  const int W = std::max( 1, m_panel->Width );
  const int H = std::max( 1, m_panel->Height);
  const int num_frames = ImageCore::GetInst()->GetNumFrames();

    
  //RIGHT BUTTON  
  if ( e->Button == System::Windows::Forms::MouseButtons::Right)
  { 
    //cancel modification 
    if ( m_pick_xyz >= 0 && m_pick_fi >= 0) 
    {
      if( IsFidSelected( W, num_frames, m_selectx0, m_selectx1, m_pick_fi) )
      {
        auto ids = CalcSelectFIDs(W, num_frames, m_selectx0, m_selectx1);
        for ( const auto id : ids )
        {
          ModeSegSwallowOrgans::GetInst()->SelectedCageVtx_Move( 
            id, m_pick_xyz, m_prev_centers[id][m_pick_xyz]  );
        }
      }      
      else
      {
        ModeSegSwallowOrgans::GetInst()->SelectedCageVtx_Move(
          m_pick_fi, m_pick_xyz, m_prev_centers[m_pick_fi][m_pick_xyz]  );
      }
    }
    FormSegSwallowOrganTimeline::m_panel_MouseUp(sender, e);
    return;
  }

  //LEFT BOTTON
  m_bLbtn = true;

  //1. selection
  if( isShiftKeyOn() )
  {
    int minv = ChartPos_Time( W, num_frames, 0);
    int maxv = ChartPos_Time( W, num_frames, num_frames-1);
    m_selectx0 = m_selectx1 = t_crop( minv, maxv, px);
    return;
  }

  //2. pick control points
  auto   gc_seq  = ModeSegSwallowOrgans::GetInst()->SelectedCageVtx_GetCentroidSeq();
  EVec3f cube    = ImageCore::GetInst()->GetCuboidF();
  int    r       = CP_RADIUS + 2;
  float  v_scale = (float)m_updown_yaxisscale->Value;

  m_pick_fi = m_pick_xyz = -1;

  for ( int fi = 0; fi < (int)gc_seq.size(); ++fi)
  {
    int    t   = ChartPos_Time( W, num_frames, fi);
    EVec3i xyz = ChartPos_XYZ ( H, cube, gc_seq[fi], v_scale);

    if ( (px-t)*(px-t) + (py-xyz[0])*(py-xyz[0]) <= r*r ) m_pick_xyz = 0;
    if ( (px-t)*(px-t) + (py-xyz[1])*(py-xyz[1]) <= r*r ) m_pick_xyz = 1;
    if ( (px-t)*(px-t) + (py-xyz[2])*(py-xyz[2]) <= r*r ) m_pick_xyz = 2;

    if ( m_pick_xyz >= 0)
    {
      m_pick_fi = fi;
      m_prev_centers = gc_seq;
      break;
    }
  }
}



System::Void FormSegSwallowOrganTimeline::m_panel_MouseMove
(
    System::Object^  sender, 
    System::Windows::Forms::MouseEventArgs^  e
)
{
  if ( !m_bLbtn ) return;
  const int px = e->X;
  const int py = e->Y;
  const int W = std::max( 1, m_panel->Width );
  const int H = std::max( 1, m_panel->Height);

  //Selection
  if( isShiftKeyOn() )
  {
    int minv = (int)(W * CHART_W_OFST);
    int maxv = (int)(W * CHART_W_OFST) + (int)(W * CHART_W_RATE);
    m_selectx1   = t_crop( minv, maxv, px);
    this->RepaintPanel();
    return;
  }
    
  //Move Control Point
  if ( m_pick_fi == -1 || m_pick_xyz < 0) return;
  const int    num_frames = ImageCore::GetInst()->GetNumFrames();
  const EVec3f cuboid     = ImageCore::GetInst()->GetCuboidF();
  

  bool b_pickselected_fid 
    = IsFidSelected( W, num_frames, m_selectx0, m_selectx1, m_pick_fi );

  if ( !b_pickselected_fid )
  {
    float vscale = (float)m_updown_yaxisscale->Value;
    float pos3d = Calc3DPos_XYZ( H, cuboid, py, m_pick_xyz, vscale);
    ModeSegSwallowOrgans::GetInst()->SelectedCageVtx_Move( 
      m_pick_fi, m_pick_xyz, pos3d);
  }
  else
  {
    float vscale = (float)m_updown_yaxisscale->Value;
    float pos3d = Calc3DPos_XYZ( H, cuboid, py, m_pick_xyz, vscale);
    float diff = pos3d - m_prev_centers[m_pick_fi][m_pick_xyz];

    auto ids = CalcSelectFIDs( W, num_frames, m_selectx0, m_selectx1);
    
    for ( const auto id : ids )
    {
      ModeSegSwallowOrgans::GetInst()->SelectedCageVtx_Move( 
        id, m_pick_xyz, m_prev_centers[id][m_pick_xyz] + diff);
    }
  }

  formMain_RedrawMainPanel();
  this->RepaintPanel();
}


System::Void FormSegSwallowOrganTimeline::m_panel_MouseUp  (
    System::Object^  sender, 
    System::Windows::Forms::MouseEventArgs^  e)
{
  m_bLbtn = false;
  m_pick_fi = m_pick_xyz = -1;
  formMain_RedrawMainPanel();
  this->RepaintPanel();
}




System::Void FormSegSwallowOrganTimeline::m_btn_smoothing_Click(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  const int W = m_panel->Width ;
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  auto ids = CalcSelectFIDs(W, num_frames, m_selectx0, m_selectx1);

  ModeSegSwallowOrgans::GetInst()->SelectedCageVtx_Smoothing(ids);
  formMain_RedrawMainPanel();
  RepaintPanel();
}




System::Void FormSegSwallowOrganTimeline::m_btm_copyfromleft_Click(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  const int W = m_panel->Width ;
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  auto ids = CalcSelectFIDs(W, num_frames, m_selectx0, m_selectx1);

  ModeSegSwallowOrgans::GetInst()->SelectedCageVtx_CopyLeft( ids); 
  formMain_RedrawMainPanel();
  RepaintPanel();
}



System::Void FormSegSwallowOrganTimeline::m_btn_copyfromright_Click(
    System::Object^  sender, 
    System::EventArgs^  e) 
{  
  const int W = m_panel->Width ;
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  auto ids = CalcSelectFIDs(W, num_frames, m_selectx0, m_selectx1);

  ModeSegSwallowOrgans::GetInst()->SelectedCageVtx_CopyRight( ids);
  formMain_RedrawMainPanel();
  RepaintPanel();
}


System::Void FormSegSwallowOrganTimeline::m_updown_yaxisscale_ValueChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  RepaintPanel();
}









