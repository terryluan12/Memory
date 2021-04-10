/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define BOX_LEN 2
#define NUM_BOXES 8

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Begin part1.s for Lab 7

volatile int pixel_buffer_start; // global variable
void clear_screen();
void draw_line(int x0, int x1, int y0, int y1, short int colour);
void swap(int* x, int* y);
void plot_pixel(int x, int y, short int line_color);
void wait();
	
int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;
	
	clear_screen();
	
	int win = 0;
	int chosen = 0;
	
	// card 1 c1
	draw_line(20, 10, 70, 10, CYAN);   // top line
	draw_line(20, 70, 70, 70, CYAN);   // bot line
	draw_line(20, 10, 20, 70, CYAN);   // left line
	draw_line(70, 10, 70, 70, CYAN);   // right line
	
	// card 2 c1
	draw_line(20, 80, 70, 80, CYAN);   // top line
	draw_line(20, 140, 70, 140, CYAN);   // bot line
	draw_line(20, 80, 20, 140, CYAN);   // left line
	draw_line(70, 80, 70, 140, CYAN);   // right line
	
	// card 3 c1
	draw_line(20, 150, 70, 150, CYAN);   // top line
	draw_line(20, 210, 70, 210, CYAN);   // bot line
	draw_line(20, 150, 20, 210, CYAN);   // left line
	draw_line(70, 150, 70, 210, CYAN);   // right line
	
	// card 1 c2
	draw_line(100, 10, 150, 10, CYAN);   // top line
	draw_line(100, 70, 150, 70, CYAN);   // bot line
	draw_line(100, 10, 100, 70, CYAN);   // left line
	draw_line(150, 10, 150, 70, CYAN);   // right line
	
	// card 2 c2
	draw_line(100, 80, 150, 80, CYAN);   // top line
	draw_line(100, 140, 150, 140, CYAN);   // bot line
	draw_line(100, 80, 100, 140, CYAN);   // left line
	draw_line(150, 80, 150, 140, CYAN);   // right line
	
	// card 3 c2
	draw_line(100, 150, 150, 150, CYAN);   // top line
	draw_line(100, 210, 150, 210, CYAN);   // bot line
	draw_line(100, 150, 100, 210, CYAN);   // left line
	draw_line(150, 150, 150, 210, CYAN);   // right line
	
	// card 1 c3
	draw_line(180, 10, 230, 10, CYAN);   // top line
	draw_line(180, 70, 230, 70, CYAN);   // bot line
	draw_line(180, 10, 180, 70, CYAN);   // left line
	draw_line(230, 10, 230, 70, CYAN);   // right line
	
	// card 2 c3
	draw_line(180, 80, 230, 80, CYAN);   // top line
	draw_line(180, 140, 230, 140, CYAN);   // bot line
	draw_line(180, 80, 180, 140, CYAN);   // left line
	draw_line(230, 80, 230, 140, CYAN);   // right line
	
	// card 3 c3
	draw_line(180, 150, 230, 150, CYAN);   // top line
	draw_line(180, 210, 230, 210, CYAN);   // bot line
	draw_line(180, 150, 180, 210, CYAN);   // left line
	draw_line(230, 150, 230, 210, CYAN);   // right line
	
	// card 1 c4
	draw_line(260, 10, 310, 10, CYAN);   // top line
	draw_line(260, 70, 310, 70, CYAN);   // bot line
	draw_line(260, 10, 260, 70, CYAN);   // left line
	draw_line(310, 10, 310, 70, CYAN);   // right line
	
	// card 2 c4
	draw_line(260, 80, 310, 80, CYAN);   // top line
	draw_line(260, 140, 310, 140, CYAN);   // bot line
	draw_line(260, 80, 260, 140, CYAN);   // left line
	draw_line(310, 80, 310, 140, CYAN);   // right line
	
	// card 3 c4
	draw_line(260, 150, 310, 150, CYAN);   // top line
	draw_line(260, 210, 310, 210, CYAN);   // bot line
	draw_line(260, 150, 260, 210, CYAN);   // left line
	draw_line(310, 150, 310, 210, CYAN);   // right line
	
	//graphic 1 - triangle
	/*draw_line(30, 60, 60, 60, MAGENTA);   // top line
	draw_line(30, 60, 45, 20, MAGENTA);   // bot line
	draw_line(60, 60, 45, 20, MAGENTA);   // left line
	*/
	
	//graphic 2 - trapezoid
	/*
	draw_line(40, 30, 50, 30, GREEN);   // top line
	draw_line(30, 50, 60, 50, GREEN);   // bot line
	draw_line(30, 50, 40, 30, GREEN);   // left line
	draw_line(50, 30, 60, 50, GREEN);   // right line
	*/
	
	//graphic 3 - rhombus
	/*
	draw_line(40, 30, 60, 30, YELLOW);   // top line
	draw_line(30, 50, 50, 50, YELLOW);   // bot line
	draw_line(30, 50, 40, 30, YELLOW);   // left line
	draw_line(50, 50, 60, 30, YELLOW);   // right line
	*/
	
	//graphic 4 - square
	/*
	draw_line(30, 25, 60, 25, RED);   // top line
	draw_line(30, 55, 60, 55, RED);   // bot line
	draw_line(30, 25, 30, 55, RED);   // left line
	draw_line(60, 25, 60, 55, RED);   // right line
	*/
	
	//graphic 5 - pentagon
	/*
	draw_line(30, 30, 45, 20, GREY);   // top-left line
	draw_line(60, 30, 45, 20, GREY);   // top-right line
	draw_line(30, 55, 60, 55, GREY);   // bot line
	draw_line(30, 30, 30, 55, GREY);   // left line
	draw_line(60, 30, 60, 55, GREY);   // right line
	*/
	
	//graphic 6 - hexagon
	/*
	draw_line(30, 30, 45, 20, WHITE);   // top-left line
	draw_line(60, 30, 45, 20, WHITE);   // top-right line
	draw_line(30, 50, 45, 60, WHITE);   // top-left line
	draw_line(60, 50, 45, 60, WHITE);   // top-right line
	draw_line(30, 30, 30, 50, WHITE);   // left line
	draw_line(60, 30, 60, 50, WHITE);   // right line
	*/
		
	return 0;
}

