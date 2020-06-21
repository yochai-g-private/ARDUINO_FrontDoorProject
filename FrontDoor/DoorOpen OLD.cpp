#include "DoorOpen.h"
#include "DoorManager.h"

#if COMPILE_DOOR_OPEN
#include <Voltmeter.h>
#include <Led.h>

static Voltmeter	voltmeter("DOOR_OPEN", PIN_DOOR_OPEN, false, 5);;
#define DOOR_OPEN_THRESHOLD		1.25
//static Led				led(PIN_DOOR_OPEN_LED);
//--------------------------------------------------------------------------
class DoorOpenObserver : public StableAnalogInputComponentObserver_double
{
public:

	DoorOpenObserver() : StableAnalogInputComponentObserver_double(voltmeter,
																   DOOR_OPEN_THRESHOLD,
#if YG_USE_LOGGER
																   "DoorOpenSensor", 
#endif //YG_USE_LOGGER
																   DELAY_DOOR_OPEN_SECONDS) {	}

private:

	void set(bool on)
	{
		if (state.u.s.door_open != on)
		{
			_LOG("DoorOpenObserver: " << on << " (" << voltmeter.GetVolts() << "V)" << NL);
//			_TRC("DoorOpenObserver: " << on << " (" << voltmeter.GetVolts() << "V)" << NL);
			state.u.s.door_open = on;
			CancelDoorOpenSheduledAction();
		}
	}
};

static DoorOpenObserver observer;
//--------------------------------------------------------------------------
bool InitializeDoorOpen()
{
	voltmeter.Register(observer);
	return true;
}
//--------------------------------------------------------------------------
//void UpdateDoorOpenState()
//{
//	state.u.s.door_open = voltmeter.GetVolts() >= DOOR_OPEN_THRESHOLD;
//}
//--------------------------------------------------------------------------
const ITestTarget& GetDoorOpenTest()
{
	struct DoorOpenTest : ITestTarget
	{
		DoorOpenTest() {	}

		void Reset()
		{
			voltmeter.Unregister(observer);
		}

		void Set()
		{
			voltmeter.Register(observer);
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
	voltmeter.SimulateScaled(open ? DOOR_OPEN_THRESHOLD * 1.1 : 0);
	//uint16_t unscaled;
	//double scaled;

	//voltmeter.Simulate(777);
	//unscaled = voltmeter.GetUnscaledValue();
	//scaled   = voltmeter.GetScaledValue();
	//_LOG("A. unscaled=" << unscaled << " scaled=" << scaled << NL);
	//voltmeter.Simulate(0);
	//voltmeter.SimulateScaled(scaled);
	//unscaled = voltmeter.GetUnscaledValue();
	//scaled = voltmeter.GetScaledValue();
	//_LOG("B. unscaled=" << unscaled << " scaled=" << scaled << NL);
}
#endif //YG_SIMULATOR
//--------------------------------------------------------------------------
#endif //COMPILE_DOOR_OPEN
