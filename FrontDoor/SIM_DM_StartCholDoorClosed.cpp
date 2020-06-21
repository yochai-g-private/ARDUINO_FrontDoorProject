#include "FrontDoor.h"

#if YG_SIMULATOR
#include <Simulator.h>

#include "Clock.h"

#include "DoorManager.h"
#include "DoorOpen.h"

static void onSetup()
{
	SetCurrentTime(DaTi(2019, Nov, 1, 15, 57, 58));
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
			YATF_CHECK_STATE_EQUAL(state.u.s.night, false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);

			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state, DoorState::LED_OFF);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_OFF);

			return Simulator::ResultNextLoop();
		}

		case 1:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night,  false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);
	
			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state,	 DoorState::LED_OFF);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_OFF);

			return Simulator::ResultTimerMillis(2500);
		}

		case 2:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night, false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);

			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

//			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.time_to_switch, true);
			
//			door_state.inputs.s.time_to_switch = false;
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.n, 0);
			
//			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state,	  DoorState::LED_TOGGLE_1_OF_15);
//			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state,  DoorState::BUZZER_TOGGLE_1_OF_15);

			// 2 seconds before Hadlaka
			SetCurrentTime(DaTi(2019, Nov, 1, 16, 12, 58));
			return Simulator::ResultNextLoop();
		}

		case 3:
		{
			YATF_CHECK_STATE_EQUAL(state.u.s.night, false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, false);

			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			DoorState door_state = GetDoorState();

//			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.time_to_switch, true);

//			door_state.inputs.s.time_to_switch = false;
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.n, 0);

//			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state,	 DoorState::LED_TOGGLE_1_OF_15);
//			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_TOGGLE_1_OF_15);

			return Simulator::ResultTimerMillis(2500);
		}

		case 4:
		{
			// is after hadlaka
			YATF_CHECK_STATE_EQUAL(state.u.s.night, false);
			YATF_CHECK_STATE_EQUAL(state.u.s.kodesh, true);

			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), false);

			DoorState door_state = GetDoorState();

//			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.time_to_switch, false);

			door_state.inputs.s.kodesh = false;
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.n, 0);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state,	  DoorState::LED_OFF);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_OFF);

			return Simulator::ResultEnd();
		}
	}
	
	return Simulator::ResultEnd();
}
//------------------------------------------------
IMPLEMENT_SIMULATOR_SCENARIO(DM_StartCholDoorClosed);

#endif //YG_SIMULATOR
