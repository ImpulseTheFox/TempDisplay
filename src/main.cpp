#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

static const int PANEL_COUNT = 4;

//Edit these pins if desired
//Avoiding pins 0 and 15 (and 12), as they are used to change boot-modes. See: https://github.com/espressif/esptool/wiki/ESP32-Boot-Mode-Selection
static const int PIN_DIG_1 = 2;
static const int PIN_DIG_2 = 4;
static const int PIN_DIG_3 = 5;
static const int PIN_DIG_4 = 18;
static const int PIN_A     = 13;
static const int PIN_B     = 12; //This is okay, no connection to voltage or ground
static const int PIN_C     = 14;
static const int PIN_D     = 27;
static const int PIN_E     = 26;
static const int PIN_F     = 25;
static const int PIN_G     = 33;
static const int PIN_DP    = 32;

Adafruit_BMP280 bme; // I2C
//Adafruit_BMP280 bme(BMP_CS); // hardware SPI
//Adafruit_BMP280 bme(BMP_CS, BMP_MOSI, BMP_MISO, BMP_SCK);

static bool bme_begin_successful;

//Setting everything to LOW
void resetDisplay()
{
	digitalWrite(PIN_DIG_1, LOW);
	digitalWrite(PIN_DIG_2, LOW);
	digitalWrite(PIN_DIG_3, LOW);
	digitalWrite(PIN_DIG_4, LOW);
	digitalWrite(PIN_A, LOW);
	digitalWrite(PIN_B, LOW);
	digitalWrite(PIN_C, LOW);
	digitalWrite(PIN_D, LOW);
	digitalWrite(PIN_E, LOW);
	digitalWrite(PIN_F, LOW);
	digitalWrite(PIN_G, LOW);
	digitalWrite(PIN_DP, LOW);
}

//Shows a digit on the display. The first position is 0. Digit should only be 0-9.
void showDigit(char character, int position, bool enableDot)
{
	resetDisplay(); //Reset first because I don't bother writing LOW here for every other option

	if (position >= 0 && position <= 3)
	{
		switch(position)
		{
			case 0:
				digitalWrite(PIN_DIG_1, HIGH);
				break;
			case 1:
				digitalWrite(PIN_DIG_2, HIGH);
				break;
			case 2:
				digitalWrite(PIN_DIG_3, HIGH);
				break;
			case 3:
				digitalWrite(PIN_DIG_4, HIGH);
				break;
			default:
				return;
		}

		switch(character)
		{
			case '0':
				digitalWrite(PIN_A, HIGH);
				digitalWrite(PIN_B, HIGH);
				digitalWrite(PIN_C, HIGH);
				digitalWrite(PIN_D, HIGH);
				digitalWrite(PIN_E, HIGH);
				digitalWrite(PIN_F, HIGH);
				break;
			case '1':
				digitalWrite(PIN_B, HIGH);
				digitalWrite(PIN_C, HIGH);
				break;
			case '2':
				digitalWrite(PIN_A, HIGH);
				digitalWrite(PIN_B, HIGH);
				digitalWrite(PIN_D, HIGH);
				digitalWrite(PIN_E, HIGH);
				digitalWrite(PIN_G, HIGH);
				break;
			case '3':
				digitalWrite(PIN_A, HIGH);
				digitalWrite(PIN_B, HIGH);
				digitalWrite(PIN_C, HIGH);
				digitalWrite(PIN_D, HIGH);
				digitalWrite(PIN_G, HIGH);
				break;
			case '4':
				digitalWrite(PIN_B, HIGH);
				digitalWrite(PIN_C, HIGH);
				digitalWrite(PIN_F, HIGH);
				digitalWrite(PIN_G, HIGH);
				break;
			case '5':
				digitalWrite(PIN_A, HIGH);
				digitalWrite(PIN_C, HIGH);
				digitalWrite(PIN_D, HIGH);
				digitalWrite(PIN_F, HIGH);
				digitalWrite(PIN_G, HIGH);
				break;
			case '6':
				digitalWrite(PIN_A, HIGH);
				digitalWrite(PIN_C, HIGH);
				digitalWrite(PIN_D, HIGH);
				digitalWrite(PIN_E, HIGH);
				digitalWrite(PIN_F, HIGH);
				digitalWrite(PIN_G, HIGH);
				break;
			case '7':
				digitalWrite(PIN_A, HIGH);
				digitalWrite(PIN_B, HIGH);
				digitalWrite(PIN_C, HIGH);
				break;
			case '8':
				digitalWrite(PIN_A, HIGH);
				digitalWrite(PIN_B, HIGH);
				digitalWrite(PIN_C, HIGH);
				digitalWrite(PIN_D, HIGH);
				digitalWrite(PIN_E, HIGH);
				digitalWrite(PIN_F, HIGH);
				digitalWrite(PIN_G, HIGH);
				break;
			case '9':
				digitalWrite(PIN_A, HIGH);
				digitalWrite(PIN_B, HIGH);
				digitalWrite(PIN_C, HIGH);
				digitalWrite(PIN_D, HIGH);
				digitalWrite(PIN_F, HIGH);
				digitalWrite(PIN_G, HIGH);
				break;
			case 'F':
				digitalWrite(PIN_A, HIGH);
				digitalWrite(PIN_E, HIGH);
				digitalWrite(PIN_F, HIGH);
				digitalWrite(PIN_G, HIGH);
				break;
			case 'A':
				digitalWrite(PIN_A, HIGH);
				digitalWrite(PIN_B, HIGH);
				digitalWrite(PIN_C, HIGH);
				digitalWrite(PIN_E, HIGH);
				digitalWrite(PIN_F, HIGH);
				digitalWrite(PIN_G, HIGH);
				break;
			case 'I':
				digitalWrite(PIN_B, HIGH);
				digitalWrite(PIN_C, HIGH);
				break;
			case 'L':
				digitalWrite(PIN_D, HIGH);
				digitalWrite(PIN_E, HIGH);
				digitalWrite(PIN_F, HIGH);
				break;
			default:
				return;
		}

		if (enableDot)
		{
			digitalWrite(PIN_DP, HIGH);
		}
	}
}

