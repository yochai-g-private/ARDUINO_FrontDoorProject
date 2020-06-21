#ifndef __Clock__
#define __Clock__

#include "FrontDoor.h"

#if YG_USE_KODESH_WATCH
#include <IRTC.h>

bool InitializeClock();
DaTi GetNextHadlaka();
DaTi GetNextMotzeyKodesh();
void UpdateClockFromSerial();

#endif //YG_USE_KODESH_WATCH
#endif //__Clock__

