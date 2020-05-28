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
    
  
    
    
    U64 occ = randU64() * randU64();
    
    
    
    Board* b = new Board{};
//
//
//    std::cout << *b;
//
//
//    auto *mv = new MoveList();
//
//
//
//
startMeasure();
    std::cout << perft(*b, 6, true, true) << std::endl;
stopMeasure();
    
    return 0;
    
}



