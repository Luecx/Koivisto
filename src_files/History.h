
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                                   by. Kim Kahre and Finn Eggers                                  *
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

using namespace bb;
using namespace move;

struct SearchData {

    Move     bestMove = 0;
    // Effort spent
    int64_t  spentEffort[N_SQUARES][N_SQUARES]                                   = {0};
    // EvalImprovement
    int      maxImprovement[N_SQUARES][N_SQUARES]                                = {0};
    // capture history table (side-from-to)
    int      captureHistory[N_COLORS][N_SQUARES * N_SQUARES]                     = {0};
    // threat history
    int      th[N_COLORS][N_SQUARES][N_SQUARES * N_SQUARES]                      = {0};
    // counter move history table (prev_piece, prev_to, side, move_piece, move_to)
    int      cmh[N_PIECE_TYPES * N_SQUARES][N_COLORS][N_PIECE_TYPES * N_SQUARES] = {0};
    // killer history
    int      fmh[N_PIECE_TYPES * N_SQUARES][N_COLORS][N_PIECE_TYPES * N_SQUARES] = {0};
    // kill table, +2 used to make sure we can always reset +2
    Move     killer[N_COLORS][MAX_INTERNAL_PLY + 2][2]                           = {0};
    // threat data
    int      threatCount[MAX_INTERNAL_PLY][N_COLORS]                             = {0};
    Square   mainThreat[MAX_INTERNAL_PLY]                                        = {0}; 
    // played moves
    Move     playedMoves[MAX_INTERNAL_PLY]                                       = {0};
    // eval history across plies
    Score    eval[N_COLORS][MAX_INTERNAL_PLY]                                    = {0};
    bool     sideToReduce;
    bool     reduce;
    bool     targetReached                                                       = 1;

    int      getHistories(Move m, Color side, Move previous, Move followup, Square threatSquare);

    void     setKiller(Move move, Depth ply, Color color);

    int      isKiller(Move move, Depth ply, Color color);

    void     setHistoricEval(Score eval, Color color, Depth ply);

    bool     isImproving(Score eval, Color color, Depth ply);
} __attribute__((aligned(64)));


#endif    // KOIVISTO_HISTORY_H
