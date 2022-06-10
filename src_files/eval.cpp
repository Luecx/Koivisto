
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
#include "board.h"
#include "uciassert.h"

#define INCBIN_STYLE INCBIN_STYLE_CAMEL

#include "incbin/incbin.h"

// clang-format off
alignas(ALIGNMENT) int16_t nn::inputWeights [INPUT_SIZE ][HIDDEN_SIZE ];
alignas(ALIGNMENT) int16_t nn::hiddenWeights[OUTPUT_SIZE][HIDDEN_DSIZE];
alignas(ALIGNMENT) int16_t nn::inputBias    [HIDDEN_SIZE];
alignas(ALIGNMENT) int32_t nn::hiddenBias   [OUTPUT_SIZE];
// clang-format on

#define INPUT_WEIGHT_MULTIPLIER  (32)
#define HIDDEN_WEIGHT_MULTIPLIER (128)

#if defined(__AVX512F__)
using avx_register_type_16 = __m512i;
using avx_register_type_32 = __m512i;
#define avx_madd_epi16(a, b) (_mm512_madd_epi16(a, b))
#define avx_add_epi32(a, b)  (_mm512_add_epi32(a, b))
#define avx_sub_epi32(a, b)  (_mm512_sub_epi32(a, b))
#define avx_add_epi16(a, b)  (_mm512_add_epi16(a, b))
#define avx_sub_epi16(a, b)  (_mm512_sub_epi16(a, b))
#define avx_max_epi16(a, b)  (_mm512_max_epi16(a, b))
#elif defined(__AVX2__) || defined(__AVX__)
using avx_register_type_16 = __m256i;
using avx_register_type_32 = __m256i;
#define avx_madd_epi16(a, b) (_mm256_madd_epi16(a, b))
#define avx_add_epi32(a, b)  (_mm256_add_epi32(a, b))
#define avx_sub_epi32(a, b)  (_mm256_sub_epi32(a, b))
#define avx_add_epi16(a, b)  (_mm256_add_epi16(a, b))
#define avx_sub_epi16(a, b)  (_mm256_sub_epi16(a, b))
#define avx_max_epi16(a, b)  (_mm256_max_epi16(a, b))
#elif defined(__SSE2__)
using avx_register_type_16 = __m128i;
using avx_register_type_32 = __m128i;
#define avx_madd_epi16(a, b) (_mm_madd_epi16(a, b))
#define avx_add_epi32(a, b)  (_mm_add_epi32(a, b))
#define avx_sub_epi32(a, b)  (_mm_sub_epi32(a, b))
#define avx_add_epi16(a, b)  (_mm_add_epi16(a, b))
#define avx_sub_epi16(a, b)  (_mm_sub_epi16(a, b))
#define avx_max_epi16(a, b)  (_mm_max_epi16(a, b))
#elif defined(__ARM_NEON)
using avx_register_type_16 = int16x8_t;
using avx_register_type_32 = int32x4_t;
#define avx_madd_epi16(a, b)                                                                         \
    (vpaddq_s32(vmull_s16(vget_low_s16(a), vget_low_s16(b)), vmull_high_s16(a, b)))
#define avx_add_epi32(a, b) (vaddq_s32(a, b))
#define avx_sub_epi32(a, b) (vsubq_s32(a, b))
#define avx_add_epi16(a, b) (vaddq_s16(a, b))
#define avx_sub_epi16(a, b) (vsubq_s16(a, b))
#define avx_max_epi16(a, b) (vmaxq_s16(a, b))
#endif

INCBIN(Eval, EVALFILE);

