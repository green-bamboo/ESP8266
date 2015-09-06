/*
*  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
*
*  You need to get streamId and privateKey at data.sparkfun.com and paste them
*  below. Or just customize this script to talk to other HTTP servers.
*
*/

#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <Wire.h>
#include <time.h>
#include "font.h"

//#define ESP01
#define ESP12

#define HOME
//#define WORK

//#define						UTILITY
#define						SHED

#define DEBUG_SERIAL
//#define DEBUG_OLED

#define PIN_ONE_WIRE_BUS	0 // DS18B20 pin
#define PIN_IC2_SDA			1 // OLED pins
#define PIN_IC2_SCL			3 // OLED pins

#define INTERVAL_SECONDS	60
#define INTERVAL_LOOPS		20







#define		ADDR_START		0
#define		ADDR_LOOP		4
#define		ADDR_ACTION		8
#define		ADDR_CONNECT	12
#define		ADDR_ATTEMPT	16
#define		ADDR_ERROR		20
#define		ADDR_OK			24



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

/*
	VCC			RST			CH_PD		TX
	Red						Red			Yellow
	Orange		Green		Purple		White

	RX			GPIO0		GPIO2		GND
	Orange		Green		White		Black
	Yellow		Blue		Grey		Black


Black
Brown
Red
1 Orange
2 Yellow
3 Green
4 Blue
5 Purple
6 Grey
7 White
8 Black

Black
White
Gray
Purple
Blue
Green
Yellow
Orange
Red
Brown
Black



	*/




#ifdef DEBUG_OLED
#define offset 0x00    // SDD1306                      // offset=0 for SSD1306 controller
#define OLED_address  0x3c                             // all the OLED's I have seen have this address
String line1 = "";
String line2 = "";
String line3 = "";
String line4 = "";
String line5 = "";
String line6 = "";
String line7 = "";
String line8 = "";
String line9 = "";
#endif


#if defined(ESP01)


#elif defined(ESP12)
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
#endif


#define	ROOM				1
#define FRIDGE				0
#define	EXCHANGER			2

#define	SHED_INDOOR			1
#define SHED_OUTDOOR		0

#if defined(HOME) && defined(UTILITY)
#define FEEDBACK_IDX		192
#define SENSORS				3
int idx[] = { 186, 187, 195 };
float temperatures[] = { 0, 0, 0 };
// 186 TEMP12_BackFridge
// 187 TEMP11_UtilityRoom
// 195 TEMP13_HeatExchanger

const char* DEVICE = "esp8266_Utilty";
const char* streamId = "g6M8ZX98J0sKnv34LxM3";
const char* privateKey = "qz9mGb2mD0Cg6Mr8kRAr";

#elif defined(HOME) && defined(SHED)

#define FEEDBACK_IDX		191
#define SENSORS				2
int idx[] = { 184, 185 };
float temperatures[] = { 0, 0, 0};
// 191 SHED_SENSOR
// 184 SHED_OUTDOOR
// 185 SHED_INDOOR

const char* DEVICE = "esp8266_shed";
const char* streamId = "XGv3QRmbxbTEArmdnzp3";
const char* privateKey = "1J0mp91xPxCl0x9apYqz";

#elif defined(WORK) && defined(UTILITY)
#define FEEDBACK_IDX		29
#define SENSORS				3
int idx[] = { 12, 11, 36 };
float temperatures[] = { 0, 0, 0, 0, 0, 0 };

const char* DEVICE = "esp8266_test";
const char* streamId = "ro1aggv676CyGod2JgDA";
const char* privateKey = "jkeRYYJEXEtY6olkpebr";


#elif defined(WORK) && defined(SHED)
#define FEEDBACK_IDX		34
#define SENSORS				3
int idx[] = { 10, 9, -1 };
float temperatures[] = { 0, 0, 0, 0, 0, 0 };

const char* DEVICE = "esp8266_test";
const char* streamId = "ro1aggv676CyGod2JgDA";
const char* privateKey = "jkeRYYJEXEtY6olkpebr";

