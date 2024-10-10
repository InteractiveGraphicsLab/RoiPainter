#pragma unmanaged 
#include "ModeRefStrokeTrim.h"

#pragma managed 
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormRefStrokeTrim.h"
#include "FormSelectMskId.h"
#include "CliMessageBox.h"
#pragma unmanaged 

#include "ImageCore.h"
#include "CrsSecCore.h"
#include "ModeCore.h"
#include "ModeCommonTools.h"

using namespace RoiPainter4D;


ModeRefStrokeTrim::ModeRefStrokeTrim():
	m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg_Seg.glsl"   ),
	m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
	std::cout << "ModeSegThreshPnt...\n";
	m_bL = m_bR = m_bM = m_bDrawTrimStr = false;  
  m_b_draw_cutstroke = false;
	m_bModified    = false;
	m_msk3D_pre    = 0;
	m_msk3D_pre_fi = 0;
	std::cout << "ModeSegThreshPnt DONE\n";
}


ModeRefStrokeTrim::~ModeRefStrokeTrim()
{
	if (m_msk3D_pre) delete[] m_msk3D_pre;
}


bool ModeRefStrokeTrim::CanEndMode() 
{
	if (!m_bModified) return true;
	
	else return ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "Leaving?");
	return true;
}



void ModeRefStrokeTrim::StartMode ()
{
	m_bL = m_bR = m_bM = false;
	m_bModified = false;

	int maskID = formSelectMasdskID_showModalDialog();

	if (maskID == -1 )
	{
		ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
		return;
	}

	if (maskID == 0 ) 
  {
		ShowMsgDlg_OK("0th region (background) cannot be trimmed", "caution");
		ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
		return;
	}

	const auto &maskdata = ImageCore::GetInst()->GetMaskData();
	if ( maskdata[maskID].lock )
	{
		ShowMsgDlg_OK("Lockのかかったmask IDを変更しようとしています", "caution");
	}

	formRefStrokeTrim_Show();

	const int num_frames = ImageCore::GetInst()->GetNumFrames();
	const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
	const std::vector<byte*> &flg4d  = ImageCore::GetInst()->m_flg4d;
	std::vector<byte*>       &mask4d = ImageCore::GetInst()->m_mask4d;

#pragma omp parallel for
	for (int f = 0; f < num_frames; ++f)
	{
    byte* flg3d = flg4d [f];
    byte* msk3d = mask4d[f];
		for (int i = 0; i < num_voxels; ++i)
		{
			flg3d[i] = (msk3d[i] == maskID) ? 255 : 0;
		}
	}

	//4D volume (cpu) --> vis volume (gpu)
  UpdateImageCoreVisVolumes();

	const int fi = formVisParam_getframeI();
	
	if (m_msk3D_pre) delete[] m_msk3D_pre;
	m_msk3D_pre = new byte[num_voxels];
	memcpy(m_msk3D_pre, &flg4d[fi][0], sizeof(byte)*num_voxels);
	m_msk3D_pre_fi = fi;
  
  m_bDrawCrssec = true;

	std::cout << "finish start mode!!!!\n";
}




////////////////////////////////////////////////////////////////////////////
//MouseListener/////////////////////////////////////////////////////////////

void ModeRefStrokeTrim::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
	m_bL = true;

	m_TrimStroke.clear();
  
  if (IsCtrKeyOn())
  {
    m_stroke.clear();
    m_b_draw_cutstroke = true;
  }
	else if ( IsShiftKeyOn() )
	{
		m_CutStroke.clear();
		m_bDrawTrimStr = true;
	}
	else
	{
		ogl->BtnDown_Trans(p);
	}
}


void ModeRefStrokeTrim::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{	
  if (m_b_draw_cutstroke)
  {
    EVec3f cube = ImageCore::GetInst()->GetCuboidF();
    CrssecCore::CreateCurvedCrssec(cube, ogl->GetCamPos(), m_stroke);
  }

	if(m_bDrawTrimStr)
	{
		updateVolFlgByStroke(*ogl);
	}
  
  m_b_draw_cutstroke = false;
	m_TrimStroke.clear();
	m_bDrawTrimStr = false;
	m_bL       = false;
	ogl->BtnUp();
	
	formMain_RedrawMainPanel();
}


void ModeRefStrokeTrim::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
	if (m_bL && m_bDrawTrimStr)
	{
		m_bL           = false;
		m_bDrawTrimStr = false;
		m_CutStroke.clear();
		m_TrimStroke.clear();
	}
	m_bR = true;
	ogl->BtnDown_Rot(p);
}


void ModeRefStrokeTrim::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
	m_bR = false;
	ogl->BtnUp();
	formMain_RedrawMainPanel();
}


void ModeRefStrokeTrim::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
	m_bM = true;
	ogl->BtnDown_Zoom(p);
}


