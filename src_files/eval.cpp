
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
#define INCBIN_STYLE INCBIN_STYLE_CAMEL
#include "incbin/incbin.h"

alignas(32) int16_t nn::inputWeights [INPUT_SIZE ][HIDDEN_SIZE];
alignas(32) int16_t nn::hiddenWeights[OUTPUT_SIZE][HIDDEN_SIZE];
alignas(32) int16_t nn::inputBias    [HIDDEN_SIZE];
alignas(32) int32_t nn::hiddenBias   [OUTPUT_SIZE];

#define INPUT_WEIGHT_MULTIPLIER  (16)
#define HIDDEN_WEIGHT_MULTIPLIER (128)

INCBIN(Eval, EVALFILE);

void nn::init() {
    
    auto data = reinterpret_cast<const float*>(gEvalData+8);

#ifdef DNDEBUG
    // figure out how many entries we will store
    uint64_t count =
        + INPUT_SIZE * HIDDEN_SIZE
        + HIDDEN_SIZE
        + HIDDEN_SIZE * OUTPUT_SIZE
        + OUTPUT_SIZE;

    uint64_t fileCount = *reinterpret_cast<const uint64_t*>(gEvalData);
    UCI_ASSERT((count * 4 + 8) == gEvalSize);
    UCI_ASSERT( count          == fileCount);
#endif
    int memoryIndex = 0;
    
    // read weights
    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int o = 0; o < HIDDEN_SIZE; o++) {
            inputWeights[i][o] = round(data[memoryIndex++]
                                       * INPUT_WEIGHT_MULTIPLIER);
        }
    }
    
    // read bias
    for (int o = 0; o < HIDDEN_SIZE; o++) {
        inputBias[o] = round(data[memoryIndex++]
                             * INPUT_WEIGHT_MULTIPLIER);
    }

    // read weights
    for (int o = 0; o < OUTPUT_SIZE; o++) {
        for (int i = 0; i < HIDDEN_SIZE; i++) {
            hiddenWeights[o][i] = round(data[memoryIndex++]
                                        * HIDDEN_WEIGHT_MULTIPLIER);
        }
    }
    
    // read bias
    for (int o = 0; o < OUTPUT_SIZE; o++) {
        hiddenBias[o] = round(data[memoryIndex++]
                              * HIDDEN_WEIGHT_MULTIPLIER
                              * INPUT_WEIGHT_MULTIPLIER);
    }

}
int  nn::Evaluator::index(bb::PieceType pieceType, bb::Color pieceColor, bb::Square square) {
    constexpr int pieceTypeFactor  = 64;
    constexpr int pieceColorFactor = 64 * 6;

    return square + pieceType * pieceTypeFactor + pieceColor * pieceColorFactor;
}

template<bool value>
void nn::Evaluator::setPieceOnSquare(bb::PieceType pieceType, bb::Color pieceColor,
                                     bb::Square square) {
    int idx = index(pieceType, pieceColor, square);

    if (inputMap[idx] == value)
        return;
    inputMap[idx] = value;

    auto wgt      = (__m256i*) (inputWeights[idx]);
    auto sum      = (__m256i*) (summation);

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
    std::memset(inputMap, 0, sizeof(bool) * INPUT_SIZE);
    std::memcpy(summation, inputBias, sizeof(int16_t) * HIDDEN_SIZE);

    for (Color c : {WHITE, BLACK}) {
        for (PieceType pt : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
            U64 bb = board->getPieceBB(c, pt);
            while (bb) {
                Square s = bitscanForward(bb);

                setPieceOnSquare<true>(pt, c, s);

                bb = lsbReset(bb);
            }
        }
    }
}

int  nn::Evaluator::evaluate(Board* board) {
    if (board != nullptr) {
        reset(board);
    }

    constexpr __m256i reluBias {};

    __m256i*          sum = (__m256i*) (summation);
    __m256i*          act = (__m256i*) (activation);

    // apply relu to the summation first
    for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
        act[i] = _mm256_max_epi16(sum[i], reluBias);
    }

    // do the sum for the output neurons
    for (int o = 0; o < OUTPUT_SIZE; o++) {

        auto    wgt = (__m256i*) (hiddenWeights[o]);

        __m256i res {};
        for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
            res = _mm256_add_epi32(res, _mm256_madd_epi16(act[i], wgt[i]));
        }

        // credit to Connor
        const __m128i reduced_4 =
            _mm_add_epi32(_mm256_castsi256_si128(res), _mm256_extractf128_si256(res, 1));
        __m128i vsum = _mm_add_epi32(reduced_4, _mm_srli_si128(reduced_4, 8));
        vsum         = _mm_add_epi32(vsum, _mm_srli_si128(vsum, 4));
        int32_t sums = _mm_cvtsi128_si32(vsum);

        output[o]    = sums + hiddenBias[0];
    }

    return output[0] / HIDDEN_WEIGHT_MULTIPLIER / INPUT_WEIGHT_MULTIPLIER;
}

template void nn::Evaluator::setPieceOnSquare<true>(bb::PieceType pieceType, bb::Color pieceColor,
                                                    bb::Square square);
template void nn::Evaluator::setPieceOnSquare<false>(bb::PieceType pieceType, bb::Color pieceColor,
                                                     bb::Square square);