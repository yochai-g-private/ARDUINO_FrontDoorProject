#include "FrontDoor.h"

#if YG_SIMULATOR
#include <Simulator.h>

#include "Clock.h"

#include "DoorManager.h"
#include "DoorOpen.h"

static void onSetup()
{
	SetCurrentTime(DaTi(2019, Nov, 1, 15, 50, 0));
	SetDoorOpen(false);
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
			return Simulator::ResultNextLoop();
		}

		case 1:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night, false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);

			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.door_open, false);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state, DoorState::LED_OFF);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_OFF);

			YATF_CHECK_EQUAL(DoorOpenActionIsScheduled(), false, nothing);

			SetDoorOpen(true);
			return Simulator::ResultTimerSeconds(2);
		}

		case 2:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night,  false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);
	
			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.door_open, true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state,	 DoorState::LED_TOGGLE);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_DELAYED);

			YATF_CHECK_EQUAL(DoorOpenActionIsScheduled(), true, nothing);

			AddToCurrentTime(DELAY_DOOR_OPEN_BUZZER_MINUTES * DaTi::SECONDS_PER_MINUTE);

			return Simulator::ResultTimerSeconds(1);
		}

		case 3:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night, false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);

			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.door_open, true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state,	 DoorState::LED_TOGGLE);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_TOGGLE_1_OF_60);

			YATF_CHECK_EQUAL(DoorOpenActionIsScheduled(), false, nothing);

			SetDoorOpen(false);
			return Simulator::ResultTimerMillis(DELAY_DOOR_OPEN_SECONDS * 1100);
		}

		case 4:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night, false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);

			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.door_open, false);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state, DoorState::LED_OFF);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_OFF);

			SetDoorOpen(true);
			return Simulator::ResultTimerSeconds(1);
		}

		case 5:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night, false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);

			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.door_open, true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state, DoorState::LED_TOGGLE);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_DELAYED);

			YATF_CHECK_EQUAL(DoorOpenActionIsScheduled(), true, nothing);

			SetDoorOpen(false);
			return Simulator::ResultNextLoop();
		}

		case 6:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night, false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);

			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.door_open, true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state, DoorState::LED_TOGGLE);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_DELAYED);

			YATF_CHECK_EQUAL(DoorOpenActionIsScheduled(), true, nothing);

			return Simulator::ResultTimerMillis(DELAY_DOOR_OPEN_SECONDS * 1100);
		}

		case 7:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night, false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);

			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.door_open, false);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state, DoorState::LED_OFF);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_OFF);

			YATF_CHECK_EQUAL(DoorOpenActionIsScheduled(), false, nothing);

			Simulator::ResultEnd();
		}

	}
	
	return Simulator::ResultEnd();
}
//------------------------------------------------
IMPLEMENT_SIMULATOR_SCENARIO(DM_StartCholOpenTheDoor);

#endif //YG_SIMULATOR
