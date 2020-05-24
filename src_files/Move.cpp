//
// Created by finne on 5/15/2020.
//

#include "Move.h"

using namespace move;

Move move::genMove(const bb::Square &from, const bb::Square &to, const Type &type, const bb::Piece &movingPiece){
    Move m{0};
    setSquareFrom(m, from);
    setSquareTo(m, to);
    setType(m, type);
    setMovingPiece(m, movingPiece);
    return m;
}

Move move::genMove(const bb::Square &from,
                   const bb::Square &to,
                   const Type &type,
                   const bb::Piece &movingPiece,
                   const bb::Piece &capturedPiece){
    
    
    
    Move m{0};
    setSquareFrom(m, from);
    setSquareTo(m, to);
    setType(m, type);
    setMovingPiece(m, movingPiece);
    setCapturedPiece(m, capturedPiece);
    
    
    return m;
}


void move::printMoveBits( Move &move){
    std::cout << " ";
    for(int i = 0; i < 8; i++){
        std::cout << ((move >> (31-i)) & 1UL) ? "1":"0";
    }
    std::cout << " ";
    for(int i = 0; i < 4; i++){
        std::cout << ((move >> (23-i)) & 1UL) ? "1":"0";
    }
    std::cout << " ";
    for(int i = 0; i < 4; i++){
        std::cout << ((move >> (19-i)) & 1UL) ? "1":"0";
    }
    std::cout << " ";
    for(int i = 0; i < 4; i++){
        std::cout << ((move >> (15-i)) & 1UL) ? "1":"0";
    }
    std::cout << " ";
    for(int i = 0; i < 6; i++){
        std::cout << ((move >> (11-i)) & 1UL) ? "1":"0";
    }
    std::cout << " ";
    for(int i = 0; i < 6; i++){
        std::cout << ((move >> (5-i)) & 1UL) ? "1":"0";
    }
    std::cout << "\n";
    std::cout << "|        |    |    |    |      |      squareFrom\n"
                 "|        |    |    |    |      +--------------------------\n"
                 "|        |    |    |    |             squareTo\n"
                 "|        |    |    |    +---------------------------------\n"
                 "|        |    |    |                  type information\n"
                 "|        |    |    +--------------------------------------\n"
                 "|        |    |                       moving piece\n"
                 "|        |    +-------------------------------------------\n"
                 "|        |                            captured piece\n"
                 "|        +------------------------------------------------\n"
                 "|                                     score information\n"
                 "+---------------------------------------------------------" << std::endl;
    
}

bb::Square move::getSquareFrom(const Move &move){
    return ((move >> SHIFT_FROM) & MASK_6);
}
bb::Square move::getSquareTo(const Move &move){
    return ((move >> SHIFT_TO) & MASK_6);
}
Type move::getType(const  Move &move){
    return ((move >> SHIFT_TYPE) & MASK_4);
}
bb::Piece move::getMovingPiece(const Move &move){
    return ((move >> SHIFT_MOVING_PIECE) & MASK_4);
}
bb::Piece move::getCapturedPiece(const  Move &move){
    return ((move >> SHIFT_CAPTURED_PIECE) & MASK_4);
}



void move::setSquareFrom(Move &move, const bb::Square &from){
    //move = (move & ~(MASK_6 << SHIFT_FROM));  //clearing
    move |= (from << SHIFT_FROM);
}
void move::setSquareTo(Move &move, const bb::Square &to){
    //move = (move & ~(MASK_6 << SHIFT_TO));  //clearing
    move |= (to << SHIFT_TO);
}

void move::setType(Move &move, const Type &type){
    //move = (move & ~(MASK_6 << SHIFT_TYPE));  //clearing
    move |= (type << SHIFT_TYPE);
}
void move::setMovingPiece(Move &move,const  bb::Piece &movingPiece){
    //move = (move & ~(MASK_6 << SHIFT_MOVING_PIECE));  //clearing
    move |= (movingPiece << SHIFT_MOVING_PIECE);
}
void move::setCapturedPiece(Move &move, const bb::Piece &capturedPiece){
    //move = (move & ~(MASK_6 << SHIFT_CAPTURED_PIECE));  //clearing
    move |= (capturedPiece << SHIFT_CAPTURED_PIECE);
}

bool move::isDoubledPawnPush(const Move &move) {
    return getType(move) == DOUBLED_PAWN_PUSH;
}
bool move::isCapture(const  Move &move){
    return move & 0x4000;
}
bool move::isCastle(const  Move &move){
    Type t = getType(move);
    return t == KING_CASTLE || t == QUEEN_CASTLE;
}
bool move::isEnPassant(const  Move &move){
 return getType(move) == EN_PASSANT;
}
bool move::isPromotion(const  Move &move){
    return move & 0x8000;
}
bb::Piece move::promotionPiece(const Move &move){
    return ((move & 0x3000) >> SHIFT_TYPE)+getMovingPiece(move)+1;
}

std::string move::toString(const Move &move) {
    
    std::string res{};
    res.append(bb::SQUARE_IDENTIFIER[getSquareFrom(move)]);
    res.append(bb::SQUARE_IDENTIFIER[getSquareTo(move)]);
    if(isPromotion(move)){
        char c = tolower(bb::PIECE_IDENTIFER[promotionPiece(move)]);
        res.push_back(c);
    }
    
    return res;
}
