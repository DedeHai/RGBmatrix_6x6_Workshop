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
 * falling blocks function also known as Tetris
 * written by Damian Schneider, October, 2013
 */


uint8_t spaceoccupied[WIDTH][HEIGHT] = {
  0,0};
#define ENDHEIGHT WIDTH-1

void dropblock(void) //main tetris function
{

  static uint8_t moveindex = 0;
  static uint8_t init = 1;
  uint8_t endgame = 0; //is set to 1 when game ends
  static block block1;
  RGB backgroundcolor;
  backgroundcolor.r = 0;
  backgroundcolor.g =15;
  backgroundcolor.b = 10;
  if(init)
  {
    block1.name = 1; //name can be anything but 0 and 255
    block1.x = 3;
    block1.y = HEIGHT-2;
    block1.color=  pixelmatrix.HSVtoRGB(random(0,255), 255,255);
    block1.type =  rand()%7;
    Block_create(&block1) ;
    Block_getgoodmove(&block1);
    init = 0;
    moveindex = 0;
  }
  else{
    //before moving delete the old block:
    Block_delete(&block1);
    Block_move(&block1, block1.moves[moveindex++]);

    if(Block_drop(&block1) == 0)
    {
      Block_setoccupied(&block1);
      if(block1.y == HEIGHT-2) endgame = 1;
      init = 1;
    }
    Block_draw(&block1);
    Block_clearfulllines();
    if(endgame) Block_clearstage();//endgame, clear
  }
  pixelmatrix.sendColors();
}


void Block_getgoodmove(block *source)
{

  /*
  algorithm goes like this:
   block gets dropped from current position and after each full drop,
   one more movement (left or right only) is added. after reaching full
   movement, a rotation is added at the beginning.
   after each full drop, goodness of drop is evaluated, in the end, the 
   best one is taken as a good move
   */
  //  Block_fullcopy(source, &temp); //create a full copy
  block temp; //create a temporary block
  temp = *source; //copy block
  uint8_t orientation = 0; //there are 4 total orientations possible
  uint8_t movecycledone = 0;
  uint8_t dropdone = 0;
  uint8_t movecounter = 0; //maximum number of moves
  uint8_t moveindex = 0; //current move
  uint8_t move = 0; //0=left, 1 = right, 2 = rotate clockwise, 3 = rotate counterclockwise
  uint16_t goodness = 65535;

  while(orientation < 4)
  {
    while(movecycledone == 0)//one movecycle is all drops for one move
    {
      while(dropdone == 0) //drop the block (&move it)
      {
        uint8_t movesuccess = 1;
        //execute one move max
        if(moveindex < movecounter){
          movesuccess = Block_move(&temp, temp.moves[moveindex]); 
          moveindex++;
        }

        //drop by one
        if(Block_drop(&temp) == 0)
        { 
          dropdone = 1;
          if(moveindex < movecounter) movecycledone = 1; //if not all moves were executed, we are done with this move
        }
        if(movesuccess == 0){
          movecycledone = 1; 
          dropdone = 1;   
          // Serial.println("move blocked"); 
        }

      }
      dropdone = 0; 

      //evaluate goodness of this move
      uint16_t tempgoodness = Block_evaluateposition(&temp);
      // Serial.println(tempgoodness);
      if(tempgoodness < goodness)
      {
        goodness = tempgoodness;
        //  Serial.print("found: ");
        //   Serial.println(goodness);
        //         Serial.print("moves: ");
        uint8_t i,j;
        j=0;
        //copy moves to final part

        //determine number of turns
        if(orientation > 2) //rotate counter clockwise
        {
          source->moves[j] = 3;
          j++;
        }
        else if(orientation > 0)
        {
          if(orientation > 1)
          {
            source->moves[j] = 2;
            j++;
          }
          source->moves[j] = 2;
          j++;
        }

        for(i=0; i<movecounter; i++)
        {
          source->moves[j++] = temp.moves[i];   
        }
        while(j < HEIGHT+1)
        {
          source->moves[j++] = 8; //do dummy moves 
        }       
      }

      //add a move
      temp.moves[movecounter] = move;
      movecounter++;
      moveindex = 0; //reset moveindex
      //reset block position
      temp.x = source->x;
      temp.y = source->y;
    }
    movecycledone = 0;
    // Serial.println("cycledone");

    if(move == 0) 
    {
      move = 1; //do right movement now
      movecounter = 1;//zero move can be skipped in second cycle
      temp.moves[0] = 1;
    }
    else
    {
      //  Serial.println("orientationchange");
      move = 0;  //do left movement
      movecounter = 0;
      orientation++;
      switch (source->type) {
      case 0: //square type
        orientation = 4; //squre type has no rotation freedom
        break;
      case 1: //T type
        //rotate piece once, do all 4 orientations
        Block_move(&temp, 2);
        break;
      case 2: //L type
        //rotate piece once, do all 4 orientations
        Block_move(&temp, 2);
        break;
      case 3: //L type inverted
        //rotate piece once, do all 4 orientations
        Block_move(&temp, 2);
        break;
      case 4: //N type
        orientation+=2; //N types have only 2 orientations
        //rotate piece once 
        Block_move(&temp, 2);
        break;
      case 5: //N type inverted
        orientation+=2; //N types have only 2 orientations
        //rotate piece once 
        Block_move(&temp, 2);
        break;
      case 6: //I type 
        orientation+=2; //I type has only 2 orientations
        //rotate piece once 
        Block_move(&temp, 2);
        break;

      default: //quit search
        orientation=4; 
        break;
      }  
    }
  }
}


