#include <Frame.h>

frameData weatherBitmap(uint16_t weatherId) {  //function to return data struct of bitmap and nr of frames given the weatherId from openweathermap
    frameData data;

    if( hour() >= 20 || hour() <= 3 ) { 
      //moon
        data.bitmap = *nightTime;
        data.frameCount = sizeof nightTime / sizeof nightTime[0];
        return data;
    }


    if(weatherId == 800) { 
     // sun with cloud
        data.bitmap = *suncloudAnimation;
        data.frameCount = sizeof suncloudAnimation / sizeof suncloudAnimation[0];
        return data;
    }

    
    if(weatherId == 801) { 
        //few clouds
        data.bitmap = *suncloudAnimation;
        data.frameCount = sizeof suncloudAnimation / sizeof suncloudAnimation[0];
        return data;
      
    }

    
    if(weatherId > 801 ) { 
        //cloudy
        data.bitmap = *sunCloud;
        data.frameCount = sizeof sunCloud / sizeof sunCloud[0];
        return data;
      
    }
    

    if(weatherId >= 300 && weatherId <= 531) { 
        // rain  
        data.bitmap = *rainAnimation;
        data.frameCount = sizeof rainAnimation / sizeof rainAnimation[0];
        return data;
    }

    
    if(weatherId >= 200 && weatherId <= 232) {
        //thunder
        data.bitmap = *lightningBolt;
        data.frameCount = sizeof lightningBolt / sizeof lightningBolt[0];
        return data;
    }

    
    if(weatherId >= 600 && weatherId <= 622) {
        //snow
        data.bitmap = *snowFlake;
        data.frameCount = sizeof snowFlake / sizeof snowFlake[0];
        return data;
    }
    
    

   data.bitmap = *empty;
   data.frameCount = 1;
   return data;
}


//-----------------------------------------------------------------------------


void Frame::setDuration(unsigned long dur)
{
    this->dur = dur;
}

void Frame::setColor(uint16_t color)
{
    this->color = color;
}

void Frame::BrightnessCheck() {
    delay(50);
    int val = analogRead(sensorPin); 
	if(val < 300) {val = 25;}
    else if (val > 300 && val < 800) {val = 35;}
    else {val = 45;}
    // val = 10 * exp((1.6e-3) * val); 
	matrix->setBrightness(val);
	matrix->show();
}


//----------------------------------------------------------------------------

TimeFrame::TimeFrame(unsigned long dur, uint16_t color) {
    this->dur = dur;
    this->color = color;
    }

void TimeFrame::displayTime_full(int8_t offset) // displays formatted time, needs special formatting due to pzim3x5 having large spacing between characters,
{                                               // offset is needed for slide/out in animation
    matrix->clear();
    matrix->setCursor(1, 6 - offset);
    matrix->print(printDigits(hour()));
    matrix->writePixel(10, 3 - offset, color);
    matrix->writePixel(10, 5 - offset, color);
    matrix->setCursor(12, 6 - offset);
    matrix->print(printDigits(minute()));
    matrix->writePixel(21, 3 - offset, color);
    matrix->writePixel(21, 5 - offset, color);
    matrix->setCursor(23, 6 - offset);
    matrix->print(printDigits(second()));
    matrix->show();
}

//function to display seconds, minutes, hours seperately available in case needed for something else

void TimeFrame::displayTime_seconds(int8_t offset) 
{
    matrix->fillRect(23, 1, 8, 6, 0x0000);
    matrix->setCursor(23, 6 - offset);
    matrix->print(printDigits(second()));
}

void TimeFrame::displayTime_minutes(int8_t offset) 
{
    matrix->fillRect(12, 1, 8, 6, 0x0000);
    matrix->setCursor(12, 6 - offset);
    matrix->print(printDigits(minute()));
}

void TimeFrame::displayTime_hours(int8_t offset) 
{
    matrix->fillRect(1, 1, 8, 6, 0x0000);
    matrix->setCursor(1, 6 - offset);
    matrix->print(printDigits(hour()));
}

