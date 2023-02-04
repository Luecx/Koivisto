
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

#include "../board.h"
#include "../uciassert.h"


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
    
    if(!accumulator_is_initialised[side]){
        addWeightsToAccumulator<value>(idx, history[history_index-1].summation[side], history[history_index].summation[side]);
        accumulator_is_initialised[side] = true;
    }
    else{
        addWeightsToAccumulator<value>(idx, history[history_index].summation[side]);
    }
}

void nn::Evaluator::reset(Board* board) {
    history.resize(1);
    this->history_index = 0;
    resetAccumulator(board, bb::WHITE);
    resetAccumulator(board, bb::BLACK);
}

void nn::Evaluator::resetAccumulator(Board* board, bb::Color color) {
    accumulator_table->use(color, board, *this);
    accumulator_is_initialised[color] = true;
}

int nn::Evaluator::evaluate(bb::Color activePlayer, Board* board) {
    if (board != nullptr) {
        reset(board);
    }
    constexpr avx_register_type_16 reluBias {};

    const auto acc_act = (avx_register_type_16*) history[history_index].summation[activePlayer];
    const auto acc_nac = (avx_register_type_16*) history[history_index].summation[!activePlayer];

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
    this->history_index = 0;
    this->accumulator_table->reset();
}

nn::Evaluator::Evaluator(const nn::Evaluator& evaluator) {
    history = evaluator.history;
    history_index = evaluator.history_index;
}
nn::Evaluator& nn::Evaluator::operator=(const nn::Evaluator& evaluator) {
    this->history = evaluator.history;
    this->history_index = evaluator.history_index;
    return *this;
}

void nn::Evaluator::addNewAccumulation() {
    history_index ++;
    // enlarge history if required
    if(history_index >= this->history.size()){
        this->history.resize(history_index + 1);
    }
    accumulator_is_initialised[bb::WHITE] = false;
    accumulator_is_initialised[bb::BLACK] = false;
}

void nn::Evaluator::popAccumulation() {
    history_index --;
    accumulator_is_initialised[bb::WHITE] = true;
    accumulator_is_initialised[bb::BLACK] = true;
}

void nn::Evaluator::clearHistory() {
    this->history.clear();
    this->history.push_back(Accumulator {});
    this->history_index = 0;
}

using namespace bb;
using namespace move;

