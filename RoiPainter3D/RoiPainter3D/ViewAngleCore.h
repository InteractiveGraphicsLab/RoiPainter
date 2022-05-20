#ifndef VIEWANGLECORE_H_
#define VIEWANGLECORE_H_
#pragma unmanaged

#include "tmath.h"
#include "OglImage.h"
class OglForCLI;

//------------------------------------
//class ViewAngleCore
//
//2019
//written by Takashi Ijiri, Daisuke Niino
//
//This class manage camera orientation indicator 
//
//-------------------------------------

class ViewAngleCore
{
  OGLImage2D4 m_texture;

  ViewAngleCore();
public:
  ~ViewAngleCore();

  static ViewAngleCore *GetInst()
  {
    static ViewAngleCore p;
    return &p;
  }

  void DrawIndicator(
    int screen_width, 
    int screen_height, 
    const EVec3f &camera_pos, 
    const EVec3f &camera_center, 
    const EVec3f &camera_ydir);

  //return ID of plane 0:non, 1:A, 2:R, 3:P, 4:L, 5:T, 6:B  
  int  PickIndicator(
    const OglForCLI &ogl, 
    int screen_width, 
    int screen_height, 
    const EVec3f &camera_pos, 
    const EVec3f &camera_center, 
    const EVec3f &camera_ydir,
    const EVec2i &cursor_pos );
};


#endif