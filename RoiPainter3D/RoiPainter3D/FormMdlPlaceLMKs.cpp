#include "FormMdlPlaceLMKs.h"
#include "FormVisParam.h"
#include "FormMain.h"

#pragma unmanaged
#include "mode/ModeMdlPlaceLMKs.h"
#pragma managed

using namespace RoiPainter3D;

System::Void FormMdlPlaceLMKs::m_trackbar_setIsoValue__Scroll(System::Object^ sender, System::EventArgs^ e) {
  m_textbox_setIsoValue->Text = m_trackbar_setIsoValue->Value.ToString();
}

System::Void FormMdlPlaceLMKs::m_btn_genIsosurface_Click(System::Object^ sender, System::EventArgs^ e) {
  int value = m_trackbar_setIsoValue->Value;
  ModeMdlPlaceLMKs::GetInst()->GenIsoSurFace(value);
}

