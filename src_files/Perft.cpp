//
// Created by finne on 5/16/2020.
//

#include "Perft.h"

using namespace std;


MoveList *buffer[100]{nullptr};
TranspositionTable tt{128};

//int generations;
//int checks;
//int moves;

//void printRes(){
//    std::cout << "generations: " << generations << " checks: " << checks << " moves: " << moves;
//}

U64 perft(Board &b, int depth, bool print, bool d1, int ply){
    
//    U64 zob = b.zobrist();
//    Entry* en = tt.get(zob);
//    if (en != nullptr && en->depth == depth && en->zobrist == zob) [[unlikely]] {
//        return tt.get(zob)->move;
//        //std::cout << zob << std::endl;
//    }
    
    if(buffer[depth] == nullptr){
        buffer[depth] = new MoveList();
    }
    
    if(ply == 0){
        tt.clear();
    }
    
    int i;
    U64 nodes = 0;
    
    if (depth == 0) return 1;
    
    
    b.getPseudoLegalMoves(buffer[depth]);
//    generations ++;
    
    for (i = 0; i < buffer[depth]->getSize(); i++) {
        
        Move m = buffer[depth]->getMove(i);
//        checks ++;
        
        
        
         if (!b.isLegal(m)) { continue; }
    
        if(d1 && depth == 1){
            nodes += 1;
        }else{
    
            
            b.move(m);
//            moves ++;
    
            U64 np = perft(b, depth - 1, false, d1, ply+1);
            //if(np == 0) np = 1;
    
            if (print) {
                std::cout << move::toString(m) << " " << np
                          << std::endl;
            }
    
            nodes += np;
            b.undoMove();
            
        }
    }
    
//    tt.put(zob, 0,nodes,0,depth);
    
    return nodes;
}