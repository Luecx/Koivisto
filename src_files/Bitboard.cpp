
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

#include "bitboard.h"
#include "attacks.h"

namespace bb {
U64 ALL_HASHES[N_PIECES][N_SQUARES] = {};
U64 IN_BETWEEN_SQUARES[N_SQUARES][N_SQUARES];

U64 seed = 1293812938;


void init() {
    generateZobristKeys();
    generateData();
}

U64 randU64() {
    seed ^= seed << 13;
    seed ^= seed >> 7;
    seed ^= seed << 17;
    return seed;
}

void generateData() {
    // in between squares
    for (Square n = A1; n <= H8; n++) {
        for (Square i = A1; i <= H8; i++) {
            if (i == n)
                continue;

            U64 m   = ZERO;
            U64 occ = ZERO;
            setBit(occ, n);
            setBit(occ, i);

            const Direction r    = i - n;
            const Direction sign = r / abs(r);

            if (rankIndex(n) == rankIndex(i)) {
                m = attacks::generateSlidingAttacks(n, EAST * sign, occ);
            } else if (fileIndex(n) == fileIndex(i)) {
                m = attacks::generateSlidingAttacks(n, NORTH * sign, occ);
            } else if (diagonalIndex(n) == diagonalIndex(i)) {
                m = attacks::generateSlidingAttacks(n, NORTH_EAST * sign, occ);
            } else if (antiDiagonalIndex(n) == antiDiagonalIndex(i)) {
                m = attacks::generateSlidingAttacks(n, NORTH_WEST * sign, occ);
            }

            m &= ~occ;

            IN_BETWEEN_SQUARES[n][i] = m;
        }
    }
}


void generateZobristKeys() {
    for (int i = 0; i < 6; i++) {
        for (int n = 0; n < 64; n++) {
            ALL_HASHES[i][n]     = randU64();
            ALL_HASHES[i + 8][n] = randU64();
        }
    }
}


void printBitmap(U64 bb) {
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
}
