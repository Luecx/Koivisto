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



Move genMove(const bb::Square from, const bb::Square to, const Type type, const bb::Piece movingPiece);

Move genMove(
              const bb::Square from,
              const bb::Square to,
              const Type type,
              const bb::Piece movingPiece,
              const bb::Piece capturedPiece);



void printMoveBits(Move move);


inline bb::Square getSquareFrom(const Move &move){
    return ((move >> SHIFT_FROM) & MASK_6);
}
inline bb::Square getSquareTo(const Move &move){
    return ((move >> SHIFT_TO) & MASK_6);
}
inline Type getType(const  Move &move){
    return ((move >> SHIFT_TYPE) & MASK_4);
}
inline bb::Piece getMovingPiece(const Move &move){
    return ((move >> SHIFT_MOVING_PIECE) & MASK_4);
}
inline bb::Piece getCapturedPiece(const Move &move){
    return ((move >> SHIFT_CAPTURED_PIECE) & MASK_4);
}



inline void setSquareFrom(Move &move, const bb::Square from){
    //move = (move & ~(MASK_6 << SHIFT_FROM));  //clearing
    move |= (from << SHIFT_FROM);
}
inline void setSquareTo(Move &move, const bb::Square to){
    //move = (move & ~(MASK_6 << SHIFT_TO));  //clearing
    move |= (to << SHIFT_TO);
}

inline void setType(Move &move, const Type type){
    //move = (move & ~(MASK_6 << SHIFT_TYPE));  //clearing
    move |= (type << SHIFT_TYPE);
}
inline void setMovingPiece(Move &move,const  bb::Piece movingPiece){
    //move = (move & ~(MASK_6 << SHIFT_MOVING_PIECE));  //clearing
    move |= (movingPiece << SHIFT_MOVING_PIECE);
}
inline void setCapturedPiece(Move &move, const bb::Piece capturedPiece){
    //move = (move & ~(MASK_6 << SHIFT_CAPTURED_PIECE));  //clearing
    move |= (capturedPiece << SHIFT_CAPTURED_PIECE);
}

// void setSquareFrom(Move &move,const bb::Square &from);
// void setSquareTo(Move &move,const bb::Square &to);
// void setType(Move &move, const Type &type);
// void setMovingPiece(Move &move,const bb::Piece &movingPiece);
// void setCapturedPiece(Move &move,const bb::Piece &capturedPiece);

bool isDoubledPawnPush(Move move);
bool isCapture(Move move);
bool isCastle(Move move);
bool isEnPassant(Move move);
bool isPromotion(Move move);
bb::Piece promotionPiece(Move move);



//inline Move &genMove(Move &m, const bb::Square &from, const bb::Square &to, const Type &type, const bb::Piece &movingPiece){
//    setSquareFrom(m, from);
//    setSquareTo(m, to);
//    setType(m, type);
//    setMovingPiece(m, movingPiece);
//    return m;
//}
//
//inline Move &genMove(Move &m,
//                     const bb::Square &from,
//                     const bb::Square &to,
//                     const Type &type,
//                     const bb::Piece &movingPiece,
//                     const bb::Piece &capturedPiece){
//
//
//    setSquareFrom(m, from);
//    setSquareTo(m, to);
//    setType(m, type);
//    setMovingPiece(m, movingPiece);
//    setCapturedPiece(m, capturedPiece);
//
//
//    return m;
//}


std::string toString(const Move move);




class MoveList{
    
    private:
        move::Move* moves;
        int size;
    
    public:
        MoveList();
        
        virtual ~MoveList();
        
        move::Move getMove(int index);
        
        void clear();
        
        void add(move::Move move);
        
        int getSize() const;
    
};



}





#endif //CHESSCOMPUTER_MOVE_H
