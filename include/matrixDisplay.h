#ifndef _matrixDisplay_h
#define _matrixDisplay_h

#include <Arduino.h>

class IPAddress;
class WiFiUDP;
class FastLED_NeoMatrix;

//Drawing functions

void timeSlideIn(uint16_t color, FastLED_NeoMatrix *matrix);

void timeSlideOut(uint16_t color, FastLED_NeoMatrix *matrix);

void dateSlideIn(uint16_t color, FastLED_NeoMatrix *matrix);

void dateSlideOut(uint16_t color, FastLED_NeoMatrix *matrix);

void currencyDisplay(uint16_t color, FastLED_NeoMatrix *matrix, int shift, int index, int main, int frac); 

void weatherDisplay(uint16_t color, FastLED_NeoMatrix *matrix, int shift, int temp, int tempfrac, uint16_t code);

void weatherAnimation(FastLED_NeoMatrix *matrix, uint16_t code);



void displayTime_full(uint16_t color, FastLED_NeoMatrix *matrix, int shift);

void displayTime_seconds(uint16_t color, FastLED_NeoMatrix *matrix);

void displayTime_minutes(uint16_t color, FastLED_NeoMatrix *matrix);

void displayTime_hours(uint16_t color, FastLED_NeoMatrix *matrix);

void displayDate(uint16_t color, FastLED_NeoMatrix *matrix, int shift);

#endif