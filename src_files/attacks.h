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


#ifndef KOIVISTO_ATTACKS_H
#define KOIVISTO_ATTACKS_H

#include "Bitboard.h"

namespace attacks{


extern const bb::U64 bishopMasks    [bb::N_SQUARES];
extern const bb::U64 rookMasks      [bb::N_SQUARES];
extern const int     bishopShifts   [bb::N_SQUARES];
extern const int     rookShifts     [bb::N_SQUARES];
extern const bb::U64 bishopMagics   [bb::N_SQUARES];
extern const bb::U64 rookMagics     [bb::N_SQUARES];

extern const bb::U64 KING_ATTACKS   [bb::N_SQUARES];
extern const bb::U64 KNIGHT_ATTACKS [bb::N_SQUARES];

extern       bb::U64 *ROOK_ATTACKS  [bb::N_SQUARES];
extern       bb::U64 *BISHOP_ATTACKS[bb::N_SQUARES];

void init();
void cleanUp();

bb::U64 generateSlidingAttacks(bb::Square sq, bb::Direction direction, bb::U64 occ);
bb::U64 generateRookAttacks   (bb::Square sq, bb::U64 occupied);
bb::U64 generateBishopAttacks (bb::Square sq, bb::U64 occupied);

/**
 * looks up the rook attack for a rook on the given square.
 * It returns a bitmap with all attackable squares highlighted.
 * @param index
 * @param occupied
 * @return
 */
inline bb::U64       lookUpRookAttacks(bb::Square index, bb::U64 occupied) {
    return ROOK_ATTACKS[index][static_cast<int>((occupied & rookMasks[index]) * rookMagics[index]
                                                >> (rookShifts[index]))];
}

/**
 * looks up the xray rook attack for a rook on the given square
 * It returns a bitmap with all attackable squares highlighted including those after the first
 * blockers.
 */
inline bb::U64 lookUpRookXRayAttack(bb::Square index, bb::U64 occupied, bb::U64 opponent) {
    bb::U64 attacks  = lookUpRookAttacks(index, occupied);
    bb::U64 blockers = opponent & attacks;
    return attacks ^ lookUpRookAttacks(index, occupied ^ blockers);
}

/**
 * looks up the bishop attack using magic bitboards
 * @param index
 * @param occupied
 * @return
 */
inline bb::U64 lookUpBishopAttacks(bb::Square index, bb::U64 occupied) {
    return BISHOP_ATTACKS[index][static_cast<int>(
        (occupied & bishopMasks[index]) * bishopMagics[index] >> (bishopShifts[index]))];
}

/**
 * looks up the xray bishop attack for a bishop on the given square
 * It returns a bitmap with all attackable squares highlighted including those after the first
 * blockers.
 */
inline bb::U64 lookUpBishopXRayAttack(bb::Square index, bb::U64 occupied, bb::U64 opponent) {
    bb::U64 attacks  = lookUpBishopAttacks(index, occupied);
    bb::U64 blockers = opponent & attacks;
    return attacks ^ lookUpBishopAttacks(index, occupied ^ blockers);
}


}

#endif    // KOIVISTO_ATTACKS_H
