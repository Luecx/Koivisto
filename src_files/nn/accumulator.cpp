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

#include "accumulator.h"
#include "../board.h"

void nn::AccumulatorTable::use(bb::Color view, Board* board, nn::Evaluator& evaluator) {
    const bb::Square king_sq   = bb::bitscanForward(board->getPieceBB(view, bb::KING));
    const bool       king_side = bb::fileIndex(king_sq) > 3;
    const int        ks_index  = kingSquareIndex(king_sq, view);
    
    // use a different entry if the king crossed the half but it would technically
    // still be within the same bucket
    const int entry_idx = king_side * 16 + ks_index;

    // get the entry
    AccumulatorTableEntry& entry = entries[view][entry_idx];
    
    // go through each piece and compute the difference.
    // and update the accumulator table entry inplace to only use a single memcpy
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
                addWeightsToAccumulator<true>(nn::index(pt, c, sq, view, king_sq), entry.accumulator.summation[view]);
                to_set = bb::lsbReset(to_set);
            }

            while (to_unset) {
                bb::Square sq = bb::bitscanForward(to_unset);
                addWeightsToAccumulator<false>(nn::index(pt, c, sq, view, king_sq), entry.accumulator.summation[view]);
                to_unset = bb::lsbReset(to_unset);
            }
            
            // store the piece data
            entry.piece_occ[c][pt] = board_bb;
        }
    }
    
    std::memcpy(evaluator.history[evaluator.history_index].summation[view], entry.accumulator.summation[view],sizeof(int16_t) * HIDDEN_SIZE);
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