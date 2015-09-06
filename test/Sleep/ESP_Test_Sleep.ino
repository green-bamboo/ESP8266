
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


void setup()
{
	Serial.begin(9600);
	Serial.println("Test ESP-12 with LEDs");

	pinMode(GPIO14, OUTPUT);
	pinMode(GPIO12, OUTPUT);
	pinMode(GPIO13, OUTPUT);
	pinMode(GPIO4, OUTPUT);
	pinMode(GPIO5, OUTPUT);
	pinMode(GPIO0, OUTPUT);
	pinMode(GPIO2, OUTPUT);
	pinMode(GPIO15, OUTPUT);

	// Test board RGB
	digitalWrite(GPIO12, LOW);
	digitalWrite(GPIO13, LOW);
	digitalWrite(GPIO15, LOW);

	// Test board LEDs
	digitalWrite(GPIO14, LOW);
	digitalWrite(GPIO4, LOW);
	digitalWrite(GPIO5, LOW);
	digitalWrite(GPIO0, LOW);
	digitalWrite(GPIO2, LOW);

	//

	delay(500);

	Serial.println("Count Down");

	delay(500);

	Serial.println(".");
	Serial.println(".");
	digitalWrite(GPIO14, LOW);
	Serial.println(".");
	digitalWrite(GPIO4, LOW);
	Serial.println(".");
	digitalWrite(GPIO5, LOW);
	Serial.println(".");
	digitalWrite(GPIO0, LOW);
	Serial.println(".");
	digitalWrite(GPIO2, LOW);

	//

	Serial.println("5");
	delay(WAIT);
	digitalWrite(GPIO14, HIGH);
	Serial.println("4");
	delay(WAIT);
	digitalWrite(GPIO4, HIGH);
	Serial.println("3");
	delay(WAIT);
	digitalWrite(GPIO5, HIGH);
	Serial.println("2");
	delay(WAIT);
	digitalWrite(GPIO0, HIGH);
	Serial.println("1");
	delay(WAIT);
	digitalWrite(GPIO2, HIGH);
	Serial.println("0");
	delay(WAIT);

	//

	Serial.println("Sleep");

	uint32_t sleep = 20 * 1000 * 1000;

	ESP.deepSleep(sleep, RFMode::RF_NO_CAL);

}

void loop()
{
}


