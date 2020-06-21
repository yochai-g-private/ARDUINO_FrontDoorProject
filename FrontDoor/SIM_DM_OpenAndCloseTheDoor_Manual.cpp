#include "FrontDoor.h"

#if YG_SIMULATOR
#include <Simulator.h>

#include "Clock.h"

#include "DoorManager.h"
#include "DoorOpen.h"

static void onSetup()
{
	SetCurrentTime(DaTi(2019, Nov, 1, 14, 0, 0));
	SetDoorOpen(false);
}
//------------------------------------------------
static Simulator::StepResult onLoop(int case_id)
{
	ShowCurrentTime();
	DoorState door_state = GetDoorState();
	ShowDoorState(door_state);

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
			return Simulator::ResultWaitForSerialInput();
		}

		case 2:
		{
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.door_open, true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.led_state,	 DoorState::LED_TOGGLE);
			YATF_CHECK_DOOR_STATE_EQUAL(door_state.outputs.buzzer_state, DoorState::BUZZER_DELAYED);

			YATF_CHECK_EQUAL(DoorOpenActionIsScheduled(), true, nothing);

			return Simulator::ResultWaitForSerialInput();
		}

		case 3:
		{
			SetDoorOpen(false);
			return Simulator::ResultWaitForSerialInput();
		}

		case 4:
		{
			SetDoorOpen(true);
			return Simulator::ResultWaitForSerialInput();
		}

		case 5:
		{
			SetDoorOpen(false);
			return Simulator::ResultWaitForSerialInput();
		}
	}
	
	return Simulator::ResultEnd();
}
//------------------------------------------------
IMPLEMENT_SIMULATOR_SCENARIO(DM_OpenAndCloseTheDoor_Manual);

#endif //YG_SIMULATOR
