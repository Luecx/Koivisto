
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
#ifndef KOIVISTO_EVAL_H
#define KOIVISTO_EVAL_H

#include "bitboard.h"

#include <cstdint>
#include <cstring>
#if defined(__ARM_NEON)
#include <arm_neon.h>
#else
#include <immintrin.h>
#endif
#include <vector>
#include <memory>

#define INPUT_SIZE     (bb::N_PIECE_TYPES * bb::N_SQUARES * 2 * 16)
#define HIDDEN_SIZE    (512)
#define HIDDEN_DSIZE   (HIDDEN_SIZE * 2)
#define OUTPUT_SIZE    (1)
 
#if defined(__AVX512F__)
#define BIT_ALIGNMENT  (512)
#elif defined(__AVX2__) || defined(__AVX__)
#define BIT_ALIGNMENT  (256)
#elif defined(__SSE2__) || defined(__ARM_NEON)
#define BIT_ALIGNMENT  (128)
#endif
#define STRIDE_16_BIT  (BIT_ALIGNMENT / 16)
#define BYTE_ALIGNMENT (BIT_ALIGNMENT / 8)
#define ALIGNMENT      (BYTE_ALIGNMENT)

class Board;

namespace nn {

struct Evaluator;

extern int16_t inputWeights [INPUT_SIZE][HIDDEN_SIZE];
extern int16_t hiddenWeights[OUTPUT_SIZE][HIDDEN_DSIZE];
extern int16_t inputBias    [HIDDEN_SIZE];
extern int32_t hiddenBias   [OUTPUT_SIZE];

// initialise and load the weights
void init();

// computes the index for a piece (piece type) and its color on the specified square
// also takes the view from with we view at the board as well as the king square of the view side
[[nodiscard]] int index(bb::PieceType pieceType, bb::Color pieceColor, bb::Square square,
                        bb::Color view, bb::Square kingSquare);

// the index is based on a king bucketing system. the relevant king bucket can be retrieved using
// the function below
[[nodiscard]] int kingSquareIndex(bb::Square kingSquare, bb::Color kingColor);

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

struct Evaluator {
    // summations
    std::vector<Accumulator> history;
    std::unique_ptr<AccumulatorTable> accumulator_table =
        std::make_unique<AccumulatorTable>(AccumulatorTable());

    Evaluator();
    
    Evaluator(const Evaluator& evaluator);
    
    Evaluator& operator=(const Evaluator& evaluator);
    
    void addNewAccumulation();
    
    void popAccumulation();
    
    void clearHistory();
    
    
    
    template<bool value>
    void setPieceOnSquare(bb::PieceType pieceType,
                          bb::Color pieceColor,
                          bb::Square square,
                          bb::Square wKingSquare,
                          bb::Square bKingSquare);
    
    template<bool value>
    void setPieceOnSquareAccumulator(bb::Color side,
                                     bb::PieceType pieceType,
                                     bb::Color pieceColor,
                                     bb::Square square,
                                     bb::Square kingSquare);

    void reset(Board* board);
    
    void resetAccumulator(Board* board, bb::Color color);
    
    [[nodiscard]] int evaluate(bb::Color activePlayer, Board* board = nullptr);
} __attribute__((aligned(128)));
}    // namespace nn

#endif    // KOIVISTO_EVAL_H
