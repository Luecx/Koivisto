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

#ifndef KOIVISTO_ACCUMULATOR_H
#define KOIVISTO_ACCUMULATOR_H

#include "defs.h"
#include "index.h"
#include "weights.h"

namespace nn{


// the accumulator which is used as the first hidden layer of the network.
// it is updated efficiently and contains the accumulated weights for whites and black pov.
struct Accumulator {
    alignas(ALIGNMENT) int16_t summation[bb::N_COLORS][HIDDEN_SIZE] {};
} __attribute__((aligned(2048)));

// the entry is used within the accumulator table (see below) to retrieve an accumulator which
// requires as little updates as possible from the position given to the evaluator.
// for computing the difference, we store the piece occupancy which resulted in that specific position
struct AccumulatorTableEntry {
    bb::U64     piece_occ[bb::N_COLORS][bb::N_PIECE_TYPES] {};
    Accumulator accumulator {};
} __attribute__((aligned(128)));

// this is used incase a king moves
// without the table we would need to fully recompute
// with the table, we are able to look into the table and get a potential similar position
// which requires fewer updates than a full reset
// The Table has 32 AccumulatorTableEntries. The 32 is chosen because we have 16 king buckets
// which might be mirrored --> 2 * 16 = 32. Depending on the resulting king bucket index and its
// half we choose the table entry. The AccumulatorTableEntry contains the accumulator which features
// white and blacks point of view. Depending on which king moved, we look and potentially copy that
// specific accumulator. Since the position from whites or blacks pov may be different, we need
// to also index based on the king color. This results in 50% of the accumulator content not being
// used but is the fastest solution.
struct AccumulatorTable {
    AccumulatorTableEntry entries[bb::N_COLORS][32] {};
    
    void use(bb::Color view, Board* board, Evaluator& evaluator);
    
    void reset();
} __attribute__((aligned(128)));



// adding weights to an accumulator
template<bool V>
inline void addWeightsToAccumulator(const int idx, int16_t* src, int16_t* target){
    const auto wgt = (avx_register_type_16*) (nn::inputWeights[idx]);
    const auto inp = (avx_register_type_16*) (src);
    const auto out = (avx_register_type_16*) (target);
    if constexpr (V) {
        for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT / 4; i++) {
            out[i * 4 + 0] = avx_add_epi16(inp[i * 4 + 0], wgt[i * 4 + 0]);
            out[i * 4 + 1] = avx_add_epi16(inp[i * 4 + 1], wgt[i * 4 + 1]);
            out[i * 4 + 2] = avx_add_epi16(inp[i * 4 + 2], wgt[i * 4 + 2]);
            out[i * 4 + 3] = avx_add_epi16(inp[i * 4 + 3], wgt[i * 4 + 3]);
        }
    } else {
        for (int i = 0; i < HIDDEN_SIZE / STRIDE_16_BIT / 4; i++) {
            out[i * 4 + 0] = avx_sub_epi16(inp[i * 4 + 0], wgt[i * 4 + 0]);
            out[i * 4 + 1] = avx_sub_epi16(inp[i * 4 + 1], wgt[i * 4 + 1]);
            out[i * 4 + 2] = avx_sub_epi16(inp[i * 4 + 2], wgt[i * 4 + 2]);
            out[i * 4 + 3] = avx_sub_epi16(inp[i * 4 + 3], wgt[i * 4 + 3]);
        }
    }
}

// adding weights to an accumulator
template<bool V>
inline void addWeightsToAccumulator(const int idx, int16_t* accumulator){
    addWeightsToAccumulator<V>(idx, accumulator, accumulator);
}

inline void setSetUnsetUnsetPiece(Accumulator* input, Accumulator* output, bb::Color side, Index set1, Index set2,
                               Index unset1, Index unset2) {
    const auto set1_wgt   = (inputWeights[set1(side)]);
    const auto set2_wgt   = (inputWeights[set2(side)]);
    const auto unset1_wgt = (inputWeights[unset1(side)]);
    const auto unset2_wgt = (inputWeights[unset2(side)]);

    const auto inp        = (input->summation[side]);
    const auto out        = (output->summation[side]);
    
    avx_register_type_16 regs[REG_COUNT] {};

    for (size_t c = 0; c < HIDDEN_SIZE / CHUNK_UNROLL_SIZE; c++) {
        auto wgt_set1   = (avx_register_type_16*) (&set1_wgt[c * CHUNK_UNROLL_SIZE]);
        auto wgt_set2   = (avx_register_type_16*) (&set2_wgt[c * CHUNK_UNROLL_SIZE]);
        auto wgt_unset1 = (avx_register_type_16*) (&unset1_wgt[c * CHUNK_UNROLL_SIZE]);
        auto wgt_unset2 = (avx_register_type_16*) (&unset2_wgt[c * CHUNK_UNROLL_SIZE]);
        auto acc_in     = (avx_register_type_16*) (&inp[c * CHUNK_UNROLL_SIZE]);
        auto acc_out    = (avx_register_type_16*) (&out[c * CHUNK_UNROLL_SIZE]);

        for (size_t i = 0; i < REG_COUNT; i++)
            regs[i] = avx_load_reg(&acc_in[i]);
        for (size_t i = 0; i < REG_COUNT; i++)
            regs[i] = avx_add_epi16(regs[i], wgt_set1[i]);
        for (size_t i = 0; i < REG_COUNT; i++)
            regs[i] = avx_add_epi16(regs[i], wgt_set2[i]);
        for (size_t i = 0; i < REG_COUNT; i++)
            regs[i] = avx_sub_epi16(regs[i], wgt_unset1[i]);
        for (size_t i = 0; i < REG_COUNT; i++)
            regs[i] = avx_sub_epi16(regs[i], wgt_unset2[i]);
        for (size_t i = 0; i < REG_COUNT; i++)
            avx_store_reg(&acc_out[i], regs[i]);
    }
}

