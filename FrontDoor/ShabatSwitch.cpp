#include "ShabatSwitch.h"

#if COMPILE_SHABAT_SWITCH
#include <DoorOpenDetector.h>
#include <Led.h>
#include "DoorManager.h"

//static DoorOpenDetector		shabat_sw(PIN_SHABAT_SWITCH);
static DipSwitch			shabat_sw("SHABAT_SWITCH", PIN_SHABAT_SWITCH);
//static Led				R_led(PIN_SHABAT_SWITCH_ON_LED);
//static Led				G_led(PIN_SHABAT_SWITCH_OFF_LED);
//--------------------------------------------------------------------------
class ShabatSwitchObserver : public StableDigitalInputComponentObserver
{
public:

	ShabatSwitchObserver() : StableDigitalInputComponentObserver(shabat_sw,
#if YG_USE_LOGGER
																 "ShabatSwitch",
#endif //YG_USE_LOGGER
																 0) 	{		}

private:

	void set(bool on)
	{
		_TRC("ShabatSwitchObserver: " << on << NL );
		state.shabat_switch = on;

		SetSensors();
	}
};

static ShabatSwitchObserver observer;
//--------------------------------------------------------------------------
bool InitializeShabatSwitch()
{
	shabat_sw.Register(observer);
	return true;
}
//--------------------------------------------------------------------------
void UpdateShabatSwitchState()
{
	state.shabat_switch = shabat_sw.IsOn();
}
//--------------------------------------------------------------------------
const ITestTarget& GetShabatSwitchTest()
{
	struct ShabatSwitchTest : ITestTarget
	{
		ShabatSwitchTest() {	}

		void Reset()
		{
//			shabat_sw.Unregister(observer);
		}

		void Set()
		{
//			shabat_sw.Register(observer);
		}

		void OnPush(Tester& tester)
		{
		}

		const char* GetName()  const { return shabat_sw.GetName(); }
	};

	static ShabatSwitchTest test;
	return test;
}
#if YG_SIMULATOR
void SetShabatSwitch(bool shabat)
{
	shabat_sw.Simulate(shabat);
}
#endif //YG_SIMULATOR
//--------------------------------------------------------------------------
#endif //COMPILE_SHABAT_SWITCH
