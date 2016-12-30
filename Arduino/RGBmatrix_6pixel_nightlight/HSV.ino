


//hsv to rgb using floats, is more accurate than using just ints
RGB hsv_to_rgb(float H, unsigned char S, unsigned char V)  //input values are from 0 to 255, hue can be a float
{
 
  float s=(float)S/255.0; //auf 1 normieren
  float v=(float)V/255.0; //auf 1 normieren
  RGB result; 		
  int i;
  float f, p, q, t;
  if(s == 0 ) //zero saturation, return gray level
  {
    result.r = round(255*v);
    result.g = round(255*v);
    result.b = round(255*v);
    return result;
  }


  i = (int)((float)H/42.5) %6;
  f = (float)H/42.5 - (int)((float)H/42.5);		
  p = v * ( 1.0 - s );
  q = v * ( 1.0 - (s * f));
  t = v * ( 1.0 - (s * ( 1.0 - f )));


  switch( i )
  {
  case 0:
    result.r = round(255*v); 
    result.g = round(255*t); 
    result.b = round(255*p);
    break;

  case 1:
    result.r = round(255*q); 
    result.g = round(255*v); 
    result.b = round(255*p); 
    break;

  case 2:
    result.r = round(255*p); 
    result.g = round(255*v); 
    result.b = round(255*t); 

    break;
    
  case 3:
    result.r = round(255*p); 
    result.g = round(255*q); 
    result.b = round(255*v); 
    break;			
    
  case 4:
    result.r = round(255*t); 
    result.g = round(255*p); 
    result.b = round(255*v); 
    break;
    
  default:								
    result.r = round(255*v); 
    result.g = round(255*p); 
    result.b = round(255*q); 
    break;								

  }

return result;

}

//function returns new faded RGB value. initialize primary buffer red channel to desired start hue

RGB fadeHSV(byte column, byte line) //fades through colors in HSV, goes around hsv-circle, returns new rgb value of current pixel for next step
{
  //uses primary rgb buffer for hue fading!

  if(colorbuffer[column][line][0].g>9) //use 10 subdivisions for hue
  {
   colorbuffer[column][line][0].r++; //increase  hue, primary red buffer is abused for hue 
   colorbuffer[column][line][0].g=0; //reset hue substep, primary green buffer is abused for hue sub steps
  }
    
   RGB result = hsv_to_rgb (  (float)colorbuffer[column][line][0].r + (float)colorbuffer[column][line][0].g/10, 255, 255);    
 
  colorbuffer[column][line][0].g++;//increase sub step
  
  return result;
}




