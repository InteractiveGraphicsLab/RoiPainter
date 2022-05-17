#pragma managed
#include "FormLoadFrameIdx.h"

using namespace RoiPainter4D;

void FormLoadFrameIdx::setIndex(int &startI, int &endI)
{
  startI = Decimal::ToInt32(startIdxUpDown->Value);
  endI = Decimal::ToInt32(endIdxUpDown->Value);
}
