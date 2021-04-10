


typedef struct Cards {

    double width = 12;
    double height = 12;
    int isFlipped = 0;
    int value;

} Card;


typedef struct MemoryGame {

    Card cards[NUMROWS][NUMCOLS];
    bool halfFlipped = false;
    int numFinished = 0;
    const int numPairs = NUMROWS * NUMCOLS;
    int highscore;

} MemGame;
