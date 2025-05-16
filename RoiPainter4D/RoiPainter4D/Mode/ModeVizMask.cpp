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


void ModeVizMask::StartMode()
{
  m_bL = m_bR = m_bM = false;
  m_mask_mesh = MaskMeshSequence();
  m_tmeshes = std::vector<TMesh>(ImageCore::GetInst()->GetNumFrames());
  //ImageCore::GetInst()->SetSelectMaskId(2); //ベクトル場を表示するマスクID、編集が必要
  ImageCore::GetInst()->SetSelectMaskId(3);
  const int scale = 2; //?
  if (!m_mask_mesh.LoadMask(scale))
  {
      std::cout << "Failed to load mask." << "\n";
  }
  for (int i = 0; i < ImageCore::GetInst()->GetNumFrames(); ++i) {
      m_tmeshes[i] = m_mask_mesh.GetMesh(i);
  }
  std::cout << "got meshes." << "\n";
  
  UpdateImageCoreVisVolumes();
  formVisMask_Show();
  formVisMask_updateList();
}


////////////////////////////////////////////////////////////////////////////
//MouseListener/////////////////////////////////////////////////////////////
void ModeVizMask::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;
  ogl->BtnDown_Trans(p);
}

void ModeVizMask::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeVizMask::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
  ogl->BtnDown_Rot(p);
}

void ModeVizMask::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeVizMask::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}

void ModeVizMask::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_RedrawMainPanel();
}


void ModeVizMask::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizMask::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizMask::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}

void ModeVizMask::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  ogl->MouseMove(p);
  formMain_RedrawMainPanel();
}



void ModeVizMask::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  if(!WheelingCrssec(p, z_delta, ogl) ) {
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


void ModeVizMask::DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
  const EVec3i reso = ImageCore::GetInst()->GetReso();

  ImageCore::GetInst()->UpdateImgMaskColor();

  BindAllVolumes();

  DrawCrossSections( cuboid, reso, !IsSpaceKeyOn(), m_crssecShader);

  if ( formVisParam_bRendVol() )
  {
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    bool b_manip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    DrawVolumeSlices( cuboid, reso, camP, camF, 
                    !IsSpaceKeyOn(), b_manip, m_volumeShader);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }
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
      glVertex3d(p[0], p[1], p[2]); glVertex3d(p[0] + (2*new_z[0]), p[1] + (2*new_z[1]), p[2] + (2 * new_z[2]));
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
          
          /*
          // 3. スケーリング: gluCylinderで描画される円錐の高さをdistance_pqに合わせる
          // gluCylinderの高さは現在2.0fで指定されている。
          // Z軸方向のスケーリングファクター = (目標の高さ distance_pq) / (gluCylinderのネイティブな高さ 2.0f)
          float cylinder_native_height = 2.0f;
          glScalef(1.0f, 1.0f, V / cylinder_native_height);
          */

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

