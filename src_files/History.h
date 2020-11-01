
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                           by. Kim Kahre, Finn Eggers and Eugenio Bruno                           *
 *                                                                                                  *
 *                 Koivisto is free software: you can redistribute it and/or modify                 *
 *               it under the terms of the GNU General Public License as published by               *
 *                 the Free Software Foundation, either version 3 of the License, or                *
 *                                (at your option) any later version.                               *
 *                    Koivisto is distributed in the hope that it will be useful,                   *
 *                  but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 *                   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                  *
 *                           GNU General Public License for more details.                           *
 *                 You should have received a copy of the GNU General Public License                *
 *                 along with Koivisto.  If not, see <http://www.gnu.org/licenses/>.                *
 *                                                                                                  *
 ****************************************************************************************************/

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
