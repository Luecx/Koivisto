//
// Created by finne on 5/15/2020.
//

#ifndef CHESSCOMPUTER_MOVE_H
#define CHESSCOMPUTER_MOVE_H

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
 
#include "Bitboard.h"

#include <cstdint>

namespace move {

/**
 *
 *
 *
 *  00000000 0000 0000 0000 000000 000000
 * |        |    |    |    |      |      squareFrom
 * |        |    |    |    |      +------
 * |        |    |    |    |             squareTo
 * |        |    |    |    +-------------
 * |        |    |    |                  type information
 * |        |    |    +------------------
 * |        |    |                       moving piece
 * |        |    +-----------------------
 * |        |                            captured piece
 * |        +----------------------------
 * |                                     score information (not used)
 * +-------------------------------------
 */

typedef uint32_t Move;
typedef uint8_t  Type;
typedef uint32_t MoveScore;

constexpr Move MASK_4  = 15;
constexpr Move MASK_6  = 63;
constexpr Move MASK_8  = 255;
constexpr Move MASK_24 = (1 << 24) - 1;

constexpr int SHIFT_FROM           = 0;
constexpr int SHIFT_TO             = 6;
constexpr int SHIFT_TYPE           = 12;
constexpr int SHIFT_MOVING_PIECE   = 16;
constexpr int SHIFT_CAPTURED_PIECE = 20;
constexpr int SHIFT_SCORE_INFO     = 24;

// https://www.chessprogramming.org/Encoding_Moves
constexpr Type QUIET                    = 0;
constexpr Type DOUBLED_PAWN_PUSH        = 1;
constexpr Type KING_CASTLE              = 2;
constexpr Type QUEEN_CASTLE             = 3;
constexpr Type CAPTURE                  = 4;
constexpr Type EN_PASSANT               = 5;
constexpr Type KNIGHT_PROMOTION         = 8;
constexpr Type BISHOP_PROMOTION         = 9;
constexpr Type ROOK_PROMOTION           = 10;
constexpr Type QUEEN_PROMOTION          = 11;
constexpr Type KNIGHT_PROMOTION_CAPTURE = 12;
constexpr Type BISHOP_PROMOTION_CAPTURE = 13;
constexpr Type ROOK_PROMOTION_CAPTURE   = 14;
constexpr Type QUEEN_PROMOTION_CAPTURE  = 15;

// Move genMove(const std::string str);

Move genMove(const bb::Square from, const bb::Square to, const Type type, const bb::Piece movingPiece);

Move genMove(const bb::Square from, const bb::Square to, const Type type, const bb::Piece movingPiece,
             const bb::Piece capturedPiece);

inline bool sameMove(const Move& m1, const Move& m2) {
    // toggle all bits in m1 by m2 and check if no bits are toggled in the least significant 24 bits
    return ((m1 ^ m2) & MASK_24) == 0;
}

inline void setScore(Move& move, const int moveScore) {
    move = (move & ~(MASK_8 << SHIFT_SCORE_INFO));    // clearing
    move |= (moveScore << SHIFT_SCORE_INFO);
}

inline int getScore(const Move& move) { return (move >> SHIFT_SCORE_INFO); }

inline bb::Square getSquareFrom(const Move& move) { return ((move >> SHIFT_FROM) & MASK_6); }

inline bb::Square getSquareTo(const Move& move) { return ((move >> SHIFT_TO) & MASK_6); }

inline Type getType(const Move& move) { return ((move >> SHIFT_TYPE) & MASK_4); }

inline bb::Piece getMovingPiece(const Move& move) { return ((move >> SHIFT_MOVING_PIECE) & MASK_4); }

inline bb::Piece getCapturedPiece(const Move& move) { return ((move >> SHIFT_CAPTURED_PIECE) & MASK_4); }

inline void setSquareFrom(Move& move, const bb::Square from) {
    // move = (move & ~(MASK_6 << SHIFT_FROM));  //clearing
    move |= (from << SHIFT_FROM);
}

inline void setSquareTo(Move& move, const bb::Square to) {
    // move = (move & ~(MASK_6 << SHIFT_TO));  //clearing
    move |= (to << SHIFT_TO);
}

inline void setType(Move& move, const Type type) {
    // move = (move & ~(MASK_6 << SHIFT_TYPE));  //clearing
    move |= (type << SHIFT_TYPE);
}

inline void setMovingPiece(Move& move, const bb::Piece movingPiece) {
    // move = (move & ~(MASK_6 << SHIFT_MOVING_PIECE));  //clearing
    move |= (movingPiece << SHIFT_MOVING_PIECE);
}

inline void setCapturedPiece(Move& move, const bb::Piece capturedPiece) {
    // move = (move & ~(MASK_6 << SHIFT_CAPTURED_PIECE));  //clearing
    move |= (capturedPiece << SHIFT_CAPTURED_PIECE);
}

bool isDoubledPawnPush(Move move);

bool isCapture(Move move);

bool isCastle(Move move);

bool isEnPassant(Move move);

bool isPromotion(Move move);

bb::Piece promotionPiece(Move move);

std::string toString(const Move move);

void printMoveBits(Move move, bool bitInfo = true);

class MoveList {

    private:
    move::Move      moves[256];
    move::MoveScore scores[256];
    int             size;

    public:
    MoveList();

    virtual ~MoveList();

    void swap(int i1, int i2);

    move::Move getMove(int index);

    void clear();

    void add(move::Move move);

    void scoreMove(int index, MoveScore score);

    MoveScore getScore(int index);

    void printMoveBits();

    int getSize() const;
};

}    // namespace move

#endif    // CHESSCOMPUTER_MOVE_H
