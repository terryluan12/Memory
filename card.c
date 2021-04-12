


typedef struct Cards {

    double width = 12;
    double height = 12;
    bool isFlipped = false;
    int value;
    bool isMatched = false;

} Card;


typedef struct MemoryGame {

    Card cards[NUMROWS][NUMCOLS];
    int pressedCard = NULL;
    int numFinished = 0;
    bool stateChanged = false;
    const int numPairs = NUMROWS * NUMCOLS;
    bool gameOver = false;
    int level;
    int highscore;

} MemGame;
