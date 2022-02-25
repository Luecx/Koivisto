
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

alignas(ALIGNMENT) int16_t nn::inputWeights[INPUT_SIZE][HIDDEN_SIZE];
alignas(ALIGNMENT) int16_t nn::hiddenWeights[OUTPUT_SIZE][HIDDEN_DSIZE];
alignas(ALIGNMENT) int16_t nn::inputBias[HIDDEN_SIZE];
alignas(ALIGNMENT) int32_t nn::hiddenBias[OUTPUT_SIZE];

#define INPUT_WEIGHT_MULTIPLIER  (128)
#define HIDDEN_WEIGHT_MULTIPLIER (256)

#if defined(__AVX512F__)
typedef __m512i avx_register_type_16;
typedef __m512i avx_register_type_32;
#define avx_madd_epi16(a, b) (_mm512_madd_epi16(a, b))
#define avx_add_epi32(a, b)  (_mm512_add_epi32(a, b))
#define avx_sub_epi32(a, b)  (_mm512_sub_epi32(a, b))
#define avx_add_epi16(a, b)  (_mm512_add_epi16(a, b))
#define avx_sub_epi16(a, b)  (_mm512_sub_epi16(a, b))
#define avx_max_epi16(a, b)  (_mm512_max_epi16(a, b))
#elif defined(__AVX2__) || defined(__AVX__)
typedef __m256i avx_register_type_16;
typedef __m256i avx_register_type_32;
#define avx_madd_epi16(a, b) (_mm256_madd_epi16(a, b))
#define avx_add_epi32(a, b)  (_mm256_add_epi32(a, b))
#define avx_sub_epi32(a, b)  (_mm256_sub_epi32(a, b))
#define avx_add_epi16(a, b)  (_mm256_add_epi16(a, b))
#define avx_sub_epi16(a, b)  (_mm256_sub_epi16(a, b))
#define avx_max_epi16(a, b)  (_mm256_max_epi16(a, b))
#elif defined(__SSE2__)
typedef __m128i avx_register_type_16;
typedef __m128i avx_register_type_32;
#define avx_madd_epi16(a, b) (_mm_madd_epi16(a, b))
#define avx_add_epi32(a, b)  (_mm_add_epi32(a, b))
#define avx_sub_epi32(a, b)  (_mm_sub_epi32(a, b))
#define avx_add_epi16(a, b)  (_mm_add_epi16(a, b))
#define avx_sub_epi16(a, b)  (_mm_sub_epi16(a, b))
#define avx_max_epi16(a, b)  (_mm_max_epi16(a, b))
#elif defined(__ARM_NEON)
typedef int16x8_t avx_register_type_16;
typedef int32x4_t avx_register_type_32;
#define avx_madd_epi16(a, b) (vpaddq_s32(vmull_s16(vget_low_s16(a),  vget_low_s16(b)), vmull_high_s16(a, b)))
#define avx_add_epi32(a, b)  (vaddq_s32(a, b))
#define avx_sub_epi32(a, b)  (vsubq_s32(a, b))
#define avx_add_epi16(a, b)  (vaddq_s16(a, b))
#define avx_sub_epi16(a, b)  (vsubq_s16(a, b))
#define avx_max_epi16(a, b)  (vmaxq_s16(a, b))
#endif

INCBIN(Eval, EVALFILE);

inline int32_t sumRegisterEpi32(avx_register_type_32 reg) {
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

#if defined(__ARM_NEON)
    return vaddvq_s32(reg);
#else
    // summarize the 128 register using SSE instructions
    __m128i vsum = _mm_add_epi32(reduced_4, _mm_srli_si128(reduced_4, 8));
    vsum = _mm_add_epi32(vsum, _mm_srli_si128(vsum, 4));
    int32_t sums = _mm_cvtsi128_si32(vsum);
    return sums;
#endif
}

