//
// Created by kimka on 6.6.2020.
//

#ifndef KOIVISTO_HISTORY_H
#define KOIVISTO_HISTORY_H

#include "Move.h"
#include "Bitboard.h"

using namespace bb;
using namespace move;

struct SearchData{

    //history table (from-to)
    int history[2][64][64] = {0};

    Move killer [2][MAX_PLY] = {0};

    Score eval[2][MAX_PLY] = {0};
    
    void addHistoryScore(Move m, Depth depth, MoveList *mv, bool side);

    MoveScore getHistoryMoveScore(Move m, bool side);

    void setKiller(Move move, Depth ply, Color color);
    bool isKiller(Move move, Depth ply, Color color);

    void setHistoricEval(Score eval, Color color, Depth ply);
    bool isImproving(Score eval, Color color, Depth ply);
};


#endif //KOIVISTO_HISTORY_H

