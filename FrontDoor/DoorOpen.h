#ifndef __DoorOpen__
#define __DoorOpen__

#include "FrontDoor.h"

#if COMPILE_DOOR_OPEN

bool InitializeDoorOpen();
//void UpdateDoorOpenState();
const ITestTarget& GetDoorOpenTest();

#if YG_SIMULATOR
void SetDoorOpen(bool open);
#endif //YG_SIMULATOR

enum
{
#if YG_NO_SIMULATOR
	DELAY_DOOR_OPEN_SECONDS = 5,
#else
	DELAY_DOOR_OPEN_SECONDS = 1,
#endif
};

#endif //COMPILE_DOOR_OPEN
#endif //__DoorOpen__

