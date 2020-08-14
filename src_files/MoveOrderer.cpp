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

void MoveOrderer::setMovesPVSearch(move::MoveList *moves, move::Move hashMove, SearchData *sd, Board* board, Depth ply) {
    
    this->moves = moves;
    this->counter = 0;
    
    for(int i = 0; i < moves->getSize(); i++){
        move::Move m = moves->getMove(i);
        
        if(sameMove(m,hashMove)){
            moves->scoreMove(i, 1e6);
        }else if(isCapture(m)){
            //add mvv lva score here
            Score SEE = board->staticExchangeEvaluation(m);
            MoveScore mvvLVA = 10*(getCapturedPiece(m) % 6) - (getMovingPiece(m) % 6);
            if(SEE >= 0){
                if(mvvLVA == 0){
                    moves->scoreMove(i, 50000 + mvvLVA);
                }else{
                    moves->scoreMove(i, 100000 + mvvLVA);
                }
            }else{
                MoveScore ms = 8;
                ms += sd->getHistoryMoveScore(m, board->getActivePlayer());
                if (ply > 1) ms += sd->getCounterMoveHistoryScore(board->getPreviousMove(), m);
                moves->scoreMove(i, ms);
            }
        }else if(isPromotion(m)){
            MoveScore mvvLVA = (getCapturedPiece(m) % 6) - (getMovingPiece(m) % 6);
            moves->scoreMove(i, 40000 + mvvLVA + promotionPiece(m));
        }else if (sd->isKiller(m, ply, board->getActivePlayer())){
            moves->scoreMove(i, 30000);
        }else{
            
            MoveScore ms = 8;
            ms += sd->getHistoryMoveScore(m, board->getActivePlayer());
            if (ply>1) ms += sd->getCounterMoveHistoryScore(board->getPreviousMove(), m);
            moves->scoreMove(i, ms);
            
            
        }
        
    }
    
    
}

void MoveOrderer::setMovesQSearch(move::MoveList *moves) {
    this->moves = moves;
    this->counter = 0;

    for(int i = 0; i < moves->getSize(); i++){
        move::Move m = moves->getMove(i);

        MoveScore mvvLVA = 10*(getCapturedPiece(m) % 6) - (getMovingPiece(m) % 6);
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
    
        if(moves->getScore(i) > moves->getScore(bestIndex)){
            bestIndex = i;
        }
    }
    moves->scoreMove(bestIndex, 0);
    counter ++;
    return moves->getMove(bestIndex);
    
}


