#include "Tasker.h"

//**********************************
// Firmware version
const String VERSION = "0.8.0";
//**********************************

int pinDown = 0;	// Shows which pin is currently connected to ground - key pressed
bool lockDown = false;

int keyPinA1 = 3;
int keyPinA2 = 4;
int keyPinA3 = 5;

int keyPinB1 = 6;
int keyPinB2 = 7;
int keyPinB3 = 8;

int keyPinC1 = 15;	// A1
int keyPinC2 = 16;	// A2
int keyPinC3 = 17;	// A3

// Toggle
int toggleLEDPin = 13;
int togglePrincipalPin = 12;

// Piezo buzzer
int buzzerPin = A4;

// Lock variables
boolean locked = false;
int lockPin = 14;	// A0
int lockLEDPin = 19;	// A5

// RGB LED variables
int rgbKeyPin = 2;
int ledRedPin = 11;
int ledGreenPin = 10;
int ledBluePin = 9;
int RED = 255, GREEN = 255, BLUE = 255;
String ledColorMessage;

char colorName;

Tasker tasker(true);

// Play tones
int tempo = 200;
// Play tones

void setup()
{
	Serial.begin(115200);
	Serial.setTimeout(50);

	// Keys
	pinMode(keyPinA1, INPUT_PULLUP);
	pinMode(keyPinA2, INPUT_PULLUP);
	pinMode(keyPinA3, INPUT_PULLUP);

	pinMode(keyPinB1, INPUT_PULLUP);
	pinMode(keyPinB2, INPUT_PULLUP);
	pinMode(keyPinB3, INPUT_PULLUP);

	pinMode(keyPinC1, INPUT_PULLUP);
	pinMode(keyPinC2, INPUT_PULLUP);
	pinMode(keyPinC3, INPUT_PULLUP);

	// Toggle principal
	pinMode(togglePrincipalPin, INPUT_PULLUP);
	pinMode(toggleLEDPin, OUTPUT);

	// Lock
	pinMode(lockPin, INPUT_PULLUP);
	pinMode(lockLEDPin, OUTPUT);

	// RGB LED
	pinMode(rgbKeyPin, INPUT_PULLUP);
	pinMode(ledRedPin, OUTPUT);
	pinMode(ledBluePin, OUTPUT);
	pinMode(ledGreenPin, OUTPUT);

	// Buzzer
	pinMode(buzzerPin, OUTPUT);

	// Turn RGB LED Off
	refreshLEDColor();

	// Print device info on connection
	showDeviceInfo();
	showLockState();

	// Init tasker jobs
	tasker.setInterval(ui, 10, 0);
	tasker.setInterval(readCommands, 10, 1);
}

void loop()
{
	tasker.loop();
}

void ui() {
	checkKeys();
	refreshLEDColor();
}

void checkKeys()
{
	checkKey(keyPinA1, "A1");
	checkKey(keyPinA2, "A2");
	checkKey(keyPinA3, "A3");

	checkKey(keyPinB1, "B1");
	checkKey(keyPinB2, "B2");
	checkKey(keyPinB3, "B3");

	checkKey(keyPinC1, "C1");
	checkKey(keyPinC2, "C2");
	checkKey(keyPinC3, "C3");

	checkKey(togglePrincipalPin, "$TOGGLEPRINCIPAL");
	checkKey(rgbKeyPin, "$MODE");

	if (!digitalRead(lockPin) && lockDown == false)
	{
		lockDown = true;
	}
	else if (digitalRead(lockPin) && lockDown == true) {
		lockDown = false;
		//Serial.println("$TOGGLELOCK");
		toggleLock();
	}
}

void checkKey(int pinNumber, String keyName)
{
	if (!digitalRead(pinNumber) && pinDown == 0)
	{
		keyDown(pinNumber, keyName);
	}
	else if (digitalRead(pinNumber) && pinDown == pinNumber) {
		keyUp(pinNumber, keyName);
	}
}
void keyDown(int pinNumber, String keyName)
{
	Serial.println(keyName + "_Down");
	pinDown = pinNumber;
}

