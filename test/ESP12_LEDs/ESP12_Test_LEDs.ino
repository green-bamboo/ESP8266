
#define GPIO0		0	// Flash
#define GPIO1		1	// TX
#define GPIO2		2
#define GPIO3		3	// RX
#define GPIO4		4
#define GPIO5		5
#define GPIO6		99 // RESERVED
#define GPIO7		99 // RESERVED
#define GPIO8		99 // RESERVED
#define GPIO9		99 // RESERVED
#define GPIO10		99 // RESERVED
#define GPIO11		99 // RESERVED
#define GPIO12		12
#define GPIO13		13 // RX2
#define GPIO14		14
#define GPIO15		15 // TX2
#define GPIO16		16	// Wake Up Timr Pin

#define WAIT		1000
#define LON			LOW
#define LOFF		HIGH

//#define DIGITAL
#define ANALOG

/*
ESP-12 test board
RST						TXD
ADC		(photocell)		RXD
CH_PD					GPIO4 (led5)
GPIO16 (led7)			GPIO5 (led4)
GPIO14 (led6)			GPIO0 (led3)
GPIO12 (green)			GPIO2 (led2)
GPIO13 (blue)			GPIO15 (red)
VCC						GND

Pin numbers in Arduino correspond directly to the ESP8266 GPIO pin numbers.
pinMode, digitalRead, and digitalWrite functions work as usual, so to read GPIO2, call digitalRead(2).

Digital pins 0—15 can be INPUT, OUTPUT, or INPUT_PULLUP.
Pin 16 can be INPUT, OUTPUT or INPUT_PULLDOWN_16. At startup, pins are configured as INPUT.

*/

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;

void setup()
{
	Serial.begin(9600);
	Serial.println("Test ESP-12 with LEDs");

#ifdef DIGITAL
	pinMode(GPIO16, OUTPUT);
	pinMode(GPIO14, OUTPUT);
	pinMode(GPIO12, OUTPUT);
	pinMode(GPIO13, OUTPUT);
	pinMode(GPIO4, OUTPUT);
	pinMode(GPIO5, OUTPUT);
	pinMode(GPIO0, OUTPUT);
	pinMode(GPIO2, OUTPUT);
	pinMode(GPIO15, OUTPUT);

	digitalWrite(GPIO16, LOFF);
	digitalWrite(GPIO14, LOFF);
	digitalWrite(GPIO12, LOW);
	digitalWrite(GPIO13, LOW);
	digitalWrite(GPIO4, LOFF);
	digitalWrite(GPIO5, LOFF);
	digitalWrite(GPIO0, LOFF);
	digitalWrite(GPIO2, LOFF);
	digitalWrite(GPIO15, LOW);
#endif

	// what ESP.deepSleep(100, RFMode::RF_NO_CAL);

	previousMillis = millis();
}

void loop()
{

	Serial.println("Loop");

#ifdef ANALOG

	Serial.println("GPIO12");
	Fade1(GPIO12);

	Serial.println("GPIO13");
	Fade1(GPIO13);

	Serial.println("GPIO15");
	Fade1(GPIO15);


	Serial.println("GPIO16");
	Fade2(GPIO16);

	Serial.println("GPIO14");
	Fade2(GPIO14);

	Serial.println("GPIO4");
	Fade2(GPIO4);

	Serial.println("GPIO5");
	Fade2(GPIO5);

	Serial.println("GPIO0");
	Fade2(GPIO0);

	Serial.println("GPIO2");
	Fade2(GPIO2);

#endif

#ifdef DIGITAL
	Serial.println("GPIO16");
	digitalWrite(GPIO16, LON);
	delay(WAIT);
	digitalWrite(GPIO16, LOFF);
	delay(WAIT);

	Serial.println("GPIO14");
	digitalWrite(GPIO14, LON);
	delay(WAIT);
	digitalWrite(GPIO14, LOFF);
	delay(WAIT);

	Serial.println("GPIO12");
	digitalWrite(GPIO12, HIGH);
	delay(WAIT);
	digitalWrite(GPIO12, LOW);
	delay(WAIT);

	Serial.println("GPIO13");
	digitalWrite(GPIO13, HIGH);
	delay(WAIT);
	digitalWrite(GPIO13, LOW);
	delay(WAIT);

	Serial.println("GPIO4");
	digitalWrite(GPIO4, LON);
	delay(WAIT);
	digitalWrite(GPIO4, LOFF);
	delay(WAIT);

	Serial.println("GPIO5");
	digitalWrite(GPIO5, LON);
	delay(WAIT);
	digitalWrite(GPIO5, LOFF);
	delay(WAIT);

	Serial.println("GPIO0");
	digitalWrite(GPIO0, LON);
	delay(WAIT);
	digitalWrite(GPIO0, LOFF);
	delay(WAIT);

	Serial.println("GPIO2");
	digitalWrite(GPIO2, LON);
	delay(WAIT);
	digitalWrite(GPIO2, LOFF);
	delay(WAIT);

	Serial.println("GPIO15");
	digitalWrite(GPIO15, HIGH);
	delay(WAIT);
	digitalWrite(GPIO15, LOW);
	delay(WAIT);
#endif


}


void Fade1(int pin)
{
	int max = PWMRANGE - 1;

	for (int i = 1; i < max; i++)
	{
		analogWrite(pin, i);
		delay(10);
	}

	for (int i = max; i > 0; i--)
	{
		analogWrite(pin, i);
		delay(10);
	}

	analogWrite(pin, 0);

}


void Fade2(int pin)
{
	int max = PWMRANGE - 1;

	for (int i = max; i > 0; i--)
	{
		analogWrite(pin, i);
		delay(10);
	}

	for (int i = 1; i < max; i++)
	{
		analogWrite(pin, i);
		delay(10);
	}

	analogWrite(pin, 0);

}