void Block_clearstage(void)
{
  uint8_t i,j;
  RGB backgroundcolor;
  backgroundcolor.r = 0;
  backgroundcolor.g =0;
  backgroundcolor.b =0;

  for(i = 0; i < WIDTH; i++) //scan all columns
  {
    for(j=0; j < HEIGHT; j++) //scan all lines in each column
    {
      pixelmatrix.setColor(i, j,backgroundcolor);
      Block_clearpixeloccupied(i,j);
    }
  }   

}
void Block_clearfulllines(void) //checks for full line, clears it and moves rest down
{
  uint8_t x,y;
  uint8_t linecount;
  for(y=0; y < WIDTH; y++) //scan all lines  
  {
    linecount = 0;
    for(x = 0; x < HEIGHT; x++) //scan all columns
    {
      if(Block_isoccupied(x, y, 2)) linecount++;
    }
    if(linecount == HEIGHT) //line is full
    {
      Block_removeline(y);
    }
  }
}

void Block_removeline(uint8_t line)
{
  uint8_t x,y;
  for(y=line; y < HEIGHT-1; y++) //scan lines above line to remove and copy them down
  {
    for(x = 0; x < WIDTH; x++) //scan all columns
    {
      spaceoccupied[x][y] = spaceoccupied[x][y+1]; //copy ocuupancy of pixels down
      pixelmatrix.setColor(x, y, pixelmatrix.getColor(x,y+1));//copy colors down
    }

  }

}


uint16_t Block_evaluateposition(block *source)
{
  uint16_t value = 0;
  uint8_t i,j;
  for(i = 0; i < source->width; i++) //scan all columns
  {
    for(j=0; j < source->height; j++) //scan all lines in each column
    {
      if(source->pixels[i][j])
      {
        value += (source->y + j)* (source->y + j);
      }
    }
  }
  //now check if blocks leaves a hole, which is very bad
  for(i = 0; i < source->width; i++) //scan all columns
  {
    //check lowest line for unoccupied pixels
    if(source->pixels[i][0] == 0 && source->pixels[i][1]) //this is a hole if true
    {
      //check if pixel in hole is not occupied, add to value if it is not
      if(spaceoccupied[source->x][source->y+i] == 0)  value += 30;
    }

  }

  return value;  

}



