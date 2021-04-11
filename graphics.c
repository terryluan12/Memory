/* This files provides address values that exist in the system */
#define BOARD                 "DE1-SoC"/* Memory */
#define DDR_BASE              0x00000000
#define DDR_END               0x3FFFFFFF
#define A9_ONCHIP_BASE        0xFFFF0000
#define A9_ONCHIP_END         0xFFFFFFFF
#define SDRAM_BASE            0xC0000000
#define SDRAM_END             0xC3FFFFFF
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_ONCHIP_END       0xC803FFFF
#define FPGA_CHAR_BASE        0xC9000000
#define FPGA_CHAR_END         0xC9001FFF
	
/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define JP1_BASE              0xFF200060
#define JP2_BASE              0xFF200070
#define PS2_BASE              0xFF200100
#define PS2_DUAL_BASE         0xFF200108
#define JTAG_UART_BASE        0xFF201000
#define JTAG_UART_2_BASE      0xFF201008
#define IrDA_BASE             0xFF201020
#define TIMER_BASE            0xFF202000
#define AV_CONFIG_BASE        0xFF203000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030
#define AUDIO_BASE            0xFF203040
#define VIDEO_IN_BASE         0xFF203060
#define ADC_BASE              0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO1_BASE        0xFF709000
#define HPS_TIMER0_BASE       0xFFC08000
#define HPS_TIMER1_BASE       0xFFC09000
#define HPS_TIMER2_BASE       0xFFD00000
#define HPS_TIMER3_BASE       0xFFD01000
#define FPGA_BRIDGE           0xFFD0501C

/* ARM A9 MPCORE devices */
#define   PERIPH_BASE         0xFFFEC000    // base address of peripheral devices
#define   MPCORE_PRIV_TIMER   0xFFFEC600    // PERIPH_BASE + 0x0600
	
/* Interrupt controller (GIC) CPU interface(s) */
#define MPCORE_GIC_CPUIF      0xFFFEC100    // PERIPH_BASE + 0x100
#define ICCICR                0x00          // offset to CPU interface control reg
#define ICCPMR                0x04          // offset to interrupt priority mask reg
#define ICCIAR                0x0C          // offset to interrupt acknowledge reg
#define ICCEOIR               0x10          // offset to end of interrupt reg
/* Interrupt controller (GIC) distributor interface(s) */
#define MPCORE_GIC_DIST       0xFFFED000    // PERIPH_BASE + 0x1000
#define ICDDCR                0x00          // offset to distributor control reg
#define ICDISER               0x100         // offset to interrupt set-enable regs
#define ICDICER               0x180         // offset to interrupt clear-enable regs
#define ICDIPTR               0x800         // offset to interrupt processor targets regs
#define ICDICFR               0xC00         // offset to interrupt configuration regs

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
#define BLACK 0x0000

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
#include <string.h>
#include <stdbool.h>

#define NUMROWS 4
#define NUMCOLS	4

// #define bool int
#define true 1
#define false 0	
		
volatile int pixel_buffer_start; // global variable
void clear_screen();
void draw_line(int x0, int x1, int y0, int y1, short int colour);
void swap(int* x, int* y);
void plot_pixel(int x, int y, short int line_color);
void wait();
void draw_graphic (int card, int x_center, int y_center, int x_left, int x_right, int y_top, int y_bot,int graphic, int clear);
void draw_front (int card, int x_center, int y_center, int x_left, int x_right, int y_top, int y_bot,int graphic, int clear);
void keyboard(unsigned char *key);
	
