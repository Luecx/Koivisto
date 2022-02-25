
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
#if defined(__ARM_NEON)
#include <arm_neon.h>
#else
#include <immintrin.h>
#endif
#include <vector>


#define INPUT_SIZE     (bb::N_PIECE_TYPES * bb::N_SQUARES * 2 * 16)
#define HIDDEN_SIZE    (512)
#define HIDDEN_DSIZE   (HIDDEN_SIZE * 2)
#define OUTPUT_SIZE    (1)
 
#if defined(__AVX512F__)
#define BIT_ALIGNMENT  (512)
#elif defined(__AVX2__) || defined(__AVX__)
#define BIT_ALIGNMENT  (256)
#elif defined(__SSE2__) || defined(__ARM_NEON)
#define BIT_ALIGNMENT  (128)
#endif
#define STRIDE_16_BIT  (BIT_ALIGNMENT / 16)
#define BYTE_ALIGNMENT (BIT_ALIGNMENT / 8)
#define ALIGNMENT      (BYTE_ALIGNMENT)

class Board;

namespace nn {

extern int16_t inputWeights [INPUT_SIZE][HIDDEN_SIZE];
extern int16_t hiddenWeights[OUTPUT_SIZE][HIDDEN_DSIZE];
extern int16_t inputBias    [HIDDEN_SIZE];
extern int32_t hiddenBias   [OUTPUT_SIZE];

void init();

struct Accumulator{
    alignas(ALIGNMENT) int16_t summation [bb::N_COLORS][HIDDEN_SIZE]{};
};

struct Evaluator {
    
    // summations
    std::vector<Accumulator> history{};
    
    alignas(ALIGNMENT) int16_t activation[HIDDEN_DSIZE] {};
    alignas(ALIGNMENT) int32_t output    [OUTPUT_SIZE ] {};

    Evaluator();
    
    void addNewAccumulation();
    
    void popAccumulation();
    
    void clearHistory();
    
    int kingSquareIndex( bb::Square kingSquare, bb::Color kingColor);
    
    int index( bb::PieceType pieceType,
               bb::Color pieceColor,
               bb::Square square,
               bb::Color view,
               bb::Square kingSquare);
    
    template<bool value>
    void setPieceOnSquare(bb::PieceType pieceType,
                          bb::Color pieceColor,
                          bb::Square square,
                          bb::Square wKingSquare,
                          bb::Square bKingSquare);
    
    template<bool value>
    void setPieceOnSquareAccumulator(bb::Color side,
                                     bb::PieceType pieceType,
                                     bb::Color pieceColor,
                                     bb::Square square,
                                     bb::Square kingSquare);

    void reset(Board* board);
    
    void resetAccumulator(Board* board, bb::Color color);
    
    int evaluate(bb::Color activePlayer, Board* board = nullptr);
} __attribute__((aligned(128)));
}    // namespace nn

#endif    // KOIVISTO_EVAL_H
