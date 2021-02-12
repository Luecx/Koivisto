
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
extern EvalScore* mobilities[6];
extern int        mobEntryCount[6];
extern float* phaseValues;
extern EvalScore kingSafetyTable[100];
extern EvalScore passer_rank_n[16];

inline bool isOutpost(Square s, Color c, U64 opponentPawns, U64 pawnCover){
    U64 sq = ONE << s;


    return ((sq & pawnCover) && (c == WHITE && ((whitePassedPawnMask[s] & ~FILES[fileIndex(s)]) & opponentPawns) == 0) ||
                                (c == BLACK && ((blackPassedPawnMask[s] & ~FILES[fileIndex(s)]) & opponentPawns) == 0) );

//    if (c == WHITE) {
//        if (((whitePassedPawnMask[s] & ~FILES[fileIndex(s)]) & opponentPawns) == 0 && (sq & pawnCover)) {
//            return true;
//        }
//    } else {
//        if (((blackPassedPawnMask[s] & ~FILES[fileIndex(s)]) & opponentPawns) == 0 && (sq & pawnCover)) {
//            return true;
//        }
//    }
//    return false;
}
inline bool hasMatingMaterial(Board* b, bool side){
    return (
        (  b->getPieces()[QUEEN + side * 6]
         | b->getPieces()[ROOK  + side * 6]
         | b->getPieces()[PAWN  + side * 6])
        || (bitCount(
                b->getPieces()[BISHOP + side * 6]|
                b->getPieces()[KNIGHT + side * 6]) > 1
            && b->getPieces()[BISHOP + side * 6]));
//        return true;
//    return false;
}
inline void addToKingSafety(U64 attacks, U64 kingZone, int& pieceCount, int& valueOfAttacks, int factor){
    valueOfAttacks += factor * bitCount(attacks & kingZone);
}

class Evaluator {
    public:

    float phase;

    EvalScore computePinnedPieces(Board* b, Color color);

    EvalScore computeHangingPieces(Board* b);

    bb::Score evaluate(Board* b);

    bb::Score evaluateTempo(Board* b);

    /**
     * returns the phase of the last calculation
     * @return
     */
    float getPhase();
};

void printEvaluation(Board* b);

#endif    // KOIVISTO_EVAL_H