int main(void) {
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;
	
	/* Declare volatile pointers to I/O registers (volatile means that IO load
	and store instructions will be used to access these pointer locations,
	instead of regular memory loads and stores) */
	
	volatile int * PS2_ptr = (int *)PS2_BASE;
	*(PS2_ptr) = 0xFF; // reset
	
	
	unsigned char pressedKey = 0;
	bool gameOver = false;
	int clear = 0;
	
	clear_screen();
	
	int val = 0;
	int N = NUMCOLS*NUMROWS;
	int x1 = 20; 
	int x2 = 20;
	int y1 = 10;
	int	y2 = 10;
	
	short int x_left[N];
	short int x_right[N];
	short int x_center[N];
	
	short int y_top[N];
	short int y_bot[N];
	short int y_center[N];
	
	short int graphics[6] = {0, 1, 2, 3, 4, 5, 6, 7};  // this can be used to randomize the order of card_graphic below
	short int card_graphic[16] = {0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7, 6, 7};
	
	
	int x_width = (RESOLUTION_X-40-(NUMCOLS*20))/NUMCOLS;
	int y_height = (RESOLUTION_Y-20-(NUMROWS*10))/NUMROWS;
	int k =0;
	
	for(int i = 0; i < NUMCOLS; i++){
		for(int j = 0; j < NUMROWS; j++){
	    	// drawCards(&game);
			draw_line(x1, y1, x1+x_width, y1, CYAN);   			// top line
			draw_line(x1, y1+y_height, x1+x_width, y1+y_height, CYAN);   	// bot line
			draw_line(x1, y1, x1, y1+y_height, CYAN);       		// left line
			draw_line(x1+x_width, y1, x1+x_width, y1+y_height, CYAN);   	// right line
			
			y_center[k] = y1+(y_height/2);
			y_top[k] = y1;
			y_bot[k] = y1 + y_height;
			
			x_center[k] = x1+(x_width/2);
			x_left[k] = x1;
			x_right[k] = x1 + x_width;	
			draw_front(k, x_center[k], y_center[k], x_left[k], x_right[k], y_top[k], y_bot[k], card_graphic[k], clear);
			k=k+1;
			
			y1 = y1 + 10 + y_height;
			y2 = y2 + 10 + y_height;
	    }
		x1 = x1 + 20 + x_width;
		x2 = x2 + 20 + x_width;
		y1 = 10;
		y2 = 10;
    }
	
	// Check drawing - remove before submit
	/*
	k = 0;
	for(int i = 0; i < NUMCOLS; i++){
		for(int j = 0; j < NUMROWS; j++){
			draw_graphic(k, x_center[k], y_center[k], x_left[k], x_right[k], y_top[k], y_bot[k], card_graphic[k]);
			k = k+1;
		}
	}
	*/
	
	while (gameOver != true) {
		int i = 30;
		keyboard(&pressedKey); 			// gets key press and cheacks for which one was pressed and acts accordingly
			if (pressedKey == 0x45){ 	// check if key is 0
				i = 0;
			}
			else if (pressedKey == 0x16){	// check if key is 1
				i = 1;
			}
			else if (pressedKey == 0x1E){	// check if key is 2
				i = 2;
			}
			else if (pressedKey == 0x26){	// check if key is 3
				i = 3;			
			}
			else if (pressedKey == 0x25){	// check if key is 4
				i = 4;			
			}
			else if (pressedKey == 0x2E){	// check if key is 5
				i = 5;			
			}
			else if (pressedKey == 0x36){	// check if key is 6
				i = 6;			
			}
			else if (pressedKey == 0x3D){	// check if key is 7
				i = 7;			
			}
			else if (pressedKey == 0x3E){	// check if key is 8
				i = 8;			
			}
			else if (pressedKey == 0x46){	// check if key is 9
				i = 9;			
			}
			else if (pressedKey == 0x1c){	// check if key is A
				i = 10;			
			}
			else if (pressedKey == 0x32){	// check if key is B
				i = 11;			
			}
			else if (pressedKey == 0x21){	// check if key is C
				i = 12;			
			}
			else if (pressedKey == 0x23){	// check if key is D
				i = 13;			
			}
			else if (pressedKey == 0x24){	// check if key is E
				i = 14;			
			}
			else if (pressedKey == 0x2B){	// check if key is F
				i = 15;			
			}
			draw_graphic(i, x_center[i], y_center[i], x_left[i], x_right[i], y_top[i], y_bot[i], card_graphic[i], clear);
	}
		
	return 0;
}


