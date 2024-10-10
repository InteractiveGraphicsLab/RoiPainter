#pragma managed
#include "FormSegLocalRGrow.h"
#include "FormMain.h"
#include "FormVisParam.h"

#pragma unmanaged
#include "Mode/ModeSegLocalRGrow.h"
#include "ImageCore.h"
#include <iostream>
#pragma managed

#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace msclr::interop;
using namespace RoiPainter4D;
using namespace System::Runtime::InteropServices;


void FormSegLocalRGrow::InitAllItems(
    float max_radius, 
    int   vol_min, 
    int   vol_max)
{
  //bar for seed rad/thresh
  m_bar_radius   ->SetRange( 1, static_cast<int>(max_radius * 10));
  m_bar_threshmin->SetRange(vol_min, vol_max);
  m_bar_threshmax->SetRange(vol_min, vol_max);
  m_bar_radius   ->Value = 1;
  //m_bar_threshmin->Value = (vol_minmax[0] + vol_minmax[1]) / 2;
  //m_bar_threshmax->Value = (vol_minmax[0] + vol_minmax[1]) / 2;
  m_bar_threshmin->Value = -1500;
  m_bar_threshmax->Value = -400;
  m_text_radius   ->Text = (m_bar_radius   ->Value / 10.0f).ToString("F1");
  m_text_threshmin->Text =  m_bar_threshmin->Value.ToString();
  m_text_threshmax->Text =  m_bar_threshmax->Value.ToString();

  //毎回変更しない（前回の状態を維持）
  //m_checkbox_allframesamepos   ->Checked = false;
  //m_checkbox_allframesamethresh->Checked = true ;
  //m_checkbox_allframesamerad   ->Checked = true ;

  //特に必要はないけどbtnも利用可能にしておく
  m_btn_cancel->Enabled = true;
  m_btn_finish->Enabled = true;
  m_btn_loadseedinfo->Enabled = true;
  m_btn_runlocalregiongrow_allframes->Enabled = true;
  m_btn_runlocalregiongrow_onlyone  ->Enabled = true;
}



//シードの選択状態が変化したときにバーの値を更新する
void FormSegLocalRGrow::UpdateSeedInfoItems( )
{
  std::cout << "UpdateSeedInfoItems\n";
  m_item_updating = true;

  const std::vector<LocalSeed> &seeds = ModeSegLocalRGrow::GetInst()->getSeedList();
  const int select_idx = ModeSegLocalRGrow::GetInst()->GetSelectedSeedId();

  if (select_idx < 0 || static_cast<int>(seeds.size()) <= select_idx ) return;

  int frame_idx = formVisParam_getframeI();
  const float  radius = seeds[select_idx].GetRadius   (frame_idx);
  const EVec2i thresh = seeds[select_idx].GetThreshold(frame_idx);

  m_bar_radius   ->Value = static_cast<int>( radius * 10);
  m_bar_threshmin->Value = thresh[0];
  m_bar_threshmax->Value = thresh[1];
  m_text_radius   ->Text = (m_bar_radius   ->Value / 10.0f).ToString("F1");
  m_text_threshmax->Text = m_bar_threshmax->Value.ToString();
  m_text_threshmin->Text = m_bar_threshmin->Value.ToString();

  m_item_updating = false;
  std::cout << "UpdateSeedInfoItems___DONE\n";
}


void FormSegLocalRGrow::UpdateSeedList()
{
  std::cout << "UpdateSeedList\n";
  m_item_updating = true;

  const std::vector<LocalSeed> &seeds = ModeSegLocalRGrow::GetInst()->getSeedList();
  const int select_idx = ModeSegLocalRGrow::GetInst()->GetSelectedSeedId();
  const int num_frames = ImageCore::GetInst()->GetNumFrames();
  const int frame_idx  = formVisParam_getframeI();

  //gen list 
  m_list_seeds->Rows->Clear();
  for (int i = 0; i < seeds.size(); ++i) m_list_seeds->Rows->Add();

  for (int i = 0; i < seeds.size(); ++i)
  {
    m_list_seeds[0, i]->Value = gcnew String( seeds[i].GetRadius( frame_idx ).ToString("F1") );

    for (int j = 0; j < num_frames; ++j) 
    {
      if ( seeds[i].GetIsEdit(j) ) 
      {
        String^ canma = ( j == 0 ) ? gcnew String("") : gcnew String(",");
        m_list_seeds[1, i]->Value += gcnew String(canma + j.ToString());
      }
    }
  }

  m_list_seeds->CurrentCell = nullptr; 
  if( 0 <= select_idx && select_idx < static_cast<int>(seeds.size()) )
    m_list_seeds->CurrentCell = m_list_seeds[0,select_idx];


  Refresh();
  m_item_updating = false;
  std::cout << "UpdateSeedList___DONE\n";
}