inline void setSetUnsetUnsetPiece(Accumulator* input, Accumulator* output, Index set1, Index set2,
                               Index unset1, Index unset2) {
    setSetUnsetUnsetPiece(input, output, bb::WHITE, set1, set2, unset1, unset2);
    setSetUnsetUnsetPiece(input, output, bb::BLACK, set1, set2, unset1, unset2);
}

inline void setUnsetUnsetPiece(Accumulator* input, Accumulator* output, bb::Color side, Index set1,
                             Index unset1, Index unset2) {
    const auto set1_wgt  = (inputWeights[set1(side)]);
    const auto unset1_wgt  = (inputWeights[unset1(side)]);
    const auto unset2_wgt = (inputWeights[unset2(side)]);

    const auto inp = (input ->summation[side]);
    const auto out = (output->summation[side]);
    
    avx_register_type_16 regs[REG_COUNT]{};
    
    for (size_t c = 0; c < HIDDEN_SIZE / CHUNK_UNROLL_SIZE; c++) {
        auto wgt_set1   = (avx_register_type_16*) (&set1_wgt[c * CHUNK_UNROLL_SIZE]);
        auto wgt_unset1 = (avx_register_type_16*) (&unset1_wgt[c * CHUNK_UNROLL_SIZE]);
        auto wgt_unset2 = (avx_register_type_16*) (&unset2_wgt[c * CHUNK_UNROLL_SIZE]);
        auto acc_in     = (avx_register_type_16*) (&inp[c * CHUNK_UNROLL_SIZE]);
        auto acc_out    = (avx_register_type_16*) (&out[c * CHUNK_UNROLL_SIZE]);

        for (size_t i = 0; i < REG_COUNT; i++)
            regs[i] = avx_load_reg(&acc_in[i]);
        for (size_t i = 0; i < REG_COUNT; i++)
            regs[i] = avx_add_epi16(regs[i], wgt_set1[i]);
        for (size_t i = 0; i < REG_COUNT; i++)
            regs[i] = avx_sub_epi16(regs[i], wgt_unset1[i]);
        for (size_t i = 0; i < REG_COUNT; i++)
            regs[i] = avx_sub_epi16(regs[i], wgt_unset2[i]);
        for (size_t i = 0; i < REG_COUNT; i++)
            avx_store_reg(&acc_out[i], regs[i]);
    }
}

inline void setUnsetUnsetPiece(Accumulator* input, Accumulator* output, Index set1,
                             Index unset1, Index unset2) {
    setUnsetUnsetPiece(input, output, bb::WHITE, set1, unset1, unset2);
    setUnsetUnsetPiece(input, output, bb::BLACK, set1, unset1, unset2);
}

inline void setUnsetPiece(Accumulator* input, Accumulator* output, bb::Color side, Index set1,
                               Index unset1) {
    const auto set1_wgt  = (inputWeights[set1(side)]);
    const auto unset1_wgt  = (inputWeights[unset1(side)]);
    
    const auto inp = (input ->summation[side]);
    const auto out = (output->summation[side]);
    
    avx_register_type_16 regs[REG_COUNT]{};
    
    for (size_t c = 0; c < HIDDEN_SIZE / CHUNK_UNROLL_SIZE; c++) {
        auto wgt_set1   = (avx_register_type_16*) (&set1_wgt[c * CHUNK_UNROLL_SIZE]);
        auto wgt_unset1 = (avx_register_type_16*) (&unset1_wgt[c * CHUNK_UNROLL_SIZE]);
        auto acc_in     = (avx_register_type_16*) (&inp[c * CHUNK_UNROLL_SIZE]);
        auto acc_out    = (avx_register_type_16*) (&out[c * CHUNK_UNROLL_SIZE]);

        for (size_t i = 0; i < REG_COUNT; i++)
            regs[i] = avx_load_reg(&acc_in[i]);
        for (size_t i = 0; i < REG_COUNT; i++)
            regs[i] = avx_add_epi16(regs[i], wgt_set1[i]);
        for (size_t i = 0; i < REG_COUNT; i++)
            regs[i] = avx_sub_epi16(regs[i], wgt_unset1[i]);
        for (size_t i = 0; i < REG_COUNT; i++)
            avx_store_reg(&acc_out[i], regs[i]);
    }
}

inline void setUnsetPiece(Accumulator* input, Accumulator* output, Index set1,
                               Index unset1) {
    setUnsetPiece(input, output, bb::WHITE, set1, unset1);
    setUnsetPiece(input, output, bb::BLACK, set1, unset1);
}


}


#endif    // KOIVISTO_ACCUMULATOR_H
