/**
   Copyright (c) Damian Schneider 2015

   This program is free software: you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free Software
   Foundation, either version 3 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
   details.

   You should have received a copy of the GNU General Public License along with
   this program. If not, see <http://www.gnu.org/licenses/>.
*/

//Die Echtzeituhr (RTC) läuft mit UTC (GMT) zeit. Die definition der Zeitzone kann hier geändert werden (darf negativ sein)
//Die Uhr stellt automatisch zwischen Sommer- und Winterzeit um (europäische sommerzeit)


#define TIMEZONE 1

//check if the input is in european summer or winter time (daylight saving time checker)
boolean isSummertime(tmElements_t &tm)
{
  if (tm.Month < 3 || tm.Month > 10) return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
  if (tm.Month > 3 && tm.Month < 10) return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep

  //Umstellung auf Sommerzeit am letzten Sonntag im März (Wday ist Wochentag, sonntag ist 1)
  if (tm.Month == 3)
  {
    if ((tm.Day - (tm.Wday - 1) >= 25) && tm.Hour > 1) return true;
    else return false;
  }
  else if (tm.Month == 10)
  {
    if ((tm.Day - (tm.Wday - 1) >= 25) && tm.Hour > 2) return false;
    else return true;
  }

  return false; //line should never be reached
}

void displayClock()
{
  static uint8_t backgroundhue = 0;
  RGB strcolors[sizeof(string)];

  sprintf(string, "%.2d:%.2d ", hour(), minute());
  // sprintf(string, "%.2d:%.2d:%.2d ", hour(), minute(), second()); //zeit inkl. sekunden
  uint8_t fonthue = random(255);
  for (byte i = 0; i < sizeof(string); i++)
  {
    strcolors[i] = pixelmatrix.HSVtoRGB(fonthue, 250, 250);
    fonthue+=5;
  }


  backgroundhue++;
  RGB background = pixelmatrix.HSVtoRGB(backgroundhue, 250, 5);
  scrolltext(string, strcolors,  background); //function scrolls the text, returns when finished or on modechangerequest

}

//get UTC time from RTC and offset the timezone and summertime
time_t getRTCtime(void)
{
  time_t RTCtime = RTC.get() + (TIMEZONE) * 3600 ;
  tmElements_t localTime;
  breakTime(RTCtime, localTime); //break the Time into year, month, day, etc.
  if (isSummertime(localTime)) //check if the time in the timezone is summer or winter time
  {
    RTCtime += 3600; //add an hour if summer time
  }
  return RTCtime ;

}
