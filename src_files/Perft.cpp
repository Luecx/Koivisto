//
// Created by finne on 5/16/2020.
//

#include "Perft.h"

using namespace std;


std::vector<Move> *buffer[100]{nullptr};

int generations;
int checks;
int moves;

void printRes(){
    std::cout << "generations: " << generations << " checks: " << checks << " moves: " << moves;
}

U64 perft(Board &b, int depth, bool print, bool d1){
    
    
    if(buffer[depth] == nullptr){
        buffer[depth] = new std::vector<Move>();
    }
    
    
    int i;
    U64 nodes = 0;
    
    if (depth == 0) return 1;
    
    std::vector<Move> ar = *(buffer[depth]);
    
    b.getPseudoLegalMoves(&ar);
    generations ++;
    
    for (i = 0; i < (ar).size(); i++) {
        
        Move m = (ar).at(i);
        checks ++;
        if (!b.isLegal(m)) { continue; }
    
        if(d1 && depth == 1){
            nodes += 1;
        }else{
    
            
            b.move(m);
            moves ++;
    
            U64 np = perft(b, depth - 1, false, d1);
            //if(np == 0) np = 1;
    
            if (print) {
                std::cout << move::toString(m) << " " << np
                          << std::endl;
            }
    
            nodes += np;
            b.undoMove();
            
        }
        
        
        
        
    }
    return nodes;
}