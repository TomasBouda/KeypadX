//**********************************
// Firmware version
const String VERSION = "0.5.0";
const String DEVICE_INFO = "KeypadX_160318";
//**********************************

int keyPinA1 = 3;
int keyPinA2 = 4;
int keyPinA3 = 5;

int keyPinB1 = 6;
int keyPinB2 = 7;
int keyPinB3 = 8;

int keyPinC1 = 9;
int keyPinC2 = 10;
int keyPinC3 = 11;

int togglePrincipalPin = 12;
int buzzerPin = A4;

// Lock variables
int lockPin = 14;
int lockCache = -1;

// RGB LED variables
int ledRedPin = A1;
int ledBluePin = A3;
int ledGreenPin = A2;
int RED, GREEN, BLUE = 0;

void setup() 
{
	Serial.begin(115200);
	Serial.setTimeout(100);

	pinMode(keyPinA1, INPUT_PULLUP);
	pinMode(keyPinA2, INPUT_PULLUP);
	pinMode(keyPinA3, INPUT_PULLUP);
	
	pinMode(keyPinB1, INPUT_PULLUP);
	pinMode(keyPinB2, INPUT_PULLUP);
	pinMode(keyPinB3, INPUT_PULLUP);
	
	pinMode(keyPinC1, INPUT_PULLUP);
	pinMode(keyPinC2, INPUT_PULLUP);
	pinMode(keyPinC3, INPUT_PULLUP);

	pinMode(togglePrincipalPin, INPUT_PULLUP);
	pinMode(lockPin, INPUT_PULLUP);

	pinMode(ledRedPin, OUTPUT);
	pinMode(ledBluePin, OUTPUT);
	pinMode(ledGreenPin, OUTPUT);

	pinMode(buzzerPin, OUTPUT);
}

void loop() 
{
	handleKeys();
	refreshLED();
}

void readSerial()
{
	if(Serial)
	{
		String value = Serial.readString();
	
		if(value == "$FW")
		{
			Serial.println("$FW_VERSION:" + VERSION);
			return;
		}
		else if(value == "$BEEP")
		{
			beep();
			return;
		}
		else
		{
			char charBuf[15];
			value.toCharArray(charBuf, 15);
			char* valPosition = strtok(charBuf, ";");
	
			while (valPosition != NULL)
			{ 
				char colorName;
				String num = String(valPosition);
				colorName = num[0];
				num.remove(0,1);
	
				if(colorName == 'R')
				{
					RED = num.toInt();
				}
				else if(colorName == 'G')
				{
					GREEN = num.toInt();
				}
				else if(colorName == 'B')
				{
					BLUE = num.toInt();
				}
		
				valPosition = strtok(NULL, ";");
			}
		}
	}
}

void handleKeys()
{
	if(!digitalRead(togglePrincipalPin))
	{
		Serial.println("$TOGGLEPRINCIPAL");
		touchDelay();
	}
	else
	{
		if(!digitalRead(keyPinA1))
		{
			Serial.println("A1");
			touchDelay();
		}
		if(!digitalRead(keyPinA2))
		{
			Serial.println("A2");
			touchDelay();
		}
		if(!digitalRead(keyPinA3))
		{
			Serial.println("A3");
			touchDelay();
		}
		if(!digitalRead(keyPinB1))
		{
			Serial.println("B1");
			touchDelay();
		}
		if(!digitalRead(keyPinB2))
		{
			Serial.println("B2");
			touchDelay();
		}
		if(!digitalRead(keyPinB3))
		{
			Serial.println("B3");
			touchDelay();
		}
		if(!digitalRead(keyPinC1))
		{
			Serial.println("C1");
			touchDelay();
		}
		if(!digitalRead(keyPinC2))
		{
			Serial.println("C2");
			touchDelay();
		}
		if(!digitalRead(keyPinC3))
		{
			Serial.println("C3");
			touchDelay();
		}
	}
	
	int lock = !digitalRead(lockPin);
	if(lock != lockCache)
	{
		if(lock == 0)
		{
			Serial.println("$UNLOCK");
		}
		else
		{
			Serial.println("$LOCK");
		}
		lockCache = lock;
		touchDelay();
	}
}

void refreshLED()
{
	readSerial();

	analogWrite(ledRedPin, RED);
	analogWrite(ledGreenPin, GREEN);
	analogWrite(ledBluePin, BLUE);
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

void touchDelay()
{
	delay(50);
}