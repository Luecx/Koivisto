
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

#include "defs.h"
#include "index.h"
#include "accumulator.h"

class Board;

namespace nn {

struct Evaluator;

struct Evaluator {
    // summations
    std::vector<Accumulator> history;
    std::unique_ptr<AccumulatorTable> accumulator_table =
        std::make_unique<AccumulatorTable>(AccumulatorTable());
    
    // pointer to which accumulator in the history is the current accumulator so we dont need
    // to use popback but instead increment /decrement a counter
    uint32_t history_index = 0;
    
    // true if the accumulator is initialised, otherwise false and we need to use the previous
    // depth accumulator as an input when updating accumulator
    bool accumulator_is_initialised[bb::N_COLORS] = {false, false};

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
