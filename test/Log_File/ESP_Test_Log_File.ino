/*
*  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
*
*  You need to get streamId and privateKey at data.sparkfun.com and paste them
*  below. Or just customize this script to talk to other HTTP servers.
*
*/

#include <ESP8266WiFi.h>
#include <FS.h>
//#include <RTC.h>

// #define HOME
#define WORK


#ifdef HOME
const char* SSID = "VM934292-2G";
const char* PASSWORD = "qufunkkn";
const char* LOCATION = "home";
const char* DEVICE = "esp8266_D";

const char* DOMO_IP = "192.168.0.204";
const int DOMO_PORT = 8080;

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
#endif


String url;
String result;
//char* host;
//int port;

int value = 0;
int count = 0;
int action = 0;


// ======================================================================================================================================================



bool SendData(const char* host, int port)
{
	url = "GET " + url;
	url += " HTTP/1.1\r\n";
	url += "Host: www.timeapi.org\r\n";
	//url += "User-Agent: esp8266\r\n";
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

	Serial.print("Requesting URL: ");
	Serial.println(url);

	Serial.print("Status: ");
	Serial.println(client.status());

	// This will send the request to the server
	client.print(url);

	for (int i = 0; i < 50; i++)
	{
		delay(50);

		Serial.print("Status: ");
		Serial.println(client.status());

		// Read all the lines of the reply from server and print them to Serial
		while (client.available())
		{
			result = client.readStringUntil('\r');

			result.replace('\n', ' ');
			result.replace('\r', ' ');
			result.trim();

			Serial.println(result.length());

			Serial.print("[[");

			Serial.print(result);

			Serial.println("]]");

			if (result.indexOf("Date:") >= 0 && result.length() >= 31)
			{
				String stamp = result.substring(6, 31);
				Serial.print("Date= [");
				Serial.print(stamp);
				Serial.println("]");
			}
		}
	}

	Serial.println();
	Serial.println("closing connection");

	return true;
} // SendData



void RunningText()
{
	//time_t = time;
}


void Test1()
{
	//udp.begin(123);

	//udp.beginPacket("time.nist.gov",)
}


// ======================================================================================================================================================


void setup()
{
	Serial.begin(9600);

	//setTime(0, 0, 0, 1, 1, 2001);

	//SPIFFS.begin();

	Serial.println("Startup");

	for (int i = 0; i < 2; i++)
	{
		Serial.println("waiting");
		delay(1000);
	}

	Serial.println("Lets Go");
} // setup


// ======================================================================================================================================================


void loop()
{
	delay(3000);
	++value;
	count++;

	Serial.println("Tick");

	//

	if (action == 1)
	{
		int status = WiFi.status();

		if (status == WL_CONNECTED)
		{
			Serial.println("Connected");
			action = 2;
		}
		else
		{
			if (count > 35)
			{
				Serial.println("Aborting");
				count = 0;
				action = 0;
			}
			else
			{
				Serial.print("Connecting ");
				Serial.print(status);
				Serial.println(" ...");
			}
		}

	}
	else if (action == 2)
	{
		IPAddress ip = WiFi.localIP();

		Serial.println("");
		Serial.println("WiFi connected");
		Serial.println("IP address: ");
		Serial.println(ip);

		time_t hello;

		action = 3;
	}
	else if (action == 3)
	{
		url = "/utc/now.json";
		url = "/gmt";

		// http: //www.earthtools.org/
		SendData("www.timeapi.org", 80);

		action = 99;
	}
	else if (action == 99)
	{
		WiFi.disconnect();

		Serial.println("WiFi Off");
		WiFi.mode(WiFiMode::WIFI_OFF);

		count = 0;
		action = 0;
	}

	//

	if (count >= 4 && action == 0)
	{

		Serial.println("WiFi On");
		WiFi.mode(WiFiMode::WIFI_STA);

		Serial.println();
		Serial.println();
		Serial.print("Connecting to ");
		Serial.println(SSID);

		WiFi.begin(SSID, PASSWORD);

		action = 1;
	}


} // loop



