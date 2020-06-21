/*
 Name:		FrontDoor.ino
 Created:	8/4/2019 11:10:48 PM
 Author:	Yochai.Glauber
*/

#include "FrontDoor.h"
#include "SelfTest.h"
#include "Clock.h"
#include "Sonar.h"
#include "Transistor.h"
#include "Motion.h"
#include "Bell.h"
#include "DoorManager.h"
#include "Timer.h"
#include "Kodesh.h"

#if COMPILE_MICROSD
#include <MicroSD.h>
#endif // COMPILE_MICROSD

extern State	state		= { 0 };
extern State	prev_state	= { 0 };

#if _USE_INITIALIZATION_DELAY_TIMER
static Timer			initialization_delay_timer;
#endif //_USE_INITIALIZATION_DELAY_TIMER

#if COMPILE_SENSORS
static NPN_Transistor	sensors_switch("SENSORS_SWITCH", PIN_SENSORS_SWITCH_TRANSISTOR, false);
#endif //COMPILE_SENSORS
static Relay			light_relay("LIGHT_RELAY",				PIN_LIGHT_RELAY,			false, false);
static Relay			other_relay("PIN_CURRENT_SWITCH_RELAY", PIN_CURRENT_SWITCH_RELAY,	true,  false);

#define INITIALIZATION_DELAY_SECONDS	10

//--------------------------------------------------------------------------
// IMPLEMENTATION
//--------------------------------------------------------------------------

static void Alarm();
static void SetAlarmTimer(bool on);

static void SensorsOnOff(bool on);
static void SensorsOn();
static void SensorsOff();
static void LightOnOff(bool on);
static void LightOn();
static void LightOff();
static void LightOnWithScheduler();
static void DoNotPlay();
static void Alarm();
static void WhoIsThere();
static void RingBell();

#if YG_SIMULATOR
#include <Simulator.h>
const Simulator::Scenarios& SIM_GetScenarios();
#endif //YG_SIMULATOR

//--------------------------------------------------------------------------
// APPLICATION
//--------------------------------------------------------------------------

class Application : public IApplication
{
public:

	const char*   GetApplicationName()  const { return "FrontDoor"; }
	Version       GetApplicationVersion() const { return 1; }

