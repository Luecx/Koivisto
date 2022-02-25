
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
#include "History.h"

int SearchData::getHistories(Move m, Move previous, Move followup, Square threatSquare) {
    if (isCapture(m)) {
        return captureHistory[getSqToSqFromCombination(m)];
    } else {
        return (2 * (followup != 0 ? fmh[getPieceTypeSqToCombination(followup)][getPieceTypeSqToCombination(m)] : 0)
               + 2 * cmh[getPieceTypeSqToCombination(previous)][getPieceTypeSqToCombination(m)]
               + 2 * th[threatSquare][getSqToSqFromCombination(m)]) / 3;
    }
}

/*
 * Set killer
 */
void SearchData::setKiller(Move move, Depth ply) {
    if (!sameMove(move, killer[ply][0])) {
        killer[ply][1] = killer[ply][0];
        killer[ply][0] = move;
    }
}

/*
 * Is killer?
 */
int SearchData::isKiller(Move move, Depth ply) {
    if (sameMove(move, killer[ply][0]))
        return 2;
    return sameMove(move, killer[ply][1]);
}

/*
 * Set historic eval
 */
void SearchData::setHistoricEval(Score ev, Depth ply) {
    eval[ply] = ev;
}

/*
 * Is improving
 */
bool SearchData::isImproving(Score ev, Depth ply) {
    if (ply >= 2) {
        return (ev > eval[ply - 2]);
    } else {
        return true;
    }
}