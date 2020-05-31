#include <iostream>
#include <time.h>



#include "Bitboard.h"
#include "Move.h"
#include "Board.h"
#include "Perft.h"
#include "search.h"


using namespace bb;

using namespace move;



int main() {
    
    
    bb_init();
    perft_init(true);
    search_init();
    
    

//    Board *b = new Board{"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"};
    Board *b = new Board{};

    std::cout << *b;

    
    
//    startMeasure();
//    std::cout << perft(*b, 6, true, true, false) << std::endl;
//    stopMeasure();
    
    
    
    startMeasure();
    bestMove(b);
    stopMeasure();
    
    search_cleanUp();
    perft_cleanUp();
    bb_cleanUp();
    
    return 0;
    
}



