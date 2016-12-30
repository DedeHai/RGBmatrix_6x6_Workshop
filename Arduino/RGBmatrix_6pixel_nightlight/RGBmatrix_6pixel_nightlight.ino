
/*
6x2 matrix using the pixelmatrix library
*/

#include "RGBpixelmatrix.h"
#include <avr/delay.h>

#define WIDTH 30 //matrix has 6 columns
#define HEIGHT 2 //and two lines
#define NOOFPIXELS WIDTH*HEIGHT
#define F_CPU 16000000UL 

RGBpixelmatrix pixelmatrix(WIDTH, HEIGHT); //create a RGBpixelmatrix

RGB colorbuffer[WIDTH][HEIGHT][2]; //backup buffer for fading color manipulation (6*2), (startcolor, endcolor)
#define BAUDRATE 115200

byte mode =1;

 static byte initializerainbow = 1; 
 
void setup()
{
 Serial.begin(BAUDRATE);
   pinMode(7, OUTPUT);
  randomSeed(analogRead(A0));
  mode = random(3);
 pinMode(4,INPUT);
 digitalWrite(4, HIGH); //pullup
 Serial.println("setup done");

}                    

void loop()
{
  //settestpattern();
  switch (mode) {
  case 0:
     fadecolors(); //fade each to random color at random speed
      _delay_ms(40);
    break;
  case 1:
    rainbow(); //rainbow pattern
     _delay_ms(8);
    break;
  case 2:
    fadecolorsHSV(); //fade all colors to same random color
     _delay_ms(25);
    break;
  default:
  mode = 0;
   break;
  }
 
  pixelmatrix.sendColors();
  
static uint8_t oneshot = 0;

if(digitalRead(4) == 0 && oneshot == 0)
{
  oneshot = 1;
  initializerainbow = 1; 
  mode++;
}
else if(digitalRead(4) == 1 && oneshot == 1)
{
  oneshot = 0;
}
 


}