void draw_graphic (int card, int x_center, int y_center, int x_left, int x_right, int y_top, int y_bot, int graphic, int clear){
	short int line_color;
	
	//graphic 1 - triangle
	if(graphic == 0) {
		if (clear == 1) {
			line_color = BLACK;
		}
		else {
			line_color = MAGENTA;
		}
		draw_line(x_left+10, y_bot-10, x_right-10, y_bot-10, line_color);  	// bot line
		draw_line(x_left+10, y_bot-10, x_center, y_top+10, line_color);  	// bot line
		draw_line(x_right-10, y_bot-10, x_center, y_top+10, line_color);   	// left line
	}
	
	//graphic 2 - trapezoid
	else if (graphic == 1) {
		if (clear == 1) {
			line_color = BLACK;
		}
		else {
			line_color = GREEN;
		}
		draw_line(x_left+15, y_top+10, x_right-15, y_top+10, line_color);  	// top line
		draw_line(x_left+10, y_bot-10, x_right-10, y_bot-10, line_color);  	// bot line
		draw_line(x_left+15, y_top+10, x_left+10, y_bot-10, line_color);   	// left line
		draw_line(x_right-10, y_bot-10, x_right-15, y_top+10, line_color);   	// right line
	}
	
	//graphic 3 - rhombus
	else if (graphic == 2) {
		if (clear == 1) {
			line_color = BLACK;
		}
		else {
			line_color = YELLOW;
		}
		draw_line(x_left+20, y_top+10, x_right-10, y_top+10, line_color);   // top line
		draw_line(x_left+10, y_bot-10, x_right-20, y_bot-10, line_color);   // bot line
		draw_line(x_left+20, y_top+10, x_left+10, y_bot-10, line_color);    // left line
		draw_line(x_right-10, y_top+10, x_right-20, y_bot-10, line_color);  // right line
	}
	
	//graphic 4 - square
	else if (graphic == 3) {
		if (clear == 1) {
			line_color = BLACK;
		}
		else {
			line_color = RED;
		}
		draw_line(x_left+10, y_top+10, x_right-10, y_top+10, line_color);   	// top line
		draw_line(x_left+10, y_bot-10, x_right-10, y_bot-10, line_color);    	// bot line
		draw_line(x_left+10, y_top+10, x_left+10, y_bot-10, line_color);   	// left line
		draw_line(x_right-10, y_top+10, x_right-10, y_bot-10, line_color);   	// right line
	}
	
	//graphic 5 - pentagon
	else if (graphic == 4) {
		if (clear == 1) {
			line_color = BLACK;
		}
		else {
			line_color = PINK;
		}
		draw_line(x_left+10, y_top+25, x_center, y_top+10, line_color);   	// top-left line
		draw_line(x_right-10, y_top+25, x_center, y_top+10, line_color);   	// top-right line
		draw_line(x_left+10, y_bot-10, x_right-10, y_bot-10, line_color);  	// bot line
		draw_line(x_left+10, y_top+25, x_left+10, y_bot-10, line_color);   	// left line
		draw_line(x_right-10, y_top+25, x_right-10, y_bot-10, line_color);   	// right line
	}
	
	//graphic 6 - hexagon
	else if (graphic == 5) {
		if (clear == 1) {
			line_color = BLACK;
		}
		else {
			line_color = WHITE;
		}
		draw_line(x_left+10, y_top+20, x_center, y_top+10, line_color);   	// top-left line
		draw_line(x_right-10, y_top+20, x_center, y_top+10, line_color);   	// top-right line
		draw_line(x_left+10, y_bot-20, x_center, y_bot-10, line_color);   	// bot-left line
		draw_line(x_right-10, y_bot-20, x_center, y_bot-10, line_color);   	// bot-right line
		draw_line(x_left+10, y_top+20, x_left+10, y_bot-20, line_color);   	// left line
		draw_line(x_right-10, y_top+20, x_right-10, y_bot-20, line_color);   	// right line
	}
	
	//graphic 7 - diamond
	else if (graphic == 6) {
		if (clear == 1) {
			line_color = BLACK;
		}
		else {
			line_color = ORANGE;
		}
		draw_line(x_left+10, y_center, x_center, y_top+10, line_color);   	// top-left line
		draw_line(x_right-10, y_center, x_center, y_top+10, line_color);   	// top-right line
		draw_line(x_left+10, y_center, x_center, y_bot-10, line_color);   	// bot-left line
		draw_line(x_right-10, y_center, x_center, y_bot-10, line_color);   	// bot-right line
	}
	
	//graphic 8 - x
	else if (graphic == 7) {
		if (clear == 1) {
			line_color = BLACK;
		}
		else {
			line_color = WHITE;
		}
		draw_line(x_left+10, y_top+10, x_right-10, y_bot-10, line_color);   	// top-left bot-right line
		draw_line(x_left+10, y_bot-10, x_right-10, y_top+10, line_color);   	// top-left bot-right line
	}
	
	return;
}

