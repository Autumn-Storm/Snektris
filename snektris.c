/*
 * snektris.c
 *
 *  Created on: Nov 1, 2018
 *      Author: emersonaj
 */

#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_avalon_pio_regs.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

//Parameters that might change
alt_u8 block_size = 32;
int gamespeed = 1000000;

//function declarations

//setup function
void setup_game(void);

//initial snake movement
void setup_snake(void);

//normal snake movement.
//return -> 0 = continue play
//       -> 1 = landed on snake (did you mean mouse?)
//       -> 2 = game over
alt_u8 move_snake(void);

//Tetris block movement
alt_u8 move_tetris(void);
//Tetris block moves down
alt_u8 fall_tetris(void);
//pause screen
void pause_screen(alt_u8 score);

// set up vector for blocks so its used
// by every function
int blocks[12][15];

// might want to make this zero in the main, not here
int ccnt = 0;
alt_u8 key01;
alt_u8 backc;
alt_u8 pts;

// might want to make this zero in the main, not here
alt_u8 player_score = 0;
// might want to make this 4 in the main, not here
int snake_size = 4;
int snake_xloc;
int snake_yloc;
alt_u8 snake_dir;
// might want to make this zero in the main, not here
int gamestate =0;
//0 = initialize
//1 = play snake
//2 = play Tetris
//3 = pause

// to start a block fsm (to anticipate next gamestate in an organized manner)
int current_block;
int next_block;

// clock or counter
alt_u16 snake_colour;
// change rainbow

alt_u8 tetris_dir;
// left or right
int block_xloc;
int block_yloc;

alt_up_pixel_buffer_dma_dev * pixel_buf_dma_dev;

int main(void)
{
	pixel_buf_dma_dev = alt_up_pixel_buffer_dma_open_dev ("/dev/video_pixel_buffer_dma_0");
	
	// initialize values
			// snake length
			// background color
			// snake position
			// game speed?
	setup_game();
	setup_snake();

	while(1) {
	
	// if in [gamestate] then do [function] 
		
	/*game over?
	 * is target block an allowed move? 
	 ------- aka, is next_block = not 3 or 0 -------
		yes -> next_gamestate = 2;
	will snake land on mouse?
	-------- aka, is next_block = 4 ----------------
		yes -> next_gamestate = 1;
	else (continue)
	---------- movement of 
			-> next_gamestate = 0;
	*/
	// if such block hits such block, go to [gamestate] etc.
	
//______________________________________________________________________________________________//
		
	// if game ended, setup game
	if(gamestate==0){
		// initialize values
			// snake length
			// background color
			// snake position
			// game speed?
		setup_game();
		// setup new snake
		setup_snake();
		// start new game
		gamestate = 1;
	}
			
	// if snake block is mouse block [next 1 block is a 4 block], start tetris
	if(next_block==4){
		// put in tetris loop
		gamestate = 2;
	}
	
	// if snake block hits edge or tetris [next 1 block is a 0 block or 3 block] then end game [0]
	if(next_block==(1||3)){
	
	}

	
	// 
	{
		int tetris_fall_delay; //whatduheck is this?
		
		if(gamestate == 1)// landed on mouse
		{
			//setup tetris horizontally
			usleep(500000);//delay 0.1 sec
			
			//function return is the "next game state" <-- thats odd. Why not just set gamestate to tetris, and then if the
			// gamestate is tetris, just move the block?
			gamestate = move_tetris();
			tetris_fall_delay++; // whats this? <<<<<<<< and vvvvvvvvv ?????
			if(tetris_fall_delay < 11)
			{
				gamestate = fall_tetris();//move tetris block down every 1 second
			}
		}
		else if(gamestate == 2) //game over
		{
			//endscreen
			//pause
		}
		else
		{
			gamestate=move_snake();
			usleep(gamespeed); //pause between moves
		}
	}
	
	    //do this for every loop, I think it would be good. its like its scanning it each time. we could set numbers in logic
	    for(int ic = 0; ic < 15; ic++){
	        for(int ir = 0; ir < 12; ir++){
	            // 0 is border, color white 0xFFFF
	            if(blocks[ir][ic]==0){
	            alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf_dma_dev, ir, ic, ir+block_size, ic+block_size, 0xFFFF, 0);
	                }
		    // varying snake color
	            if(blocks[ir][ic]==1){
	            alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf_dma_dev, ir, ic, ir+block_size, ic+block_size, snake_colour, 0);
	            }
	            //background color
	            if(blocks[ir][ic]==2){
	            alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf_dma_dev, ir, ic, ir+block_size, ic+block_size, backc, 0);
	            }
	            // gray if mouse 0x8C51
	            if(blocks[ir][ic]==4){
	            alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf_dma_dev, ir, ic, ir+block_size, ic+block_size, 0x8C51, 0);
	            }
	        }
	    }
	
	//redraw placed tetris blocks ?? 
	return 0;
}

