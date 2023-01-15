
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

int SearchData::getHistories(Move m, Color side, Move previous, Move followup, Square threatSquare) const {
    if (isCapture(m)) {
        return CAPTURE_HISTORY(this, getMovingPiece(m),m);
    } else {
        auto fmh_value = (followup != 0 ? FMH(this, followup, side, m) : 0);
        auto cmh_value = CMH(this, previous, side, m);
        auto th_value  = THREAT_HISTORY(this, side, threatSquare, m);
        return (2 * fmh_value + 2 * cmh_value + 2 * th_value) / 3;
    }
}

/*
 * Set killer
 */
void SearchData::setKiller(Move move, Depth ply, Color color) {
    if (!sameMove(move, KILLER1(this, color, ply))) {
        KILLER2(this, color, ply) = KILLER1(this, color, ply);
        KILLER1(this, color, ply) = move;
    }
}


/*
 * Is killer?
 */
int SearchData::isKiller(Move move, Depth ply, Color color) const {
    if (sameMove(move, KILLER1(this, color, ply)))
        return 2;
    return sameMove(move, KILLER2(this, color, ply));
}
/*
 * Set historic eval
 */
void SearchData::setHistoricEval(Score ev, Color color, Depth ply) {
    EVAL_HISTORY(this, color, ply) = ev;
}

/*
 * Is improving
 */
bool SearchData::isImproving(Score ev, Color color, Depth ply) const {
    if (ply >= 2) {
        return (ev > EVAL_HISTORY(this, color, ply-2));
    }
    return true;
}

void SearchData::clear() {
//    std::memset(this->th, 0, 2*64*4096*4);
//    std::memset(this->captureHistory, 0, 2*4096*4);
//    std::memset(this->cmh, 0, 384*2*384*4);
//    std::memset(this->fmh, 0, 384*2*384*4);
//    std::memset(this->killer, 0, 2*257*2*4);
//    std::memset(this->maxImprovement, 0, 64*64*4);
    std::memset(this->th, 0, sizeof(this->th));
    std::memset(this->captureHistory, 0, sizeof(this->captureHistory));
    std::memset(this->cmh, 0, sizeof(this->cmh));
    std::memset(this->fmh, 0, sizeof(this->fmh));
    std::memset(this->killer, 0, sizeof(this->killer));
    std::memset(this->maxImprovement, 0, sizeof(this->maxImprovement));
}