void ModeRefStrokeTrim::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
	m_bM = false;
	ogl->BtnUp();
	formMain_RedrawMainPanel();
}


void ModeRefStrokeTrim::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
	if (!m_bL && !m_bR && !m_bM) return;

	EVec3f rayP, rayD, pos;
	ogl->GetCursorRay( p, rayP, rayD);

  if (m_b_draw_cutstroke)
  {
    m_stroke.push_back(rayP + 0.1f * rayD);
  }
	else if (m_bDrawTrimStr)
	{
		m_CutStroke.push_back( rayP + 0.1f * rayD );
		m_TrimStroke.push_back(p);

	}
	else 
	{
		ogl->MouseMove(p);
	}
	
	formMain_RedrawMainPanel();
}


void ModeRefStrokeTrim::MouseWheel(const EVec2i &p, short z_delta,  OglForCLI *ogl)
{
  bool wheeling = WheelingCrssec ( p, z_delta, ogl);
  if (!wheeling ) ogl->ZoomCamera(z_delta * 0.1f);
	formMain_RedrawMainPanel();
}


void ModeRefStrokeTrim::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeRefStrokeTrim::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeRefStrokeTrim::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


void ModeRefStrokeTrim::KeyDown(int nChar) 
{

	if (nChar == 'Z' && IsCtrKeyOn())
	{
	  const int      fi = formVisParam_getframeI();

		if ( fi == m_msk3D_pre_fi || ShowMsgDlgYesNo("現在のフレームではないフレームを変更しようとしています．\nよろしいですか？", "caution") )
		{
			std::cout << "undo!!";
	    const int      num_voxels = ImageCore::GetInst()->GetNumVoxels();
			std::vector<byte*> &flg4d      = ImageCore::GetInst()->m_flg4d;
      memcpy(&flg4d[m_msk3D_pre_fi][0], m_msk3D_pre, sizeof(byte) * num_voxels);
      UpdateImageCoreVisVolumes();
		}
	}
  else if (nChar == VK_TAB)
    m_bDrawCrssec = !m_bDrawCrssec;
}

void ModeRefStrokeTrim::KeyUp(int nChar) 
{
}



void ModeRefStrokeTrim::DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{	
	const EVec3i reso     = ImageCore::GetInst()->GetReso();

  //bind volumes ---------------------------------------
  BindAllVolumes();

	if (m_bDrawCrssec)
  {
    DrawCrossSections(cuboid, reso, !IsSKeyOn(), m_crssecShader);
  }

  if (m_b_draw_cutstroke)
  {
    DrawPolyLine( EVec3f(1,1,0), 3, m_stroke, false);
  }

	if (m_bDrawTrimStr)
  {
    DrawPolyLine( EVec3f(1,0,1), 4, m_CutStroke, true);
  }

	const bool b_onmanip  = (formVisParam_bOnManip() || m_bL || m_bR || m_bM) 
                         && FormRefStrokeTrim_bUseRoughRend();

	glDisable( GL_DEPTH_TEST);
	glEnable ( GL_BLEND);
  DrawVolumeSlices(cuboid, reso, camP, camF, true, b_onmanip, m_volumeShader);
	glDisable( GL_BLEND);
	glEnable(GL_DEPTH_TEST);
  
}



static double calcAngle(const EVec2d &d1, const EVec2d &d2)
{
	double l = d1.norm() * d2.norm();
	if (l == 0) return 0;

	double cosT = Crop(-1.0, 1.0, (d1.dot(d2)) / l);

	if (d1[0] * d2[1] - d1[1] * d2[0] >= 0) return  acos(cosT);
	else					                          return -acos(cosT);
}



static bool isInClosedStroke(
		double x, 
		double y, 
		const std::vector<EVec2i> &stroke)
{
	//stroke.size() > 3 
	EVec2d d1, d2;
	double sum = 0;

	d1 << stroke.back().x() - x, stroke.back().y() - y;
	d2 << stroke[0].x() - x, stroke[0].y() - y;
	sum = calcAngle(d1, d2);

	for (int i = 1; i < (int)stroke.size(); ++i)
	{
		d1 << stroke[i - 1].x() - x, stroke[i - 1].y() - y;
		d2 << stroke[i].x() - x, stroke[i].y() - y;
		sum += calcAngle(d1, d2);
	}

	return fabs(sum) > M_PI * 1.5;
}



void t_calcBoundBox(const std::vector<EVec2i> &vs, EVec2i &minBB, EVec2i &maxBB)
{
	if (vs.size() <= 0) return;
	minBB << vs.front().x(), vs.front().y();
	maxBB << vs.front().x(), vs.front().y();
	for (const auto &v : vs)
	{
		minBB[0] = std::min(v.x(), minBB[0]);
		minBB[1] = std::min(v.y(), minBB[1]);
		maxBB[0] = std::max(v.x(), maxBB[0]);
		maxBB[1] = std::max(v.y(), maxBB[1]);
	}
}


