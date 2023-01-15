
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
#include "history.h"

using namespace bb;
using namespace move;

/**
 * getHistories function retrieves the history scores of a given move, side, previous move, followup
 * move, and threat square. The function checks if the move is a capture move, and returns the
 * corresponding capture history score. If not, it computes the FMH, CMH and THREAT_HISTORY scores and
 * returns their average.
 *
 * @param m The move to retrieve history scores for
 * @param side The side the move is being made by
 * @param previous The previous move that was made in the search tree
 * @param followup The next move after the given move in the search tree
 * @param threatSquare The square that is being threatened by the given move
 * @return The history score for the given move
 */
int SearchData::getHistories(Move m, Color side, Move previous, Move followup,
                             Square threatSquare) const {
    if (isCapture(m)) {
        return CAPTURE_HISTORY(this, side, m);
    } else {
        auto fmh_value = (followup != 0 ? FMH(this, followup, side, m) : 0);
        auto cmh_value = CMH(this, previous, side, m);
        auto th_value  = THREAT_HISTORY(this, side, threatSquare, m);
        return (2 * fmh_value + 2 * cmh_value + 2 * th_value) / 3;
    }
}

/**
 * setKiller sets a given move as the killer move for a given ply and color. It checks if the move is
 * not the same as the current killer move, and updates the killer move accordingly.
 *
 * @param move The move to set as the killer move
 * @param ply The ply in the search tree
 * @param color The color making the move
 * @return None
 */
void SearchData::setKiller(Move move, Depth ply, Color color) {
    if (!sameMove(move, KILLER1(this, color, ply))) {
        KILLER2(this, color, ply) = KILLER1(this, color, ply);
        KILLER1(this, color, ply) = move;
    }
}

/**
 * isKiller checks if a given move is a killer move for a given ply and color.
 * It compares the given move to the first and second killer moves and returns 2 if it matches the
 * first, 1 if it matches the second, and 0 otherwise.
 *
 * @param move The move to check if it is a killer move
 * @param ply The ply in the search tree
 * @param color The color making the move
 * @return 2 if the move is the first killer move, 1 if the move is the second killer move, 0
 * otherwise
 */
int SearchData::isKiller(Move move, Depth ply, Color color) const {
    if (sameMove(move, KILLER1(this, color, ply)))
        return 2;
    return sameMove(move, KILLER2(this, color, ply));
}

/**
 * setHistoricEval sets the evaluation score for a given color and ply in the search tree.
 *
 * @param ev The evaluation score to set
 * @param color The color of the position being evaluated
 * @param ply The ply in the search tree
 * @return None
 */
void SearchData::setHistoricEval(Score ev, Color color, Depth ply) {
    EVAL_HISTORY(this, color, ply) = ev;
}

/**
 * isImproving checks if the evaluation score for a given color and ply in the search tree is
 * improving. It compares the given evaluation score to the previous evaluation score from 2 plies ago
 * and returns true if it is greater, false otherwise.
 *
 * @param ev The evaluation score to check for improvement
 * @param color The color of the position being evaluated
 * @param ply The ply in the search tree
 * @return true if the given evaluation score is greater than the previous evaluation score from 2
 * plies ago, false otherwise
 */
bool SearchData::isImproving(Score ev, Color color, Depth ply) const {
    if (ply >= 2) {
        return (ev > EVAL_HISTORY(this, color, ply - 2));
    }
    return true;
}

/**
 * clear function sets all elements in the `th`, `captureHistory`, `cmh`, `fmh`, `killer` and
 * `maxImprovement` arrays to 0. This function is used to reset the search data for a new search
 * iteration.
 *
 * @return None
 */
void SearchData::clear() {
    std::memset(this->th, 0, sizeof(this->th));
    std::memset(this->captureHistory, 0, sizeof(this->captureHistory));
    std::memset(this->cmh, 0, sizeof(this->cmh));
    std::memset(this->fmh, 0, sizeof(this->fmh));
    std::memset(this->killer, 0, sizeof(this->killer));
    std::memset(this->maxImprovement, 0, sizeof(this->maxImprovement));
}
