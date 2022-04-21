
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "global.h" // if using CPUlator, copy global.h here
#include "cards.h" // if using CPUlator, copy cards.h here

int NUM_ROWS;
int NUM_COLS;
int NUM_CARDS;
	

volatile int pixel_buffer_start; // global variable
volatile int *KEY_ptr = (int *) 0xFF20005C; // push buttons 
volatile int * PS2_ptr = (int *)0xFF200100; // ps2 keyboard

// function declarations
void clear_screen();
void draw_line(int x0, int x1, int y0, int y1, short int colour);
void swap(int* x, int* y);
void plot_pixel(int x, int y, short int line_color);
void draw_graphic (int graphic, int x_left, int y_top, int clear);
void draw_front (int card, int x_left,int y_top, int clear);
void keyboard(unsigned char *key);
void getKey(int keyID, int *keyPressed);
bool cardsMatch(MemGame *game, int secondCardId);
void drawCards(MemGame *game);
void initializeGame(MemGame *game);
void wait_for_vsync();
void shuffle(int cards[], int num_cards);

void HEX_PS2(char b1, char b2);
void HEX_PS2_1(char b1, char b2);

// important global variables
int g_width;
int g_height;

// these can help with logic but can be removed if not used
int level = -1;
int easy = 1;
int medium = 2;
int hard = 3;
int v_hard = 4;

bool firstgame = true;

int main(void) {
	
	while(true){
		volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
		/* Read location of the pixel buffer from the pixel buffer controller */
		pixel_buffer_start = *pixel_ctrl_ptr;

		*(PS2_ptr) = 0xFF; // reset
		
		clear_screen(); // clear screen

		// initialize the game
		MemGame game;
		
		bool choice = false;
		// choose game level
		while (choice == false) {
			if (*KEY_ptr == 0b0001) { 		// press KEY[0] = easy
				game.level = easy;
				NUM_ROWS = 1;
				NUM_COLS = 4;	
				choice = true;
			}  
			else if (*KEY_ptr == 0b0010) { 		// press KEY[1] = medium
				game.level = medium;
				NUM_ROWS = 2;
				NUM_COLS = 4;	
				choice = true;
			} 
			else if (*KEY_ptr == 0b0100){ 		// press KEY[2] = hard
				game.level = hard;
				NUM_ROWS = 3;
				NUM_COLS = 4;		
				choice = true;
			}
			else if(*KEY_ptr == 0b1000){ 		// press KEY[3] = very hard
				game.level = v_hard;
				NUM_ROWS = 4;
				NUM_COLS = 4;		
				choice = true;
			}
			
		}
		
		// get the number of cards
		NUM_CARDS = NUM_ROWS*NUM_COLS;
		
		*KEY_ptr = 0xFF;	// reset KEYs
		
		// get the height and width of the cards
		g_width = (RESOLUTION_X-40-(NUM_COLS*20))/NUM_COLS;
		g_height = (RESOLUTION_Y-20-(NUM_ROWS*10))/NUM_ROWS;
		
		
		initializeGame(&game);
		char a, b;
		
		if (firstgame == true){
			HEX_PS2_1(0, 0);
		}
		
		int PS2_data, RVALID;
		
		while(!game.gameOver){
			// if the game state has changed, then draw the game again on the screen
			if(game.stateChanged){
				drawCards(&game);
				game.stateChanged = false;
				b = (char) game.score%10;
				a = (char) game.score/10;
				HEX_PS2(a, b);
			}
		
		
			// read keyboard val and "flip" card if it is chosen
			PS2_data = *(PS2_ptr);
			char keyID = 0;
			int flippedCard = -1;
			RVALID = PS2_data & 0x8000;
			
			// if the input is valid
			if(RVALID){	
				// get the keyID of the pressed keys
				keyID = *(PS2_ptr) & 0xFF;
				// get and store the key in flippedCard
				getKey(keyID, &flippedCard);
				// reset the keyboard
				*(PS2_ptr) = 0xFF;
				//while it's still valid, write to the register
				while (PS2_data & 0x8000) {
					PS2_data = *PS2_ptr;
				}
			}

			// if the button is pressed
			if(flippedCard != -1 ){
				// change the state of the game (and therefore redraw)
				game.stateChanged = true;
				game.score += 1;
				// if one card is pressed already and the new card flipped isn't flipped already
				if(game.pressedCard != -1 && !game.cards[flippedCard].isFlipped){
					// flip over the new card temporarily and wait a couple of seconds
					game.cards[flippedCard].isFlipped = true;
					drawCards(&game);
					for(int i = 0; i < 1000000; i++){}
					
					// Updates the state of the cards and the game
					cardsMatch(&game, flippedCard);
					
					// next time, the player looking for a new pair
					game.pressedCard = -1;	

					// if the number of pairs finished is equal to total number of pairs, it's game over
					if(game.numFinished == NUM_CARDS/2){
						game.gameOver = true;
						if (game.score > game.highscore){
							game.highscore = game.score;
							b = (char) game.highscore%10;
							a = (char) game.highscore/10;
							HEX_PS2_1(a, b);
						}
						firstgame = false;
					}
				}

				// if no card is flipped, then flip it face up
				else if(!game.cards[flippedCard].isFlipped){
					game.pressedCard = flippedCard;
					game.cards[game.pressedCard].isFlipped = true;
				}
			}
		}
	}	
	return 0;
}

