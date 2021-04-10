

int main(){

    // initialize the game
    MemGame game;

    while(1){
	// if the game state has changed, then draw the game again on the screen
	if(game.stateChanged){
	    clear_screen();
            for(int i = 0; i < NUMROWS; i++){
	        for(int j = 0; j < NUMCOLS; j++){
	            drawCards(&game);
	        }
            }
	    game.stateChanged = false;
            wait_for_vsync();
	}

	// if the button is pressed
        if(buttonIsPressed){
	    // get the row and column of the card pressed
	    // int cardRow = ;
	    // int cardCol = ;
	    // change state of card of
	
	    // change the state of the card chosen
	    // changeState(game, cardRow, cardCol);
            
	    // if one card is pressed already
	    if(game.onePressed){
		// change state of the game
	        game.onePressed = false;
		// see if the cards match. In the function
		// it also updates the state of the cards and the game
	        bool isMatched = cardsMatch(game);

	        if(isMatched){
		    game.numFinished++;
	        }
	
	    }else{
		// otherwise only one card is face up now.
	        game.onePressed = true;
	    }
        }
    }
}









