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
 * written by Damian Schneider, October 6th, 2013.
 * based on fire algorithm by giladaya (https://github.com/giladaya/arduino-led-matrix/blob/master/fire/fire.ino)
 *
 */




uint8_t firematrix[WIDTH][HEIGHT];
uint8_t firebaseline[WIDTH];


void fire(void) {
  static uint8_t pcnt = 0;
  
  if(initmode)
  {
  init_fire();
  initmode=0;
  }
  
  
  if (pcnt >= 95){
    fire_shiftUp();
    fire_generateLine();
    pcnt = 0;
  }
  fire_drawFrame(pcnt);
  pcnt+=10;
}



void init_fire() {

  memset(firematrix, 0,sizeof(firematrix));
  memset(firebaseline, 0,sizeof(firebaseline));
  fire_generateLine();
}



/**
 * Randomly generate the next line (matrix row)
 */
void fire_generateLine(void) {
  uint8_t x;
  //change base line (=creates random fire base)
  for (x = 0; x < WIDTH; x++) {

    firebaseline[x] = rand()%200;// firebaseline[x] + (rand() % 10) - 5;

  }
}

/**
 * shift all values in the matrix up one row
 */
void fire_shiftUp(void) {
  uint8_t x, y;
  for (x = 0; x < WIDTH; x++) {
    for (y = HEIGHT - 1; y > 0; y--) {
      firematrix[y][x] = (firematrix[y - 1][x])*0.93; //shift intensitiy up and fade
    }
    firematrix[0][x] = firebaseline[x];
  }
}

void fire_drawFrame(uint8_t percent) {
  uint8_t h, v;
  uint8_t x, y;
  int16_t tempval;

  //each row interpolates with the one before it
  for (y=HEIGHT-1;y>0;y--) {
    for (x=0;x<WIDTH;x++) {
      tempval =
        (int16_t)(((100-percent)*(int16_t)firematrix[y][x]
        + percent*0.93*(int16_t)firematrix[y-1][x]))/100
        - (y*y+40);
      v = (uint8_t) max(0, tempval) ;
      h = 22-y*4 + v/20; //hue depends on fire hight and fire intensity, 30= yellow, 0 = red
      pixelmatrix.setColor(x,y,pixelmatrix.HSVtoRGB(h,255,v));
    }
  }

  //first row interpolates with the base line
  for(x=0;x<WIDTH;x++) {
    tempval = (uint8_t)(((100-percent)*(int16_t)firematrix[0][x] + percent*0.93*(int16_t)firebaseline[x])/100);
    v = (uint8_t) max(0, tempval);
    h = 22-y*4 + v/20; //hue depends on fire hight and fire intensity
    pixelmatrix.setColor(x,y,pixelmatrix.HSVtoRGB(h,255,v));
  }
}




