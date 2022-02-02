
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

alignas(ALIGNMENT) int16_t nn::l1_weights[INPUT_SIZE  ][HIDDEN_SIZE ]{};
alignas(ALIGNMENT)  int8_t nn::l2_weights[HIDDEN2_SIZE][HIDDEN_DSIZE]{};
alignas(ALIGNMENT) float   nn::l3_weights[HIDDEN2_SIZE]{};
alignas(ALIGNMENT) int16_t nn::l1_bias   [HIDDEN_SIZE ]{};
alignas(ALIGNMENT) int32_t nn::l2_bias   [HIDDEN2_SIZE]{};
alignas(ALIGNMENT) float   nn::l3_bias{};


#define INPUT_WEIGHT_MULTIPLIER  (128)
#define HIDDEN_WEIGHT_MULTIPLIER (256)

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


const __m256i ONES     = _mm256_set1_epi16(1);
const __m256i ZERO_256 = _mm256_set1_epi16(0);
const __m128i ZERO_128 = _mm_set1_epi32(0);


void nn::init() {
    int memoryIndex = 0;
    std::memcpy(l1_weights, &gEvalData[memoryIndex],   INPUT_SIZE * HIDDEN_SIZE * sizeof(int16_t));
    memoryIndex += INPUT_SIZE * HIDDEN_SIZE * sizeof(int16_t);
    std::memcpy(l1_bias   , &gEvalData[memoryIndex],                HIDDEN_SIZE * sizeof(int16_t));
    memoryIndex +=              HIDDEN_SIZE * sizeof(int16_t);

    std::memcpy(l2_weights, &gEvalData[memoryIndex],  HIDDEN_DSIZE * HIDDEN2_SIZE * sizeof(int8_t));
    memoryIndex += HIDDEN_DSIZE * HIDDEN2_SIZE * sizeof(int8_t);
    std::memcpy(l2_bias   , &gEvalData[memoryIndex],                 HIDDEN2_SIZE * sizeof(int32_t));
    memoryIndex +=                HIDDEN2_SIZE * sizeof(int32_t);
    
    std::memcpy(l3_weights, &gEvalData[memoryIndex],  HIDDEN2_SIZE * OUTPUT_SIZE * sizeof(float));
    memoryIndex += HIDDEN2_SIZE * OUTPUT_SIZE * sizeof(float);
    std::memcpy(&l3_bias  , &gEvalData[memoryIndex],                 OUTPUT_SIZE * sizeof(float));
    memoryIndex +=                OUTPUT_SIZE * sizeof(float);
    
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

    constexpr int pieceTypeFactor  = 64;
    constexpr int pieceColorFactor = 64 * 6;
    constexpr int kingSquareFactor = 64 * 6 * 2;
    
    const bool      kingSide           = fileIndex(kingSquare) > 3;
    const int       ksIndex            = kingSquareIndex(kingSquare, view);
    Square          relativeSquare     = view == WHITE ? square : mirrorVertically(square);
    
    if (kingSide) {
        relativeSquare = mirrorHorizontally(relativeSquare);
    }
    
    return relativeSquare
           + pieceType              * pieceTypeFactor
           + (pieceColor == view)   * pieceColorFactor
           + ksIndex                * kingSquareFactor;

}

int nn::Evaluator::kingSquareIndex( bb::Square relativeKingSquare, bb::Color kingColor){
    // return zero if there is no king on the board yet ->
    // requires manual reset
    if(relativeKingSquare > 63) return 0;
    
    constexpr int indices[N_SQUARES]{
        0 ,1 ,2 ,3 ,3 ,2 ,1 ,0 ,
        4 ,5 ,6 ,7 ,7 ,6 ,5 ,4 ,
        8 ,9 ,10,11,11,10,9 ,8 ,
        8 ,9 ,10,11,11,10,9 ,8 ,
        12,12,13,13,13,13,12,12,
        12,12,13,13,13,13,12,12,
        14,14,15,15,15,15,14,14,
        14,14,15,15,15,15,14,14,
        
    };
    
    if(kingColor == BLACK){
        relativeKingSquare = mirrorVertically(relativeKingSquare);
    }
    return indices[relativeKingSquare];
}

