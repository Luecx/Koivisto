//
// Created by finne on 6/3/2020.
//

#include "MoveOrderer.h"

using namespace move;

MoveOrderer::MoveOrderer() {

}

MoveOrderer::~MoveOrderer() {

}

void MoveOrderer::setMovesPVSearch(move::MoveList *moves, move::Move hashMove) {
    
    this->moves = moves;
    this->counter = 0;
    
    for(int i = 0; i < moves->getSize(); i++){
        move::Move m = moves->getMove(i);
        
        
        if(m == hashMove){
            moves->scoreMove(i, 128);
        }else if(isCapture(m)){
            //add mvv lva score here
            MoveScore mvvLVA = (getCapturedPiece(m) % 6) - (getMovingPiece(m) % 6);
            moves->scoreMove(i, 64 + mvvLVA);
        }else if(isPromotion(m)){
            MoveScore mvvLVA = (getCapturedPiece(m) % 6) - (getMovingPiece(m) % 6);
            moves->scoreMove(i, 32 + mvvLVA);
        }else{
            moves->scoreMove(i, 8);
        }
        
    }
    
}

void MoveOrderer::setMovesQSearch(move::MoveList *moves) {

}

bool MoveOrderer::hasNext() {
    return counter < moves->getSize();
}

move::Move MoveOrderer::next() {
    
    int bestIndex = 0;
    //Move best = moves->getMove(0);
    for(int i = 1; i < moves->getSize(); i++){
        if(moves->getMove(i) > moves->getMove(bestIndex)){
            bestIndex = i;
        }
    }
    moves->scoreMove(bestIndex, 0);
    counter ++;
    return moves->getMove(bestIndex);
    
}


