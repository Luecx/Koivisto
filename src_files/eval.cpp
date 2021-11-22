
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

alignas(ALIGNMENT) int16_t nn::l1Weights[INPUT_SIZE ][L1_SIZE];
alignas(ALIGNMENT) int16_t nn::l2Weights[L2_SIZE    ][L1_SIZE*2];
alignas(ALIGNMENT) int32_t nn::l3Weights[OUTPUT_SIZE][L2_SIZE];
alignas(ALIGNMENT) int16_t nn::l1Bias   [L1_SIZE];
alignas(ALIGNMENT) int32_t nn::l2Bias   [L2_SIZE];
alignas(ALIGNMENT) int64_t nn::l3Bias   [OUTPUT_SIZE];

#define L1_SCALAR  (8)
#define L2_SCALAR  (128)
#define L3_SCALAR  (512)

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
    std::memcpy(l1Weights, &gEvalData[memoryIndex],   INPUT_SIZE * L1_SIZE * sizeof(int16_t));
    memoryIndex += INPUT_SIZE * L1_SIZE * sizeof(int16_t);
    std::memcpy(l1Bias, &gEvalData[memoryIndex], L1_SIZE * sizeof(int16_t));
    memoryIndex += L1_SIZE * sizeof(int16_t);
    
    std::memcpy(l2Weights, &gEvalData[memoryIndex],  L1_SIZE * 2 * L2_SIZE * sizeof(int16_t));
    memoryIndex += L1_SIZE * 2 * L2_SIZE * sizeof(int16_t);
    std::memcpy(l2Bias, &gEvalData[memoryIndex],                   L2_SIZE * sizeof(int32_t));
    memoryIndex +=               L2_SIZE * sizeof(int32_t);
    
    std::memcpy(l3Weights, &gEvalData[memoryIndex],  OUTPUT_SIZE * L2_SIZE * sizeof(int32_t));
    memoryIndex += OUTPUT_SIZE * L2_SIZE * sizeof(int32_t);
    std::memcpy(l3Bias, &gEvalData[memoryIndex],               OUTPUT_SIZE * sizeof(int64_t));
    memoryIndex +=               OUTPUT_SIZE * sizeof(int64_t);
}
int nn::Evaluator::index(bb::PieceType pieceType,
                         bb::Color pieceColor,
                         bb::Square square,
                         bb::Color view,
                         bb::Square kingSquare) {

    constexpr int pieceTypeFactor  = 64;
    constexpr int pieceColorFactor = 64 * 6;

    Square  relativeSquare   = view == WHITE ? square : mirrorSquareVertical(square);
    
    if (kingSquare & 4) // alternative: (fileIndex(kingSquare) > 3)
        relativeSquare = mirrorSquareHorizontal(relativeSquare);
    
    return relativeSquare
           + pieceType * pieceTypeFactor
           + (pieceColor == view) * pieceColorFactor;
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

        auto wgt = (avx_register_type*) (l1Weights[idx[c]]);
        auto sum = (avx_register_type*) (history.back().summation[c]);
        if constexpr (value) {
            for (int i = 0; i < L1_SIZE / STRIDE_16_BIT; i++) {
                sum[i] = avx_add_epi16(sum[i], wgt[i]);
            }
        } else {
            for (int i = 0; i < L1_SIZE / STRIDE_16_BIT; i++) {
                sum[i] = avx_sub_epi16(sum[i], wgt[i]);
            }
        }
    }
}

void nn::Evaluator::reset(Board* board) {
    std::memcpy(history.back().summation[WHITE], l1Bias, sizeof(int16_t) * L1_SIZE);
    std::memcpy(history.back().summation[BLACK], l1Bias, sizeof(int16_t) * L1_SIZE);
    
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

int nn::Evaluator::evaluate(bb::Color activePlayer, Board* board) {
    if (board != nullptr) {
        reset(board);
    }
    
    // concat based on stm
    std::memcpy( l1_activation         , history.back().summation[ activePlayer], sizeof(uint16_t) * L1_SIZE);
    std::memcpy(&l1_activation[L1_SIZE], history.back().summation[!activePlayer], sizeof(uint16_t) * L1_SIZE);

    constexpr avx_register_type reluBias {};
    avx_register_type*          act = (avx_register_type*) (l1_activation);

    // apply relu to the summation first
    for (int i = 0; i < L1_SIZE * 2 / STRIDE_16_BIT; i++) {
        act[i] = avx_max_epi16(act[i], reluBias);
    }
    
//    float accum = 0;
//    for(int i = 0; i < L1_SIZE * 2; i++){
//        accum += l1_activation[i] * l2Weights[31][i];
//        std::cout << i << "  " << l1_activation[i] << "  " << l2Weights[31][i] << "   " << accum << std::endl;
//    }
    
    // do L2
    for (int o = 0; o < L2_SIZE; o++) {

        auto              wgt = (avx_register_type*) (l2Weights[o]);
        avx_register_type res {};
        for (int i = 0; i < L1_SIZE * 2 / STRIDE_16_BIT; i++) {
            res = avx_add_epi32(res, avx_madd_epi16(act[i], wgt[i]));
        }
        // credit to Connor
        int32_t sums = sumRegisterEpi32(res);
        l2_activation[o]    = std::max(0,sums + l2Bias[o]);
    }
    
    // do L3
    int64_t outp = l3Bias[0];
    for(int i = 0; i < L2_SIZE; i++){
        outp += l2_activation[i] * l3Weights[0][i];
    }
    
    
//    for(int i = 0; i < L1_SIZE*2; i++){
//        std::cout << l1_activation[i] / (float) (L1_SCALAR) << "  ";
//    }std::cout << std::endl;
//
//    for(int i = 0; i < L2_SIZE; i++){
//        std::cout << l2_activation[i] / (float) (L1_SCALAR * L2_SCALAR)<< "  ";
//    }std::cout << std::endl;
    
    return outp / L1_SCALAR / L2_SCALAR / L3_SCALAR;
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