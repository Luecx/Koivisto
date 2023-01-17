
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

#include "bitboard.h"
#include "move.h"

#include <cstring>

// clang-format off
#define SPENT_EFFORT(sd, sq_a, sq_b)    ((sd)->spentEffort                                           \
                                                  [(sq_a)]                                           \
                                                  [(sq_b)])
#define MAX_IMPROVEMENT(sd, sq_a, sq_b) ((sd)->maxImprovement                                        \
                                                  [(sq_a)]                                           \
                                                  [(sq_b)])
#define CAPTURE_HISTORY(sd, side, m)    ((sd)->captureHistory                                        \
                                                  [(side)]                                           \
                                                  [(getSqToSqFromCombination((m)))])
#define THREAT_HISTORY(sd,side,sq,m)    ((sd)->th [(side)]                                           \
                                                  [(sq)]                                             \
                                                  [(getSqToSqFromCombination((m)))])
#define CMH(sd, prev, side, m)          ((sd)->cmh[getPieceTypeSqToCombination((prev))]              \
                                                  [(side)]                                           \
                                                  [(getPieceTypeSqToCombination((m)))])
#define FMH(sd, followup, side, m)      ((sd)->fmh[getPieceTypeSqToCombination((followup))]          \
                                                  [(side)]                                           \
                                                  [(getPieceTypeSqToCombination((m)))])
#define KILLER(sd, side, ply, id)       ((sd)->killer                                                \
                                                  [(side)]                                           \
                                                  [(ply)]                                            \
                                                  [(id)])
#define THREATS(sd, ply, side)         ((sd)->threats                                                \
                                                  [(ply)]                                            \
                                                  [(side)])
#define THREAT_COUNT(sd, ply, side)     ((sd)->threatCount                                           \
                                                  [(ply)]                                            \
                                                  [(side)])
#define MAIN_THREAT(sd, ply)            ((sd)->mainThreat                                            \
                                                  [(ply)])
#define EVAL_HISTORY(sd, side, ply)     ((sd)->eval                                                  \
                                                  [(side)]                                           \
                                                  [(ply)])
#define KILLER1(sd, side, ply)          (KILLER(sd, side, ply, 0))
#define KILLER2(sd, side, ply)          (KILLER(sd, side, ply, 1))

struct SearchData {
    move::Move     bestMove = 0;
    // Effort spent
    int64_t    spentEffort    [bb::N_SQUARES                        ]
                              [bb::N_SQUARES                        ] = {0};
    // EvalImprovement
    int        maxImprovement [bb::N_SQUARES                        ]
                              [bb::N_SQUARES                        ] = {0};
    // capture history table (side-from-to)
    int        captureHistory [bb::N_COLORS                         ]
                              [bb::N_SQUARES * bb::N_SQUARES        ] = {0};
    // threat history
    int        th             [bb::N_COLORS                         ]
                              [bb::N_SQUARES + 1                    ]
                              [bb::N_SQUARES * bb::N_SQUARES        ] = {0};
    // counter move history table (prev_piece, prev_to, side, move_piece, move_to)
    int        cmh            [bb::N_PIECE_TYPES * bb::N_SQUARES    ]
                              [bb::N_COLORS                         ]
                              [bb::N_PIECE_TYPES * bb::N_SQUARES    ] = {0};
    // followup move history
    int        fmh            [bb::N_PIECE_TYPES * bb::N_SQUARES + 1]
                              [bb::N_COLORS                         ]
                              [bb::N_PIECE_TYPES * bb::N_SQUARES    ] = {0};
    // kill table, +2 used to make sure we can always reset +2
    move::Move killer         [bb::N_COLORS                         ]
                              [bb::MAX_INTERNAL_PLY + 2             ]
                              [2                                    ] = {0};
    // threat data
    bb::U64    threats        [bb::MAX_INTERNAL_PLY                 ]
                              [bb::N_COLORS                         ] = {0};
    int        threatCount    [bb::MAX_INTERNAL_PLY                 ]
                              [bb::N_COLORS                         ] = {0};
    bb::Square mainThreat     [bb::MAX_INTERNAL_PLY                 ] = {0};
    // eval history across plies
    bb::Score  eval           [bb::N_COLORS                         ]
                              [bb::MAX_INTERNAL_PLY                 ] = {0};
    bool     sideToReduce{};
    bool     reduce{};
    bool     targetReached{1};
    // clang-format on

    // function to extract histories
    [[nodiscard]] int getHistories(move::Move m, bb::Color side, move::Move previous,
                                   move::Move followup, bb::Square threatSquare) const;

    // function to set killers
    void setKiller(move::Move move, bb::Depth ply, bb::Color color);

    // checks if a given move is a killer move
    [[nodiscard]] int isKiller(move::Move move, bb::Depth ply, bb::Color color) const;

    // sets historic evals across plies and saves them
    void setHistoricEval(bb::Score eval, bb::Color color, bb::Depth ply);

    // clears the history. This involves resetting all history tables
    void clear();

    // checks if the evaluation is improving using the historic evals
    [[nodiscard]] bool isImproving(bb::Score eval, bb::Color color, bb::Depth ply) const;
} __attribute__((aligned(64)));

// clang-format on

#endif    // KOIVISTO_HISTORY_H
