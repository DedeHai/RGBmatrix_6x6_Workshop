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

 GameOfLife - cellular automat based on Conways Game of life
 with added color 'genetics'
 
 Created by Damian Schneider, March 12th, 2013.
 
 To save ram, the secondary colorbuffer is used to save game states:
 green channel is alive status of a cell (0=dead, 1=alive)
 red channel is alive status of a cell in the next round (0=dead, 1=alive)
 
 game of life ruleset:
 if(numneighbours == 3) create(l,c) or live on;
 if(numneighbours > 3) kill(l,c); //overpopulated
 if(numneighbours < 2) kill(l,c); //lonelyness
 
 
 */


#define FADESTEPS 80 //total number of fadesteps until endcolor is reached
#define FADEDELAY 10 //delay in ms for fading



void gameOfLife(void) {
  static uint16_t watchdog = 0; //to end game eventually (repeating pattern over several iterations)
  watchdog++;
  uint8_t l, c;

  if(initmode==1)
  {
    initGOL();
    initmode = 0;
  }


  //copy alive states of next round to current round and clear it for next round
  for (c = 0; c < WIDTH; c++) {
    for (l = 0; l < HEIGHT; l++) {
      colorbuffer2[c][l].g=     colorbuffer2[c][l].r;
      colorbuffer2[c][l].r= 0;
    }
  }

  //get new colors for next round, also update alive state for next round
  for (c = 0; c < WIDTH; c++) {
    for (l = 0; l < HEIGHT; l++) {
      colorbuffer1[c][l]=newcolor(c, l);
    }
  }

  //   at this point, secondary color buffer (red channel) contains colors for next round and the green channel 
  //   contains the cells that are alive right now

  //check if game has ended:
  static uint8_t isstatic[2] = {
    0                          }; 
  uint16_t isequal[2] = {
    0                          };

  //check if pattern is still changing and not repeating

  for (c = 0; c < WIDTH; c++) {
    for (l = 0; l < HEIGHT; l++) {


      if( colorbuffer2[c][l].g == colorbuffer2[c][l].r) isequal[0]++; //if true, cell did not change
      if( colorbuffer2[c][l].r == colorbuffer2[c][l].b) isequal[1]++; //if true, cell repeats
      colorbuffer2[c][l].b = colorbuffer2[c][l].g; //copy current alive state to blue channel for repeating pattern detection
    }
  }

  if(isequal[0] >= WIDTH * HEIGHT) isstatic[0]++; //if all cell states are equal
  if(isequal[1] >= WIDTH * HEIGHT) isstatic[1]++; //if all cell states are repeating

  //check if game is done now:
  if (isstatic[1] > 6 || isstatic[0] > 1 || watchdog > 100 ) {
    watchdog = 0;
    isstatic[0] = 0;
    isstatic[1] = 0;
    delay(800); //wait for user to see end picture
    initGOL(); //reinitialize if game has ended
  }

  //new colors are now in the primary buffer not in the actual color matrix
  //they need to be faded:
  uint16_t step;
  for (step = 0; step <= FADESTEPS; step++) {
    calculatefadecolors(step);
    pixelmatrix.sendColors();
    if(modechangerequest==1) break;
    delay(FADEDELAY);
  }

}

