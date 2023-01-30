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

//
// Created by Luecx on 25.01.2022.
//

#include "attacks.h"

using namespace bb;


U64 attacks::ROOK_ATTACKS  [N_SQUARES][4096]{};
U64 attacks::BISHOP_ATTACKS[N_SQUARES][ 512]{};
U64 attacks::PAWN_ATTACKS  [N_SQUARES][N_COLORS]{};


U64 populateMask(U64 mask, U64 index) {
    U64    res = 0;
    Square i   = 0;
    
    while (mask) {
        const Square bit = bitscanForward(mask);
        
        if (getBit(index, i)) {
            setBit(res, bit);
        }
        
        mask = lsbReset(mask);
        i++;
    }
    
    return res;
}

void attacks::init() {
    for (int n = 0; n < 64; n++) {
        const auto rook_shift = rookShifts[n];
        const auto bish_shift = bishopShifts[n];
        
        const U64  rook_entries   = (ONE << (64 - rook_shift));
        const U64  bish_entries   = (ONE << (64 - bish_shift));
        
        for (U64 i = 0; i < rook_entries; i++) {
            const U64 rel_occ      = populateMask(rookMasks[n], i);
#ifdef USE_PEXT
            const int index        = static_cast<int>(_pext_u64(rel_occ, rookMasks[n]));
#else
            const int index        = static_cast<int>((rel_occ * rookMagics[n]) >> rook_shift);
#endif
            ROOK_ATTACKS[n][index] = generateRookAttacks(n, rel_occ);
        }
        
        for (U64 i = 0; i < bish_entries; i++) {
            const U64 rel_occ        = populateMask(bishopMasks[n], i);
#ifdef USE_PEXT
            const int index          = static_cast<int>(_pext_u64(rel_occ, bishopMasks[n]));
#else
            const int index          = static_cast<int>((rel_occ * bishopMagics[n]) >> bish_shift);
#endif
            BISHOP_ATTACKS[n][index] = generateBishopAttacks(n, rel_occ);
        }
        
        // pawn attacks
        PAWN_ATTACKS[n][WHITE] = shiftNorthWest(ONE << n) | shiftNorthEast(ONE << n);
        PAWN_ATTACKS[n][BLACK] = shiftSouthWest(ONE << n) | shiftSouthEast(ONE << n);
    }
}



U64 attacks::generateSlidingAttacks(Square sq, Direction direction, U64 occ) {
    U64              res {0};
    
    static const U64 topBottom = RANK_1_BB | RANK_8_BB;
    static const U64 leftRight = FILE_A_BB | FILE_H_BB;
    
    if ((1ULL << sq) & RANK_1_BB && direction < -2) {
        return res;
    }
    if ((1ULL << sq) & RANK_8_BB && direction > 2) {
        return res;
    }
    if ((1ULL << sq) & FILE_A_BB
        && (direction == WEST || direction == SOUTH_WEST || direction == NORTH_WEST)) {
        return res;
    }
    if ((1ULL << sq) & FILE_H_BB
        && (direction == EAST || direction == SOUTH_EAST || direction == NORTH_EAST)) {
        return res;
    }
    
    while (true) {
        sq += direction;
        
        const U64 currentSq = (U64) 1 << sq;
        
        res |= currentSq;
        
        if (occ & currentSq) {
            return res;
        }
        if (abs(direction) == 8) {
            if (currentSq & topBottom) {
                return res;
            }
        } else if (abs(direction) == 1) {
            if (currentSq & leftRight) {
                return res;
            }
        } else {
            if (currentSq & OUTER_SQUARES_BB) {
                return res;
            }
        }
    }
}

U64 attacks::generateRookAttacks(Square square, U64 occupied) {
    return   generateSlidingAttacks(square, NORTH, occupied)
           | generateSlidingAttacks(square, EAST , occupied)
           | generateSlidingAttacks(square, WEST , occupied)
           | generateSlidingAttacks(square, SOUTH, occupied);
}

U64 attacks::generateBishopAttacks(Square square, U64 occupied) {
    return   generateSlidingAttacks(square, NORTH_WEST, occupied)
           | generateSlidingAttacks(square, NORTH_EAST, occupied)
           | generateSlidingAttacks(square, SOUTH_WEST, occupied)
           | generateSlidingAttacks(square, SOUTH_EAST, occupied);
}