void nn::Evaluator::processMove(Board* board, move::Move m) {
    const Square    sqFrom  = getSquareFrom(m);
    const Square    sqTo    = getSquareTo(m);
    const PieceType ptFrom  = getMovingPieceType(m);
    const MoveType  mType   = getType(m);
    const Color     color   = getMovingPieceColor(m);
    const int       factor  = color == WHITE ? 1 : -1;
    
    Square          wKingSq = bitscanForward(board->getPieceBB<WHITE, KING>());
    Square          bKingSq = bitscanForward(board->getPieceBB<BLACK, KING>());
    
    PieceType       ptTo    = ptFrom;
    PieceType       ptCap   = getCapturedPieceType(m);

    this->addNewAccumulation();
    
    if (ptFrom == KING) {
        // we handle this case seperately so we return after this finished.
        bool requires_accumulator_reset = false;
        if(     nn::kingSquareIndex(sqTo, color) !=
                nn::kingSquareIndex(sqFrom, color)
            ||  fileIndex(sqFrom) + fileIndex(sqTo) == 7){
            requires_accumulator_reset = true;
        }
        // normal handling if no reset is required
        if(!requires_accumulator_reset){
            if (isCapture(m)) {
                nn::setUnsetUnsetPiece(
                    &history[history_index - 1],
                    &history[history_index],
                    Index{ptTo  ,  color, sqTo  , wKingSq, bKingSq},
                    Index{ptFrom,  color, sqFrom, wKingSq, bKingSq},
                    Index{ptCap , !color, sqTo  , wKingSq, bKingSq});
            }else if (isCastle(m)){
                Square rookSquare = sqFrom + (mType == QUEEN_CASTLE ? -4 : 3);
                Square rookTarget = sqTo + (mType == QUEEN_CASTLE ? 1 : -1);
                nn::setSetUnsetUnsetPiece(
                    &history[history_index - 1],
                    &history[history_index],
                    Index {ptTo  , color, sqTo      , wKingSq, bKingSq},
                    Index {ROOK  , color, rookTarget, wKingSq, bKingSq},
                    Index {ptFrom, color, sqFrom    , wKingSq, bKingSq},
                    Index {ROOK  , color, rookSquare, wKingSq, bKingSq});
            }else{
                nn::setUnsetPiece(&history[history_index - 1],
                                  &history[history_index],
                                  Index {ptTo, color, sqTo, wKingSq, bKingSq},
                                  Index {ptFrom, color, sqFrom, wKingSq, bKingSq});
            }
        }else{
            if (isCapture(m)) {
                nn::setUnsetUnsetPiece(
                    &history[history_index - 1],
                    &history[history_index],
                    !color,
                    Index{ptTo  ,  color, sqTo  , wKingSq, bKingSq},
                    Index{ptFrom,  color, sqFrom, wKingSq, bKingSq},
                    Index{ptCap , !color, sqTo  , wKingSq, bKingSq});
            }else if(isCastle(m)){
                Square rookSquare = sqFrom + (mType == QUEEN_CASTLE ? -4 : 3);
                Square rookTarget = sqTo + (mType == QUEEN_CASTLE ? 1 : -1);
                nn::setSetUnsetUnsetPiece(
                    &history[history_index - 1],
                    &history[history_index],
                    !color,
                    Index {ptTo  , color, sqTo      , wKingSq, bKingSq},
                    Index {ROOK  , color, rookTarget, wKingSq, bKingSq},
                    Index {ptFrom, color, sqFrom    , wKingSq, bKingSq},
                    Index {ROOK  , color, rookSquare, wKingSq, bKingSq});
            }else{
                nn::setUnsetPiece(&history[history_index - 1],
                                  &history[history_index],
                                  !color,
                                  Index {ptTo, color, sqTo    , wKingSq, bKingSq},
                                  Index {ptFrom, color, sqFrom, wKingSq, bKingSq});
            }
            resetAccumulator(board, color);
        }
    }
    else{
        if(isPromotion(m)){
            ptTo = getPromotionPieceType(m);
        }
        if (isCapture(m)) {
            if(isEnPassant(m)){
                Square ep_cap_sq = sqTo - 8 * factor;
                nn::setUnsetUnsetPiece(&history[history_index - 1],
                                       &history[history_index],
                                       Index {ptTo  ,  color, sqTo     , wKingSq, bKingSq},
                                       Index {ptFrom,  color, sqFrom   , wKingSq, bKingSq},
                                       Index {PAWN  , !color, ep_cap_sq, wKingSq, bKingSq});
                
            }else{
                nn::setUnsetUnsetPiece(&history[history_index - 1],
                                       &history[history_index],
                                       Index {ptTo  , color, sqTo  , wKingSq, bKingSq},
                                       Index {ptFrom, color, sqFrom, wKingSq, bKingSq},
                                       Index {ptCap ,!color, sqTo  , wKingSq, bKingSq});
            }
        } else {
            nn::setUnsetPiece(&history[history_index - 1],
                              &history[history_index],
                              Index {ptTo  , color, sqTo  , wKingSq, bKingSq},
                              Index {ptFrom, color, sqFrom, wKingSq, bKingSq});
        }
    }
}

template void nn::Evaluator::setPieceOnSquare<true>(bb::PieceType pieceType, bb::Color pieceColor,
                                                    bb::Square square, bb::Square wKingSquare,
                                                    bb::Square bKingSquare);

template void nn::Evaluator::setPieceOnSquare<false>(bb::PieceType pieceType, bb::Color pieceColor,
                                                     bb::Square square, bb::Square wKingSquare,
                                                     bb::Square bKingSquare);

template void nn::Evaluator::setPieceOnSquareAccumulator<false>(bb::Color side, bb::PieceType pieceType, bb::Color pieceColor, bb::Square square, bb::Square kingSquare);
template void nn::Evaluator::setPieceOnSquareAccumulator<true>(bb::Color side, bb::PieceType pieceType, bb::Color pieceColor, bb::Square square, bb::Square kingSquare);
