#include "FrontDoor.h"

#if YG_SIMULATOR
#include <Simulator.h>

#include "Clock.h"

#include "DoorManager.h"
#include "DoorOpen.h"
#include "ShabatSwitch.h"

static void onSetup()
{
	SetCurrentTime(DaTi_Before_MOTZEY);
	SetShabatSwitch(true);
}
//------------------------------------------------
static Simulator::StepResult onLoop(int case_id)
{
	ShowCurrentTime();
	DoorState door_state = GetDoorState();
	ShowDoorState(door_state);

	switch (case_id)
	{
		SKIP_FIRST_TIME();

		case 1:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night, true);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, true);
			YATF_CHECK_STATE_EQUAL(state.shabat_switch, true);

			return ResultKodeshSwitch();
		}

		case 2:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night, true);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);
			YATF_CHECK_STATE_EQUAL(state.shabat_switch, true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.alarm_disabled_switch, true);
			//YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.time_to_switch, true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state, DoorState::LED_TOGGLE_SLOW);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_OFF);
			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), false);

			return Simulator::ResultTimerSeconds(2);
		}

		case 3:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night, true);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);
			YATF_CHECK_STATE_EQUAL(state.shabat_switch, true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.alarm_disabled_switch, true);
//			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.time_to_switch, true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state, DoorState::LED_TOGGLE_SLOW);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_OFF);
			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), false);

			SetShabatSwitch(false);

			return Simulator::ResultTimerSeconds(1);
		}

		case 4:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night, true);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);
			YATF_CHECK_STATE_EQUAL(state.shabat_switch, false);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.alarm_disabled_switch, false);
//			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.time_to_switch, false);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state, DoorState::LED_OFF);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_OFF);
			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			return Simulator::ResultEnd();
		}
	}
	
	return Simulator::ResultEnd();
}
//------------------------------------------------
IMPLEMENT_SIMULATOR_SCENARIO(DM_BeforeMotzeyShabat_2);

#endif //YG_SIMULATOR
