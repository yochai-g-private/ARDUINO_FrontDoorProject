#include "Sonar.h"

#if COMPILE_SONAR
#include <DistanceDetector.h>
#include <Led.h>
#include <Timer.h>

static DistanceDetector	sonar("SONAR", PIN_SONAR_TRIG, PIN_SONAR_ECHO);
static Led				led("DISTANCE_CHANGED", PIN_LED_DISTANCE_CHANGED);
static Timer			delay_timer;
//--------------------------------------------------------------------------
class DistanceDetectorObserver : public Observer
{
	void OnChange(InputComponent& component)
	{
		state.distance = sonar.GetDistance();

		if(delay_timer.IsStopped())
		{
			led.On();
			state.u.s.distance_changed = true;
		}

		delay_timer.RestartMilli(5 * 1000, delay_done, NULL, 1);
	}

	static void delay_done(Timer& timer, void* context)
	{
		led.Off();
		state.u.s.distance_changed = false;
	}
};

static DistanceDetectorObserver observer;
//--------------------------------------------------------------------------
bool InitializeSonar()
{
	sonar.Register(observer);
	sonar.Start();

	return true;
}
//--------------------------------------------------------------------------
Led& GetSonarLed() { return led; }
//--------------------------------------------------------------------------
const ITestTarget& GetSonarTest()
{
	struct SonarTest : ITestTarget
	{
		SonarTest() {	}

		void Reset()
		{
			sonar.Unregister(observer);
		}

		void Set()
		{
			sonar.Register(observer);
		}

		void OnPush(Tester& tester)
		{
		}

#if YG_USE_LOGGER
		const char* GetName()  const { return "SONAR"; }
#endif
	};

	static SonarTest test;
	return test;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
#endif //COMPILE_SONAR
