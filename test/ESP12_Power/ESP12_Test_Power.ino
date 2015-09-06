/*
*  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
*
*  You need to get streamId and privateKey at data.sparkfun.com and paste them
*  below. Or just customize this script to talk to other HTTP servers.
*
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <time.h>

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

#define HOME
//#define WORK


#ifdef HOME
const char* SSID = "VM934292-2G";
const char* PASSWORD = "qufunkkn";
const char* LOCATION = "home";
const char* DEVICE = "esp8266_D";

const char* DOMO_IP = "192.168.0.205";
const int DOMO_PORT = 8080;

const char* sparkfun = "data.sparkfun.com";
const char* streamId = "aGOYD75Q5LH4EyroXWjr";
const char* privateKey = "KEolj14K4ptxGAnvjWkn";

#define FEEDBACK_IDX		191
#define SENSORS				2
int idx[] = { 184, 185 };
float temperatures[] = { 0, 0, 0 };
// 191 SHED_SENSOR
// 184 SHED_OUTDOOR
// 185 SHED_INDOOR

#endif


#ifdef WORK
const char* SSID = "BTHomeHub2-FTH5";
const char* PASSWORD = "4b65ad7b28";
const char* LOCATION = "work";
const char* DEVICE = "esp8266_C";

const char* DOMO_IP = "192.168.1.223"; //  "10.22.38.15";
const int DOMO_PORT = 6262;

const char* sparkfun = "data.sparkfun.com";
const char* streamId = "aGOYD75Q5LH4EyroXWjr";
const char* privateKey = "KEolj14K4ptxGAnvjWkn";

#define FEEDBACK_IDX		34
#define SENSORS				2
int idx[] = { 10, 9 };

#endif


String url;
int port;

int total = 0;
int count = 99999;
int attempt = 0;
int ok = 0;
double temp = 11.0;


// ======================================================================================================================================================



bool SendData(const char* host, int port)
{
	url = "GET " + url;
	url += " HTTP/1.1\r\n";
	url += "Host: 1.1.1.1\r\n";
	url += "User-Agent: esp8266\r\n";
	url += "\r\n";

	Serial.print("connecting to ");
	Serial.println(host);

	// Use WiFiClient class to create TCP connections
	WiFiClient client;

	if (!client.connect(host, port))
	{
		Serial.println("connection failed");
		return false;
	}

	//Serial.print("Requesting URL: ");
	//Serial.println(url);

	// This will send the request to the server
	client.print(url);

	delay(50);

	// Read all the lines of the reply from server and print them to Serial
	while (client.available())
	{
		String line = client.readStringUntil('\r');
		//Serial.print(line);
	}

	/*
	for (int i = 0; i < 70; i++)
	{
	delay(10);

	// Read all the lines of the reply from server and print them to Serial
	while (client.available())
	{
	String line = client.readStringUntil('\r');
	Serial.print(line);

	if (line.indexOf("Date:") >= 0 && line.length() >= 31)
	{
	line.replace('\n', ' ');
	line.replace('\r', ' ');
	line.trim();
	startedStamp = line.substring(6, 31);
	startedStamp.replace(' ', '_');
	startedStamp.replace(',', '_');
	startedStamp.replace("__", "_");


	}
	}
	}
	*/

	Serial.println();
	Serial.println("closing connection");

	return true;
} // SendData



void Sleep()
{
	uint32_t seconds = 5 * 60; // 5 minutes
	uint32_t sleep = seconds * 1000 * 1000;
	ESP.deepSleep(sleep, RFMode::RF_NO_CAL);
}

// ======================================================================================================================================================


void setup()
{
	Serial.begin(9600);

	Serial.println("Startup");

	// turn off all LEDs
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
	digitalWrite(GPIO0, HIGH);
	digitalWrite(GPIO2, HIGH);

	for (int i = 5; i > 0; i--)
	{
		Serial.print("waiting ");
		Serial.println(i);
		delay(1000);
	}

	//

	digitalWrite(GPIO14, HIGH);

	Serial.println("WiFi On");
	WiFi.mode(WiFiMode::WIFI_STA);

	delay(1000);

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

	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	delay(1000);

	digitalWrite(GPIO4, HIGH);

	//

	// SparkFun

	url = "/input/";
	url += streamId;
	url += "?private_key=";
	url += privateKey;
	url += "&action=";
	url += "TICK";
	url += "&device=";
	url += DEVICE;
	url += "&location=";
	url += LOCATION;

	SendData(sparkfun, 80);

	// Domoticz Temperature

	temp = 15.0;

	url = "/json.htm";
	url += "?type=command";
	url += "&param=udevice";
	url += "&idx=";
	url += idx[0];
	url += "&nvalue=0";
	url += "&svalue=";
	url += temp; // temperature

	SendData(DOMO_IP, DOMO_PORT);

	// Domoticz Feedback

	url = "/json.htm";
	url += "?type=command";
	url += "&param=udevice";
	url += "&idx=";
	url += FEEDBACK_IDX;
	url += "&nvalue=0";
	url += "&svalue=Hello";

	SendData(DOMO_IP, DOMO_PORT);

	//

	digitalWrite(GPIO5, HIGH);

	delay(1000);

	WiFi.disconnect();

	Serial.println("WiFi Off");
	WiFi.mode(WiFiMode::WIFI_OFF);

	//

	Sleep();

} // setup


// ======================================================================================================================================================


void loop()
{
} // loop



