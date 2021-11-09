
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

alignas(ALIGNMENT) int16_t nn::inputWeights [INPUT_SIZE][HIDDEN_SIZE];
alignas(ALIGNMENT) int16_t nn::hiddenWeights[OUTPUT_SIZE][HIDDEN_DSIZE];
alignas(ALIGNMENT) int16_t nn::inputBias    [HIDDEN_SIZE];
alignas(ALIGNMENT) int32_t nn::hiddenBias   [OUTPUT_SIZE];

#define INPUT_WEIGHT_MULTIPLIER  (128)
#define HIDDEN_WEIGHT_MULTIPLIER (128)

#if defined(__AVX512F__)
typedef __m512i avx_register_type;
#define avx_madd_epi16(a, b) (_mm512_madd_epi16(a, b))
#define avx_add_epi32(a, b)  (_mm512_add_epi32(a, b))
#define avx_sub_epi32(a, b)  (_mm512_sub_epi32(a, b))
#define avx_add_epi16(a, b)  (_mm512_add_epi16(a, b))
#define avx_sub_epi16(a, b)  (_mm512_sub_epi16(a, b))
#define avx_max_epi16(a, b)  (_mm512_max_epi16(a, b))
#elif defined(__AVX2__) || defined(__AVX__)
typedef __m256i avx_register_type;
#define avx_madd_epi16(a, b) (_mm256_madd_epi16(a, b))
#define avx_add_epi32(a, b)  (_mm256_add_epi32(a, b))
#define avx_sub_epi32(a, b)  (_mm256_sub_epi32(a, b))
#define avx_add_epi16(a, b)  (_mm256_add_epi16(a, b))
#define avx_sub_epi16(a, b)  (_mm256_sub_epi16(a, b))
#define avx_max_epi16(a, b)  (_mm256_max_epi16(a, b))
#elif defined(__SSE2__)
typedef __m128i avx_register_type;
#define avx_madd_epi16(a, b) (_mm_madd_epi16(a, b))
#define avx_add_epi32(a, b)  (_mm_add_epi32(a, b))
#define avx_sub_epi32(a, b)  (_mm_sub_epi32(a, b))
#define avx_add_epi16(a, b)  (_mm_add_epi16(a, b))
#define avx_sub_epi16(a, b)  (_mm_sub_epi16(a, b))
#define avx_max_epi16(a, b)  (_mm_max_epi16(a, b))
#endif

INCBIN(Eval, EVALFILE);

inline int32_t sumRegisterEpi32(avx_register_type reg){
    // first summarize in case of avx512 registers into one 256 bit register
#if defined(__AVX512F__)
    const __m256i reduced_8 =
            _mm256_add_epi32(_mm512_castsi512_si256(reg), _mm512_extracti32x8_epi32(reg, 1));
#elif defined(__AVX2__) || defined(__AVX__)
    const __m256i reduced_8 = reg;
#endif
    
    // then summarize the 256 bit register into a 128 bit register
#if defined(__AVX512F__) || defined(__AVX2__) || defined(__AVX__)
    const __m128i reduced_4 =
                      _mm_add_epi32(_mm256_castsi256_si128(reduced_8), _mm256_extractf128_si256(reduced_8, 1));
#elif defined(__SSE2__)
    const __m128i reduced_4 = reg;
#endif
    
    // summarize the 128 register using SSE instructions
    __m128i vsum = _mm_add_epi32(reduced_4, _mm_srli_si128(reduced_4, 8));
    vsum         = _mm_add_epi32(vsum, _mm_srli_si128(vsum, 4));
    int32_t sums = _mm_cvtsi128_si32(vsum);
    return sums;
}

