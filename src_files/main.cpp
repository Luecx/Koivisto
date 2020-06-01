#include <iostream>
#include <time.h>



#include "Bitboard.h"
#include "Move.h"
#include "Board.h"
#include "Perft.h"
#include "search.h"
#include "uci.h"

using namespace bb;

using namespace move;



int main() {
    
    uci_loop();
    
    
    
//    bb_init();
//    perft_init(true);
//    search_init();
//
//
//
//    Board *b = new Board{"2bqkbn1/2pppp2/np2N3/r3P1p1/p2N2B1/5Q2/PPPPKPP1/RNB2r2 w KQkq - 0 1"};
////    Board *b = new Board{};
//
//    std::cout << *b;
//
//
//
////    startMeasure();
////    std::cout << perft(*b, 6, true, true, false) << std::endl;
////    stopMeasure();
//
//
//
//    startMeasure();
//    bestMove(b);
//    stopMeasure();
//
//    search_cleanUp();
//    perft_cleanUp();
//    bb_cleanUp();
//
    return 0;
    
}



