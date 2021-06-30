
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
#ifndef KOIVISTO_EVAL_H
#define KOIVISTO_EVAL_H

#include "Bitboard.h"

#include <cstdint>
#include <cstring>
#include <immintrin.h>


#define INPUT_SIZE     (bb::N_PIECE_TYPES * bb::N_SQUARES * 2)
#define HIDDEN_SIZE    (512)
#define OUTPUT_SIZE    (1)

#define BIT_ALIGNMENT  (256)
#define BYTE_ALIGNMENT (BIT_ALIGNMENT / 8)
#define ALIGNMENT      (BYTE_ALIGNMENT)

#define STRIDE_16_BIT  (256 / 16)


class Board;

namespace nn {

extern int16_t inputWeights [INPUT_SIZE][HIDDEN_SIZE];
extern int16_t hiddenWeights[OUTPUT_SIZE][HIDDEN_SIZE];
extern int16_t inputBias    [HIDDEN_SIZE];
extern int32_t hiddenBias   [OUTPUT_SIZE];

void init();

struct Evaluator {
    
    // inputs and outputs
    bool inputMap[INPUT_SIZE] {};
    
    // summations
    alignas(ALIGNMENT) int16_t summation [HIDDEN_SIZE]{};
    
    alignas(ALIGNMENT) int16_t activation[HIDDEN_SIZE] {};
    alignas(ALIGNMENT) int32_t output    [OUTPUT_SIZE] {};
    
    
    int index(bb::PieceType pieceType, bb::Color pieceColor, bb::Square square);

    template<bool value>
    void setPieceOnSquare(bb::PieceType pieceType, bb::Color pieceColor, bb::Square square);

    void reset(Board* board);
    
    int evaluate(Board* board = nullptr);
};
}    // namespace nn

#endif    // KOIVISTO_EVAL_H
