
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

alignas(ALIGNMENT)  int16_t nn::l1_weights[INPUT_SIZE ][L1_SIZE_HALF];
alignas(ALIGNMENT)  int16_t nn::l1_bias   [L1_SIZE_HALF];

alignas(ALIGNMENT)  int16_t nn::l2_weights[L2_SIZE * L1_SIZE];
alignas(ALIGNMENT)  int32_t nn::l2_bias   [L2_SIZE];

alignas(ALIGNMENT)  float   nn::l3_weights[OUTPUT_SIZE * L2_SIZE];
alignas(ALIGNMENT)  float   nn::l3_bias   [OUTPUT_SIZE];

#define INPUT_WEIGHT_MULTIPLIER  (64)
#define HIDDEN_WEIGHT_MULTIPLIER (64)

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

void nn::init() {
    #define LOAD(memory, source, type, size, memoryIndex){                       \
        std::memcpy((memory), &(source)[(memoryIndex)], (size) * sizeof(type));  \
        (memoryIndex) += (size) * sizeof(type);                                  \
    }

    int memoryIndex = 0;
    
    LOAD(l1_weights, gEvalData, int16_t, INPUT_SIZE  * L1_SIZE_HALF, memoryIndex);
    LOAD(l1_bias   , gEvalData, int16_t,               L1_SIZE_HALF, memoryIndex);
    LOAD(l2_weights, gEvalData, int16_t, L2_SIZE     * L1_SIZE     , memoryIndex);
    LOAD(l2_bias   , gEvalData, float  , L2_SIZE                   , memoryIndex);
    LOAD(l3_weights, gEvalData, float  , OUTPUT_SIZE * L2_SIZE     , memoryIndex);
    LOAD(l3_bias   , gEvalData, float  , OUTPUT_SIZE               , memoryIndex);
    
    #undef LOAD
}