bool cardsMatch(MemGame *game, int secondCardId){
	
	// initialize important variables
	Card *firstCard, *secondCard;
	firstCard = &game->cards[game->pressedCard];
	secondCard = &game->cards[secondCardId];
	bool matched = firstCard->value == secondCard->value; 

	// if the cards match, then flip over both cards to their face
	if(matched){
		secondCard->isFlipped = true;
		firstCard->isFlipped = true;
		game->numFinished++;
	}
	
	// otherwise flip over both to the back
	else{
		firstCard->isFlipped = false;
		secondCard->isFlipped = false;
	}
	return matched;
}

void initializeGame(MemGame *game){
		
	int card_graphic[NUM_CARDS]; 
	int card_shapes[16] = {0, 1, 0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7};
	
	for (int i = 0; i < NUM_CARDS; i++){
		card_graphic[i] = card_shapes[i];
	}
	shuffle (card_graphic, NUM_CARDS);

	// set important variables of game to default. Set stateChanged to true, to draw it for the first time
	game->pressedCard = -1;
	game->stateChanged = true;
	game->gameOver = false;
	if (firstgame == true){
		game->highscore = 0;
	}
	game->score = 0;
	game->numFinished = 0;
	
	for(int i = 0; i < NUM_ROWS; i++){
		for(int j = 0; j < NUM_COLS; j++){
			game->cards[NUM_COLS * i + j].row = i;
			game->cards[NUM_COLS * i + j].col = j;
		}
	}
	// set the pixel locations for the top and left values of the cards
	for(int i = 0; i < NUM_CARDS; i++){
		game->cards[i].y_top = 10 + 10 * game->cards[i].row + g_height * game->cards[i].row;
		game->cards[i].x_left = 20 + 20 * game->cards[i].col + g_width * game->cards[i].col;
		game->cards[i].value = card_graphic[i];
		game->cards[i].isFlipped = false;
		
		// draw outlines of the cards
		draw_line(game->cards[i].x_left, game->cards[i].y_top, game->cards[i].x_left+g_width, game->cards[i].y_top, CYAN);   			// top line
		draw_line(game->cards[i].x_left, game->cards[i].y_top+g_height, game->cards[i].x_left+g_width, game->cards[i].y_top+g_height, CYAN);  	// bot line
		draw_line(game->cards[i].x_left, game->cards[i].y_top, game->cards[i].x_left, game->cards[i].y_top+g_height, CYAN);       		// left line
		draw_line(game->cards[i].x_left+g_width, game->cards[i].y_top, game->cards[i].x_left+g_width, game->cards[i].y_top+g_height, CYAN);   	// right line			
	}
}

void drawCards(MemGame *game){

	for(int i = 0; i < NUM_CARDS; i++){
		 
		if(game->cards[i].isFlipped){
			// draw over the front with black and draw the graphic
			draw_front(i, game->cards[i].x_left, game->cards[i].y_top, 1);
			draw_graphic(game->cards[i].value, game->cards[i].x_left, game->cards[i].y_top, 0);
		}
		else {
			// draw over the graphic with black, draw the front and change wasFlipped to false
			draw_graphic(game->cards[i].value, game->cards[i].x_left, game->cards[i].y_top, 1);
			draw_front(i, game->cards[i].x_left, game->cards[i].y_top, 0);
		}
	}
			
}