#endif

#ifdef HOME
const char* SSID = "VM934292-2G";
const char* PASSWORD = "qufunkkn";
const char* LOCATION = "home";

const char* DOMO_IP = "192.168.0.205";
const int DOMO_PORT = 8080;

#endif


#ifdef WORK
const char* SSID = "BTHomeHub2-FTH5";
const char* PASSWORD = "4b65ad7b28";
const char* LOCATION = "work";

const char* DOMO_IP = "192.168.1.223"; //  "10.22.38.15";
const int DOMO_PORT = 6262;

#endif


// Actions
const char* actionStreamId = "aGOYD75Q5LH4EyroXWjr";
const char* actionPrivateKey = "KEolj14K4ptxGAnvjWkn";


const char* sparkfun = "data.sparkfun.com";
String url;
int port;

OneWire oneWire(PIN_ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

int state = 0;
String temp;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;

long starts = 0;
long loops = 0;
long actions = 0;
long connected = 0;
long attempts = 0;
long errors = 0;
long oks = 0;


// ======================================================================================================================================================


void Debug()
{
#if defined(DEBUG_SERIAL)
	Serial.println();

#elif defined(DEBUG_OLED)

	temp = "";
	temp += state;
	sendStrXY(temp.c_str(), 0, 0);

	temp = "";
	temp += day();

	sendStrXY(temp.c_str(), 0, 5);

	temp = "";
	temp += hour();
	temp += ":";
	temp += minute();
	temp += ":";
	temp += second();
	temp += ":";

	sendStrXY(temp.c_str(), 0, 7);

#endif
}


void Debug(String msg)
{

#if defined(DEBUG_SERIAL)
	Serial.println(msg);

#elif defined(DEBUG_OLED)

	//clear_display();

	while (msg.length() < 16)
	{
		msg += " ";
	}

	line9 = line8;
	line8 = line7;
	line7 = line6;
	line6 = line5;
	line5 = line4;
	line4 = line3;
	line3 = line2;
	line2 = line1;
	line1 = msg;

	sendStrXY(line1.c_str(), 1, 0);
	sendStrXY(line2.c_str(), 2, 0);
	sendStrXY(line3.c_str(), 3, 0);
	sendStrXY(line4.c_str(), 4, 0);
	sendStrXY(line5.c_str(), 5, 0);
	sendStrXY(line6.c_str(), 6, 0);
	sendStrXY(line7.c_str(), 7, 0);

	Debug();

#endif

}


#if defined(DEBUG_OLED)
//==========================================================//
// Resets display depending on the actual mode.
static void reset_display(void)
{
	displayOff();
	clear_display();
	displayOn();
}

//==========================================================//
// Turns display on.
void displayOn(void)
{
	sendcommand(0xaf);        //display on
}

//==========================================================//
// Turns display off.
void displayOff(void)
{
	sendcommand(0xae);		//display off
}

//==========================================================//
// Clears the display by sendind 0 to all the screen map.
static void clear_display(void)
{
	unsigned char i, k;
	for (k = 0; k < 8; k++)
	{
		setXY(k, 0);
		{
			for (i = 0; i < (128 + 2 * offset); i++)     //locate all COL
			{
				SendChar(0);         //clear all COL
				//delay(10);
			}
		}
	}
}

//==========================================================//
// Actually this sends a byte, not a char to draw in the display. 
// Display's chars uses 8 byte font the small ones and 96 bytes
// for the big number font.
static void SendChar(unsigned char data)
{
	//if (interrupt && !doing_menu) return;   // Stop printing only if interrupt is call but not in button functions

	Wire.beginTransmission(OLED_address); // begin transmitting
	Wire.write(0x40);//data mode
	Wire.write(data);
	Wire.endTransmission();    // stop transmitting
}

//==========================================================//
// Prints a display char (not just a byte) in coordinates X Y,
// being multiples of 8. This means we have 16 COLS (0-15) 
// and 8 ROWS (0-7).
static void sendCharXY(unsigned char data, int X, int Y)
{
	setXY(X, Y);
	Wire.beginTransmission(OLED_address); // begin transmitting
	Wire.write(0x40);//data mode

	for (int i = 0; i < 8; i++)
		Wire.write(pgm_read_byte(myFont[data - 0x20] + i));

	Wire.endTransmission();    // stop transmitting
}

//==========================================================//
// Used to send commands to the display.
static void sendcommand(unsigned char com)
{
	Wire.beginTransmission(OLED_address);     //begin transmitting
	Wire.write(0x80);                          //command mode
	Wire.write(com);
	Wire.endTransmission();                    // stop transmitting
}

//==========================================================//
// Set the cursor position in a 16 COL * 8 ROW map.
static void setXY(unsigned char row, unsigned char col)
{
	sendcommand(0xb0 + row);                //set page address
	sendcommand(offset + (8 * col & 0x0f));       //set low col address
	sendcommand(0x10 + ((8 * col >> 4) & 0x0f));  //set high col address
}


//==========================================================//
// Prints a string regardless the cursor position.
static void sendStr(unsigned char *string)
{
	unsigned char i = 0;
	while (*string)
	{
		for (i = 0; i < 8; i++)
		{
			SendChar(pgm_read_byte(myFont[*string - 0x20] + i));
		}
		*string++;
	}
}

//==========================================================//
// Prints a string in coordinates X Y, being multiples of 8.
// This means we have 16 COLS (0-15) and 8 ROWS (0-7).
static void sendStrXY(const char *string, int X, int Y)
{
	setXY(X, Y);
	unsigned char i = 0;
	while (*string)
	{
		for (i = 0; i < 8; i++)
		{
			SendChar(pgm_read_byte(myFont[*string - 0x20] + i));
		}
		*string++;
	}
}


//==========================================================//
// Inits oled and draws logo at startup
static void init_OLED(void)
{
	sendcommand(0xae);		//display off
	sendcommand(0xa6);            //Set Normal Display (default)
	// Adafruit Init sequence for 128x64 OLED module
	sendcommand(0xAE);             //DISPLAYOFF
	sendcommand(0xD5);            //SETDISPLAYCLOCKDIV
	sendcommand(0x80);            // the suggested ratio 0x80
	sendcommand(0xA8);            //SSD1306_SETMULTIPLEX
	sendcommand(0x3F);
	sendcommand(0xD3);            //SETDISPLAYOFFSET
	sendcommand(0x0);             //no offset
	sendcommand(0x40 | 0x0);      //SETSTARTLINE
	sendcommand(0x8D);            //CHARGEPUMP
	sendcommand(0x14);
	sendcommand(0x20);             //MEMORYMODE
	sendcommand(0x00);             //0x0 act like ks0108

	//sendcommand(0xA0 | 0x1);      //SEGREMAP   //Rotate screen 180 deg
	sendcommand(0xA0);

	//sendcommand(0xC8);            //COMSCANDEC  Rotate screen 180 Deg
	sendcommand(0xC0);

	sendcommand(0xDA);            //0xDA
	sendcommand(0x12);           //COMSCANDEC
	sendcommand(0x81);           //SETCONTRAS
	sendcommand(0xCF);           //
	sendcommand(0xd9);          //SETPRECHARGE 
	sendcommand(0xF1);
	sendcommand(0xDB);        //SETVCOMDETECT                
	sendcommand(0x40);
	sendcommand(0xA4);        //DISPLAYALLON_RESUME        
	sendcommand(0xA6);        //NORMALDISPLAY             

	clear_display();
	sendcommand(0x2e);            // stop scroll
	//----------------------------REVERSE comments----------------------------//
	sendcommand(0xa0);		//seg re-map 0->127(default)
	sendcommand(0xa1);		//seg re-map 127->0
	sendcommand(0xc8);
	delay(1000);
	//----------------------------REVERSE comments----------------------------//
	// sendcommand(0xa7);  //Set Inverse Display  
	// sendcommand(0xae);		//display off
	sendcommand(0x20);            //Set Memory Addressing Mode
	sendcommand(0x00);            //Set Memory Addressing Mode ab Horizontal addressing mode
	//  sendcommand(0x02);         // Set Memory Addressing Mode ab Page addressing mode(RESET)  
}

#endif


long GetCounter(int address)
{
	long four = EEPROM.read(address + 0);
	long three = EEPROM.read(address + 1);
	long two = EEPROM.read(address + 2);
	long one = EEPROM.read(address + 3);

	long value = ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);

	return value;
} // GetCounter



