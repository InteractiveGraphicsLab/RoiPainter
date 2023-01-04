#pragma managed 
#include "FormMain.h"

#pragma unmanaged 
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"
#include <iostream>
#include "mode/ModeVizMask.h"
#include "mode/ModeVizNormal.h"
#include "mode/ModeSegRGrow.h"
#include "mode/ModeSegGCut.h"
#include "mode/ModeSegLocalRGrow.h"
#include "mode/ModeSegThreshfieldPaint.h"
#include "mode/ModeSegVoxelPaint.h"
#include "mode/ModeRefStrokeTrim.h"
#include "mode/ModeRefSplitByPlane.h"

#pragma managed 



using namespace System;
using namespace RoiPainter3D;


[STAThreadAttribute]
int main()
{
  std::cout << "start main function\n";

  //Initialize non-managed Singletons
  //��managed�ȃV���O���g���͂����ŏ�����
  //���������̃^�C�~���O�ł́AForm�𐶐���show���Ă͂��߁D
    
  ModeCore  ::GetInst();
  ImageCore ::GetInst();
  CrssecCore::GetInst();

  ModeVizMask::getInst();
  ModeVizNormal::getInst();
  ModeSegRGrow::GetInst();
  ModeSegGCut::getInst();
  ModeSegLocalRGrow::GetInst();
  ModeSegThreshfieldPaint::getInst();
  ModeSegVoxelPaint::GetInst();
  ModeRefStrokeTrim::GetInst();
  ModeRefSplitByPlane::GetInst();

  std::cout << "FormMain::getInst()->ShowDialog() \n";
  FormMain::getInst()->ShowDialog();
  std::cout << "FormMain::getInst()->ShowDialog() DONE\n";

  return 0;
}



// UPDATE Memo
// mode_vis_norm   OK
//   implement overall workflow    OK
//   support pixel val vis         OK
//   support gradient magnitude    OK
//   support gradient magnitude tf OK
//
// modeVisMask OK
//   implement overall workflow OK
//   formVisMask                OK
//   maskManipulations          OK
//   I/O                        OK
//
// modeSegRgrow
//   formSegRGrow    OK
//   Thesholing      OK
//   RegionGrowing6  OK
//   RegionGrowing26 OK
//
// modeSegRgrow
//   formSegGCut
//   watershed (bk-thread) 
//   graphcut Wsd level   
//   graphcut voxel level 
//   erode/dilate/fillhole 
//
//
//
// CORE_CLASSES
//   ImageCore     OK 
//   CrssecCore    OK
//   ViewAngleCore OK
//   ModeCore      80%
//
// 
// event handler (MainForm): 
//    Wheeling OK
//    mouse    OK
//    resize   OK
//    repaint  OK
//
//
// IO
//   initial volume    OK
//   2D slices bmp/tif OK
//   2D slices dcm     OK
//   3D traw3D         OK
//   3D dcm            OK
//   save / load mask  OK 
//   export traw3d_ss  TODO YET TODO YET 
//   open fav          TODO YET TODO YET 
//   save fav          TODO YET TODO YET 
//   show_dlg for stack orientation TODO YET TODO YET 
//   
// etc...

/*

todo 
fav�Ή�
Fav�̃R���p�C��
1. xerces_c_3.1.4���擾
2. xerces-c-3.1.4\projects\Win32\VC14\xerces-all���̃v���W�F�N�g�t�@�C�����J��
3. x64��release���[�h��debug���[�h�ŃR���p�C��
4. fav-project���N���[��
5. fav�v���W�F�N�g���@fav-project\FavLibrary\FavLibrary.Win\packages\��xerces-c-3.1.4�t�H���_���R�s�[
6. fav�v���W�F�N�g���R���p�C�� (FavLibrary.dll/FavLibrary.lib���ł���)
*/