inline void print_256i_epi8(const __m256i &h){
    printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d \n",
           (signed) _mm256_extract_epi8(h,0),
           (signed) _mm256_extract_epi8(h,1),
           (signed) _mm256_extract_epi8(h,2),
           (signed) _mm256_extract_epi8(h,3),
           (signed) _mm256_extract_epi8(h,4),
           (signed) _mm256_extract_epi8(h,5),
           (signed) _mm256_extract_epi8(h,6),
           (signed) _mm256_extract_epi8(h,7),
           (signed) _mm256_extract_epi8(h,8),
           (signed) _mm256_extract_epi8(h,9),
           (signed) _mm256_extract_epi8(h,10),
           (signed) _mm256_extract_epi8(h,11),
           (signed) _mm256_extract_epi8(h,12),
           (signed) _mm256_extract_epi8(h,13),
           (signed) _mm256_extract_epi8(h,14),
           (signed) _mm256_extract_epi8(h,15),
           (signed) _mm256_extract_epi8(h,16+0),
           (signed) _mm256_extract_epi8(h,16+1),
           (signed) _mm256_extract_epi8(h,16+2),
           (signed) _mm256_extract_epi8(h,16+3),
           (signed) _mm256_extract_epi8(h,16+4),
           (signed) _mm256_extract_epi8(h,16+5),
           (signed) _mm256_extract_epi8(h,16+6),
           (signed) _mm256_extract_epi8(h,16+7),
           (signed) _mm256_extract_epi8(h,16+8),
           (signed) _mm256_extract_epi8(h,16+9),
           (signed) _mm256_extract_epi8(h,16+10),
           (signed) _mm256_extract_epi8(h,16+11),
           (signed) _mm256_extract_epi8(h,16+12),
           (signed) _mm256_extract_epi8(h,16+13),
           (signed) _mm256_extract_epi8(h,16+14),
           (signed) _mm256_extract_epi8(h,16+15));
}

inline void print_256i_epi16(const __m256i &h){
    printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d \n",
           (signed short)_mm256_extract_epi16(h,0),
           (signed short)_mm256_extract_epi16(h,1),
           (signed short)_mm256_extract_epi16(h,2),
           (signed short)_mm256_extract_epi16(h,3),
           (signed short)_mm256_extract_epi16(h,4),
           (signed short)_mm256_extract_epi16(h,5),
           (signed short)_mm256_extract_epi16(h,6),
           (signed short)_mm256_extract_epi16(h,7),
           (signed short)_mm256_extract_epi16(h,8),
           (signed short)_mm256_extract_epi16(h,9),
           (signed short)_mm256_extract_epi16(h,10),
           (signed short)_mm256_extract_epi16(h,11),
           (signed short)_mm256_extract_epi16(h,12),
           (signed short)_mm256_extract_epi16(h,13),
           (signed short)_mm256_extract_epi16(h,14),
           (signed short)_mm256_extract_epi16(h,15));
}

inline void print_256i_epi32(const __m256i &h){
    printf("%d %d %d %d %d %d %d %d \n",
           (signed int)_mm256_extract_epi32(h,0),
           (signed int)_mm256_extract_epi32(h,1),
           (signed int)_mm256_extract_epi32(h,2),
           (signed int)_mm256_extract_epi32(h,3),
           (signed int)_mm256_extract_epi32(h,4),
           (signed int)_mm256_extract_epi32(h,5),
           (signed int)_mm256_extract_epi32(h,6),
           (signed int)_mm256_extract_epi32(h,7));
}

inline void print_128i_epi32(const __m128i &h){
    printf("%d %d %d %d \n",
           (signed int)_mm_extract_epi32(h,0),
           (signed int)_mm_extract_epi32(h,1),
           (signed int)_mm_extract_epi32(h,2),
           (signed int)_mm_extract_epi32(h,3));
}


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


template<int I, int O>
inline void affine_epi8(uint8_t* input, int8_t* weights, int32_t* bias, float* result){

    UCI_ASSERT(O % 8 == 0);
    UCI_ASSERT(I % BYTE_ALIGNMENT == 0);
    
    constexpr int ROW_ITER = O / 4;
    constexpr int COL_ITER = I / 32;
    
    auto* inp = (__m256i*) input;
    auto* wgt = (__m256i*) weights;
    auto* bia = (__m128i*) bias;
    auto* res = (__m128 *) result;
    
    
    // only do avx2 for now
    for(int o = 0; o < ROW_ITER; o++){
        __m256i acc1{};
        __m256i acc2{};
        __m256i acc3{};
        __m256i acc4{};

        for (int i = 0; i < COL_ITER; i++) {
            int offset = i + 4 * o * COL_ITER;
            acc1 = _mm256_add_epi32(acc1, _mm256_madd_epi16(ONES, _mm256_maddubs_epi16(inp[i], wgt[offset + 0])));
            acc2 = _mm256_add_epi32(acc2, _mm256_madd_epi16(ONES, _mm256_maddubs_epi16(inp[i], wgt[offset + 1 * COL_ITER])));
            acc3 = _mm256_add_epi32(acc3, _mm256_madd_epi16(ONES, _mm256_maddubs_epi16(inp[i], wgt[offset + 2 * COL_ITER])));
            acc4 = _mm256_add_epi32(acc4, _mm256_madd_epi16(ONES, _mm256_maddubs_epi16(inp[i], wgt[offset + 3 * COL_ITER])));
        }
        
        __m128i total = hsum4_epi32(acc1, acc2, acc3, acc4);
        res[o] = _mm_cvtepi32_ps((_mm_add_epi32(bia[o],total)));
    }
}

