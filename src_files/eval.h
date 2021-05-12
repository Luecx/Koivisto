
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
#ifndef KOIVISTO_EVAL_H
#define KOIVISTO_EVAL_H

#include "Bitboard.h"
#include "Board.h"
#include "psqt.h"

extern EvalScore bishop_pawn_same_color_table_o[9];
extern EvalScore bishop_pawn_same_color_table_e[9];

extern EvalScore* evfeatures[];
extern EvalScore  hangingEval[5];
extern EvalScore  pinnedEval[15];
extern EvalScore* mobilities[N_PIECE_TYPES];
extern int        mobEntryCount[N_PIECE_TYPES];
extern float* phaseValues;
extern EvalScore kingSafetyTable[100];
extern EvalScore passer_rank_n[2*N_RANKS];

extern int lazyEvalAlphaBound;
extern int lazyEvalBetaBound ;

bool isOutpost          (Square s, Color c, U64 opponentPawns, U64 pawnCover);
bool hasMatingMaterial  (Board* b, bool side);
void addToKingSafety    (U64 attacks, U64 kingZone, int& pieceCount, int& valueOfAttacks, int factor);

struct EvalData{
    
    U64 attacks     [N_COLORS][N_PIECE_TYPES]{};
    U64 kingZone    [N_COLORS]{};
    U64 allAttacks  [N_COLORS]{};
    
};

class Evaluator {
    public:

    EvalData evalData;
    float phase;

    EvalScore computePinnedPieces(Board* b, Color color);

    EvalScore computeHangingPieces(Board* b);

    bb::Score evaluate(Board* b, Score alpha = -MAX_MATE_SCORE, Score beta = +MAX_MATE_SCORE);

    bb::Score evaluateTempo(Board* b);
    
    float getPhase();
};

void printEvaluation(Board* b);

#endif    // KOIVISTO_EVAL_H
