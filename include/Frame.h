#ifndef _Frame_h
#define _Frame_h

#include <Arduino.h>
#include <FastLED.h>
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <pzim3x5.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <sprites.h>

extern FastLED_NeoMatrix *matrix;

struct frameData { //data struct to hold bitmap array for animation and nr of frames (array rows) in the animation.
    const uint16_t *bitmap;
    uint8_t frameCount;
};

frameData weatherBitmap(uint16_t weatherId); //funct to return data struct containing correct weatherbitmap given weatherId from openweathermap


class Frame //base class for a frame with duration, color and sensorPin for LDR
{
    private:
    
    protected:
    unsigned long dur;
    uint16_t color;
    static uint8_t sensorPin;

    public:
    void setDuration(unsigned long dur);
    void setColor(uint16_t color);
    void BrightnessCheck(); 
};

class TimeFrame: public Frame { //subclass of Frame for displaying time

    private:
    void displayTime_full(int8_t offset);
    void displayTime_seconds(int8_t offset);
    void displayTime_minutes(int8_t offset);
    void displayTime_hours(int8_t offset);

    protected:


    public:
    TimeFrame(unsigned long dur, uint16_t color);
    void display();

};

class StaticFrame: public Frame { //subclass of Frame for displaying static bitmap + text
    private:

    protected:
    const uint16_t *bitmap;
    const GFXfont *f = nullptr;
    bool dFont = false;
    String text = "";
    int x;
    int y;
    int w;
    int h;
    uint8_t textOffset = 0;

    public:
    StaticFrame(unsigned long dur, uint16_t color);
    void setBitmap(const uint16_t *bitmap, int w, int h);
    void setText(String text, uint8_t offset);
    void digitFont();
    void display();

};

class AnimatedFrame: public StaticFrame { //subclass of StaticFrame for displaying animated bitmap + text
    private:
    uint16_t nrOfFrames;
    uint16_t frameSpeed;
    
    public:
    AnimatedFrame(unsigned long dur, uint16_t color);  
    void setBitmap(const uint16_t *bitmap, uint16_t nrOfFrames, uint16_t frameSpeed, int w, int h);
    void display();

}; 

#endif