// code not shown for clear_screen() and draw_line() subroutines

void clear_screen() {
	for (int x = 0; x < RESOLUTION_X; x++) {
		for (int y = 0; y < RESOLUTION_Y; y++) {
			plot_pixel(x, y, 0x0000); 	// colour all pixels black
		}
	}
}

void draw_line(int x0, int y0, int x1, int y1, short int colour) {
	bool is_steep = abs(y1 - y0) > abs(x1 - x0);
	
	if (is_steep) {
		swap(&x0, &y0);
		swap(&x1, &y1);
	}
	
	if (x0 > x1) {
		swap(&x0, &x1);
		swap(&y0, &y1);
	}
	
	int deltax = x1 - x0;
	int deltay = abs(y1 - y0);
	
	int error = -(deltax / 2);
	int y = y0;
	int y_step = 0;
	
	if (y0 < y1) {
		y_step = 1;
	}
	else {
		y_step = -1;
	}
	
	for (int x = x0; x <= x1; x++) {
		if (is_steep) { 
			plot_pixel(y, x, colour);
		}
		else {
			plot_pixel(x, y, colour);
		}
		
		error = error + deltay;
		
		if (error >= 0) {
			y = y + y_step;
			error = error - deltax;
		}
	}
}

void swap(int* x, int* y){
	int temp = *x;
	*x = *y;
	*y = temp;
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void wait(){
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	volatile int * status = (int *)0xFF20302C;
	
	*pixel_ctrl_ptr = 1;
		
	while((*status &0x01) != 0){
		status = status;
	}
	
}