int nn::Evaluator::index(bb::PieceType pieceType,
                         bb::Color pieceColor,
                         bb::Square square,
                         bb::Color view,
                         bb::Square kingSquare) {

    constexpr int pieceTypeFactor  = 64;
    constexpr int pieceColorFactor = 64 * 6;
    constexpr int kingSquareFactor = 64 * 6 * 2;

    const bool    kingSide         = fileIndex(kingSquare) > 3;
    const int     ksIndex          = kingSquareIndex(kingSquare, view);
    Square        relativeSquare   = view == WHITE ? square : mirrorVertically(square);

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
             0,  1,  2,  3,  3,  2,  1,  0,
             4,  5,  6,  7,  7,  6,  5,  4,
             8,  9, 10, 11, 11, 10,  9,  8,
             8,  9, 10, 11, 11, 10,  9,  8,
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

inline __m128i hsum4_epi32(__m256i a,
                           __m256i b,
                           __m256i c,
                           __m256i d) {
    __m256i sumab           = _mm256_hadd_epi32(a, b);
    __m256i sumcd           = _mm256_hadd_epi32(c, d);
    
    __m256i sumabcd_twice   = _mm256_hadd_epi32(sumab, sumcd);
    
    __m128i sumabcd_lo      = _mm256_castsi256_si128  (sumabcd_twice);
    __m128i sumabcd_hi      = _mm256_extracti128_si256(sumabcd_twice, 1);
    
    return _mm_add_epi32(sumabcd_hi, sumabcd_lo);
}

inline void relu_accumulator(int16_t* accumulator_1,
                             int16_t* accumulator_2,
                             int16_t * result,
                             Color activePlayer){
    
    // for avx2 we can process 256 bits at once. 256 equals 16 * 16 bits (short) in the result
    // therefor we need to process S/16 chunks where S is half the size of the result.
    // if size(result) == 1024, then size(acc1) = size(acc2) = S
    constexpr int chunks = L1_SIZE_HALF / 16;
    
    const auto* acc1 = (__m256i*) accumulator_1;
    const auto* acc2 = (__m256i*) accumulator_2;
    
    auto* res_acc_1 = (__m256i*) &result[activePlayer == WHITE ? 0:L1_SIZE_HALF];
    auto* res_acc_2 = (__m256i*) &result[activePlayer == WHITE ? L1_SIZE_HALF:0];
    
    auto ZERO_256 = _mm256_set1_epi16(0);
    
    // compute each chunk for accumulator 1
    // we store the result in res_acc_1 which is either the first or second half of
    // the result array. This depends on the active player
    for(int i = 0; i < chunks; i++){
        res_acc_1[i] = _mm256_max_epi16(acc1[i], ZERO_256);
    }
    for(int i = 0; i < chunks; i++){
        res_acc_2[i] = _mm256_max_epi16(acc2[i], ZERO_256);
    }
}

template<int I, int O, int S>
inline void affine_epi16(int16_t* weights,
                         int16_t* input,
                         int32_t* bias,
                         float* output){
    
    constexpr int ROWS = O / 4;
    constexpr int COLS = I / 16;
    
    __m128i RELU = _mm_set1_epi32(0);
    
    auto* inp = (avx_register_type_16*) (input);
    auto* wgt = (avx_register_type_16*) (weights);
    auto* bia = (__m128i*) (bias);
    auto* out = (__m128*) (output);
    
    for(int o = 0; o < ROWS; o++){
        // track two sets of 32 bits (128 bits in total)
        avx_register_type_32 acc1{};
        avx_register_type_32 acc2{};
        avx_register_type_32 acc3{};
        avx_register_type_32 acc4{};
        
        
        for(int i = 0; i < COLS; i++){
            int offset = i + 4 * o * COLS;
            
            acc1 = avx_add_epi32(avx_madd_epi16(wgt[offset + 0 * COLS], inp[i]), acc1);
            acc2 = avx_add_epi32(avx_madd_epi16(wgt[offset + 1 * COLS], inp[i]), acc2);
            acc3 = avx_add_epi32(avx_madd_epi16(wgt[offset + 2 * COLS], inp[i]), acc3);
            acc4 = avx_add_epi32(avx_madd_epi16(wgt[offset + 3 * COLS], inp[i]), acc4);
        }
        
        // sum up into 32 bit
        __m128i total_1 = hsum4_epi32(acc1, acc2, acc3, acc4);
        // apply relu onto the 32 bit registers
        total_1 = _mm_max_epi32(RELU, _mm_add_epi32(bia[o],total_1));
        // shift 32 bits down
        total_1 = _mm_srai_epi32(total_1, S);
        // convert to floating point
        out[o] = _mm_cvtepi32_ps(total_1);
    }
    
}

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

    auto wgt = (avx_register_type_16 *) (l1_weights[idx]);
    auto sum = (avx_register_type_16 *) (history.back().summation[side]);
    if constexpr (value) {
        for (int i = 0; i < L1_SIZE_HALF / STRIDE_16_BIT / 4; i++) {
            sum[i * 4 + 0] = avx_add_epi16(sum[i * 4 + 0], wgt[i * 4 + 0]);
            sum[i * 4 + 1] = avx_add_epi16(sum[i * 4 + 1], wgt[i * 4 + 1]);
            sum[i * 4 + 2] = avx_add_epi16(sum[i * 4 + 2], wgt[i * 4 + 2]);
            sum[i * 4 + 3] = avx_add_epi16(sum[i * 4 + 3], wgt[i * 4 + 3]);
        }
    } else {
        for (int i = 0; i < L1_SIZE_HALF / STRIDE_16_BIT / 4; i++) {
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
    std::memcpy(history.back().summation[color], l1_bias, sizeof(int16_t) * L1_SIZE_HALF);

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
    
    relu_accumulator(history.back().summation[WHITE],
                     history.back().summation[BLACK],
                     l1_activation,
                     activePlayer);

    affine_epi16<L1_SIZE, L2_SIZE, 0>(l2_weights, l1_activation, l2_bias, l2_activation);
    
    float res = l3_bias[0];
    for(int i = 0; i < 8; i++){
        res += l2_activation[i] * l3_weights[i];
    }
    
    return res / INPUT_WEIGHT_MULTIPLIER / HIDDEN_WEIGHT_MULTIPLIER;
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