void nn::init() {


    int memoryIndex = 0;
    std::memcpy(inputWeights, &gEvalData[memoryIndex], INPUT_SIZE * HIDDEN_SIZE * sizeof(int16_t));
    memoryIndex += INPUT_SIZE * HIDDEN_SIZE * sizeof(int16_t);
    std::memcpy(inputBias, &gEvalData[memoryIndex], HIDDEN_SIZE * sizeof(int16_t));
    memoryIndex += HIDDEN_SIZE * sizeof(int16_t);

    std::memcpy(hiddenWeights, &gEvalData[memoryIndex], HIDDEN_DSIZE * OUTPUT_SIZE * sizeof(int16_t));
    memoryIndex += HIDDEN_DSIZE * OUTPUT_SIZE * sizeof(int16_t);
    std::memcpy(hiddenBias, &gEvalData[memoryIndex], OUTPUT_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * sizeof(int32_t);

}

int nn::Evaluator::index(bb::PieceType pieceType,
                         bb::Color pieceColor,
                         bb::Square square,
                         bb::Color view,
                         bb::Square kingSquare) {

//    constexpr int pieceTypeFactor  = 64;
//    constexpr int pieceColorFactor = 64 * 6;
//    constexpr int kingSideFactor   = 64 * 6 * 2;
//
//    const Square  relativeSquare   = view == WHITE ? square : mirrorVertically(square);
//
//    return relativeSquare
//           + pieceType * pieceTypeFactor
//           + (pieceColor == view) * pieceColorFactor
//           + (fileIndex(kingSquare) > 3) * kingSideFactor;

    constexpr int pieceTypeFactor = 64;
    constexpr int pieceColorFactor = 64 * 6;
    constexpr int kingSquareFactor = 64 * 6 * 2;

    const bool kingSide = fileIndex(kingSquare) > 3;
    const int ksIndex = kingSquareIndex(kingSquare, view);
    Square relativeSquare = view == WHITE ? square : mirrorVertically(square);

    if (kingSide) {
        relativeSquare = mirrorHorizontally(relativeSquare);
    }

    return relativeSquare
           + pieceType * pieceTypeFactor
           + (pieceColor == view) * pieceColorFactor
           + ksIndex * kingSquareFactor;

}

int nn::Evaluator::kingSquareIndex(bb::Square relativeKingSquare, bb::Color kingColor) {
    // return zero if there is no king on the board yet ->
    // requires manual reset
    if (relativeKingSquare > 63) return 0;

    constexpr int indices[N_SQUARES]{
            0, 1, 2, 3, 3, 2, 1, 0,
            4, 5, 6, 7, 7, 6, 5, 4,
            8, 9, 10, 11, 11, 10, 9, 8,
            8, 9, 10, 11, 11, 10, 9, 8,
            12, 12, 13, 13, 13, 13, 12, 12,
            12, 12, 13, 13, 13, 13, 12, 12,
            14, 14, 15, 15, 15, 15, 14, 14,
            14, 14, 15, 15, 15, 15, 14, 14,

    };

    if (kingColor == BLACK) {
        relativeKingSquare = mirrorVertically(relativeKingSquare);
    }
    return indices[relativeKingSquare];
}

#if not(defined(__ARM_NEON))
inline void print_256i_epi16(const __m256i &h) {
    printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d \n",
           _mm256_extract_epi16(h, 0),
           _mm256_extract_epi16(h, 1),
           _mm256_extract_epi16(h, 2),
           _mm256_extract_epi16(h, 3),
           _mm256_extract_epi16(h, 4),
           _mm256_extract_epi16(h, 5),
           _mm256_extract_epi16(h, 6),
           _mm256_extract_epi16(h, 7),
           _mm256_extract_epi16(h, 8),
           _mm256_extract_epi16(h, 9),
           _mm256_extract_epi16(h, 10),
           _mm256_extract_epi16(h, 11),
           _mm256_extract_epi16(h, 12),
           _mm256_extract_epi16(h, 13),
           _mm256_extract_epi16(h, 14),
           _mm256_extract_epi16(h, 15));
}

inline void print_256i_epi32(const __m256i &h) {
    printf("%d %d %d %d %d %d %d %d \n",
           _mm256_extract_epi32(h, 0),
           _mm256_extract_epi32(h, 1),
           _mm256_extract_epi32(h, 2),
           _mm256_extract_epi32(h, 3),
           _mm256_extract_epi32(h, 4),
           _mm256_extract_epi32(h, 5),
           _mm256_extract_epi32(h, 6),
           _mm256_extract_epi32(h, 7));
}
#endif


template<bool value>
void nn::Evaluator::setPieceOnSquare(bb::PieceType pieceType,
                                     bb::Color pieceColor,
                                     bb::Square square,
                                     bb::Square wKingSquare,
                                     bb::Square bKingSquare) {

    setPieceOnSquareAccumulator<value>(WHITE, pieceType, pieceColor, square, wKingSquare);
    setPieceOnSquareAccumulator<value>(BLACK, pieceType, pieceColor, square, bKingSquare);
}