void nn::init() {
    
    
    int memoryIndex = 0;
    std::memcpy(inputWeights, &gEvalData[memoryIndex],   INPUT_SIZE * HIDDEN_SIZE * sizeof(int16_t));
    memoryIndex += INPUT_SIZE * HIDDEN_SIZE * sizeof(int16_t);
    std::memcpy(inputBias   , &gEvalData[memoryIndex],                HIDDEN_SIZE * sizeof(int16_t));
    memoryIndex +=              HIDDEN_SIZE * sizeof(int16_t);
    
    std::memcpy(hiddenWeights, &gEvalData[memoryIndex],  HIDDEN_DSIZE * OUTPUT_SIZE * sizeof(int16_t));
    memoryIndex += HIDDEN_DSIZE * OUTPUT_SIZE * sizeof(int16_t);
    std::memcpy(hiddenBias   , &gEvalData[memoryIndex],                 OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex +=               OUTPUT_SIZE * sizeof(int32_t);
}
int nn::Evaluator::index(bb::PieceType pieceType,
                         bb::Color pieceColor,
                         bb::Square square,
                         bb::Color view,
                         bb::Square kingSquare) {

    constexpr int pieceTypeFactor  = 64;
    constexpr int pieceColorFactor = 64 * 6;
    constexpr int kingSideFactor   = 64 * 6 * 2;

    const Square  relativeSquare   = view == WHITE ? square : mirrorSquare(square);

    return relativeSquare
           + pieceType * pieceTypeFactor
           + (pieceColor == view) * pieceColorFactor
           + (fileIndex(kingSquare) > 3) * kingSideFactor;
}

template<bool value>
void nn::Evaluator::setPieceOnSquare(bb::PieceType pieceType,
                                     bb::Color pieceColor,
                                     bb::Square square,
                                     bb::Square wKingSquare,
                                     bb::Square bKingSquare) {
    
    int idxWhite    = index(pieceType, pieceColor, square, WHITE, wKingSquare);
    int idxBlack    = index(pieceType, pieceColor, square, BLACK, bKingSquare);
    int idx[N_COLORS] {idxWhite, idxBlack};
    
    for (Color c : {WHITE, BLACK}) {

        auto wgt = (avx_register_type*) (inputWeights[idx[c]]);
        auto sum = (avx_register_type*) (history.back().summation[c]);
        if constexpr (value) {
            for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
                sum[i] = avx_add_epi16(sum[i], wgt[i]);
            }
        } else {
            for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
                sum[i] = avx_sub_epi16(sum[i], wgt[i]);
            }
        }
    }
}

void nn::Evaluator::reset(Board* board) {
    std::memcpy(history.back().summation[WHITE], inputBias, sizeof(int16_t) * HIDDEN_SIZE);
    std::memcpy(history.back().summation[BLACK], inputBias, sizeof(int16_t) * HIDDEN_SIZE);
    
    Square wKingSq = bitscanForward(board->getPieceBB<WHITE>(KING));
    Square bKingSq = bitscanForward(board->getPieceBB<BLACK>(KING));
    
    for (Color c : {WHITE, BLACK}) {
        for (PieceType pt : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
            U64 bb = board->getPieceBB(c, pt);
            while (bb) {
                Square s = bitscanForward(bb);

                setPieceOnSquare<true>(pt, c, s, wKingSq, bKingSq);

                bb = lsbReset(bb);
            }
        }
    }
}

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

int nn::Evaluator::evaluate(bb::Color activePlayer, Board* board) {
    if (board != nullptr) {
        reset(board);
    }
    
    // concat based on stm
    std::memcpy( activation             , history.back().summation[ activePlayer], sizeof(uint16_t) * HIDDEN_SIZE);
    std::memcpy(&activation[HIDDEN_SIZE], history.back().summation[!activePlayer], sizeof(uint16_t) * HIDDEN_SIZE);

    constexpr avx_register_type reluBias {};
    avx_register_type*          act = (avx_register_type*) (activation);

    // apply relu to the summation first
    for (int i = 0; i < HIDDEN_DSIZE / STRIDE_16_BIT; i++) {
        act[i] = avx_max_epi16(act[i], reluBias);
    }

    // do the sum for the output neurons
    for (int o = 0; o < OUTPUT_SIZE; o++) {
//        int check_sum = 0;
//        for(int i = 0; i < 512; i++){
//            check_sum += (int)hiddenWeights[o][i] * (int)activation[i];
//            std::cout << i << "    "
//                      << (int)hiddenWeights[o][i] << "    "
//                      << (int)activation[i] << "    "
//                      << check_sum / HIDDEN_WEIGHT_MULTIPLIER / INPUT_WEIGHT_MULTIPLIER << std::endl;
//        }
    
        auto              wgt = (avx_register_type*) (hiddenWeights[o]);
        avx_register_type res {};
        for (int i = 0; i < HIDDEN_DSIZE / STRIDE_16_BIT; i++) {
            res = avx_add_epi32(res, avx_madd_epi16(act[i], wgt[i]));
        }

        // credit to Connor
        int32_t sums = sumRegisterEpi32(res);

        output[o]    = sums + hiddenBias[0];
    }

    return output[0] / INPUT_WEIGHT_MULTIPLIER / HIDDEN_WEIGHT_MULTIPLIER;
}

nn::Evaluator::Evaluator() {
    this->history.push_back(Accumulator{});
}
void          nn::Evaluator::addNewAccumulation() {
    this->history.emplace_back(this->history.back());
}
void          nn::Evaluator::popAccumulation() {
    this->history.pop_back();
}
void nn::Evaluator::clearHistory() {
    this->history.clear();
    this->history.push_back(Accumulator{});
}

template void nn::Evaluator::setPieceOnSquare<true>(bb::PieceType pieceType,
                                                    bb::Color pieceColor,
                                                    bb::Square square,
                                                    bb::Square wKingSquare,
                                                    bb::Square bKingSquare);
template void nn::Evaluator::setPieceOnSquare<false>(bb::PieceType pieceType,
                                                     bb::Color pieceColor,
                                                     bb::Square square,
                                                     bb::Square wKingSquare,
                                                     bb::Square bKingSquare);