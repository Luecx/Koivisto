
#include "uci.h"


#include "Bitboard.h"
#include "Board.h"
#include "Move.h"
#include "MoveOrderer.h"
#include "Verification.h"

using namespace std;
using namespace bb;
using namespace move;


int main() {
    
  
   
//    bb_init();
//
//
//    Board* b = new Board("1k1r4/1pp4p/p7/4p3/8/P5P1/1PP4P/2K1R3 w - -");
//    Move m = genMove(E1, E5, CAPTURE, WHITE_ROOK, BLACK_PAWN);
//    startMeasure();
//    for(int i = 0; i < 1e7; i++){
//        b->staticExchangeEvaluation(m);
//    }
//    std::cout << stopMeasure();
//
////    Board* b = new Board("1k1r3q/1ppn3p/p4b2/4p3/8/P2N2P1/1PP1R1BP/2K1Q3 w - -");
////    Move m = genMove(D3, E5, CAPTURE, WHITE_KNIGHT, BLACK_PAWN);
////    startMeasure();
////    for(int i = 0; i < 1e7; i++){
////        b->staticExchangeEvaluation(m);
////    }
////    std::cout << stopMeasure();
//
//
////    verifySEE();
//
//    bb_cleanUp();
    

      uci_loop();
   
    
      
    
    return 0;
    
}



