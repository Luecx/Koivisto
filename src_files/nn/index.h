//
// Created by Luecx on 28.01.2023.
//

#ifndef KOIVISTO_INDEX_H
#define KOIVISTO_INDEX_H

#include "defs.h"

namespace nn{

constexpr int kingSquareIndices[bb::N_SQUARES] {
    0,  1,  2,  3,  3,  2,  1,  0,
    4,  5,  6,  7,  7,  6,  5,  4,
    8,  9,  10, 11, 11, 10, 9,  8,
    8,  9,  10, 11, 11, 10, 9,  8,
    12, 12, 13, 13, 13, 13, 12, 12,
    12, 12, 13, 13, 13, 13, 12, 12,
    14, 14, 15, 15, 15, 15, 14, 14,
    14, 14, 15, 15, 15, 15, 14, 14,
};

// the index is based on a king bucketing system. the relevant king bucket can be retrieved using
// the function below
[[nodiscard]] inline int kingSquareIndex(bb::Square kingSquare, bb::Color kingColor){
    UCI_ASSERT(kingSquare >= bb::A1);
    UCI_ASSERT(kingSquare <= bb::H8);
    kingSquare = (56 * kingColor) ^ kingSquare;
    return kingSquareIndices[kingSquare];
    
}

// computes the index for a piece (piece type) and its color on the specified square
// also takes the view from with we view at the board as well as the king square of the view side
[[nodiscard]] inline int index(bb::PieceType pieceType, bb::Color pieceColor, bb::Square square,
                               bb::Color view, bb::Square kingSquare){
    const int ksIndex = kingSquareIndex(kingSquare, view);
    square ^= 56 * view;
    square ^= 7 * !!(kingSquare & 0x4);

    // clang-format off
    return square
           + pieceType * 64
           + !(pieceColor ^ view) * 64 * 6
           + ksIndex * 64 * 6 * 2;
    // clang-format on
}


}

#endif    // KOIVISTO_INDEX_H