void draw_graphic (int graphic, int x_left, int y_top, int clear){
	
	short int line_color;
	short int y_center = y_top + (g_height/2);
	short int y_bot = y_top + g_height;
	
	short int x_center = x_left + (g_width/2);
	short int x_right = x_left + g_width;	
	
	//graphic 1 - triangle
	if(graphic == 0) {
		if (clear == 1) {
			line_color = BLACK;
		}
		else {
			line_color = MAGENTA;
		}
		draw_line(x_left+10, y_bot-10, x_right-10, y_bot-10, line_color);  	// bot line
		draw_line(x_left+10, y_bot-10, x_center, y_top+10, line_color);   	// bot line
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
		draw_line(x_right-10, y_bot-10, x_right-15, y_top+10, line_color);  	// right line
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

void draw_front (int card, int x_left,int y_top, int clear){
	short int line_color;
	short int y_center = y_top + (g_height/2);
	short int y_bot = y_top + g_height;
	
	short int x_center = x_left + (g_width/2);
	short int x_right = x_left + g_width;	
	
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

void getKey(int keyID, int *keyPressed){
	
	if (keyID == 0x45){ 		// check if key is 0
		*keyPressed = 0;
	}
	else if (keyID == 0x16){	// check if key is 1
		*keyPressed = 1;
	}
	else if (keyID == 0x1E){	// check if key is 2
		*keyPressed = 2;
	}
	else if (keyID == 0x26){	// check if key is 3
		*keyPressed = 3;	
	}
	else if (keyID == 0x25){	// check if key is 4
		*keyPressed = 4;
	}
	else if (keyID == 0x2E){	// check if key is 5
		*keyPressed = 5;
	}
	else if (keyID == 0x36){	// check if key is 6
		*keyPressed = 6;
	}
	else if (keyID == 0x3D){	// check if key is 7
		*keyPressed = 7;
	}
	else if (keyID == 0x3E){	// check if key is 8
		*keyPressed = 8;
	}
	else if (keyID == 0x46){	// check if key is 9
		*keyPressed = 9;
	}
	else if (keyID == 0x1c){	// check if key is A
		*keyPressed = 10;
	}
	else if (keyID == 0x32){	// check if key is B
		*keyPressed = 11;
	}
	else if (keyID == 0x21){	// check if key is C
		*keyPressed = 12;
	}
	else if (keyID == 0x23){	// check if key is D
		*keyPressed = 13;
	}
	else if (keyID == 0x24){	// check if key is E
		*keyPressed = 14;
	}
	else if (keyID == 0x2B){	// check if key is F
		*keyPressed = 15;
	}
	else{
		*keyPressed = -1;
	}
}

void draw_line(int x0, int y0, int x1, int y1, short int colour) {
	// drawn using Bresenham's algorithm
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

void shuffle(int cards[], int num_cards) {
    
	srand(time(NULL));
    
    for(int i = num_cards-1; i > 0; i--) {
        int j = rand() % (i+1);
        swap(&cards[i], &cards[j]);
    }
}

void swap(int* x, int* y){
	
	int temp = *x;
	*x = *y;
	*y = temp;
}

void plot_pixel(int x, int y, short int line_color) {
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

// displays current score
void HEX_PS2(char b1, char b2) {
	volatile int * HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
	/* SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in
	* a single 7-seg display in the DE1-SoC Computer, for the hex digits 0 - F
	*/
	unsigned char seven_seg_decode_table[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
											  0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned int shift_buffer, nibble;
	unsigned char code;
	
	shift_buffer =  (b1 << 4) | b2;
	for (int i = 0; i < 4; ++i) {
		nibble = shift_buffer & 0x0000000F; // character is in rightmost nibble
		code = seven_seg_decode_table[nibble];
		hex_segs[i] = code;
		shift_buffer = shift_buffer >> 4;
	}
	/* drive the hex displays */
	*(HEX3_HEX0_ptr) = *(int *)(hex_segs);
}

// dislays highscore
void HEX_PS2_1(char b1, char b2) {
	volatile int * HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;
	/* SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in
	* a single 7-seg display in the DE1-SoC Computer, for the hex digits 0 - F
	*/
	unsigned char seven_seg_decode_table[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
											  0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned int shift_buffer, nibble;
	unsigned char code;
	
	shift_buffer =  (b1 << 4) | b2;
	for (int i = 4; i < 6; ++i) {
		nibble = shift_buffer & 0x0000000F; // character is in rightmost nibble
		code = seven_seg_decode_table[nibble];
		hex_segs[i] = code;
		shift_buffer = shift_buffer >> 4;
	}
	/* drive the hex displays */
	*(HEX5_HEX4_ptr) = *(int *)(hex_segs + 4);
}
