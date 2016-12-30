#include <stdlib.h>
#define RANDOMIZEALL 0

void setrandomcolors(void)
{
  RGB colortoset;
  colortoset.r = random(256);
  colortoset.g = random(256);
  colortoset.b = random(256);

  for(uint8_t line = 0; line<HEIGHT; line++) //go through all lines (2)
  {
    for(uint8_t column = 0; column<WIDTH; column++) //go through all columns (3x3)
    {
      if(RANDOMIZEALL)
      {
        colortoset.r = random(256);
        colortoset.g = random(256);
        colortoset.b = random(256);
        pixelmatrix.setColor(column, line,colortoset); 
      }
      else
      {
        pixelmatrix.setColor(column, line,colortoset); 
      }
    }
  }
}



void settestpattern(void)
{
  static byte index = 0;
  RGB color =(RGB){
    200,200,0                                } 
  ;
  //        pixelmatrix.setColor(0,0,color);
  pixelmatrix.setColor(index%WIDTH,(index)/WIDTH,color);
  pixelmatrix.setColor((index-1)%WIDTH, (index-1)/WIDTH,(RGB){
    0,0,0                                }
  );
  (index++)%NOOFPIXELS;
  /*
RGB color =(RGB){
   0,0,0} ;
   byte counter = 10;
   
   for(uint8_t line = 0; line<HEIGHT; line++) //go through all lines (9)
   {
   
   for(uint8_t column = 0; column<WIDTH; column++) //go through all columns (16x3)
   {
   color.g = counter;
   // color.g = counter;
   counter+=30;      
   // if(column%2)  pixelmatrix.setColor(column, line,color1); 
   pixelmatrix.setColor(column, line,color);
   }
   
   }
   */
}



RGB calculatefadetocolor( uint8_t fadestep,  uint8_t totalfadesteps,  uint8_t c,  uint8_t l)
{
  RGB returncolor;
  float factor =  (float)(fadestep)/(float)totalfadesteps;
  returncolor.r = (uint8_t)(colorbuffer[c][l][0].r - (factor)*((int)colorbuffer[c][l][0].r - (int)colorbuffer[c][l][1].r));
  returncolor.g = (uint8_t)(colorbuffer[c][l][0].g - (factor)*((int)colorbuffer[c][l][0].g - (int)colorbuffer[c][l][1].g));
  returncolor.b =(uint8_t)(colorbuffer[c][l][0].b - (factor)*((int)colorbuffer[c][l][0].b - (int)colorbuffer[c][l][1].b));  
  return returncolor;
}



void copyendbuffertostartbuffer(byte c, byte l)
{
  //copy current color to start-colorbuffer
  colorbuffer[c][l][0] =  colorbuffer[c][l][1];   //copy endcolor to new startcolor

} 

