#include <Arduino.h>
#include <matrixDisplay.h>
#include <FastLED.h>
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <sprites.h>

#include <TimeLib.h>
#include <NTPClient.h>



//drawing help

void timeSlideIn(uint16_t color, FastLED_NeoMatrix *matrix) {
    for(int i = 6; i >= 0; i--) {
        displayTime_full(color, matrix, i);
        delay(50);
    }
}

void timeSlideOut(uint16_t color, FastLED_NeoMatrix *matrix) {
    for(int i = 0; i <= 6 ; i++) {
        displayTime_full(color, matrix, -i);
        delay(50);
    }
    delay(100);
}

void dateSlideIn(uint16_t color, FastLED_NeoMatrix *matrix) 
{   

    for(int i = 8; i >= 0; i--) 
    {
        displayDate(color, matrix, i);
        delay(50);
    }
    
}

void dateSlideOut(uint16_t color, FastLED_NeoMatrix *matrix)
{
    for(int i = 0; i <= 8; i++) {
        displayDate(color,matrix, -i);
        delay(50);
    }
}

void currencyDisplay(uint16_t color, FastLED_NeoMatrix *matrix, int shift, int index, int main, int frac ) 
{
    matrix->clear();
	matrix->drawRGBBitmap(-1, 0-shift, currencySprite[index], 8, 8 );
	matrix->setCursor(7,6+shift);
	matrix->print(String(main));
    if(main >= 10) {
        matrix->writePixel(16,6+shift, color);
	    matrix->setCursor(18,6+shift);
    }
    else {
        matrix->writePixel(12,6+shift, color);
	    matrix->setCursor(14,6+shift);
    }
	matrix->print(String(frac));
	matrix->show();
}

void weatherDisplay(uint16_t color, FastLED_NeoMatrix *matrix, int shift, int temp, int tempfrac, uint16_t code)
{
    uint8_t x = 0;

    matrix->clear();

    if( hour() >= 20 ) { 
            matrix->drawRGBBitmap(0, 0, sunCloud[0], 8, 8-shift);
            return;
    }


    if(code == 800) { 
        if( hour() >= 20 ) { 
            //moon
            matrix->drawRGBBitmap(0, 0, sunCloud[0], 8, 8-shift);
        }
        else{
            //sun
            matrix->drawRGBBitmap(0, 0, suncloudAnimation[0], 8, 8-shift);
        }
    }


    if(code == 801) { 
        //few clouds
        matrix->drawRGBBitmap(0, 0, sunCloud[0], 8, 8-shift);
    }


    if(code > 801 ) { 
        //cloudy
        matrix->drawRGBBitmap(0, 0, sunCloud[0], 8, 8-shift);
    }

    if(code >= 300 && code <= 531) { 
        // rain
        matrix->drawRGBBitmap(0, 0, rainAnimation[0], 8, 8-shift);
    }

    if(code >= 200 && code <= 232) {
        //thunder
    }

    if(code >= 600 && code <= 622) {
        //snow
    }

    matrix->setCursor(9, 6+shift);
    matrix->print(String(temp));
    
    if(temp>=10){x = 5;}
        
    matrix->writePixel(13+x,6+shift, color);  
    matrix->fillRect(20+x, 2+shift, 2, 2, color);
    matrix->setCursor(15+x,6+shift);  
    matrix->print(String(tempfrac));
    matrix->setCursor(23+x,6+shift);    
    matrix->print("C");
    matrix->show();

}

void weatherAnimation(FastLED_NeoMatrix *matrix, uint16_t code) {

     if(code == 800 && hour() < 20) {
         for(int i = 0; i<14; i++) {
            matrix->fillRect(0,0,8,8,0x0000);
            matrix->drawRGBBitmap(0, 0, suncloudAnimation[i], 8, 8);
            matrix->show();
            delay(200);    
        }
    }
    

    if(code >= 300 && code <= 531) { 
        //rain
        for(int i = 0; i<6; i++) {
            matrix->fillRect(0,0,8,8,0x0000);
            matrix->drawRGBBitmap(0, 0, rainAnimation[i], 8, 8);
            matrix->show();
            delay(200);   
        }
    }

    
}



void displayTime_full(uint16_t color, FastLED_NeoMatrix *matrix, int shift)
{   
    matrix->clear();
    matrix->setTextColor(color);
    matrix->setCursor(1, 6-shift);
    matrix->print(printDigits(hour()));
    matrix->writePixel(10, 3-shift, color);
    matrix->writePixel(10, 5-shift, color);
    matrix->setCursor(12, 6-shift);
    matrix->print(printDigits(minute()));
    matrix->writePixel(21, 3-shift, color);
    matrix->writePixel(21, 5-shift, color);
    matrix->setCursor(23, 6-shift);
    matrix->print(printDigits(second()));
    matrix->show();
}

void displayTime_seconds(uint16_t color, FastLED_NeoMatrix *matrix)
{
    matrix->fillRect(23, 1, 8, 6, 0x0000);
    matrix->setCursor(23, 6);
    matrix->setTextColor(color);
    matrix->print(printDigits(second()));
    matrix->show();
}

void displayTime_minutes(uint16_t color, FastLED_NeoMatrix *matrix)
{
    matrix->fillRect(12, 1, 8, 6, 0x0000);
    matrix->setCursor(12, 6);
    matrix->setTextColor(color);
    matrix->print(printDigits(minute()));
    matrix->show();
}

void displayTime_hours(uint16_t color, FastLED_NeoMatrix *matrix)
{
    matrix->fillRect(1, 1, 8, 6, 0x0000);
    matrix->setCursor(1, 6);
    matrix->setTextColor(color);
    matrix->print(printDigits(hour()));
    matrix->show();
}


void displayDate(uint16_t color, FastLED_NeoMatrix *matrix, int shift)
{
    matrix->clear();
	matrix->drawRGBBitmap(0, 0-shift, calendar[day()-1], 9, 8 );
	matrix->setFont();
    matrix->setCursor(12, 1+shift);
    matrix->print(getDay());
    matrix->show();
}
