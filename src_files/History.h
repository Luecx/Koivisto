//
// Created by kimka on 6.6.2020.
//

#ifndef KOIVISTO_HISTORY_H
#define KOIVISTO_HISTORY_H

#include "eval.h"
#include "Move.h"
#include "Bitboard.h"

using namespace bb;
using namespace move;



struct SearchData {
    
    MoveList   **moves;
    Evaluator  evaluator{};
    
    int   history[2][64][64]   = {0};  //history table (from-to)
    int   cmh[6][64][2][6][64] = {0};  //counter move history table (prev_piece, prev_to, side, move_piece, move_to)
    Move  killer[2][MAX_PLY]   = {0};
    Score eval[2][MAX_PLY]     = {0};
    bool  sideToReduce;
    
    
    SearchData();
    
    virtual ~SearchData();
    
    void addHistoryScore(Move m, Depth depth, MoveList *mv, bool side);
    
    MoveScore getHistoryMoveScore(Move m, bool side) const;
    
    void setKiller(Move move, Depth ply, Color color);
    
    bool isKiller(Move move, Depth ply, Color color);
    
    void setHistoricEval(Score ev, Color color, Depth ply);
    
    bool isImproving(Score ev, Color color, Depth ply) const;
    
    void addCounterMoveHistoryScore(Move previous, Move m, Depth depth, MoveList *mv);
    
    MoveScore getCounterMoveHistoryScore(Move previous, Move m) const;
};


/**
 * data about each thread
 */
struct ThreadData {
    int threadID = 0;
    int nodes = 0;
    int seldepth = 0;
    int tbhits = 0;
    
    SearchData* searchData;
    
    ThreadData(int threadId);
};


#endif //KOIVISTO_HISTORY_H

