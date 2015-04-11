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
 * Heat transfer physics simulation with color display 
 * written by Damian Schneider, March, 2013
 * 
 */


uint16_t heatmatrix[2][WIDTH][HEIGHT];

int newheat(uint8_t column, uint8_t line) //returns the next frame heat value
{

  int16_t actualheat = heatmatrix[1][column][line];
  //summed heat difference to all neighbours, negtive means heat is lost
  int16_t delta = ((int16_t) heatmatrix[1][(column + 1) % WIDTH][line]
    - (int16_t) actualheat)
    + ((int16_t) heatmatrix[1][column][(line + 1) % HEIGHT]
      - (int16_t) actualheat)
      + ((int16_t) heatmatrix[1][(column + WIDTH - 1) % WIDTH][line]
        - (int16_t) actualheat)
        + ((int16_t) heatmatrix[1][column][(line + HEIGHT - 1) % HEIGHT]
          - (int16_t) actualheat);
  return constrain(actualheat+delta/15-6,0,8000);

}

RGB heattocolor(uint16_t heatvalue)
{
  static byte colorscheme = random(6);

  RGB returncolor;
  if(modechangerequest == 2)
  {
    modechangerequest=0;
    colorscheme = (colorscheme+1)%6;
  }

  switch (colorscheme) {


  case 0: //RED
    if (heatvalue > 570) {
      returncolor.r = 255;
      returncolor.g = 255;
      returncolor.b = 225 - 164;
      return returncolor;
    } 
    else if (heatvalue > 511) {
      returncolor.r = 255;
      returncolor.g = 255;
      returncolor.b = heatvalue - 512;
      return returncolor;
    } 
    else if (heatvalue > 255) {
      returncolor.r = 255;
      returncolor.g = heatvalue - 256;
      returncolor.b = 0;
      return returncolor;
    } 
    else //fade red to black
    {
      returncolor.r = heatvalue;
      returncolor.g = 0;
      returncolor.b = 0;
      return returncolor;
    }
    break;
  case 1: //green
    if (heatvalue > 630) {
      returncolor.g = 255;
      returncolor.r = 255;
      returncolor.b = 120;
      return returncolor;
    } 
    else if (heatvalue > 511) {
      returncolor.g = 255;
      returncolor.r = 255;
      returncolor.b = heatvalue - 512;
      return returncolor;
    } 
    else if (heatvalue > 255) {
      returncolor.g = 255;
      returncolor.r = heatvalue - 256;
      returncolor.b = 0;
      return returncolor;
    } 
    else //fade red to black
    {
      returncolor.g = heatvalue;
      returncolor.r = 0;
      returncolor.b = 0;
      return returncolor;
    }
    break;

  case 2: //blue
    if (heatvalue > 670) {
      returncolor.g = 255;
      returncolor.r = 255;
      returncolor.b = 161;
      return returncolor;
    } 
    else if (heatvalue > 415) {
      returncolor.b = 161;
      returncolor.r = 255;
      returncolor.g = heatvalue - 416;
      return returncolor;
    } 
    else if (heatvalue > 160) {
      returncolor.b = 161;
      returncolor.r = heatvalue - 160;
      returncolor.g = 0;
      return returncolor;
    } 
    else //fade red to black
    {
      returncolor.b = heatvalue;
      returncolor.r = 0;
      returncolor.g = 0;
      return returncolor;
    }
    break;

  case 3: //RED2
    if (heatvalue > 570) {
      returncolor.r = 255;
      returncolor.g = 255;
      returncolor.b = 225 - 164;
      return returncolor;
    } 
    else if (heatvalue > 511) {
      returncolor.r = 255;
      returncolor.g =  heatvalue - 512;
      returncolor.b = 255;
      return returncolor;
    } 
    else if (heatvalue > 255) {
      returncolor.r = 255;
      returncolor.g = 0;
      returncolor.b = heatvalue - 256;
      return returncolor;
    } 
    else //fade red to black
    {
      returncolor.r = heatvalue;
      returncolor.g = 0;
      returncolor.b = 0;
      return returncolor;
    }
    break;

  case 4: //green2
    if (heatvalue > 630) {
      returncolor.g = 255;
      returncolor.r = 255;
      returncolor.b = 120;
      return returncolor;
    } 
    else if (heatvalue > 511) {
      returncolor.g = 255;
      returncolor.b = 255;
      returncolor.r = heatvalue - 512;
      return returncolor;
    } 
    else if (heatvalue > 255) {
      returncolor.g = 255;
      returncolor.r = 0;
      returncolor.b =  heatvalue - 256;
      return returncolor;
    } 
    else //fade red to black
    {
      returncolor.g = heatvalue;
      returncolor.r = 0;
      returncolor.b = 0;
      return returncolor;
    }
    break;

  case 5: //blue2
    if (heatvalue > 670) {
      returncolor.g = 255;
      returncolor.r = 255;
      returncolor.b = 161;
      return returncolor;
    } 
    else if (heatvalue > 415) {
      returncolor.b = 161;
      returncolor.g = 255;
      returncolor.r = heatvalue - 416;
      return returncolor;
    } 
    else if (heatvalue > 160) {
      returncolor.b = 161;
      returncolor.g = heatvalue - 160;
      returncolor.r = 0;
      return returncolor;
    } 
    else //fade red to black
    {
      returncolor.b = heatvalue;
      returncolor.r = 0;
      returncolor.g = 0;
      return returncolor;
    }
    break;
  default:
    colorscheme = 0;

    break;

  }
 return returncolor;
}




