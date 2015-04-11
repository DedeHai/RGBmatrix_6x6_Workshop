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
displays small font ascii characters or strings with base at (c/l)
 */
#include "font.h"

#define SCROLLDELAY 90 //delay between pixel scrolling

void displaychar(unsigned char c, int16_t x, int16_t y, RGB color) {
  uint16_t bitmask;
  int16_t xout, yout; //output positions
  uint16_t data;
  uint8_t index = 0; //read index of array

  /*
	 * scan chars from bottom left upward -> bitmask must be used from MSB to LSB
   	 * each byte represents one vertical line (y direction)
   	 */

  if (c >= 0x80) //extended char set?
  {
    //problem: input is not encoded in code page 850 but char definition in header file is. need to convert extended chars:
    if (c == 0xE4)
      c = 0x84; //
    if (c == 0xF6)
      c = 0x94; //
    if (c == 0xFC)
      c = 0x81; //
    if (c == 0xDF)
      c = 0xE1; //
    if(c>0xE1) c = 0xB1;
  }

  for (xout = x; xout < x + FONTWIDTH; xout++) {
    data = pgm_read_byte(&font[c][index++]); 
    if (xout >= WIDTH)
      break;
    else if (xout < 0)
      continue;

    bitmask = 0x01 << 8; //0x01 << fheight;
    for (yout = y; yout < y + FONTHEIGHT; yout++) {
      bitmask = bitmask >> 1;
      if (yout >= HEIGHT)
        break;
      else if (yout < 0)
        continue;
      if (data & bitmask)
        pixelmatrix.setColor(xout, yout, color);
    }
  }
}

//display a string at x,y (can be negative) with color, fwidth&fheight are character size

void displaystring(const char* str, int16_t x, int16_t y, RGB* colors) {

  uint8_t i = 0;
  unsigned char c = str[i];
  RGB charcolor = colors[i];
  //skip out of picture chars:
  while (x <= -FONTWIDTH && c != '\0') {
    x += FONTWIDTH;
    i++;
    c = str[i];
    charcolor = colors[i];
  }

  while (c != '\0' && x < WIDTH) //scan zero terminated string
  {
    displaychar(c, x, y, charcolor);
    x += FONTWIDTH;
    i++;
    c = str[i];
    charcolor = colors[i];
  }
}

//scroll function:

void scrolltext(const char* str, RGB* colors, RGB background) {
  int16_t x;
  int16_t y = -2;
  uint16_t i,j;
  int16_t limit = -FONTWIDTH * (strlen(str) + 1);

  for (x = WIDTH + 2; x > limit; x--) {
    //set background color:
    for (i = 0; i < WIDTH; i++) {
      for (j = 0; j < HEIGHT; j++) {
        pixelmatrix.setColor(i, j, background);
      }
    }
    //sprintf(string, "%.2d:%.2d:%.2d ", hour(), minute(), second());
    displaystring(string, x, y, colors);
    //  displaystring(str, x, y, colors);
    pixelmatrix.sendColors();
    delay(SCROLLDELAY);
    //  delay(310);
    if (modechangerequest==1)
    {
      break; //stop displaying when modechange is requested
    }

  }
}






