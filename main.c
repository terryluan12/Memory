

int main(){

    MemGame game[NUMROWS][NUMCOLS];
    int stateChanged = 1;




    while(1){
	if(stateChanged){
	    clear_screen();
            for(int i = 0; i < NUMROWS; i++){
	        for(int j = 0; j < NUMCOLS; j++){
	            drawCard(game[i][j]);
	        }
            }
	    stateChanged = 0;
	}

    wait_for_vsync();

    if(buttonIsPressed){
	stateChanged = 0;
	// change state of card of 
    }
}









