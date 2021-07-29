
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


#define INPUT_SIZE      (bb::N_PIECE_TYPES * bb::N_SQUARES * 2)
#define HIDDEN1_SIZE    (256)
#define HIDDEN2_SIZE    (16)
#define OUTPUT_SIZE     (1)

#if defined(__AVX512F__)
#define BIT_ALIGNMENT  (512)
#elif defined(__AVX2__) || defined(__AVX__)
#define BIT_ALIGNMENT  (256)
#elif defined(__SSE2__)
#define BIT_ALIGNMENT  (128)
#endif
#define STRIDE_16_BIT  (BIT_ALIGNMENT / 16)
#define BYTE_ALIGNMENT (BIT_ALIGNMENT / 8)
#define ALIGNMENT      (BYTE_ALIGNMENT)

class Board;

namespace nn {

extern int16_t hidden1Weights[INPUT_SIZE][HIDDEN1_SIZE];
extern int16_t hidden2Weights[HIDDEN2_SIZE][HIDDEN1_SIZE];
extern int16_t outputWeights [OUTPUT_SIZE ][HIDDEN2_SIZE];

extern int16_t hidden1Bias    [HIDDEN1_SIZE];
extern int32_t hidden2Bias    [HIDDEN2_SIZE];
extern int32_t outputBias     [OUTPUT_SIZE];

void init();

struct Evaluator {
    
    // inputs and outputs
    bool inputMap[INPUT_SIZE] {};
    
    // summations
    alignas(ALIGNMENT) int16_t summation [bb::N_COLORS][HIDDEN1_SIZE]{};
    
    alignas(ALIGNMENT) int16_t hidden1Activation[HIDDEN1_SIZE] {};
    alignas(ALIGNMENT) int16_t hidden2Activation[HIDDEN2_SIZE] {};
    alignas(ALIGNMENT) int16_t output           [OUTPUT_SIZE ] {};
    
    int index(bb::PieceType pieceType, bb::Color pieceColor, bb::Square square, bb::Color activePlayer);

    template<bool value>
    void setPieceOnSquare(bb::PieceType pieceType, bb::Color pieceColor, bb::Square square);

    void reset(Board* board);
    
    int evaluate(bb::Color activePlayer, Board* board = nullptr);
};
}    // namespace nn

#endif    // KOIVISTO_EVAL_H
