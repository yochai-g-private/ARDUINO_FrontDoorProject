#ifndef __Sonar__
#define __Sonar__

#include "FrontDoor.h"

#if COMPILE_SONAR

bool InitializeSonar();
Led& GetSonarLed();
const ITestTarget& GetSonarTest();

#endif //COMPILE_SONAR
#endif //__Sonar__