uint8_t Block_isoccupied(uint8_t x, uint8_t y, uint8_t reserved) //tests if x,y is occupied or out of picture
{
  //expects unsigned value, if <0, rolls over to 255
  if(x >= WIDTH) return 1; //x and y are uint, cannot be negative
  if(y >= HEIGHT) return 1; //so only positive overflow needs checking
  //now check if pixel is occupied and not reserved for us
  if(spaceoccupied[x][y] != 0 && spaceoccupied[x][y] != reserved) return 1;

  return 0; 
}


void Block_setoccupied(block* source)
{
  uint8_t i,j;
  //check if x,y position of block is out of picture     
  for(i = 0; i < source->width; i++) //scan all columns
  {
    for(j=0; j < source->height; j++) //scan all lines in each column
    {
      if(source->pixels[i][j]) //if pixel is set
      {
        Block_setpixeloccupied(source->x+i, source->y + j);
      }
    }
  }

}

void Block_setpixeloccupied(uint8_t x, uint8_t y) //sets a pixel occupied
{
  spaceoccupied[x][y] = 255;
}

void Block_clearpixeloccupied(uint8_t x, uint8_t y) //sets a pixel un-occupied
{
  spaceoccupied[x][y] = 0;
}


uint8_t Block_checkcollision(block* source) //checks if a block collides with anything (occupied area or wall), returns 0 if not collided
{
  uint8_t i,j;
  //check if x,y position of block is out of picture     

  for(j=0; j < source->height; j++) //scan all lines in each column (from bottom up)
  {
    for(i = 0; i < source->width; i++) //scan all columns
    {
      if(source->pixels[i][j]) //if pixel is set, check if it collides 
      {
        if(source->x+i >= HEIGHT){
          return 1;
        }
        if(source->y+j >= WIDTH)
        {
          if(source->y+j > WIDTH+6) return 1; //detect collisions only on bottom side of y, blocks cannot collide over the top
          return 2; //collision over the top
        }
        if(Block_isoccupied(source->x+i, source->y+j,source->name)) return 1;
      }
    }
  }
  return 0;    
}




void Block_rotate(block *source, uint8_t dir) //dir = 1 -> clockwise, dir = 0, counterclockwise
{
  //  Serial.println("roatating");
  uint8_t tempblock[source->height][source->width]; //temporary block to hold rotated piece
  uint8_t i,j;
  if(dir) //clockwise
  {
    for(i = 0; i < source->width; i++) //scan all columns
    {
      for(j=0; j < source->height; j++) //scan all lines in each column
      {
        tempblock[j][source->width - 1 - i] = source->pixels[i][j]; //rotate piece
      }
    }   
  }
  else //counterclockwise
  {
    for(i = 0; i < source->width; i++) //scan all columns
    {
      for(j=0; j < source->height; j++) //scan all lines in each column
      {
        tempblock[source->height - 1 - j][i] = source->pixels[i][j]; //rotate piece
      }
    }   
  }

  //switch width and height
  i = source->width;
  source->width = source->height;
  source->height = i;

  //now copy the temporary block back 

  for(i = 0; i < source->width; i++) //scan all columns
  {
    for(j=0; j < source->height; j++) //scan all lines in each column
    {
      source->pixels[i][j] = tempblock[i][j] ; //rotate piece
    }
  }


}


uint8_t Block_move(block *source, uint8_t move) //executes one move (left right or rotate), returns 0 if move blocked
{
  //also check if move is valid and only do it if valid
  //0=left, 1 = right, 2 = rotate clockwise, 3 = rotate counterclockwise
  switch (move) {
  case 0: //move left
    source->x--;
    //check for collision
    if(Block_checkcollision(source)==1){ 
      source->x++; 
      return 0;
    } //move back if collision
    break;
  case 1: //move right
    source->x++;
    //check for collision
    if(Block_checkcollision(source)==1){ 
      source->x--; 
      return 0;
    } //move back if collision
    break;
  case 2: //rotate clockwise
    Block_rotate(source,1);
    //check for collision
    if(Block_checkcollision(source)==1){  
      Block_rotate(source,0); 
      return 0;
    } //rotate back if collision
    break;
  case 3:  //rotate counterclockwise
    Block_rotate(source,0);
    //check for collision
    if(Block_checkcollision(source)==1){  
      Block_rotate(source,1); 
      return 0;
    } //rotate back if collision
    break;
  default: //do nothing
    break;
  }
  return 1;
}

