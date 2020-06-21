#include "DoorOpen.h"
#include "DoorManager.h"

#if COMPILE_DOOR_OPEN
#include <DoorOpenDetector.h>
#include <Led.h>
#include "DoorManager.h"

static DipSwitch	door_open("DOOR_OPEN", PIN_DOOR_OPEN);
//--------------------------------------------------------------------------
class DoorOpenObserver : public StableDigitalInputComponentObserver
{
public:

	DoorOpenObserver() : StableDigitalInputComponentObserver(door_open,
#if YG_USE_LOGGER
															 "DoorOpen",
#endif //YG_USE_LOGGER
															 DELAY_DOOR_OPEN_SECONDS,
															 StableDigitalInputComponentObserver::DELAY_ON_HIGH) 	
	{	
		//OnChange(door_open);
	}


private:

	void set(bool on)
	{
		_TRC("DoorOpenObserver: " << !on << NL);
		state.u.s.door_open = !on;

		if(!state.u.s.door_open)
			CancelDoorOpenSheduledAction();
	}
};

static DoorOpenObserver observer;
//--------------------------------------------------------------------------
bool InitializeDoorOpen()
{
	//door_open.SetReverseMode(true);
	door_open.Register(observer);
	return true;
}
//--------------------------------------------------------------------------
const ITestTarget& GetDoorOpenTest()
{
	struct DoorOpenTest : ITestTarget
	{
		DoorOpenTest() {	}

		void Reset()
		{
			door_open.Unregister(observer);
		}

		void Set()
		{
			door_open.Register(observer);
		}

		void OnPush(Tester& tester)
		{
		}

#if YG_USE_LOGGER
		const char* GetName()  const { return "DOOR"; }
#endif
	};
	
	static DoorOpenTest test;
	return test;
}
//--------------------------------------------------------------------------
#if YG_SIMULATOR
void SetDoorOpen(bool open)
{
	//TRACER << "YOCHAI: " << __FUNCTION__ << NL);
	door_open.Simulate(!open);
}
#endif //YG_SIMULATOR
//--------------------------------------------------------------------------
#endif //COMPILE_DOOR_OPEN
