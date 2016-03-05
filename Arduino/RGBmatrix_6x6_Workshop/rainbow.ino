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
 * rainbow color test function 
 * written by Damian Schneider, February, 2013
 */


void rainbow(void) 
{
  byte c,l;
  int j;
  if(initmode==1)
  {

    initmode = 0;
    byte increment = 255/random(7,18); //divide hue into 4 to 12 parts parts (=12 pixels for full rainbow)
    byte totalfadesteps = 150; //maximum fade steps for fade in
    byte currentfadestep = 0; //current fadestep 

    //initialize rainbow (first round)
       for( j = 0; j<NOOFPIXELS; j++)
    {
      colorbuffer1[j%WIDTH][j/WIDTH].r = increment*(j%WIDTH)+j*2; //initialize hue to same color for same column
      colorbuffer1[j%WIDTH][j/WIDTH].g = 0; //initialize substep to 0     
    }

    //generate first frame and put it to secondary buffer:
    RGB result;     
    for (c = 0; c < WIDTH; c++) {
      for (l = 0; l < HEIGHT; l++) {

        result = fadeHSV(c, l);
        colorbuffer2[c][l].r = result.r;
        colorbuffer2[c][l].g = result.g;
        colorbuffer2[c][l].b = result.b; 


      }
    }
    for (c = 0; c < WIDTH; c++) {
      for (l = 0; l < HEIGHT; l++) {

        //reset primary buffer to 0 (to fade in from black):
        colorbuffer1[c][l].r = 0;
        colorbuffer1[c][l].g = 0;
        colorbuffer1[c][l].b = 0; 

      }
    }
    //now fade  steps towards first frame:
    for( byte i  = 0; i<149; i++)
    {
      //fade in from black:
      for (c = 0; c < WIDTH; c++) {
        for (l = 0; l < HEIGHT; l++) {
          RGB result; //resulting color that will be sent to pixel
          result = calculatefadetocolor(currentfadestep,  totalfadesteps,c, l);
          pixelmatrix.setColor(c,  l, result);
        }
         if(modechangerequest == 1) break;
      }
      currentfadestep++;
      pixelmatrix.sendColors();
      delay(12); 
    }


    //now reset primary rgb buffer for rainbow action so first frame can be created for good this time:
        for( j = 0; j<NOOFPIXELS; j++)
    {
      colorbuffer1[j%WIDTH][j/WIDTH].r = increment*(j%WIDTH)+j*2; //initialize hue to same color for same column
      colorbuffer1[j%WIDTH][j/WIDTH].g = 0; //initialize substep to 0     
    }
    //now go on and rainbow away!
  }

  //fade all colors to new hue
  for (c = 0; c < WIDTH; c++) {
    for (l = 0; l < HEIGHT; l++) {
      RGB result; //resulting color that will be sent to pixel
      result = fadeHSV(c, l);
      pixelmatrix.setColor(c,  l, result);   
    }
 if(modechangerequest == 1) break;
  }
}


//function returns new faded RGB value. initialize primary buffer red channel to desired start hue
RGB fadeHSV(byte column, byte line) //fades through colors in HSV, goes around hsv-circle, returns new rgb value of current pixel for next step
{
  //uses primary rgb buffer for hue fading!
  if(colorbuffer1[column][line].g>9) //use 10 subdivisions for hue
  {
    colorbuffer1[column][line].r++; //increase  hue, primary red buffer is abused for hue 
    colorbuffer1[column][line].g=0; //reset hue substep, primary green buffer is abused for hue sub steps
  }

  RGB result = pixelmatrix.HSVtoRGB((float)colorbuffer1[column][line].r + (float)colorbuffer1[column][line].g/10, 255, 255);    
  colorbuffer1[column][line].g++;//increase sub step

  return result;
}