template<bool value>
void nn::Evaluator::setPieceOnSquareAccumulator(bb::Color side,
                                                bb::PieceType pieceType,
                                                bb::Color pieceColor,
                                                bb::Square square,
                                                bb::Square kingSquare) {
    int idx = index(pieceType, pieceColor, square, side, kingSquare);

    auto wgt = (avx_register_type_16 *) (inputWeights[idx]);
    auto sum = (avx_register_type_16 *) (history.back().summation[side]);
    if constexpr (value) {
        for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT / 4; i++) {
            sum[i * 4 + 0] = avx_add_epi16(sum[i * 4 + 0], wgt[i * 4 + 0]);
            sum[i * 4 + 1] = avx_add_epi16(sum[i * 4 + 1], wgt[i * 4 + 1]);
            sum[i * 4 + 2] = avx_add_epi16(sum[i * 4 + 2], wgt[i * 4 + 2]);
            sum[i * 4 + 3] = avx_add_epi16(sum[i * 4 + 3], wgt[i * 4 + 3]);
        }
    } else {
        for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT / 4; i++) {
            sum[i * 4 + 0] = avx_sub_epi16(sum[i * 4 + 0], wgt[i * 4 + 0]);
            sum[i * 4 + 1] = avx_sub_epi16(sum[i * 4 + 1], wgt[i * 4 + 1]);
            sum[i * 4 + 2] = avx_sub_epi16(sum[i * 4 + 2], wgt[i * 4 + 2]);
            sum[i * 4 + 3] = avx_sub_epi16(sum[i * 4 + 3], wgt[i * 4 + 3]);
        }
    }
}

void nn::Evaluator::reset(Board *board) {
    resetAccumulator(board, WHITE);
    resetAccumulator(board, BLACK);
}

void nn::Evaluator::resetAccumulator(Board *board, bb::Color color) {
    std::memcpy(history.back().summation[color], inputBias, sizeof(int16_t) * HIDDEN_SIZE);

    Square kingSquare = bitscanForward(board->getPieceBB(color, KING));

    for (Color c : {WHITE, BLACK}) {
        for (PieceType pt : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
            U64 bb = board->getPieceBB(c, pt);
            while (bb) {
                Square s = bitscanForward(bb);

                setPieceOnSquareAccumulator<true>(color, pt, c, s, kingSquare);

                bb = lsbReset(bb);
            }
        }
    }
}


int nn::Evaluator::evaluate(bb::Color activePlayer, Board *board) {
    if (board != nullptr) {
        reset(board);
    }
    constexpr avx_register_type_16 reluBias{};

    auto acc_act = (avx_register_type_16 *) history.back().summation[activePlayer];
    auto acc_nac = (avx_register_type_16 *) history.back().summation[!activePlayer];

    // compute the dot product
    avx_register_type_32 res{};
    auto wgt = (avx_register_type_16 *) (hiddenWeights[0]);
    for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
        res = avx_add_epi32(res, avx_madd_epi16(avx_max_epi16(acc_act[i], reluBias), wgt[i]));
    }
    for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
        res = avx_add_epi32(res,
                            avx_madd_epi16(avx_max_epi16(acc_nac[i], reluBias), wgt[i + HIDDEN_SIZE / STRIDE_16_BIT]));
    }


    auto outp = sumRegisterEpi32(res) + hiddenBias[0];
    return outp / INPUT_WEIGHT_MULTIPLIER / HIDDEN_WEIGHT_MULTIPLIER;
}

nn::Evaluator::Evaluator() {
    this->history.push_back(Accumulator{});
}

void nn::Evaluator::addNewAccumulation() {
    this->history.emplace_back(this->history.back());
}

void nn::Evaluator::popAccumulation() {
    this->history.pop_back();
}

void nn::Evaluator::clearHistory() {
    this->history.clear();
    this->history.push_back(Accumulator{});
}

template
void nn::Evaluator::setPieceOnSquare<true>(bb::PieceType pieceType,
                                           bb::Color pieceColor,
                                           bb::Square square,
                                           bb::Square wKingSquare,
                                           bb::Square bKingSquare);

template
void nn::Evaluator::setPieceOnSquare<false>(bb::PieceType pieceType,
                                            bb::Color pieceColor,
                                            bb::Square square,
                                            bb::Square wKingSquare,
                                            bb::Square bKingSquare);