void ModeRefStrokeTrim::updateVolFlgByStroke(OglForCLI &ogl)
{
	if (m_TrimStroke.size() < 3) return;

	if (!ogl.IsDrawing()) ogl.oglMakeCurrent();


	std::vector<byte*> &img4D_flg = ImageCore::GetInst()->m_flg4d;
	const EVec3i  reso  = ImageCore::GetInst()->GetReso();
	const EVec3f  pitch = ImageCore::GetInst()->GetPitch();
	const int     WH    = reso[0] * reso[1];
	const int     fi    = formVisParam_getframeI();

	memcpy(m_msk3D_pre, &img4D_flg[fi][0], sizeof(byte) * reso[0] * reso[1] * reso[2]);
	m_msk3D_pre_fi = fi;

	double modelMat[16], projMat[16];
	int vpMat[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMat);
	glGetDoublev(GL_PROJECTION_MATRIX, projMat);
	glGetIntegerv(GL_VIEWPORT, vpMat);
	const int &vpW = vpMat[2];
	const int &vpH = vpMat[3];

	std::cout << "-------------gen in out image "<< m_TrimStroke.size() << "---------\n";

	byte *insideFlgs = new byte[vpW * vpH];
	memset(insideFlgs, 0, sizeof(byte) * vpW * vpH);

	for (auto &p : m_TrimStroke) p.y() = vpH - p.y();

	EVec2i minBB, maxBB;
	t_calcBoundBox(m_TrimStroke, minBB, maxBB);
	minBB[0] = std::max(0, minBB[0] - 1);
	minBB[1] = std::max(0, minBB[1] - 1);
	maxBB[0] = std::min(vpW - 1, maxBB[0] + 1);
	maxBB[1] = std::min(vpH - 1, maxBB[1] + 1);


#pragma omp parallel for
	for (int y = minBB[1]; y < maxBB[1]; ++y)
	for (int x = minBB[0]; x < maxBB[0]; ++x)
	{
		insideFlgs[x + y * vpW] = isInClosedStroke(x, y, m_TrimStroke) ? 255 : 0;
	}
	
	std::cout << "project all pixels\n";

#pragma omp parallel for
	for (int z = 0; z < reso[2]; ++z)
	for (int y = 0; y < reso[1]; ++y)
	for (int x = 0; x < reso[0]; ++x)
	{
		const int I = x + y * reso[0] + z * WH;
		if (img4D_flg[fi][I] != 255) continue;

		EVec3d vpPos;
		gluProject((x + 0.5)*pitch[0], (y + 0.5)*pitch[1], (z + 0.5)*pitch[2], modelMat, projMat, vpMat, &vpPos.x(), &vpPos.y(), &vpPos.z());

		if (0 <= vpPos.x() && vpPos.x() <= vpW - 1 &&
			0 <= vpPos.y() && vpPos.y() <= vpH - 1 &&
			insideFlgs[(int)vpPos.x() + (int)vpPos.y()*vpW] == 255)
		{
			img4D_flg[fi][I] = 1;
		}
	}
		

	std::cout << "DONE\n";

	if (!ogl.IsDrawing()) wglMakeCurrent(NULL, NULL);

  UpdateImageCoreVisVolumes();	
	m_bModified = true;
	delete[] insideFlgs;
}


void ModeRefStrokeTrim::finishTrim()
{
	const int  num_frames = ImageCore::GetInst()->GetNumFrames();
	const int  num_voxels = ImageCore::GetInst()->GetNumVoxels();
	std::vector<byte*> &flg4d = ImageCore::GetInst()->m_flg4d;

	if (!m_bModified)
	{
		ShowMsgDlg_OK("トリムされた領域が存在しません．\n No trimmed voxel exist", "no trimmed.");
		return;
	}

	//このタイミングでfalseにしないとcanEndModeでダイアログが表示される
	m_bModified = false;

	for (int fi = 0; fi < num_frames; ++fi)
  {
    byte* flg3d = flg4d[fi];
#pragma omp parallel for 
		for (int i = 0; i < num_voxels; ++i)
    {
			if (     flg3d[i] == 255) flg3d[i] = 1;
			else if (flg3d[i] == 1  ) flg3d[i] = 255;
    }
    formMain_SetProgressValue( fi/(float)num_frames);
  }
  formMain_SetProgressValue(0);

	ImageCore::GetInst()->mask_storeCurrentForeGround();
	ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
	formMain_RedrawMainPanel();
}


void ModeRefStrokeTrim::cancelBtnClick()
{
	if (ShowMsgDlgYesNo("現在のトリム結果を破棄してよいですか\nDo you want to cancel current Trimming?", "cancel?"))
	{
		m_bModified = false;
		ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
		formMain_RedrawMainPanel();
	}
}
