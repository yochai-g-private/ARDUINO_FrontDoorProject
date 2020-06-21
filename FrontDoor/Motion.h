#ifndef __Motion__
#define __Motion__

#include "FrontDoor.h"

#if COMPILE_MOTION

bool InitializeMotion();
Led& GetMotionLed();
const ITestTarget& GetMotionTest();


#endif //COMPILE_MOTION
#endif //__Motion__

