#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define GPIO0		0	// Flash
#define GPIO1		1	// TX
#define GPIO2		2
#define GPIO3		3	// RX
#define GPIO4		4
#define GPIO5		5

#define ANALOG


#define HOME
//#define WORK

#define PIN			2
#define PIXELS		50
#define MAX			50

#ifdef HOME
const char* SSID = "VM934292-2G";
const char* PASSWORD = "qufunkkn";
const char* LOCATION = "home";
const char* DEVICE = "esp8266_W";

IPAddress LAN_IP(192, 168, 0, 180);
IPAddress LAN_GATEWAY(192, 168, 0, 1);
IPAddress LAN_SUBNET(255, 255, 255, 0);
#endif


#ifdef WORK
const char* SSID = "BTHomeHub2-FTH5";
const char* PASSWORD = "4b65ad7b28";
const char* LOCATION = "work";
const char* DEVICE = "esp8266_C";

IPAddress LAN_IP(192, 168, 1, 180);
IPAddress LAN_GATEWAY(192, 168, 1, 254);
IPAddress LAN_SUBNET(255, 255, 255, 0);
#endif

unsigned int localPort = 4545;
const int NTP_PACKET_SIZE = 3;
byte packetBuffer[NTP_PACKET_SIZE];
WiFiUDP udp;


unsigned long currentMillis = 0;
unsigned long previousMillis = 0;


int Shade(int shade)
{
	int value = PWMRANGE - (shade * 4);

	if (value < 0)
		value = 0;

	if (value > PWMRANGE)
		value = PWMRANGE;

	return value;
}


void setup()
{
	Serial.begin(9600);
	Serial.println("Test ESP-12 with LEDs");

	pinMode(GPIO0, OUTPUT);
	pinMode(GPIO1, OUTPUT);
	pinMode(GPIO2, OUTPUT);

	//

	Serial.println("WiFi On");
	WiFi.mode(WiFiMode::WIFI_STA);

	Serial.print("Setting static IP address ");
	Serial.print(LAN_IP);
	Serial.print(",");
	Serial.print(LAN_GATEWAY);
	Serial.print(",");
	Serial.print(LAN_SUBNET);
	Serial.println();
	WiFi.config(LAN_IP, LAN_GATEWAY, LAN_SUBNET);

	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(SSID);

	WiFi.begin(SSID, PASSWORD);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	//

	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	delay(1000);

	//Serial.println("Start Server");
	//server.begin();

	//delay(1000);

	//printWifiStatus();

	Serial.println("Starting UDP");
	udp.begin(localPort);
	Serial.print("Local port: ");
	Serial.println(udp.localPort());

	previousMillis = millis();


}


void loop6()
{

	//Serial.println("Loop");

	analogWrite(GPIO0, 0);
	analogWrite(GPIO1, 0);
	analogWrite(GPIO2, 0);

	delay(5000);

	analogWrite(GPIO0, PWMRANGE); // green
	analogWrite(GPIO1, PWMRANGE); // blue
	analogWrite(GPIO2, 0); // red

	delay(1000);

	analogWrite(GPIO0, 0); // green
	analogWrite(GPIO1, PWMRANGE);
	analogWrite(GPIO2, PWMRANGE);

	delay(1000);

	analogWrite(GPIO0, PWMRANGE);
	analogWrite(GPIO1, 0); // blue
	analogWrite(GPIO2, PWMRANGE);

	delay(1000);

	analogWrite(GPIO0, PWMRANGE);
	analogWrite(GPIO1, 0);
	analogWrite(GPIO2, 0);

	delay(1000);

	analogWrite(GPIO0, 0);
	analogWrite(GPIO1, PWMRANGE);
	analogWrite(GPIO2, 0);

	delay(1000);

	analogWrite(GPIO0, 0);
	analogWrite(GPIO1, 0);
	analogWrite(GPIO2, PWMRANGE);

	delay(1000);

	analogWrite(GPIO0, PWMRANGE);
	analogWrite(GPIO1, PWMRANGE);
	analogWrite(GPIO2, PWMRANGE);

	delay(1000);


	//Serial.println("GPIO0");
	//Fade2(GPIO0);

	//Serial.println("GPIO2");
	//Fade2(GPIO2);

	for (int i = 0; i < 255; i++)
	{

		analogWrite(GPIO0, Shade(0)); // green
		analogWrite(GPIO1, Shade(0)); // blue
		analogWrite(GPIO2, Shade(i)); // red

		delay(10);
	}

	for (int i = 0; i < 255; i++)
	{

		analogWrite(GPIO0, Shade(i)); // green
		analogWrite(GPIO1, Shade(0)); // blue
		analogWrite(GPIO2, Shade(0)); // red

		delay(10);
	}

	for (int i = 0; i < 255; i++)
	{

		analogWrite(GPIO0, Shade(0)); // green
		analogWrite(GPIO1, Shade(i)); // blue
		analogWrite(GPIO2, Shade(0)); // red

		delay(10);
	}

	for (int i = 0; i < 255; i++)
	{

		analogWrite(GPIO0, Shade(i)); // green
		analogWrite(GPIO1, Shade(i)); // blue
		analogWrite(GPIO2, Shade(i)); // red

		delay(10);
	}


}

void loop()
{

	while (udp.parsePacket())
	{
		int read = udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

		uint8_t red = packetBuffer[0];
		uint8_t green = packetBuffer[1];
		uint8_t blue = packetBuffer[2];

		analogWrite(GPIO0, Shade(green)); // green
		analogWrite(GPIO1, Shade(blue)); // blue
		analogWrite(GPIO2, Shade(red)); // red
	}


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
		//delay(10);
	}

	for (int i = 1; i < max; i++)
	{
		analogWrite(pin, i);
		//delay(10);
	}

	analogWrite(pin, 0);

}


