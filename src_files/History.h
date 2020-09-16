//
// Created by kimka on 6.6.2020.
//

#ifndef KOIVISTO_HISTORY_H
#define KOIVISTO_HISTORY_H

#include "Bitboard.h"
#include "Move.h"
#include "eval.h"

using namespace bb;
using namespace move;

struct SearchData {

    Move bestMove = 0;

    MoveList** moves;
    Evaluator  evaluator {};

    int   history[2][64][64]   = {0};    // history table (from-to)
    int   cmh[6][64][2][6][64] = {0};    // counter move history table (prev_piece, prev_to, side, move_piece, move_to)
    Move  killer[2][MAX_PLY]   = {0};
    Score eval[2][MAX_PLY]     = {0};
    bool  sideToReduce;

    SearchData();

    virtual ~SearchData();

    void addHistoryScore(Move m, Depth depth, MoveList* mv, bool side);

    MoveScore getHistoryMoveScore(Move m, bool side);

    void setKiller(Move move, Depth ply, Color color);

    bool isKiller(Move move, Depth ply, Color color);

    void setHistoricEval(Score eval, Color color, Depth ply);

    bool isImproving(Score eval, Color color, Depth ply);

    void addCounterMoveHistoryScore(Move previous, Move m, Depth depth, MoveList* mv);

    MoveScore getCounterMoveHistoryScore(Move previous, Move m);
};

/**
 * data about each thread
 */
struct ThreadData {
    int threadID = 0;
    U64 nodes    = 0;
    int seldepth = 0;
    int tbhits   = 0;

    SearchData* searchData;

    ThreadData(int threadId);
};

#endif    // KOIVISTO_HISTORY_H