void TimeFrame::display()
{
    uint8_t prevSec = 0;
    uint8_t offset = 0;

    matrix->clear();
    matrix->setFont(&pzim3x5);
    matrix->setTextColor(color);

    for (int i = 8; i >= 0; i--)                // prints formatted time with decreasing offset -> time slides down from above (slide in animation)
    {
        displayTime_full(i);
        delay(50);
    }

    unsigned long prevtime = millis();

    while (millis() - prevtime <= dur * 1000)   //loop this for dur seconds
    { 
        if (second() != prevSec)
        {                                       //update the display only if time has changed
            prevSec = second();
            displayTime_full(offset);
    
        }

        delay(100);
        BrightnessCheck();

    }

    for (int i = 0; i <= 8; i++)                //prints formatted time with increasing offset -> time slides down out of view
    {
        displayTime_full(-i);
        delay(50);
    }
}

//------------------------------------------------------

StaticFrame::StaticFrame(unsigned long dur, uint16_t color){
    this->dur = dur;
    this->color = color;
    bitmap = 0;
    x = 0;
    y = 0;
    w = 0;
    h = 0;
}

void StaticFrame::setBitmap(const uint16_t *bitmap, int w, int h) {
    this->bitmap = bitmap;
    this->w = w;
    this->h = h;
}

void StaticFrame::setText(String text, uint8_t textOffset) {
    this->text = text;
    this->textOffset = textOffset;
}

void StaticFrame::digitFont() {
    this->f = &pzim3x5;
    dFont = true;
}

void StaticFrame::display() { 

    uint8_t d = 1;

    if(dFont){d = 6;}                                   //set offset needed for pzim3x5 font due to it being 5 steps offset above.

    matrix->clear();
    matrix->setFont(f);
    matrix->setTextColor(color);

    for(int i = h; i >= 0; i--)                         //prints bitmap + text with increasing offset -> time slides down from above (slide in animation)
    {   
        matrix->clear();
        matrix->drawRGBBitmap(x, y-i, bitmap, w, h);
        matrix->setCursor(w+textOffset, d+i);
        matrix->print(text);
        matrix->show();
        delay(50);
    }

    unsigned long prevtime = millis();

    while (millis() - prevtime <= dur * 1000)           //loop this for dur seconds
    { 
        BrightnessCheck();
        delay(150);
    }
    

    for(int i = 0; i <= h; i++)                         //prints bitmap + text with increasing offset -> time slides down out of view (slide out animation)
    {
        matrix->clear();
        matrix->drawRGBBitmap(x, y+i, bitmap, w, h);
        matrix->setCursor(w+textOffset, d-i);
        matrix->print(text);
        matrix->show();
        delay(50);
    }


}

//------------------------------------------------------------------

AnimatedFrame::AnimatedFrame(unsigned long dur, uint16_t color) : StaticFrame(dur, color) //use same constructor as StaticFrame
{

}

void AnimatedFrame::setBitmap(const uint16_t *bitmap, uint16_t nrOfFrames, uint16_t frameSpeed, int w, int h) {
    this->bitmap = bitmap;
    this->nrOfFrames = nrOfFrames;
    this->frameSpeed = frameSpeed-50; //brightnessCheck has delay of 50ms already
    if(this->frameSpeed < 50 ) 
    {
        this->frameSpeed = 50;
    }                           
    this->w = w;
    this->h = h;
}

void AnimatedFrame::display(){
    uint8_t d = 0;
    int pstep = w*h;                                            //pointer step to next bitmap
    if(dFont){d = 6;}                                           //offset needed for pzim3x5

    matrix->clear();
    matrix->setFont(f);
    matrix->setTextColor(color);

    for(int i = h; i >= 0; i--)                                 //slide in animation
    {   
        matrix->clear();
        matrix->drawRGBBitmap(x, y-i, bitmap, w, h);
        matrix->setCursor(w+textOffset,d+i);
        matrix->print(text);
        matrix->show();
        delay(50);
    }

    long prevtime = millis();
    int j = 0;

    while(millis() - prevtime <= dur*1000){                     //loop this for dur seconds
        matrix->fillRect(x, y, w, h, 0x0000);
        matrix->drawRGBBitmap(x, y,(bitmap + pstep*j), w, h);   //animation is pointer to 2-dim array, use pointer arithmetic to access elements.
        matrix->show();
        j = (j+1) % nrOfFrames;
        delay(frameSpeed);                                      // set duration as argument in setAnimation later
        BrightnessCheck();
    }


    for(int i = 0; i <= h; i++)                                 //slide out animation
    {
        matrix->clear();
        matrix->drawRGBBitmap(x, y+i, bitmap, w, h);
        matrix->setCursor(w+textOffset,d-i);
        matrix->print(text);
        matrix->show();
        delay(50);
    }

}

