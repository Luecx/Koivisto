
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

#include "Bitboard.h"

using namespace bb;

U64* bb::ROOK_ATTACKS  [N_SQUARES]{};
U64* bb::BISHOP_ATTACKS[N_SQUARES]{};

U64 bb::ALL_HASHES[N_PIECES][N_SQUARES] = {};

U64 bb::IN_BETWEEN_SQUARES[N_SQUARES][N_SQUARES];
U64 bb::PASSED_PAWN_MASK[N_COLORS][N_SQUARES];

U64 bb::seed = 1293812938;


void bb::init() {
    
    generateZobristKeys();
    generateData();
}

void bb::cleanUp() {
    for (int i = 0; i < N_SQUARES; i++) {
        delete[] ROOK_ATTACKS[i];
        ROOK_ATTACKS[i] = nullptr;
        delete[] BISHOP_ATTACKS[i];
        BISHOP_ATTACKS[i] = nullptr;
    }
}

U64 bb::randU64() {
    seed ^= seed << 13;
    seed ^= seed >> 7;
    seed ^= seed << 17;
    return seed;
}

U64 bb::generateSlidingAttacks(Square sq, Direction direction, U64 occ) {
    U64 res {0};
    
    static const U64 topBottom = RANK_1_BB | RANK_8_BB;
    static const U64 leftRight = FILE_A_BB | FILE_H_BB;
    
    if ((1ULL << sq) & RANK_1_BB && direction < -2) {
        return res;
    }
    if ((1ULL << sq) & RANK_8_BB && direction > 2) {
        return res;
    }
    if ((1ULL << sq) & FILE_A_BB && (direction == WEST || direction == SOUTH_WEST || direction == NORTH_WEST)) {
        return res;
    }
    if ((1ULL << sq) & FILE_H_BB && (direction == EAST || direction == SOUTH_EAST || direction == NORTH_EAST)) {
        return res;
    }
    
    while (true) {
        sq += direction;
        
        U64 currentSq = (U64) 1 << sq;
        
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
            if (currentSq & CIRCLE_A_BB) {
                return res;
            }
        }
    }
}

void bb::generateData() {
    for (int n = 0; n < 64; n++) {
        ROOK_ATTACKS[n]   = new U64[ONE << (64 - rookShifts[n])];
        BISHOP_ATTACKS[n] = new U64[ONE << (64 - bishopShifts[n])];
        
        for (int i = 0; i < pow(2, 64 - rookShifts[n]); i++) {
            U64 rel_occ            = populateMask(rookMasks[n], i);
            int index              = static_cast<int>((rel_occ * rookMagics[n]) >> rookShifts[n]);
            ROOK_ATTACKS[n][index] = generateRookAttack(n, rel_occ);
        }
        
        for (int i = 0; i < pow(2, 64 - bishopShifts[n]); i++) {
            U64 rel_occ              = populateMask(bishopMasks[n], i);
            int index                = static_cast<int>((rel_occ * bishopMagics[n]) >> bishopShifts[n]);
            BISHOP_ATTACKS[n][index] = generateBishopAttack(n, rel_occ);
        }
    }
    
    // in between squares
    for (Square n = A1; n <= H8; n++) {
        
        for (Square i = A1; i <= H8; i++) {
            if (i == n)
                continue;
            
            U64 m   = ZERO;
            U64 occ = ZERO;
            setBit(occ, n);
            setBit(occ, i);
            
            Direction r = i - n;
            
            if (rankIndex(n) == rankIndex(i)) {
                m = generateSlidingAttacks(n, EAST * r / abs(r), occ);
            } else if (fileIndex(n) == fileIndex(i)) {
                m = generateSlidingAttacks(n, NORTH * r / abs(r), occ);
            } else if (diagonalIndex(n) == diagonalIndex(i)) {
                m = generateSlidingAttacks(n, NORTH_EAST * r / abs(r), occ);
            } else if (antiDiagonalIndex(n) == antiDiagonalIndex(i)) {
                m = generateSlidingAttacks(n, NORTH_WEST * r / abs(r), occ);
            }
            
            m &= ~occ;
            
            IN_BETWEEN_SQUARES[n][i] = m;
        }
    }
    
    // passed pawn mask
    for (Color c:{WHITE,BLACK}){
        for(Square s = A1; s <= H8; s++){
            U64 h = ONE << s;
            h |= shiftWest(h) | shiftEast(h);
            if(c == WHITE){
                PASSED_PAWN_MASK[c][s] = fillNorth(shiftNorth(h));
            }else{
                PASSED_PAWN_MASK[c][s] = fillSouth(shiftSouth(h));
            }
        }
    }
}

/**
 * populates the given bitboard with 1 and zeros.
 * The population is unique for the given index.
 * A 1 or 0 will only be placed where there is a 1 in the mask.
 * @param mask
 * @param index
 * @return
 */
U64 bb::populateMask(U64 mask, U64 index) {
    
    U64    res = 0;
    Square i   = 0;
    
    while (mask) {
        Square bit = bitscanForward(mask);
        
        if (getBit(index, i)) {
            setBit(res, bit);
        }
        
        mask = lsbReset(mask);
        i++;
    }
    
    return res;
}

void bb::generateZobristKeys() {
    
    for (int i = 0; i < 6; i++) {
        for (int n = 0; n < 64; n++) {
            ALL_HASHES[i][n]     = randU64();
            ALL_HASHES[i + 8][n] = randU64();
        }
    }
}

U64 bb::generateRookAttack(Square sq, U64 occupied) {
    return generateSlidingAttacks(sq, NORTH, occupied) | generateSlidingAttacks(sq, EAST, occupied)
           | generateSlidingAttacks(sq, WEST, occupied) | generateSlidingAttacks(sq, SOUTH, occupied);
}

U64 bb::generateBishopAttack(Square sq, U64 occupied) {
    return generateSlidingAttacks(sq, NORTH_WEST, occupied) | generateSlidingAttacks(sq, NORTH_EAST, occupied)
           | generateSlidingAttacks(sq, SOUTH_WEST, occupied) | generateSlidingAttacks(sq, SOUTH_EAST, occupied);
}

void bb::printBitmap(U64 bb) {
    for (int i = 7; i >= 0; i--) {
        for (int n = 0; n < 8; n++) {
            if ((bb >> (i * 8 + n)) & (U64) 1) {
                std::cout << "1";
            } else {
                std::cout << "0";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}
