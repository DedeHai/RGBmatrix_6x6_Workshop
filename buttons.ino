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



void buttonSetup(void)
{
  pinMode(BUTTONPIN_A,INPUT); //mode change button (mode+1)
  digitalWrite(BUTTONPIN_A, HIGH); //pullup
  pinMode(BUTTONPIN_B,INPUT); //setting button (change mode setting if available)
  digitalWrite(BUTTONPIN_B, HIGH); //pullup
  pinMode(BUTTON_GND,OUTPUT); //GND for buttons
  digitalWrite(BUTTON_GND, LOW); //set it low (GND)
  //interrupt setup 
  pciSetup(BUTTONPIN_A);
  pciSetup(BUTTONPIN_B);
}

//Pin-change interrupt enable
void pciSetup(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

//Interrupt function is called when input pin voltage changes (low to high or high to low)
ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
{    

  //check which button caused the interrupt (which one has been pressed?)
  _delay_ms(20); //simple way to reject noise: wait until button has settled. Can not use standard delay() inside iterrupt!
  
  //read buttons:
  if(digitalRead(BUTTONPIN_A) == 0)
  {
    modechangerequest = 1;
  }

  if(digitalRead(BUTTONPIN_B) == 0 )
  {
    modechangerequest = 2;
  }
  

}


ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here
{
  //not used
}  

ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
{
  //not used
}  