//seed selection was changed
System::Void FormSegLocalRGrow::SeedListSelectionChanged(System::Object^  sender, System::EventArgs^  e)
{
  if (m_item_updating) return;
  ModeSegLocalRGrow::GetInst()->SetSelectedSeedId( m_list_seeds->CurrentCell->RowIndex );
  UpdateSeedInfoItems( );
  FormMain::GetInst()->RedrawMainPanel();
}


//track bars
System::Void FormSegLocalRGrow::BarRadiusScroll( System::Object^ sender, System::EventArgs^  e)
{
  if (m_item_updating) return;
  m_item_updating = true;

  const int   frame_idx = formVisParam_getframeI();
  const float radius    = m_bar_radius->Value / 10.0f; 
  m_text_radius->Text   = radius.ToString("F1");
  
  ModeSegLocalRGrow::GetInst()->SelectSeed_SetRadius( radius );
  ModeSegLocalRGrow::GetInst()->SelectSeed_RunInterpolation();
  UpdateSeedList();
  FormMain::GetInst()->RedrawMainPanel();

  m_item_updating = false;
}


System::Void FormSegLocalRGrow::BarThreshMinScroll(System::Object^  sender, System::EventArgs^  e)
{
  if (m_item_updating) return;
  m_item_updating = true;

  ModeSegLocalRGrow::GetInst()->SelectSeed_SetThreshMin( m_bar_threshmin->Value);
  ModeSegLocalRGrow::GetInst()->SelectSeed_RunInterpolation();
  m_text_threshmin->Text =  m_bar_threshmin->Value.ToString();
  UpdateSeedList();

  m_item_updating = false;
}


System::Void FormSegLocalRGrow::BarThreshMaxScroll(System::Object^  sender, System::EventArgs^  e)
{
  if (m_item_updating) return;
  m_item_updating = true;
  
  std::cout  << "BarThreshMaxScroll\n";
  ModeSegLocalRGrow::GetInst()->SelectSeed_SetThreshMax( m_bar_threshmax->Value);
  ModeSegLocalRGrow::GetInst()->SelectSeed_RunInterpolation();
  m_text_threshmax->Text =  m_bar_threshmax->Value.ToString();
  UpdateSeedList();
  std::cout  << "BarThreshMaxScroll_done\n";
  
  m_item_updating = false;
}


void FormSegLocalRGrow::TextboxRadiusChangedLeave(bool b_Leave)
{
  if (m_item_updating) return;
  m_item_updating = true;

  float x;
  if ( float::TryParse( m_text_radius->Text, x)) 
  {
    x = Crop( m_bar_radius->Minimum / 10.0f, m_bar_radius->Maximum / 10.0f, x );
  }
  else 
  {
    x = m_bar_radius->Value / 10.0f;
  }
  m_bar_radius ->Value = (int)( x * 10.0f);
  if(b_Leave) m_text_radius->Text = (x).ToString("F1");
  
  ModeSegLocalRGrow::GetInst()->SelectSeed_SetRadius( m_bar_radius->Value / 10.0f );
  ModeSegLocalRGrow::GetInst()->SelectSeed_RunInterpolation();
  UpdateSeedList();
  FormMain::GetInst()->RedrawMainPanel();
  
  m_item_updating = false;
}
//text box changed (radius)
System::Void FormSegLocalRGrow::textboxRadiusChanged(System::Object^  sender, System::EventArgs^  e)
{
  TextboxRadiusChangedLeave( false );
  std::cout  << "textboxRadiusChanged_done\n";
}

