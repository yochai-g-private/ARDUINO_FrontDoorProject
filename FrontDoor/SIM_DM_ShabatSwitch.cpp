#include "FrontDoor.h"

#if YG_SIMULATOR
#include <Simulator.h>

#include "Clock.h"

#include "DoorManager.h"
#include "DoorOpen.h"
#include "ShabatSwitch.h"

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
			YATF_CHECK_STATE_EQUAL(state.shabat_switch, false);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.alarm_disabled_switch, false);
			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			return Simulator::ResultTimerMillis(200);
		}

		case 2:
		{
			SetShabatSwitch(true);
			return Simulator::ResultTimerMillis(200);
		}

		case 3:
		{
			YATF_CHECK_STATE_EQUAL(state.shabat_switch, true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.alarm_disabled_switch, true);
			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), false);

			return Simulator::ResultTimerMillis(200);
		}

		case 4:
		{
			SetShabatSwitch(false);
			return Simulator::ResultTimerMillis(200);
		}

		case 5:
		{
			YATF_CHECK_STATE_EQUAL(state.shabat_switch, false);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.alarm_disabled_switch, false);
			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), true);

			return Simulator::ResultTimerMillis(200);
		}

		case 6:
		{
			SetShabatSwitch(true);
			return Simulator::ResultTimerMillis(200);
		}

		case 7:
		{
			YATF_CHECK_STATE_EQUAL(state.shabat_switch, true);

			YATF_CHECK_DOOR_STATE_EQUAL(door_state.inputs.s.alarm_disabled_switch, true);
			YATF_CHECK_DOOR_STATE_EQUAL(GetDoorInternalsBooleanState(DOOR_SHABAT_RELAY_IS_ON), false);

			return Simulator::ResultEnd();
		}

	}
	
	return Simulator::ResultEnd();
}
//------------------------------------------------
IMPLEMENT_SIMULATOR_SCENARIO(DM_ShabatSwitch);

#endif //YG_SIMULATOR
