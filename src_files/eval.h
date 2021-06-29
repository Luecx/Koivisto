
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
#include "Board.h"

#include <cstdint>
#include <cstring>
#include <immintrin.h>

namespace nn {

#define INPUT_SIZE     (bb::N_PIECE_TYPES * bb::N_SQUARES * 2)
#define HIDDEN_SIZE    (512)
#define OUTPUT_SIZE    (1)

#define BIT_ALIGNMENT  (256)
#define BYTE_ALIGNMENT (BIT_ALIGNMENT / 8)
#define ALIGNMENT      (BYTE_ALIGNMENT)

#define STRIDE_16_BIT  (256 / 16)

extern int16_t inputWeights [INPUT_SIZE][HIDDEN_SIZE];
extern int16_t hiddenWeights[OUTPUT_SIZE][HIDDEN_SIZE];
extern int16_t inputBias    [HIDDEN_SIZE];
extern int16_t hiddenBias   [OUTPUT_SIZE];

void init();

inline void print_256i_epi16(const __m256i &h){
    printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d \n",
           _mm256_extract_epi16(h,0),
           _mm256_extract_epi16(h,1),
           _mm256_extract_epi16(h,2),
           _mm256_extract_epi16(h,3),
           _mm256_extract_epi16(h,4),
           _mm256_extract_epi16(h,5),
           _mm256_extract_epi16(h,6),
           _mm256_extract_epi16(h,7),
           _mm256_extract_epi16(h,8),
           _mm256_extract_epi16(h,9),
           _mm256_extract_epi16(h,10),
           _mm256_extract_epi16(h,11),
           _mm256_extract_epi16(h,12),
           _mm256_extract_epi16(h,13),
           _mm256_extract_epi16(h,14),
           _mm256_extract_epi16(h,15));
}


struct Evaluator {

    Evaluator() {
        std::memcpy(input_weights , inputWeights , sizeof(int16_t) * INPUT_SIZE  * HIDDEN_SIZE);
        std::memcpy(input_bias    , inputBias    , sizeof(int16_t) * HIDDEN_SIZE);
        std::memcpy(hidden_weights, hiddenWeights, sizeof(int16_t) * HIDDEN_SIZE * OUTPUT_SIZE);
        std::memcpy(hidden_bias   , hiddenBias   , sizeof(int16_t) * OUTPUT_SIZE);
    }

    // inputs and outputs
    bool inputMap[INPUT_SIZE] {};
    alignas(ALIGNMENT) int16_t summation [HIDDEN_SIZE] {};
    alignas(ALIGNMENT) int16_t activation[HIDDEN_SIZE] {};
    alignas(ALIGNMENT) int32_t output    [OUTPUT_SIZE] {};
    
    // bias
    alignas(ALIGNMENT) int16_t input_bias [HIDDEN_SIZE] {};
    alignas(ALIGNMENT) int16_t hidden_bias[OUTPUT_SIZE] {};

    // weights
    alignas(ALIGNMENT) int16_t input_weights [INPUT_SIZE ][HIDDEN_SIZE] {};
    alignas(ALIGNMENT) int16_t hidden_weights[OUTPUT_SIZE][HIDDEN_SIZE] {};
    
    int index(bb::PieceType pieceType, bb::Color pieceColor, bb::Square square) {
        constexpr int pieceTypeFactor  = 64;
        constexpr int pieceColorFactor = 64 * 6;

        return square
               + pieceType  * pieceTypeFactor
               + pieceColor * pieceColorFactor;
    }

    template<bool value>
    void setPieceOnSquare(bb::PieceType pieceType, bb::Color pieceColor, bb::Square square) {
        int idx = index(pieceType, pieceColor, square);

        if (inputMap[idx] == value)
            return;

        auto wgt = (__m256i*) (input_weights[idx]);
        auto sum = (__m256i*) (summation);

        if constexpr (value) {
            for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
                sum[i] = _mm256_add_epi16(sum[i], wgt[i]);
            }
        } else {
            for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
                sum[i] = _mm256_sub_epi16(sum[i], wgt[i]);
            }
        }
    }

    void reset(Board* board){
        std::memset(inputMap , 0, sizeof(bool) * INPUT_SIZE);
        std::memcpy(summation, input_bias, sizeof(int16_t) * HIDDEN_SIZE);
        
        for(Color c:{WHITE,BLACK}){
            for(PieceType pt:{PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}){
                U64 bb = board->getPieceBB(c, pt);
                while(bb){
                    Square s = bitscanForward(bb);
                    
                    setPieceOnSquare<true>(pt, c, s);
                    
                    bb = lsbReset(bb);
                }
            }
        }
    }
    
    int evaluate(Board* board = nullptr) {
        if(board != nullptr){
            reset(board);
        }
        
        constexpr __m256i reluBias {};
    
        __m256i*          sum = (__m256i*) (summation);
        __m256i*          act = (__m256i*) (activation);

        // apply relu to the summation first
        for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
            act[i] = _mm256_srli_epi16(_mm256_max_epi16(sum[i], reluBias), 0);

//            print_256i_epi16(act[i]);
//            for(int h = 0; h < 16; h++){
//                int t =  _mm256_extract_epi16(act[i], h);
//                std::cout << t << std::endl;
////                std::cout << (int16_t) _mm256_extract_epi16(act[i], h) << std::endl;
//            }
            
        }
        
        // do the sum for the output neurons
        for (int o = 0; o < OUTPUT_SIZE; o++) {

            auto wgt = (__m256i*) (hidden_weights[o]);

            __m256i  res {};
            for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
                res = _mm256_add_epi32(res, _mm256_madd_epi16(act[i], wgt[i]));
            }
            
            // credit to Connor
            const __m128i reduced_4 = _mm_add_epi32(_mm256_castsi256_si128(res), _mm256_extractf128_si256 (res, 1));
            __m128i vsum = _mm_add_epi32(reduced_4, _mm_srli_si128(reduced_4, 8));
            vsum = _mm_add_epi32(vsum, _mm_srli_si128(vsum, 4));
            int32_t sum = _mm_cvtsi128_si32(vsum);
            
            output[o] = sum + hidden_bias[o];
        }
        
        
        
        return output[0] * 100 / 128 / 1024;
    }
};
}    // namespace nn

#endif    // KOIVISTO_EVAL_H
