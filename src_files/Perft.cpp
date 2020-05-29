//
// Created by finne on 5/16/2020.
//

#include "Perft.h"

using namespace std;


MoveList **buffer;
TranspositionTable *tt;

//int generations;
//int checks;
//int moves;

//void perft_res(){
//    std::cout << "generations: " << generations << " checks: " << checks << " moves: " << moves;
//}


void perft_prepare(bool hash){
    if(hash)
        tt = new TranspositionTable(512);
    
    buffer = new MoveList*[100];
    
    for(int i = 0; i < 100; i++){
        buffer[i] = new MoveList();
    }
    
}

void perft_clean(){
    
    if(tt != nullptr)
        delete tt;
    
    for(int i = 0; i < 100; i++){
        delete buffer[i];
    }
    
    delete buffer;
}

void perft_res(){

}


U64 perft(Board &b, int depth, bool print, bool d1, bool hash, int ply){
    
    U64 zob;
    if(hash){
    
        if(ply == 0){
            tt->clear();
        }
        
        zob = b.zobrist();
        Entry* en = tt->get(zob);
        if (en != nullptr && en->depth == depth && en->zobrist == zob) [[unlikely]] {
            return tt->get(zob)->move;
        }
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
    
            U64 np = perft(b, depth - 1, false, d1, hash,ply+1);
            //if(np == 0) np = 1;
    
            if (print) {
                std::cout << move::toString(m) << " " << np
                          << std::endl;
            }
    
            nodes += np;
            b.undoMove();
            
        }
    }
    
    if(hash){
        tt->put(zob, 0,nodes,0,depth);
    }
    
    
    return nodes;
}