void SetCounter(int address, long value)
{
	byte four = (value & 0xFF);
	byte three = ((value >> 8) & 0xFF);
	byte two = ((value >> 16) & 0xFF);
	byte one = ((value >> 24) & 0xFF);

	EEPROM.write(address + 0, four);
	EEPROM.write(address + 1, three);
	EEPROM.write(address + 2, two);
	EEPROM.write(address + 3, one);

	EEPROM.commit();
} // SetCounter


long IncrementCounter(int address)
{
	long value = GetCounter(address);

	value++;

	SetCounter(address, value);

	return value;
} // IncrementCounter


bool SendData(const char* host, int port)
{
	Debug("Sending to");
	Debug(host);

	url = "GET " + url;
	url += " HTTP/1.1\r\n";
	url += "Host: 1.1.1.1\r\n";
	url += "User-Agent: esp8266\r\n";
	url += "\r\n";

	// Use WiFiClient class to create TCP connections
	WiFiClient client;

	if (!client.connect(host, port))
	{
		Debug("connection failed");
		return false;
	}

	Debug("Requesting URL");
	Debug(url);

	// This will send the request to the server
	client.print(url);

	delay(50);

	// Read all the lines of the reply from server and print them
	while (client.available())
	{
		String line = client.readStringUntil('\r');
		Debug(line);
	}

	Debug("closing connection");

	return true;
} // SendData


