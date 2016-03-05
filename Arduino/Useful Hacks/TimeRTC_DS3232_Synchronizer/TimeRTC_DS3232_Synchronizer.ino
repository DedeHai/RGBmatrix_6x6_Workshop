
/*
 * Original Work by Jack Christensen  08Aug2013                         *
 * Added functionality for easier use by Damian Schneider, 2014         *
 *                                                                      *
 * This work is licensed under the Creative Commons Attribution-        *
 * ShareAlike 3.0 Unported License. To view a copy of this license,     *
 * visit http://creativecommons.org/licenses/by-sa/3.0/ or send a       *
 * letter to Creative Commons, 171 Second Street, Suite 300,            *
 * San Francisco, California, 94105, USA.                               *
 *----------------------------------------------------------------------*/ 

/*

RTC synchronizer:
 
 How to use:
 
 Plug in a module with the correct time, reset the Arduino.
 Remove module, plugin another, send 's' over serial monitor 
 and wait 2 seconds, the time will have been 
 synchronized. If no clock is available, set the time manually using the 
 serial monitor:
 
 * Set the date and time by entering the following on the Arduino       *
 * serial monitor:                                                      *
 *    year,month,day,hour,minute,second,                                *
 *                                                                      *
 * Where                                                                *
 *    year can be two or four digits,                                   *
 *    month is 1-12,                                                    *
 *    day is 1-31,                                                      *
 *    hour is 0-23, and                                                 *
 *    minute and second are 0-59.   
 *    example: 2016,3,4,11,43,15
 */

#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <Time.h>         //http://www.arduino.cc/playground/Code/Time  
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)

int inSync = 0;
time_t t;
void setup(void)
{
  Serial.begin(9600);
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus() != timeSet) 
    Serial.println("Unable to sync with the RTC");
  else
  {
    Serial.println("RTC has set the system time. The time is: ");     
    digitalClockDisplay();  
    inSync = 1;        
  }
    setSyncInterval(36000);    //zeit alle 100 minuten synchronisieren mit RTC
  


}

long timestamp=0;
tmElements_t tm;

void loop(void)
{
  if(Serial.available())
  {
    if(Serial.peek() == 's') //synchronize arduino updates external RTC
    {
      while (Serial.available() > 0) Serial.read();     
      if(inSync)
      {
        RTC.set(now());  
        Serial.println("external RTC has been written."); 
      }    
    } 
  }
  if (Serial.available() >= 12) {
    //note that the tmElements_t Year member is an offset from 1970,
    //but the RTC wants the last two digits of the calendar year.
    //use the convenience macros from Time.h to do the conversions.
    int y = Serial.parseInt();
    if (y >= 1000)
      tm.Year = CalendarYrToTm(y);
    else    //(y < 100)
    tm.Year = y2kYearToTm(y);
    tm.Month = Serial.parseInt();
    tm.Day = Serial.parseInt();
    tm.Hour = Serial.parseInt();
    tm.Minute = Serial.parseInt();
    tm.Second = Serial.parseInt();
    t = makeTime(tm);
    RTC.set(t);        //use the time_t value to ensure correct weekday is set
    setTime(t);        
    Serial.print("RTC set.");
    //dump any extraneous input
    while (Serial.available() > 0) Serial.read();     
  }

  //output the time every second:  
  if(millis()-timestamp >= 1000)
  {
    timestamp = millis();
    digitalClockDisplay();  
  }

}

void digitalClockDisplay(void)
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(' ');
  Serial.print(day());
  Serial.print(' ');
  Serial.print(month());
  Serial.print(' ');
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits)
{
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(':');
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

