#pragma unmanaged 
#include "ModeVizMask.h"
#include "OglForCLI.h"
#include "ImageCore.h"
#include "ModeCommonTools.h"

#pragma managed 
#include "FormMain.h"
#include "FormVisParam.h"
#include "FormVisMask.h"
#include "FormSelectMskId.h"
#pragma unmanaged 


using namespace RoiPainter4D;


ModeVizMask::ModeVizMask() :
  m_volumeShader("shader/volVtx.glsl", "shader/volFlg_Msk.glsl"),
  m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg_Msk.glsl")
{
  std::cout << "ModeVizMask const...----------------------\n";

  m_bL = m_bR = m_bM = false;

  std::cout << "ModeVizMask DONE -------------------------\n";
}

ModeVizMask::~ModeVizMask()
{
}


bool ModeVizMask::CanEndMode()
{
  return true;
}

int CalcIdx(const EVec3i &reso, const int &x, const int &y, const int &z)
{
  return (x + (y * reso[0]) + (z * reso[0] * reso[1]));
}

EVec3i CalcPos(const EVec3i &reso, const int& idx)
{
  int z = (int)(idx / (reso[0] * reso[1]));
  int y = (int)((idx % (reso[0] * reso[1])) / reso[0]);
  int x = idx % reso[0];
  return EVec3i(x, y, z);
}

EVec3i SearchManhattan(const EVec3i &reso, const std::vector<int> &goalIdxes, const EVec3i &startPos, const int &maxDist)
{
  for (int dist = 1; dist <= maxDist; dist++)
  {
	for (int dy = -dist; dy <= dist; dy++)
	{
	  int dz = dist - abs(dy);
	  int y1 = startPos[1] + dy;
	  int z1 = startPos[2] + dz;
	  int y2 = startPos[1] + dy;
	  int z2 = startPos[2] - dz;
	  for (int i = 0; i < goalIdxes.size(); i++)
	  {
		if (CalcIdx(reso, startPos[0], y1, z1) == goalIdxes[i]) return EVec3i(startPos[0], y1, z1);
		if (CalcIdx(reso, startPos[0], y2, z2) == goalIdxes[i]) return EVec3i(startPos[0], y2, z2);
	  }
	}
  }
  return EVec3i(-1, -1, -1);
}

EVec3f CalcVec(const EVec3f& pitch, const EVec3i& a, const EVec3i& b)
{
  EVec3f a_pos = EVec3f(pitch[0] * (float)(a[0] + 0.5), pitch[1] * (float)(a[1] + 0.5), pitch[2] * (float)(a[2] + 0.5));
  EVec3f b_pos = EVec3f(pitch[0] * (float)(b[0] + 0.5), pitch[1] * (float)(b[1] + 0.5), pitch[2] * (float)(b[2] + 0.5));
  return b_pos - a_pos;
}

