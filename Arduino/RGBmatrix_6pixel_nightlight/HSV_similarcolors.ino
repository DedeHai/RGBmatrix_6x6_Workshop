
void fadecolorsHSV(void) 
{

  static byte totalfadesteps[NOOFPIXELS]; //maximum fade steps for each pixel
  static byte currentfadestep[NOOFPIXELS]; //current fadestep for each pixel
  static byte initializefade = 1;
  byte j;
  RGB result;
  static byte hue = random(256);
  
  static int huechanger = 0;

huechanger++;
if(huechanger > 5000)
{
 huechanger = 0; 
  hue = random(256);
}
  if(initializefade)
  {
    initializefade = 0;
    for( j = 0; j<NOOFPIXELS; j++)
    {
      totalfadesteps[j] = 200;
      currentfadestep[j] = 0;
      byte col = j%WIDTH;
      byte line = j/WIDTH;

     // result = hsv_to_rgb (random(256), 255, random(2)*255); //full color only, can be black    
   result = hsv_to_rgb (byte(hue+random(20)-random(20)), 255, random(255)); //full color only, can be black    


      //now set new endbuffer color
      colorbuffer[col][line][1].r = result.r;
      colorbuffer[col][line][1].g = result.g;
      colorbuffer[col][line][1].b = result.b;
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
      totalfadesteps[j] = random(200)+55;
      byte col = j%WIDTH;
      byte line = j/WIDTH;

    //  result = hsv_to_rgb (random(256), 255, random(2)*255); //full color only, can be black    
       result = hsv_to_rgb (byte(hue+random(30)-random(30)), 255, random(200)+50); 

      copyendbuffertostartbuffer(col, line);
      //now set new endbuffer color
      colorbuffer[col][line][1].r = result.r;
      colorbuffer[col][line][1].g = result.g;
      colorbuffer[col][line][1].b = result.b;
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











