#pragma unmanaged
#include "ModeSegClosestPix.h"
#include "ModeCore.h"
#include "ImageCore.h"
#include "CrsSecCore.h"
#include <iostream>
#include "ModeCommonTools.h"

#pragma managed
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegClosestPix.h"
#include "CliMessageBox.h"
#pragma unmanaged

using namespace RoiPainter4D;


ModeSegClosestPix::~ModeSegClosestPix() 
{
}

ModeSegClosestPix::ModeSegClosestPix () :
	m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg.glsl"),
	m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
{
	std::cout << "ModeSegClosestPix...\n";
	
	m_bL = m_bR = m_bM = false;
	m_bDrawStr = false;

	std::cout << "ModeSegClosestPix done\n";
}


bool ModeSegClosestPix::canEndMode()
{ 
	//TODO 変更の有無を記憶しておく
	//変更がなければ return true 
	//変更があれば --> return CLI_MessageBox_YESNO_Show("Leaving?", "現在の分割結果を破棄してモードを変更しますか？");
	return true;
}

void ModeSegClosestPix::startMode() 
{
	m_bL = m_bR = m_bM = false;
	formSegClosestPix_Show();
}



void ModeSegClosestPix::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
	m_bL = true;

	if (isCtrKeyOn())
	{
		m_stroke.clear();
		m_bDrawStr = true;
	}
	ogl->BtnDown_Trans(p);
}

void ModeSegClosestPix::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
	if( m_bDrawStr ) 
	{
		EVec3f cube = ImageCore::GetInst()->GetCuboidF();
		CrssecCore::CreateCurvedCrssec( cube, ogl->GetCamPos(), m_stroke);
	}

	m_bDrawStr = false;
	m_bL       = false;
	ogl->BtnUp();

	formMain_RedrawMainPanel();
}

void ModeSegClosestPix::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
	m_bR = true;
	ogl->BtnDown_Rot(p);
}
void ModeSegClosestPix::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
	m_bR = false;
	ogl->BtnUp();
	formMain_RedrawMainPanel();
}
void ModeSegClosestPix::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
	m_bM = true;
	ogl->BtnDown_Zoom(p);
}

void ModeSegClosestPix::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
	m_bM = false;
	ogl->BtnUp();
	formMain_RedrawMainPanel();
}


void ModeSegClosestPix::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegClosestPix::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegClosestPix::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}

void ModeSegClosestPix::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
	EVec3f rayP, rayD, pos;
	ogl->GetCursorRay( p, rayP, rayD);
	
	if (m_bDrawStr)
	{
		m_stroke.push_back( rayP + 0.1f * rayD );
		formMain_RedrawMainPanel();
	}
	else if ( m_bL || m_bR ||  m_bM)
	{
		ogl->MouseMove(p);
		formMain_RedrawMainPanel();
	}
}


void ModeSegClosestPix::MouseWheel(const EVec2i &p, short z_delta,  OglForCLI *ogl)
{
  if(!WheelingCrssec(p, z_delta, ogl) ) {
    ogl->ZoomCamera(z_delta * 0.1f);
  }

	formMain_RedrawMainPanel();
}


void ModeSegClosestPix::keyUp  (int nChar) {}
void ModeSegClosestPix::keyDown(int nChar) {}










void ModeSegClosestPix::drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
	const EVec3i reso     = ImageCore::GetInst()->GetReso();

	//bind volumes ---------------------------------------
  BindAllVolumes();
  DrawCrossSections ( cuboid, reso, false, m_crssecShader);	

	if (m_bDrawStr)
	{
		const int N = (int) m_stroke.size();
		int *idx = new int[N];
		for( int i=0; i < N; ++i) idx[i] = i;

		glColor3d( 1,1,0);
		glLineWidth( 3 );

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, m_stroke.data() );
		glDrawElements (GL_LINE_STRIP , (int)m_stroke.size(), GL_UNSIGNED_INT, idx);
		glDisableClientState(GL_VERTEX_ARRAY);
	
		delete[] idx;
	}

	if ( formVisParam_bRendVol() )
	{
		glDisable( GL_DEPTH_TEST);
		glEnable ( GL_BLEND);
	  const bool b_manip   = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolumeSlices( cuboid, reso, camP, camF, false, b_manip, m_volumeShader );
		glDisable( GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
}

inline int pos2Idx(const EVec3i &pos, const EVec3i &WHD) {
	return pos.x() + pos.y()*WHD[0] + pos.z()*WHD[0]*WHD[1];
}

// 0:not change, 1:back, 255:fore
// NowFrame:(x, y, z) BeforeFrame(xx, yy, zz)
inline void t_IDSet(
	const int W,
	const int H,
	const int D,
	const byte *beforeFlgs,
	const byte *beforeMasks,
	const byte *flgs,
	byte *masks
) {
	
	EVec3i WHD(W, H, D);

#pragma omp parallel for
	for (int z = 0; z < D; z++)
	for (int y = 0; y < H; y++)
	for (int x = 0; x < W; x++)
	{
		int posIdx = pos2Idx(EVec3i(x, y, z), WHD);
		if (flgs[posIdx] == 255)
		{
			if (beforeFlgs[posIdx] == 255) {
				masks[posIdx] = beforeMasks[posIdx];
				continue;
			}

			float minDist = FLT_MAX;
			int nearestPosIdx;
			for (int zz = 0; zz < D; zz++)
			for (int yy = 0; yy < H; yy++)
			for (int xx = 0; xx < W; xx++){
				if (beforeFlgs[xx+yy*W+zz*W*H] != 255) continue;
				float dist = (float) (EVec3i(xx, yy, zz) - EVec3i(x, y, z)).norm();
				if (minDist > dist)
				{
					minDist = dist;
					nearestPosIdx = pos2Idx(EVec3i(xx, yy, zz), WHD);
				}
			}

			masks[posIdx] = beforeMasks[nearestPosIdx];
		}
	}

}

// 前景に対してのみ，前のフレームの各画素
void ModeSegClosestPix::runSegClosestPix(const int &startI, const int &endI) {

	std::cout << "updateMask-----------------------\n";

	std::vector<byte*>  &flg4D = ImageCore::GetInst()->m_flg4d;
	std::vector<byte*>  &msk4D = ImageCore::GetInst()->m_mask4d;

	const int    W = ImageCore::GetInst()->GetReso()[0];
	const int    H = ImageCore::GetInst()->GetReso()[1];
	const int    D = ImageCore::GetInst()->GetReso()[2];
	const int    WHD = W * H * D;

	byte* flgs = new byte[WHD];

	for (int fI = startI; fI <= endI; ++fI)
	{
		// volの独立領域にIDを設定。前フレームのIDを利用。
		std::cout << fI << "...";
		t_IDSet(W, H, D, flg4D[fI-1], msk4D[fI-1], flg4D[fI], msk4D[fI]);
	}
	delete[] flgs;

	std::cout << "finish\n";
}

void ModeSegClosestPix::finishSegmentation()
{

}

void ModeSegClosestPix::cancelSegmentation()
{
	if (ShowMsgDlgYesNo(MESSAGE_TRUSH_LEAVE, "cancel?"))
	{
		ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
		formMain_RedrawMainPanel();
	}
}

#pragma managed