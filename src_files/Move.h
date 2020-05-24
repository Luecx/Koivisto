//
// Created by finne on 5/15/2020.
//

#ifndef CHESSCOMPUTER_MOVE_H
#define CHESSCOMPUTER_MOVE_H


#include <cstdint>

#include "Bitboard.h"

namespace move{



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
 * |                                     score information
 * +-------------------------------------
 */

typedef uint32_t            Move;
typedef uint8_t             Type;

constexpr Move MASK_4                         = 15;
constexpr Move MASK_6                         = 63;
constexpr Move MASK_8                         = 255;

constexpr int SHIFT_FROM                      = 0;
constexpr int SHIFT_TO                        = 6;
constexpr int SHIFT_TYPE                      = 12;
constexpr int SHIFT_MOVING_PIECE              = 16;
constexpr int SHIFT_CAPTURED_PIECE            = 20;
constexpr int SHIFT_SCORE_INFO                = 24;

//https://www.chessprogramming.org/Encoding_Moves
constexpr Type QUIET                          = 0;
constexpr Type DOUBLED_PAWN_PUSH              = 1;
constexpr Type KING_CASTLE                    = 2;
constexpr Type QUEEN_CASTLE                   = 3;
constexpr Type CAPTURE                        = 4;
constexpr Type EN_PASSANT                     = 5;
constexpr Type KNIGHT_PROMOTION               = 8;
constexpr Type BISHOP_PROMOTION               = 9;
constexpr Type ROOK_PROMOTION                 = 10;
constexpr Type QUEEN_PROMOTION                = 11;
constexpr Type KNIGHT_PROMOTION_CAPTURE       = 12;
constexpr Type BISHOP_PROMOTION_CAPTURE       = 13;
constexpr Type ROOK_PROMOTION_CAPTURE         = 14;
constexpr Type QUEEN_PROMOTION_CAPTURE        = 15;


Move genMove(
        const bb::Square &from,
        const bb::Square &to,
        const Type &type,
        const bb::Piece &movingPiece,
        const bb::Piece &capturedPiece);
Move genMove(const bb::Square &from,
             const bb::Square &to,
             const Type &type,
             const bb::Piece &movingPiece);

void printMoveBits(Move &move);

bb::Square getSquareFrom(const Move &move);
bb::Square getSquareTo(const Move &move);

Type getType(const Move &move);
bb::Piece getMovingPiece(const Move &move);
bb::Piece getCapturedPiece(const Move &move);


void setSquareFrom(Move &move,const bb::Square &from);
void setSquareTo(Move &move,const bb::Square &to);

void setType(Move &move, const Type &type);
void setMovingPiece(Move &move,const bb::Piece &movingPiece);
void setCapturedPiece(Move &move,const bb::Piece &capturedPiece);

bool isDoubledPawnPush(const Move &move);
bool isCapture(const Move &move);
bool isCastle(const Move &move);
bool isEnPassant(const Move &move);
bool isPromotion(const Move &move);
bb::Piece promotionPiece(const Move &move);

std::string toString(const Move &move);
}



#endif //CHESSCOMPUTER_MOVE_H
