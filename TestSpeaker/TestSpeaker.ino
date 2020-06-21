/*
 Name:		TestSpeaker.ino
 Created:	12/9/2019 9:30:36 AM
 Author:	MCP
*/

// the setup function runs once when you press reset or power the board

int buzzerPin = 4;    // the number of the buzzer pin
float sinVal;         // Define a variable to save sine value
int toneVal;          // Define a variable to save sound frequency

void setup() {
	pinMode(buzzerPin, OUTPUT); // Set Buzzer pin to output mode
}

void loop() {
#if 1
	tone(buzzerPin, 3136 / 2);
	delay(500);
	tone(buzzerPin, 2637 / 2);
	delay(1000);
	noTone(buzzerPin);
	delay(1000);
#else
	for (int x = 0; x < 360; x++) {       // X from 0 degree->360 degree
		sinVal = sin(x * (PI / 180));       // Calculate the sine of x
		toneVal = 2000 + sinVal * 500;      // Calculate sound frequency according to the sine of x
		tone(buzzerPin, toneVal);           // Output sound frequency to buzzerPin
		delay(1);
#endif
}