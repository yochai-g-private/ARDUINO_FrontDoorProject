#include "FrontDoor.h"

#if YG_SIMULATOR
#include <Simulator.h>

#include "Clock.h"

#include "DoorManager.h"
#include "DoorOpen.h"
#include "ShabatSwitch.h"

static void onSetup()
{
	SetCurrentTime(DaTi(2019, Nov, 14, 8, 1, 2));
	SetDoorOpen(false);
	SetShabatSwitch(false);
}
//------------------------------------------------
static Simulator::StepResult onLoop(int case_id)
{
	ShowCurrentTime();
	DoorState door_state = GetDoorState();

	switch(case_id)
	{
		case 0:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night,		false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh,	false);
			YATF_CHECK_STATE_EQUAL(state.shabat_switch, false);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.alarm_disabled_switch, false);

			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state,		DoorState::LED_OFF);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state,	DoorState::BUZZER_OFF);

			SetShabatSwitch(true);

			return Simulator::ResultTimerMillis(500);
		}

		case 1:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night,		false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh,	false);
			YATF_CHECK_STATE_EQUAL(state.shabat_switch, true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.alarm_disabled_switch, true);

			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), false);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state,	 DoorState::LED_TOGGLE_SLOW);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_OFF);

			SetShabatSwitch(false);

			return Simulator::ResultTimerMillis(500);
		}

		case 2:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night, false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);
			YATF_CHECK_STATE_EQUAL(state.shabat_switch, false);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.alarm_disabled_switch, false);

			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state, DoorState::LED_OFF);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_OFF);

			return Simulator::ResultEnd();
		}
	}
	
	return Simulator::ResultEnd();
}
//------------------------------------------------
IMPLEMENT_SIMULATOR_SCENARIO(DM_StartCholSwitchToShabat);

#endif //YG_SIMULATOR
