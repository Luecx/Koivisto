
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
        return captureHistory[getMovingPiece(m)][getSqToSqFromCombination(m)];
    } else {
        auto fmh_value = (followup != 0 ? fmh[getPieceTypeSqToCombination(followup)][side]
                                             [getPieceTypeSqToCombination(m)] : 0);
        auto cmh_value = cmh[getPieceTypeSqToCombination(previous)][side][getPieceTypeSqToCombination(m)];
        auto th_vaue   = th[side][threatSquare][getSqToSqFromCombination(m)];
        return (2 * fmh_value + 2 * cmh_value + 2 * th_vaue) / 3;
    }
}

/*
 * Set killer
 */
void SearchData::setKiller(Move move, Depth ply, Color color) {
    if (!sameMove(move, killer[color][ply][0])) {
        killer[color][ply][1] = killer[color][ply][0];
        killer[color][ply][0] = move;
    }
}

/*
 * Is killer?
 */
int SearchData::isKiller(Move move, Depth ply, Color color) const {
    if (sameMove(move, killer[color][ply][0]))
        return 2;
    return sameMove(move, killer[color][ply][1]);
}

/*
 * Set historic eval
 */
void SearchData::setHistoricEval(Score ev, Color color, Depth ply) {
    eval[color][ply] = ev;
}

/*
 * Is improving
 */
bool SearchData::isImproving(Score ev, Color color, Depth ply) const {
    if (ply >= 2) {
        return (ev > eval[color][ply - 2]);
    } else {
        return true;
    }
}