//text box changed (radius)
System::Void FormSegLocalRGrow::textboxRadiusLeave(System::Object^  sender, System::EventArgs^  e)
{
  TextboxRadiusChangedLeave( true );
  std::cout  << "textboxRadiusLeave_done\n";
}


//text box changed (max thresh)
System::Void FormSegLocalRGrow::textboxThreshMaxChanged(System::Object^  sender, System::EventArgs^  e)
{
  if (m_item_updating) return;
  m_item_updating = true;
  std::cout  << "textboxThreshMaxChanged\n";


  int x;
  if ( Int32::TryParse(m_text_threshmax->Text, x)) 
  {
    x = Crop(m_bar_threshmax->Minimum, m_bar_threshmax->Maximum, x);
  }
  else if(m_text_threshmax->Text == "-")
  {
    x = Crop(m_bar_threshmax->Minimum, m_bar_threshmax->Maximum, 0);
  }
  else 
  {
    x = m_bar_threshmax->Value;
  }
  m_bar_threshmax->Value = x;
  m_text_threshmax->Text = (x).ToString();

  ModeSegLocalRGrow::GetInst()->SelectSeed_SetThreshMax( m_bar_threshmax->Value );
  ModeSegLocalRGrow::GetInst()->SelectSeed_RunInterpolation();
  UpdateSeedList();
  std::cout  << "textboxThreshMaxChanged_done\n";
 
  m_item_updating = false;
}



//text box changed (min thresh)
System::Void FormSegLocalRGrow::textboxThreshMinChanged(System::Object^  sender, System::EventArgs^  e)
{
  if (m_item_updating) return;
  m_item_updating = true;

  int x;
  if ( Int32::TryParse( m_text_threshmin->Text, x)) 
  {
    x = Crop(m_bar_threshmin->Minimum, m_bar_threshmin->Maximum, x);
  }
  else if( m_text_threshmin->Text == "-" )
  {
    x = Crop(m_bar_threshmin->Minimum, m_bar_threshmin->Maximum, 0);    
  }
  else 
  {
    x = m_bar_threshmin->Value;
  }

  m_bar_threshmin->Value = x;
  m_text_threshmin->Text = (x).ToString();

  ModeSegLocalRGrow::GetInst()->SelectSeed_SetThreshMin( m_bar_threshmin->Value );
  ModeSegLocalRGrow::GetInst()->SelectSeed_RunInterpolation();
  UpdateSeedList();

  m_item_updating = false;
}


//cancelボタンクリック
System::Void FormSegLocalRGrow::BtnCancelClick(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegLocalRGrow::GetInst()->CancelSegmentation();
}

//OKボタン
System::Void FormSegLocalRGrow::BtnFinishClick(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegLocalRGrow::GetInst()->FinishSegmentation();
}

//シード成長
System::Void FormSegLocalRGrow::BtnRunLocalRGrowAllFrameClick(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegLocalRGrow::GetInst()->RunLocalRegionGrow_Allframe();
}

System::Void FormSegLocalRGrow::BtnRunLocalRGrowCurrentFrameClick(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegLocalRGrow::GetInst()->RunLocalRegionGrow_OnlyCurrentFrame();
}


System::Void FormSegLocalRGrow::BtnLoadSeedInfoClick(System::Object^  sender, System::EventArgs^  e)
{
	OpenFileDialog^ dlg = gcnew OpenFileDialog();
	dlg->Filter = "txt files(*.txt)|*.txt";
	dlg->Multiselect = false;

	if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

	IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  std::cout << fname;

	ModeSegLocalRGrow::GetInst()->LoadSeedInfo(fname);
	formMain_RedrawMainPanel();
}

System::Void FormSegLocalRGrow::BtnSaveSeedInfoClick(System::Object^  sender, System::EventArgs^  e)
{
	SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "txt files (*.txt)|*.txt";
  dlg->AddExtension = true;
  dlg->DefaultExt = "txt";
  dlg->CheckFileExists = false;

	if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

	IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());
  std::cout << fname;

	ModeSegLocalRGrow::GetInst()->SaveSeedInfo(fname);
	formMain_RedrawMainPanel();
}

System::Void FormSegLocalRGrow::CheckBoxMeshDrawChanged(System::Object^  sender, System::EventArgs^  e)
{
	formMain_RedrawMainPanel();
}