void GetTemperatures()
{
	DS18B20.requestTemperatures();

	for (int i = 0; i < SENSORS; i++)
	{
		temperatures[i] = DS18B20.getTempCByIndex(i);

		temp = "T ";
		temp += i;
		temp += " : ";
		temp += temperatures[i];

		Debug(temp);
	}
}


void Sleep()
{

	Debug("SLEEP");

#if defined(ESP01)
	state = 201; // ignore, do nothing

	for (int i = 0; i < INTERVAL_SECONDS; i++)
	{
		Debug();
		delay(500);
		Debug();
		delay(500);
	}

	// ESP.restart();
	state = 0;

#elif defined(ESP12)
	uint32_t sleep = INTERVAL_SECONDS * 1000 * 1000;
	ESP.deepSleep(sleep, RFMode::RF_NO_CAL);

#else
	Debug();
	Debug();
	Debug("=========================================================");
	Debug("");
	Debug("=========================================================");
	Debug();
	Debug();
#endif

} // GetTemperatures


void GetTemperatureURL()
{

#if defined(HOME) && defined(UTILITY)
	url = "/input/";
	url += streamId;
	url += "?private_key=";
	url += privateKey;

	url += "&attempt=";
	url += attempts;

	url += "&errors=";
	url += errors;

	url += "&exchanger=";
	url += temperatures[EXCHANGER];

	url += "&fridge=";
	url += temperatures[FRIDGE];

	url += "&room=";
	url += temperatures[ROOM];

	url += "&run=";
	url += oks;

#elif defined(HOME) && defined(SHED)
	url = "/input/";
	url += streamId;
	url += "?private_key=";
	url += privateKey;

	url += "&attempt=";
	url += attempts;

	url += "&errors=";
	url += errors;

	url += "&outdoors=";
	url += temperatures[SHED_OUTDOOR];

	url += "&run=";
	url += oks;

	url += "&shed=";
	url += temperatures[SHED_INDOOR];

#elif defined(WORK)
	url = "/input/";
	url += streamId;
	url += "?private_key=";
	url += privateKey;

	url += "&attempt=";
	url += attempts;

	url += "&errors=";
	url += errors;

	url += "&exchanger=";
	url += temperatures[0];

	url += "&fridge=";
	url += temperatures[1];

	url += "&room=";
	url += temperatures[2];

	url += "&run=";
	url += oks;

#endif

} // GetTemperatureURL


