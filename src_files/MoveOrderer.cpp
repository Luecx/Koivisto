
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                           by. Kim Kahre, Finn Eggers and Eugenio Bruno                           *
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
#include "MoveOrderer.h"

#include "History.h"

using namespace move;

MoveOrderer::MoveOrderer() {}

MoveOrderer::~MoveOrderer() {}

void MoveOrderer::setMovesPVSearch(move::MoveList* p_moves, move::Move hashMove, SearchData* sd, Board* board,
                                   Depth ply) {

    this->moves   = p_moves;
    this->counter = 0;
    this->skip    = false;

    for (int i = 0; i < moves->getSize(); i++) {
        move::Move m = moves->getMove(i);

        if (sameMove(m, hashMove)) {
            moves->scoreMove(i, 1e6);
        } else if (isCapture(m)) {
            // add mvv lva score here
            Score     SEE    = board->staticExchangeEvaluation(m);
            MoveScore mvvLVA = 100 * (getCapturedPiece(m) % 6) - 10 * (getMovingPiece(m) % 6)
                               + (getSquareTo(board->getPreviousMove()) == getSquareTo(m));
            if (SEE >= 0) {
                if (mvvLVA == 0) {
                    moves->scoreMove(i, 50000 + mvvLVA);
                } else {
                    moves->scoreMove(i, 100000 + mvvLVA);
                }
            } else {
                MoveScore ms = 8;
                moves->scoreMove(i, ms);
            }
        } else if (isPromotion(m)) {
            MoveScore mvvLVA = (getCapturedPiece(m) % 6) - (getMovingPiece(m) % 6);
            moves->scoreMove(i, 40000 + mvvLVA + promotionPiece(m));
        } else if (sd->isKiller(m, ply, board->getActivePlayer())) {
            moves->scoreMove(i, 30000);
        } else {

            MoveScore ms = 8;
            ms += sd->getHistoryMoveScore(m, board->getActivePlayer());
            if (ply > 1)
                ms += sd->getCounterMoveHistoryScore(board->getPreviousMove(), m);
            moves->scoreMove(i, ms);
        }
    }
}

void MoveOrderer::setMovesQSearch(move::MoveList* p_moves, Board* b) {
    this->moves   = p_moves;
    this->counter = 0;
    this->skip    = false;

    for (int i = 0; i < moves->getSize(); i++) {
        move::Move m = moves->getMove(i);

        MoveScore mvvLVA = 100 * (getCapturedPiece(m) % 6) - 10 * (getMovingPiece(m) % 6)
                           + (getSquareTo(b->getPreviousMove()) == getSquareTo(m));
        moves->scoreMove(i, 240 + mvvLVA);
    }
}

bool MoveOrderer::hasNext() { return counter < moves->getSize(); }

move::Move MoveOrderer::next() {
    if (skip) {
        moves->scoreMove(counter, 0);
        return moves->getMove(counter++);
    }
    int bestIndex = counter;
    // Move best = moves->getMove(0);
    for (int i = counter + 1; i < moves->getSize(); i++) {
        if (moves->getScore(i) > moves->getScore(bestIndex)) {
            bestIndex = i;
        }
    }
    moves->scoreMove(bestIndex, 0);
    moves->swap(bestIndex, counter);
    return moves->getMove(counter++);
}
