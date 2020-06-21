/*
 Name:		TestMotion.ino
 Created:	12/9/2019 9:46:17 AM
 Author:	MCP
*/

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


void setup() 
{
	Serial.begin(115200);
	pinMode(PIN_MOTION_DETECTOR, INPUT);  // initialize the sensor pin as input   
	pinMode(PIN_LED_MOTION_DETECTED, OUTPUT);    // initialize the LED pin as output 
	pinMode(PIN_SENSORS_SWITCH_TRANSISTOR, OUTPUT);    // initialize the LED pin as output 

	digitalWrite(PIN_SENSORS_SWITCH_TRANSISTOR, HIGH);
}

int counter = 0;
bool prev_motion = false;

void loop() 
{    
	// Turn on or off LED according to Infrared Motion Sensor   
	bool motion = HIGH == digitalRead(PIN_MOTION_DETECTOR);

	if (motion)
	{
		if (!prev_motion)
			counter = 0;
		else
			counter++;
	}
	else
	{
		if (prev_motion)
			Serial.println("---------------------");
		counter = 0;
	}

	prev_motion = motion;

	if (counter)
	{
		Serial.print("motion=");

		Serial.println(counter);
	}

	digitalWrite(PIN_LED_MOTION_DETECTED, motion);
	delay(1000);              // wait for a second 
} 