void draw_front (int card, int x_center, int y_center, int x_left, int x_right, int y_top, int y_bot,int graphic, int clear){
	short int line_color;
	
	if (clear == 1) {
		line_color = BLACK;
	}
	else {
		line_color = CYAN;
	}
	
	// 0
	if(card == 0) {
		draw_line(x_left+30, y_top+20, x_right-30, y_top+20, line_color);   	// top line
		draw_line(x_left+30, y_bot-20, x_right-30, y_bot-20, line_color);    	// bot line
		draw_line(x_left+30, y_top+20, x_left+30, y_bot-20, line_color);   	// left line
		draw_line(x_right-30, y_top+20, x_right-30, y_bot-20, line_color);   	// right line
	}
	
	// 1
	else if(card == 1) {
		draw_line(x_center, y_top+20, x_center, y_bot-20, line_color);   	// center line
	}
	
	// 2
	else if(card == 2) {
		draw_line(x_left+30, y_top+20, x_right-30, y_top+20, line_color);   	// top line
		draw_line(x_left+30, y_center, x_right-30, y_center, line_color);   	// center line
		draw_line(x_left+30, y_bot-20, x_right-30, y_bot-20, line_color);    	// bot line
		draw_line(x_left+30, y_top+20, x_left+30,y_center, line_color);   	// left line
		draw_line(x_right-30, y_center, x_right-30, y_bot-20, line_color);   	// right line
	}
	
	// 3
	else if(card == 3) {
		draw_line(x_left+30, y_top+20, x_right-30, y_top+20, line_color);   	// top line
		draw_line(x_left+30, y_center, x_right-30, y_center, line_color);   	// center line
		draw_line(x_left+30, y_bot-20, x_right-30, y_bot-20, line_color);    	// bot line
		draw_line(x_left+30, y_top+20, x_left+30, y_bot-20, line_color);   	// left line
	}
	
	// 4
	else if(card == 4) {
		draw_line(x_left+30, y_center, x_right-30, y_center, line_color);   	// center line
		draw_line(x_left+30, y_top+20, x_left+30, y_bot-20, line_color);   	// left line
		draw_line(x_right-30, y_top+20, x_right-30, y_center, line_color);   	// right line
	}
	
	// 5
	else if(card == 5) {
		draw_line(x_left+30, y_top+20, x_right-30, y_top+20, line_color);   	// top line
		draw_line(x_left+30, y_center, x_right-30, y_center, line_color);   	// center line
		draw_line(x_left+30, y_bot-20, x_right-30, y_bot-20, line_color);    	// bot line
		draw_line(x_left+30, y_center, x_left+30, y_bot-20, line_color);   	// left line
		draw_line(x_right-30, y_top+20, x_right-30, y_center, line_color);   	// right line
	}
	
	// 6
	else if(card == 6) {
		draw_line(x_left+30, y_top+20, x_right-30, y_top+20, line_color);   	// top line
		draw_line(x_left+30, y_center, x_right-30, y_center, line_color);   	// center line
		draw_line(x_left+30, y_bot-20, x_right-30, y_bot-20, line_color);    	// bot line
		draw_line(x_left+30, y_center, x_left+30, y_bot-20, line_color);   	// left line
		draw_line(x_right-30, y_top+20, x_right-30, y_bot-20, line_color);   	// right line
	}
	
	// 7
	else if(card == 7) {
		draw_line(x_left+30, y_top+20, x_right-30, y_top+20, line_color);   	// top line
		draw_line(x_left+30, y_top+20, x_left+30, y_bot-20, line_color);   	// left line
	}
	
	// 8
	else if(card == 8) {
		draw_line(x_left+30, y_top+20, x_right-30, y_top+20, line_color);   	// top line
		draw_line(x_left+30, y_center, x_right-30, y_center, line_color);   	// center line
		draw_line(x_left+30, y_bot-20, x_right-30, y_bot-20, line_color);    	// bot line
		draw_line(x_left+30, y_top+20, x_left+30, y_bot-20, line_color);   	// left line
		draw_line(x_right-30, y_top+20, x_right-30, y_bot-20, line_color);   	// right line
	}
	
	// 9
	else if(card == 9) {
		draw_line(x_left+30, y_top+20, x_right-30, y_top+20, line_color);   	// top line
		draw_line(x_left+30, y_center, x_right-30, y_center, line_color);   	// center line
		draw_line(x_left+30, y_bot-20, x_right-30, y_bot-20, line_color);    	// bot line
		draw_line(x_left+30, y_top+20, x_left+30, y_bot-20, line_color);   	// left line
		draw_line(x_right-30, y_top+20, x_right-30, y_center, line_color);   	// right line
	}
	
	// A
	else if(card == 10) {
		draw_line(x_left+30, y_top+20, x_right-30, y_top+20, line_color);   	// top line
		draw_line(x_left+30, y_center, x_right-30, y_center, line_color);   	// center line
		draw_line(x_left+30, y_top+20, x_left+30, y_bot-20, line_color);   	// left line
		draw_line(x_right-30, y_top+20, x_right-30, y_bot-20, line_color);   	// right line
	}
	
	// B
	else if(card == 11) {
		draw_line(x_left+30, y_center, x_right-30, y_center, line_color);   	// center line
		draw_line(x_left+30, y_bot-20, x_right-30, y_bot-20, line_color);    	// bot line
		draw_line(x_left+30, y_center, x_left+30, y_bot-20, line_color);   	// left line
		draw_line(x_right-30, y_top+20, x_right-30, y_bot-20, line_color);   	// right line
	}
	
	// C
	if(card == 12) {
		draw_line(x_left+30, y_top+20, x_right-30, y_top+20, line_color);   	// top line
		draw_line(x_left+30, y_bot-20, x_right-30, y_bot-20, line_color);    	// bot line
		draw_line(x_right-30, y_top+20, x_right-30, y_bot-20, line_color);   	// right line
	}
	
	// D
	else if(card == 13) {
		draw_line(x_left+30, y_center, x_right-30, y_center, line_color);   	// center line
		draw_line(x_left+30, y_bot-20, x_right-30, y_bot-20, line_color);    	// bot line
		draw_line(x_left+30, y_top+20, x_left+30, y_bot-20, line_color);   	// left line
		draw_line(x_right-30, y_center, x_right-30, y_bot-20, line_color);   	// right line
	}
	
	// E
	else if(card == 14) {
		draw_line(x_left+30, y_top+20, x_right-30, y_top+20, line_color);   	// top line
		draw_line(x_left+30, y_center, x_right-30, y_center, line_color);   	// center line
		draw_line(x_left+30, y_bot-20, x_right-30, y_bot-20, line_color);    	// bot line
		draw_line(x_right-30, y_top+20, x_right-30, y_bot-20, line_color);   	// right line
	}
	
	// F
	else if(card == 15) {
		draw_line(x_left+30, y_top+20, x_right-30, y_top+20, line_color);   	// top line
		draw_line(x_left+30, y_center, x_right-30, y_center, line_color);   	// center line
		draw_line(x_right-30, y_top+20, x_right-30, y_bot-20, line_color);   	// right line
	}
	
	return;
}

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

void keyboard(unsigned char *key) {
	volatile int * PS2_ptr = (int *)0xFF200100;
	volatile int * card = PS2_ptr;
	
	*key = *card & 0xFF;

	while (*card & 0x8000) {
		card = PS2_ptr;
	}
}
