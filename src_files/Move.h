
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

#ifndef CHESSCOMPUTER_MOVE_H
#define CHESSCOMPUTER_MOVE_H


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
 * |        |    |    |                  moving piece
 * |        |    |    +------------------
 * |        |    |                       type information
 * |        |    +-----------------------
 * |        |                            captured piece
 * |        +----------------------------
 * |                                     score information (not used)
 * +-------------------------------------
 */

typedef uint32_t Move;
typedef uint8_t  MoveType;
typedef uint32_t MoveScore;

enum MoveTypes{
    QUIET                    = 0,
    DOUBLED_PAWN_PUSH        = 1,
    KING_CASTLE              = 2,
    QUEEN_CASTLE             = 3,
    CAPTURE                  = 4,
    EN_PASSANT               = 5,
    KNIGHT_PROMOTION         = 8,
    BISHOP_PROMOTION         = 9,
    ROOK_PROMOTION           = 10,
    QUEEN_PROMOTION          = 11,
    KNIGHT_PROMOTION_CAPTURE = 12,
    BISHOP_PROMOTION_CAPTURE = 13,
    ROOK_PROMOTION_CAPTURE   = 14,
    QUEEN_PROMOTION_CAPTURE  = 15,
};

enum MoveTypeMasks {
    PROMOTION_MASK = 0x8,
    CAPTURE_MASK   = 0x4,
    SPECIAL_MASK   = 0x3,
};

enum MoveShifts{
    SHIFT_FROM           = 0,
    SHIFT_TO             = 6,
    SHIFT_TYPE           = 16,
    SHIFT_MOVING_PIECE   = 12,
    SHIFT_CAPTURED_PIECE = 20,
    SHIFT_SCORE_INFO     = 24,
};

template<uint8_t N>
constexpr uint32_t MASK = (1 << N) - 1;

inline bool sameMove(const Move& m1, const Move& m2) {
    // toggle all bits in m1 by m2 and check if no bits are toggled in the least significant 24 bits
    return ((m1 ^ m2) & MASK<24>) == 0;
}
inline void setScore(      Move& move, const int moveScore) {
    move = (move & ~(MASK<8> << SHIFT_SCORE_INFO));    // clearing
    move |= (moveScore << SHIFT_SCORE_INFO);
}
inline int  getScore(const Move& move) { return (move >> SHIFT_SCORE_INFO); }

inline int getPieceSqToCombination(const Move& move) {return (move >> SHIFT_TO) & MASK<10>;}
inline int getPieceTypeSqToCombination(const Move& move) {return (move >> SHIFT_TO) & MASK<9>;}
inline int getSqToSqFromCombination(const Move& move) {return move & MASK<12>;}

inline bb::Square    getSquareFrom          (const Move& move) { return ((move >> SHIFT_FROM) & MASK<6>); }
inline bb::Square    getSquareTo            (const Move& move) { return ((move >> SHIFT_TO) & MASK<6>); }
inline MoveType      getType                (const Move& move) { return ((move >> SHIFT_TYPE) & MASK<4>); }
inline bb::Piece     getMovingPiece         (const Move& move) { return ((move >> SHIFT_MOVING_PIECE) & MASK<4>); }
inline bb::PieceType getMovingPieceType     (const Move& move) { return ((move >> SHIFT_MOVING_PIECE) & MASK<3>);}
inline bb::Piece     getCapturedPiece       (const Move& move) { return ((move >> SHIFT_CAPTURED_PIECE) & MASK<4>); }
inline bb::PieceType getCapturedPieceType   (const Move& move) { return ((move >> SHIFT_CAPTURED_PIECE) & MASK<3>); }
inline bb::Color     getMovingPieceColor    (const Move& move) { return ((move >> SHIFT_MOVING_PIECE) & 0x8);}
inline bb::Piece     getPromotionPiece      (const Move& move){ return ((move & 0x30000) >> SHIFT_TYPE) + getMovingPiece(move) + 1; }
inline bb::Piece     getPromotionPieceType  (const Move& move){ return ((move & 0x30000) >> SHIFT_TYPE) + 1; }
inline void setSquareFrom   (Move& move, const bb::Square from) {
    // move = (move & ~(MASK_6 << SHIFT_FROM));  //clearing
    move |= (from << SHIFT_FROM);
}
inline void setSquareTo     (Move& move, const bb::Square to) {
    // move = (move & ~(MASK_6 << SHIFT_TO));  //clearing
    move |= (to << SHIFT_TO);
}
inline void setType         (Move& move, const MoveType type) {
    // move = (move & ~(MASK_6 << SHIFT_TYPE));  //clearing
    move |= (type << SHIFT_TYPE);
}
inline void setMovingPiece  (Move& move, const bb::Piece movingPiece) {
    // move = (move & ~(MASK_6 << SHIFT_MOVING_PIECE));  //clearing
    move |= (movingPiece << SHIFT_MOVING_PIECE);
}
inline void setCapturedPiece(Move& move, const bb::Piece capturedPiece) {
    // move = (move & ~(MASK_6 << SHIFT_CAPTURED_PIECE));  //clearing
    move |= (capturedPiece << SHIFT_CAPTURED_PIECE);
}

inline Move genMove(const bb::Square &from, const bb::Square &to, const MoveType&type, const bb::Piece &movingPiece){
    Move m {0};
    setSquareFrom(m, from);
    setSquareTo(m, to);
    setType(m, type);
    setMovingPiece(m, movingPiece);
    return m;
}
inline Move genMove(const bb::Square &from, const bb::Square &to, const MoveType&type, const bb::Piece &movingPiece,
                    const bb::Piece &capturedPiece){
    Move m {0};
    
    setSquareFrom(m, from);
    setSquareTo(m, to);
    setType(m, type);
    setMovingPiece(m, movingPiece);
    setCapturedPiece(m, capturedPiece);
    
    return m;
}

inline bool         isDoubledPawnPush   (Move move){
    
    return getType(move) == DOUBLED_PAWN_PUSH;
}
inline bool         isCapture           (Move move){
    return move & 0x40000;
}
inline bool         isCastle            (Move move){
    MoveType t = getType(move);
    return t == KING_CASTLE || t == QUEEN_CASTLE;
}
inline bool         isEnPassant         (Move move){
    return getType(move) == EN_PASSANT;
}
inline bool         isPromotion         (Move move){
    return move & 0x80000;
}

std::string toString(const Move& move);
void        printMoveBits(Move move, bool bitInfo = true);

class MoveList {
    
    private:
    move::Move      moves [256];
    move::MoveScore scores[256];
    int             size;
    
    public:
    void       swap(int i1, int i2);
    void       sort(int lo =0, int hi =-1);
    move::Move getMove(int index);
    void       clear();
    void       add(move::Move move);
    void       scoreMove(int index, MoveScore score);
    MoveScore  getScore(int index);
    void       printMoveBits();
    int        getSize() const;
};

}    // namespace move

#endif    // CHESSCOMPUTER_MOVE_H