//setup function
void setup_game(void)
{
	//reset screen
	alt_up_pixel_buffer_dma_clear_screen (pixel_buf_dma_dev, 0); //clear screen
	
		// set score to 0
		score = 0;
	
		// set background to all 2s
	blocks[12][15] = {
	       {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ,
	       {0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0} ,
	       {0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0} ,
	       {0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0} ,
	       {0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0} ,
	       {0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0} ,
	       {0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0} ,
	       {0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0} ,
	       {0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0} ,
	       {0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0} ,
	       {0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0} ,
	       {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	    };
	
//decide new snake color
	ccnt++;
	if(ccnt == 1){
		snake_colour = 0xE124;
	}
	else if(ccnt == 2){
		snake_colour = 0xF782;
	}
	else if(ccnt == 3){
		snake_colour = 0x3F1D;
	}
	else if(ccnt == 4){
		snake_colour = 0x423D;
	}
	else if(ccnt == 5){
		snake_colour = 0xA21D;
	}
	else {
		snake_colour = 0xEA1B;
		ccnt = 0;
	}
	//yellow = 0xF782
	//green = 0x5F26
	//blue = 0x3F1D
	//purple = 0x423D
	//megenta = 0xA21D
	//pink = 0xEA1B
	
	    // 32x32
	    // if assignment 0 then
	    //
	    for(int ic = 0; ic < 15; ic++){
	        for(int ir = 0; ir < 12; ir++){
	            // 0 is border, color white 0xFFFF
	            if(blocks[ir][ic]==0){
	            alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf_dma_dev, ir, ic, ir+block_size, ic+block_size, 0xFFFF, 0);
	                }
		    // varying snake color
	            if(blocks[ir][ic]==1){
	            alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf_dma_dev, ir, ic, ir+block_size, ic+block_size, snake_colour, 0);
	            }
	            //background color
	            if(blocks[ir][ic]==2){
	            alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf_dma_dev, ir, ic, ir+block_size, ic+block_size, backc, 0);
	            }
	            // gray if mouse 0x8C51
	            if(blocks[ir][ic]==4){
	            alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf_dma_dev, ir, ic, ir+block_size, ic+block_size, 0x8C51, 0);
	            }
	        }
	    }
	
		// set beginning speed?
		// set beginning snake length
		snake_length = 4;
	
	//banners
	//place mouse block 4 placement = rand(); 
	
}

//initialize snake
void setup_snake(void)
{
	//decide new snake color
	ccnt++;
	if(ccnt == 1){
		snake_colour = 0xE124;
	}
	else if(ccnt == 2){
		snake_colour = 0xF782;
	}
	else if(ccnt == 3){
		snake_colour = 0x3F1D;
	}
	else if(ccnt == 4){
		snake_colour = 0x423D;
	}
	else if(ccnt == 5){
		snake_colour = 0xA21D;
	}
	else {
		snake_colour = 0xEA1B;
		ccnt = 0;
	}
	//yellow = 0xF782
	//green = 0x5F26
	//blue = 0x3F1D
	//purple = 0x423D
	//megenta = 0xA21D
	//pink = 0xEA1B
	
	//draw snake head
	snake_xloc = 320;
	snake_yloc=448;
	alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf_dma_dev, snake_xloc, snake_yloc, snake_xloc+block_size, snake_yloc+block_size, snake_colour, 0);
	snake_dir=2; //move at 180 deg (left)
}

//normal snake movement
alt_u8 move_snake(void)
{
	alt_u8 next_gamestate; //0 = continue playing, 1 = Tetris, 2 = game over
	key01 = IORD_ALTERA_AVALON_PIO_DATA(KEY01_PIO_BASE);

	//determine snake direction
	if(!(key01&1)){ // 0 is pressed
		// right or clockwise
		if(snake_dir==0)
			snake_dir = 3;
		else
			snake_dir = snake_dir-1;
	}
	else if(!(key01&(1<<1))){ // 1 is pressed
		// left or counter-clockwise
		if(snake_dir==3)
			snake_dir=0;
		else
			snake_dir = snake_dir+1;
	}
	else //no or all key press
	{

	}

	//move snake
	for(int i_blk = 0; i_blk < snake_size; i_blk++)
	{
		if(snake_dir==0)
			snake_xloc=snake_xloc + block_size;
		if(snake_dir==1)
			snake_yloc=snake_yloc + block_size;
		if(snake_dir==2)
			snake_xloc=snake_xloc - block_size;
		if(snake_dir==3)
			snake_yloc=snake_yloc - block_size;
		alt_up_pixel_buffer_dma_draw_rectangle(pixel_buf_dma_dev, snake_xloc, snake_yloc, snake_xloc+block_size, snake_yloc+block_size, snake_colour, 0);
	}
	return next_gamestate;

}
//Tetris block movement
alt_u8 move_tetris(void)
{
	//shift side to side
	key01 = IORD_ALTERA_AVALON_PIO_DATA(KEY01_PIO_BASE);
	alt_u8 next_gamestate; //0 = continue playing, 1 = tetris, 2 = game over
	return next_gamestate;
}


alt_u8 fall_tetris(void){
	//shift down by 1
	
	// if block is 1, then drop every block by 1 downward, or -1 blocksize
	tetris_dir = 3; // tetris_yloc-block_size; 
	
	// Isnt gamestate for the game logic in 
	// the main? would it be easier to just have logic for gamestate in main, and then just purely do the function here?
	alt_u8 next_gamestate; //0 = continue playing, 1 = tetris, 2 = game over
	return next_gamestate;
}

alt_u8 place_tetris(void){
	//for when the block gets to the bottom. logic will be above, telling when this will occur
	
	// if block is 1, then drop every block by 1 downward, or -1 blocksize
	tetris_dir = 3; // tetris_yloc-block_size; 
	
	// Isnt gamestate for the game logic in 
	// the main? would it be easier to just have logic for gamestate in main, and then just purely do the function here?
	alt_u8 next_gamestate; //0 = continue playing, 1 = tetris, 2 = game over
	return next_gamestate;
}