template<int S, int K>
inline void clipped_relu_accumulator(int16_t* accumulator_1, int16_t* accumulator_2, uint8_t* result, Color activePlayer){
    
    // for avx2 we can process 256 bit at once. 256 equals 32 words (unsigned bytes) in the result
    // therefor we need to process S/32 chunks where S is half the size of the result.
    // if size(result) == 1024, then size(acc1) = size(acc2) = S
    constexpr int chunks = S / 32;
    
    const auto* acc1 = (__m256i*) accumulator_1;
    const auto* acc2 = (__m256i*) accumulator_2;
    
    auto* res_acc_1 = (__m256i*) &result[activePlayer == WHITE ? 0:S];
    auto* res_acc_2 = (__m256i*) &result[activePlayer == WHITE ? S:0];
    
    // control sequence to unshuffle the pack
    const auto shuffle_key = 0b11011000;  // 3 1 2 0
    
    // compute each chunk for accumulator 1
    // we store the result in res_acc_1 which is either the first or second helf of
    // the result array. This depends on the active player
    for(int i = 0; i < chunks; i++){

        const __m256i reduced = _mm256_packs_epi16(_mm256_max_epi16(acc1[2 * i + 0], ZERO_256),
                                                   _mm256_max_epi16(acc1[2 * i + 1], ZERO_256));
        res_acc_1[i] = _mm256_permute4x64_epi64(reduced, shuffle_key);
    }
    for(int i = 0; i < chunks; i++){
        const __m256i reduced = _mm256_packs_epi16(_mm256_max_epi16(acc2[2 * i + 0], ZERO_256),
                                                   _mm256_max_epi16(acc2[2 * i + 1], ZERO_256));
        res_acc_2[i] = _mm256_permute4x64_epi64(reduced, shuffle_key);
    }
}

template<int S>
inline void clipped_relu_32_to_8(int32_t* input, uint8_t* result){
    
    constexpr int chunks = S / 32;
    
    auto* inp = (__m256i*) input;
    auto* res = (__m256i*) result;
    
    // shuffling also from stockfish
    const __m256i control = _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0);
    
    // idea from stockfish
    // do 4 chunks of 8 at the same time. this will fit nicely since we reduce from 32 bit
    // to 8 bit
    for(int i = 0; i < chunks; i++){
        
        __m256i res16_1 = _mm256_packs_epi32(inp[i * 4 + 0],
                                             inp[i * 4 + 1]);
        __m256i res16_2 = _mm256_packs_epi32(inp[i * 4 + 2],
                                             inp[i * 4 + 3]);
        
        __m256i res8    = _mm256_packs_epi16(res16_1, res16_2);
        res[i] = _mm256_permutevar8x32_epi32(_mm256_max_epi8(res8, ZERO_256), control);
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
                                                bb::Square kingSquare){
    int idx  = index(pieceType, pieceColor, square, side, kingSquare);
    
    auto wgt = (avx_register_type*) (l1_weights[idx]);
    auto sum = (avx_register_type*) (history.back().summation[side]);
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

void nn::Evaluator::reset(Board* board) {
    resetAccumulator(board, WHITE);
    resetAccumulator(board, BLACK);
}

void nn::Evaluator::resetAccumulator(Board* board, bb::Color color){
    std::memcpy(history.back().summation[color], l1_bias, sizeof(int16_t) * HIDDEN_SIZE);
    
    Square kingSquare = bitscanForward(board->getPieceBB(color, KING));
    
    for (Color c : {WHITE, BLACK}) {
        for (PieceType pt : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
            U64 bb = board->getPieceBB(c, pt);
            while (bb) {
                Square s = bitscanForward(bb);
                
                setPieceOnSquareAccumulator<true>(color, pt, c, s,kingSquare);

                bb = lsbReset(bb);
            }
        }
    }
}


int nn::Evaluator::evaluate(bb::Color activePlayer, Board* board) {
    if (board != nullptr) {
        reset(board);
    }
    
    
    clipped_relu_accumulator<HIDDEN_SIZE, 0>(
        history.back().summation[WHITE],
        history.back().summation[BLACK],
        l1_activation, activePlayer);
    
//    // compute the dot product
    affine_epi8<HIDDEN_DSIZE,HIDDEN2_SIZE>(
                          l1_activation,
                          l2_weights[0],
                          l2_bias,
                          l2_activation);
   
    
    float res = 0;
    for(int i = 0; i < 8; i++){
        res += l2_activation[i] * l3_weights[i];
    }
    return res / 64.0f / 128.0f + l3_bias;
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
