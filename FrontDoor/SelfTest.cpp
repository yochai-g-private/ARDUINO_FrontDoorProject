#include "SelfTest.h""

#if COMPILE_SELF_TEST
#include "Bell.h""
#include "Motion.h""
#include "Sonar.h""
#include "DoorOpen.h""

//--------------------------------------------------------------------------
static ITestTarget* tests[] =
{
	&BeginTests::sm_instance,
#if COMPILE_BELL
	&GetBellTest(),
#endif //COMPILE_BELL
#if COMPILE_MOTION
	&GetMotionTest(),
#endif //COMPILE_MOTION
#if COMPILE_SONAR
	&GetSonarTest(),
#endif //COMPILE_SONAR
#if COMPILE_DOOR_OPEN
	&GetDoorOpenTest(),
#endif //COMPILE_DOOR_OPEN
	&EndTests::sm_instance,
	NULL
};
//--------------------------------------------------------------------------
static DualTimedButton	button(PIN_SELF_TEST_BUTTON);

#if YG_USE_LOGGER
typedef Tester	SelfTest;
#else
class SelfTest : public Tester
{
public:

	SelfTest(DualTimedButton& button, ITestTarget** tests) : Tester(button, tests)	{	}	

	void EnterTarget(const ITestTarget& target)
	{
		Muuuu();
	}

	virtual void Signal(const ITestTarget& target, bool on)
	{
		Tzif();
	}
};
#endif //YG_USE_LOGGER

static SelfTest	button_observer(button, tests);
//--------------------------------------------------------------------------
bool InitializeSelfTest()
{
	button.Register(button_observer);
	return true;
}
//--------------------------------------------------------------------------
#endif //COMPILE_SELF_TEST