void ModeVizMask::StartMode()
{
  m_bL = m_bR = m_bM = false;
  /*
  m_mask_mesh = MaskMeshSequence();
  m_tmeshes = std::vector<TMesh>(ImageCore::GetInst()->GetNumFrames());
  */

  formVisMask_Init();
  m_edge_4dIdx.clear();
  m_num_edgeIdxes.clear();
  m_vectors.clear();

  byte selectMaskIdx = static_cast<byte>(ImageCore::GetInst()->GetSelectMaskIdx());
  const EVec3i reso = ImageCore::GetInst()->GetReso();
  const int NUM_FRM = ImageCore::GetInst()->GetNumFrames();
  //一番外側にあるidx取得
  int a = selectMaskIdx;
  if (selectMaskIdx != -1 && selectMaskIdx != 0)
  {
	for (int f = 0; f < NUM_FRM; f++)
	{
	  int numIdx = 0;
	  for (int x = 0; x < reso[0]; x += 10)
	  {
		for (int y = 0; y < reso[1]; y++)
		{
		  for (int z = 0; z < reso[2]; z++)
		  {
			if (ImageCore::GetInst()->m_mask4d[f][CalcIdx(reso, x, y, z)] == selectMaskIdx)
			{
			  if (ImageCore::GetInst()->m_mask4d[f][CalcIdx(reso, x, y + 1, z)] != selectMaskIdx ||
				ImageCore::GetInst()->m_mask4d[f][CalcIdx(reso, x, y - 1, z)] != selectMaskIdx ||
				ImageCore::GetInst()->m_mask4d[f][CalcIdx(reso, x, y, z + 1)] != selectMaskIdx ||
				ImageCore::GetInst()->m_mask4d[f][CalcIdx(reso, x, y, z - 1)] != selectMaskIdx)
			  {
				m_edge_4dIdx.push_back(CalcIdx(reso, x, y, z));
				numIdx++;
			  }
			}
		  }
		}
	  }
	  m_num_edgeIdxes.push_back(numIdx);
	}

	//次時相の最近傍点までのベクトルを計算
	const EVec3f pitch = ImageCore::GetInst()->GetPitch();
	int num_idxes = 0;
	for (int f = 0; f < NUM_FRM - 1; f++)
	{
	  std::cout << "calcFrame" << f << "\n";
	  int num_nowIdxes = m_num_edgeIdxes[f];
	  for (int i = num_idxes; i < num_idxes + num_nowIdxes; i++)
	  {
		EVec3i pos = CalcPos(reso, m_edge_4dIdx[i]);
		int MAX_DIST = 100;
		std::vector<int> nextFrmIdxes;
		for (int j = 1; j < m_num_edgeIdxes[f + 1]; j++)
		{
		  nextFrmIdxes.push_back(m_edge_4dIdx[j + num_idxes + num_nowIdxes]);
		}
		EVec3i goal = SearchManhattan(reso, nextFrmIdxes, pos, MAX_DIST);
		if (goal[1] != -1 && goal[2] != -1)
		{
		  m_vectors.push_back(CalcVec(pitch, pos, goal));
		}
		else
		{
		  m_vectors.push_back(EVec3f(0, 0, 0));
		}
	  }
	  num_idxes += num_nowIdxes;
	}
	std::cout << m_vectors.size() << ", " << m_edge_4dIdx.size() << "\n";
  }
  /*
  //debug
  for (int i = 0; i < m_vectors.size(); i += 10)
  {
	std::cout << m_vectors[i][0] << ", " << m_vectors[i][1] << ", " << m_vectors[i][2] << "\n";
  }
  */
  

  /*
  //メッシュモデル取得
  const int scale = 2; //?
  if (!m_mask_mesh.LoadMask(scale))
  {
	std::cout << "Failed to load mask." << "\n";
  }
  for (int i = 0; i < ImageCore::GetInst()->GetNumFrames(); ++i) {
	m_tmeshes[i] = m_mask_mesh.GetMesh(i);
  }
  std::cout << "got meshes." << "\n";
  */
  UpdateImageCoreVisVolumes();
  formVisMask_Show();
  formVisMask_updateList();
}


////////////////////////////////////////////////////////////////////////////
//MouseListener/////////////////////////////////////////////////////////////
void ModeVizMask::LBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = true;
  ogl->BtnDown_Trans(p);
}

void ModeVizMask::LBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bL = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeVizMask::RBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = true;
  ogl->BtnDown_Rot(p);
}

void ModeVizMask::RBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeVizMask::MBtnDown(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}