//returns the new color for next round
//updates alive status for next round
RGB newcolor(uint8_t column, uint8_t line) 
{
  RGB result = (RGB) {
    0,0,0                          };
  RGB tempcolor; //for pixel color readback
  int8_t numberofneighbours=0;
  //find indices of nighbours
  int8_t nextline = (line + 1)%HEIGHT;
  int8_t lastline = (line + HEIGHT - 1)%HEIGHT;
  int8_t nextcolumn = (column + 1) % WIDTH;
  int8_t lastcolumn = (column + WIDTH - 1) % WIDTH;

  //secondary colorbuffer (green channel) is used to determine alive cells
  if(colorbuffer2[lastcolumn][lastline].g)
  {
    tempcolor = pixelmatrix.getColor(lastcolumn, lastline);
    result.r += tempcolor.r/3;
    result.g += tempcolor.g/3;
    result.b += tempcolor.b/3;
    numberofneighbours++;
  }
  if(colorbuffer2[column][lastline].g)
  {
    tempcolor = pixelmatrix.getColor( column, lastline);
    result.r += tempcolor.r/3;
    result.g += tempcolor.g/3;
    result.b += tempcolor.b/3;
    numberofneighbours++;
  }
  if(colorbuffer2[nextcolumn][lastline].g)
  {
    tempcolor = pixelmatrix.getColor( nextcolumn, lastline);
    result.r += tempcolor.r/3;
    result.g += tempcolor.g/3;
    result.b += tempcolor.b/3;
    numberofneighbours++;
  }

  if(colorbuffer2[lastcolumn][line].g)
  {
    tempcolor = pixelmatrix.getColor( lastcolumn, line);
    result.r += tempcolor.r/3;
    result.g += tempcolor.g/3;
    result.b += tempcolor.b/3;
    numberofneighbours++;
  }
  if(colorbuffer2[nextcolumn][line].g )
  {
    tempcolor = pixelmatrix.getColor( nextcolumn, line);
    result.r += tempcolor.r/3;
    result.g += tempcolor.g/3;
    result.b += tempcolor.b/3;
    numberofneighbours++;
  }

  if(colorbuffer2[lastcolumn][nextline].g)
  {
    tempcolor = pixelmatrix.getColor( lastcolumn, nextline);
    result.r += tempcolor.r/3;
    result.g += tempcolor.g/3;
    result.b += tempcolor.b/3;
    numberofneighbours++;
  }
  if(colorbuffer2[column][nextline].g)
  {
    tempcolor = pixelmatrix.getColor( column, nextline);
    result.r += tempcolor.r/3;
    result.g += tempcolor.g/3;
    result.b += tempcolor.b/3;
    numberofneighbours++;
  }
  if(colorbuffer2[nextcolumn][nextline].g)
  {
    tempcolor = pixelmatrix.getColor( nextcolumn, nextline);
    result.r += tempcolor.r/3;
    result.g += tempcolor.g/3;
    result.b += tempcolor.b/3;
    numberofneighbours++;
  }


  if(numberofneighbours == 3)
  {
    //if already alive, grow older (dim intensity down)
    if( colorbuffer2[column][line].g)
    {
      result = pixelmatrix.getColor( column, line);
      result.r = constrain((int)result.r - 5 ,30,255);
      result.g = constrain((int)result.g - 5 ,30,255);
      result.b = constrain((int)result.b - 5 ,30,255);
      colorbuffer2[column][line].r = 1; //cell will be alive in next round (red color of buffer used)
    }
    //if not alive, life is created
    else
    {
      //use colors found from three neighbours, intensify color at random
      result.r = constrain((int)result.r + rand()%(result.r/2+3)-rand()%(result.r/2),0,255);
      result.g = constrain((int)result.g + rand()%(result.g/2+3)-rand()%(result.g/2) ,0,255);
      result.b = constrain((int)result.b +rand()%(result.b/2+3)-rand()%(result.b/2),0,255);
    }
    colorbuffer2[column][line].r = 1; //cell will be alive in next round (red color of buffer used)
  }
  else if(numberofneighbours == 2 &&  colorbuffer2[column][line].g) //cell stays alive but is not created and does not grow older
  {
    result = pixelmatrix.getColor( column, line);
    colorbuffer2[column][line].r = 1; //cell will be alive in next round as we well(red color of buffer used)
  }
  else if(numberofneighbours > 3) //cell dies from overpopulation
  {
    //copy current color for fading purposes
    result = pixelmatrix.getColor( column, line);
    colorbuffer2[column][line].r = 0; //cell dies 
  }
  else //cell dies from lonelyness
  {
    //copy current color for fading purposes
    result = pixelmatrix.getColor( column, line);
    colorbuffer2[column][line].r = 0; //cell dies 
  }
  return result;
}



