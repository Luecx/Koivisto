
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

#ifndef KOIVISTO_PSQT_H
#define KOIVISTO_PSQT_H

#include "Bitboard.h"
using namespace bb;

#define M(mg, eg)    ((EvalScore)((unsigned int) (eg) << 16) + (mg))
#define MgScore(s)   ((Score)((uint16_t)((unsigned) ((s)))))
#define EgScore(s)   ((Score)((uint16_t)((unsigned) ((s) + 0x8000) >> 16)))
#define showScore(s) std::cout << "(" << MgScore(s) << ", " << EgScore(s) << ")" << std::endl;

#define pst_index_white_s(s)                                squareIndex(7 - rankIndex(s), fileIndex(s))
#define pst_index_black_s(s)                                s
#define pst_index_white(i, kside)                           squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))
#define pst_index_black(i, kside)                           squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))
#define pst_index_relative_white(pieceSquare, kingSquare)  (   (rankIndex(kingSquare)-rankIndex(pieceSquare)+7))*15+(fileIndex(kingSquare)-fileIndex(pieceSquare)+7)
#define pst_index_relative_black(pieceSquare, kingSquare)  (14-(rankIndex(kingSquare)-rankIndex(pieceSquare)+7))*15+(fileIndex(kingSquare)-fileIndex(pieceSquare)+7)

// indexed by piece
extern EvalScore piece_values[6];

// indexed by piece, relation to king
extern EvalScore piece_our_king_square_table[5][15 * 15];
extern EvalScore piece_opp_king_square_table[5][15 * 15];

// indexed by piece, sameSideCastle, square
extern EvalScore piece_square_table[6][2][64];

// indexed by wking, bking, piece, square
extern EvalScore piece_kk_square_tables[64][64][14][64];

void             psqt_init();

#endif    // KOIVISTO_PSQT_H
