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


void similarcolors(void) 
{

  static byte totalfadesteps[NOOFPIXELS]; //maximum fade steps for each pixel
  static byte currentfadestep[NOOFPIXELS]; //current fadestep for each pixel
  byte j;
  RGB result;
  static byte hue = random(256);
  

if(modechangerequest == 2)
{
 modechangerequest=0;
 hue += 30;
}
  if(initmode)
  {
    initmode = 0;
    for( j = 0; j<NOOFPIXELS; j++)
    {
      totalfadesteps[j] = 200;
      currentfadestep[j] = 0;
      byte col = j%WIDTH;
      byte line = j/WIDTH;

     // result = hsv_to_rgb (random(256), 255, random(2)*255); //full color only, can be black    
   result =  pixelmatrix.HSVtoRGB(byte(hue+random(20)-random(20)), 255, 100+random(155)); //full color only, can be dark    


      //now set new endbuffer color
      colorbuffer2[col][line].r = result.r;
      colorbuffer2[col][line].g = result.g;
      colorbuffer2[col][line].b = result.b;
      //set startcolor to black
      colorbuffer1[col][line].r = 0;
      colorbuffer1[col][line].g = 0;
      colorbuffer1[col][line].b =0;
    }
  }

  for( j = 0; j<(NOOFPIXELS); j++)
  {
    if( currentfadestep[j] >=  totalfadesteps[j]) //fade is done, set new end color, restet currentfadestep, set new totalfadestep
    {
      currentfadestep[j] = 0;
      totalfadesteps[j] = random(200)+55;
      byte col = j%WIDTH;
      byte line = j/WIDTH;

 
       result =  pixelmatrix.HSVtoRGB(byte(hue+random(25)-random(25)), 255, random(200)+56); //full color only, can be dark


     colorbuffer1[col][line] =  colorbuffer2[col][line];
      //now set new endbuffer color
      colorbuffer2[col][line].r = result.r;
      colorbuffer2[col][line].g = result.g;
      colorbuffer2[col][line].b = result.b;
    }
  }



  //fade all colors towards endcolor
  for( j = 0; j<NOOFPIXELS; j++)
  {
   
    currentfadestep[j]++;
    byte col = j%WIDTH;
    byte line = j/WIDTH;


    result = calculatefadetocolor(currentfadestep[j],  totalfadesteps[j],col, line);
    pixelmatrix.setColor(col,  line, result);


  }

}


void randomfade(void) 
{

  static byte totalfadesteps[NOOFPIXELS]; //maximum fade steps for each pixel
  static byte currentfadestep[NOOFPIXELS]; //current fadestep for each pixel
  static byte initializefade = 1;
  byte j;

  if(initializefade)
  {
    initializefade = 0;
    for( j = 0; j<NOOFPIXELS; j++)
    {
      totalfadesteps[j] = 200;
      currentfadestep[j] = 0;
      byte col = j%WIDTH;
      byte line = j/WIDTH;
      //now set new endbuffer color
      colorbuffer2[col][line].r = random(255);
      colorbuffer2[col][line].g = random(255);
      colorbuffer2[col][line].b = random(255);
      //set startcolor to black
      colorbuffer1[col][line].r = 0;
      colorbuffer1[col][line].g = 0;
      colorbuffer1[col][line].b =0;
    }
  }
  for( j = 0; j<(NOOFPIXELS); j++)
  {
    if( currentfadestep[j] >=  totalfadesteps[j]) //fade is done, set new end color, restet currentfadestep, set new totalfadestep
    {
      currentfadestep[j] = 0;
      totalfadesteps[j] = random(200)+30;
      byte col = j%WIDTH;
      byte line = j/WIDTH;
      //copy end buffer to start buffer
      colorbuffer1[col][line] =  colorbuffer2[col][line];

      //now set new endbuffer color
      colorbuffer2[col][line].r = random(255);
      colorbuffer2[col][line].g = random(255);
      colorbuffer2[col][line].b = random(255);
    }
  }



  //fade all colors towards endcolor
  for( j = 0; j<NOOFPIXELS; j++)
  {
    RGB result; //resulting color that will be sent to pixel
    currentfadestep[j]++;
    byte col = j%WIDTH;
    byte line = j/WIDTH;


    result = calculatefadetocolor(currentfadestep[j],  totalfadesteps[j],col, line);
    pixelmatrix.setColor(col,  line, result);


  }

}


//returns color for fading from buffer1 to buffer2
RGB calculatefadetocolor( uint8_t fadestep,  uint8_t totalfadesteps,  uint8_t c,  uint8_t l)
{
  RGB returncolor;
  float factor =  (float)(fadestep)/(float)totalfadesteps;
  returncolor.r = (uint8_t)(colorbuffer1[c][l].r - (factor)*((int)colorbuffer1[c][l].r - (int)colorbuffer2[c][l].r));
  returncolor.g = (uint8_t)(colorbuffer1[c][l].g - (factor)*((int)colorbuffer1[c][l].g - (int)colorbuffer2[c][l].g));
  returncolor.b =(uint8_t)(colorbuffer1[c][l].b - (factor)*((int)colorbuffer1[c][l].b - (int)colorbuffer2[c][l].b));  
  return returncolor;
}




