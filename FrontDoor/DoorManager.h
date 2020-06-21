#ifndef __DoorManager__
#define __DoorManager__

#include <Tester.h>

using namespace YG;

bool InitializeDoorManager();
bool CheckDoorStatus(bool first_loop);
const ITestTarget& GetDoorOpenTest();
const ITestTarget& GetShabatSwitchTest();

struct DoorState
{
	union Inputs
	{
		struct
		{
			bool door_open					: 1;
			bool alarm_disabled_switch		: 1;
			bool TEMP_REMOVED_time_to_switch				: 1;
			bool kodesh						: 1;
			int	 reserved_for_future_use	: 4;
		}		s;

		uint8_t n;
	};

	enum LedState
	{
		LED_OFF				= 0,
		LED_TOGGLE,
		LED_TOGGLE_FAST,
		LED_TOGGLE_SLOW,
		LED_TOGGLE_1_OF_15,
		LED_TOGGLE_SOS,
	};

	enum BuzzerState
	{
		BUZZER_OFF				= 0,
		BUZZER_DELAYED,
		BUZZER_TOGGLE_1_OF_15,
		BUZZER_TOGGLE_1_OF_60,		// after delay
	};

	struct Outputs
	{
		LedState	led_state;
		BuzzerState	buzzer_state;
	};

	Inputs	inputs;
	Outputs	outputs;
};

DoorState GetDoorState();
void ShowDoorState(const DoorState& state);
bool CancelDoorOpenSheduledAction();
bool DoorOpenActionIsScheduled();

enum DoorInternalsId
{
	DOOR_BUZZER_TIMER_IS_STARTED,
	DOOR_BUZZER_IS_ON,
	DOOR_LED_IS_ON,
	DOOR_SHABAT_RELAY_IS_ON,
};

bool GetDoorInternalsBooleanState(DoorInternalsId id);

enum
{
#if YG_NO_SIMULATOR
	DELAY_DOOR_OPEN_BUZZER_MINUTES = 5,
#else
	DELAY_DOOR_OPEN_BUZZER_MINUTES = 1,
#endif
};

#define YATF_CHECK_DOOR_STATE_EQUAL(a,b) YATF_CHECK_EQUAL(a,b,ShowDoorState(GetDoorState()))

#endif //__DoorManager__

