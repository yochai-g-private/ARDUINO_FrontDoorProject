#include "DoorManager.h"

#include "FrontDoor.h"

#include "ShabatSwitch.h"
#include "DoorOpen.h"
#include <Led.h>
#include <Buzzer.h>
#include <PushButton.h>


#if YG_USE_KODESH_WATCH
#include "Clock.h"
#endif //YG_USE_KODESH_WATCH

typedef DoorState::Inputs	DoorManagerInputs;
typedef DoorState::Outputs	DoorManagerOutputs;

static Relay				shabat_relay("SHABAT_RELAY", PIN_ALARM_RELAY, false, false);
static Led					led("DOOR_LED", PIN_DOOR_LED);
static ActiveBuzzer			buzzer("DOOR_BUZZER", PIN_DOOR_BUZZER);
static PushButton			button("DOOR_QUIET", PIN_DOOR_BUTTON);
static Timer				buzzer_timer;
//static bool					time_to_switch = false;

static DoorState			st_door_state;
static DoorState::Inputs&	prev_inputs  = st_door_state.inputs;
static DoorState::Outputs&	prev_outputs = st_door_state.outputs;

static uint64_t				door_open_scheduled_action_handler = 0;
//--------------------------------------------------------------------------
static void update_buzzer(DoorState::BuzzerState buzzer_state);
static void update_led(DoorState::LedState led_state);
static bool check_door_status(bool first_time);
//--------------------------------------------------------------------------

class DoorButtonObserver : public StableDigitalInputComponentObserver
{
public:
	DoorButtonObserver() : StableDigitalInputComponentObserver(button,
#if YG_USE_LOGGER
															   "DoorButton",
#endif //YG_USE_LOGGER
															   0), pressed(false)	{		}
	bool Get()
	{
		return pressed;
	}

	void Reset()
	{
		pressed = false;
	}

private:

	void set(bool on)
	{
		if (on && (prev_outputs.buzzer_state == DoorState::BUZZER_TOGGLE_1_OF_60 ||
				   prev_outputs.buzzer_state == DoorState::BUZZER_DELAYED))
		{
			_TRC("DoorButtonObserver: " << on << NL);

			buzzer.Off();
			buzzer_timer.Stop();

			pressed = true;

			update_led(DoorState::LED_TOGGLE_FAST);
		}
	}

	bool pressed;
};

