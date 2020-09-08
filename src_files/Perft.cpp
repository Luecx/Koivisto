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

/**
 * called at the start of the program
 * @param hash
 */
void perft_init(bool hash){
    if(hash)
        tt = new TranspositionTable(512);
    
    buffer = new MoveList*[100];
    
    for(int i = 0; i < 100; i++){
        buffer[i] = new MoveList();
    }
    
}

/**
 * called at the end of the program.
 */
void perft_cleanUp(){
    
    if(tt != nullptr)
        delete tt;
    
    for(int i = 0; i < 100; i++){
        delete buffer[i];
    }
    
    delete[] buffer;
}


/**
 * does nothing yet.
 * Supposed to print an overview of the previous perft call.
 */
void perft_res(){

}


U64 perft(Board *b, int depth, bool print, bool d1, bool hash, int ply){
    
    U64 zob = ZERO;
    if(hash){
    
        if(ply == 0){
            tt->clear();
        }
        
        zob = b->zobrist();
        Entry en = tt->get(zob);
        if (en.depth == depth && en.zobrist == zob){
            return en.move;
        }
    }
    
    int i;
    U64 nodes = 0;
    
    if (depth == 0) return 1;
    
    
    b->getPseudoLegalMoves(buffer[depth]);
//    generations ++;
    
    for (i = 0; i < buffer[depth]->getSize(); i++) {
        
        Move m = buffer[depth]->getMove(i);
//        checks ++;
        
        
        
         if (!b->isLegal(m)) { continue; }
    
        if(d1 && depth == 1){
            nodes += 1;
        }else{
    
    
            b->move(m);
            
    
            U64 np = perft(b, depth - 1, false, d1, hash,ply+1);
    
            if (print) {
                std::cout << move::toString(m) << " " << np
                          << std::endl;
            }
    
            nodes += np;
            b->undoMove();
    
    
        }
    }
    
    if(hash){
        tt->put(zob, 0,nodes,0,depth);
    }
    
    
    return nodes;
}