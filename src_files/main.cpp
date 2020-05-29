#include <iostream>
#include <time.h>



#include "Bitboard.h"
#include "Move.h"
#include "Board.h"
#include "Perft.h"
#include "TranspositionTable.h"


using namespace bb;

using namespace move;

int main() {



    init();

    Board *b = new Board{};
    
    
    std::cout << *b;
    
    
    perft_prepare(false);
    startMeasure();
    std::cout << perft(*b, 6, true, true, false) << std::endl;
    stopMeasure();
    perft_clean();

    return 0;
    
}