//Shows a string on the display for 10 seconds
void showString(String str)
{
	String strWithoutDot = str;
	strWithoutDot.replace(".", "");
	if(strWithoutDot.length() <= PANEL_COUNT)
	{
		unsigned long startMillis = millis();
		while (true)
		{
			for (int i = 0; i < str.length(); i++)
			{
				char currentChar = str.charAt(i);
				if (currentChar != '.') //Skip the dot
				{
					int displayIndex; //Index without the dot

					if (str.indexOf('.') != -1 && i > str.indexOf('.'))
					{
						displayIndex = i - 1;
					}
					else
					{
						displayIndex = i;
					}

					bool nextIsDot = str.charAt(i + 1) == '.';
					showDigit(currentChar, displayIndex, nextIsDot);
				}
				//Add a delay here to make flickering visible. Increase value of delay for more flickering.
				//delay(10);
			}

			if ((millis() - startMillis) >= (10 * 1000)) //Break after 10 seconds
			{
				break;
			}
		}
	}
}

//Shows a temperature on the display for 10 seconds
void showTemp(double temp)
{
	if (temp < 100 && temp > -100)
	{
		char tempString[8];
		sprintf(tempString, "%05.2f", temp);
		
		showString(tempString);
	}
}

void setup()
{
	bme_begin_successful = bme.begin();

	pinMode(PIN_DIG_1, OUTPUT);
	pinMode(PIN_DIG_2, OUTPUT);
	pinMode(PIN_DIG_3, OUTPUT);
	pinMode(PIN_DIG_4, OUTPUT);
	pinMode(PIN_A, OUTPUT);
	pinMode(PIN_B, OUTPUT);
	pinMode(PIN_C, OUTPUT);
	pinMode(PIN_D, OUTPUT);
	pinMode(PIN_E, OUTPUT);
	pinMode(PIN_F, OUTPUT);
	pinMode(PIN_G, OUTPUT);
	pinMode(PIN_DP, OUTPUT);

	Serial.begin(115200);
}

void loop()
{
	if (bme_begin_successful)
	{
		double temp = bme.readTemperature();
		Serial.println("Temp: " + String(temp) + "°C");
		showTemp(temp);
	}
	else
	{
		Serial.println("Failed reading temperature!");
		showString("FAIL");
	}
}
