
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

constexpr float qsScores [6][6][2] = {
{{0.174014, 0.475132}, {0.768855, 0.341403}, {0.803026, 1.1157}, {1.22278, 1.20512}, {3.69442, 0.886295}, {0, 0}, },
{{0.170543, 0.280273}, {0.60715, 0.528267}, {0.492412, 1.11591}, {1.44572, 0.887692}, {4.51259, 0.766263}, {0, 0}, },
{{0.3146, 0.986342}, {0.257381, 0.501457}, {0.469711, 1.2027}, {1.26536, 0.691528}, {5.10933, 1.12951}, {0, 0}, },
{{0.384586, 0.516599}, {0.264181, 0.371305}, {0.684162, 0.793687}, {0.662365, 0.763265}, {4.39545, 1.47403}, {0, 0}, },
{{0.512669, 0.409176}, {1.3532, 0.586555}, {1.69863, 0.90625}, {0.83362, 1.28186}, {1.62636, 0.854795}, {0, 0}, },
{{0.379562, 0.536443}, {1.36465, 1.17591}, {1.38639, 0.703393}, {3.3883, 1.69439}, {0.885131, 0.891185}, {0, 0}, },
};


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
                    moves->scoreMove(i, 50000 + mvvLVA + sd->getHistories(m, board->getActivePlayer(), board->getPreviousMove()));
                } else {
                    moves->scoreMove(i, 100000 + mvvLVA + sd->getHistories(m, board->getActivePlayer(), board->getPreviousMove()));
                }
            } else {
                moves->scoreMove(i, 10000 + sd->getHistories(m, board->getActivePlayer(), board->getPreviousMove()));
            }
        } else if (isPromotion(m)) {
            MoveScore mvvLVA = (getCapturedPiece(m) % 6) - (getMovingPiece(m) % 6);
            moves->scoreMove(i, 40000 + mvvLVA + promotionPiece(m));
        } else if (sd->isKiller(m, ply, board->getActivePlayer())) {
            moves->scoreMove(i, 30000 + sd->isKiller(m, ply, board->getActivePlayer()));
        } else {
            moves->scoreMove(i, 20000 + sd->getHistories(m, board->getActivePlayer(), board->getPreviousMove()));
        }
    }
}

void MoveOrderer::setMovesQSearch(move::MoveList* p_moves, Board* b) {
    this->moves   = p_moves;
    this->counter = 0;
    this->skip    = false;

    for (int i = 0; i < moves->getSize(); i++) {
        move::Move m = moves->getMove(i);
        moves->scoreMove(i, 1000*qsScores[getMovingPiece(m)][getCapturedPiece(m)][(getSquareTo(b->getPreviousMove()) == getSquareTo(m))]);
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
