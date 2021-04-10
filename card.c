


typedef struct Cards {

    double width = 12;
    double height = 12;
    bool isFlipped = false;
    int value;
    bool isMatched = false;

} Card;


typedef struct MemoryGame {

    Card cards[NUMROWS][NUMCOLS];
    bool onePressed = false;
    int numFinished = 0;
    bool stateChanged = false;
    const int numPairs = NUMROWS * NUMCOLS;
    int level;
    int highscore;

} MemGame;
