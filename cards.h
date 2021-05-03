


typedef struct Cards {

    int row, col;
    int y_top;
    int x_left;
    bool isFlipped;
    int value;
    bool isMatched;

} Card;


typedef struct MemoryGame {

    Card cards[NUM_CARDS];
    int pressedCard;
    int numFinished;
    bool stateChanged;
    bool gameOver;
    int level;
    int highscore;

} MemGame;
