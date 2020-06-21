#include "Motion.h"

#if COMPILE_MOTION
#include <MotionDetector.h>
#include <Led.h>
#include <Timer.h>

static MotionDetector	motion("MOTION", PIN_MOTION_DETECTOR);
static Led				led("MOTION_DETECTED", PIN_LED_MOTION_DETECTED);
//--------------------------------------------------------------------------
class MotionObserver : public StableDigitalInputComponentObserver
{
public:

	MotionObserver() : StableDigitalInputComponentObserver(motion, 
#if YG_USE_LOGGER
														   "MotionSensor",
#endif //YG_USE_LOGGER
														   10) {	}

private:

	void set(bool on)
	{
		state.u.s.motion = on;
		led.Set(on);
	}
};

static MotionObserver observer;
//--------------------------------------------------------------------------
bool InitializeMotion()
{
	motion.Register(observer);
	return true;
}
//--------------------------------------------------------------------------
Led& GetMotionLed() { return led; }
//--------------------------------------------------------------------------
const ITestTarget& GetMotionTest()
{
	struct MotionTest : ITestTarget
	{
		MotionTest() {	}

		void Reset()
		{
			motion.Unregister(observer);
		}

		void Set()
		{
			motion.Register(observer);
		}

		void OnPush(Tester& tester)
		{
		}

#if YG_USE_LOGGER
		const char* GetName()  const { return "MOTION"; }
#endif
	};

	static MotionTest test;
	return test;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
#endif //COMPILE_MOTION