uint8_t Block_drop(block *source) //drops block by one, returns 1 if successful, 0 if blocked
{
  source->y--;
  if(Block_checkcollision(source)==1)
  {   
    //Serial.println("collision");

    source->y++; //move back if collision
    return 0;
  }

  return 1;
}

void Block_draw(block *source) //draws block, needs to be within limits in x direction, is not checked here!
{

  uint8_t i,j;
  for(i = 0; i < source->width; i++) //scan all columns
  {
    for(j=0; j < source->height; j++) //scan all lines in each column
    {
      if(source->pixels[i][j] && source->y + j < HEIGHT)
      {
        pixelmatrix.setColor(source->x+i,source->y + j, source->color);
      }
    }
  }   
}

void Block_delete(block *source) //deletes block (do this before moving it)
{

  RGB backgroundcolor;
  backgroundcolor.r = 0;
  backgroundcolor.g =0;
  backgroundcolor.b = 0;
  uint8_t i,j;
  for(i = 0; i < source->width; i++) //scan all columns
  {
    for(j=0; j < source->height; j++) //scan all lines in each column
    {
      if(source->pixels[i][j])
      {
        pixelmatrix.setColor(source->x+i,source->y + j, backgroundcolor);
      }
    }
  }   
}

void Block_create(block *source) //creates pixels for a block, type is 0-5
{
  uint8_t i,j;
  source->color =   pixelmatrix.HSVtoRGB(rand()%255, 255,255);

  for(i = 0; i < 4; i++) //scan all columns
  {
    for(j=0; j < 4; j++) //scan all lines in each column
    {
      source->pixels[i][j] = 0; //set empty
    }
  }

  switch (source->type) {
  case 0: //square type
    source->pixels[0][0] = 1;
    source->pixels[1][0] = 1;
    source->pixels[0][1] = 1;
    source->pixels[1][1] = 1;
    source->width = 2;
    source->height = 2;
    break;
  case 1: //T type
    source->pixels[0][0] = 1;
    source->pixels[1][0] = 1;
    source->pixels[2][0] = 1;
    source->pixels[1][1] = 1;
    source->width = 3;
    source->height = 2;
    break;
  case 2: //L type
    source->pixels[0][0] = 1;
    source->pixels[1][0] = 1;
    source->pixels[2][0] = 1;
    source->pixels[2][1] = 1;
    source->width = 3;
    source->height = 2;
    break;
  case 3: //L type inverted
    source->pixels[0][0] = 1;
    source->pixels[1][0] = 1;
    source->pixels[2][0] = 1;
    source->pixels[0][1] = 1;
    source->width = 3;
    source->height = 2;
    break;
  case 4: //N type
    source->pixels[0][0] = 1;
    source->pixels[0][1] = 1;
    source->pixels[1][1] = 1;
    source->pixels[1][2] = 1;
    source->width = 2;
    source->height = 3;
    break;
  case 5: //N type inverted
    source->pixels[1][0] = 1;
    source->pixels[1][1] = 1;
    source->pixels[0][1] = 1;
    source->pixels[0][2] = 1;
    source->width = 2;
    source->height = 3;
    break;
  case 6: //I type 
    source->pixels[0][0] = 1;
    source->pixels[0][1] = 1;
    source->pixels[0][2] = 1;
    source->pixels[0][3] = 1;
    source->width = 1;
    source->height = 4;
    break;

  default:
    source->pixels[0][0] = 1;
    source->width = 1;
    source->height = 1;
    break;
  }

  //now do a random orientation
  for(i = 0; i < rand()%4; i++) //maximum rotate four times
  {
    Block_rotate(source,0);
  }
}