static DoorButtonObserver button_observer;
//--------------------------------------------------------------------------
#if YG_USE_KODESH_WATCH
static DaTi get_before()
{
	#define BEEP_BEFORE_HADLAKA_MINUTES		15
	return GetNextHadlaka().AddMinutes(-BEEP_BEFORE_HADLAKA_MINUTES);
}
//--------------------------------------------------------------------------
static DaTi get_after()
{
	#define BEEP_AFTER_MOTZEY_KODESH		DaTi::MINUTES_PER_HOUR
	return GetNextMotzeyKodesh();
}
//--------------------------------------------------------------------------
bool CancelDoorOpenSheduledAction()
{
	return RTC.CancelScheduledAction(door_open_scheduled_action_handler, "Delay before warn door open");
}
//--------------------------------------------------------------------------
bool DoorOpenActionIsScheduled()
{
	return RTC.ActionIsScheduled(door_open_scheduled_action_handler);
}
//--------------------------------------------------------------------------
static void schedule_time_to_switch(const DaTi& dt, Action action, const char* description)
{
	RTC.ScheduleAction(dt, action, NULL, description);
	//time_to_switch = true;
	//SIM_LOG("DEBUG #6 time_to_switch=" << time_to_switch << NL);
}
//--------------------------------------------------------------------------
static void beep_after(const DaTi& dt, void* ctx);
static void beep_before(const DaTi& dt, void* ctx)
{
	schedule_time_to_switch(get_after(), beep_after, "BEEP after");
}
//--------------------------------------------------------------------------
static void beep_after(const DaTi& dt, void* ctx)
{
	schedule_time_to_switch(get_before(), beep_before, "BEEP before");
	//time_to_switch = st_door_state.inputs.s.alarm_disabled_switch;
	//SIM_LOG("DEBUG #1 time_to_switch=" << time_to_switch << NL);
}
#endif //YG_USE_KODESH_WATCH
//--------------------------------------------------------------------------
bool InitializeDoorManager()
{
#if COMPILE_DOOR_OPEN
	_LOG(F("Initializing DoorOpen...") << NL);
	if (!InitializeDoorOpen())
		return false;
#endif //COMPILE_DOOR_OPEN

#if COMPILE_SHABAT_SWITCH
	_LOG(F("Initializing ShabatSwitch...") << NL);
	if (!InitializeShabatSwitch())
		return false;
#endif //COMPILE_SHABAT_SWITCH

	button.Register(button_observer);
	buzzer.Toggle(1000, true, 1);
	led.Toggle(1000, true, 1);
	delay(1000);

#	define ALARM_SHOULD_BE_DISABLED	(!state.u.s.kodesh && !state.shabat_switch)
	shabat_relay.Set(ALARM_SHOULD_BE_DISABLED);

	prev_inputs.n = 0;

	return true;
}
//--------------------------------------------------------------------------
static void update_led(DoorState::LedState led_state)
{
	if (prev_outputs.led_state == led_state)
		return;

	prev_outputs.led_state = led_state;

	led.Off();

	switch(led_state)
	{
		case DoorState::LED_OFF	:
		{
			_LOG("LED_OFF" << NL);
			break;
		}

		case DoorState::LED_TOGGLE:
		{
			_LOG("LED_TOGGLE" << NL);
			led.Toggle(1000);
			break;
		}

		case DoorState::LED_TOGGLE_FAST:
		{
			_LOG("LED_TOGGLE_FAST" << NL);
			led.Toggle(500);
			break;
		}

		case DoorState::LED_TOGGLE_SLOW:
		{
			_LOG("LED_TOGGLE_SLOW" << NL);
			led.Toggle2(1000, 60000);
			break;
		}

		case DoorState::LED_TOGGLE_1_OF_15:
		{
			_LOG("LED_TOGGLE_1_OF_15" << NL);
			led.Toggle2(1000, 14000);
			break;
		}

		case DoorState::LED_TOGGLE_SOS:
		{
			_LOG("LED_TOGGLE_SOS" << NL);
			const uint32_t* intervals; uint8_t n_intervals;

			MicroController::GetSOS(intervals, n_intervals);

			led.ToggleEx(intervals, n_intervals);

			break;
		}
	}
}
//--------------------------------------------------------------------------
//static void start_buzzer_after_delay(Timer& timer, void* context)
static void start_buzzer_after_delay()
{
	_LOG("start_buzzer_after_delay" << NL);

	if (button_observer.Get())
	{
		if (prev_outputs.led_state == DoorState::LED_TOGGLE)
		{
			update_led(DoorState::LED_TOGGLE_FAST);
		}

		return;
	}

	door_open_scheduled_action_handler = 0;

	update_buzzer(DoorState::BUZZER_TOGGLE_1_OF_60);
}
//--------------------------------------------------------------------------
static void update_buzzer(DoorState::BuzzerState buzzer_state)
{
	if (state.u.s.kodesh)
	{
		// THIS NEVER SHOULD HAPPENED, but anyway...
		// ...make sure that it is kept quiet
		prev_outputs.buzzer_state = DoorState::BUZZER_OFF;
		buzzer.Off();
		buzzer_timer.Stop();
		button_observer.Reset();
		CancelDoorOpenSheduledAction();
		return;
	}

	if (prev_outputs.buzzer_state == buzzer_state)
		return;

	prev_outputs.buzzer_state = buzzer_state;

	buzzer.Off();

	switch (buzzer_state)
	{
		case DoorState::BUZZER_OFF:
		{
			_LOG("BUZZER_OFF" << NL);
			buzzer_timer.Stop();
			button_observer.Reset();
			break;
		}

		case DoorState::BUZZER_DELAYED:
		{
			_LOG("BUZZER_DELAYED" << NL);
			buzzer_timer.Stop();

			//uint32_t interval_milli_seconds = (uint32_t)60 * 1000 * DELAY_DOOR_OPEN_BUZZER_MINUTES;
			//_LOG("Buzzer delay: " << interval_milli_seconds << NL);
			//buzzer_timer.StartMilli(interval_milli_seconds,
			//						start_buzzer_after_delay,
			//						NULL,
			//						1);
			door_open_scheduled_action_handler = RTC.ScheduleAction(DELAY_DOOR_OPEN_BUZZER_MINUTES * DaTi::SECONDS_PER_MINUTE,
																	start_buzzer_after_delay,
																	NULL,
																	"Delay before warn door open");

			break;
		}

		case DoorState::BUZZER_TOGGLE_1_OF_60:
		{
			_LOG("BUZZER_TOGGLE_1_OF_60" << NL);
			buzzer.Toggle2(200, 59800);
			break;
		}

		case DoorState::BUZZER_TOGGLE_1_OF_15:
		{
			_LOG("BUZZER_TOGGLE_1_OF_15" << NL);
			buzzer_timer.Stop();

			buzzer.Toggle2(1000, 14000);
			break;
		}
	}
}
//--------------------------------------------------------------------------
static void AllOff()
{
	_LOG(__FUNCTION__ << NL);

	update_buzzer(DoorState::BUZZER_OFF);
	update_led   (DoorState::LED_OFF);
}
//--------------------------------------------------------------------------
static void OnDoorOpen()
{
	_LOG(__FUNCTION__ << NL);

	update_buzzer(DoorState::BUZZER_DELAYED);
	
	if (prev_outputs.led_state != DoorState::LED_TOGGLE &&
		prev_outputs.led_state != DoorState::LED_TOGGLE_FAST)
	{
		update_led(DoorState::LED_TOGGLE);
	}
}
//--------------------------------------------------------------------------
static void OnAlarmDisabled()
{
	//_LOG(__FUNCTION__ << NL);

	update_buzzer(DoorState::BUZZER_OFF);
	update_led   (DoorState::LED_TOGGLE_SLOW);
	shabat_relay.Off();
}
//--------------------------------------------------------------------------
static void DoorSensorBug()
{
	_LOG(__FUNCTION__ << NL);

	update_buzzer(DoorState::BUZZER_OFF);
	update_led   (DoorState::LED_TOGGLE_SOS);
}
//--------------------------------------------------------------------------
static void PleaseToggleAlarmDisabledSwitch()
{
	_LOG(__FUNCTION__ << NL);

	update_buzzer(DoorState::BUZZER_TOGGLE_1_OF_15);
	update_led   (DoorState::LED_TOGGLE_1_OF_15);
}
//--------------------------------------------------------------------------
static void process_changes(DoorManagerInputs& inputs)
{
	if (inputs.s.kodesh)
	{
		if (!prev_inputs.s.kodesh)
		{
			AllOff();
			shabat_relay.Off();
			CancelDoorOpenSheduledAction();
		}

		// clean all states
		bool hold_alarm_disabled_switch = inputs.s.alarm_disabled_switch;
		inputs.n = 0;
		// except kodesh
		inputs.s.kodesh = 1;
		inputs.s.alarm_disabled_switch = hold_alarm_disabled_switch;
		//time_to_switch  = false;

		return;
	}

	//TRACE_INFO("shabat_relay.Set(ALARM_SHOULD_BE_DISABLED)");
	//shabat_relay.Set(ALARM_SHOULD_BE_DISABLED);

	Action actions[8] =
	{
		AllOff,
		OnDoorOpen,
		DoorSensorBug,
		OnAlarmDisabled,
		PleaseToggleAlarmDisabledSwitch,
		PleaseToggleAlarmDisabledSwitch,
		DoorSensorBug,
		OnAlarmDisabled,
	};

	actions[inputs.n]();
}
//--------------------------------------------------------------------------
void ShowDoorState(const DoorState& state)
{
	#define SHOW_STATE	1
	#if SHOW_STATE
	#define _SHOW_STATE_FLAG(id, fld)		_LOG(F("  " #id ": ") << fld << NL)
	#define SHOW_INPUTS_STATE_FLAG(id)		_SHOW_STATE_FLAG(id, state.inputs.s.id)
	#define SHOW_OUTPUTS_STATE_FLAG(id)		_SHOW_STATE_FLAG(id, state.outputs.id)

	_LOG(F("-------- Door State (") << RXBIN << state.inputs.n << F(") >>>>>>>>>>") << NL);
	ShowCurrentTime();

	SHOW_INPUTS_STATE_FLAG(door_open);
	SHOW_INPUTS_STATE_FLAG(alarm_disabled_switch);
	//SHOW_INPUTS_STATE_FLAG(time_to_switch);
	SHOW_INPUTS_STATE_FLAG(kodesh);

	_LOG(F("  - - - - - - -") << NL);

	SHOW_OUTPUTS_STATE_FLAG(led_state);
	SHOW_OUTPUTS_STATE_FLAG(buzzer_state);
	_LOG(F("<<<<<<<< Door State ----------") << NL);
#endif
}
//--------------------------------------------------------------------------
bool GetDoorInternalsBooleanState(DoorInternalsId id)
{
	switch (id)
	{
	case DOOR_BUZZER_TIMER_IS_STARTED:
		return buzzer_timer.IsStarted();

	case DOOR_BUZZER_IS_ON:
		return buzzer.IsOn() || buzzer.IsToggling();

	case DOOR_LED_IS_ON:
		return led.IsOn()    || led.IsToggling();

	case DOOR_SHABAT_RELAY_IS_ON:
		return shabat_relay.IsOn();
	}
	
	_TRC(F("Unknown id") << NL);

	return false;
}
//--------------------------------------------------------------------------
DoorState GetDoorState()
{
	return st_door_state;
}
//--------------------------------------------------------------------------
bool CheckDoorStatus(bool first_loop)
{
	DoorState door_state;
	DoorManagerInputs&  inputs = door_state.inputs;
	DoorManagerOutputs& outputs = door_state.outputs;

	outputs = st_door_state.outputs;
	inputs.n = 0;

	inputs.s.door_open				= state.u.s.door_open;
	inputs.s.alarm_disabled_switch	= state.shabat_switch;
	inputs.s.kodesh					= state.u.s.kodesh;
	//inputs.s.time_to_switch = time_to_switch;

	//if (inputs.s.alarm_disabled_switch != prev_inputs.s.alarm_disabled_switch)
	//	inputs.s.time_to_switch = false;

	if (inputs.s.kodesh)
	{
		//inputs.s.time_to_switch = false;
		if (!inputs.s.alarm_disabled_switch)
		{
			shabat_relay.Off();
		}
	}

	//if (time_to_switch != inputs.s.time_to_switch)
	//{
	//	time_to_switch = inputs.s.time_to_switch;
	//	//SIM_LOG("DEBUG #2 time_to_switch=" << time_to_switch << NL);
	//}

	if (first_loop)
	{
		DaTi current = GetCurrentTime();
		DaTi before = get_before();
		DaTi after = get_after();

		_LOG(F("Next HADLAKA: ")); before.GetAddingMinutes(BEEP_BEFORE_HADLAKA_MINUTES).Show();
		_LOG(F("Next MOTZEY : ")); after.Show();

		if (before < after)
		{
			beep_after(after, NULL);
		}
		else
		{
			beep_before(before, NULL);
		}

//		time_to_switch = false;

		//if (!state.u.s.kodesh)
		//{
		//	if (before <= current && !st_door_state.inputs.s.alarm_disabled_switch)
		//	{
		//		//SIM_LOG("before <= current && !st_door_state.inputs.s.alarm_disabled_switch" << NL);
		//		if(!inputs.s.time_to_switch)
		//		{
		//			inputs.s.time_to_switch = true;
		//			//SIM_LOG("DEBUG #3 time_to_switch=" << time_to_switch << NL);
		//		}
		//	}
		//	else if (after <= current && st_door_state.inputs.s.alarm_disabled_switch)
		//	{
		//		inputs.s.time_to_switch = true;
		//		if (!inputs.s.time_to_switch)
		//		{
		//			inputs.s.time_to_switch = true;
		//			//SIM_LOG("DEBUG #4 time_to_switch=" << time_to_switch << NL);
		//		}
		//	}
		//}
	}

	//time_to_switch = false;


	//TRACE_INFO("shabat_relay.Set(ALARM_SHOULD_BE_DISABLED)");
	shabat_relay.Set(ALARM_SHOULD_BE_DISABLED);
	//if (time_to_switch != inputs.s.time_to_switch)
	//{
	//	time_to_switch = inputs.s.time_to_switch;
	//	//SIM_LOG("DEBUG #7 time_to_switch=" << time_to_switch << NL);
	//}

	bool changed = false;

	if (first_loop || prev_inputs.n != inputs.n)
	{
		changed = true;

		ShowDoorState(door_state);

		process_changes(inputs);

		prev_inputs.n = inputs.n;

		if (memcmp(&outputs, &st_door_state.outputs, sizeof(st_door_state.outputs)))
			ShowDoorState(st_door_state);
	}

	return changed;
}
//--------------------------------------------------------------------------