inline int32_t sumRegisterEpi32(avx_register_type_32& reg) {
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
    vsum         = _mm_add_epi32(vsum, _mm_srli_si128(vsum, 4));
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

int nn::index(bb::PieceType pieceType, bb::Color pieceColor, bb::Square square, bb::Color view,
              bb::Square kingSquare) {
    constexpr int pieceTypeFactor  = 64;
    constexpr int pieceColorFactor = 64 * 6;
    constexpr int kingSquareFactor = 64 * 6 * 2;

    const bool    kingSide         = bb::fileIndex(kingSquare) > 3;
    const int     ksIndex          = kingSquareIndex(kingSquare, view);
    bb::Square    relativeSquare   = view == bb::WHITE ? square : bb::mirrorVertically(square);

    if (kingSide) {
        relativeSquare = bb::mirrorHorizontally(relativeSquare);
    }

    // clang-format off
    return relativeSquare
           + pieceType * pieceTypeFactor
           + (pieceColor == view) * pieceColorFactor
           + ksIndex * kingSquareFactor;
    // clang-format on
}

int nn::kingSquareIndex(bb::Square relativeKingSquare, bb::Color kingColor) {
    // return zero if there is no king on the board yet ->
    // requires manual reset
    if (relativeKingSquare > 63)
        return 0;
    // clang-format off
    constexpr int indices[bb::N_SQUARES] {
        0,  1,  2,  3,  3,  2,  1,  0,
        4,  5,  6,  7,  7,  6,  5,  4,
        8,  9,  10, 11, 11, 10, 9,  8,
        8,  9,  10, 11, 11, 10, 9,  8,
        12, 12, 13, 13, 13, 13, 12, 12,
        12, 12, 13, 13, 13, 13, 12, 12,
        14, 14, 15, 15, 15, 15, 14, 14,
        14, 14, 15, 15, 15, 15, 14, 14,
    };
    // clang-format on
    if (kingColor == bb::BLACK) {
        relativeKingSquare = bb::mirrorVertically(relativeKingSquare);
    }
    return indices[relativeKingSquare];
}

void nn::AccumulatorTable::put(bb::Color view, Board* board, nn::Accumulator& accumulator) {
    const bb::Square king_sq   = bb::bitscanForward(board->getPieceBB(view, bb::KING));
    const bool       king_side = bb::fileIndex(king_sq) > 3;
    const int        ks_index  = kingSquareIndex(king_sq, view);

    // use a different entry if the king crossed the half but it would technically
    // still be within the same bucket
    const int entry_idx = king_side * 16 + ks_index;

    // get the entry
    AccumulatorTableEntry& entry = entries[view][entry_idx];

    // store the accumulator data
    std::memcpy(entry.accumulator.summation[view], accumulator.summation[view],
                sizeof(int16_t) * HIDDEN_SIZE);

    // store the piece data
    for (bb::Color c : {bb::WHITE, bb::BLACK}) {
        for (bb::PieceType pt : {bb::PAWN, bb::KNIGHT, bb::BISHOP, bb::ROOK, bb::QUEEN, bb::KING}) {
            bb::U64 bb             = board->getPieceBB(c, pt);
            entry.piece_occ[c][pt] = bb;
        }
    }
}

void nn::AccumulatorTable::use(bb::Color view, Board* board, nn::Evaluator& evaluator) {
    const bb::Square king_sq   = bb::bitscanForward(board->getPieceBB(view, bb::KING));
    const bool       king_side = bb::fileIndex(king_sq) > 3;
    const int        ks_index  = kingSquareIndex(king_sq, view);

    // use a different entry if the king crossed the half but it would technically
    // still be within the same bucket
    const int entry_idx = king_side * 16 + ks_index;

    // get the entry
    AccumulatorTableEntry& entry = entries[view][entry_idx];

    // first retrieve the accumulator from the table and put that into the evaluator
    std::memcpy(evaluator.history.back().summation[view], entry.accumulator.summation[view],
                sizeof(int16_t) * HIDDEN_SIZE);

    // go through each piece and compute the difference.
    for (bb::Color c : {bb::WHITE, bb::BLACK}) {
        for (bb::PieceType pt : {bb::PAWN, bb::KNIGHT, bb::BISHOP, bb::ROOK, bb::QUEEN, bb::KING}) {

            // get the piece bb from the board and the stored entry and compute the
            // squares which need to be set/unset based on that
            bb::U64 board_bb = board->getPieceBB(c, pt);
            bb::U64 entry_bb = entry.piece_occ[c][pt];

            bb::U64 to_set   = board_bb & ~entry_bb;
            bb::U64 to_unset = entry_bb & ~board_bb;
            // as a reference: to_keep = board_bb ^ entry_bb

            // go through both sets and call the evaluator to update the accumulator
            while (to_set) {
                bb::Square sq = bb::bitscanForward(to_set);
                evaluator.setPieceOnSquareAccumulator<true>(view, pt, c, sq, king_sq);
                to_set = bb::lsbReset(to_set);
            }

            while (to_unset) {
                bb::Square sq = bb::bitscanForward(to_unset);
                evaluator.setPieceOnSquareAccumulator<false>(view, pt, c, sq, king_sq);
                to_unset = bb::lsbReset(to_unset);
            }
        }
    }
    // this set has most likely been done on a reset. its handy to just put the new state
    // into the table
    put(view, board, evaluator.history.back());
}

void nn::AccumulatorTable::reset() {
    // clearing will erase all information from the table and reset every entry to an empty board.
    // This will require the accumulators to be initialised to the bias
    for (bb::Color c : {bb::WHITE, bb::BLACK}) {
        for (int s = 0; s < 32; s++) {
            std::memcpy(entries[c][s].accumulator.summation[c], inputBias,
                        sizeof(int16_t) * HIDDEN_SIZE);
        }
    }
}

template<bool value>
void nn::Evaluator::setPieceOnSquare(bb::PieceType pieceType, bb::Color pieceColor, bb::Square square,
                                     bb::Square wKingSquare, bb::Square bKingSquare) {
    setPieceOnSquareAccumulator<value>(bb::WHITE, pieceType, pieceColor, square, wKingSquare);
    setPieceOnSquareAccumulator<value>(bb::BLACK, pieceType, pieceColor, square, bKingSquare);
}

template<bool value>
void nn::Evaluator::setPieceOnSquareAccumulator(bb::Color side, bb::PieceType pieceType,
                                                bb::Color pieceColor, bb::Square square,
                                                bb::Square kingSquare) {
    const int  idx = index(pieceType, pieceColor, square, side, kingSquare);

    const auto wgt = (avx_register_type_16*) (inputWeights[idx]);
    const auto sum = (avx_register_type_16*) (history.back().summation[side]);
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

void nn::Evaluator::reset(Board* board) {
    history.resize(1);
    resetAccumulator(board, bb::WHITE);
    resetAccumulator(board, bb::BLACK);
}

void nn::Evaluator::resetAccumulator(Board* board, bb::Color color) {
    accumulator_table->use(color, board, *this);
}

int nn::Evaluator::evaluate(bb::Color activePlayer, Board* board) {
    if (board != nullptr) {
        reset(board);
    }
    constexpr avx_register_type_16 reluBias {};

    const auto acc_act = (avx_register_type_16*) history.back().summation[activePlayer];
    const auto acc_nac = (avx_register_type_16*) history.back().summation[!activePlayer];

    // compute the dot product
    avx_register_type_32 res {};
    const auto           wgt = (avx_register_type_16*) (hiddenWeights[0]);
    for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
        res = avx_add_epi32(res, avx_madd_epi16(avx_max_epi16(acc_act[i], reluBias), wgt[i]));
    }
    for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT; i++) {
        res = avx_add_epi32(res, avx_madd_epi16(avx_max_epi16(acc_nac[i], reluBias),
                                                wgt[i + HIDDEN_SIZE / STRIDE_16_BIT]));
    }

    const auto outp = sumRegisterEpi32(res) + hiddenBias[0];
    return outp / INPUT_WEIGHT_MULTIPLIER / HIDDEN_WEIGHT_MULTIPLIER;
}

nn::Evaluator::Evaluator() {
    this->history.push_back(Accumulator {});
    this->accumulator_table->reset();
}

nn::Evaluator::Evaluator(const nn::Evaluator& evaluator) {
    history = evaluator.history;
}
nn::Evaluator& nn::Evaluator::operator=(const nn::Evaluator& evaluator) {
    this->history = evaluator.history;
    return *this;
}

void nn::Evaluator::addNewAccumulation() { this->history.emplace_back(this->history.back()); }

void nn::Evaluator::popAccumulation() { this->history.pop_back(); }

void nn::Evaluator::clearHistory() {
    this->history.clear();
    this->history.push_back(Accumulator {});
}

template void nn::Evaluator::setPieceOnSquare<true>(bb::PieceType pieceType, bb::Color pieceColor,
                                                    bb::Square square, bb::Square wKingSquare,
                                                    bb::Square bKingSquare);

template void nn::Evaluator::setPieceOnSquare<false>(bb::PieceType pieceType, bb::Color pieceColor,
                                                     bb::Square square, bb::Square wKingSquare,
                                                     bb::Square bKingSquare);
