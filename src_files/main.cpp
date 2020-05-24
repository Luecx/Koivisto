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
    
  
    //Board* b = new Board{};
    
    
//    std::cout << *b;
    
    
    
    U64 k = 0b1111101010100010101;
    int indices[]{0,2,4,8,10,12,13,14,15,16,17};
    
    std::cout << k << std::endl;

    int index = 0;
    startMeasure();
    for (int i = 0; i < 1e8; i++) {
        for (int i:indices) {
            index = i;
        }
        
    }
    stopMeasure();
    
    
    startMeasure();
    for (int i = 0; i < 1e8; i++) {
        U64 b = k;
        while(b){
            bitscanForward(b);
            b &= (b-1);
        }
        
    }
    stopMeasure();



//    std::cout << perft(*b, 6, true, true) << std::endl;
    
    
    return 0;
    
}



