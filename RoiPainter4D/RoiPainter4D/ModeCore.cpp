#pragma unmanaged
#include "ModeCore.h"

#include "Mode/ModeVizNormal.h"
#include "Mode/ModeVizMask.h"
#include "Mode/ModeSegRGrow.h"
#include "Mode/ModeSegPixPaint.h"
#include "Mode/ModeSegRigidICP.h"
#include "Mode/ModeSegLocalRGrow.h"
#include "Mode/ModeSegBolus.h"
#include "Mode/ModeSegSwallowOrgans.h"
#include "Mode/ModeRefStrokeTrim.h"
#include "Mode/ModeSegJointTracker.h"
#include "Mode/ModeSegBronchi.h"
#include "Mode/ModeSegStrokeFfd.h"
#include "Mode/ModePlaceCPs.h"
#include "Mode/ModeRefCurveDeform.h"

#pragma managed
#include "FormVisMask.h"
#include "FormVisNorm.h"
#include "FormSegPixPaint.h"
#include "FormSegRGrow.h"
#include "FormSegRigidICP.h"
#include "FormSegLocalRGrow.h"
#include "FormSegJointTracker.h"
#include "FormSegBolus.h"
#include "FormSegSwallowOrgans.h"
#include "FormSegSwallowOrganTimeline.h"
#include "FormSegBronchi.h"
#include "FormRefStrokeTrim.h"
#include "FormSegStrokeFfd.h"
#include "FormPlaceCPs.h"
#include "FormRefCurveDeform.h"
#pragma unmanaged

using namespace RoiPainter4D;


ModeCore::ModeCore()
{
  std::cout << "ModeCore Constructor\n";

  m_mode = ModeVizNormal::GetInst();
  //m_mode->startMode();

  std::cout << "ModeCore Constructor Done \n";
}



ModeCore::~ModeCore()
{}


void ModeCore::ModeSwitch(MODE_ID m)
{

  std::cout << "\n ModeSwitch " << m << "\n\n";

  if (!m_mode->CanEndMode()) return;

  if      (m == MODE_VIS_MASK   )   m_mode = ModeVizMask::GetInst();
  else if (m == MODE_SEG_REGGROW)   m_mode = ModeSegRGrow::GetInst();
  else if (m == MODE_SEG_PIXPAINT)  m_mode = ModeSegPixPaint::GetInst();
  else if (m == MODE_SEG_RIGIDICP)  m_mode = ModeSegRigidICP::GetInst();
  //else if (m == MODE_SEG_PARACONT) 
  //  m_mode = ModeSegParaConts::getInst();
  else if (m == MODE_SEG_LCLRGROW)  m_mode = ModeSegLocalRGrow::GetInst();
  else if (m == MODE_SEG_BOLUS)     m_mode = ModeSegBolus::getInst();
  else if (m == MODE_SEG_SWALLOW)   m_mode = ModeSegSwallowOrgans::GetInst();
  else if (m == MODE_REF_STRKTRIM)  m_mode = ModeRefStrokeTrim::GetInst();
  else if (m == MODE_SEG_JTRACKER)  m_mode = ModeSegJointTracker::GetInst();
  else if (m == MODE_SEG_STROKEFFD) m_mode = ModeSegStrokeFfd::GetInst();
  else if (m == MODE_SEG_BRONCHI )  m_mode = ModeSegBronchi::GetInst();
  else if (m == MODE_PLC_CPS)       m_mode = ModePlaceCPs  ::GetInst();
  else if (m == MODE_REF_CURVEDEFORM)       m_mode = ModeRefCurveDeform::GetInst();
  else
    m_mode = ModeVizNormal ::GetInst();

  // Hide all Forms
  formVisMask_Hide();
  formVisNorm_Hide();
  formSegPixPaint_Hide();
  formSegRGrow_Hide();
  FormSegRigidICPHide();
  FormSegLocalRGrow_Hide();
  FormSegSwallowOrgans_Hide();
  formRefStrokeTrim_Hide();
	formSegBolus_Hide();
  FormSegJointTracker_Hide();
  FormSegSwallowTimeline_Hide();
  formSegBronchi_Hide();
  FormSegStrokeFfd_Hide();
  FormPlaceCPs_Hide();
  FormRefCurveDeform_Hide();

  m_mode->StartMode();
}


//
//static void t_drawFrame(const EVec3f &c)
//{
//  glDisable(GL_LIGHTING);
//  glLineWidth(2);
//  glColor3d(1, 1, 0);
//  glBegin(GL_LINES);
//  glVertex3d( 0  ,  0  ,  0  ); glVertex3d(c[0],   0 ,   0 );
//  glVertex3d(c[0],  0  ,  0  ); glVertex3d(c[0], c[1],   0 );
//  glVertex3d(c[0], c[1],  0  ); glVertex3d( 0  , c[1],   0 );
//  glVertex3d( 0  , c[1],  0  ); glVertex3d( 0  ,   0 ,   0 );
//  glVertex3d( 0  ,  0  , c[2]); glVertex3d(c[0],   0 , c[2]);
//  glVertex3d(c[0],  0  , c[2]); glVertex3d(c[0], c[1], c[2]);
//  glVertex3d(c[0], c[1], c[2]); glVertex3d( 0  , c[1], c[2]);
//  glVertex3d( 0  , c[1], c[2]); glVertex3d( 0  ,   0 , c[2]);
//  glVertex3d( 0  ,  0  ,  0  ); glVertex3d( 0  ,   0 , c[2]);
//  glVertex3d(c[0],  0  ,  0  ); glVertex3d(c[0],   0 , c[2]);
//  glVertex3d(c[0], c[1],  0  ); glVertex3d(c[0], c[1], c[2]);
//  glVertex3d( 0  , c[1],  0  ); glVertex3d( 0  , c[1], c[2]);
//  glEnd();
//}
//
