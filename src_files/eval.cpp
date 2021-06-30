
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

#include "eval.h"
#include "Board.h"

#include "UCIAssert.h"

alignas(32) int16_t nn::inputWeights [INPUT_SIZE ][HIDDEN_SIZE];
alignas(32) int16_t nn::hiddenWeights[OUTPUT_SIZE][HIDDEN_SIZE];
alignas(32) int16_t nn::inputBias    [HIDDEN_SIZE];
alignas(32) int32_t nn::hiddenBias   [OUTPUT_SIZE];

void    nn::init() {
    FILE *f = fopen("debug.net", "rb");
    
    // figure out how many entries we will store
    uint64_t count =
        + INPUT_SIZE * HIDDEN_SIZE
        + HIDDEN_SIZE
        + HIDDEN_SIZE * OUTPUT_SIZE
        + OUTPUT_SIZE;
   
    
    uint64_t fileCount = 0;
    fread(&fileCount, sizeof(uint64_t), 1, f);
    UCI_ASSERT((count ) == fileCount);
    
    float tempInputWeights [INPUT_SIZE ][HIDDEN_SIZE];
    float tempHiddenWeights[OUTPUT_SIZE][HIDDEN_SIZE];
    float tempInputBias    [HIDDEN_SIZE];
    float tempHiddenBias   [OUTPUT_SIZE];
    
    fread(tempInputWeights  , sizeof(float), INPUT_SIZE * HIDDEN_SIZE, f);
    fread(tempInputBias     , sizeof(float), HIDDEN_SIZE, f);
    fread(tempHiddenWeights , sizeof(float), HIDDEN_SIZE * OUTPUT_SIZE, f);
    fread(tempHiddenBias    , sizeof(float), OUTPUT_SIZE, f);
    
    for(int o = 0; o < HIDDEN_SIZE; o++){
        for(int i = 0; i < INPUT_SIZE; i++){
            inputWeights[i][o] = round(tempInputWeights[i][o] * 16);
        }
        inputBias[o] = round(tempInputBias[o] * 16);
    }
    
    for(int o = 0; o < OUTPUT_SIZE; o++){
        for(int i = 0; i < HIDDEN_SIZE; i++){
            hiddenWeights[o][i] = round(tempHiddenWeights[o][i] * 1024);
        }
        hiddenBias[o] = round(tempHiddenBias[o] * 1024 * 16);
    }

    fclose(f);
}
int nn::Evaluator::index(bb::PieceType pieceType, bb::Color pieceColor, bb::Square square) {
    constexpr int pieceTypeFactor  = 64;
    constexpr int pieceColorFactor = 64 * 6;
    
    return square
           + pieceType  * pieceTypeFactor
           + pieceColor * pieceColorFactor;
}
template<bool value>
void nn::Evaluator::setPieceOnSquare(bb::PieceType pieceType, bb::Color pieceColor,
                                     bb::Square square) {
    int idx = index(pieceType, pieceColor, square);
    
    if (inputMap[idx] == value)
        return;
    
    auto wgt = (__m256i*) (inputWeights[idx]);
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
void nn::Evaluator::reset(Board* board) {
    std::memset(inputMap , 0, sizeof(bool) * INPUT_SIZE);
    std::memcpy(summation, inputBias, sizeof(int16_t) * HIDDEN_SIZE);
    
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
int nn::Evaluator::evaluate(Board* board) {
    if(board != nullptr){
        reset(board);
    }
    
    constexpr __m256i reluBias {};
    
    __m256i*          sum = (__m256i*) (summation);
    __m256i*          act = (__m256i*) (activation);
    
    // apply relu to the summation first
    for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
        act[i] = _mm256_max_epi16(sum[i], reluBias);
//            print_256i_epi16(act[i]);
    }
    
    // do the sum for the output neurons
    for (int o = 0; o < OUTPUT_SIZE; o++) {
        
        auto wgt = (__m256i*) (hiddenWeights[o]);
        
        __m256i  res {};
        for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
            res = _mm256_add_epi32(res, _mm256_madd_epi16(act[i], wgt[i]));
        }
        
        // credit to Connor
        const __m128i reduced_4 = _mm_add_epi32(_mm256_castsi256_si128(res), _mm256_extractf128_si256 (res, 1));
        __m128i vsum = _mm_add_epi32(reduced_4, _mm_srli_si128(reduced_4, 8));
        vsum = _mm_add_epi32(vsum, _mm_srli_si128(vsum, 4));
        int32_t sum = _mm_cvtsi128_si32(vsum);
        
        output[o] = sum + hiddenBias[0];
    }
    
    
    return output[0] / 16 / 1024;
}

template
void nn::Evaluator::setPieceOnSquare<true>(bb::PieceType pieceType, bb::Color pieceColor,bb::Square square);
template
void nn::Evaluator::setPieceOnSquare<false>(bb::PieceType pieceType, bb::Color pieceColor,bb::Square square);