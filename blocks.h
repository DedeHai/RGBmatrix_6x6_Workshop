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




typedef struct {
  uint8_t moves[7]; //record for block moves
  uint8_t name; //number representing the name of the block for space reservations
  uint8_t type; //block type (0...6)
  uint8_t x; //x postition
  uint8_t y; //y position
  uint8_t height; //block height in current orientation
  uint8_t width; //block with in current orientation
  uint8_t pixels[4][4]; //pixel data containing current block type and orientation
  uint8_t xmin; //x min movement limit before collision
  uint8_t xmax; //x max movement limit before collision
  uint8_t target; //target drop position
  RGB color;
}
block;
