
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
#include "MoveOrderer.h"

#include "History.h"

using namespace move;

MoveOrderer::MoveOrderer(move::MoveList* p_moves) {
    this->moves     = p_moves;
    this->counter   = 0;
    this->skip      = false;
}

MoveOrderer::~MoveOrderer() {}

bool MoveOrderer::hasNext() { return counter < moves->getSize(); }

move::Move MoveOrderer::next() {
    if (skip) {    
        for (int i = counter; i < moves->getSize(); i++) {
            moves->scoreMove(i, 0);
            if (isCapture(moves->getMove(i))) {
                counter = i+1;
                return moves->getMove(i);
            }
        }
        return 0;
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