void ModeVizMask::MBtnUp(const EVec2i& p, OglForCLI* ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeVizMask::LBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeVizMask::RBtnDclk(const EVec2i& p, OglForCLI* ogl) {}
void ModeVizMask::MBtnDclk(const EVec2i& p, OglForCLI* ogl) {}

void ModeVizMask::MouseMove(const EVec2i& p, OglForCLI* ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  ogl->MouseMove(p);
  formMain_RedrawMainPanel();
}



void ModeVizMask::MouseWheel(const EVec2i& p, short z_delta, OglForCLI* ogl)
{
  if (!WheelingCrssec(p, z_delta, ogl)) {
	ogl->ZoomCamera(z_delta * 0.1f);
  }
  formMain_RedrawMainPanel();
}


void ModeVizMask::KeyDown(int nChar) {}
void ModeVizMask::KeyUp(int nChar) {}

void ConvertToOpenGLMatrix(GLfloat m0[16])
{
  //変換前
	  //  a0   a1   a2   a3
	  //  a4   a5   a6   a7
	  //  a8   a9  a10  a11
	  // a12  a13  a14  a15


  //変換後
	  // a0  a4   a8  a12
	  // a1  a5   a9  a13
	  // a2  a6  a10  a14
	  // a3  a7  a11  a15

  GLfloat m1[16] = { 0 };

  m1[0] = m0[0];
  m1[1] = m0[4];
  m1[2] = m0[8];
  m1[3] = m0[12];
  m1[4] = m0[1];
  m1[5] = m0[5];
  m1[6] = m0[9];
  m1[7] = m0[13];
  m1[8] = m0[2];
  m1[9] = m0[6];
  m1[10] = m0[10];
  m1[11] = m0[14];
  m1[12] = m0[3];
  m1[13] = m0[7];
  m1[14] = m0[11];
  m1[15] = m0[15];

  memcpy(m0, m1, sizeof(GLfloat) * 16);
}


void ModeVizMask::DrawScene(const EVec3f& cuboid, const EVec3f& camP, const EVec3f& camF)
{
  const EVec3i reso = ImageCore::GetInst()->GetReso();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();

  ImageCore::GetInst()->UpdateImgMaskColor();

  BindAllVolumes();

  DrawCrossSections(cuboid, reso, !IsSpaceKeyOn(), m_crssecShader);

  //ベクトル表示
  if (m_vectors.size() > 0)
  {
	const int frm_idx = formVisParam_getframeI();
	int now_frmIdx = 0;
	for (int i = 0; i < frm_idx; i++)
	{
	  now_frmIdx += m_num_edgeIdxes[i];
	}

	
	glDisable(GL_LIGHTING);
	glLineWidth(3);
	// 半透明にするための設定
	glEnable(GL_BLEND); // ブレンドを有効にする
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 標準的なブレンド関数を設定

	// アルファ値を0.5（半透明）に設定
	glColor4d(0, 1, 0, 0.5);
	if (frm_idx != (ImageCore::GetInst()->GetNumFrames() - 1))
	{
	  for (int i = 0; i < m_num_edgeIdxes[frm_idx]; i++)
	  {
		int idx = i + now_frmIdx;
		EVec3i startPosIdx = CalcPos(reso, m_edge_4dIdx[idx]);
		EVec3f startPosf = EVec3f(pitch[0] * (startPosIdx[0] + 0.5), pitch[1] * (startPosIdx[1] + 0.5), pitch[2] * (startPosIdx[2] + 0.5));
		glBegin(GL_LINES);
		glVertex3f(startPosf[0], startPosf[1], startPosf[2]); glVertex3f(startPosf[0] + m_vectors[idx][0], startPosf[1] + m_vectors[idx][1], startPosf[2] + m_vectors[idx][2]);
		glEnd();
		if (true)
		{
		  // 矢じりのサイズと角度を設定
		  float arrowHeadLength = 0.15f; // 矢じりの長さ
		  float arrowHeadAngle = M_PI / 6.0f; // 矢じりの開き角度（30度）

		  // ベクトルの正規化
		  EVec3f direction = m_vectors[idx].normalized();

		  // 線分の終点
		  EVec3f endPos = startPosf + m_vectors[idx];

		  // 矢じりのための回転軸と角度を計算
		  // 矢じりはYZ平面に描画されるため、回転軸はX軸となります。
		  // ベクトルがX軸に平行な場合、回転軸を適切に選択する必要があります。
		  // ここでは、デフォルトの垂直方向をY軸正方向と仮定し、
		  // ベクトルとY軸との外積を取ることで回転軸を決定します。
		  EVec3f referenceVec(0.0f, 1.0f, 0.0f); // Y軸正方向を基準とする
		  if (direction.isApprox(referenceVec) || direction.isApprox(-referenceVec)) {
			// ベクトルがY軸に平行な場合、X軸を回転軸とする
			referenceVec = EVec3f(1.0f, 0.0f, 0.0f);
		  }

		  EVec3f rotationAxis = direction.cross(referenceVec).normalized();
		  if (rotationAxis.norm() < 1e-6) { // もし外積がゼロベクトルに近い場合（ベクトルと基準ベクトルが平行な場合）
			rotationAxis = EVec3f(1.0f, 0.0f, 0.0f); // X軸をデフォルトの回転軸とする
		  }

		  // 矢じりの2つの点を計算するための基底ベクトルを生成
		  // ベクトルに垂直で、YZ平面に存在するようなベクトル
		  // ここでは、方向ベクトルと回転軸の外積を取ることで、
		  // 矢じりの「横方向」のベクトルを得る
		  EVec3f perpendicularVec = direction.cross(rotationAxis).normalized();

		  // 矢じりの左側の点
		  // 終点から方向ベクトルを逆向きにarrowHeadLengthだけ進み、
		  // そこからperpendicularVecをarrowHeadAngleだけ回転させた方向のベクトルを加える
		  EVec3f leftPoint = endPos - direction * arrowHeadLength + perpendicularVec * std::tan(arrowHeadAngle) * arrowHeadLength;

		  // 矢じりの右側の点
		  EVec3f rightPoint = endPos - direction * arrowHeadLength - perpendicularVec * std::tan(arrowHeadAngle) * arrowHeadLength;

		  // 矢じりの描画 (GL_LINES)
		  // 矢じりの中心の頂点から左右の頂点へ線を描画
		  glBegin(GL_LINES);
		  glVertex3f(endPos[0], endPos[1], endPos[2]);
		  glVertex3f(leftPoint[0], leftPoint[1], leftPoint[2]);

		  glVertex3f(endPos[0], endPos[1], endPos[2]);
		  glVertex3f(rightPoint[0], rightPoint[1], rightPoint[2]);
		  glEnd();
		}
	  }
	  glDisable(GL_BLEND);
	}
  }

  if (formVisParam_bRendVol())
  {

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	bool b_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
	DrawVolumeSlices(cuboid, reso, camP, camF,
	  !IsSpaceKeyOn(), b_manip, m_volumeShader);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
  }

  /*
  //メッシュモデル上の最近傍点へのベクトル表示
  if (false)
  {
	int f_idx = formVisParam_getframeI();
	if (m_tmeshes[f_idx + 1].m_vSize)
	{
	  glDisable(GL_LIGHTING);
	  //glLineWidth(2);
	  glColor3d(0, 1, 0);
	  for (int i = 0; i < m_tmeshes[f_idx].m_vSize; i += 250)
	  {
		int p_idx = i;
		EVec3f p = m_tmeshes[f_idx].m_vVerts[p_idx];
		int q_idx = m_tmeshes[f_idx + 1].GetNearestVertexIdx(p);
		EVec3f q = m_tmeshes[f_idx + 1].m_vVerts[q_idx];
		EVec3f vec = EVec3f(q[0] - p[0], q[1] - p[1], q[2] - p[2]);
		float V = std::sqrtf(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
		float l = std::sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
		EVec3f new_z = EVec3f(vec[0] / V, vec[1] / V, vec[2] / V);

		glBegin(GL_LINES);
		glVertex3d(p[0], p[1], p[2]); glVertex3d(p[0] + (2 * new_z[0]), p[1] + (2 * new_z[1]), p[2] + (2 * new_z[2]));
		glEnd();

		glPushMatrix();
		{
		  glTranslatef(p[0] + (2 * new_z[0]), p[1] + (2 * new_z[1]), p[2] + (2 * new_z[2]));
		  EVec3f up(0.0f, 1.0f, 0.0f);
		  EVec3f new_x = up.cross(vec);
		  new_x.normalize();
		  EVec3f new_y = vec.cross(new_x);
		  GLfloat m1[16] = {
			  new_x[0], new_x[1], new_x[2], 0.0f,
			  new_y[0], new_y[1], new_y[2], 0.0f,
			  new_z[0], new_z[1], new_z[2], 0.0f,
				  0.0f,     0.0f,     0.0f, 1.0f
		  };
		  glMultMatrixf(m1);

		  
		  // 3. スケーリング: gluCylinderで描画される円錐の高さをdistance_pqに合わせる
		  // gluCylinderの高さは現在2.0fで指定されている。
		  // Z軸方向のスケーリングファクター = (目標の高さ distance_pq) / (gluCylinderのネイティブな高さ 2.0f)
		  float cylinder_native_height = 2.0f;
		  glScalef(1.0f, 1.0f, V / cylinder_native_height);
		  

		  //オブジェクト生成
		  GLUquadricObj* sphere = gluNewQuadric(); // 変数名はsphereのままですが、実際はconeです
		  //描画スタイルの設定
		  gluQuadricDrawStyle(sphere, GLU_FILL);
		  //円錐の描画 (底面半径0.5, 先端半径0, ネイティブ高さ2.0)
		  gluCylinder(sphere, 0.5, 0, 2.0, 20, 20);
		  //メモリ解放
		  gluDeleteQuadric(sphere);
		}
		glPopMatrix();
	  }
	}
  }
  */

}

