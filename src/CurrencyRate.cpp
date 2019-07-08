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
#include <CurrencyRate.h>

CurrencyRate::CurrencyRate()
{
}


void CurrencyRate::updateCurrent(CurrencyRateData *data, String url)
{   
    doUpdate(data, url);
}

void CurrencyRate::doUpdate(CurrencyRateData *data, String url)
{
    unsigned long lostTest = 10000UL;
    unsigned long lost_do = millis();
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
    if (httpCode > 0)
    {

        WiFiClient *client = http.getStreamPtr();

        while (client->connected() || client->available())
        {
            while ((size = client->available()) > 0)
            {
                if ((millis() - lost_do) > lostTest)
                {
                    Serial.println("lost in client with a timeout");
                    client->stop();
                    ESP.restart();
                }
                c = client->read();
                if (c == '{' || c == '[')
                {

                    isBody = true;
                }
                if (isBody)
                {
                    parser.parse(c);
                }
                // give WiFi and TCP/IP libraries a chance to handle pending events
                yield();
            }
        }
    }
    this->data = nullptr;
}

void CurrencyRate::whitespace(char c)
{
    Serial.println("whitespace");
}

void CurrencyRate::startDocument()
{
    Serial.println("start document");
}

void CurrencyRate::key(String key)
{
    currentKey = String(key);
}

void CurrencyRate::value(String value)
{

    // only get rates for USD and EUR
    
    // check if json object "id" is eur
    if (currentParent == "usd")
    {
        // "inverseRate": 9.281722370085
        if (currentKey == "inverseRate")                                    //parse to the inverse rate, USD::SEK
        {
            this->data->USD = value.toInt();                                //get currencyrate
            this->data->usdF = (int) ((value.toFloat()-value.toInt())*100); //round off to two decimals
        }
    }
    // check if json object "id" is eur
    if (currentParent == "eur")
    {
        // "inverseRate": 9.281722370085
        if (currentKey == "inverseRate")
        {
            this->data->EUR = value.toInt();
            this->data->eurF = (int) ((value.toFloat()-value.toInt())*100);
        }
    }
}

void CurrencyRate::endArray()
{
}

void CurrencyRate::startObject()
{
    currentParent = currentKey;
}

void CurrencyRate::endObject()
{
    currentParent = "";
}

void CurrencyRate::endDocument()
{
}

void CurrencyRate::startArray()
{
}