void heat(void)
{
  static uint8_t movingvector = 0; //point where the heat travels to (0 to 7 for all 8 neighbouring pixels)
  static uint8_t heatpoint[2] = { 
    1, 1       }; //point in x, y where heat is applied
  static uint8_t counter = 0;

  if(counter <300)
  {
    heatmatrix[0][heatpoint[0]][heatpoint[1]] += 260; //constrain (heatmatrix[0][heatpoint[0]][heatpoint[1]] + 300,0,5000);
    if (counter % 8 == 0) //move heating point every x frames
    {
      movingvector = (movingvector + rand() % 2 - rand() % 2) % 8;
      movepoint(movingvector, heatpoint);
    }
  }
  counter = (counter+1)%360;
  uint8_t c, l;

  //get new heat
  for (c = 0; c < WIDTH; c++) {
    for (l = 0; l < HEIGHT; l++) {
      heatmatrix[0][c][l] = newheat(c, l);
      //set new color
      pixelmatrix.setColor(c, l, heattocolor(heatmatrix[0][c][l]));
      copyheatbuffer();
    }
  }

}

void copyheatbuffer(void)
{
  for (uint8_t c = 0; c<WIDTH;c++)
  {
    for(uint8_t l = 0; l<HEIGHT; l++)
    {
      heatmatrix[1][c][l] =  heatmatrix[0][c][l];

    }
  } 
}



void movepoint(uint8_t direction, uint8_t* point) {
  /*
	 * moves a point in a direction (0-8)
   	 */

  if (direction == 0) {
    point[0] = (point[0] + 1) % WIDTH;
  } 
  else if (direction == 1) {
    point[0] = (point[0] + 1) % WIDTH;
    point[1] = (point[1] + 1) % HEIGHT;
  } 
  else if (direction == 2) {
    point[1] = (point[1] + 1) % HEIGHT;
  } 
  else if (direction == 3) {
    point[0] = (point[0] + WIDTH - 1) % WIDTH;
    point[1] = (point[1] + 1) % HEIGHT;
  } 
  else if (direction == 4) {
    point[0] = (point[0] + WIDTH - 1) % WIDTH;

  } 
  else if (direction == 5) {
    point[0] = (point[0] + WIDTH - 1) % WIDTH;
    point[1] = (point[1] + HEIGHT - 1) % HEIGHT;
  } 
  else if (direction == 6) {
    point[1] = (point[1] + HEIGHT - 1) % HEIGHT;
  } 
  else if (direction == 7) {
    point[0] = (point[0] + 1) % WIDTH;
    point[1] = (point[1] + HEIGHT - 1) % HEIGHT;
  }

}







