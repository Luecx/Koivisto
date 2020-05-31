//
// Created by finne on 5/31/2020.
//

#include "eval.h"


/**
 * evaluates the board.
 * @param b
 * @return
 */
bb::Score evaluate(Board *b) {
    
    Score res = 0;
    
    res += (bitCount(b->getPieces()[WHITE_BISHOP]) - bitCount(b->getPieces()[BLACK_BISHOP])) * 325;
    res += (bitCount(b->getPieces()[WHITE_KNIGHT]) - bitCount(b->getPieces()[BLACK_KNIGHT])) * 310;
    res += (bitCount(b->getPieces()[WHITE_ROOK])   - bitCount(b->getPieces()[BLACK_ROOK]))   * 500;
    res += (bitCount(b->getPieces()[WHITE_QUEEN])  - bitCount(b->getPieces()[BLACK_QUEEN]))  * 920;
    res += (bitCount(b->getPieces()[WHITE_PAWN])   - bitCount(b->getPieces()[BLACK_PAWN]))   * 100;
    
    
    
    
    return res;
    

}