void DoAction()
{

	//digitalWrite(GPIO14, HIGH);

	Debug("WiFi On");
	WiFi.mode(WiFiMode::WIFI_STA);

	delay(1000);

	Debug("Connecting ");
	Debug(SSID);

	WiFi.begin(SSID, PASSWORD);

	int counter = 0;
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);

		temp = ". ";
		temp += counter;
		temp += " ";
		temp += WiFi.status();

		Debug(temp);

		if (counter++ > 100)
		{
			Debug("No Connection");
			Sleep();
		}
	}

	connected = IncrementCounter(ADDR_CONNECT);

	Debug("IP address: ");
	temp = "";
	temp += WiFi.localIP();
	Debug(temp);

	GetTemperatures(); // ignore, allow to "warm up"

	delay(1000);

	GetTemperatures(); // use these ones

	//digitalWrite(GPIO4, HIGH);

	//

	Debug("Counters 1");
	attempts = IncrementCounter(ADDR_ATTEMPT);
	oks = GetCounter(ADDR_OK);
	errors = GetCounter(ADDR_ERROR);

	Debug("Strings 1");

	String debug = "S";
	debug += starts;
	debug += "+A";
	debug += actions;
	debug += "+C";
	debug += connected;
	debug += "+A";
	debug += attempts;
	debug += "+E";
	debug += errors;
	debug += "+";
	debug += oks;

	// SparkFun - Actions

	url = "/input/";
	url += actionStreamId;
	url += "?private_key=";
	url += actionPrivateKey;
	url += "&action=";
	url += debug;
	url += "&device=";
	url += DEVICE;
	url += "&location=";
	url += LOCATION;
	SendData(sparkfun, 80);

	// SparkFun - Temperature

	GetTemperatureURL();

	if (SendData(sparkfun, 80))
		IncrementCounter(ADDR_OK);
	else
		IncrementCounter(ADDR_ERROR);

	// Domoticz Temperature

	Debug("Domoticz");

	for (int i = 0; i < SENSORS; i++)
	{
		int index = idx[i];

		if (index >= 0)
		{
			temp = "Domoticz ";
			temp += i;
			temp += " ";
			temp += index;
			temp += " ";
			temp += temperatures[i];
			Debug(temp);

			url = "/json.htm";
			url += "?type=command";
			url += "&param=udevice";
			url += "&idx=";
			url += index;
			url += "&nvalue=0";
			url += "&svalue=";
			url += temperatures[i];

			if (SendData(DOMO_IP, DOMO_PORT))
				IncrementCounter(ADDR_OK);
			else
				IncrementCounter(ADDR_ERROR);
		}
	}

	// Domoticz Feedback

	Debug("Domoticz Feedback");

	url = "/json.htm";
	url += "?type=command";
	url += "&param=udevice";
	url += "&idx=";
	url += FEEDBACK_IDX;
	url += "&nvalue=0";
	url += "&svalue=";
	url += debug;

	if (SendData(DOMO_IP, DOMO_PORT))
		IncrementCounter(ADDR_OK);
	else
		IncrementCounter(ADDR_ERROR);

	//

	//digitalWrite(GPIO5, HIGH);

	delay(1000);

	WiFi.disconnect();

	Debug("WiFi Off");
	WiFi.mode(WiFiMode::WIFI_OFF);

	//

} // DoAction


// ======================================================================================================================================================


