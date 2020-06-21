#ifndef __ShabatSwitch__
#define __ShabatSwitch__

#include "FrontDoor.h"

#if COMPILE_SHABAT_SWITCH

bool InitializeShabatSwitch();
void UpdateShabatSwitchState();
const ITestTarget& GetShabatSwitchTest();

#if YG_SIMULATOR
void SetShabatSwitch(bool shabat);
#endif //YG_SIMULATOR

#endif //COMPILE_SHABAT_SWITCH
#endif //__ShabatSwitch__

