/**The MIT License (MIT)
 
 Copyright (c) 2018 by ThingPulse Ltd., https://thingpulse.com
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include "OpenWeatherMapCurrent.h"
#include <sprites.h>

OpenWeatherMapCurrent::OpenWeatherMapCurrent() {

}

void OpenWeatherMapCurrent::updateCurrent(OpenWeatherMapCurrentData *data, String appId, String location) {
  doUpdate(data, buildUrl(appId, "q=" + location));
}

void OpenWeatherMapCurrent::updateCurrentById(OpenWeatherMapCurrentData *data, String appId, String locationId) {
  doUpdate(data, buildUrl(appId, "id=" + locationId));
}

String OpenWeatherMapCurrent::buildUrl(String appId, String locationParameter) {
  String units = metric ? "metric" : "imperial";
  return "http://api.openweathermap.org/data/2.5/weather?" + locationParameter + "&appid=" + appId + "&units=" + units + "&lang=" + language;
}

void OpenWeatherMapCurrent::doUpdate(OpenWeatherMapCurrentData *data, String url) {
  unsigned long lostTest = 10000UL;
  unsigned long lost_do = millis();
  this->weatherItemCounter = 0;
  this->data = data;
  JsonStreamingParser parser;
  parser.setListener(this);
  Serial.printf("Getting url: %s\n", url.c_str());
  HTTPClient http;

  http.begin(url);
  bool isBody = false;
  char c;
  int size;
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  Serial.printf("[HTTP] GET... code: %d\n", httpCode);
  if(httpCode > 0) {

    WiFiClient * client = http.getStreamPtr();

    while(client->connected() || client->available()) {
      while((size = client->available()) > 0) {
		if ((millis() - lost_do) > lostTest) {
			Serial.println ("lost in client with a timeout");
			client->stop();
			ESP.restart();
	    }
        c = client->read();
        if (c == '{' || c == '[') {

          isBody = true;
        }
        if (isBody) {
          parser.parse(c);
        }
        // give WiFi and TCP/IP libraries a chance to handle pending events
        yield();
      }
    }
  }
  this->data = nullptr;
}

void OpenWeatherMapCurrent::whitespace(char c) {
  Serial.println("whitespace");
}

void OpenWeatherMapCurrent::startDocument() {
  Serial.println("start document");
}

void OpenWeatherMapCurrent::key(String key) {
  currentKey = String(key);
}

void OpenWeatherMapCurrent::value(String value) {
  
  // weatherItemCounter: only get the first item if more than one is available
  if (currentParent == "weather" && weatherItemCounter == 0) {
    // "id": 521, weatherId weatherId;
    if (currentKey == "id") {
      this->data->weatherId = value.toInt();
    }
    // "main": "Rain", String main;
    if (currentKey == "main") {
      this->data->main = value;
    }
    // "description": "shower rain", String description;
    if (currentKey == "description") {
      this->data->description = value;
    }

  }

  // "temp": 290.56, float temp;
  if (currentKey == "temp") {
    this->data->temp = round(value.toFloat());
    this->data->tempfrac = (int) ((value.toFloat()-value.toInt())*10);
  }
  // "humidity": 87, uint8_t humidity;
  if (currentKey == "humidity") {
    this->data->humidity = value.toInt();
  }
  // "wind": {"speed": 1.5}, float windSpeed;
  if (currentKey == "speed") {
    this->data->windSpeed = value.toFloat();
  }
  // "clouds": {"all": 90}, uint8_t clouds;
  if (currentKey == "all") {
    this->data->clouds = value.toInt();
  }
  // "dt": 1527015000, uint64_t observationTime;
  if (currentKey == "dt") {
    this->data->observationTime = value.toInt();
  }
  // "name": "Zurich", String cityName;
  if (currentKey == "name") {
    this->data->cityName = value;
  }
}

void OpenWeatherMapCurrent::endArray() {

}


void OpenWeatherMapCurrent::startObject() {
  currentParent = currentKey;
}

void OpenWeatherMapCurrent::endObject() {
  if (currentParent == "weather") {
    weatherItemCounter++;
  }
  currentParent = "";
}

void OpenWeatherMapCurrent::endDocument() {

}


void OpenWeatherMapCurrent::startArray() {

}