	void OnSetup()
	{
#if COMPILE_MICROSD
		if (MicroSD::Begin())
		{
			const char* log_fn = 
#if YG_SIMULATOR	
				"SIMULAT.log";
#else
				"FronDoor.log";
#endif //YG_SIMULATOR	
			OpenLogFile(log_fn);
		}
		else
		{
			_LOG("Failed to initialize MicroSD" << NL);
		}
#endif // COMPILE_MICROSD

#define CHECK_STATE_BITS	0
#if CHECK_STATE_BITS
		#define CHECK_BIT(id, expected)	\
			memset(&state, 0, sizeof(state));\
			state.u.s.id = true;\
			if (state.u.n != expected) { _LOG(F(#id "=") << state.u.n << NL); ThrowException(-778, "bad", nothing); } else
			
		CHECK_BIT(distance_changed, 0x01);
		CHECK_BIT(motion,			0x02);
		CHECK_BIT(door_open,		0x04);
		CHECK_BIT(night,			0x08);
		CHECK_BIT(kodesh,			0x10);

		if(MicroController::FAILED())
			return;
#endif //CHECK_STATE_BITS

		memset(&state, 0, sizeof(state));
		memset(&prev_state, 0, sizeof(prev_state));

		state.first_time = true;

#if YG_USE_KODESH_WATCH
		_LOG("Initializing Clock..." << NL);
		if (!InitializeClock())
			return;
#endif // YG_USE_KODESH_WATCH

#if COMPILE_SONAR
		_LOG("Initializing Sonar..." << NL);
		if (!InitializeSonar())
			return;
#endif //COMPILE_SONAR

#if COMPILE_MOTION
		_LOG("Initializing Motion..." << NL);
		if (!InitializeMotion())
			return;
#endif //COMPILE_MOTION

		_LOG("Initializing DoorManager..." << NL);
		if (!InitializeDoorManager())
			return;

#if COMPILE_SELF_TEST
		_LOG("Initializing SelfTest..." << NL);
		if (!InitializeSelfTest())
			return;
#endif //COMPILE_SELF_TEST

#if COMPILE_SONAR
		GetSonarLed().Toggle(500, true, INITIALIZATION_DELAY_SECONDS);
#endif //COMPILE_SONAR

#if COMPILE_MOTION
		GetMotionLed().Toggle(500, true, INITIALIZATION_DELAY_SECONDS);
#endif //COMPILE_MOTION

#if _USE_INITIALIZATION_DELAY_TIMER
		initialization_delay_timer.StartMilli(INITIALIZATION_DELAY_SECONDS * 1000,
											  initalization_delay_done,
											  NULL,
											  1);
#endif //_USE_INITIALIZATION_DELAY_TIMER
	}

	void OnLoop(bool first_loop)
	{
#if _USE_INITIALIZATION_DELAY_TIMER
		if (initialization_delay_timer.IsStarted())
			return;
#endif //_USE_INITIALIZATION_DELAY_TIMER

		CheckDoorStatus(first_loop);
		bool state_changed = false;

		if (state.first_time || prev_state.u.n != state.u.n)
		{
			OnChange();
			state_changed = true;
		}

		if (state_changed)
		{
			state.first_time = false;
			ShowState();
			memcpy(&prev_state, &state, sizeof(state));
		}
	}

	void OnError()
	{
	}

	void OnChange();

#if _USE_INITIALIZATION_DELAY_TIMER
	static void initalization_delay_done(Timer& timer, void* context)
	{
#if COMPILE_SONAR
		GetSonarLed().Off();
#endif //COMPILE_SONAR

#if COMPILE_MOTION
		GetMotionLed().Off();
#endif //COMPILE_MOTION

		// Business logic >>>
		if (state.u.s.kodesh)
		{
			SensorsOff();
			LightOnOffWithScheduler();
		}
		else
		{
			SensorsOn();
			LightOnOff(state.u.s.night && state.u.s.motion);
		}
	}
#endif //_USE_INITIALIZATION_DELAY_TIMER

#if YG_USE_LOGGER && !YG_SIMULATOR
	void OnSerialAvailable()
	{
#if YG_USE_KODESH_WATCH
		LOGGER << F("Updating RTC from serial...") << NL;
		UpdateClockFromSerial();
#endif //YG_USE_KODESH_WATCH
	}
#endif //YG_USE_LOGGER && !YG_SIMULATOR

#if YG_SIMULATOR
	const Simulator::Scenarios& GetScenarios()	const
	{
		return SIM_GetScenarios();
	}
#endif //YG_SIMULATOR

} application;
//--------------------------------------------------------------------------
IApplication& YG::GetApplication() { return application; }
//--------------------------------------------------------------------------
struct StateAction
{
	Action	sensor_action,
			light_action,
			bell_action;
};
//--------------------------------------------------------------------------
void DoStateAction(const StateAction& state_action)
{
	state_action.sensor_action();
	state_action.light_action();
	state_action.bell_action();
}
//--------------------------------------------------------------------------
static void StartAlarm(Timer& timer, void* context)
{
	Alarm();
}
//--------------------------------------------------------------------------
static void SetAlarmTimer(bool on)
{
	static Timer timer;

	if (on)
	{
		timer.RestartMilli(60 * 1000, StartAlarm, NULL, 1);
	}
	else
	{
		timer.Stop();
	}
}
//--------------------------------------------------------------------------
void ShowState()
{
#define SHOW_STATE	1
#if SHOW_STATE
	#define _SHOW_STATE_FLAG(id, fld)		_LOG(F("  " #id ": ") << fld << NL)
	#define SHOW_STATE_FLAG(id)				_SHOW_STATE_FLAG(id, state.u.s.id)

	_LOG(F("-------- State (") << RXBIN << state.u.n << F(") >>>>>>>>>>") << NL);
	ShowCurrentTime();

	SHOW_STATE_FLAG(kodesh);
	SHOW_STATE_FLAG(night);
	SHOW_STATE_FLAG(door_open);
	SHOW_STATE_FLAG(motion);
	SHOW_STATE_FLAG(distance_changed);

	_SHOW_STATE_FLAG(distance,		state.distance);
	_SHOW_STATE_FLAG(shabat_switch, state.shabat_switch);

	_LOG(F("<<<<<<<< State ----------") << NL);
#endif
}
//--------------------------------------------------------------------------
void Application::OnChange()
{
	static const StateAction chol[16]  = 
	{ 
		// DAY
		{ SensorsOn,		LightOff,		DoNotPlay	},	// 0: אין פעילות
		{ SensorsOn,		LightOff,		Alarm		},	// 1: חשוד, בא מן הצד
		{ SensorsOn,		LightOff,		WhoIsThere	},	// 2: מישהו בא. אחרי דקה שלא עושה כלום הוא הופך להיות חשוד
		{ SensorsOn,		LightOff,		RingBell	},	// 3: כאילו נגע בפעמון
		{ SensorsOn,		LightOff,		DoNotPlay	},	// 4: הדלת פתוחה
		{ SensorsOn,		LightOff,		DoNotPlay	},	// 5: הדלת פתוחה
		{ SensorsOn,		LightOff,		DoNotPlay	},	// 6: הדלת פתוחה
		{ SensorsOn,		LightOff,		DoNotPlay	},	// 7: הדלת פתוחה
		// NIGHT
		{ SensorsOn,		LightOff,		DoNotPlay	},	// 8: אין פעילות
		{ SensorsOn,		LightOn,		Alarm		},	// 9: חשוד, בא מן הצד
		{ SensorsOn,		LightOn,		WhoIsThere	},	// A: מישהו בא. אחרי דקה שלא עושה כלום הוא הופך להיות חשוד
		{ SensorsOn,		LightOn,		RingBell	},	// B: כאילו נגע בפעמון
		{ SensorsOn,		LightOn,		DoNotPlay	},	// C: הדלת פתוחה
		{ SensorsOn,		LightOn,		DoNotPlay	},	// D: הדלת פתוחה
		{ SensorsOn,		LightOn,		DoNotPlay	},	// E: הדלת פתוחה
		{ SensorsOn,		LightOn,		DoNotPlay	},	// F: הדלת פתוחה
	};

	static const StateAction kodesh[2] = 
	{ 
		{ SensorsOff,		LightOff,				DoNotPlay	},	// 16..23: אין פעילות
		{ SensorsOff,		LightOnWithScheduler,	DoNotPlay	},	// 24..31: אין פעילות
	};

	SetAlarmTimer(false);

	if (state.u.n < 16)
	{
		DoStateAction(chol[state.u.n]);
	}
	else
	{
		int index = (state.u.n >= 24);
		DoStateAction(kodesh[index]);
	}
}
//--------------------------------------------------------------------------
static void DigitalOutputComponentOnOff(DigitalOutputComponent& c, const __FlashStringHelper* name, bool on)
{
	if (c.IsOn() == on)
		return;

	_LOG(name << F(" set ") << ((on) ? F("ON") : F("OFF")) << NL);
	c.Set(on);
}
//--------------------------------------------------------------------------
static void SensorsOnOff(bool on)
{
#if COMPILE_SENSORS
	DigitalOutputComponentOnOff(sensors_switch, F("Sensors"), on);
#endif //COMPILE_SENSORS
}
//--------------------------------------------------------------------------
static void SensorsOn()
{ 
	SensorsOnOff(true);
}
//--------------------------------------------------------------------------
static void SensorsOff() 
{ 
	SensorsOnOff(false);
}
//--------------------------------------------------------------------------
static void LightOnOff(bool on)
{
	DigitalOutputComponentOnOff(light_relay, F("Light"), on);
}
//--------------------------------------------------------------------------
static void LightOn() 
{ 
	LightOnOff(true);
}
//--------------------------------------------------------------------------
static void LightOffOnScheduler(const DaTi& dt, void* ctx)
{
	LightOff();
}
//--------------------------------------------------------------------------
static void LightOnWithScheduler()
{
	LightOn();

	DaTi dt = RTC.Get();
	if (dt.Hour() < 12)
		return;

	dt = dt.GetNextDay();

	RTC.ScheduleAction(dt, LightOffOnScheduler, NULL, "Light off");
}
//--------------------------------------------------------------------------
static void LightOnOffWithScheduler()
{
	if (state.u.s.night)
		LightOnWithScheduler();
	else
		LightOff();
}

//--------------------------------------------------------------------------
static void LightOff()
{ 
	LightOnOff(false);
}
//--------------------------------------------------------------------------
static void DoNotPlay() 
{ 
#if COMPILE_BELL
	StopPlaying();
#endif //COMPILE_BELL
}
//--------------------------------------------------------------------------
static void Alarm() 
{ 
#if COMPILE_BELL
	Play(MELODY_Beethoven5, 10);
#endif //COMPILE_BELL
}
//--------------------------------------------------------------------------
static void WhoIsThere() 
{ 
	SetAlarmTimer(true); 
}
//--------------------------------------------------------------------------
static void RingBell() 
{ 
#if COMPILE_BELL
	Play(MELODY_DoorBell, 2);
#endif //COMPILE_BELL
}
//--------------------------------------------------------------------------
void SetSensors()
{
	bool sensors_on = !state.shabat_switch && !state.u.s.kodesh;
#if COMPILE_SENSORS
	sensors_switch.Set(sensors_on);
#endif //COMPILE_SENSORS
}
//--------------------------------------------------------------------------
