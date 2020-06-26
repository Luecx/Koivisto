//
// Created by finne on 6/3/2020.
//

#ifndef KOIVISTO_MOVEORDERER_H
#define KOIVISTO_MOVEORDERER_H


#include "Move.h"
#include "History.h"
#include "Board.h"

class MoveOrderer {

    private:
        move::MoveList* moves;
        int counter;
    
    public:
        MoveOrderer();
        
        virtual ~MoveOrderer();
        
        void setMovesPVSearch(move::MoveList* moves, move::Move hashMove, SearchData *sd, Board* board, Depth ply);
        
        void setMovesQSearch(move::MoveList* moves);
        
        bool hasNext();
        
        move::Move next();
};


#endif //KOIVISTO_MOVEORDERER_H
