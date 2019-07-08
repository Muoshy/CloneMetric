#include <Arduino.h>

//WiFi
#include <ESP8266WiFi.h> 
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>

#include <WiFiManager.h> 
#include <DNSServer.h>
#include <ESP8266WebServer.h>

//NTP
#include <TimeLib.h> 	// library for keeping time
#include <NTPClient.h> 	// helper functions for requesting ntp time

//Json parsing
//#include <JsonStreamingParser.h> 
//#include <JsonListener.h>
#include <OpenWeatherMapCurrent.h>
#include <CurrencyRate.h>

//fastLED and GFX libs
#include <SPI.h>
#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>

//frame class for creating and displaying frames;
#include <Frame.h>

//Misc	  
#include <sprites.h>	  //sprite bitmaps 

//#define sensorPin 0

// WiFi ---------------------------
//const char *WIFI_SSID = " "; 	//wifi ssid if you don't want to use wifimanager
//const char *WIFI_PWD = " ";	//wifi pass
// WiFi ---------------------------



//Json parsing -----------------------------------
WiFiUDP Udp; 																//wificlient for json

OpenWeatherMapCurrent wClient; 												//initialize client for parsing weather data
OpenWeatherMapCurrentData weatherData; 										//initialize data structure for holdng weather data
const String OPEN_WEATHER_MAP_APP_ID = ""; 									//your openweathermap api key
const String OPEN_WEATHER_MAP_LOCATION_ID = "";					  			//your city code
const String OPEN_WEATHER_MAP_LANGUAGE = "en";							   	//english
const boolean IS_METRIC = true;											   	//get metric, false for imperial units

CurrencyRate cClient; 
CurrencyRateData ratesData;
const String CurrencyRateUrl = "http://www.floatrates.com/daily/sek.json"; 	//url json for currencyrates of chosen currency,
//Json parsing ------------------------------------



//NTP----------------------------------------------
const int NTP_PACKET_SIZE = 48;												//NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; 										//buffer to hold incoming & outgoing packets
const char *ntpServerName = "0.europe.pool.ntp.org";								//server name for ntp server
const int UTC_OFFSET = 2;	  												//set utc offset
unsigned int localPort = 8888; 												//local port to listen for UDP packets

time_t getNtpHelper()														//passing function without argument to return time_t for setsyncinterval
{ 																			
	return getNtpTime(Udp);
}
//NTP----------------------------------------------


//FastLED NeoMatrix -------------------------------
#define mw 32				//matrix width
#define mh 8				//matrix height
#define NUMMATRIX (mw * mh) //number of leds

#define DATA_PIN D2		 	//data for led comms
#define BRIGHTNESS 30		//starting brightness [0,255]

CRGB leds[NUMMATRIX]; 		//initialize led array

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, mw, mh, //matrix initialization
	NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
	NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
//FastLED NeoMatrix -------------------------------



//Frame----------------------------------------------
uint8_t Frame::sensorPin = 0; 			//set sensorPin for all frames (static member)

TimeFrame timeFrame(15, 0xFFFF); 		// create object of subclasses of frame(duration, textcolor, sensorpin for brightness control)

StaticFrame dateFrame(5, 0x07E0); 		//green
StaticFrame dollarFrame(5,0x07FF);		//cyan
StaticFrame euroFrame(5, 0xFEE0);		//yellow

AnimatedFrame weatherFrame(5, 0xFBE0);	//orange
frameData fdata; 						//create data structure containing pointer to array of animation frames and number of frames
//Frame-----------------------------------------------


void setup()
{
	
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUMMATRIX);	//Matrix setup
	FastLED.setBrightness(BRIGHTNESS);
	matrix->setTextWrap(false);
	matrix->setTextSize(1);

	Serial.begin(9600);

	/*
	WiFi.begin(WIFI_SSID, WIFI_PWD);						//Wifi setup if you want to use hardcoded SSID and pass

	//Serial.print("Connecting");
	uint8_t count = 0;
	
	while (WiFi.status() != WL_CONNECTED)
	{
		matrix->drawRGBBitmap(9, 0, wifi_sprite[count], 13, 8);
		matrix->show();
		delay(200);
		matrix->clear();
		count = (count + 1) & 3;
	}

	matrix->clear();
	matrix->show();
	*/

	WiFiManager wifiManager;								//remove if you decide to use hardcoded SSID and password instead
	delay(1000);

    wifiManager.autoConnect();

	Udp.begin(localPort);									//ntp

	setSyncProvider(getNtpHelper); 							//get NTP time
	setSyncInterval(300); 									//resync time every 10 mins (300s)

	matrix->drawRGBBitmap(12, 1, checkmark, 6, 6); 			// draw checkmark to indicate successfull connection
	matrix->show();
	
	cClient.updateCurrent(&ratesData, CurrencyRateUrl);		//get currency data
	wClient.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);			
	wClient.setMetric(IS_METRIC);
	wClient.updateCurrentById(&weatherData, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID); //get weather data

	matrix->clear();

	//Serial.println();
	//Serial.print("Connected, IP address: ");
	//Serial.println(WiFi.localIP());

	dateFrame.setBitmap(calendar[day()-1], 9, 8); 									//bitmap array, width, height
	dateFrame.setText(getDay(), 3);													//text to be displayed, offset from bitmap

	dollarFrame.setBitmap(currencySprite[0], 8, 8); 								
	dollarFrame.setText(String(ratesData.USD) + "." + String(ratesData.usdF), 0); 	
	dollarFrame.digitFont();

	euroFrame.setBitmap(currencySprite[1], 8, 8); 
	euroFrame.setText(String(ratesData.EUR) + "." + String(ratesData.eurF), 0); 
	euroFrame.digitFont();

	fdata = weatherBitmap(weatherData.weatherId);									//choose bitmap array based on weatherId and get nrOfFrames(rows) from the array, defined in Frames.cpp
	weatherFrame.setBitmap(fdata.bitmap, fdata.frameCount, 200, 8, 8); 				//setBitmap(2dim array with animation frames, nr of frames, fps, width, height)
	weatherFrame.setText(String(weatherData.temp) + "'" + "C", 2); 					
	weatherFrame.digitFont();

}


void loop()
{	
	
	timeFrame.display();
	dateFrame.display();
	dollarFrame.display();
	euroFrame.display();
	weatherFrame.display();

																											//fix method to update day more frequently later on

	EVERY_N_MINUTES(10) 																					//display something for this?
	{	
		dateFrame.setText(getDay(), 3);
		dateFrame.setBitmap(calendar[day()-1], 9 , 8);																		//update day can also be placed in a more frequently running loop, every 15mins is good enough for me
		wClient.updateCurrentById(&weatherData, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID);
		fdata = weatherBitmap(weatherData.weatherId);				
		weatherFrame.setBitmap(fdata.bitmap, fdata.frameCount, 200, 8, 8); 	
		weatherFrame.setText(String(weatherData.temp) + "'" + "C", 2);
	}

	EVERY_N_HOURS(24)
	{	
		cClient.updateCurrent(&ratesData, CurrencyRateUrl); 												//refresh currencydata every 24h
		dollarFrame.setText(String(ratesData.USD) + "." + String(ratesData.usdF), 0);
		euroFrame.setText(String(ratesData.EUR) + "." + String(ratesData.eurF), 0);
	}
	
}	

