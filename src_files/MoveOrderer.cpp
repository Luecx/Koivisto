//
// Created by finne on 6/3/2020.
//

#include "MoveOrderer.h"
#include "History.h"

using namespace move;

MoveOrderer::MoveOrderer() {

}

MoveOrderer::~MoveOrderer() {

}

void MoveOrderer::setMovesPVSearch(move::MoveList *moves, move::Move hashMove, SearchData *sd) {
    
    this->moves = moves;
    this->counter = 0;
    
    for(int i = 0; i < moves->getSize(); i++){
        move::Move m = moves->getMove(i);
        
        
        if(m == hashMove){
            moves->scoreMove(i, 255);
        }else if(isCapture(m)){
            //add mvv lva score here
            MoveScore mvvLVA = (getCapturedPiece(m) % 6) - (getMovingPiece(m) % 6);
            moves->scoreMove(i, 240 + mvvLVA);
        }else if(isPromotion(m)){
            MoveScore mvvLVA = (getCapturedPiece(m) % 6) - (getMovingPiece(m) % 6);
            moves->scoreMove(i, 220 + mvvLVA);
        }else{
            moves->scoreMove(i, 8 + sd->history[getSquareFrom(m)][getSquareTo(m)]/3);
        }
        
    }
    
}

void MoveOrderer::setMovesQSearch(move::MoveList *moves) {
    this->moves = moves;
    this->counter = 0;

    for(int i = 0; i < moves->getSize(); i++){
        move::Move m = moves->getMove(i);

        MoveScore mvvLVA = (getCapturedPiece(m) % 6) - (getMovingPiece(m) % 6);
        moves->scoreMove(i, 240 + mvvLVA);
    }
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


