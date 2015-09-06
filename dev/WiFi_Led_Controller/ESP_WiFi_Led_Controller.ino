#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

#define HOME
//#define WORK

#define DIGITAL
//#define ANALOG


#ifdef HOME
const char* SSID = "VM934292-2G";
const char* PASSWORD = "qufunkkn";
const char* LOCATION = "home";
const char* DEVICE = "esp8266_W";

IPAddress LAN_IP(192, 168, 0, 182);
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





#if defined(DIGITAL)
#define PIN			0
#define PIXELS		150
#define MAX			100

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ800);

#elif defined(ANALOG)

#define GPIO0		0	// Flash
#define GPIO1		1	// TX
#define GPIO2		2
#define GPIO3		3	// RX
#define GPIO4		45
#define GPIO5		5

#define GPIO_RED		0	// Flash
#define GPIO_GREEN		1	// TX
#define GPIO_BLUE		2

int strip = 1;

#endif


unsigned int localPort = 4545;      // local port to listen for UDP packets


const int NTP_PACKET_SIZE = 3; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

WiFiServer server(80);
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long receivedMillis = 0;



int Shade(int shade)
{
	int value = PWMRANGE - (shade * 4);

	if (value < 0)
		value = 0;

	if (value > PWMRANGE)
		value = PWMRANGE;

	return value;
}


void printWifiStatus()
{
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print your WiFi shield's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");

	Serial.print("Server Status : ");
	Serial.println(server.status());

}


void SetStrip(int r, int g, int b)
{

#if defined(DIGITAL)

	uint32 colour = strip.Color(r, g, b);

	for (int i = 0; i < strip.numPixels(); i++)
		strip.setPixelColor(i, colour);

#elif defined(ANALOG)

	analogWrite(GPIO_RED, Shade(r)); // red
	analogWrite(GPIO_GREEN, Shade(g)); // green
	analogWrite(GPIO_BLUE, Shade(b)); // blue

#endif

}


void BlackStrip()
{
	// Initialize all pixels to 'off'

	SetStrip(0, 0, 0);

#if defined(DIGITAL)
	strip.show();
#endif

}

void SetupStrip()
{

#if defined(DIGITAL)

	Serial.println("Clear strip");
	strip.begin();

#elif defined(ANALOG)

	pinMode(GPIO0, OUTPUT);
	pinMode(GPIO1, OUTPUT);
	pinMode(GPIO2, OUTPUT);

#endif

	BlackStrip();

}

// ======================================================================================================================================================


void setup()
{
	Serial.begin(9600);
	Serial.println("ESP WiFi Neo Pixel LED Controller");

	SetupStrip();

	for (int i = 0; i < 5; i++)
	{
		Serial.println("waiting");
		delay(1000);
	}

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

	Serial.println("Start Server");
	server.begin();

	delay(1000);

	printWifiStatus();

	Serial.println("Starting UDP");
	udp.begin(localPort);
	Serial.print("Local port: ");
	Serial.println(udp.localPort());

	previousMillis = millis();
}


void loop3()
{
	currentMillis = millis();

	if (currentMillis - previousMillis > 1000)
	{
		previousMillis = currentMillis;

		Serial.print("Tick,");
		Serial.print(server.status());
		Serial.print(",");
		Serial.print(WiFi.status());
		Serial.println();
	}

	// listen for incoming clients
	WiFiClient client = server.available();
	if (client)
	{
		Serial.print("new client,");
		Serial.print(client.remoteIP());
		Serial.print(",");
		Serial.print(client.remotePort());
		Serial.println();

		// an http request ends with a blank line
		boolean currentLineIsBlank = true;
		while (client.connected())
		{
			if (client.available())
			{
				String data = client.readStringUntil(' ');

				if (data.indexOf("GET") >= 0)
				{
					Serial.println("GET");
					Serial.println(data);
				}
				else if (data.indexOf("red") >= 0)
				{
					SetStrip(100, 0, 0);
				}
				else if (data.indexOf("colour") >= 0 && data.indexOf("?") >= 0)
				{
					Serial.println("DATA");
					Serial.println(data);
				}

			}
		}

		// give the web browser time to receive the data
		delay(1);

		// close the connection:
		client.stop();

		Serial.println("client disonnected");
	}
}


void loop()
{
	currentMillis = millis();

	if (currentMillis - previousMillis > 1000)
	{
		previousMillis = currentMillis;

		//Serial.print("Tick,");
		//Serial.print(server.status());
		//Serial.print(",");
		//Serial.print(WiFi.status());
		//Serial.println();

		if (WiFi.status() == WL_CONNECTED)
			Serial.println(":");
		else
			Serial.println(".");
	}

	//

	if (currentMillis - receivedMillis > (1000 * 60 * 5)) // 5 minute black out
	{
		Serial.println("black out");
		BlackStrip();
		receivedMillis = millis();
	}
	//else if (currentMillis - receivedMillis > 5000) // 1 second reset pixel start
	//{
	//		Serial.println("reset");
	//	receivedMillis = millis();
	//}

	//

	int packets = udp.parsePacket();

	while (packets > 0)
	{
#if defined(DIGITAL)

		//Serial.print("~");
		//Serial.println(packets);

		receivedMillis = millis();

		int loops = packets / 3;
		if (loops > PIXELS)
			loops = PIXELS;

		for (int i = 0; i < loops; i++)
		{
			int read = udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

			uint8_t red = packetBuffer[0];
			//Serial.print(red);
			//Serial.print(",");

			uint8_t green = packetBuffer[1];
			//Serial.print(green);
			//Serial.print(",");

			uint8_t blue = packetBuffer[2];
			//Serial.print(blue);
			//Serial.println();

			strip.setPixelColor(i, red, green, blue);
		}

		//if (pixel >= PIXELS)
		//{
		//Serial.println("#");
		strip.show();
		//}

#elif defined(ANALOG)

		int read = udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

		uint8_t red = packetBuffer[0];
		uint8_t green = packetBuffer[1];
		uint8_t blue = packetBuffer[2];

		SetStrip(red, green, blue);
#endif

		receivedMillis = millis();

		packets = udp.parsePacket();
	}


}
// ======================================================================================================================================================


