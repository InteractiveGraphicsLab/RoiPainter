#ifndef VIEWINDICORE_H_
#define VIEWINDICORE_H_

#pragma unmanaged
#include "tmath.h"
#include "OglImage.h"

class OglForCLI;

class ViewIndiCore
{
  OGLImage2D4 m_texture;

  ViewIndiCore();
public:
  ~ViewIndiCore();

  static ViewIndiCore *getInst()
  {
    static ViewIndiCore p;
    return &p;
  }

  void DrawIndicator(int curViewW, int curViewH, EVec3f camP, EVec3f camC, EVec3f camY);
  int  PickIndicator(const OglForCLI *ogl, 
                      int curViewW, int curViewH, 
                      EVec3f camP, EVec3f camC, EVec3f camY, EVec2i p);//0:non, 1:A, 2:R, 3:P, 4:L, 5:T, 6:B  
};

#endif