void setup()
{
#if defined(DEBUG_SERIAL)
	Serial.begin(9600);

#elif defined(DEBUG_OLED)

	Wire.begin(PIN_IC2_SDA, PIN_IC2_SCL);                                // Initialize I2C and OLED Display
	init_OLED();
	reset_display();

	Debug("Startup");

	clear_display();

	sendStrXY("0", 0, 0);

	sendStrXY("1010", 10, 10);
	sendStrXY("1050", 10, 50);
	sendStrXY("5050", 50, 50);
	sendStrXY("5010", 50, 10);

	sendStrXY("5090", 50, 90);

	sendStrXY("9050", 90, 50);

#endif

	setTime(0, 0, 0, 1, 1, 2000);

	//Debug("Startup");

#ifdef ESP12
	// turn off all LEDs
	pinMode(GPIO14, OUTPUT);
	pinMode(GPIO12, OUTPUT);
	pinMode(GPIO13, OUTPUT);
	pinMode(GPIO4, OUTPUT);
	pinMode(GPIO5, OUTPUT);
	//pinMode(GPIO0, OUTPUT);
	//pinMode(GPIO2, OUTPUT);
	pinMode(GPIO15, OUTPUT);

	// Test board RGB
	digitalWrite(GPIO12, LOW);
	digitalWrite(GPIO13, LOW);
	digitalWrite(GPIO15, LOW);

	// Test board LEDs
	digitalWrite(GPIO14, HIGH);
	digitalWrite(GPIO4, HIGH);
	digitalWrite(GPIO5, HIGH);
	//digitalWrite(GPIO0, HIGH);
	//digitalWrite(GPIO2, HIGH);
#endif

	EEPROM.begin(1024);

	// read in values from storage

	starts = IncrementCounter(ADDR_START);
	connected = 0;
	attempts = 0;
	errors = 0;
	oks = 0;

	//

	previousMillis = millis();
} // setup


// ======================================================================================================================================================


void loop()
{
#if defined(DEBUG_SERIAL)
	if (Serial.available() > 0)
	{
		String input = Serial.readStringUntil('\n');
		if (input != NULL && input.indexOf("RESET") >= 0)
		{
			SetCounter(ADDR_START, 0);
			SetCounter(ADDR_LOOP, 0);
			SetCounter(ADDR_ACTION, 0);
			SetCounter(ADDR_CONNECT, 0);
			SetCounter(ADDR_ATTEMPT, 0);
			SetCounter(ADDR_ERROR, 0);
			SetCounter(ADDR_OK, 0);

			ESP.restart();
		}

	}
#endif

	//

	currentMillis = millis();

	if ((currentMillis - previousMillis) > 1000)
	{
		Debug();

		if (state == 0)
		{
			Debug("waiting 5");
			state = 1;
		}
		else if (state == 1)
		{
			Debug("waiting 4");
			state = 2;
		}
		else if (state == 2)
		{
			Debug("waiting 3");
			state = 3;
		}
		else if (state == 3)
		{
			Debug("waiting 2");
			state = 4;
		}
		else if (state == 4)
		{
			Debug("waiting 1");
			state = 5;
		}
		else if (state == 5)
		{
			Debug("waiting 0");
			state = 50;
		}
		else if (state == 50)
		{
			state = 51;

			loops = IncrementCounter(ADDR_LOOP);

			temp = "Loops ";
			temp += loops;
			Debug(temp);

			if (loops < INTERVAL_LOOPS)
			{
				Debug("Not Ready");
				Debug("Sleeping");
				Sleep();
			}
			else
			{
				SetCounter(ADDR_LOOP, 0);
				actions = IncrementCounter(ADDR_ACTION);

				GetTemperatures(); // ignore, allow to "warm up"

				Debug("Action");
				DoAction();
				state = 60;
			}
		}
		else if (state == 60)
		{
			// Just waiting
			state = 91;
		}
		else if (state > 90 && state < 99)
		{
			// Just waiting
			Debug("waiting to sleep");
			state++;
		}
		else if (state == 99)
		{
			state = 101;
			Sleep();
		}

		previousMillis = millis();
	}


} // loop