void keyUp(int pinNumber, String keyName)
{
	Serial.println(keyName + "_Up");
	pinDown = 0;
}

void refreshLEDColor()
{
	analogWrite(ledRedPin, RED);
	analogWrite(ledGreenPin, GREEN);
	analogWrite(ledBluePin, BLUE);

	// Confirm color
	String message = "$R" + String(flipValue(RED), DEC) + ";G" + String(flipValue(GREEN), DEC) + ";B" + String(flipValue(BLUE), DEC) + ";";
	if (message != ledColorMessage) {
		ledColorMessage = message;
		Serial.println(ledColorMessage);
	}
}

void readCommands()
{
	if (Serial)
	{
		String value = Serial.readString();

		if (value == "") {
			return;
		}

		if (value == "$FW")
		{
			showDeviceInfo();
			return;
		}
		else if (value == "$BEEP")
		{
			beep();
			return;
		}
		else if (value == "$TOGGLELOCK")
		{
			toggleLock();
			return;
		}
		else if (value.startsWith("R"))
		{
			// Parse out colors from string e.g. R0;G0;B255;
			char charBuf[15];
			value.toCharArray(charBuf, 15);
			char* valPosition = strtok(charBuf, ";");

			while (valPosition != NULL)
			{
				String num = String(valPosition);
				colorName = num[0];
				num.remove(0, 1);

				if (colorName == 'R')
				{
					RED = flipValue(num.toInt());
				}
				else if (colorName == 'G')
				{
					GREEN = flipValue(num.toInt());
				}
				else if (colorName == 'B')
				{
					BLUE = flipValue(num.toInt());
				}

				valPosition = strtok(NULL, ";");
			}
		}
		else if (value == "$RED:1") {
			digitalWrite(toggleLEDPin, 1);
		}
		else if (value == "$RED:0") {
			digitalWrite(toggleLEDPin, 0);
		}
		else if (value.startsWith("$PLAY:")) {
			value.remove(0, 6);
			char charBuf[250];
			value.toCharArray(charBuf, 250);
			char* valPosition = strtok(charBuf, ";");
			while (valPosition != NULL)
			{
				String noteAndLength = String(valPosition);
				char note = noteAndLength[0];
				noteAndLength.remove(0, 1);
				int duration = noteAndLength.toInt();

				if (note == 't') {
					tempo = duration;
				}
				else if (note == ' ') {
					delay(duration * tempo);
				}
				else {
					playNote(note, duration * tempo);
				}

				delay(tempo / 2);

				valPosition = strtok(NULL, ";");
			}
		}
	}
}

int flipValue(int value)
{
	return 255 - value; // 255 - value
}

void beep()
{
	for (int i = 0; i < 250; i++)
	{
		digitalWrite(buzzerPin, HIGH);
		delayMicroseconds(500);
		digitalWrite(buzzerPin, LOW);
		delayMicroseconds(500);
	}
}

void toggleLock() {
	locked = !locked;
	digitalWrite(lockLEDPin, locked);
	showLockState();
}

void showDeviceInfo() {
	Serial.println("$FW_VERSION:" + VERSION);
}

void showLockState() {
	if (locked) {
		Serial.println("$LOCKED");
	}
	else {
		Serial.println("$UNLOCKED");
	}
}

void playTone(int tone, int duration) {
	for (long i = 0; i < duration * 1000L; i += tone * 2) {
		digitalWrite(buzzerPin, HIGH);
		delayMicroseconds(tone);
		digitalWrite(buzzerPin, LOW);
		delayMicroseconds(tone);
	}
}

void playNote(char note, int duration) {
	char names[] = { 'c', 'd', 'e', 'f', 's', 'g', 'a', 'v', 'b', 'C', 'D', 'E' };
	int tones[] = { 1915, 1700, 1519, 1432, 1352, 1275, 1136, 1073, 1014, 956, 852, 758 };

	// play the tone corresponding to the note name
	for (int i = 0; i < sizeof(names); i++) {
		if (names[i] == note) {
			playTone(tones[i], duration);
		}
	}
}