void initGOL(void) {
  uint8_t c,l;
  if(initmode==0)
  {
    //fade all to black
    //clear alive matrix buffer so all will fade to black
    for (c = 0; c < WIDTH; c++) {
      for (l = 0; l < HEIGHT; l++) {
        colorbuffer2[c][l].g = 1; //set all to alive state (kills old frames)
        colorbuffer2[c][l].r = 0; //cell is dead in next frame
        //fade function operates on primary buffer not on active frame, copy it over:
        colorbuffer1[c][l] = pixelmatrix.getColor(c,l);
      }
    }

    //now fade to new buffer matrix state, as all cells are dead, all are faded to black
    uint16_t steps;
    for (steps = 0; steps <= FADESTEPS; steps++) {
      calculatefadecolors(steps);
      pixelmatrix.sendColors();
      delay(FADEDELAY);
      if(modechangerequest==1) break;
    }
  }

  //clear cell states
  for (c = 0; c < WIDTH; c++) {
    for (l = 0; l < HEIGHT; l++) {
      colorbuffer2[c][l].r = 0; 
      colorbuffer2[c][l].g = 0; 
      colorbuffer2[c][l].b = 0; 
    }
  }

  uint8_t x, y, z;
  //create color scheme:
  uint8_t deadcolor = 1 << (rand() % 3); //one color is "dead", to make no pixel white
  x = constrain(rand()%256*(~deadcolor&1),3,255);  //'~' is the binary 'NOT' operator, '&' is the binary 'AND' operator
  y = constrain(rand()%256*(~deadcolor&2),3,255);
  z = constrain(rand()%256*(~deadcolor&4),3,255);

  for (c = 0; c < WIDTH; c++) {
    for (l = 0; l < HEIGHT; l++) {
      colorbuffer2[c][l].r = rand() % 2; //randomly set pixels alive for next round
      if (colorbuffer2[c][l].r) {
        RGB randomcolor;
        randomcolor.r = rand() % z;
        randomcolor.g = rand() % y;
        randomcolor.b = rand() % x;

        if(initmode) //on very first game round, set pixelmatrix to the colors
        {
        pixelmatrix.setColor(c,l,randomcolor);
        }
        else
        {
                  colorbuffer1[c][l]= randomcolor;
        }
      }      
    }
  }
}


RGB fadetoblack(uint16_t fadestep, RGB startcolor) {
  RGB returncolor;
  float factor = (float) (FADESTEPS - fadestep) / (float) FADESTEPS;
  returncolor.r = (uint8_t)((float) startcolor.r * factor);
  returncolor.g = (uint8_t)((float) startcolor.g * factor);
  returncolor.b = (uint8_t)((float) startcolor.b * factor);
  return returncolor;
}

RGB fadetocolor(uint16_t fadestep, RGB endcolor) {
  RGB returncolor;
  float factor = (float) (fadestep) / (float) FADESTEPS;
  returncolor.r = (uint8_t)((factor) * (uint16_t) endcolor.r);
  returncolor.g = (uint8_t)((factor) * (uint16_t) endcolor.g);
  returncolor.b = (uint8_t)((factor) * (uint16_t) endcolor.b);
  return returncolor;
}

//calculates new color matrix at fadestep
//current color is in the primary color buffer, it fades to black (dies) or is black and fades to the color
void calculatefadecolors(uint16_t fadestep) 
{
  uint8_t c,l;
  RGB result; //resulting color
  RGB currentcolor; //current color of cell (either startcolor or endcolor)

  //check if cell was created or if it needs to be killed and fade it to color

    for (c = 0; c < WIDTH; c++) {
    for (l = 0; l < HEIGHT; l++) {

      if (colorbuffer2[c][l].g
        && colorbuffer2[c][l].r == 0) //if cell is alive and is dying -> fade to black
      {
        currentcolor = colorbuffer1[c][l];
        result = fadetoblack(fadestep, currentcolor);
        pixelmatrix.setColor(c,l,result);
      } 
      else if (colorbuffer2[c][l].g ==0
        && colorbuffer2[c][l].r) //if cell is coming to live -> fade to color
      {
        currentcolor = colorbuffer1[c][l];
        result = fadetocolor(fadestep, currentcolor);
        pixelmatrix.setColor(c,l,result);
      }
    }
  }

}

















