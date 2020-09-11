//
// Created by finne on 6/3/2020.
//

#ifndef KOIVISTO_MOVEORDERER_H
#define KOIVISTO_MOVEORDERER_H

#include "Board.h"
#include "History.h"
#include "Move.h"

class MoveOrderer {
   private:
    move::MoveList *moves;
    int counter;

   public:
    bool skip;

    MoveOrderer();

    virtual ~MoveOrderer();

    void setMovesPVSearch(move::MoveList *p_moves,
                          move::Move hashMove,
                          SearchData *sd,
                          Board *board,
                          Depth ply);

    void setMovesQSearch(move::MoveList *p_moves, Board *b);

    bool hasNext();

    move::Move next();
};

#endif  // KOIVISTO_MOVEORDERER_H
