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



#include <ffft.h> //fft library by Elm Chan
#include <math.h>

#define ANALOG_PIN 0

uint16_t      spectrum[FFT_N/2]; //output of FFT    
int16_t       rawdata[FFT_N];    //ADC samples here
complex_t     butterfly[FFT_N];  //butterfly buffer for FFT
volatile byte writeposition = 0; 
uint8_t basehue;

float bins[6];   //spectrum bins


void spectrumsetup() {
  analogRead(ANALOG_PIN); //initialize pin to analog mode
  //Initialize ADC: free-run mod on channel 'ANALOG_PIN'
  ADMUX  = ANALOG_PIN | _BV(REFS1)| _BV(REFS0); //right adjust, use internal voltage reference (1.2V)                                                                                                                                                                                                                                                                                                                                           as ref
  ADCSRA = _BV(ADEN)  |  _BV(ADSC)  |  _BV(ADATE) |  _BV(ADIE)  |  _BV(ADPS2) | _BV(ADPS1);// prescaler: 64, 13 clocks to sample so: 16MHz/64 / 13 = 19.2 kHz
  ADCSRB = 0;     //no triggering
  pinMode(MICPIN_VCC,OUTPUT); //+5V for microphone
  digitalWrite(MICPIN_VCC, HIGH); //set it high (5V)
  pinMode(MICPIN_GND,OUTPUT); //GND for microphone
  digitalWrite(MICPIN_GND, LOW); //set it low (GND)
  
  TIMSK0 = 0;    // Timer0 off, dont forget to switch on afterwards! will disable millis() and other stuff to work properly
  sei(); //interrupts on
  
  basehue = random(255);
  
}


void spectrumdisplay() {
  
  uint8_t  i, k;
  int      sum;
   ADCSRA |= _BV(ADIE);      //start sampling, sample while not doing anything else to reduce noise (it's still fast enough)
  while(ADCSRA & _BV(ADIE)); //wait for interrupt to finish its work

  /*
  Serial.println( "raw");
   for(i=0; i<FFT_N; i++)
   {
   Serial.print( rawdata[i]);
   Serial.print(", ");
   _delay_ms(5);
   }*/

//do some FFT magic:
  fft_input(rawdata, butterfly);   
  writeposition = 0;                   
  fft_execute(butterfly);         
  fft_output(butterfly, spectrum);

  //subtract some noise from spectrum:
  for(k=0; k<FFT_N/2; k++) {
    if(spectrum[k] <= 3) spectrum[k]=0;
    else spectrum[k] = spectrum[k]-3;
  }
  /*
  Serial.println( "bins");
   for(i=0; i<64; i++)
   {
   Serial.print( spectrum[i]);
   Serial.print(", ");
   _delay_ms(5);
   }*/

  //Clear matrix
  pixelmatrix.clear();

  uint8_t average = 1;  //number of output data to average
  uint8_t currentspec = 2; //current output data bin of raw spectrum
  uint8_t outputBin = 0; //output bin to write data to
  

  for(k=0; k<6;k++) {
    for(sum=0, i=0; i<average; i++)
    {
      sum += spectrum[currentspec++]; 
      if(currentspec>=FFT_N/2) break;
    }
    float  soundvalue = (float)sum/average;
    
    //convert to "dB" (logarithmic scale)
     soundvalue = 10 *  log10(soundvalue);

    //new sound value?
    if(bins[outputBin] < soundvalue)   bins[outputBin] = soundvalue;
    else
    {
   //fade out:
    bins[outputBin] =0.9*bins[outputBin];   
    }

    //draw colors of bar:
    uint8_t i = 0; 
 
      for(i=0; i<(uint8_t)bins[outputBin] ; i++)
      {
        if(i<HEIGHT) //do not write out of matrix boundaries (or it can crash!)
        {
          pixelmatrix.setColor(k, i, pixelmatrix.HSVtoRGB(basehue-(15*i),255,255)); //draw dots with changing colors depending on height    
        }
      }
    outputBin++;
    average =average*1.7; //exponential averaging

  }

  pixelmatrix.sendColors();
  
  if(modechangerequest == 2) 
  {
    basehue = random(255);
   modechangerequest = 0; 
  }


}



//deinitializes spectrum, put interrupts the way they were:
void spectrumcleanup() {

  ADCSRA = 0; //disable ADC
  TIMSK0 = 1<<TOIE0;               // Timer0 interrupt reenabled (used in millis() and delay() function!)

}



//ADC interrupt:
ISR(ADC_vect) { 
     
  rawdata[writeposition] = ADC; //read sample from ADC
  if(++writeposition >= FFT_N) ADCSRA &= ~_BV(ADIE); // Buffer full, interrupt off
   
}

