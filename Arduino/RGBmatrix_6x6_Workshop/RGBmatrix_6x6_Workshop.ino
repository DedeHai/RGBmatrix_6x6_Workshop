/**
 * Copyright (c) Damian Schneider 2015
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */


/*

 Program for the 6x6 pixels RGB matrix using my Pixelmatrix library.
 
 It supports a microphone for spectrum display and an RTC clock to display time.
 The font display supports scrolling of a string, so any message can be displayed. For long messages, it may need to be modified to a single color per char.
 GOL is the Conways game of life with added color-genetics.
 Spectrum displays the audio frequency spectrum read from the microphone using Fourier Transformation.
 Colorfade contains functions to fade colors of pixels.
 Fire displays a digital random fire based on an algorithm.
 Heat is a simplified finite-elements-simulation of the physics of heat transfer.
 Rainbow was initially a color test function making use of the color fade functions to display a nice color spectrum.
 
 https://github.com/DedeHai/
 
 */



#include "RGBpixelmatrix.h" //Pixel Matrix library
#include <util/delay.h>
#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <Time.h>         //http://www.arduino.cc/playground/Code/Time  
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)
#include "blocks.h" //struct definition of tetris block

#define WIDTH 6//matrix has 6 columns (direction of led strip ) number of leds per strip)
#define HEIGHT 6 //and 6 lines (number of led strips)
#define NOOFPIXELS WIDTH*HEIGHT
#define BUTTONPIN_A 8 //primärer schalter an diesem pin
#define BUTTONPIN_B 9 //sekundärer schalter an diesem pin
#define BUTTON_GND 10 //ground für die buttons an diesem pin

#define MICPIN_VCC 15 //5V pin für mikrofon (A1)
#define MICPIN_GND 16 //GND pin für mikrofon (A2)

#define BAUDRATE 115200 //Serial in/out

RGBpixelmatrix pixelmatrix(WIDTH, HEIGHT); //create a RGBpixelmatrix, output on pin7
RGB colorbuffer1[WIDTH][HEIGHT]; //primärer buffer für fading
RGB colorbuffer2[WIDTH][HEIGHT]; //sekundärer buffer für fading

volatile uint8_t modechangerequest = 0; //volatile, is changed in interrupt
uint8_t initmode = 1;
byte mode;
char string[16];// = "~Pixelmatrix~";

void setup()
{
  buttonSetup();



//  Serial.begin(BAUDRATE);

  //setup RTC 
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  /*
  if(timeStatus() != timeSet) 
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");   
  */  
  setSyncInterval(60);    //zeit alle 60 sekunden synchronisieren mit RTC modul

  randomSeed(analogRead(A0)); //read some audio noise from analog input to seed the random generator
  mode = random(6);
  pixelmatrix.clear();

//  Serial.println("6x6 Pixelmatrix");
}                    

void loop()
{

  switch (mode) {

  case 0:


    similarcolors(); //fade each pixel to similar color at random speed
    pixelmatrix.sendColors();
    delay(5);
    if (modechangerequest == 1) //mode will change, fade to black
    {
      pixelmatrix.clear();
      delay(100);
      modechangerequest = 0;
      initmode = 1;
      mode = (mode + 1);
    }
    break;
  case 1:
    rainbow(); //fade each to similar color at random speed
    delay(1);
    pixelmatrix.sendColors();
    if (modechangerequest == 1) //mode will change, fade to black
    {
      pixelmatrix.clear();
      delay(100);
      modechangerequest = 0;
      initmode = 1;
      mode = (mode + 1);
    }
    break;
  case 2:

    randomfade();  //fade each to random color at random speed, HSV full colors only
    delay(10);
    pixelmatrix.sendColors();

    if (modechangerequest == 1) //mode will change, fade to black
    {
      pixelmatrix.clear();
      delay(100);
      modechangerequest = 0;
      initmode = 1;
      mode = (mode + 1);
    }
    break;
  case 3:

    heat();   
    pixelmatrix.sendColors();
    delay(30);

    if (modechangerequest == 1) //mode will change, fade to black
    {
      pixelmatrix.clear();
      delay(100);
      modechangerequest = 0;
      initmode = 1;
      mode = (mode + 1);
    }

    break;

  case 4:
    static int16_t dropdelay = 310;
    dropblock();
    delay(dropdelay);
    pixelmatrix.sendColors();

    if (modechangerequest == 2) //mode will change, fade to black
    {
      modechangerequest = 0;
      dropdelay -= 50;
      if(dropdelay < 0) dropdelay =500;
    }

    if (modechangerequest == 1) //mode will change, fade to black
    {
      pixelmatrix.clear();
      delay(100);
      modechangerequest = 0;
      initmode = 1;
      mode = (mode + 1);
    }
    break;

  case 5:

    if(initmode)
    {
      initmode = 0;
      spectrumsetup();
      modechangerequest = 0;
    }
    else
    {
      spectrumdisplay();
    }
    if (modechangerequest == 1) //mode will change, fade to black
    {
      //set interrupts back to normal: 
      spectrumcleanup();
      pixelmatrix.clear();
      delay(100);
      modechangerequest = 0;
      initmode = 1;
      mode = (mode + 1);
    }
    break;

  case 6:
    RGB fontcolor;
    RGB strcolors[sizeof(string)];
    // sprintf(string, "%.2d:%.2d:%.2d ", hour(), minute(), second());
    sprintf(string, "%.2d:%.2d ", hour(), minute());
    fontcolor =  pixelmatrix.HSVtoRGB(random(255),190,250);
    for(byte i = 0; i<sizeof(string); i++)
    {
      strcolors[i] = fontcolor;
    }
    RGB background;
    background.r =255*0;
    background.g =255*.05;
    background.b= 255*0;


    scrolltext(string, strcolors,  background); //function scrolls the text, returns when finished or on modechangerequest

    if (modechangerequest == 1) //mode will change, fade to black
    {
      pixelmatrix.clear();
      delay(100);
      modechangerequest = 0;
      initmode = 1;
      mode = (mode + 1);
    }

    break;



  case 7:
    fire();
    pixelmatrix.sendColors();

    if (modechangerequest == 1) //mode will change, fade to black
    {
      pixelmatrix.clear();
      delay(100);
      modechangerequest = 0;
      initmode = 1;
      mode = (mode + 1);
    }
    break;


  case 8:

    gameOfLife();

    pixelmatrix.sendColors();

    if (modechangerequest == 1) //mode will change, fade to black
    {
      pixelmatrix.clear();
      delay(100);
      modechangerequest = 0;
      initmode = 1;
      mode = (mode + 1);
    }
    break;

  default:
    mode = 0;
    break;
  }



}


















