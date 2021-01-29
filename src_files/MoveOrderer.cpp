
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
{{0.188024, 0.54811}, {0.881031, 0.449353}, {0.722395, 1.12936}, {1.06505, 1.29812}, {3.89109, 0.984601}, {0, 0}, },
{{0.176432, 0.293945}, {0.705936, 0.633118}, {0.452813, 1.24464}, {1.65848, 0.947188}, {4.2965, 0.682878}, {0, 0}, },
{{0.329544, 0.96614}, {0.241385, 0.532156}, {0.459449, 1.30235}, {1.22462, 0.695323}, {7.11918, 0.959011}, {0, 0}, },
{{0.366328, 0.534537}, {0.210768, 0.356265}, {0.631569, 0.760728}, {0.636569, 0.685637}, {4.04181, 1.3699}, {0, 0}, },
{{0.549921, 0.402432}, {1.16079, 0.710888}, {1.83148, 0.73315}, {0.846314, 1.30195}, {1.65167, 0.854603}, {0, 0}, },
{{0.35358, 0.459513}, {1.29793, 1.20226}, {1.33366, 0.642995}, {2.77478, 1.62673}, {0.833268, 0.947274}, {0, 0}, },
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
