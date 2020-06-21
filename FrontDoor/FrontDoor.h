#ifndef __FrontDoor__
#define __FrontDoor__

//--------------------------------------------------------------------------
// INCLUDES
//--------------------------------------------------------------------------

#include <YG.h>
#include <MicroController.h>
#include <PreciseRTC.h>
#include <Led.h>
#include <Tester.h>

#ifndef PROJECT_FrontDoor
#	error Invalid AppDefines.h !!!
#endif //PROJECT_FrontDoor

using namespace YG;

//--------------------------------------------------------------------------
// PIN NUMBERS
//--------------------------------------------------------------------------

enum Pins
{
	//-----------------------------------
	PIN_SENSORS_SWITCH_TRANSISTOR	=  2,
	PIN_SELF_TEST_BUTTON			=  3,
	PIN_PASSIVE_BUZZER				=  4,
	PIN_LIGHT_RELAY					=  5,
	PIN_CURRENT_SWITCH_RELAY		=  6,
	//-----------------------------------
	PIN_DOOR_LED					=  7,
	PIN_ALARM_RELAY					=  8,
	PIN_DOOR_BUTTON					=  9,
	PIN_SHABAT_SWITCH				= 10,
	PIN_DOOR_BUZZER					= 11,
	PIN_DOOR_OPEN					= 12,
	//-----------------------------------
	PIN_SDA							= 20,
	PIN_SCL							= 21,
	//-----------------------------------
	PIN_LED_DISTANCE_CHANGED		= 22,
	PIN_LED_MOTION_DETECTED			= 23,
	PIN_SONAR_TRIG					= 24,
	PIN_SONAR_ECHO					= 25,
	PIN_MOTION_DETECTOR				= 26,
	PIN_IR_RECEIVER					= 40,
	//-----------------------------------
};

#define USE_RGB_CATHODE_LED		false

union DigitalInputs
{
	struct 
	{
		bool	distance_changed			: 1;	// 0x01
		bool	motion						: 1;	// 0x02
		bool	door_open					: 1;	// 0x04
		bool	night						: 1;	// 0x08
		bool	kodesh						: 1;	// 0x10
		int		reserved_for_future_use		: 3;
	}		s;

	uint8_t n;
};


struct State
{
	bool			first_time;
	DigitalInputs	u;
	bool			shabat_switch;
	float			distance;
};

extern State state;
extern State prev_state;

void ShowState();
void SetSensors();

#define YATF_CHECK_STATE_EQUAL(a,b) YATF_CHECK_EQUAL(a,b,ShowState())

typedef void(*Action)();

#endif //__FrontDoor__