void fadecolors(void) 
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
      colorbuffer[col][line][1].r = random(255);
      colorbuffer[col][line][1].g = random(255);
      colorbuffer[col][line][1].b = random(255);
      //set startcolor to black
      colorbuffer[col][line][0].r = 0;
      colorbuffer[col][line][0].g = 0;
      colorbuffer[col][line][0].b =0;
    }
  }
  for( j = 0; j<(NOOFPIXELS); j++)
  {
    if( currentfadestep[j] >=  totalfadesteps[j]) //fade is done, set new end color, restet currentfadestep, set new totalfadestep
    {
      currentfadestep[j] = 0;
      totalfadesteps[j] = random(200)+50;
      byte col = j%WIDTH;
      byte line = j/WIDTH;

      copyendbuffertostartbuffer(col, line);
      //now set new endbuffer color
      colorbuffer[col][line][1].r = random(255);
      colorbuffer[col][line][1].g = random(255);
      colorbuffer[col][line][1].b = random(255);
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

void fadeall(void)
{

  static byte totalfadesteps; //maximum fade steps for each pixel
  static byte currentfadestep; //current fadestep for each pixel
  static byte initializefade = 1;
  byte j;

  if(initializefade)
  {   
    byte red = random(255);
    byte green = random(255);
    byte blue = random(255);

    initializefade = 0;
    for( j = 0; j<NOOFPIXELS; j++)
    {
      totalfadesteps = 200;
      currentfadestep = 0;
      byte col = j%WIDTH;
      byte line = j/WIDTH;
      //now set new endbuffer color
      colorbuffer[col][line][1].r = red;
      colorbuffer[col][line][1].g = green;
      colorbuffer[col][line][1].b = blue;
      //set startcolor to black
      colorbuffer[col][line][0].r = 0;
      colorbuffer[col][line][0].g = 0;
      colorbuffer[col][line][0].b =0;
    }
  }

  if( currentfadestep >=  totalfadesteps) //fade is done, set new end color, restet currentfadestep, set new totalfadestep
  {
    byte red = random(255);
    byte green = random(255);
    byte blue = random(255);

    for( j = 0; j<(NOOFPIXELS); j++)
    {
      currentfadestep = 0;
      totalfadesteps = random(200)+50;
      byte col = j%WIDTH;
      byte line = j/WIDTH;

      copyendbuffertostartbuffer(col, line);
      //now set new endbuffer color
      colorbuffer[col][line][1].r = red;
      colorbuffer[col][line][1].g = green;
      colorbuffer[col][line][1].b = blue;
    }
  }




  //fade all colors towards endcolor
  RGB result; //resulting color that will be sent to pixel


  for( j = 0; j<NOOFPIXELS; j++)
  {
    byte col = j%WIDTH;
    byte line = j/WIDTH;
    result = calculatefadetocolor(currentfadestep,  totalfadesteps,col, line);
    currentfadestep++;
    pixelmatrix.setColor(col,  line, result);
  }

}


void rainbow(void) 
{
 
 //initilization is only done once
  byte j;

  if(initializerainbow==1)
  {
   
    initializerainbow = 0;
    byte increment = 255/random(4,13); //divide hue into 4 to 12 parts parts (=12 pixels for full rainbow)
    byte totalfadesteps = 150; //maximum fade steps for fade in
    byte currentfadestep = 0; //current fadestep 

    // initialize the secondary (fade to) buffer: 

    //initialize rainbow (first round)

    for( j = 0; j<NOOFPIXELS; j++)
    {
      colorbuffer[j%WIDTH][j/WIDTH][0].r = increment*(j%WIDTH)+j*4; //initialize hue to same color for same column
      colorbuffer[j%WIDTH][j/WIDTH][0].g = 0; //initialize substep to 0     
    }

    //generate first frame and put it to secondary buffer:
    for( j = 0; j<NOOFPIXELS; j++)
    {
      RGB result; 
      byte col = j%WIDTH;
      byte line = j/WIDTH;

      result = fadeHSV(col, line);
      colorbuffer[col][line][1].r = result.r;
      colorbuffer[col][line][1].g = result.g;
      colorbuffer[col][line][1].b = result.b;
    }

    //reset primary buffer to 0:
    for( j = 0; j<NOOFPIXELS; j++)
    {
      colorbuffer[j%WIDTH][j/WIDTH][0].r = 0;
      colorbuffer[j%WIDTH][j/WIDTH][0].g = 0; 
      colorbuffer[j%WIDTH][j/WIDTH][0].b = 0; 
    }

    //now fade 199 steps towards first frame:
    for( byte i  = 0; i<149; i++)
    {
      //fade all colors towards endcolor
      for(  j = 0; j<NOOFPIXELS; j++)
      {
        RGB result; //resulting color that will be sent to pixel
        byte col = j%WIDTH;
        byte line = j/WIDTH;
        result = calculatefadetocolor(currentfadestep,  totalfadesteps,col, line);
        pixelmatrix.setColor(col,  line, result);
      }
      currentfadestep++;
      pixelmatrix.sendColors();
      _delay_ms(40); 
    }
  

    //now reset primary rgb buffer for rainbow action so first frame can be created for good this time:
    for( j = 0; j<NOOFPIXELS; j++)
    {
      colorbuffer[j%WIDTH][j/WIDTH][0].r = increment*(j%WIDTH)+j*4; //initialize hue to same color for same column
      colorbuffer[j%WIDTH][j/WIDTH][0].g = 0; //initialize substep to 0     
    }

    //now go on and rainbow away!


  }


  //fade all colors to new hue
  for( j = 0; j<NOOFPIXELS; j++)
  {
    RGB result; //resulting color that will be sent to pixel
    byte col = j%WIDTH;
    byte line = j/WIDTH;

    result = fadeHSV(col, line);
    pixelmatrix.setColor(col,  line, result);

  }

}





















