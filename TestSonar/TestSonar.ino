/*
 Name:		TestSonar.ino
 Created:	12/9/2019 10:34:41 AM
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

#define trigPin A2 //12 // define TrigPin
#define echoPin A3 //11 // define EchoPin.
#define MAX_DISTANCE 200 // Maximum sensor distance is rated at 400-500cm.
// define the timeOut according to the maximum range. timeOut= 2*MAX_DISTANCE /100 /340 *1000000 = MAX_DISTANCE*58.8
float timeOut = MAX_DISTANCE * 60;
int soundVelocity = 340; // define sound speed=340m/s

void setup() {
	pinMode(PIN_SONAR_TRIG, OUTPUT);// set trigPin to output mode
	pinMode(PIN_SONAR_ECHO, INPUT); // set echoPin to input mode
	pinMode(PIN_LED_DISTANCE_CHANGED, OUTPUT);
	Serial.begin(9600);    // Open serial monitor at 9600 baud to see ping results.
}

float prev_cm;

void loop() {
	delay(1000); // Wait 1000ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
	Serial.print("Ping: ");
	float cm;

	do
	{
		cm = getSonar();
	} while (cm <= 0);

	float diff = cm - prev_cm;
	if (diff < 0)
		diff = -diff;

	digitalWrite(PIN_LED_DISTANCE_CHANGED, diff >= 10 ? HIGH : LOW);
	prev_cm = cm;

	Serial.print(cm); // Send ping, get distance in cm and print result (0 = outside set distance range)
	Serial.println("cm");
}

float getSonar() {
	unsigned long pingTime;
	float distance;
	digitalWrite(PIN_SONAR_TRIG, HIGH); // make trigPin output high level lasting for 10μs to triger HC_SR04,
	delayMicroseconds(10);
	digitalWrite(PIN_SONAR_TRIG, LOW);
	pingTime = pulseIn(PIN_SONAR_ECHO, HIGH, timeOut); // Wait HC-SR04 returning to the high level and measure out this waitting time
	distance = (float)pingTime * soundVelocity / 2 / 10000; // calculate the distance according to the time
	return distance; // return the distance value
}