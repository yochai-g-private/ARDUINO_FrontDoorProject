#include "DoorManager.h"

#include "FrontDoor.h"

#include "ShabatSwitch.h"
#include "DoorOpen.h"
#include <Led.h>
#include <Buzzer.h>
#include <PushButton.h>

#if COMPILE_CLOCK
#include "Clock.h"
#endif //COMPILE_CLOCK


static Relay			shabat_relay(PIN_ALARM_RELAY, false, false);
static Led				led(PIN_DOOR_LED);
static ActiveBuzzer		buzzer(PIN_DOOR_BUZZER);
static PushButton		button(PIN_DOOR_BUTTON);
static Timer			buzzer_timer;
static bool				buzzer_because_shabat_switch = false;

class DoorButtonObserver : public StableDigitalInputComponentObserver
{
public:
	DoorButtonObserver() : StableDigitalInputComponentObserver(button,
#if YG_USE_SERIAL
															   "DoorButton",
#endif //YG_USE_SERIAL
															   0), pressed(false)	{		}

	bool Get()
	{
		if (pressed)
		{
			pressed = false;
			return true;
		}

		return false;
	}
private:

	void set(bool on)
	{
		if (on)
		{
			TRACER << "DoorButtonObserver: " << on << NL;
			pressed = true;
		}
	}

	bool pressed;
};

static DoorButtonObserver button_observer;
//--------------------------------------------------------------------------
static DaTi get_before()
{
	//TRACE_FUNC_IN();
	#define BEEP_BEFORE_HADLAKA_MINUTES		15
	return GetNextHadlaka().AddMinutes(-BEEP_BEFORE_HADLAKA_MINUTES);
}
//--------------------------------------------------------------------------
static DaTi get_after()
{
	//TRACE_FUNC_IN();
	#define BEEP_AFTER_MOTZEY_KODESH		DaTi::MINUTES_PER_HOUR
	return GetNextMotzeyKodesh().AddMinutes(1);
}
//--------------------------------------------------------------------------
static void stop_buzzer_because_shabat_switch(Timer& timer, void* context)
{
	//TRACE_FUNC_IN();
	buzzer.Off();

	if (!state.shabat_switch)
		led.Off();
}
//--------------------------------------------------------------------------
static void start_buzzer_because_shabat_switch(uint32_t minutes)
{
	//TRACE_FUNC_IN();
	uint32_t cycle		  = 10000,
			 interval_on  = 1000,
		     interval_off = cycle - interval_on;

	buzzer_because_shabat_switch = true;
	buzzer.Toggle2(interval_on, interval_off);
	led.Toggle(500);

	buzzer_timer.Stop();
	buzzer_timer.StartMilli(minutes * DaTi::SECONDS_PER_MINUTE * 1000,
							stop_buzzer_because_shabat_switch,
							NULL,
							1);
}
//--------------------------------------------------------------------------
static void beep_after(const DaTi& dt, void* ctx);
static void beep_before(const DaTi& dt, void* ctx)
{
	//TRACE_FUNC_IN();
	SheduleAction(get_after(), beep_after, NULL);
	
	if (!state.shabat_switch)
		start_buzzer_because_shabat_switch(BEEP_BEFORE_HADLAKA_MINUTES);
}
//--------------------------------------------------------------------------
static void beep_after(const DaTi& dt, void* ctx)
{
	//TRACE_FUNC_IN();
	SheduleAction(get_before(), beep_before, NULL);

	if (state.shabat_switch)
		start_buzzer_because_shabat_switch(BEEP_AFTER_MOTZEY_KODESH);
}
//--------------------------------------------------------------------------
void CancelBuzzerBecauseShabatSwitch()
{
	if (!buzzer_because_shabat_switch)
		return;

	//TRACE_FUNC_IN();

	buzzer_because_shabat_switch = false;
	buzzer.Off();
	buzzer_timer.Stop();
}
//--------------------------------------------------------------------------
bool InitializeDoorManager()
{
#if COMPILE_DOOR_OPEN
	LOGGER << F("Initializing DoorOpen...") << NL;
	if (!InitializeDoorOpen())
		return false;
#endif //COMPILE_DOOR_OPEN

#if COMPILE_SHABAT_SWITCH
	LOGGER << F("Initializing ShabatSwitch...") << NL;
	if (!InitializeShabatSwitch())
		return false;
#endif //COMPILE_SHABAT_SWITCH

#if COMPILE_CLOCK

	//IRTC& rtc = GetRTC();
	//DaTi test(19, Oct, 4, 16, 27, 0);
	//rtc.Set(test, false);
	//rtc.Get().Show();
	//LOGGER << "Week day=" << test.GetWeekDayName(false) << NL;

	DaTi before	= get_before();
	DaTi after  = get_after();

//	DaTi test2(19, Oct, 5, 18, 00, 0);
//	rtc.Set(test2, false);

	LOGGER << F("Next HADLAKA: "); before.Show();
	LOGGER << F("Next MOTZEY : "); after.Show();

	if (before < after)
	{
		SheduleAction(before, beep_before, NULL);
	}
	else
	{
		SheduleAction(after, beep_after, NULL);
	}
#endif //COMPILE_CLOCK

	button.Register(button_observer);
	buzzer.Toggle(1000, true, 1);
	led.Toggle(1000, true, 1);
	delay(1000);

	return true;
}
//--------------------------------------------------------------------------
static void start_buzzer_after_delay(Timer& timer, void* context)
{
	//TRACE_FUNC_IN();
	if(!buzzer_because_shabat_switch)
		buzzer.Toggle2(200, 4800);
}
//--------------------------------------------------------------------------
static void stop_buzzer()
{
	//TRACE_FUNC_IN();
	if (buzzer_because_shabat_switch)
		return;

	buzzer.Off();
	buzzer_timer.Stop();
	led.Off();
}
//--------------------------------------------------------------------------
bool CheckDoorStatus()
{
	bool changed = false;

	if (state.first_time)
	{
		UpdateShabatSwitchState();
		UpdateDoorOpenState();
	}

	if (state.first_time || state.shabat_switch != prev_state.shabat_switch)
	{
		TRACER << F("SHABAT RELAY: ") << (state.shabat_switch ? "Shabat" : "Chol") << NL;
		shabat_relay.Set(!state.shabat_switch);
		changed = true;
	}

	if (state.first_time || state.u.s.door_open != prev_state.u.s.door_open)
	{
		TRACER << F("DOOR ") << (state.u.s.door_open ? "open" : "closed") << NL;
		changed = true;
	}

	if (!state.shabat_switch && button_observer.Get())
	{
		stop_buzzer();
	}

	if (!changed)
		return false;

	TRACER << F("DOOR state changed") << NL;

	stop_buzzer();

	if (state.shabat_switch && state.u.s.door_open)
	{
		LOGGER << F("Something gone wrong!!! Door cannot be open while Shabat Switch is ON") << NL;

		if (!buzzer_because_shabat_switch)
		{
			buzzer.Toggle(250);
			led.Toggle(250);
		}

		return changed;
	}

	if (state.shabat_switch)
	{
		led.Off();
		led.Toggle2(250, 10000 - 250);
	}
	else
	{
		if (state.u.s.door_open)
		{
#define DELAY_BUZZER_SECONDS	60

			if (!buzzer_because_shabat_switch)
			{
				led.Toggle(1000);

				buzzer_timer.StartMilli(DELAY_BUZZER_SECONDS * 1000,
										start_buzzer_after_delay,
										NULL,
										1);
			}
		}
	}

	return changed;
}
